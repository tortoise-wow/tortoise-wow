#ifndef MANTECH_MEMORY_LEDGER_H
#define MANTECH_MEMORY_LEDGER_H
#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>
namespace ManTech {
enum class MemoryKind : std::size_t { Units, AuraBuckets, UpdateFields, EventHolders, NavTiles, NavShared, Network, OptionalState, PathScratch, Terrain, DatabaseWork, Collision, NavQueries, AuraIndexes, Count };
struct MemorySample { std::uint64_t count, bytes, peak; };
class MemoryLedger {
    struct Counter { std::atomic<std::uint64_t> count{0}, bytes{0}, peak{0}; };
    inline static std::array<Counter, static_cast<std::size_t>(MemoryKind::Count)> counters{};
public:
    static void Add(MemoryKind kind, std::uint64_t bytes, std::uint64_t count=1) {
        auto& c=counters[static_cast<std::size_t>(kind)]; c.count.fetch_add(count,std::memory_order_relaxed);
        auto value=c.bytes.fetch_add(bytes,std::memory_order_relaxed)+bytes;
        auto peak=c.peak.load(std::memory_order_relaxed);
        while(value>peak && !c.peak.compare_exchange_weak(peak,value,std::memory_order_relaxed)) {}
    }
    static void Remove(MemoryKind kind, std::uint64_t bytes, std::uint64_t count=1) {
        auto& c=counters[static_cast<std::size_t>(kind)]; c.count.fetch_sub(count,std::memory_order_relaxed); c.bytes.fetch_sub(bytes,std::memory_order_relaxed);
    }
    static MemorySample Read(MemoryKind kind) {
        auto& c=counters[static_cast<std::size_t>(kind)];
        return {c.count.load(std::memory_order_relaxed),c.bytes.load(std::memory_order_relaxed),c.peak.load(std::memory_order_relaxed)};
    }
};
}
#endif
