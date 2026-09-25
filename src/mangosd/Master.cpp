/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** \file
    \ingroup mangosd
*/

#ifndef WIN32
    #include "PosixDaemon.h"
#endif

#include "WorldSocketMgr.h"

#include "httplib.h"

#include "Common.h"
#include "Master.h"
#include "ExecutionWatch.h"
#include "WorldSocket.h"
#include "WorldRunnable.h"
#ifdef ENABLE_SOAP
#include "MaNGOSsoap.h"
#endif
#include <memory>
#include "World.h"
#include "Log.h"
#include "ScriptObjects.h"
#include "Timer.h"
#include "Policies/SingletonImp.h"
#include "SystemConfig.h"
#include "revision.h"
#include "Config/Config.h"
#include "Database/DatabaseEnv.h"
#include "CliRunnable.h"
#include "Util.h"
#include "MassMailMgr.h"
#include "DBCStores.h"
#include "re2/re2.h"


#include <fstream>
#include <iostream>
#include <ace/OS_NS_signal.h>
#include <ace/TP_Reactor.h>
#include <ace/Dev_Poll_Reactor.h>
#include <signal.h>

// Windows crash-dump capture. Catches each major failure path:
//   - unhandled SEH exceptions (SetUnhandledExceptionFilter)
//   - vectored handler for paths that bypass SEH dispatch (fast-fail,
//     heap-corruption RaiseFailFastException, invalid CRT parameter,
//     pure-virtual call)
//   - C++ runtime aborts (std::terminate, signal handlers)
// Writes a .dmp + a .txt sibling next to the running mangosd.exe so an
// unattended crash leaves enough state for post-mortem analysis.
#ifdef WIN32
#include <windows.h>
#include <dbghelp.h>
#include <new>          // _set_new_handler
#include <cstdlib>      // _set_invalid_parameter_handler
#include <stdlib.h>
#include <crtdbg.h>
#pragma comment(lib, "dbghelp.lib")

#ifdef BUILD_PLAYERBOTS
// Forward-decl the SC_PHASE TLS into mangosd so the crash handler can read
// it without #including any playerbots header (which would pull the whole
// vendor tree's include chain into mangosd). Definitions live in
// BotDiagnostics.cpp; writes happen in SC_PHASE iff AiPlayerbot.EnableActionLog=1.
// The matching read site (Mangosd_WriteCrashDump) guards the read with
// __try/__except so the TLS being corrupted by the crash we're trying to
// dump can't crash the dump path itself.
namespace ai { namespace botdiag {
    extern thread_local const char* gLastPhaseTag;
    extern thread_local const char* gLastPhaseBotName;
}}
#endif

// Re-entrancy guard: if our handler itself crashes, we must NOT recurse —
// just let the process die. Per-thread so concurrent crashes are handled.
static thread_local int g_inCrashHandler = 0;

