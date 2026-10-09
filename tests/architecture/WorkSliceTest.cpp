#include "WorkSlice.h"
#include "ExecutionWatch.h"
#include "DetailedWorkDiagnostics.h"
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iostream>

static void Require(bool ok, char const* reason)
{
    if (!ok) throw std::runtime_error(reason);
}

int main()
{
    // A whole population of no-ops must still stop at the attempt bound.
    for (unsigned count : {8u, 800u, 1000u, 2000u, 6000u, 20000u})
    {
        std::vector<unsigned> visited(count);
        unsigned cursor = 0;
        for (unsigned remaining = count; remaining;)
        {
            WorkSlice slice(100, 64, 5000);
            while (remaining && slice.Take(100))
            {
                ++visited[cursor++];
                --remaining;
            }
            Require(slice.Attempts() <= 64, "no-op scan exceeded limit");
        }
        Require(std::all_of(visited.begin(), visited.end(), [](unsigned n) { return n == 1; }),
            "bounded passes starved or repeated a candidate");
    }
    WorkSlice timeBudget(100, 64, 5000);
    Require(timeBudget.Take(100), "first candidate rejected");
    Require(!timeBudget.Take(5100), "time limit ignored");
    WorkSlice slowFirst(100, 64, 5000);
    Require(slowFirst.Take(10000), "minimum progress lost");
    Require(!slowFirst.Take(10001), "slow operation allowed another operation");
    WorkSlice empty(0, 0, 5000);
    Require(!empty.Take(0), "zero count must do no work");

    ExecutionWatch::Set(ExecutionWatch::WorldStart, 1, 2, 3);
    auto* slot = ExecutionWatch::Current();
    auto const oldStart = slot->changed.load();
    {
        ExecutionWatch::Scope parent(ExecutionWatch::BotMaintenance);
        auto const parentStart = slot->changed.load();
        {
            ExecutionWatch::Scope child(ExecutionWatch::BotProcess, 0, 0, 42);
            Require(slot->guid.load() == 42, "active operation GUID missing");
        }
        Require(slot->phase.load() == ExecutionWatch::BotMaintenance && slot->changed.load() == parentStart,
            "child phase did not restore enclosing timing");
    }
    Require(slot->phase.load() == ExecutionWatch::WorldStart && slot->changed.load() == oldStart &&
        slot->map.load() == 1 && slot->instance.load() == 2 && slot->guid.load() == 3,
        "stale watchdog attribution after return");
    std::cout << "Work slices and nested watchdog attribution passed\n";
}
