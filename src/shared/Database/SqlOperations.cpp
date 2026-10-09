/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "SqlOperations.h"
#include "ArchitectureDiagnostics.h"
#include "SqlDelayThread.h"
#include "DatabaseEnv.h"
#include "DatabaseImpl.h"
#include "Timer.h"
#include "ThreadPool.h"

#define LOCK_DB_CONN(conn) SqlConnection::Lock guard(conn)

/// ---- ASYNC STATEMENTS / TRANSACTIONS ----

bool SqlPlainRequest::Execute(SqlConnection *conn)
{
    /// just do it
    LOCK_DB_CONN(conn);
    return conn->Execute(m_sql);
}

bool SqlMultilineRequest::Execute(SqlConnection* conn)
{
    LOCK_DB_CONN(conn);
    return conn->ExecuteMultiline(m_sql.c_str());
}

SqlTransaction::~SqlTransaction()
{
    while(!m_queue.empty())
    {
        delete m_queue.back();
        m_queue.pop_back();
    }
}

bool SqlTransaction::Execute(SqlConnection *conn)
{
    if (m_queue.empty()) return true;
    LOCK_DB_CONN(conn);
    // Only explicitly replayable native saves opt in. Mixed-engine databases
    // retain failure behavior until their transactional migration is installed.
    unsigned const attempts = m_retryDeadlock && conn->CanReplayTransaction() ? 3 : 1;
    for (unsigned attempt = 0; attempt < attempts; ++attempt)
    {
        if (!conn->BeginTransaction()) return false;
        bool failed = false;
        bool deadlock = false;
        for (SqlOperation* statement : m_queue)
        {
            conn->SetStatementDeadlock(false);
            if (!statement->Execute(conn))
            {
                failed = true;
                deadlock = conn->LastStatementWasDeadlock();
                break;
            }
        }
        if (!failed)
        {
            // A lost COMMIT response is ambiguous; never replay it.
            if (conn->CommitTransaction()) return true;
            conn->RollbackTransaction();
            return false;
        }
        if (!conn->RollbackTransaction()) return false;
        if (!deadlock || attempt + 1 == attempts) return false;
        sLog.outError("DB_TRANSACTION_RETRY serial=%u attempt=%u statements=%zu reason=deadlock",
            GetSerialId(), attempt + 2, m_queue.size());
    }
    return false;
}

SqlPreparedRequest::SqlPreparedRequest(int nIndex, SqlStmtParameters * arg ) : m_nIndex(nIndex), m_param(arg)
{
}

SqlPreparedRequest::~SqlPreparedRequest()
{
    delete m_param;
}

bool SqlPreparedRequest::Execute( SqlConnection *conn )
{
    LOCK_DB_CONN(conn);
    return conn->ExecuteStmt(m_nIndex, *m_param);
}

/// ---- ASYNC QUERIES ----

bool SqlQuery::Execute(SqlConnection *conn)
{
    if(!m_callback || !m_queue)
        return false;

    LOCK_DB_CONN(conn);
    /// execute the query and store the result in the callback
    m_callback->SetResult(conn->Query(m_sql));
    /// add the callback to the sql result queue of the thread it originated from
    m_queue->Add(m_callback, m_highPriority);

    return true;
}

void SqlResultQueue::Update(uint32 timeout)
{
    uint32 const begin = WorldTimer::getMSTime();
    // CMaNGOS-style owner-thread completion. Async queries remain async; only
    // application of their results is serialized with world/map lifetime.
    // A bounded count also guarantees progress when the clock has low resolution.
    for (unsigned n = 0; n < 64; ++n)
    {
        if (n && timeout && WorldTimer::getMSTimeDiffToNow(begin) >= timeout)
            break;
        MaNGOS::IQueryCallback* callback = nullptr;
        bool found = false;
        // Prefer player logins, but do not starve background bot completions.
        if (++m_priorityBurst >= 8)
        {
            m_priorityBurst = 0;
            found = nextCallback(callback, false);
        }
        if (!found)
            found = nextCallback(callback, true) || nextCallback(callback, false);
        if (!found)
            break;
        std::unique_ptr<MaNGOS::IQueryCallback> owned(callback);
        uint32 const start = WorldTimer::getMSTime();
        {
            TurtleDiagnostics::Scope diagnosticCallback(TurtleDiagnostics::Callback);
            owned->Execute();
        }
        uint32 const elapsed = WorldTimer::getMSTimeDiffToNow(start);
        if (elapsed >= 100)
            sLog.out(LOG_PERFORMANCE, "DB_CALLBACK_SLOW elapsed_ms=%u high_priority=%u pending=%zu",
                elapsed, owned->IsHighPriority() ? 1 : 0, PendingCount());
    }
}