// Shared minidump-write helper. Called from every crash entry-point we
// install (vectored, SEH-unhandled, terminate, signal, invalid-param,
// purecall). Always returns; caller decides whether to continue or die.
//
// `ep` may be nullptr — for non-SEH paths (signal/terminate/etc.) we
// synthesize a context by calling RtlCaptureContext below.
//
// `synthCode` is the exception code we tag the dump with when we
// synthesize a context; ignored when `ep` is set.
//
// `tag` is a short label for the crash kind ("VEH-heap-corruption",
// "terminate", "signal-SIGABRT", etc.) — written into the .txt.
static void Mangosd_WriteCrashDump(EXCEPTION_POINTERS* ep, DWORD synthCode, const char* tag)
{
    // Re-entrancy / recursion guard.
    if (g_inCrashHandler)
        return;
    g_inCrashHandler = 1;

    // Synthesize a context if we don't have one (terminate/signal paths).
    EXCEPTION_RECORD synthRec = {0};
    CONTEXT          synthCtx = {0};
    EXCEPTION_POINTERS synthEp = {0};
    if (!ep)
    {
        synthRec.ExceptionCode = synthCode;
        synthRec.ExceptionAddress = (PVOID)Mangosd_WriteCrashDump;
        RtlCaptureContext(&synthCtx);
        synthEp.ExceptionRecord = &synthRec;
        synthEp.ContextRecord = &synthCtx;
        ep = &synthEp;
    }

    SYSTEMTIME st; GetLocalTime(&st);
    char tsBuf[64];
    snprintf(tsBuf, sizeof(tsBuf), "%04d%02d%02d_%02d%02d%02d",
             st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

    // mangosd's CWD is `bin/` per the launcher .bat (`pushd "...\bin"`),
    // but logs/ lives at the parent (`..\logs\`). Try parent-logs first,
    // then sibling bin-local logs/, then CWD root as last resort.
    const char* candidatePaths[] = { "..\\logs", "logs", ".", nullptr };

    char filename[512] = {0};
    HANDLE hFile = INVALID_HANDLE_VALUE;
    for (int i = 0; candidatePaths[i] != nullptr; ++i)
    {
        snprintf(filename, sizeof(filename), "%s\\crash_%s.dmp",
                 candidatePaths[i], tsBuf);
        hFile = CreateFileA(filename, GENERIC_WRITE, 0, nullptr,
                             CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
            break;
    }
    if (hFile == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "[CRASH] mangosd unhandled %s, but failed to open %s for write\n",
                tag ? tag : "(unknown)", filename);
        g_inCrashHandler = 0;
        return;
    }

    MINIDUMP_EXCEPTION_INFORMATION mei = { 0 };
    mei.ThreadId          = GetCurrentThreadId();
    mei.ExceptionPointers = ep;
    mei.ClientPointers    = FALSE;

    // For heap corruption, we want enough memory in the dump to
    // reconstruct the corrupted allocation. WithDataSegs+ThreadInfo+
    // IndirectlyReferenced gives us local variables + the chain of
    // referenced pointers around the failing thread, which is what
    // matters for use-after-free / overrun analysis. Adding
    // WithProcessThreadData ensures the heap metadata travels too.
    MINIDUMP_TYPE dumpType = (MINIDUMP_TYPE)(
        MiniDumpWithDataSegs |
        MiniDumpWithThreadInfo |
        MiniDumpWithIndirectlyReferencedMemory |
        MiniDumpWithProcessThreadData);

    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),
                       hFile, dumpType, &mei, nullptr, nullptr);
    CloseHandle(hFile);

    // Read SC_PHASE thread-locals (guarded — TLS itself can be corrupted
    // when the corruption was in arbitrary memory). The TLS is only set
    // when AiPlayerbot.EnableActionLog=1; otherwise these stay nullptr
    // and we report "(no phase set)".
    const char* phaseTag = "(no phase set)";
    const char* phaseBot = "(no bot)";
#ifdef BUILD_PLAYERBOTS
    __try
    {
        if (ai::botdiag::gLastPhaseTag)
            phaseTag = ai::botdiag::gLastPhaseTag;
        if (ai::botdiag::gLastPhaseBotName)
            phaseBot = ai::botdiag::gLastPhaseBotName;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}
#endif

    char txtFilename[512];
    snprintf(txtFilename, sizeof(txtFilename), "%s.txt", filename);
    if (FILE* tf = fopen(txtFilename, "w"))
    {
        fprintf(tf, "Crash timestamp: %s\n", tsBuf);
        fprintf(tf, "Crash kind: %s\n", tag ? tag : "(unknown)");
        fprintf(tf, "Exception code: 0x%08lx", ep->ExceptionRecord->ExceptionCode);
        // Decode common fast-fail / heap codes for grep-friendly logs.
        switch (ep->ExceptionRecord->ExceptionCode)
        {
            case 0xC0000005L: fprintf(tf, " (ACCESS_VIOLATION)\n"); break;
            case 0xC0000094L: fprintf(tf, " (INTEGER_DIVIDE_BY_ZERO)\n"); break;
            case 0xC0000374L: fprintf(tf, " (HEAP_CORRUPTION)\n"); break;
            case 0xC0000409L: fprintf(tf, " (STACK_BUFFER_OVERRUN / FAIL_FAST)\n"); break;
            case 0xC0000420L: fprintf(tf, " (ASSERTION_FAILURE)\n"); break;
            case 0xC0000602L: fprintf(tf, " (UNHANDLED_CXX_EXCEPTION)\n"); break;
            case 0xE06D7363L: fprintf(tf, " (CXX_THROW)\n"); break;
            default:          fprintf(tf, "\n"); break;
        }
        fprintf(tf, "Exception address: 0x%p\n", ep->ExceptionRecord->ExceptionAddress);
        if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION
            && ep->ExceptionRecord->NumberParameters >= 2)
        {
            fprintf(tf, "Access violation: %s at 0x%p\n",
                    ep->ExceptionRecord->ExceptionInformation[0] == 0 ? "READ" :
                    ep->ExceptionRecord->ExceptionInformation[0] == 1 ? "WRITE" :
                    ep->ExceptionRecord->ExceptionInformation[0] == 8 ? "EXECUTE" : "?",
                    (void*)ep->ExceptionRecord->ExceptionInformation[1]);
        }
        fprintf(tf, "Crashing thread id: %lu\n", GetCurrentThreadId());
        fprintf(tf, "Last SC_PHASE tag: %s\n", phaseTag);
        fprintf(tf, "Last SC_PHASE bot: %s\n", phaseBot);
        fclose(tf);
    }

    fprintf(stderr, "[CRASH] mangosd wrote minidump: %s (kind=%s)\n", filename, tag ? tag : "?");
    fprintf(stderr, "[CRASH] last phase: %s on bot %s\n", phaseTag, phaseBot);
    sLog.outError("[CRASH] %s (code 0x%08lx). Minidump: %s",
                   tag ? tag : "Unhandled exception",
                   ep->ExceptionRecord->ExceptionCode, filename);
    sLog.outError("[CRASH] Last SC_PHASE: %s (bot=%s)", phaseTag, phaseBot);

    g_inCrashHandler = 0;
}

