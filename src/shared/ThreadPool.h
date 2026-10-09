/*
 * Copyright (C) 2017 Elysium Project <https://github.com/elysium-project>
 * Distributed under the GNU General Public License, version 2 or later.
 */
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#ifdef WIN32
#undef ERROR
#undef IGNORE
#endif
// Publish immutable work, join ALL workers, then permit reuse. Captured game
// objects must remain alive until the returned batch future has completed.
class ThreadPool
{
public:
    struct SingleQueue { static constexpr bool mysql = false, multi = false; };
    struct MultiQueue { static constexpr bool mysql = false, multi = true; };
    template<class T = SingleQueue>
    struct MySQL { static constexpr bool mysql = true, multi = T::multi; };
    using Callable = std::function<void()>;
    using workload_t = std::vector<Callable>;
    enum class Status { ERROR = -1, STOPPED, STARTING, READY, PROCESSING, TERMINATING };
    enum class ClearMode { NEVER, UPPON_COMPLETION, AT_NEXT_WORKLOAD };
    enum class ErrorHandling { NONE, IGNORE, LOG, TERMINATE };
    ThreadPool(int numThreads, std::string name,
        ClearMode when = ClearMode::AT_NEXT_WORKLOAD, ErrorHandling mode = ErrorHandling::NONE);
    ~ThreadPool();
    ThreadPool(ThreadPool const&) = delete;
    ThreadPool& operator=(ThreadPool const&) = delete;
    template<class T = SingleQueue> void start() { StartWorkers(T::mysql, T::multi); }
    std::future<void> processWorkload(Callable pre = {}, Callable post = {});
    std::future<void> processWorkload(workload_t& workload, Callable pre = {}, Callable post = {});
    std::future<void> processWorkload(workload_t&& workload, Callable pre = {}, Callable post = {});
    Status status() const { return m_status.load(std::memory_order_acquire); }
    size_t size() const { return m_size; }
    std::vector<std::exception_ptr> taskErrors() const;
    ThreadPool& operator<<(Callable function);
    void clearWorkload();
private:
    void StartWorkers(bool mysql, bool multi);
    void RunWorker(size_t id, bool mysql, bool multi);
    void Execute(Callable const& function);
    std::future<void> Publish(Callable pre, Callable post);
    void RequireIdle() const;
    std::string const m_name;
    size_t const m_size;
    ClearMode const m_clearMode;
    ErrorHandling const m_errorHandling;
    std::atomic<Status> m_status{Status::STOPPED};
    mutable std::mutex m_mutex;
    std::condition_variable m_ready, m_idle;
    workload_t m_workload;
    std::vector<std::thread> m_workers;
    std::vector<std::exception_ptr> m_errors;
    std::promise<void> m_result;
    Callable m_pre, m_post;
    std::atomic<size_t> m_index{0};
    std::atomic<bool> m_failed{false};
    size_t m_generation = 0, m_active = 0, m_participants = 0;
    bool m_dirty = false, m_stopping = false;
};
template<typename T>
std::unique_ptr<ThreadPool>& operator<<(std::unique_ptr<ThreadPool>& pool, T&& function)
{
    (*pool) << std::forward<T>(function);
    return pool;
}
#endif
