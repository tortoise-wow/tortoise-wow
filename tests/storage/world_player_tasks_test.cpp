#include "../../src/game/WorldPlayerTasks.h"
#include <atomic>
#include <cstdlib>
#include <iostream>
#include <thread>

void AddTaskFromAnotherTranslationUnit(int* completed);

static unsigned checks = 0;
static void Check(bool condition, const char* message)
{
    ++checks;
    if (!condition)
    {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

int main()
{
    int crossUnit = 0;
    AddTaskFromAnotherTranslationUnit(&crossUnit);
    Check(crossUnit == 0, "producer cannot execute player mutation inline");
    WorldPlayerTasks::RunPending();
    Check(crossUnit == 1, "producer and consumer share one queue across translation units");

    WorldPlayerTaskQueue queue;
    const auto worldThread = std::this_thread::get_id();
    constexpr int producers = 4;
    constexpr int perProducer = 100;
    int next[producers] = {};
    std::vector<std::thread> threads;
    for (int producer = 0; producer < producers; ++producer)
        threads.emplace_back([&, producer]() {
            for (int sequence = 0; sequence < perProducer; ++sequence)
                queue.Add([&, producer, sequence]() {
                    Check(std::this_thread::get_id() == worldThread, "mutations run on the world consumer");
                    Check(next[producer]++ == sequence, "producer FIFO order is preserved");
                });
        });
    for (auto& thread : threads)
        thread.join();
    for (int value : next)
        Check(value == 0, "background producers do not mutate actors");
    queue.RunPending();
    for (int value : next)
        Check(value == perProducer, "all concurrent producer deliveries ran once");

    int phase = 0;
    queue.Add([&]() {
        ++phase;
        queue.Add([&]() { ++phase; });
        bool rejected = false;
        try { queue.RunPending(); }
        catch (const std::logic_error&) { rejected = true; }
        Check(rejected, "reentrant drain cannot execute player tasks during another task");
    });
    queue.RunPending();
    Check(phase == 1, "a newly queued task waits for the next world tick");
    queue.RunPending();
    Check(phase == 2, "next-tick delivery is retained");
    queue.RunPending();
    Check(phase == 2, "a drained task cannot run twice");

    // Model the exact unsafe interleaving: background shop deliveries arrive
    // while the map actor owns a captured stack and awaits its SQL COMMIT.
    int stack = 10;
    bool dirty = true;
    const int committedStack = stack;
    std::thread shop([&]() {
        queue.Add([&]() { ++stack; dirty = true; });
        queue.Add([&]() { ++stack; dirty = true; });
    });
    shop.join();
    Check(stack == committedStack, "deliveries cannot merge into an ACK-waiting captured stack");
    dirty = false; // The map actor consumes only its acknowledged snapshot.
    queue.RunPending(); // The next world phase runs after that map update.
    Check(stack == 12 && dirty, "both later merges remain dirty for the next save");

    std::atomic<bool> inside{false};
    std::atomic<bool> release{false};
    queue.Add([&]() { inside = true; while (!release) std::this_thread::yield(); });
    std::thread consumer([&]() { queue.RunPending(); });
    while (!inside) std::this_thread::yield();
    bool rejected = false;
    try { queue.RunPending(); }
    catch (const std::logic_error&) { rejected = true; }
    release = true;
    consumer.join();
    Check(rejected, "two consumers cannot mutate player objects concurrently");
    std::cout << "PASS " << checks << " production world-player task queue checks\n";
}
