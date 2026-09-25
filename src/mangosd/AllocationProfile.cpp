// Optional diagnostic executable only. No allocator headers are inserted into
// user blocks; matching CRT/mimalloc allocation families retain their ownership.
#include "Memory/AllocationProfile.h"
#include <atomic>
#include <new>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <algorithm>
#include <malloc.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#ifdef MANTECH_USE_MIMALLOC
#include <mimalloc.h>
#endif
namespace {
// The full-population diagnostic exceeded 4096 distinct sites. Keep
// bounded tables, but size them for the observed 10k workload. Diagnostic only.
constexpr unsigned Slots=262144, Sites=131072, Frames=12, Probes=32;
struct Allocation { void* pointer; std::size_t bytes; unsigned site; };
struct Site { void* frames[Frames]; unsigned depth; std::uint64_t allocated, freed, liveBytes, liveCount, peakBytes; };
Allocation allocations[Slots]{};
Site sites[Sites]{};
std::atomic_flag gate=ATOMIC_FLAG_INIT;
thread_local bool recursive=false,ignoreNewSamples=false;
thread_local std::uint64_t randomState=0;
std::atomic<std::uint64_t> missedAllocations{0}, missedSites{0}, samples{0};
std::atomic<unsigned> sampleMask{1023};
std::uint64_t totalAllocated=0,totalFreed=0,totalLive=0;
std::atomic<bool> enabled{false}, tracking{false};
constexpr unsigned BloomWords=32768;
std::atomic<std::uint64_t> bloom[BloomWords]{};
std::atomic<std::uint64_t> activeSamples{0};
std::atomic_flag dumping=ATOMIC_FLAG_INIT;
struct Guard { Guard(){while(gate.test_and_set(std::memory_order_acquire)) YieldProcessor();} ~Guard(){gate.clear(std::memory_order_release);} };
unsigned hashPointer(void* p) { auto v=reinterpret_cast<std::uintptr_t>(p)>>4; v^=v>>23; v*=0x2127599bf4325c37ULL; return unsigned(v^(v>>32)); }
void forget(void* p) noexcept {
    if (!p || recursive || !tracking.load(std::memory_order_relaxed) || !activeSamples.load(std::memory_order_relaxed)) return;
    unsigned bh=hashPointer(p); if(!(bloom[(bh>>6)&(BloomWords-1)].load(std::memory_order_relaxed)&(1ULL<<(bh&63))))return;
    Guard lock;
    unsigned h=hashPointer(p)&(Slots-1);
    for(unsigned n=0;n<Probes;++n) {
        auto& a=allocations[(h+n)&(Slots-1)];
        if (!a.pointer) return;
        if(a.pointer==p) {
            auto& s=sites[a.site]; s.freed+=a.bytes; s.liveBytes-=a.bytes;totalFreed+=a.bytes;totalLive-=a.bytes; --s.liveCount;
            a.pointer=reinterpret_cast<void*>(1); activeSamples.fetch_sub(1,std::memory_order_relaxed); return;
        }
    }
}
void remember(void* p,std::size_t bytes) noexcept {
    if (!p || recursive || ignoreNewSamples || !enabled.load(std::memory_order_relaxed)) return;
    if (!randomState) randomState=GetCurrentThreadId()*0x9e3779b97f4a7c15ULL+1;
    randomState^=randomState<<13; randomState^=randomState>>7; randomState^=randomState<<17;
    if (randomState & sampleMask.load(std::memory_order_relaxed)) return;
    recursive=true;
    void* stack[Frames]{};
    unsigned depth=CaptureStackBackTrace(2,Frames,stack,nullptr);
    if (!depth) { ++missedSites; recursive=false; return; }
    unsigned h=0;
    for(unsigned i=0;i<depth;++i) h=h*16777619U^hashPointer(stack[i]);
    {
        Guard lock;
        if(!enabled.load(std::memory_order_relaxed)){recursive=false;return;}
        unsigned site=Sites;
        for(unsigned i=0;i<Probes;++i) {
            unsigned k=(h+i)&(Sites-1); auto& s=sites[k];
            if(!s.depth) { s.depth=depth; std::memcpy(s.frames,stack,sizeof(stack)); site=k; break; }
            if(s.depth==depth && !std::memcmp(s.frames,stack,sizeof(stack))) { site=k; break; }
        }
        if(site==Sites) ++missedSites;
        else {
            bool stored=false; h=hashPointer(p)&(Slots-1);
            for(unsigned i=0;i<Probes;++i) {
                auto& a=allocations[(h+i)&(Slots-1)];
                if(reinterpret_cast<std::uintptr_t>(a.pointer)<=1) {
                    a={p,bytes,site}; auto& s=sites[site]; s.allocated+=bytes; s.liveBytes+=bytes;totalAllocated+=bytes;totalLive+=bytes; ++s.liveCount;
                    if(s.liveBytes>s.peakBytes)s.peakBytes=s.liveBytes;
                    unsigned bh=hashPointer(p);bloom[(bh>>6)&(BloomWords-1)].fetch_or(1ULL<<(bh&63),std::memory_order_relaxed);
                    activeSamples.fetch_add(1,std::memory_order_relaxed);++samples; stored=true; break;
                }
            }
            if(!stored) ++missedAllocations;
        }
    }
    recursive=false;
}
void* allocate(std::size_t n,std::size_t alignment=0) {
    void* p=nullptr;
    for(;;) {
#ifdef MANTECH_USE_MIMALLOC
        p=alignment ? mi_malloc_aligned(n ? n : 1,alignment) : mi_malloc(n ? n : 1);
#else
        p=alignment ? _aligned_malloc(n ? n : 1,alignment) : std::malloc(n ? n : 1);
#endif
        if(p)break;
        auto handler=std::get_new_handler(); if(!handler)throw std::bad_alloc(); handler();
    }
    remember(p,n); return p;
}
void release(void* p,bool aligned=false) noexcept {
    forget(p);
#ifdef MANTECH_USE_MIMALLOC
    mi_free(p);
#else
    if(aligned)_aligned_free(p); else std::free(p);
#endif
}
}
namespace ManTech {
void BeginAllocationProfile(unsigned mask) { sampleMask=mask; tracking=true; enabled=true; }
void EndAllocationProfile() { enabled=false; Guard lock; }
void IgnoreProfileThread(){ignoreNewSamples=true;}
AllocationTotals ProfileTotals(){Guard lock;return {totalAllocated,totalFreed,totalLive,activeSamples.load(),samples.load(),missedAllocations.load(),missedSites.load(),sampleMask.load()+1,enabled.load()};}
std::size_t ProfileCapacity(){return sizeof(allocations)+sizeof(sites)*2+sizeof(bloom)+sizeof(unsigned)*Sites+sizeof(bool)*Sites;}
void DumpAllocationProfile() {
    if(!tracking || dumping.test_and_set())return;
    recursive=true;
    static Site snapshot[Sites];
    { Guard lock; std::memcpy(snapshot,sites,sizeof(sites)); }
    static unsigned rawSequence=0;
    char rawPath[80];std::snprintf(rawPath,sizeof(rawPath),"logs/DevDiagnostics-alloc-%u.tsv",(++rawSequence)%8);
    if(FILE* raw=std::fopen(rawPath,"w")){
        std::fprintf(raw,"# pid=%lu; probability=1/%u; all sampled sites; stacks are resolved in matching arch4-heap export\nsite\tlive_bytes\tcount\tallocated_bytes\tfreed_bytes\n",GetCurrentProcessId(),sampleMask.load()+1);
        for(unsigned i=0;i<Sites;++i)if(snapshot[i].depth)std::fprintf(raw,"%u\t%llu\t%llu\t%llu\t%llu\n",i,(unsigned long long)snapshot[i].liveBytes,(unsigned long long)snapshot[i].liveCount,(unsigned long long)snapshot[i].allocated,(unsigned long long)snapshot[i].freed);
        std::fclose(raw);
    }
    // Export the largest retained and churn sites, not an unbounded symbol dump.
    static unsigned rank[Sites]; static bool selected[Sites];
    std::memset(selected,0,sizeof(selected));unsigned used=0;
    for(unsigned i=0;i<Sites;++i)if(snapshot[i].depth)rank[used++]=i;
    std::sort(rank,rank+used,[&](unsigned a,unsigned b){return snapshot[a].liveBytes>snapshot[b].liveBytes;});
    for(unsigned i=0;i<(std::min)(1024u,used);++i)selected[rank[i]]=true;
    std::sort(rank,rank+used,[&](unsigned a,unsigned b){return snapshot[a].allocated>snapshot[b].allocated;});
    for(unsigned i=0;i<(std::min)(1024u,used);++i)selected[rank[i]]=true;
    static unsigned sequence=0;
    char path[80]; std::snprintf(path,sizeof(path),"arch4-heap-%03u.tsv",(++sequence)%8);
    FILE* f=std::fopen(path,"w");
    if(f) {
        std::fprintf(f,"# Sampled C++ allocations only; probability=1/%u; samples=%llu; allocation_drops=%llu; site_drops=%llu; fixed_table_bytes=%zu; export_limit=2048; selection=top_retained_and_allocated\n",
            sampleMask.load()+1,static_cast<unsigned long long>(samples.load()),static_cast<unsigned long long>(missedAllocations.load()),static_cast<unsigned long long>(missedSites.load()),sizeof(allocations)+sizeof(sites)+sizeof(snapshot)+sizeof(bloom)+sizeof(rank)+sizeof(selected));
        std::fprintf(f,"site\tlive_sample_bytes\tlive_samples\tallocated_sample_bytes\tfreed_sample_bytes\tpeak_sample_bytes\tstack\n");
        bool ownsSymbols=!!SymInitialize(GetCurrentProcess(),nullptr,TRUE);
        for(unsigned i=0;i<Sites;++i) {
            auto const& s=snapshot[i]; if(!s.depth||!selected[i])continue;
            std::fprintf(f,"%u\t%llu\t%llu\t%llu\t%llu\t%llu\t",i,(unsigned long long)s.liveBytes,(unsigned long long)s.liveCount,(unsigned long long)s.allocated,(unsigned long long)s.freed,(unsigned long long)s.peakBytes);
            for(unsigned j=0;j<s.depth;++j) {
                alignas(SYMBOL_INFO) char storage[sizeof(SYMBOL_INFO)+512]{};
                auto symbol=reinterpret_cast<SYMBOL_INFO*>(storage);symbol->SizeOfStruct=sizeof(SYMBOL_INFO);symbol->MaxNameLen=255;
                DWORD64 offset=0;
                if(SymFromAddr(GetCurrentProcess(),reinterpret_cast<DWORD64>(s.frames[j]),&offset,symbol))
                    std::fprintf(f,"%s+0x%llx%s",symbol->Name,(unsigned long long)offset,j+1<s.depth?";":"");
                else std::fprintf(f,"%p%s",s.frames[j],j+1<s.depth?";":"");
            }
            std::fputc('\n',f);
        }
        if(ownsSymbols)SymCleanup(GetCurrentProcess());
        std::fclose(f);
    }
    recursive=false; dumping.clear();
}
}
void* operator new(std::size_t n){return allocate(n);}
void* operator new[](std::size_t n){return allocate(n);}
void operator delete(void* p) noexcept {release(p);}
void operator delete[](void* p) noexcept {release(p);}
void operator delete(void* p,std::size_t) noexcept {release(p);}
void operator delete[](void* p,std::size_t) noexcept {release(p);}
void* operator new(std::size_t n,std::align_val_t a){return allocate(n,std::size_t(a));}
void* operator new[](std::size_t n,std::align_val_t a){return allocate(n,std::size_t(a));}
void operator delete(void* p,std::align_val_t) noexcept {release(p,true);}
void operator delete[](void* p,std::align_val_t) noexcept {release(p,true);}
void operator delete(void* p,std::size_t,std::align_val_t) noexcept {release(p,true);}
void operator delete[](void* p,std::size_t,std::align_val_t) noexcept {release(p,true);}
void* operator new(std::size_t n,std::nothrow_t const&) noexcept {try{return allocate(n);}catch(...){return nullptr;}}
void* operator new[](std::size_t n,std::nothrow_t const&) noexcept {try{return allocate(n);}catch(...){return nullptr;}}
void operator delete(void* p,std::nothrow_t const&) noexcept {release(p);}
void operator delete[](void* p,std::nothrow_t const&) noexcept {release(p);}
void* operator new(std::size_t n,std::align_val_t a,std::nothrow_t const&) noexcept {try{return allocate(n,std::size_t(a));}catch(...){return nullptr;}}
void* operator new[](std::size_t n,std::align_val_t a,std::nothrow_t const&) noexcept {try{return allocate(n,std::size_t(a));}catch(...){return nullptr;}}
void operator delete(void* p,std::align_val_t,std::nothrow_t const&) noexcept {release(p,true);}
void operator delete[](void* p,std::align_val_t,std::nothrow_t const&) noexcept {release(p,true);}

namespace { struct ProfileRegistration { ProfileRegistration() { ManTech::allocationProfileStart=ManTech::BeginAllocationProfile; ManTech::allocationProfileWrite=ManTech::DumpAllocationProfile; ManTech::allocationProfileStop=ManTech::EndAllocationProfile; ManTech::allocationProfileCapacity=ManTech::ProfileCapacity; ManTech::allocationProfileTotals=ManTech::ProfileTotals; ManTech::allocationProfileIgnoreThread=ManTech::IgnoreProfileThread; } } profileRegistration; }
