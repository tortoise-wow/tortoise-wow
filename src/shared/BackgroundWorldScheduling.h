#pragma once

#include <algorithm>
#include <cstdint>

// ManTech CMaNGOS background-world policies, adapted without replacing Turtle's
// map ownership, world-clock spell timers, transports, or scripted updates.
// Pure decisions are shared with the standalone regression tests.
namespace BackgroundWorld
{
enum class Motion { Idle, Random, Other };
struct CreatureState
{
    bool continent = false;
    bool foreground = false;
    bool alive = true;
    bool controlled = false;
    bool combat = false;
    bool events = false;
    bool auras = false;
    bool casting = false;
    bool scripted = false;
    bool active = false;
    bool transport = false;
    Motion motion = Motion::Other;
};

inline uint32_t BackgroundStride(uint32_t averageMs, bool hasPlayers, uint32_t maxSkip)
{
    // CMaNGOS sheds distant active-object work only above 100 ms average:
    // approximately avg/10 passes with players, three times that without them.
    // Retain Turtle's configured cap and deterministic GUID-staggered cadence.
    if (averageMs <= 100) return 1;
    uint64_t stride = (uint64_t(averageMs) + 9) / 10;
    if (!hasPlayers) stride *= 3;
    return uint32_t(std::min<uint64_t>(std::max<uint32_t>(1, maxSkip), stride));
}

inline uint32_t CreatureInterval(CreatureState const& state, uint32_t guid)
{
    if (!state.continent || state.foreground || !state.alive || state.controlled ||
        state.combat || state.events || state.auras || state.casting || state.scripted ||
        state.active || state.transport)
        return 0;
    // A stable per-spawn interval spreads deadlines without global RNG state.
    uint32_t mixed = guid * 2654435761u;
    mixed ^= mixed >> 16;
    if (state.motion == Motion::Idle) return 500 + mixed % 501;
    if (state.motion == Motion::Random) return 250 + mixed % 251;
    return 0; // waypoint/escort/chase/flight and other movement stay immediate
}

struct UpdateDecision { bool due; uint32_t logicalDiff; };
inline UpdateDecision DecideUpdate(uint32_t elapsed, uint32_t mapDiff, uint32_t interval)
{
    if (!interval) return {true, mapDiff}; // exact pre-port path for protected objects
    if (elapsed && elapsed < interval) return {false, 0};
    // Spell/aura/regen clocks still receive the full real elapsed time separately.
    // Catch up idle/random movement and generic AI, but never replay hours of
    // movement when a previously unloaded cell is revisited.
    return {true, std::min<uint32_t>(1000, elapsed ? elapsed : mapDiff)};
}
}