// Decide whether VEH should claim this exception. VEH fires for EVERY
// exception including SEH-handled ones (e.g. C++ throws inside try/catch),
// so claim only fatal codes that we KNOW bypass SEH or the cmangos
// codebase can't handle. Returning true → write the dump and let the OS
// terminate (we don't tail-call ExitProcess; we let the failing primitive
// continue its own death sequence).
static bool MangosdShouldClaimVehException(DWORD code)
{
    switch (code)
    {
        case 0xC0000374L: // STATUS_HEAP_CORRUPTION (RtlReportFatalFailure path)
        case 0xC0000409L: // STATUS_STACK_BUFFER_OVERRUN / __fastfail
        case 0xC0000420L: // STATUS_ASSERTION_FAILURE
        case 0x80000003L: // STATUS_BREAKPOINT (only if not under debugger; treat as crash)
            return true;
        // 0xC0000005 (AV) and 0xE06D7363 (C++ throw) are too noisy in VEH —
        // they happen during normal operation inside try/catch frames.
        // Let the existing UnhandledExceptionFilter catch genuinely-unhandled
        // ones via the SEH chain.
        default:
            return false;
    }
}

static LONG CALLBACK MangosdVectoredExceptionHandler(EXCEPTION_POINTERS* ep)
{
    if (!ep || !ep->ExceptionRecord)
        return EXCEPTION_CONTINUE_SEARCH;

    DWORD code = ep->ExceptionRecord->ExceptionCode;
    if (!MangosdShouldClaimVehException(code))
        return EXCEPTION_CONTINUE_SEARCH;

    // Pick a label by code so the .txt clearly identifies the crash kind.
    const char* tag = "VEH";
    switch (code)
    {
        case 0xC0000374L: tag = "VEH-heap-corruption"; break;
        case 0xC0000409L: tag = "VEH-stack-buffer-overrun-or-fastfail"; break;
        case 0xC0000420L: tag = "VEH-assertion-failure"; break;
        case 0x80000003L: tag = "VEH-breakpoint-no-debugger"; break;
    }

    Mangosd_WriteCrashDump(ep, code, tag);
    return EXCEPTION_CONTINUE_SEARCH; // let the original kill chain proceed
}

// Original SEH unhandled-filter path. Kept for non-fast-fail unhandled
// exceptions (mainly access violations that escape every __try/catch).
static LONG WINAPI MangosdUnhandledExceptionFilter(EXCEPTION_POINTERS* ep)
{
    Mangosd_WriteCrashDump(ep,
                            ep && ep->ExceptionRecord ? ep->ExceptionRecord->ExceptionCode : 0,
                            "SEH-unhandled");
    return EXCEPTION_EXECUTE_HANDLER;
}

// Synthetic exception codes used by the non-SEH crash paths below. Reusing
// real NT status values means the .dmp opens in WinDbg / Visual Studio
// without a "missing status code" warning, and the analyst sees a familiar
// tag instead of zero. (0xC0000602 = STATUS_FAIL_FAST_EXCEPTION,
// 0xC0000420 = STATUS_ASSERTION_FAILURE.)

// std::terminate() — uncaught C++ exception, set_terminate target,
// or std::terminate() called explicitly. Bypasses SEH entirely.
static void MangosdTerminateHandler()
{
    Mangosd_WriteCrashDump(nullptr, 0xC0000602L /* synthetic UNHANDLED_CXX */,
                            "terminate");
    // Fall through to default terminate (process dies cleanly here).
    abort();
}

// SIGABRT: triggered by abort(), assert() failure, uncaught CRT errors.
static void MangosdSignalHandler(int sig)
{
    const char* tag = "signal-unknown";
    switch (sig)
    {
        case SIGABRT: tag = "signal-SIGABRT"; break;
        case SIGSEGV: tag = "signal-SIGSEGV"; break;
        case SIGFPE:  tag = "signal-SIGFPE"; break;
        case SIGILL:  tag = "signal-SIGILL"; break;
    }
    Mangosd_WriteCrashDump(nullptr, 0xC0000420L /* synthetic ASSERTION_FAILURE */,
                            tag);
    // Don't reinstall — the next abort() would loop. Default handler runs.
    signal(sig, SIG_DFL);
    raise(sig);
}

