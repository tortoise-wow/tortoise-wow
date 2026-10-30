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
#include <future>
#include <algorithm>
#include <atomic>

/// ---- BASE ---

class Database;
class SqlConnection;
class SqlDelayThread;
class SqlStmtParameters;

class SqlOperation
{
    public:
        SqlOperation(uint32 id) : serialId(id) {}
        SqlOperation() : serialId(0) {}
        uint32 GetSerialId() const { return serialId; }
        virtual void OnRemove() { delete this; }
        virtual bool Execute(SqlConnection *conn) = 0;
        virtual bool IsWriteOperation() const { return true; }
        virtual void Cancel() {}
        // Called under the ordered admission gate, before queue exposure.
        virtual void OnAdmission() {}
        virtual ~SqlOperation() { try { Complete(false); } catch (...) {} }

        const auto& GetCallback() const { return callback; }

        void SetCallback(std::function<void(bool)>* cb)
        {
            callback = std::make_unique<std::function<void(bool)>>(*cb);
        }

        void SetCompletion(std::shared_ptr<std::promise<bool>> completion) { m_completion = std::move(completion); }
        void Complete(bool result)
        {
            auto completion = std::move(m_completion);
            if (completion) completion->set_value(result);
        }

    protected:
        uint32 serialId;
        std::unique_ptr<std::function<void(bool)>> callback;
        std::shared_ptr<std::promise<bool>> m_completion;
};

/// ---- ASYNC STATEMENTS / TRANSACTIONS ----

class SqlPlainRequest : public SqlOperation
{
    private:
        const char *m_sql;
    public:
        SqlPlainRequest(const char *sql) : m_sql(mangos_strdup(sql)){}
        ~SqlPlainRequest() { char* tofree = const_cast<char*>(m_sql); delete [] tofree; }
        bool Execute(SqlConnection *conn);
};

class SqlMultilineRequest : public SqlOperation
{
private:
    std::string m_sql;
public:
    SqlMultilineRequest(const char* sql) : m_sql(sql) {}

    bool Execute(SqlConnection* conn);
};

class SqlTransaction : public SqlOperation
{
    private:
        std::vector<SqlOperation * > m_queue;
        const uint32 m_knownAbortRetries;
        bool m_appendFailed = false;
        std::vector<uint32> m_participants;
        std::shared_ptr<std::atomic<uint32>> m_builderLease;

    public:
        SqlTransaction(uint32 serialId, uint32 knownAbortRetries = 0) :
            SqlOperation(serialId), m_knownAbortRetries(knownAbortRetries) {}
        ~SqlTransaction();

        bool DelayExecute(SqlOperation* sql);
        void MarkAppendFailure() { m_appendFailed = true; }
        bool HasAppendFailure() const { return m_appendFailed; }
        bool Empty() const { return m_queue.empty(); }
        void SetParticipants(std::vector<uint32> participants) { m_participants = std::move(participants); }
        bool IsOwned() const { return !m_participants.empty(); }
        bool HasParticipant(uint32 guid) const { return std::binary_search(m_participants.begin(), m_participants.end(), guid); }
        void HoldBuilderLease(std::shared_ptr<std::atomic<uint32>> counter);
        void OnAdmission() override;

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
        typedef LockedQueue<MaNGOS::IQueryCallback*, std::mutex> CallbackQueue;
        CallbackQueue _threadUnsafeWaitingQueries;
        uint32 numUnsafeQueries;
        std::unique_ptr<ThreadPool> m_callbackThreads;
};

class SqlQuery : public SqlOperation
{
    private:
        const char *m_sql;
        MaNGOS::IQueryCallback * m_callback;
        SqlResultQueue * m_queue;
    public:
        SqlQuery(const char *sql, MaNGOS::IQueryCallback * callback, SqlResultQueue * queue)
            : m_sql(mangos_strdup(sql)), m_callback(callback), m_queue(queue) {}
        ~SqlQuery() { char* tofree = const_cast<char*>(m_sql); delete [] tofree; delete m_callback; }
        bool Execute(SqlConnection *conn);
        bool IsWriteOperation() const override { return false; }
        void Cancel() override;
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
        QueryResult* GetResult(size_t index);
        void SetResult(size_t index, QueryResult *result);
        bool Execute(MaNGOS::IQueryCallback * callback, Database *db, SqlResultQueue *queue);
        void DeleteAllResults();
        uint32 GetSerialId() const { return serialId; }
};

class SqlQueryHolderEx : public SqlOperation
{
    private:
        SqlQueryHolder * m_holder;
        MaNGOS::IQueryCallback * m_callback;
        SqlResultQueue * m_queue;
    public:
        SqlQueryHolderEx(SqlQueryHolder *holder, MaNGOS::IQueryCallback * callback, SqlResultQueue * queue, uint32 id)
            : SqlOperation(id), m_holder(holder), m_callback(callback), m_queue(queue) {}
        ~SqlQueryHolderEx() override { delete m_callback; }
        bool Execute(SqlConnection *conn);
        bool IsWriteOperation() const override { return false; }
        void Cancel() override;
};
#endif                                                      //__SQLOPERATIONS_H
