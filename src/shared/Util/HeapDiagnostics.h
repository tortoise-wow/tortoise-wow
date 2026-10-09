#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <cstdint>
#include <ctime>
#include <string>
#pragma once
// DEV observer only. HeapSummary includes CRT/library allocations using Windows
// heaps; it is not a call-stack profiler and does not cover direct VirtualAlloc.
#include <windows.h>
#include <malloc.h>
#include <array>
#include <sstream>
#include <chrono>
namespace ManTech::Diag {
inline std::string HeapSnapshot() {
    struct Summary { DWORD cb; SIZE_T allocated,committed,reserved,maxReserve; };
    using Fn=BOOL(WINAPI*)(HANDLE,DWORD,Summary*);
    static auto query=reinterpret_cast<Fn>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"HeapSummary"));
    auto started=std::chrono::steady_clock::now();
    if(!query)return "{\"available\":false,\"reason\":\"HeapSummary unavailable on this Windows build\"}";
    // Fixed storage avoids allocation while enumerating. Unsupported heaps and
    // a changing heap list are reported as incomplete rather than as zero usage.
    struct Row { HANDLE handle{};Summary s{};DWORD error=0;bool ok=false; };
    std::array<HANDLE,256> handles{};std::array<Row,256> rows{};
    DWORD found=GetProcessHeaps(DWORD(handles.size()),handles.data());
    if(!found)return "{\"available\":false,\"reason\":\"GetProcessHeaps failed\"}";
    // GetProcessHeaps does not promise a complete handle array when too small.
    if(found>handles.size())return "{\"available\":false,\"reason\":\"Heap handle capacity exceeded\"}";
    unsigned good=0;std::uint64_t allocated=0,committed=0;
    for(unsigned i=0;i<found;++i){auto& r=rows[i];r.handle=handles[i];r.s.cb=sizeof(Summary);r.ok=!!query(r.handle,0,&r.s);r.error=r.ok?0:GetLastError();if(r.ok){++good;allocated+=r.s.allocated;committed+=r.s.committed;}}
    auto elapsed=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-started).count();
    std::ostringstream out;out<<"{\"available\":true,\"timestamp\":"<<std::time(nullptr)<<",\"pid\":"<<GetCurrentProcessId()<<",\"capture_us\":"<<elapsed<<",\"heaps_found\":"<<found<<",\"heaps_summarized\":"<<good<<",\"allocated_bytes\":"<<allocated<<",\"committed_bytes\":"<<committed<<",\"committed_minus_allocated_bytes\":"<<(committed>=allocated?committed-allocated:0)<<",\"heaps\":[";
    for(unsigned i=0;i<found;++i){auto& r=rows[i];if(i)out<<',';out<<"{\"handle\":"<<reinterpret_cast<std::uintptr_t>(r.handle)<<",\"default_heap\":"<<(r.handle==GetProcessHeap()?"true":"false")<<",\"crt_heap\":"<<(r.handle==reinterpret_cast<HANDLE>(_get_heap_handle())?"true":"false")<<",\"ok\":"<<(r.ok?"true":"false")<<",\"error\":"<<r.error<<",\"allocated_bytes\":"<<r.s.allocated<<",\"committed_bytes\":"<<r.s.committed<<",\"reserved_bytes\":"<<r.s.reserved<<'}';}
    out<<"],\"coverage\":\"HeapSummary totals include supported Windows heaps, including CRT and library allocations. Committed minus allocated includes allocator overhead and reusable space; it is not an exact fragmentation measure. Direct VirtualAlloc, images and mapped files are outside these heap totals. Concurrent heap creation/destruction can make snapshots incomplete. Capture cost can affect timing; excluded from normal timing comparisons.\"}";return out.str();
}
}
