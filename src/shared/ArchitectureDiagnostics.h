#pragma once
// Temporary TD01-TD10: remove this header and TurtleDiagnostics call sites after
// matched workload acceptance. No gameplay decisions depend on these counters.
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <algorithm>

namespace TurtleDiagnostics
{
using Clock = std::chrono::steady_clock;
inline uint64_t Micros() { return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now().time_since_epoch()).count(); }
enum Kind { MapTotal, MapQueue, Selection, Discovery, DiscoveryWait, ObjectUpdates,
    CreatureUpdate, GameObjectUpdate, DynamicObjectUpdate, CreatureAI, Motion, Path,
    PlayerCore, BotAI, SendObjects, Visibility, DatabaseWait, DatabaseRead, Callback,
    InputQueue, InputHandler, Completion, Scripts, GridMaintenance,
    WorldPrelude, WorldSessions, WorldTasks, WorldTransports, WorldMaps, WorldServices, WorldTail,
    CellCount, ObjectCount, StaleObject, BackgroundSkipped, BotGridOnly, BackgroundCreatureDeferred,
    CreatureForeground, CreatureInteractive, CreatureScriptProtected, CreatureAuraProtected, CreatureOther, Count };
inline char const* const names[] = {"map_total", "map_queue", "cell_select", "cell_discovery", "cell_worker_wait", "object_updates",
    "creature", "gameobject", "dynamicobject", "creature_ai", "motion", "path", "player_core", "bot_ai",
    "send_objects", "visibility", "db_connection_wait", "db_read", "db_callback", "input_queue", "input_handler",
    "map_completion", "scripts", "grid_maintenance",
    "world_prelude", "world_sessions", "world_tasks", "world_transports", "world_maps", "world_services", "world_tail",
    "selected_cells", "discovered_objects", "stale_objects", "background_skipped", "bot_grid_only", "background_creature_deferred",
    "creature_foreground", "creature_interactive", "creature_script_protected", "creature_aura_protected", "creature_other"};
static_assert(sizeof(names) / sizeof(names[0]) == Count, "diagnostic metric names must match their enum");

// Creature-only phases: Unit::Update probes attach only to this exact creature,
// never to the independent player update pass. No world pointers are retained.
enum CreaturePhase { CreatureHooks, CreatureState, UnitHooks, UnitVisibility,
    UnitEvents, UnitSpells, UnitAuraCleanup, UnitCombat, UnitReactives,
    UnitMovementChecks, UnitSpline, UnitMotion, UnitDeferredMotion, UnitWorld,
    CreatureCombat, CreatureScriptAI, CreatureRegen, CreaturePhaseCount };
inline char const* const creaturePhaseNames[] = {"hooks", "state", "unit_hooks", "visibility",
    "events", "spells", "aura_cleanup", "unit_combat", "reactives", "movement_checks",
    "spline", "motion", "deferred_motion", "world_actions", "creature_combat", "ai", "regen"};
static_assert(sizeof(creaturePhaseNames) / sizeof(creaturePhaseNames[0]) == CreaturePhaseCount,
    "creature phase names must match their enum");
