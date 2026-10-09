#include "MapTaskExecutor.h"
#include "ArchitectureDiagnostics.h"
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

static void Check(bool value) { if (!value) throw std::runtime_error("architecture assertion failed"); }
static std::vector<std::string> diagnosticLines;
int main()
{
    for (size_t workers : {0, 1, 2, 4})
    {
        MapTaskExecutor pool(workers);
        std::vector<std::thread> owners;
        std::atomic<unsigned> complete{0};
        for (unsigned map = 0; map < 16; ++map)
            owners.emplace_back([&] {
                for (size_t population : {0, 8, 156, 186, 1000, 4000, 5217, 6000, 3000, 20000})
                {
                    std::vector<unsigned> found(population, 0);
                    MapTaskJoin group;
                    for (size_t chunk = 0; chunk < 8; ++chunk)
                        group.tasks.push_back(pool.Submit([&, chunk] {
                            for (size_t n = chunk; n < population; n += 8) ++found[n];
                        }));
                    group.Get();
                    for (unsigned count : found) Check(count == 1);
                }
                ++complete;
            });
        for (auto& owner : owners) owner.join();
        Check(complete == 16);
        std::atomic<bool> drained{false};
        bool caught = false;
        try
        {
            MapTaskJoin group;
            group.tasks.push_back(pool.Submit([] { throw std::runtime_error("expected"); }));
            group.tasks.push_back(pool.Submit([&] { drained = true; }));
            group.Get();
        }
        catch (std::runtime_error const&) { caught = true; }
        Check(caught && drained);
        pool.Submit([] {}).get(); // reusable after failure
    }
    std::atomic<unsigned> finished{0};
    { MapTaskExecutor pool(2); for (unsigned n = 0; n < 1000; ++n) pool.Submit([&] { ++finished; }); }
    Check(finished == 1000); // pool destruction drains accepted jobs

    std::atomic<unsigned> entered{0}, left{0};
    {
        MapTaskExecutor pool(2, [&] { ++entered; }, [&] { ++left; });
        Check(entered == 2 && left == 0);
        TurtleDiagnostics::Summary transferred;
        for (unsigned tick = 0; tick < 100; ++tick)
        {
            auto work = pool.Submit([&, tick] {
                Check(TurtleDiagnostics::current == nullptr);
                {
                    TurtleDiagnostics::OwnerHandoff ownership(&transferred, {42, 7, tick});
                    Check(TurtleDiagnostics::context.map == 42 && TurtleDiagnostics::context.tick == tick);
                    ++TurtleDiagnostics::current->metrics[TurtleDiagnostics::CellCount].count;
                }
                Check(TurtleDiagnostics::current == nullptr);
            });
            work.get(); // owner must join before accessing transferred state
            Check(transferred.metrics[TurtleDiagnostics::CellCount].count == tick + 1);
        }
    }
    Check(left == 2);
    bool initFailed = false;
    std::atomic<unsigned> starts{0}, exits{0};
    try
    {
        MapTaskExecutor failed(3, [&] {
            if (++starts == 2) throw std::runtime_error("worker initialization failure");
        }, [&] { ++exits; });
    }
    catch (std::runtime_error const&) { initFailed = true; }
    Check(initFailed && starts == 2 && exits == 1);

    TurtleDiagnostics::Metric m;
    Check(m.Percentile(95) == 0);
    for (unsigned n = 1; n <= 100; ++n) m.Add(n);
    Check(m.count == 100 && m.total == 5050 && m.peak == 100);
    Check(m.Percentile(95) >= 95 && m.Percentile(99) >= 99);
    TurtleDiagnostics::Metric overflow;
    overflow.Add(uint64_t(1) << 40);
    Check(overflow.Percentile(99) == (uint64_t(1) << 40));
    TurtleDiagnostics::Summary a, b;
    TurtleDiagnostics::enabled = true;
    {
        TurtleDiagnostics::Frame outer(a, 1, 0, 1);
        Check(TurtleDiagnostics::context.map == 1 && TurtleDiagnostics::context.tick == 1);
        TurtleDiagnostics::Record(TurtleDiagnostics::CellCount, 7);
        { TurtleDiagnostics::Frame inner(b, 0, 0, 1); TurtleDiagnostics::Record(TurtleDiagnostics::CellCount, 9); }
        Check(TurtleDiagnostics::context.map == 1);
        TurtleDiagnostics::Record(TurtleDiagnostics::CellCount, 3);
    }
    Check(a.metrics[TurtleDiagnostics::CellCount].total == 10);
    Check(b.metrics[TurtleDiagnostics::CellCount].total == 9);
    Check(TurtleDiagnostics::current == nullptr);
    Check(TurtleDiagnostics::context.tick == 0);

    // Detailed attribution must remain per-map and scoped to the exact unit.
    TurtleDiagnostics::Summary creatureSummary;
    int creature = 0, other = 0;
    {
        TurtleDiagnostics::Frame frame(creatureSummary, 0, 7, 123);
        {
            TurtleDiagnostics::CreatureProbe probe(&creature, 42, 80856, 1, true, 300);
            auto* outerProbe = TurtleDiagnostics::activeCreature;
            TurtleDiagnostics::CreatureProbe::Stage(&other, TurtleDiagnostics::UnitSpells);
            {
                TurtleDiagnostics::CreatureProbe nested(&other, 43, 1, 0, false, 50);
                Check(TurtleDiagnostics::activeCreature != outerProbe);
                TurtleDiagnostics::CreatureProbe::Stage(&other, TurtleDiagnostics::UnitEvents);
            }
            Check(TurtleDiagnostics::activeCreature == outerProbe);
            TurtleDiagnostics::CreatureProbe::Stage(&creature, TurtleDiagnostics::UnitVisibility);
        }
        Check(TurtleDiagnostics::activeCreature == nullptr);
    }
    Check(creatureSummary.creatureSamples == 2);
    auto const measured = creatureSummary.slowestCreature;
    Check((measured.guid == 42 || measured.guid == 43) && measured.tick == 123);
    Check(measured.phases[TurtleDiagnostics::UnitSpells] == 0); // unrelated unit cannot change phase
    Check(std::accumulate(measured.phases.begin(), measured.phases.end(), uint64_t(0)) == measured.elapsed);
    for (auto const& phase : creatureSummary.creaturePhases) Check(phase.count == 2);

    // Independent worker threads must not attach to another map's active probe.
    TurtleDiagnostics::Summary threadSummary;
    std::thread isolated([&] {
        Check(TurtleDiagnostics::activeCreature == nullptr);
        TurtleDiagnostics::Frame frame(threadSummary, 1, 0, 456);
        TurtleDiagnostics::CreatureProbe probe(&other, 99, 100, 0, false, 40);
        TurtleDiagnostics::CreatureProbe::Stage(&other, TurtleDiagnostics::UnitMotion);
    });
    isolated.join();
    Check(threadSummary.creatureSamples == 1 && threadSummary.slowestCreature.guid == 99);
    Check(creatureSummary.creatureSamples == 2 && TurtleDiagnostics::activeCreature == nullptr);

    // Deterministic worst-sample selection, independent of microsecond clock resolution.
    TurtleDiagnostics::CreatureSample known;
    known.guid = 42; known.entry = 80856; known.tick = 123;
    known.combat = true; known.state = 1; known.updateDiff = 300;
    known.elapsed = measured.elapsed + 100;
    known.phases[TurtleDiagnostics::UnitSpells] = known.elapsed;
    TurtleDiagnostics::ObserveCreature(creatureSummary, known);
    Check(creatureSummary.slowestCreature.guid == 42 && creatureSummary.slowestCreature.combat);
    Check(creatureSummary.slowestCreature.updateDiff == 300 && creatureSummary.slowestCreature.state == 1);

    TurtleDiagnostics::sink = [](char const* line) { diagnosticLines.emplace_back(line); };
    creatureSummary.lastReport = 0; // flush without sleeping or changing gameplay clocks
    { TurtleDiagnostics::Frame report(creatureSummary, 0, 7, 124); }
    Check(diagnosticLines.size() == TurtleDiagnostics::CreaturePhaseCount + 1);
    Check(diagnosticLines.back().find("TW_CREATURE_SLOWEST map=0 inst=7 tick=123 guid=42 entry=80856") == 0);
    Check(diagnosticLines.back().find("regen_us=") != std::string::npos); // bounded line was not truncated
    Check(creatureSummary.creatureSamples == 0 && creatureSummary.slowestCreature.guid == 0);
    for (auto const& phase : creatureSummary.creaturePhases) Check(phase.count == 0);
    TurtleDiagnostics::sink = nullptr;
    TurtleDiagnostics::enabled = false;
    {
        TurtleDiagnostics::Frame off(a, 0, 0, 1);
        TurtleDiagnostics::Record(TurtleDiagnostics::CellCount, 100);
        TurtleDiagnostics::CreatureProbe probe(&creature, 1, 1, 0, false, 10);
        TurtleDiagnostics::CreatureProbe::Stage(&creature, TurtleDiagnostics::UnitSpells);
        Check(TurtleDiagnostics::activeCreature == nullptr);
    }
    Check(a.metrics[TurtleDiagnostics::CellCount].total == 10);
    Check(a.creatureSamples == 0);
    // Report probe-only cost; never assert a timing threshold on a shared host.
    // This is not a game simulation or a promise of live-server overhead.
    auto probeCost = [&](bool enabled) {
        TurtleDiagnostics::enabled = enabled;
        TurtleDiagnostics::Summary benchmark;
        auto const begin = TurtleDiagnostics::Micros();
        {
            TurtleDiagnostics::Frame frame(benchmark, 0, 0, 1);
            for (unsigned n = 0; n < 50000; ++n)
            {
                TurtleDiagnostics::CreatureProbe probe(&creature, n, 1, 0, false, 50);
                for (unsigned phase = 0; phase < TurtleDiagnostics::CreaturePhaseCount; ++phase)
                    TurtleDiagnostics::CreatureProbe::Stage(&creature, TurtleDiagnostics::CreaturePhase(phase));
            }
        }
        auto const elapsed = TurtleDiagnostics::Micros() - begin;
        Check(benchmark.creatureSamples == (enabled ? 50000 : 0));
        return elapsed;
    };
    auto const disabledCost = probeCost(false);
    auto const enabledCost = probeCost(true);
    TurtleDiagnostics::enabled = false;
    std::cout << "Probe-only microbenchmark: disabled_us=" << disabledCost
        << " enabled_us=" << enabledCost << " samples=50000 phases=17\n";
    std::cout << "Map discovery concurrency, population changes, draining, exception and diagnostic tests passed\n";
}
