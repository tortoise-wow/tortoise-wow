#include "BackgroundWorldScheduling.h"
#include <array>
#include <cstdint>
#include <iostream>
#include <stdexcept>

static void Check(bool condition) { if (!condition) throw std::runtime_error("background-world policy assertion failed"); }
int main()
{
    using namespace BackgroundWorld;
    Check(BackgroundStride(0, false, 20) == 1);
    Check(BackgroundStride(100, false, 20) == 1);
    Check(BackgroundStride(101, true, 100) == 11);
    Check(BackgroundStride(200, false, 100) == 60);
    Check(BackgroundStride(UINT32_MAX, false, 20) == 20);
    Check(BackgroundStride(1000, true, 0) == 1);
    CreatureState idle;
    idle.continent = true;
    idle.motion = Motion::Idle;
    std::array<unsigned, 10> buckets{};
    for (unsigned population : {8, 156, 186, 1000, 4000, 5217, 6000, 20000})
        for (unsigned guid = 1; guid <= population; ++guid)
        {
            uint32_t interval = CreatureInterval(idle, guid);
            Check(interval >= 500 && interval <= 1000);
            Check(CreatureInterval(idle, guid) == interval);
            Check(DecideUpdate(0, 40, interval).due); // first visit cannot starve
            Check(!DecideUpdate(interval - 1, 40, interval).due);
            Check(DecideUpdate(interval, 40, interval).due);
            Check(DecideUpdate(interval, 40, interval).logicalDiff == interval);
            ++buckets[guid % buckets.size()];
        }
    for (auto count : buckets) Check(count > 0);
    for (auto flag : {&CreatureState::foreground, &CreatureState::controlled,
         &CreatureState::combat, &CreatureState::events, &CreatureState::auras,
         &CreatureState::casting, &CreatureState::scripted, &CreatureState::active,
         &CreatureState::transport})
    {
        CreatureState protectedState = idle;
        protectedState.*flag = true;
        Check(CreatureInterval(protectedState, 123) == 0);
        auto const immediate = DecideUpdate(600, 40, CreatureInterval(protectedState, 123));
        Check(immediate.due && immediate.logicalDiff == 40);
    }
    auto instance = idle; instance.continent = false;
    auto corpse = idle; corpse.alive = false;
    auto escort = idle; escort.motion = Motion::Other;
    Check(!CreatureInterval(instance, 1) && !CreatureInterval(corpse, 1) && !CreatureInterval(escort, 1));
    auto roaming = idle; roaming.motion = Motion::Random;
    for (unsigned guid = 0; guid < 6000; ++guid)
        Check(CreatureInterval(roaming, guid) >= 250 && CreatureInterval(roaming, guid) <= 500);
    Check(DecideUpdate(3600000, 40, 500).logicalDiff == 1000);
    Check(DecideUpdate(3600000, 40, 0).logicalDiff == 40);

    // Simulate owner ticks, including uint32 clock wrap. A skipped update does
    // not reset the last-update clock; accumulated elapsed time reaches its due
    // threshold, and entering combat bypasses that threshold immediately.
    uint32_t last = UINT32_MAX - 250, now = last;
    unsigned updates = 0;
    uint64_t advanced = 0;
    for (unsigned tick = 0; tick < 100; ++tick)
    {
        now += 40;
        uint32_t elapsed = now - last;
        auto const decision = DecideUpdate(elapsed, 40, 500);
        if (decision.due) { ++updates; advanced += decision.logicalDiff; last = now; }
    }
    Check(updates == 7 && advanced == 3640);
    Check(DecideUpdate(now - last, 40, 0).due);
    std::cout << "PASS: shared background scheduling, protected gameplay, population independence, elapsed timers and wrap\n";
}
