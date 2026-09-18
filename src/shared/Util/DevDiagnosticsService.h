#pragma once
#include "Util/DevDiagnostics.h"
#ifdef MANTECH_DEV_DIAGNOSTICS
#include "Memory/MemoryLedger.h"
#include "Memory/AllocationProfile.h"
#include "Memory/EntityLedger.h"
#include <ctime>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <mutex>
#include <psapi.h>
#include <vector>
#include "Util/HeapDiagnostics.h"
namespace ManTech::Diag {
inline std::string Json(char const* s) {std::string r="\"";for(;*s;++s){if(*s=='"'||*s=='\\')r+='\\';if(static_cast<unsigned char>(*s)>=32)r+=*s;}return r+'"';}
inline std::uint64_t Percentile(std::array<std::uint64_t,Bins> const& h,unsigned percent) {
    std::uint64_t n=0;for(auto x:h)n+=x;if(!n)return 0;auto target=(n*percent+99)/100;std::uint64_t sum=0;
    for(unsigned b=0;b<Bins;++b){sum+=h[b];if(sum>=target)return Upper(b);}return 0;
}
inline void Publish(char const* path,std::string const& contents) {
    std::string temp=std::string(path)+".tmp";{std::ofstream f(temp,std::ios::binary);f<<contents;if(!f)return;}
    MoveFileExA(temp.c_str(),path,MOVEFILE_REPLACE_EXISTING);
}
inline std::string AllocationJson(){auto a=ReadAllocationTotals();std::ostringstream o;o<<"{\"allocated_sample_bytes\":"<<a.allocated<<",\"freed_sample_bytes\":"<<a.freed<<",\"live_sample_bytes\":"<<a.live<<",\"live_samples\":"<<a.count<<",\"samples\":"<<a.samples<<",\"allocation_drops\":"<<a.drops<<",\"site_drops\":"<<a.siteDrops<<",\"probability_denominator\":"<<a.probability<<",\"recording\":"<<(a.recording?"true":"false")<<'}';return o.str();}
inline std::string SlowJson(){
    struct Row{std::uint64_t start,duration,cpu,context,kind,label,id,thread,os;};
    std::vector<Row> rows;auto now=Now();std::uint64_t total=0;
    for(unsigned t=0;t<std::min(Assigned.load(),MaxThreads);++t){total+=Threads[t].slowCount.load();for(auto& e:Threads[t].slow){auto version=e.version.load(std::memory_order_acquire);if(version&1)continue;Row r{e.start.load(),e.duration.load(),e.cpu.load(),e.context.load(),e.kind.load(),e.label.load(),e.generation.load(),t,Threads[t].osId.load()};if(version!=e.version.load(std::memory_order_acquire)||!r.id||r.kind>=Metrics||r.start+r.duration+15000000<now)continue;rows.push_back(r);}}
    std::sort(rows.begin(),rows.end(),[](auto const& a,auto const& b){return a.duration>b.duration;});
    auto available=rows.size();std::vector<Row> selected;unsigned major=0,detail=0;
    for(auto const& r:rows){bool coarse=r.kind<=unsigned(Metric::ObjectBuild)||r.kind==unsigned(Metric::JobQueue)||r.kind==unsigned(Metric::JobExecute)||r.kind==unsigned(Metric::TaskWait);auto& count=coarse?major:detail;if(count++<100)selected.push_back(r);}rows.swap(selected);std::ostringstream o;
    o<<"{\"monotonic_us\":"<<now<<",\"threshold_detail_us\":2000,\"threshold_world_map_us\":100000,\"recorded_total\":"<<total<<",\"recent_available\":"<<available<<",\"spans\":[";
    bool first=true;for(auto const& r:rows){if(!first)o<<',';first=false;o<<"{\"metric\":"<<Json(Names[r.kind])<<",\"start_us\":"<<r.start<<",\"duration_us\":"<<r.duration<<",\"cpu_us\":"<<r.cpu<<",\"thread_slot\":"<<r.thread<<",\"os_thread_id\":"<<r.os<<",\"map\":"<<(r.context>>32)<<",\"instance\":"<<(r.context&0xffffffff)<<",\"sequence\":"<<r.id;if(r.label&&r.label<=LabelCapacity&&Labels[r.label-1].hash.load(std::memory_order_acquire)>=2)o<<",\"operation\":"<<Json(Labels[r.label-1].text);o<<'}';}o<<"],\"coverage\":\"Largest 100 major and 100 detailed retained slow spans completed in the last 15 seconds; fixed per-thread rings may overwrite detail. Detailed operations remain sampled. Concurrent nested spans overlap and must not be summed.\"}";return o.str();
}
inline std::string MemoryJson() {
    PROCESS_MEMORY_COUNTERS_EX p{};p.cb=sizeof(p);GetProcessMemoryInfo(GetCurrentProcess(),reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&p),sizeof(p));
    std::uint64_t priv=0,image=0,mapped=0,reserved=0,regions=0;MEMORY_BASIC_INFORMATION info{};
    for(std::uintptr_t address=0;VirtualQuery(reinterpret_cast<void*>(address),&info,sizeof(info));) {
        if(info.State==MEM_COMMIT){if(info.Type==MEM_PRIVATE)priv+=info.RegionSize;else if(info.Type==MEM_IMAGE)image+=info.RegionSize;else if(info.Type==MEM_MAPPED)mapped+=info.RegionSize;}
        if(info.State==MEM_RESERVE)reserved+=info.RegionSize;++regions;
        auto next=reinterpret_cast<std::uintptr_t>(info.BaseAddress)+info.RegionSize;if(next<=address)break;address=next;
    }
    static char const* names[]={"unit_base","aura_lists","update_fields","event_holders","nav_private","nav_shared","network_quota","optional_state","path_scratch","terrain","database_work","collision","nav_queries","aura_index_pages"};
    std::ostringstream s;s<<"{\"private_bytes\":"<<p.PrivateUsage<<",\"working_set_bytes\":"<<p.WorkingSetSize<<",\"private_committed_regions_bytes\":"<<priv<<",\"image_committed_regions_bytes\":"<<image<<",\"mapped_committed_regions_bytes\":"<<mapped<<",\"reserved_address_bytes\":"<<reserved<<",\"regions\":"<<regions<<",\"diagnostic_fixed_capacity_bytes\":"<<(sizeof(Threads)+sizeof(Labels)+AllocationProfileCapacity())<<",\"ledger\":[";
    for(unsigned k=0;k<unsigned(MemoryKind::Count);++k){auto v=MemoryLedger::Read(MemoryKind(k));bool measured=k==unsigned(MemoryKind::Units)||k==unsigned(MemoryKind::AuraBuckets)||k==unsigned(MemoryKind::UpdateFields)||k==unsigned(MemoryKind::OptionalState)||k==unsigned(MemoryKind::AuraIndexes)||k==unsigned(MemoryKind::EventHolders)||k==unsigned(MemoryKind::Network)||k==unsigned(MemoryKind::Terrain)||k==unsigned(MemoryKind::DatabaseWork)||k==unsigned(MemoryKind::NavTiles)||k==unsigned(MemoryKind::NavShared)||k==unsigned(MemoryKind::NavQueries)||k==unsigned(MemoryKind::Collision)||k==unsigned(MemoryKind::PathScratch);if(k)s<<',';s<<"{\"name\":"<<Json(names[k])<<",\"instrumented\":"<<(measured?"true":"false")<<",\"count\":"<<v.count<<",\"requested_bytes\":"<<v.bytes<<",\"peak_requested_bytes\":"<<v.peak<<'}';}
    s<<"],\"entities\":[";
    for(unsigned k=0;k<unsigned(EntityKind::Count);++k){if(k)s<<',';s<<"{\"name\":"<<Json(EntityLedger::Name(EntityKind(k)))<<",\"count\":"<<EntityLedger::Read(EntityKind(k))<<'}';}
    s<<"],\"coverage\":\"Ledger categories are partial and can overlap. Do not subtract their sum from private bytes or call the remainder a leak. Committed image/mapped regions are address-space classification, not resident RAM. Allocation captures exclude pre-existing allocations and external DLL malloc heaps.\"}";return s.str();
}
inline std::string Snapshot() {
    std::ostringstream s;s<<"{\"version\":1,\"pid\":"<<GetCurrentProcessId()<<",\"timestamp\":"<<std::time(nullptr)<<",\"enabled\":"<<(Enabled.load()?"true":"false")<<",\"capture_active\":"<<(TraceDeadline.load()>Now()?"true":"false")<<",\"threads_registered\":"<<Assigned.load()<<",\"thread_drops\":"<<ThreadDrops.load()<<",\"label_drops\":"<<LabelDrops.load()<<",\"metrics\":[";
    auto n=std::min(Assigned.load(),MaxThreads);
    for(unsigned m=0;m<Metrics;++m) {
        std::uint64_t calls=0,samples=0,wall=0,cpu=0,cpuSamples=0,max=0;std::array<std::uint64_t,Bins> hist{};
        for(unsigned t=0;t<n;++t){auto& a=Threads[t].stats[m];calls+=a.calls.load();samples+=a.samples.load();wall+=a.wall.load();cpu+=a.cpu.load();cpuSamples+=a.cpuSamples.load();max=std::max(max,a.maximum.load());for(unsigned b=0;b<Bins;++b)hist[b]+=a.histogram[b].load();}
        if(m)s<<',';s<<"{\"name\":"<<Json(Names[m])<<",\"calls\":"<<calls<<",\"samples\":"<<samples<<",\"sampled_wall_us\":"<<wall<<",\"mean_sample_us\":"<<(samples?wall/samples:0)<<",\"cpu_us\":"<<cpu<<",\"cpu_samples\":"<<cpuSamples<<",\"p50_upper_us\":"<<Percentile(hist,50)<<",\"p95_upper_us\":"<<Percentile(hist,95)<<",\"p99_upper_us\":"<<Percentile(hist,99)<<",\"histogram_overflow\":"<<hist.back()<<",\"max_us\":"<<max<<",\"histogram\":[";
        for(unsigned b=0;b<Bins;++b){if(b)s<<',';s<<hist[b];}s<<"]}";
    }
    s<<"],\"named_samples\":[";bool first=true;std::uint64_t drops=0;
    for(unsigned t=0;t<n;++t){drops+=Threads[t].namedDrops.load();for(auto& a:Threads[t].named){auto key=a.key.load();if(!key)continue;auto label=unsigned(key&0xffffffff);auto m=unsigned(key>>32)-1;if(label==0||label>LabelCapacity||m>=Metrics||Labels[label-1].hash.load(std::memory_order_acquire)<2)continue;if(!first)s<<',';first=false;s<<"{\"thread_slot\":"<<t<<",\"metric\":"<<Json(Names[m])<<",\"name\":"<<Json(Labels[label-1].text)<<",\"samples\":"<<a.samples.load()<<",\"sampled_wall_us\":"<<a.wall.load()<<",\"max_us\":"<<a.maximum.load()<<'}';}}
    s<<"],\"named_drops\":"<<drops<<",\"allocation_totals\":"<<AllocationJson()<<",\"slow_operations\":"<<SlowJson()<<",\"memory\":"<<MemoryJson()<<",\"notes\":\"Counters are cumulative concurrent snapshots. Detailed operations sample every 32 calls; names contain sampled counts. Durations are inclusive; do not sum nested scopes or parallel workers. Percentiles are histogram upper bounds (0 with overflow means unbounded). CPU clocks have OS resolution limits.\"}";
    return s.str();
}
inline void SaveTrace(std::uint64_t generation) {
    std::ostringstream s;s<<"{\"traceEvents\":[";bool first=true;std::uint64_t overwritten=0;
    for(unsigned t=0;t<std::min(Assigned.load(),MaxThreads);++t){auto total=Threads[t].eventCount.load();if(total>TraceCapacity)overwritten+=total-TraceCapacity;
        for(auto& e:Threads[t].events){auto before=e.version.load(std::memory_order_acquire);if(before&1)continue;auto gen=e.generation.load(),start=e.start.load(),duration=e.duration.load(),cpu=e.cpu.load(),context=e.context.load(),kind=e.kind.load(),label=e.label.load();if(before!=e.version.load(std::memory_order_acquire)||gen!=generation||kind>=Metrics)continue;
            if(!first)s<<',';first=false;s<<"{\"ph\":\"X\",\"name\":"<<Json(Names[kind])<<",\"pid\":"<<GetCurrentProcessId()<<",\"tid\":"<<t<<",\"ts\":"<<start<<",\"dur\":"<<duration<<",\"args\":{\"map\":"<<(context>>32)<<",\"instance\":"<<(context&0xffffffff)<<",\"cpu_us\":"<<cpu;
            if(label&&label<=LabelCapacity&&Labels[label-1].hash.load(std::memory_order_acquire)>=2)s<<",\"operation\":"<<Json(Labels[label-1].text);s<<"}}";
        }
    }
    s<<"],\"displayTimeUnit\":\"ms\",\"generation\":"<<generation<<",\"overwritten_records\":"<<overwritten<<",\"notes\":\"Instrumented scope timeline, not sampled CPU call stacks. Detailed scopes are sampled. CPU stacks require the separate Windows CPU capture tool. Ring retains the latest records; dropped records never affect gameplay.\"}";
    auto path="logs/DevDiagnostics-trace-"+std::to_string(generation%4)+".json";Publish(path.c_str(),s.str());
}
// One background observer does JSON formatting, virtual-address enumeration and
// optional symbolization. World/worker threads never write files or wait for it.
class Observer {
    std::atomic<bool> stopping{false};std::thread worker;
    void Run() {
        IgnoreAllocationProfileThread();
        std::uint64_t next=0,tracePending=0,memoryDeadline=0,retentionDeadline=0;
        while(!stopping.load()) {
            auto now=Now();
            std::ifstream command("logs/DevDiagnostics.control");std::string op;unsigned seconds=0;
            if(command>>op){command>>seconds;command.close();std::remove("logs/DevDiagnostics.control");
                if(op=="enabled")Enabled.store(seconds!=0);
                if(op=="capture"&&!tracePending){seconds=std::clamp(seconds,1u,30u);for(auto& t:Threads)t.eventCount.store(0);tracePending=TraceGeneration.fetch_add(1)+1;TraceDeadline.store(now+seconds*1000000ULL);}
                if(op=="heap"&&!memoryDeadline&&!retentionDeadline)Publish("logs/DevDiagnostics-heaps.json",HeapSnapshot());
                if(op=="memory"&&!memoryDeadline&&!retentionDeadline){seconds=std::clamp(seconds,1u,60u);Publish("logs/DevDiagnostics-heaps-before.json",HeapSnapshot());Publish("logs/DevDiagnostics-retention-before.json",Snapshot());StartAllocationProfile();memoryDeadline=Now()+seconds*1000000ULL;}
                next=0;
            }
            if(tracePending&&now>TraceDeadline.load()+500000){TraceDeadline.store(0);SaveTrace(tracePending);tracePending=0;}
            if(memoryDeadline&&now>=memoryDeadline){StopAllocationProfile();Publish("logs/DevDiagnostics-retention-end.json",Snapshot());Publish("logs/DevDiagnostics-heaps-end.json",HeapSnapshot());WriteAllocationProfile();memoryDeadline=0;retentionDeadline=Now()+60000000;}
            if(retentionDeadline&&now>=retentionDeadline){Publish("logs/DevDiagnostics-retention-after.json",Snapshot());Publish("logs/DevDiagnostics-heaps-after.json",HeapSnapshot());WriteAllocationProfile();retentionDeadline=0;}
            if(now>=next){auto started=Now();auto data=Snapshot();Publish("logs/DevDiagnostics-latest.json",data);std::error_code ec;auto size=std::filesystem::file_size("logs/DevDiagnostics-history.jsonl",ec);if(!ec&&size>32*1024*1024)MoveFileExA("logs/DevDiagnostics-history.jsonl","logs/DevDiagnostics-history.previous.jsonl",MOVEFILE_REPLACE_EXISTING);std::ofstream f("logs/DevDiagnostics-history.jsonl",std::ios::app);f<<data<<'\n';next=Now()+10000000;Publish("logs/DevDiagnostics-observer.json","{\"last_snapshot_us\":"+std::to_string(Now()-started)+",\"memory_capture_active\":"+((memoryDeadline||retentionDeadline)?"true":"false")+"}");}
            for(unsigned n=0;n<10&&!stopping.load();++n)std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if(memoryDeadline){StopAllocationProfile();WriteAllocationProfile();}
    }
public:Observer():worker([this]{Run();}){} ~Observer(){stopping=true;if(worker.joinable())worker.join();}
};
inline void StartObserver(){static Observer observer;}
}
#else
namespace ManTech::Diag {inline void StartObserver(){}}
#endif
