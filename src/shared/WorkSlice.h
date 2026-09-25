#pragma once
#include <cstdint>

// Cooperative budget: stop between operations, never interrupt game state
// changes halfway through. Count attempts, including no-ops and failures.
class WorkSlice
{
public:
    WorkSlice(uint64_t nowUs, uint32_t maxAttempts, uint64_t budgetUs)
        : start(nowUs), duration(budgetUs), limit(maxAttempts) {}
    bool Take(uint64_t nowUs)
    {
        if (used >= limit || (used && nowUs - start >= duration))
            return false;
        ++used;
        return true;
    }
    uint32_t Attempts() const { return used; }
private:
    uint64_t start, duration;
    uint32_t limit, used = 0;
};
