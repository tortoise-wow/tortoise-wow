#include "ThreadPool.h"
#include "MapWork.h"
#include "ExecutionWatch.h"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <set>
using namespace std::chrono_literals;
static void Require(bool value, char const* what)
{
    if (!value) throw std::runtime_error(what);
}
static void Join(std::future<void>& future)
{
    Require(future.valid(), "missing completion");
    Require(future.wait_for(5s) == std::future_status::ready, "batch hung");
    future.get();
}
template<class Queue> void ChangingPopulations()
{
    ThreadPool pool(4, "regression");
    pool.start<Queue>();
    for (size_t count : {1, 8, 40, 156, 186, 1000, 4000, 5217, 6000, 3000, 6000, 20000, 0, 1})
    {
        std::cout << "batch " << count << std::endl;
        std::vector<std::atomic<unsigned>> seen(count);
        for (auto& value : seen) value = 0;
        ThreadPool::workload_t work;
        for (size_t i = 0; i < count; ++i)
            work.emplace_back([&, i] { ++seen[i]; });
        auto done = pool.processWorkload(work);
        if (count) Join(done);
        else Require(!done.valid(), "empty work should not wait");
        for (auto& value : seen)
            Require(value == 1, "work lost or duplicated after resizing");
    }
    // Many short batches expose completion/publication and wake-up races.
    std::atomic<unsigned> total{0};
    for (unsigned i = 0; i < 4000; ++i)
    {
        pool << [&] { ++total; };
        auto done = pool.processWorkload();
        Join(done);
    }
    Require(total == 4000, "lost wake-up or replay");
}
static void FailureAndReuse()
{
    ThreadPool pool(2, "failure");
    pool.start();
    pool << [] { throw std::runtime_error("injected task failure"); };
    auto bad = pool.processWorkload();
    Require(bad.wait_for(5s) == std::future_status::ready, "exception stranded completion");
    bool threw = false;
    try { bad.get(); } catch (std::runtime_error const&) { threw = true; }
    Require(threw, "task exception swallowed");
    std::atomic<unsigned> calls{0};
    pool << [&] { ++calls; };
    auto good = pool.processWorkload();
    Join(good);
    Require(calls == 1, "failed batch poisoned next batch");

    std::promise<void> release;
    auto gate = release.get_future().share();
    pool << [gate] { gate.wait(); };
    auto busy = pool.processWorkload();
    bool rejected = false;
    try { pool << [] {}; } catch (std::logic_error const&) { rejected = true; }
    release.set_value();
    Join(busy);
    Require(rejected, "active captures overwritten");
}
static void MapPhases()
{
    ThreadPool pool(2, "map-owners");
    pool.start();
    std::atomic<unsigned> simulated{0}, completed{0}, entered{0};
    // More maps than workers must work: jobs must not wait on sibling maps.
    ThreadPool::workload_t maps;
    for (unsigned i = 0; i < 64; ++i) maps.emplace_back([&] { ++simulated; });
    auto phase = pool.processWorkload(maps);
    Join(phase);
    maps.clear();
    for (unsigned i = 0; i < 64; ++i)
        maps.emplace_back([&] { Require(simulated == 64, "cleanup before maps joined"); ++completed; });
    phase = pool.processWorkload(maps);
    Join(phase);
    Require(completed == 64, "missing map completion");
    ++entered; // transfer/spawn application after both joins
    Require(entered == 1, "transfer repeated");
    MapWorkStamp stamp{0, 3, 17};
    Require(stamp.Matches(0, 3, 17, true), "valid job rejected");
    Require(!stamp.Matches(1, 3, 17, true), "cross-continent stale work");
    Require(!stamp.Matches(0, 4, 17, true), "instance stale work");
    Require(!stamp.Matches(0, 3, 18, true), "near-teleport stale work");
    Require(!stamp.Matches(0, 3, 17, false), "logged-out stale work");
    for (uint32_t guid = 1; guid < 20000; ++guid)
    {
        unsigned updates = 0;
        for (uint64_t pass = 0; pass < 7; ++pass)
            updates += IsStaggeredMapWorkDue(pass, guid, 7);
        Require(updates == 1, "population-independent cadence");
    }
}
static void ClearModesAndLifetime()
{
    for (auto mode : {ThreadPool::ClearMode::NEVER, ThreadPool::ClearMode::UPPON_COMPLETION,
        ThreadPool::ClearMode::AT_NEXT_WORKLOAD})
    {
        ThreadPool pool(3, "clear-modes", mode);
        pool.start();
        unsigned count = 0;
        pool << [&] { ++count; };
        auto first = pool.processWorkload();
        Join(first);
        auto replay = pool.processWorkload();
        if (mode == ThreadPool::ClearMode::NEVER)
        {
            Join(replay);
            Require(count == 2, "retained workload not replayed");
        }
        else
            Require(!replay.valid() && count == 1, "completed work replayed");
    }
    std::atomic<unsigned> finished{0};
    {
        ThreadPool pool(2, "destructor");
        pool.start();
        for (unsigned i = 0; i < 100; ++i)
            pool << [&] { ++finished; };
        auto completion = pool.processWorkload();
        // No get/wait here: destructor must still drain and join owned work.
    }
    Require(finished == 100, "worker lifetime escaped pool");
}
int main()
{
    try
    {
        ChangingPopulations<ThreadPool::SingleQueue>();
        std::cout << "single queue passed" << std::endl;
        ChangingPopulations<ThreadPool::MultiQueue>();
        std::cout << "multi queue passed" << std::endl;
        FailureAndReuse();
        MapPhases();
        ClearModesAndLifetime();
        ExecutionWatch::Set(ExecutionWatch::BotAI, 1, 4, 123);
        Require(ExecutionWatch::Current()->guid.load() == 123, "diagnostic GUID lost");
        Require(ExecutionWatch::Current()->phase.load() == ExecutionWatch::BotAI, "diagnostic phase lost");
        ExecutionWatch::Set(ExecutionWatch::Idle);
        std::cout << "PASS: real worker pool resizing, repeated batches, failures, ownership phases, transition stamps\n";
    }
    catch (std::exception const& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
