#pragma once

#include <cstdint>
#include <unordered_map>

// Build once from the area storage, before map workers start. Preserve vanilla
// duplicate-flag semantics: first exact-map match, last cross-map fallback.
class AreaLookupIndex
{
public:
    void Add(uint32_t id, uint32_t flag, uint32_t map, bool zone)
    {
        if (flag)
        {
            exact.emplace((uint64_t(map) << 32) | flag, id);
            fallback[flag] = id;
        }
        if (zone && map != 0 && map != 1)
            mapFlags.emplace(map, flag);
    }
    uint32_t Find(uint32_t flag, uint32_t map) const
    {
        auto it = exact.find((uint64_t(map) << 32) | flag);
        if (it != exact.end()) return it->second;
        auto other = fallback.find(flag);
        return other == fallback.end() ? 0 : other->second;
    }
    uint32_t MapFlag(uint32_t map) const
    {
        auto it = mapFlags.find(map);
        return it == mapFlags.end() ? 0 : it->second;
    }
private:
    std::unordered_map<uint64_t, uint32_t> exact;
    std::unordered_map<uint32_t, uint32_t> fallback, mapFlags;
};