// MSVC CRT invalid-parameter handler — fires when CRT API gets bad args
// (e.g. passing nullptr to printf %s, scanf format mismatch). Default
// behavior is to fast-fail; override so we capture the dump first.
static void MangosdInvalidParameterHandler(const wchar_t* /*expr*/,
                                             const wchar_t* /*func*/,
                                             const wchar_t* /*file*/,
                                             unsigned int /*line*/,
                                             uintptr_t /*reserved*/)
{
    Mangosd_WriteCrashDump(nullptr, 0xC0000420L,
                            "invalid-parameter");
    // Don't return — CRT expects this handler to terminate. abort() will
    // re-enter our SIGABRT handler, but g_inCrashHandler stops recursion.
    abort();
}

// Pure virtual call handler — called when a pure virtual function is
// invoked (typically from a destructor on a partially-destroyed object).
// MSVC's _purecall hook.
static void MangosdPureCallHandler()
{
    Mangosd_WriteCrashDump(nullptr, 0xC0000420L, "pure-virtual-call");
    abort();
}

// Install all crash-capture hooks. Called once at the start of Master::Run.
static void MangosdInstallCrashHandlers()
{
    // VEH first — it fires before SEH frame walk, so we capture the dump
    // even when ntdll is about to call __fastfail and bypass SEH entirely.
    // Argument 1 = "first" (run before any other vectored handler).
    AddVectoredExceptionHandler(1, MangosdVectoredExceptionHandler);

    // Backstop SEH filter — catches anything that DOES propagate up the
    // SEH chain unhandled (rare for fast-fails, common for normal AVs).
    SetUnhandledExceptionFilter(MangosdUnhandledExceptionFilter);

    // C++ runtime hooks for paths that bypass SEH altogether.
    std::set_terminate(MangosdTerminateHandler);
    _set_invalid_parameter_handler(MangosdInvalidParameterHandler);
    _set_purecall_handler(MangosdPureCallHandler);

    // Disable the WER "process has stopped working" dialog — we want
    // the process to die immediately so the launcher .bat can restart
    // it without operator interaction. The dump is already on disk.
    _CrtSetReportMode(_CRT_WARN, 0);
    _CrtSetReportMode(_CRT_ERROR, 0);
    _CrtSetReportMode(_CRT_ASSERT, 0);

    // Signal-based hooks (mostly redundant with the above on Windows,
    // but cover edge cases like stdlib code calling abort() directly).
    signal(SIGABRT, MangosdSignalHandler);
    signal(SIGSEGV, MangosdSignalHandler);
    signal(SIGFPE,  MangosdSignalHandler);
    signal(SIGILL,  MangosdSignalHandler);
}
#endif

#include "ace/MMAP_Memory_Pool.h"
#include "ace/Shared_Memory_MM.h"
#include "ace/ACE.h"
#include "ace/Malloc_T.h"

Master sMaster;

volatile uint32 Master::m_masterLoopCounter = 0;
volatile bool Master::m_handleSigvSignals = false;

template <typename T>
class SharedMemoryAllocator : public std::allocator<T>
{
public:
    typedef size_t size_type;
    typedef T* pointer;
    typedef const T* const_pointer;

    SharedMemoryAllocator(ACE_Malloc_T<ACE_MMAP_MEMORY_POOL, ACE_Process_Mutex, ACE_Control_Block>& memory_pool)
        : memory_allocator_(memory_pool) {}

    pointer allocate(size_type n, const void* hint = 0) {
        return static_cast<pointer>(memory_allocator_.malloc(n * sizeof(T)));
    }

    void deallocate(pointer p, size_type n) {
        memory_allocator_.free(p);
    }

private:
    ACE_Malloc_T<ACE_MMAP_MEMORY_POOL, ACE_Process_Mutex, ACE_Control_Block>& memory_allocator_;
};



void freezeDetector(uint32 _delaytime)
{
    if (!_delaytime)
        return;

    sLog.outString("Starting up anti-freeze thread (%u seconds max stuck time)...",_delaytime/1000);
    uint32 loops = 0;
    uint32 lastchange = 0;

    while (!World::IsStopped())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        uint32 curtime = WorldTimer::getMSTime();

        // normal work
        if (loops != World::m_worldLoopCounter)
        {
            lastchange = curtime;
            loops = World::m_worldLoopCounter;
        }
        // possible freeze
#ifdef NDEBUG
        else if (WorldTimer::getMSTimeDiff(lastchange, curtime) > _delaytime)
        {
            sLog.outError("World Thread hangs, kicking out server!");
            std::terminate(); // bang crash
        }
#endif
    }
};

