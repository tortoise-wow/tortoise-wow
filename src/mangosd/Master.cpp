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

Master::Master()
{
    
}

Master::~Master()
{
}


/// Main function
int Master::Run()
{
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

    world_thread.join();

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
