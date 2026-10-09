#include "Util/DevDiagnostics.h"
/*
 * Copyright (C) 2017 Elysium Project <https://github.com/elysium-project>
 * Distributed under the GNU General Public License, version 2 or later.
 */
#include "ThreadPool.h"
#include <algorithm>
#include <stdexcept>
#ifndef THREADPOOL_STANDALONE_TEST
#include "Log.h"
#include <mysql.h>
#endif

ThreadPool::ThreadPool(int threads, std::string name, ClearMode clear, ErrorHandling errors)
    : m_name(std::move(name)), m_size(std::max(0, threads)), m_clearMode(clear), m_errorHandling(errors) {}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_idle.wait(lock, [this] { return m_active == 0; });
        m_stopping = true;
        m_status.store(Status::TERMINATING, std::memory_order_release);
    }
    m_ready.notify_all();
    for (auto& thread : m_workers)
        thread.join();
}
void ThreadPool::StartWorkers(bool mysql, bool multi)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (status() != Status::STOPPED || !m_size)
        return;
    m_status = Status::STARTING;
    try
    {
        for (size_t i = 0; i < m_size; ++i)
            m_workers.emplace_back([this, i, mysql, multi] { RunWorker(i, mysql, multi); });
        m_status = Status::READY;
    }
    catch (...)
    {
        m_stopping = true;
        m_ready.notify_all();
        throw;
    }
}
void ThreadPool::RequireIdle() const
{
    if (m_active || m_stopping)
        throw std::logic_error("ThreadPool: attempted to replace an active batch");
}
std::future<void> ThreadPool::Publish(Callable pre, Callable post)
{
    RequireIdle(); // caller holds m_mutex
    if (status() != Status::READY || m_workload.empty())
        return {};
    m_result = std::promise<void>();
    auto result = m_result.get_future();
    m_pre = std::move(pre);
    m_post = std::move(post);
    m_errors.clear();
    m_failed = false;
    m_index = 0;
    m_participants = m_active = std::min(m_size, m_workload.size());
    m_dirty = true;
    ++m_generation;
    m_status = Status::PROCESSING;
    m_ready.notify_all();
    return result;
}
std::future<void> ThreadPool::processWorkload(Callable pre, Callable post)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    RequireIdle();
    if (m_clearMode == ClearMode::AT_NEXT_WORKLOAD && m_dirty)
    {
        m_workload.clear();
        m_dirty = false;
    }
    return Publish(std::move(pre), std::move(post));
}
std::future<void> ThreadPool::processWorkload(workload_t& work, Callable pre, Callable post)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    RequireIdle();
    m_workload = work;
    return Publish(std::move(pre), std::move(post));
}
std::future<void> ThreadPool::processWorkload(workload_t&& work, Callable pre, Callable post)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    RequireIdle();
    m_workload = std::move(work);
    return Publish(std::move(pre), std::move(post));
}
ThreadPool& ThreadPool::operator<<(Callable function)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    RequireIdle();
    if (m_clearMode == ClearMode::AT_NEXT_WORKLOAD && m_dirty)
        m_workload.clear();
    m_dirty = false;
    m_workload.emplace_back(std::move(function));
    return *this;
}
void ThreadPool::clearWorkload()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    RequireIdle();
    m_workload.clear();
    m_dirty = false;
}
std::vector<std::exception_ptr> ThreadPool::taskErrors() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_errors;
}
void ThreadPool::Execute(Callable const& function)
{
    if (!function)
        return;
    try { MANTECH_DIAG_SCOPE(JobExecute, 1, m_name.c_str()); function(); }
    catch (...)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_errors.push_back(std::current_exception());
        }
        if (m_errorHandling == ErrorHandling::NONE || m_errorHandling == ErrorHandling::TERMINATE)
            m_failed = true;
#ifndef THREADPOOL_STANDALONE_TEST
        if (m_errorHandling == ErrorHandling::LOG)
            sLog.outError("ThreadPool %s: task threw; batch completion remains guaranteed.", m_name.c_str());
#endif
    }
}
void ThreadPool::RunWorker(size_t id, bool mysql, bool multi)
{
#ifndef THREADPOOL_STANDALONE_TEST
    std::string name = "Pool " + m_name + " " + std::to_string(id);
    thread_name(name.c_str());
#endif
    size_t seen = 0;
    for (;;)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_ready.wait(lock, [&] { return m_stopping || m_generation != seen; });
            if (m_stopping)
                return;
            seen = m_generation;
            if (id >= m_participants)
                continue;
        }
#ifndef THREADPOOL_STANDALONE_TEST
        if (mysql)
            mysql_thread_init();
#endif
        Execute(m_pre);
        size_t index = multi ? id : m_index.fetch_add(1);
        while (index < m_workload.size() && !m_failed.load())
        {
            Execute(m_workload[index]);
            index = multi ? index + m_size : m_index.fetch_add(1);
        }
        Execute(m_post);
#ifndef THREADPOOL_STANDALONE_TEST
        if (mysql)
            mysql_thread_end();
#endif
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (--m_active == 0)
            {
                if (m_clearMode == ClearMode::UPPON_COMPLETION)
                    m_workload.clear();
                m_status.store(Status::READY, std::memory_order_release);
                if (m_failed && !m_errors.empty())
                    m_result.set_exception(m_errors.front());
                else
                    m_result.set_value();
                m_idle.notify_all();
            }
        }
    }
}