static void WatchWorldProgress(uint32 thresholdMs, std::string logPath)
{
    uint32 loop = World::m_worldLoopCounter.load();
    uint64 lastProgress = ExecutionWatch::Now();
    bool reported = false;
    while (!World::IsStopped())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        uint64 const now = ExecutionWatch::Now();
        uint32 const current = World::m_worldLoopCounter.load();
        if (current != loop)
        {
            loop = current;
            lastProgress = now;
            reported = false;
        }
        else if (!reported && now - lastProgress >= thresholdMs)
        {
            if (FILE* file = fopen(logPath.c_str(), "a"))
            {
                fprintf(file, "\nSTALL epoch=%llu (diagnostic only; no restart or termination)\n",
                    static_cast<unsigned long long>(time(nullptr)));
                ExecutionWatch::Dump(file, loop, now - lastProgress);
                fclose(file);
            }
            reported = true;
        }
    }
}

Master::Master()
{
    
}

Master::~Master()
{
}


/// Main function
int Master::Run()
{
#ifdef WIN32
    // Install crash-dump capture ASAP so any startup-time crash
    // (e.g. DBC load failure) also produces a usable dump. VEH + CRT
    // hooks catch heap corruption / fast-fail / std::terminate paths
    // that bypass SetUnhandledExceptionFilter. See
    // MangosdInstallCrashHandlers above for the full hook inventory.
    MangosdInstallCrashHandlers();
    if (sConfig.GetBoolDefault("Console.DisableQuickEdit", true))
    {
        HANDLE const input = GetStdHandle(STD_INPUT_HANDLE);
        DWORD mode = 0;
        if (GetConsoleMode(input, &mode))
            SetConsoleMode(input, (mode | ENABLE_EXTENDED_FLAGS) & ~ENABLE_QUICK_EDIT_MODE);
    }
#endif

    /// worldd PID file creation
    std::string pidfile = sConfig.GetStringDefault("PidFile", "");
    if (!pidfile.empty())
    {
        uint32 pid = CreatePIDFile(pidfile);
        if (!pid)
        {
            sLog.outError( "Cannot create PID file %s.\n", pidfile.c_str() );
            Log::WaitBeforeContinueIfNeed();
            return 1;
        }

        sLog.outString( "Daemon PID: %u\n", pid );
    }

    ///- Start the databases
    if (!_StartDB())
    {
        Log::WaitBeforeContinueIfNeed();
        return 1;
    }

#if 0
    ACE_MMAP_Memory_Pool_Options opt;
    opt.unique_ = true;
   // opt.file_mode_ = S_IRUSR | S_IWUSR;

    ACE_Malloc_T<ACE_MMAP_MEMORY_POOL, ACE_Process_Mutex, ACE_Control_Block> memory_allocator_{ "twow_pool", "twow_pool", &opt};

    using pair_t = std::pair<const uint32, std::array<uint8, 20>>;
    using shm_umap = std::unordered_map<uint32, std::array<uint8, 20>, std::hash<uint32>, std::equal_to<uint32>,
        SharedMemoryAllocator<pair_t>>;

    SharedMemoryAllocator<pair_t> shm_alloc{ memory_allocator_ };

    shm_umap* mp = new (memory_allocator_.malloc(sizeof(shm_umap))) shm_umap(shm_alloc);
    mp->insert({ 5, std::array<uint8, 20>{} });

    if (mp)
    {
        int err = memory_allocator_.bind("sessionkey_store", mp);
        if (!err)
        {
            memory_allocator_.sync();
            Log::WaitBeforeContinueIfNeed();
        }
    }
#endif

    ///- Initialize the World
    sWorld.SetInitialWorldSettings();
    

    #ifndef WIN32
    detachDaemon();
    #endif
    //server loaded successfully => enable async DB requests
    //this is done to forbid any async transactions during server startup!
    CharacterDatabase.AllowAsyncTransactions();
    WorldDatabase.AllowAsyncTransactions();
    LoginDatabase.AllowAsyncTransactions();
    LogsDatabase.AllowAsyncTransactions();

    ///- Catch termination signals
    _HookSignals();

   // sWorld.RestoreLostGOs();

    ///- Launch WorldRunnable thread
    std::thread world_thread{WorldRunnable()};

#ifdef ENABLE_SOAP
    ///- Start the SOAP remote command interface (off unless SOAP.Enabled = 1)
    std::unique_ptr<SOAPThread> soapThread;
    if (sConfig.GetBoolDefault("SOAP.Enabled", false))
        soapThread.reset(new SOAPThread(sConfig.GetStringDefault("SOAP.IP", "127.0.0.1"),
                                        sConfig.GetIntDefault("SOAP.Port", 7878)));
#endif
    std::thread progress_thread;
    if (uint32 seconds = sConfig.GetIntDefault("Diagnostics.StallSeconds", 10))
    {
        std::string directory = sConfig.GetStringDefault("LogsDir", ".");
        if (directory.empty())
            directory = ".";
        progress_thread = std::thread(WatchWorldProgress, std::min<uint32>(seconds, 3600) * 1000,
            directory + "/StallBreadcrumb.log");
    }

    // set realmbuilds depend on mangosd expected builds, and set server online
    {
        std::string builds = AcceptableClientBuildsListStr();
        LoginDatabase.escape_string(builds);

        LoginDatabase.PExecute("UPDATE realmlist SET realmflags = realmflags & ~(%u), population = 0, realmbuilds = '%s'  WHERE id = '%u'", REALM_FLAG_OFFLINE, builds.c_str(), realmID);
    }

    std::thread* cliThread = nullptr;

    if (sConfig.GetBoolDefault("Console.Enable", true))
    {
        ///- Launch CliRunnable thread
        cliThread = new std::thread(CliRunnable());
    }

    ///- Handle affinity for multiple processors and process priority on Windows
    #ifdef WIN32
    {
        HANDLE hProcess = GetCurrentProcess();

        uint32 Aff = sConfig.GetIntDefault("UseProcessors", 0);
        if (Aff > 0)
        {
            ULONG_PTR appAff;
            ULONG_PTR sysAff;

            if (GetProcessAffinityMask(hProcess,&appAff,&sysAff))
            {
                ULONG_PTR curAff = Aff & appAff;            // remove non accessible processors

                if (!curAff)
                {
                    sLog.outError("Processors marked in UseProcessors bitmask (hex) %x not accessible for mangosd. Accessible processors bitmask (hex): %x",Aff,appAff);
                }
                else
                {
                    if (SetProcessAffinityMask(hProcess,curAff))
                        sLog.outString("Using processors (bitmask, hex): %x", curAff);
                    else
                        sLog.outError("Can't set used processors (hex): %x",curAff);
                }
            }
            
        }

        bool Prio = sConfig.GetBoolDefault("ProcessPriority", false);
        if (!Prio)
            sLog.outError("Can't set mangosd process priority class.");
    }
    #endif

    ///- Start up freeze catcher thread
    std::thread* freeze_thread = nullptr;
    if (uint32 freeze_delay = sConfig.GetIntDefault("MaxCoreStuckTime", 0))
        freeze_thread = new std::thread(std::bind(&freezeDetector,freeze_delay*1000));

    ///- Launch the world listener socket
    uint16 wsport = sWorld.getConfig (CONFIG_UINT32_PORT_WORLD);
    std::string bind_ip = sConfig.GetStringDefault ("BindIP", "0.0.0.0");

    // Start WorldSockets
    sWorldSocketMgr->SetOutKBuff(sConfig.GetIntDefault("Network.OutKBuff", -1));
    sWorldSocketMgr->SetOutUBuff(sConfig.GetIntDefault("Network.OutUBuff", 65536));
    sWorldSocketMgr->SetThreads(sConfig.GetIntDefault("Network.Threads", 1) + 1);
    sWorldSocketMgr->SetInterval(sConfig.GetIntDefault("Network.Interval", 10));
    sWorldSocketMgr->SetTcpNodelay(sConfig.GetBoolDefault("Network.TcpNodelay", true));

    if (sWorldSocketMgr->StartNetwork(wsport, bind_ip) == -1)
    {
        sLog.outError ("Failed to start WorldSocket network");
        Log::WaitBeforeContinueIfNeed();
        World::StopNow(ERROR_EXIT_CODE);
        // go down and shutdown the server
    }
    else
    {
        ScriptRegistry<ServerScript>::ForEachEnabledHook(SERVERHOOK_ON_NETWORK_START, [](ServerScript* script)
        {
            script->OnNetworkStart();
        });
    }

    // Do NOT also call sWorldSocketMgr->Wait() here. WorldRunnable's shutdown
    // path (run on world_thread) ends by calling sWorldSocketMgr->StopNetwork(),
    // which itself stops and joins every network thread before returning.
    // Waiting on the network threads a second time from this thread raced
    // StopNetwork()'s own join (both called ACE_Task_Base::wait() on the same
    // ReactorRunnable threads concurrently, which is undefined behavior) and
    // was the root cause of the intermittent "Stopping network threads..."
    // shutdown hang. world_thread.join() below is sufficient on its own: it
    // cannot return until StopNetwork() -- and thus the network-thread join --
    // has completed.
    //
    // when the main thread closes the singletons get unloaded
    // since worldrunnable uses them, it will crash if unloaded after master
    world_thread.join();
    // Master exits with quick_exit, so local destructors are not guaranteed.
#ifdef ENABLE_SOAP
    // Join SOAP while authentication databases and queued-command owners exist.
    soapThread.reset();
#endif
    if (progress_thread.joinable())
        progress_thread.join();


    ///- Stop freeze protection before shutdown tasks
    if (freeze_thread)
    {
        freeze_thread->join();
        delete freeze_thread;
    }

    ///- Remove signal handling before leaving
    _UnhookSignals();

    ///- Clean account database before leaving
    sLog.outString("Cleaning character database...");
    clearOnlineAccounts();

    // send all still queued mass mails (before DB connections shutdown)
    sLog.outString("Sending queued mail...");
    sMassMailMgr.Update(true);

    ///- Wait for DB delay threads to end
    sLog.outString("Closing database connections...");
    CharacterDatabase.StopServer();
    WorldDatabase.StopServer();
    LoginDatabase.StopServer();
    LogsDatabase.StopServer();

    sLog.outString("Halting process...");

#if 0
    mp->~shm_umap();
    memory_allocator_.free(mp);

    // Cleanup
    memory_allocator_.sync();
    memory_allocator_.remove();

#endif

    if (cliThread)
    {
        #ifdef WIN32

        // this only way to terminate CLI thread exist at Win32 (alt. way exist only in Windows Vista API)
        //_exit(1);
        // send keyboard input to safely unblock the CLI thread
        INPUT_RECORD b[5];
        HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
        b[0].EventType = KEY_EVENT;
        b[0].Event.KeyEvent.bKeyDown = TRUE;
        b[0].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[0].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[0].Event.KeyEvent.wRepeatCount = 1;

        b[1].EventType = KEY_EVENT;
        b[1].Event.KeyEvent.bKeyDown = FALSE;
        b[1].Event.KeyEvent.uChar.AsciiChar = 'X';
        b[1].Event.KeyEvent.wVirtualKeyCode = 'X';
        b[1].Event.KeyEvent.wRepeatCount = 1;

        b[2].EventType = KEY_EVENT;
        b[2].Event.KeyEvent.bKeyDown = TRUE;
        b[2].Event.KeyEvent.dwControlKeyState = 0;
        b[2].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[2].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[2].Event.KeyEvent.wRepeatCount = 1;
        b[2].Event.KeyEvent.wVirtualScanCode = 0x1c;

        b[3].EventType = KEY_EVENT;
        b[3].Event.KeyEvent.bKeyDown = FALSE;
        b[3].Event.KeyEvent.dwControlKeyState = 0;
        b[3].Event.KeyEvent.uChar.AsciiChar = '\r';
        b[3].Event.KeyEvent.wVirtualKeyCode = VK_RETURN;
        b[3].Event.KeyEvent.wVirtualScanCode = 0x1c;
        b[3].Event.KeyEvent.wRepeatCount = 1;
        DWORD numb;
        WriteConsoleInput(hStdIn, b, 4, &numb);
#else
        fclose(stdin);
#endif
        if (cliThread->joinable())
            cliThread->join();

        delete cliThread;
    }

    ///- Exit the process with specified return value
	// WORLD SHUTDOWN

	sWorld.InternalShutdown();

    uint8 exitCode = World::GetExitCode();
    std::quick_exit(exitCode);
    return exitCode;
}

