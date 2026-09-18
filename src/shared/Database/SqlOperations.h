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

#ifndef __SQLOPERATIONS_H
#define __SQLOPERATIONS_H

#include "Common.h"

#include "LockedQueue.h"
#include <queue>
#include "Utilities/Callback.h"
#include <memory>
#include <optional>
#include <functional>
#include "Memory/MemoryLedger.h"

/// ---- BASE ---

class Database;
class SqlConnection;
class SqlDelayThread;
class SqlStmtParameters;

class SqlOperation
{
    public:
        SqlOperation(uint32 id) : serialId(id) { ManTech::MemoryLedger::Add(ManTech::MemoryKind::DatabaseWork, sizeof(SqlOperation)); }
        SqlOperation() : SqlOperation(0) {}
        uint32 GetSerialId() const { return serialId; }
        virtual void OnRemove() { delete this; }
        virtual bool Execute(SqlConnection *conn) = 0;
        virtual bool IsReadOnly() const { return false; }
        virtual ~SqlOperation() { ManTech::MemoryLedger::Remove(ManTech::MemoryKind::DatabaseWork, sizeof(SqlOperation) + m_payloadBytes); }
        void AccountQueryPayload(size_t bytes) { m_payloadBytes += bytes; ManTech::MemoryLedger::Add(ManTech::MemoryKind::DatabaseWork, bytes, 0); }
    private:
        size_t m_payloadBytes = 0;
    public:

        const auto& GetCallback() const { return callback; }

        void SetCallback(std::function<void(bool)>* cb)
        {
            callback = std::make_unique<std::function<void(bool)>>(*cb);
        }

    protected:
        uint32 serialId;
        std::unique_ptr<std::function<void(bool)>> callback;
};

/// ---- ASYNC STATEMENTS / TRANSACTIONS ----

class SqlPlainRequest : public SqlOperation
{
    private:
        const char *m_sql;
    public:
        SqlPlainRequest(const char *sql) : m_sql(mangos_strdup(sql)) { AccountQueryPayload(strlen(m_sql) + 1); }
        ~SqlPlainRequest() { char* tofree = const_cast<char*>(m_sql); delete [] tofree; }
        bool Execute(SqlConnection *conn);
};

class SqlMultilineRequest : public SqlOperation
{
private:
    std::string m_sql;
public:
    SqlMultilineRequest(const char* sql) : m_sql(sql) { AccountQueryPayload(m_sql.capacity() + 1); }

    bool Execute(SqlConnection* conn);
};

class SqlTransaction : public SqlOperation
{
    private:
        std::vector<SqlOperation * > m_queue;
        bool m_retryDeadlock;

    public:
        SqlTransaction(uint32 serialId, bool retryDeadlock = false) : SqlOperation(serialId), m_retryDeadlock(retryDeadlock) {}
        ~SqlTransaction();

        void DelayExecute(SqlOperation * sql)   {   m_queue.push_back(sql); }

        bool Execute(SqlConnection *conn);
};

class SqlPreparedRequest : public SqlOperation
{
    public:
        SqlPreparedRequest(int nIndex, SqlStmtParameters * arg);
        ~SqlPreparedRequest();

        bool Execute(SqlConnection *conn);

    private:
        const int m_nIndex;
        SqlStmtParameters * m_param;
};

/// ---- ASYNC QUERIES ----

class SqlQuery;                                             /// contains a single async query
class QueryResult;                                          /// the result of one
class SqlResultQueue;                                       /// queue for thread sync
class SqlQueryHolder;                                       /// groups several async quries
class SqlQueryHolderEx;                                     /// points to a holder, added to the delay thread

class ThreadPool;

class SqlResultQueue : public LockedQueue<MaNGOS::IQueryCallback* , std::mutex>
{
    public:
        SqlResultQueue(const char* Name);
        ~SqlResultQueue();
        void CancelAll();
        void Update(uint32 maxTime);
        void Add(MaNGOS::IQueryCallback* callback, bool highPriority = false);
        size_t PendingCount() const
        {
            return size() + _priorityWaitingQueries.size() + _threadUnsafeWaitingQueries.size() +
                _priorityThreadUnsafeWaitingQueries.size();
        }
        typedef LockedQueue<MaNGOS::IQueryCallback*, std::mutex> CallbackQueue;
        CallbackQueue _priorityWaitingQueries;
        CallbackQueue _priorityThreadUnsafeWaitingQueries;
        CallbackQueue _threadUnsafeWaitingQueries;
        uint32 numUnsafeQueries;
    private:
        unsigned m_priorityBurst = 0;
        bool nextCallback(MaNGOS::IQueryCallback*& callback, bool priority);
};

class SqlQuery : public SqlOperation
{
    private:
        const char *m_sql;
        MaNGOS::IQueryCallback * m_callback;
        SqlResultQueue * m_queue;
    public:
        SqlQuery(const char *sql, MaNGOS::IQueryCallback * callback, SqlResultQueue * queue, bool highPriority = false)
            : m_sql(mangos_strdup(sql)), m_callback(callback), m_queue(queue), m_highPriority(highPriority) { AccountQueryPayload(strlen(m_sql) + 1); }
        ~SqlQuery() { char* tofree = const_cast<char*>(m_sql); delete [] tofree; }
        bool Execute(SqlConnection *conn);
        bool IsReadOnly() const override { return true; }
        bool m_highPriority;
};

class SqlQueryHolder
{
    friend class SqlQueryHolderEx;
    private:
        typedef std::pair<const char*, QueryResult*> SqlResultPair;
        std::vector<SqlResultPair> m_queries;

        uint32 serialId;
    public:
        SqlQueryHolder(uint32 id) : serialId(id) {}
        SqlQueryHolder() : serialId(0) {}
        virtual ~SqlQueryHolder();
        bool SetQuery(size_t index, const char *sql);
        bool SetPQuery(size_t index, const char *format, ...) ATTR_PRINTF(3,4);
        void SetSize(size_t size);
        size_t GetSize() const { return m_queries.size(); }
        size_t GetQueryPayloadBytes() const {
            size_t bytes = m_queries.capacity() * sizeof(SqlResultPair);
            for (auto const& q : m_queries) if (q.first) bytes += strlen(q.first) + 1;
            return bytes;
        }
        QueryResult* GetResult(size_t index);
        void SetResult(size_t index, QueryResult *result);
        bool Execute(MaNGOS::IQueryCallback * callback, Database *db, SqlResultQueue *queue, bool highPriority = false);
        void DeleteAllResults();
        uint32 GetSerialId() const { return serialId; }
};

class SqlQueryHolderEx : public SqlOperation
{
    private:
        SqlQueryHolder * m_holder;
        MaNGOS::IQueryCallback * m_callback;
        SqlResultQueue * m_queue;
        bool m_highPriority;
    public:
        SqlQueryHolderEx(SqlQueryHolder *holder, MaNGOS::IQueryCallback * callback, SqlResultQueue * queue, uint32 id, bool highPriority = false)
            : SqlOperation(id), m_holder(holder), m_callback(callback), m_queue(queue), m_highPriority(highPriority) { AccountQueryPayload(holder->GetQueryPayloadBytes()); }
        bool Execute(SqlConnection *conn);
        bool IsReadOnly() const override { return true; }
};
#endif                                                      //__SQLOPERATIONS_H
