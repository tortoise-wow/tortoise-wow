#include "TickDiffWindow.h"
#include <iostream>
#include <limits>
#include <stdexcept>
static void Check(bool ok) { if (!ok) throw std::runtime_error("tick telemetry assertion failed"); }
int main()
{
    TickDiffWindow<50> ticks;
    Check(ticks.Current() == 0 && ticks.Maximum() == 0 && ticks.Average() == 0);
    ticks.Record(250);
    Check(ticks.Current() == 250 && ticks.Maximum() == 250 && ticks.Average() == 5);
    for (unsigned n = 0; n < 49; ++n) ticks.Record(50);
    Check(ticks.Current() == 50 && ticks.Maximum() == 250 && ticks.Average() == 54);
    ticks.Record(50); // oldest spike expires, rather than staying forever
    Check(ticks.Maximum() == 50 && ticks.Average() == 50);
    for (unsigned n = 0; n < 50; ++n) ticks.Record(std::numeric_limits<uint32_t>::max());
    Check(ticks.Average() == UINT32_MAX && ticks.Maximum() == UINT32_MAX);
    for (unsigned n = 0; n < 50; ++n) ticks.Record(0);
    Check(ticks.Current() == 0 && ticks.Maximum() == 0 && ticks.Average() == 0);
    uint32_t reference[50]{};
    for (unsigned n = 0; n < 20000; ++n)
    {
        uint32_t value = (n * 7919u) % 10001;
        reference[n % 50] = value;
        ticks.Record(value);
        uint64_t sum = 0;
        uint32_t peak = 0;
        for (auto sample : reference) { sum += sample; peak = std::max(peak, sample); }
        Check(ticks.Current() == value && ticks.Average() == sum / 50 && ticks.Maximum() == peak);
    }
    std::cout << "PASS: measured current/average/max, zero-padding, peak expiration, overflow and ring wrap\n";
}