struct CreatureSample
{
    uint64_t guid = 0, tick = 0, elapsed = 0;
    uint32_t entry = 0, state = 0, updateDiff = 0;
    bool combat = false;
    std::array<uint64_t, CreaturePhaseCount> phases{};
};
struct Metric
{
    uint64_t count = 0, total = 0, peak = 0;
    std::array<uint64_t, 32> buckets{};
    void Add(uint64_t value)
    {
        ++count; total += value; peak = std::max(peak, value);
        unsigned bucket = 0; uint64_t bound = 1;
        while (bound < value && bucket + 1 < buckets.size()) { bound <<= 1; ++bucket; }
        ++buckets[bucket];
    }
    uint64_t Percentile(unsigned percent) const
    {
        if (!count) return 0;
        uint64_t target = (count * percent + 99) / 100, seen = 0;
        for (unsigned n = 0; n < buckets.size(); ++n)
            if ((seen += buckets[n]) >= target)
                return n + 1 == buckets.size() ? peak : std::min(peak, uint64_t(1) << n);
        return peak;
    }
};
struct Summary
{
    std::array<Metric, Count> metrics{};
    std::array<Metric, CreaturePhaseCount> creaturePhases{};
    CreatureSample slowestCreature;
    uint64_t creatureSamples = 0, lastReport = Micros(), lastSlowReport = 0;
};
struct Context { uint32_t map = UINT32_MAX, instance = 0; uint64_t tick = 0; };
inline std::atomic<bool> enabled{false};
inline std::atomic<uint32_t> intervalMs{30000};
inline void (*sink)(char const*) = nullptr; // assigned once before worker startup
inline thread_local Summary* current = nullptr;
inline thread_local Context context;
// The submitting map owner must remain joined/inactive until this scope ends.
// Unlike Frame this only transfers attribution; it does not report/reset data.
class OwnerHandoff
{
public:
    OwnerHandoff(Summary* owner, Context next) : previous(current), prior(context)
    { current = owner; context = next; }
    ~OwnerHandoff() { current = previous; context = prior; }
    OwnerHandoff(OwnerHandoff const&) = delete;
    OwnerHandoff& operator=(OwnerHandoff const&) = delete;
private:
    Summary* previous;
    Context prior;
};
inline void ObserveCreature(Summary& summary, CreatureSample const& sample)
{
    for (unsigned n = 0; n < CreaturePhaseCount; ++n)
        summary.creaturePhases[n].Add(sample.phases[n]);
    if (!summary.creatureSamples || sample.elapsed > summary.slowestCreature.elapsed)
        summary.slowestCreature = sample;
    ++summary.creatureSamples;
}
class CreatureProbe;
inline thread_local CreatureProbe* activeCreature = nullptr;
class CreatureProbe
{
public:
    CreatureProbe(void const* unit, uint64_t guid, uint32_t entry, uint32_t state, bool combat, uint32_t updateDiff)
        : owner(current), previous(activeCreature), unit(unit)
    {
        if (!owner) return;
        sample.guid = guid; sample.entry = entry; sample.state = state;
        sample.combat = combat; sample.updateDiff = updateDiff; sample.tick = context.tick;
        started = phaseStarted = Micros();
        activeCreature = this;
    }
    ~CreatureProbe()
    {
        if (!owner) return;
        uint64_t const now = Micros();
        sample.phases[phase] += now - phaseStarted;
        sample.elapsed = now - started;
        ObserveCreature(*owner, sample);
        activeCreature = previous;
    }
    static void Stage(void const* unit, CreaturePhase next)
    {
        CreatureProbe* probe = activeCreature;
        if (!probe || probe->unit != unit) return;
        uint64_t const now = Micros();
        probe->sample.phases[probe->phase] += now - probe->phaseStarted;
        probe->phase = next;
        probe->phaseStarted = now;
    }
    CreatureProbe(CreatureProbe const&) = delete;
    CreatureProbe& operator=(CreatureProbe const&) = delete;
private:
    Summary* owner;
    CreatureProbe* previous;
    void const* unit;
    CreatureSample sample;
    CreaturePhase phase = CreatureHooks;
    uint64_t started = 0, phaseStarted = 0;
};
inline void Record(Kind kind, uint64_t value)
{
    if (current) current->metrics[kind].Add(value);
}
class Scope
{
public:
    explicit Scope(Kind kind) : owner(current), kind(kind), start(owner ? Micros() : 0) {}
    ~Scope() { Finish(); }
    void Finish() { if (owner) { owner->metrics[kind].Add(Micros() - start); owner = nullptr; } }
private:
    Summary* owner;
    Kind kind;
    uint64_t start;
};
class Frame
{
public:
    Frame(Summary& summary, uint32_t map, uint32_t instance, uint64_t tick)
        : summary(summary), previous(current), previousContext(context), map(map), instance(instance), tick(tick)
    {
        current = enabled.load(std::memory_order_relaxed) ? &summary : nullptr;
        context = current ? Context{map, instance, tick} : Context{};
        if (current)
        {
            started = Micros();
            for (unsigned n = 0; n < Count; ++n) baseline[n] = summary.metrics[n].total;
        }
    }
    ~Frame()
    {
        uint64_t const now = current ? Micros() : 0;
        if (current && sink && now - started >= 250000 && now - summary.lastSlowReport >= 1000000)
        {
            auto delta = [this](Kind kind) { return static_cast<unsigned long long>(summary.metrics[kind].total - baseline[kind]); };
            char line[768];
            std::snprintf(line, sizeof(line),
                "TW_DIAG_SLOW map=%u inst=%u tick=%llu elapsed_us=%llu queue_us=%llu select_us=%llu discovery_us=%llu objects_us=%llu creature_ai_us=%llu path_us=%llu player_us=%llu bot_ai_us=%llu send_us=%llu visibility_us=%llu db_wait_us=%llu db_read_us=%llu callback_us=%llu completion_us=%llu scripts_us=%llu grids_us=%llu",
                map, instance, static_cast<unsigned long long>(tick), static_cast<unsigned long long>(now - started),
                delta(MapQueue), delta(Selection), delta(Discovery), delta(ObjectUpdates), delta(CreatureAI), delta(Path),
                delta(PlayerCore), delta(BotAI), delta(SendObjects), delta(Visibility), delta(DatabaseWait), delta(DatabaseRead), delta(Callback),
                delta(Completion), delta(Scripts), delta(GridMaintenance));
            sink(line);
            summary.lastSlowReport = now;
        }
        if (current && sink && Micros() - summary.lastReport >= uint64_t(intervalMs.load()) * 1000)
        {
            auto const thread = static_cast<unsigned long long>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
            char line[512];
            uint64_t const window = Micros() - summary.lastReport;
            for (unsigned n = 0; n < Count; ++n)
            {
                auto& m = summary.metrics[n];
                if (m.count)
                {
                    std::snprintf(line, sizeof(line), "TW_DIAG map=%u inst=%u tick=%llu thread=%llu window_us=%llu kind=%s unit=%s n=%llu total=%llu mean=%llu p95_upper=%llu p99_upper=%llu max=%llu",
                        map, instance, static_cast<unsigned long long>(tick), thread, static_cast<unsigned long long>(window), names[n], n >= CellCount ? "count" : "us",
                        static_cast<unsigned long long>(m.count), static_cast<unsigned long long>(m.total), static_cast<unsigned long long>(m.total / m.count),
                        static_cast<unsigned long long>(m.Percentile(95)), static_cast<unsigned long long>(m.Percentile(99)), static_cast<unsigned long long>(m.peak));
                    sink(line);
                }
                m = {};
            }
            if (summary.creatureSamples)
            {
                for (unsigned n = 0; n < CreaturePhaseCount; ++n)
                {
                    Metric& m = summary.creaturePhases[n];
                    std::snprintf(line, sizeof(line),
                        "TW_CREATURE_PHASE map=%u inst=%u window_us=%llu phase=%s n=%llu total_us=%llu mean_us=%llu p95_upper_us=%llu max_us=%llu",
                        map, instance, static_cast<unsigned long long>(window), creaturePhaseNames[n],
                        static_cast<unsigned long long>(m.count), static_cast<unsigned long long>(m.total),
                        static_cast<unsigned long long>(m.count ? m.total / m.count : 0),
                        static_cast<unsigned long long>(m.Percentile(95)), static_cast<unsigned long long>(m.peak));
                    sink(line);
                    m = {};
                }
                auto const& slow = summary.slowestCreature;
                char detail[1024];
                int written = std::snprintf(detail, sizeof(detail),
                    "TW_CREATURE_SLOWEST map=%u inst=%u tick=%llu guid=%llu entry=%u state=%u combat=%u update_diff_ms=%u elapsed_us=%llu",
                    map, instance, static_cast<unsigned long long>(slow.tick), static_cast<unsigned long long>(slow.guid),
                    slow.entry, slow.state, unsigned(slow.combat), slow.updateDiff, static_cast<unsigned long long>(slow.elapsed));
                size_t used = written > 0 ? std::min(size_t(written), sizeof(detail) - 1) : 0;
                for (unsigned n = 0; n < CreaturePhaseCount && used < sizeof(detail) - 1; ++n)
                {
                    written = std::snprintf(detail + used, sizeof(detail) - used, " %s_us=%llu",
                        creaturePhaseNames[n], static_cast<unsigned long long>(slow.phases[n]));
                    if (written <= 0) break;
                    used += std::min(size_t(written), sizeof(detail) - used - 1);
                }
                sink(detail);
                summary.slowestCreature = {};
                summary.creatureSamples = 0;
            }
            summary.lastReport = Micros();
        }
        current = previous;
        context = previousContext;
    }
private:
    Summary& summary;
    Summary* previous;
    Context previousContext;
    std::array<uint64_t, Count> baseline{};
    uint64_t started = 0;
    uint32_t map, instance;
    uint64_t tick;
};
}
