#ifndef TURTLE_THORN_GORGE_DIAGNOSTICS_H
#define TURTLE_THORN_GORGE_DIAGNOSTICS_H
#include <algorithm>
#include <cstdint>
namespace ThornGorge
{
// Match-owner only; no clock reads, locks, allocations or file access when off.
struct DiagnosticBudget
{
    unsigned level=0, interval=5000, events=0, suppressed=0;
    unsigned manualCooldown=0;
    std::uint64_t sinceSnapshot=0, sinceWindow=0;
    void Configure(int requestedLevel, int requestedInterval)
    {
        *this=DiagnosticBudget{};
        level=unsigned(std::clamp(requestedLevel,0,2));
        interval=unsigned(std::clamp(requestedInterval,1000,60000));
    }
    bool Advance(unsigned diff)
    {
        if (!level) return false;
        manualCooldown=diff>=manualCooldown ? 0 : manualCooldown-diff;
        sinceWindow+=diff;
        if (sinceWindow>=1000) { sinceWindow%=1000; events=0; }
        sinceSnapshot+=diff;
        if (sinceSnapshot<interval) return false;
        sinceSnapshot=0; // one snapshot after a stall, never a catch-up burst
        return true;
    }
    bool Event(bool critical=false)
    {
        if (!level) return false;
        if (critical) return true;
        if (events<64) { ++events; return true; }
        ++suppressed; return false;
    }
    bool ManualSnapshot()
    {
        if (!level || manualCooldown) return false;
        manualCooldown=interval; return true;
    }
    unsigned TakeSuppressed() { unsigned value=suppressed; suppressed=0; return value; }
};
}
#endif