bool StartDB(std::string name, DatabaseType& database)
{
    ///- Get database info from configuration file
    std::string dbstring = sConfig.GetStringDefault((name + "Database.Info").c_str(), "");
    int nConnections = sConfig.GetIntDefault((name + "Database.Connections").c_str(), 1);
    int nAsyncConnections = sConfig.GetIntDefault((name + "Database.WorkerThreads").c_str(), 1);
    if (dbstring.empty())
    {
        sLog.outError("%s database not specified in configuration file", name.c_str());
        return false;
    }

    // Remove password from DB string for log output
    // format: 127.0.0.1;3306;mangos;mangos;characters
    // In a properly formatted string, token 4 is the password
    std::string dbStringLog = dbstring;

    if (std::count(dbStringLog.begin(), dbStringLog.end(), ';') == 4)
    {
        // Have correct number of tokens, can replace
        std::string::iterator start = dbStringLog.end(), end = dbStringLog.end();

        int occurrence = 0;
        for (std::string::iterator itr = dbStringLog.begin(); itr != dbStringLog.end(); ++itr)
        {
            if (*itr == ';')
                ++occurrence;

            if (occurrence == 3 && start == dbStringLog.end())
                start = ++itr;
            else if (occurrence == 4 && end == dbStringLog.end())
                end = itr;

            if (start != dbStringLog.end() && end != dbStringLog.end())
                break;
        }

        dbStringLog.replace(start, end, "*");
    }
    else
    {
        sLog.outError("Incorrectly formatted database connection string for database %s", name.c_str());
        return false;
    }

    ///- Initialise the world database
    if (!database.Initialize(name.c_str(), dbstring.c_str(), nConnections, nAsyncConnections))
    {
        sLog.outError("Cannot connect to world database %s", name.c_str());
        return false;
    }

    return true;
}
/// Initialize connection to the databases
bool Master::_StartDB()
{
    ///- Get the realm Id from the configuration file
    realmID = sConfig.GetIntDefault("RealmID", 0);
    if (!realmID)
    {
        sLog.outError("Realm ID not defined in configuration file");
        return false;
    }

    if (!StartDB("World", WorldDatabase) ||
        !StartDB("Character", CharacterDatabase) ||
        !StartDB("Login", LoginDatabase) ||
        !StartDB("Logs", LogsDatabase))
    {
        WorldDatabase.HaltDelayThread();
        CharacterDatabase.HaltDelayThread();
        LoginDatabase.HaltDelayThread();
        LogsDatabase.HaltDelayThread();
        return false;
    }

    sLog.outString("Welcome to Turtle WoW! Realm ID: %d", realmID);

    ///- Clean the database before starting
    clearOnlineAccounts();
    return true;
}

