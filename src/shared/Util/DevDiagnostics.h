#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
// DEV-only, fixed-capacity timing and trace recorder. No heap allocation or
// shared mutex in Scope. Cross-thread snapshots use relaxed atomic counters.
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <algorithm>
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
namespace ManTech::Diag {
inline std::uint64_t ThreadCpuMicros() {
#ifdef _WIN32
    FILETIME created{}, exited{}, kernel{}, user{};
    if (GetThreadTimes(GetCurrentThread(), &created, &exited, &kernel, &user)) {
        auto ticks=[](FILETIME t){return (std::uint64_t(t.dwHighDateTime)<<32)|t.dwLowDateTime;};
        return (ticks(kernel)+ticks(user))/10;
    }
#endif
    return 0;
}
enum class Metric : unsigned {
    World, Maps, Map, MapQueue, MapBarrier, GridWorker, ObjectBuild,
    BotBatch, PlayerCore, Objects, Scripts, Send, MovementFlush,
    Player, Creature, Auras, Spell, Movement, Path, Visibility, Session,
    Packet, DbCallbacks, BotAI, BotReaction, BotDecision, BotTrigger,
    BotUseful, BotPossible, BotExecute, BotValue, DbExecute, JobQueue, JobExecute, TaskWait, DbCall, DbLock, Count
};
inline constexpr char const* Names[]={"world","map_batch","map_update","map_queue_wait","map_barrier_wait","grid_worker","object_build_worker","map_bot_batch","map_player_core","map_objects","map_scripts","map_send","map_movement_flush","player_update","creature_update","auras","spell_update","movement","pathfinding","visibility","session","packet","database_callbacks","bot_ai","bot_reaction","bot_decision","bot_trigger","bot_action_useful","bot_action_possible","bot_action_execute","bot_value","database_execute","job_queue_wait","job_execute","task_group_wait","mysql_call","database_connection_lock"};
inline constexpr unsigned Metrics=unsigned(Metric::Count), MaxThreads=64, Bins=256, TraceCapacity=2048, NamedCapacity=2048, LabelCapacity=4096, SlowCapacity=512;
inline std::uint64_t Now() {return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();}
inline unsigned Bucket(std::uint64_t us) { auto ms=us/1000; return ms<128 ? unsigned(ms) : unsigned(std::min<std::uint64_t>(255,128+(ms-128)/8)); }
inline std::uint64_t Upper(unsigned b) { return b==255 ? 0 : (b<128 ? (b+1)*1000ULL : (128+(b-128+1)*8)*1000ULL); }
inline void Maximum(std::atomic<std::uint64_t>& a,std::uint64_t v) { auto p=a.load(std::memory_order_relaxed);while(p<v&&!a.compare_exchange_weak(p,v,std::memory_order_relaxed)){} }
struct Stat {
    std::atomic<std::uint64_t> calls{0},samples{0},wall{0},cpu{0},cpuSamples{0},maximum{0};
    std::array<std::atomic<std::uint64_t>,Bins> histogram{};
};
struct NamedStat {std::atomic<std::uint64_t> key{0},samples{0},wall{0},maximum{0};};
struct Event {std::atomic<std::uint64_t> version{0},generation{0},start{0},duration{0},cpu{0},context{0},kind{0},label{0};};
struct alignas(64) Thread {
    std::array<Stat,Metrics> stats{};
    std::array<NamedStat,NamedCapacity> named{};
    std::array<Event,TraceCapacity> events{};
    std::array<Event,SlowCapacity> slow{};
    std::atomic<std::uint64_t> slowCount{0},osId{0};
    std::atomic<std::uint64_t> eventCount{0},namedDrops{0};
};
struct Label {std::atomic<std::uint64_t> hash{0};char text[96]{};};
#ifdef MANTECH_DEV_DIAGNOSTICS
inline std::array<Thread,MaxThreads> Threads{};
inline std::array<Label,LabelCapacity> Labels{};
inline std::atomic<unsigned> Assigned{0};
inline std::atomic<std::uint64_t> ThreadDrops{0},LabelDrops{0},TraceGeneration{0},TraceDeadline{0};
inline std::atomic<bool> Enabled{true};
inline thread_local Thread* Local=nullptr;
inline thread_local bool Registered=false;
inline thread_local std::uint64_t Context=0;
inline char const* MapLabel(){thread_local char name[80];std::snprintf(name,sizeof(name),"map:%u instance:%u",unsigned(Context>>32),unsigned(Context));return name;}
inline thread_local std::array<std::uint64_t,Metrics> Sequence{};
inline Thread* GetThread() {
    if(!Registered) {Registered=true;auto id=Assigned.fetch_add(1);if(id<MaxThreads){Local=&Threads[id];Local->osId=GetCurrentThreadId();}else ThreadDrops.fetch_add(1);}
    return Local;
}
inline unsigned LabelId(char const* s) {
    if(!s||!*s)return 0;
    std::uint64_t h=1469598103934665603ULL;for(auto p=s;*p;++p){h^=static_cast<unsigned char>(*p);h*=1099511628211ULL;}if(h<2)h+=2;
    for(unsigned n=0;n<16;++n) {
        auto i=unsigned((h+n)%LabelCapacity);auto& l=Labels[i];auto found=l.hash.load(std::memory_order_acquire);
        if(found==h)return i+1;
        if(!found&&l.hash.compare_exchange_strong(found,1,std::memory_order_acq_rel)) {
            std::strncpy(l.text,s,sizeof(l.text)-1);l.hash.store(h,std::memory_order_release);return i+1;
        }
    }
    LabelDrops.fetch_add(1,std::memory_order_relaxed);return 0;
}
inline void Named(Thread& t,unsigned metric,unsigned label,std::uint64_t us) {
    if(!label)return;
    std::uint64_t key=(std::uint64_t(metric+1)<<32)|label;
    for(unsigned n=0;n<16;++n) {
        auto& s=t.named[((key^(key>>32))*11400714819323198485ULL+n)%NamedCapacity];auto k=s.key.load(std::memory_order_relaxed);
        if(!k)s.key.compare_exchange_strong(k,key,std::memory_order_relaxed);
        if(k==key||(!k&&s.key.load(std::memory_order_relaxed)==key)) {s.samples.fetch_add(1,std::memory_order_relaxed);s.wall.fetch_add(us,std::memory_order_relaxed);Maximum(s.maximum,us);return;}
    }
    t.namedDrops.fetch_add(1,std::memory_order_relaxed);
}
// Always-on slow spans are separate from the short high-volume trace ring.
// They describe sampled detailed operations, not every call or OS CPU stacks.
inline void Slow(Thread& t,Metric m,unsigned label,std::uint64_t start,std::uint64_t duration,std::uint64_t cpu,std::uint64_t context) {
    auto threshold=(m==Metric::World||m==Metric::Maps||m==Metric::Map||m==Metric::MapBarrier)?100000ULL:2000ULL;
    if(duration<threshold)return;
    auto n=t.slowCount.fetch_add(1,std::memory_order_relaxed);auto& e=t.slow[n%SlowCapacity];
    e.version.fetch_add(1,std::memory_order_acq_rel);
    e.start=start;e.duration=duration;e.cpu=cpu;e.context=context;e.kind=unsigned(m);e.label=label;e.generation=n+1;
    e.version.fetch_add(1,std::memory_order_release);
}
inline void Trace(Thread& t,Metric m,unsigned label,std::uint64_t start,std::uint64_t duration,std::uint64_t cpu,std::uint64_t context) {
    if(!TraceDeadline.load(std::memory_order_relaxed)||Now()>TraceDeadline.load(std::memory_order_relaxed))return;
    auto gen=TraceGeneration.load(std::memory_order_acquire);
    auto n=t.eventCount.fetch_add(1,std::memory_order_relaxed);auto& e=t.events[n%TraceCapacity];
    e.version.fetch_add(1,std::memory_order_acq_rel);
    e.generation.store(gen,std::memory_order_relaxed);e.start.store(start,std::memory_order_relaxed);e.duration.store(duration,std::memory_order_relaxed);
    e.cpu.store(cpu,std::memory_order_relaxed);e.context.store(context,std::memory_order_relaxed);e.kind.store(unsigned(m),std::memory_order_relaxed);e.label.store(label,std::memory_order_relaxed);
    e.version.fetch_add(1,std::memory_order_release);
}
inline void Sample(Thread& t,Metric metric,std::uint64_t us,std::uint64_t cpu=0,bool hasCpu=false) {
    auto& s=t.stats[unsigned(metric)];s.samples.fetch_add(1,std::memory_order_relaxed);s.wall.fetch_add(us,std::memory_order_relaxed);Maximum(s.maximum,us);
    s.histogram[Bucket(us)].fetch_add(1,std::memory_order_relaxed);
    if(hasCpu){s.cpu.fetch_add(cpu,std::memory_order_relaxed);s.cpuSamples.fetch_add(1,std::memory_order_relaxed);}
}
class MapContext {
    std::uint64_t old;
public: MapContext(unsigned map,unsigned instance):old(Context){Context=(std::uint64_t(map)<<32)|instance;}
    ~MapContext(){Context=old;}
};
class Scope {
    Thread* t=nullptr;Metric metric;unsigned label=0;std::uint64_t started=0,cpuStarted=0,context=0;bool cpuEnabled=false;
public:
    explicit Scope(Metric m,unsigned every=1,char const* name=nullptr):metric(m) {
        if(!Enabled.load(std::memory_order_relaxed))return;
        auto thread=GetThread();if(!thread)return;
        thread->stats[unsigned(m)].calls.fetch_add(1,std::memory_order_relaxed);
        if((++Sequence[unsigned(m)]%std::max(1u,every))!=0)return;
        t=thread;context=Context;label=LabelId(name);
        cpuEnabled=(m==Metric::World||m==Metric::Map||m==Metric::Maps||m==Metric::MapBarrier||m==Metric::GridWorker||m==Metric::ObjectBuild||m==Metric::JobExecute||m==Metric::TaskWait);
        started=Now();if(cpuEnabled)cpuStarted=ThreadCpuMicros();
    }
    void Finish(){if(t){auto duration=Now()-started;auto cpu=cpuEnabled?ThreadCpuMicros():0;cpu=cpu>=cpuStarted?cpu-cpuStarted:0;Sample(*t,metric,duration,cpu,cpuEnabled);Named(*t,unsigned(metric),label,duration);Trace(*t,metric,label,started,duration,cpu,context);Slow(*t,metric,label,started,duration,cpu,context);t=nullptr;}}
    ~Scope(){Finish();}
    bool Sampling()const{return t!=nullptr;}
    void SetName(char const* name){if(t&&name){label=LabelId(name);started=Now();if(cpuEnabled)cpuStarted=ThreadCpuMicros();}}
    Scope(Scope const&)=delete;Scope& operator=(Scope const&)=delete;
};
inline void Record(Metric m,std::uint64_t us) {
    if(!Enabled.load(std::memory_order_relaxed))return;
    if(auto t=GetThread()){t->stats[unsigned(m)].calls.fetch_add(1,std::memory_order_relaxed);Sample(*t,m,us);}
}
inline void Queue(std::uint64_t queued,unsigned map,unsigned instance) {
    if(!queued||!Enabled.load(std::memory_order_relaxed))return;
    if(auto t=GetThread()){auto us=Now()-queued;Record(Metric::MapQueue,us);Trace(*t,Metric::MapQueue,0,queued,us,0,(std::uint64_t(map)<<32)|instance);Slow(*t,Metric::MapQueue,0,queued,us,0,(std::uint64_t(map)<<32)|instance);}
}
inline void JobQueued(std::uint64_t queued,char const* name) {
    if(!Enabled.load(std::memory_order_relaxed)||!queued)return;
    if(auto t=GetThread()){auto us=Now()-queued;auto label=LabelId(name);Record(Metric::JobQueue,us);Named(*t,unsigned(Metric::JobQueue),label,us);Slow(*t,Metric::JobQueue,label,queued,us,0,Context);Trace(*t,Metric::JobQueue,label,queued,us,0,Context);}
}

#else
class Scope {public: explicit Scope(Metric,unsigned=1,char const* = nullptr){}};
class MapContext {public:MapContext(unsigned,unsigned){}};
inline void Record(Metric,std::uint64_t){} inline void Queue(std::uint64_t,unsigned,unsigned){}
#endif
}
#ifdef MANTECH_DEV_DIAGNOSTICS
#define MANTECH_DIAG_JOIN_I(a,b) a##b
#define MANTECH_DIAG_JOIN(a,b) MANTECH_DIAG_JOIN_I(a,b)
#define MANTECH_DIAG_SCOPE(metric,every,name) MANTECH_DIAG_BEGIN(MANTECH_DIAG_JOIN(devDiagScope_,__LINE__),metric,every,name)
#define MANTECH_DIAG_CONTEXT(map,instance) ManTech::Diag::MapContext MANTECH_DIAG_JOIN(devDiagContext_,__LINE__)(map,instance)
#define MANTECH_DIAG_BEGIN(var,metric,every,name) ManTech::Diag::Scope var(ManTech::Diag::Metric::metric,every); if(var.Sampling())var.SetName(name)
#define MANTECH_DIAG_END(var) var.Finish()
#else
#define MANTECH_DIAG_SCOPE(metric,every,name) ((void)0)
#define MANTECH_DIAG_CONTEXT(map,instance) ((void)0)
#define MANTECH_DIAG_BEGIN(var,metric,every,name) ((void)0)
#define MANTECH_DIAG_END(var) ((void)0)
#endif
