#ifndef MANGOS_MAP_WORK_H
#define MANGOS_MAP_WORK_H
#include <cstdint>
#include <algorithm>

// No pointers survive a scheduler pass. A near teleport also invalidates work,
// even though the player comes back to the same map and instance.
struct MapWorkStamp
{
    uint32_t map, instance;
    uint64_t generation;
    bool Matches(uint32_t newMap, uint32_t newInstance, uint64_t newGeneration, bool inWorld) const
    {
        return inWorld && map == newMap && instance == newInstance && generation == newGeneration;
    }
};

inline bool IsStaggeredMapWorkDue(uint64_t pass, uint32_t guid, uint32_t stride)
{
    stride = std::max<uint32_t>(1, stride);
    return pass % stride == guid % stride;
}
#endif