/// Clear 'online' status for all accounts with characters in this realm
void Master::clearOnlineAccounts()
{
    // Cleanup online status for characters hosted at current realm
    /// \todo Only accounts with characters logged on *this* realm should have online status reset. Move the online column from 'account' to 'realmcharacters'?
    LoginDatabase.PExecute("UPDATE account SET current_realm = 0 WHERE current_realm = '%u'", realmID);

    CharacterDatabase.Execute("UPDATE characters SET online = 0 WHERE online<>0");

    // Battleground instance ids reset at server restart
    CharacterDatabase.Execute("UPDATE character_battleground_data SET instance_id = 0");
}

#include "ObjectAccessor.h"
#include "Language.h"
void createdump(void)
{
#ifndef WIN32
    if (!fork())
        abort(); // Crash the app
#endif
    
}
/// Handle termination signals
void Master::SigvSignalHandler()
{
    if (m_handleSigvSignals)
        _OnSignal(SIGSEGV);

    exit(1);
}

void Master::_OnSignal(int s)
{
    switch (s)
    {
        case SIGINT:
        {
            World::StopNow(RESTART_EXIT_CODE);
            break;
        }
        case SIGTERM:
        #ifdef _WIN32
        case SIGBREAK:
        #endif
            World::StopNow(SHUTDOWN_EXIT_CODE);
            break;
        case SIGSEGV:
        {
            signal(SIGSEGV, 0);
            if (!m_handleSigvSignals)
                return;

            std::exception_ptr exc = std::current_exception();
            m_handleSigvSignals = false; // Desarm anticrash
            sWorld.SetAnticrashRearmTimer(sWorld.getConfig(CONFIG_UINT32_ANTICRASH_REARM_TIMER));
            uint32 anticrashOptions = sWorld.getConfig(CONFIG_UINT32_ANTICRASH_OPTIONS);
            // Log crash stack
            sLog.outInfo("Received SIGSEGV");
            ACE_Stack_Trace st;
            sLog.outInfo("%s", st.c_str());

            if (anticrashOptions & ANTICRASH_GENERATE_COREDUMP)
                createdump();

            if (anticrashOptions & ANTICRASH_OPTION_ANNOUNCE_PLAYERS)
            {
                if (anticrashOptions & ANTICRASH_OPTION_SAVEALL)
                    sWorld.SendWorldText(LANG_SYSTEMMESSAGE, "Server has crashed. Now saving online players ...");
                else
                    sWorld.SendWorldText(LANG_SYSTEMMESSAGE, "Crash server occurred :(");
    
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

            if (anticrashOptions & ANTICRASH_OPTION_SAVEALL)
            {
                CharacterDatabase.ThreadStart();
                sObjectAccessor.SaveAllPlayers();
                std::this_thread::sleep_for(std::chrono::seconds(25));
            }

            std::rethrow_exception(exc); // Crash for real now.
            return;
        }
    }

    signal(s, _OnSignal);
}

void Master::_HookSignals()
{
    signal(SIGINT, _OnSignal);
    signal(SIGTERM, _OnSignal);
    signal(SIGSEGV, _OnSignal);
    #ifdef _WIN32
    signal(SIGBREAK, _OnSignal);
    #endif
    ArmAnticrash();
}

void Master::ArmAnticrash()
{
    m_handleSigvSignals = true;
}

/// Unhook the signals before leaving
void Master::_UnhookSignals()
{
    signal(SIGINT, 0);
    signal(SIGTERM, 0);
    signal(SIGSEGV, 0);
    #ifdef _WIN32
    signal(SIGBREAK, 0);
    #endif
    m_handleSigvSignals = false;
}
