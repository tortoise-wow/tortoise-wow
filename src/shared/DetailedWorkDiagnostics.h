#pragma once
#include "ArchitectureDiagnostics.h"
#include <map>
#include <utility>

// Temporary TD11: bounded summaries, inclusive nested timings, numeric GUIDs
// only. These probes neither retain world pointers nor change scheduling.
namespace DetailedWork
{
enum Kind { Manager, Sessions, TeleportAck, GhostRecovery, Activity, Memory,
    LoginManager, Population, MaintenanceBatch, MaintenanceBot, FreeBots,
    LocationLog, Facing, AuctionMirror, Auctions, RandomLock, RandomPoint,
    RandomLaunch, TargetLock, TargetLocation, NavQuery, WalkPoly, RandomNav,
    WalkHeight, AuctionPurchase, AuctionProposition, AuctionCleanup,
    BotPackets, BotCacheCleanup, BotNearby, BotRandomize, BotStrategy,
    TeleportPlan, TeleportFaction, TeleportShuffle, TeleportActive,
    TeleportArea, TeleportCommit, BotRefresh, BotEventRead,
    RandomPath, SplineLaunch, PacketCompression, MovementDelivery, Count };
inline char const* const names[] = {"manager", "sessions", "teleport_ack", "ghost_recovery",
    "activity", "memory", "login_manager", "population", "maintenance_batch", "maintenance_bot",
    "free_bots", "location_log", "facing", "auction_mirror", "auctions", "random_lock",
    "random_point", "random_launch", "target_lock", "target_location", "nav_query", "walk_poly",
    "random_nav", "walk_height", "auction_purchase", "auction_proposition", "auction_cleanup",
    "bot_packets", "bot_cache_cleanup", "bot_nearby", "bot_randomize", "bot_strategy",
    "teleport_plan", "teleport_faction", "teleport_shuffle", "teleport_active",
    "teleport_area", "teleport_commit", "bot_refresh", "bot_event_read",
    "random_path", "spline_launch", "packet_compression", "movement_delivery"};
static_assert(sizeof(names) / sizeof(*names) == Count, "work diagnostic names");
struct Sample
{
    TurtleDiagnostics::Metric metric;
    uint32_t slowestGuid = 0;
};
struct Summary
{
    std::array<Sample, Count> samples{};
    uint64_t report = TurtleDiagnostics::Micros();
};
inline thread_local std::map<std::pair<uint32_t, uint32_t>, Summary> summaries;
class Scope
{
public:
    Scope(Kind kind, uint32_t guid = 0) : kind(kind), guid(guid)
    {
        if (!TurtleDiagnostics::current) return;
        context = TurtleDiagnostics::context;
        summary = &summaries[{context.map, context.instance}];
        start = TurtleDiagnostics::Micros();
    }
    ~Scope() { Finish(); }
    void Finish()
    {
        if (!summary) return;
        uint64_t const now = TurtleDiagnostics::Micros();
        auto& sample = summary->samples[kind];
        if (now - start >= sample.metric.peak) sample.slowestGuid = guid;
        sample.metric.Add(now - start);
        if (TurtleDiagnostics::sink && now - summary->report >=
            uint64_t(TurtleDiagnostics::intervalMs.load(std::memory_order_relaxed)) * 1000)
        {
            char line[512];
            for (unsigned n = 0; n < Count; ++n)
            {
                auto& s = summary->samples[n];
                auto& m = s.metric;
                if (!m.count) continue;
                std::snprintf(line, sizeof(line),
                    "TW_WORK map=%u inst=%u tick=%llu phase=%s window_us=%llu n=%llu total_us=%llu mean_us=%llu p95_upper_us=%llu max_us=%llu slowest_guid=%u",
                    context.map, context.instance, (unsigned long long)context.tick, names[n],
                    (unsigned long long)(now - summary->report), (unsigned long long)m.count,
                    (unsigned long long)m.total, (unsigned long long)(m.total / m.count),
                    (unsigned long long)m.Percentile(95), (unsigned long long)m.peak, s.slowestGuid);
                TurtleDiagnostics::sink(line);
                s = {};
            }
            summary->report = now;
        }
        summary = nullptr;
    }
    Scope(Scope const&) = delete;
    Scope& operator=(Scope const&) = delete;
private:
    Summary* summary = nullptr;
    TurtleDiagnostics::Context context;
    Kind kind;
    uint32_t guid;
    uint64_t start = 0;
};
}