bool SqlResultQueue::nextCallback(MaNGOS::IQueryCallback*& callback, bool priority)
{
    // Legacy owner-only queues remain supported for shutdown/draining.
    if (priority ? _priorityThreadUnsafeWaitingQueries.next(callback) : _threadUnsafeWaitingQueries.next(callback))
    {
        if (numUnsafeQueries)
            --numUnsafeQueries;
        return true;
    }
    return priority ? _priorityWaitingQueries.next(callback) : next(callback);
}

SqlResultQueue::SqlResultQueue(const char* /*Name*/) : numUnsafeQueries(0) {}
SqlResultQueue::~SqlResultQueue(){}

void SqlResultQueue::Add(MaNGOS::IQueryCallback* callback, bool highPriority)
{
    callback->SetHighPriority(highPriority);
    if (highPriority)
        _priorityWaitingQueries.add(callback);
    else
        add(callback);
}

void SqlResultQueue::CancelAll()
{
    MaNGOS::IQueryCallback* cb;
    while (nextCallback(cb, true) || nextCallback(cb, false))
    {
        cb->SetResult(nullptr);
        cb->Execute();
        delete cb;
    }
}

bool SqlQueryHolder::Execute(MaNGOS::IQueryCallback * callback, Database *database, SqlResultQueue *queue, bool highPriority)
{
    if(!callback || !database || !queue)
        return false;

    /// delay the execution of the queries, sync them with the delay thread
    /// which will in turn resync on execution (via the queue) and call back
    SqlQueryHolderEx *holderEx = new SqlQueryHolderEx(this, callback, queue, serialId, highPriority);

    if (highPriority)
        database->AddToPrioritySerialDelayQueue(holderEx);
    else
        database->AddToSerialDelayQueue(holderEx);
    return true;
}

bool SqlQueryHolder::SetQuery(size_t index, const char *sql)
{
    if(m_queries.size() <= index)
    {
        sLog.outError("Query index (" SIZEFMTD ") out of range (size: " SIZEFMTD ") for query: %s", index, m_queries.size(), sql);
        return false;
    }

    if(m_queries[index].first != nullptr)
    {
        sLog.outError("Attempt assign query to holder index (" SIZEFMTD ") where other query stored (Old: [%s] New: [%s])",
            index,m_queries[index].first,sql);
        return false;
    }

    /// not executed yet, just stored (it's not called a holder for nothing)
    m_queries[index] = SqlResultPair(mangos_strdup(sql), (QueryResult*)nullptr);
    return true;
}

bool SqlQueryHolder::SetPQuery(size_t index, const char *format, ...)
{
    if(!format)
    {
        sLog.outError("Query (index: " SIZEFMTD ") is empty.",index);
        return false;
    }

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf( szQuery, MAX_QUERY_LEN, format, ap );
    va_end(ap);

    if(res==-1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return false;
    }

    return SetQuery(index,szQuery);
}

QueryResult* SqlQueryHolder::GetResult(size_t index)
{
    if(index < m_queries.size())
    {
        /// the query strings are freed on the first GetResult or in the destructor
        if(m_queries[index].first != nullptr)
        {
            delete [] (const_cast<char*>(m_queries[index].first));
            m_queries[index].first = nullptr;
        }
        /// when you get a result aways remember to delete it!
        return m_queries[index].second;
    }
    else
        return nullptr;
}

void SqlQueryHolder::SetResult(size_t index, QueryResult *result)
{
    /// store the result in the holder
    if(index < m_queries.size())
        m_queries[index].second = result;
}

SqlQueryHolder::~SqlQueryHolder()
{
    for(size_t i = 0; i < m_queries.size(); i++)
    {
        /// if the result was never used, free the resources
        /// results used already (getresult called) are expected to be deleted
        if(m_queries[i].first != nullptr)
        {
            delete [] (const_cast<char*>(m_queries[i].first));
            if(m_queries[i].second)
            {
                delete m_queries[i].second;
                m_queries[i].second = nullptr;
            }
        }
    }
}

void SqlQueryHolder::DeleteAllResults()
{
    for(size_t i = 0; i < m_queries.size(); i++)
    {
        /// if the result was never used, free the resources
        /// results used already (getresult called) are expected to be deleted
        if (m_queries[i].second != nullptr)
        {
            delete m_queries[i].second;
            m_queries[i].second = nullptr;
        }
    }
}

void SqlQueryHolder::SetSize(size_t size)
{
    /// to optimize push_back, reserve the number of queries about to be executed
    m_queries.resize(size);
}

bool SqlQueryHolderEx::Execute(SqlConnection *conn)
{
    if(!m_holder || !m_callback || !m_queue)
        return false;

    LOCK_DB_CONN(conn);
    /// we can do this, we are friends
    std::vector<SqlQueryHolder::SqlResultPair> &queries = m_holder->m_queries;
    for(size_t i = 0; i < queries.size(); i++)
    {
        /// execute all queries in the holder and pass the results
        char const *sql = queries[i].first;
        if (sql)
            m_holder->SetResult(i, conn->Query(sql));
    }

    /// sync with the caller thread
    m_queue->Add(m_callback, m_highPriority);

    return true;
}
