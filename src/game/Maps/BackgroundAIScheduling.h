#pragma once
#include <algorithm>
#include <cstdint>

// CMaNGOS load-derived cadence: independent full turns remain available even
// on maps with no humans. Minimal updates between them are not full decisions.
namespace BackgroundAI
{
inline uint32_t FullCadence(uint32_t averageMs, bool hasHumans)
{
    uint64_t const scaled = uint64_t(averageMs) * (hasHumans ? 1 : 3);
    return uint32_t(std::max<uint64_t>(1, (scaled + 9) / 10));
}

inline uint32_t NextMinimalDelay(uint32_t current, uint32_t passive, uint32_t react,
    uint32_t guid, uint32_t jitterMs)
{
    uint32_t const jitter = jitterMs ? ((guid * 2654435761u) % (uint64_t(jitterMs) + 1)) : 0;
    uint64_t const delay = std::max<uint64_t>(passive, uint64_t(react) * 10) + jitter;
    return std::max(current, uint32_t(std::min<uint64_t>(UINT32_MAX, delay)));
}
}
