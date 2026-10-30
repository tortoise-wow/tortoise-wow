#pragma once

#include <functional>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <vector>

// Producers own values/identities, never a Player pointer. The world thread drains
// one batch before map updates; tasks added during that batch wait for the next tick.
class WorldPlayerTaskQueue
{
    std::mutex m_pendingMutex;
    std::mutex m_consumerMutex;
    std::vector<std::function<void()>> m_pending;

public:
    void Add(std::function<void()> task)
    {
        if (!task)
            return;
        std::lock_guard<std::mutex> guard(m_pendingMutex);
        m_pending.push_back(std::move(task));
    }

    void RunPending()
    {
        std::unique_lock<std::mutex> consumer(m_consumerMutex, std::try_to_lock);
        if (!consumer.owns_lock())
            throw std::logic_error("Player task queue requires one world-thread consumer");
        std::vector<std::function<void()>> ready;
        {
            std::lock_guard<std::mutex> guard(m_pendingMutex);
            ready.swap(m_pending);
        }
        for (auto& task : ready)
            task();
    }
};

namespace WorldPlayerTasks
{
// External inline linkage is deliberate: producers and the world consumer in
// different translation units must share the same process-wide queue.
inline WorldPlayerTaskQueue& Queue()
{
    static WorldPlayerTaskQueue queue;
    return queue;
}

inline void Add(std::function<void()> task) { Queue().Add(std::move(task)); }
inline void RunPending() { Queue().RunPending(); }
}
