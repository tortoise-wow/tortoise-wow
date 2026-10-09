#pragma once
#include "Log.h"
#include <array>
#include <chrono>
#include <cstdint>
#include <thread>

// Thread-local aggregation: no cross-worker counter lock, SQL text, packet
// payloads or per-action logging. Times are elapsed time, not CPU utilization.
namespace WorkMetrics
{
enum Kind { DatabaseWait, DatabaseRead, Path, ValueCleanup, SocketQueue,
    ForegroundCells, QueueCells, BackgroundCells, MotionBatch, Count };
struct Metric { uint64_t count = 0, total = 0, peak = 0; };
inline thread_local std::array<Metric, Count> counters;
inline thread_local auto lastReport = std::chrono::steady_clock::now();

class Probe
{
public:
    explicit Probe(Kind kind) : m_kind(kind), m_start(std::chrono::steady_clock::now()) {}
    ~Probe() { Finish(); }
    void Finish()
    {
        if (m_finished) return;
        m_finished = true;
        uint64_t const us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_start).count();
        auto& metric = counters[m_kind];
        ++metric.count;
        metric.total += us;
        if (us > metric.peak) metric.peak = us;
    }
private:
    Kind m_kind;
    std::chrono::steady_clock::time_point m_start;
    bool m_finished = false;
};

inline void Flush()
{
    auto const now = std::chrono::steady_clock::now();
    if (now - lastReport < std::chrono::seconds(30)) return;
    lastReport = now;
    static char const* names[] = {"db_connection_wait", "db_read", "path", "value_cleanup", "socket_queue",
        "foreground_cells", "enqueue_cells", "background_cells", "motion_batch"};
    auto const tid = static_cast<unsigned long long>(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    for (size_t i = 0; i < Count; ++i)
    {
        auto& metric = counters[i];
        if (metric.count)
            Log::Instance().out(LOG_PERFORMANCE, "WORK_COST thread=%llu kind=%s count=%llu total_us=%llu max_us=%llu", tid, names[i],
                static_cast<unsigned long long>(metric.count), static_cast<unsigned long long>(metric.total),
                static_cast<unsigned long long>(metric.peak));
        metric = {};
    }
}
}
