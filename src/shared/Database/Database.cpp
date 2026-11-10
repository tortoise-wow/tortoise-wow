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

#include "Util.h"
#include "DatabaseEnv.h"
#include "Config/Config.h"
#include "Database/SqlOperations.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <memory>

#define MIN_CONNECTION_POOL_SIZE 1
#define MAX_CONNECTION_POOL_SIZE 16

namespace
{
thread_local Database* orderedExecutor = nullptr;

class SynchronousSqlOperation : public SqlOperation
{
    std::function<bool(SqlConnection*)> m_execute;
    bool m_write;
public:
    SynchronousSqlOperation(bool write, std::function<bool(SqlConnection*)> execute) :
        m_execute(std::move(execute)), m_write(write) {}
    bool Execute(SqlConnection* connection) override
    {
        SqlConnection::Lock guard(connection);
        return m_execute(connection);
    }
    bool IsWriteOperation() const override { return m_write; }
};
}

bool Database::ConfigureOrderedPersistence()
{
    if (m_pAsyncConn || !m_pQueryConnections.empty() || !m_threadsBodies.empty())
        return false;
    m_openNormalBuilders = std::make_shared<std::atomic<uint32>>(0);
    m_orderedPersistence = true;
    return true;
}

bool Database::EnableOwnedTransactions()
{
    if (!m_orderedPersistence || !m_pAsyncConn || m_threadsBodies.size() != 1 ||
        m_orderedStopping || m_orderedFault)
        return false;
    m_ownedTransactionsEnabled = true;
    return true;
}

void Database::MarkOrderedPersistenceFault()
{
    if (m_orderedPersistence && !m_orderedFault.exchange(true))
        sLog.outError("Ordered database persistence failed; later work is fenced until operator recovery.");
}

bool Database::IsOrderedExecutorThread() const
{
    return m_orderedPersistence && orderedExecutor == this;
}

void Database::MarkAppendFailure()
{
    if (SqlTransaction* transaction = m_TransStorage->get())
        transaction->MarkAppendFailure();
}

bool Database::RejectOwnedBuilder()
{
    SqlTransaction* transaction = m_TransStorage->get();
    if (!transaction || !transaction->IsOwned())
        return false;
    transaction->MarkAppendFailure();
    sLog.outError("Owned transaction builder refused a nested/direct/query operation.");
    return true;
}

bool Database::RunOrderedSynchronously(std::unique_ptr<SqlOperation> operation)
{
    if (IsOrderedExecutorThread())
    {
        // Never wait for this executor from one of its own callbacks.
        if (operation->IsWriteOperation()) MarkOrderedPersistenceFault();
        sLog.outError("Synchronous ordered database operation refused on its executor thread.");
        return false;
    }
    if (RejectOwnedBuilder())
        return false;
    try
    {
        auto completion = std::make_shared<std::promise<bool>>();
        std::future<bool> result = completion->get_future();
        operation->SetCompletion(completion);
        if (!AddToDelayQueue(operation.release()))
            return false;
        return result.get();
    }
    catch (...)
    {
        MarkOrderedPersistenceFault();
        return false;
    }
}

QueryResult* Database::Query(const char* sql)
{
    if (RejectOwnedBuilder() || !sql || m_orderedFault || m_orderedStopping)
        return nullptr;
    if (m_orderedPersistence && m_bAllowAsyncTransactions)
    {
        QueryResult* result = nullptr;
        std::string request(sql);
        RunOrderedSynchronously(std::unique_ptr<SqlOperation>(new SynchronousSqlOperation(false,
            [&result, request](SqlConnection* connection) { result = connection->Query(request.c_str()); return true; })));
        return result;
    }
    SqlConnection::Lock guard(getQueryConnection());
    return guard->Query(sql);
}

QueryNamedResult* Database::QueryNamedInternal(const char* sql)
{
    if (RejectOwnedBuilder() || !sql || m_orderedFault || m_orderedStopping)
        return nullptr;
    if (m_orderedPersistence && m_bAllowAsyncTransactions)
    {
        QueryNamedResult* result = nullptr;
        std::string request(sql);
        RunOrderedSynchronously(std::unique_ptr<SqlOperation>(new SynchronousSqlOperation(false,
            [&result, request](SqlConnection* connection) { result = connection->QueryNamed(request.c_str()); return true; })));
        return result;
    }
    SqlConnection::Lock guard(getQueryConnection());
    return guard->QueryNamed(sql);
}

#ifdef ENABLE_ELUNA
std::shared_ptr<QueryNamedResult> Database::QueryNamed(const char* sql)
{
    return std::shared_ptr<QueryNamedResult>(QueryNamedInternal(sql));
}
#else
QueryNamedResult* Database::QueryNamed(const char* sql) { return QueryNamedInternal(sql); }
#endif

bool Database::DirectExecute(const char* sql)
{
    if (RejectOwnedBuilder() || !m_pAsyncConn || !sql || m_orderedFault || m_orderedStopping)
        return false;
    if (m_orderedPersistence && m_bAllowAsyncTransactions)
        return RunOrderedSynchronously(std::unique_ptr<SqlOperation>(new SqlPlainRequest(sql)));
    SqlConnection::Lock guard(m_pAsyncConn);
    return guard->Execute(sql);
}

Database::OwnedTransaction::OwnedTransaction(Database& db, SqlTransaction* transaction,
    std::unique_lock<std::mutex> admission) : m_database(db), m_transaction(transaction), m_admission(std::move(admission)) {}

Database::OwnedTransaction::~OwnedTransaction()
{
    if (m_transaction)
    {
        // Only this handle may destroy its builder. Abandonment after game-state
        // mutation cannot safely be followed by an unrelated save.
        if (m_database.m_TransStorage->get() == m_transaction)
            m_database.m_TransStorage->reset();
        m_database.MarkOrderedPersistenceFault();
    }
}

bool Database::OwnedTransaction::Commit()
{
    if (!m_transaction)
        return false;
    if (m_database.m_TransStorage->get() != m_transaction)
    {
        m_database.MarkOrderedPersistenceFault();
        m_transaction = nullptr;
        m_admission.unlock();
        return false;
    }
    std::unique_ptr<SqlTransaction> transaction(m_database.m_TransStorage->detach());
    m_transaction = nullptr;
    if (transaction->Empty() || transaction->HasAppendFailure() || m_database.m_orderedFault || m_database.m_orderedStopping)
    {
        m_database.MarkOrderedPersistenceFault();
        m_admission.unlock();
        return false;
    }
    try
    {
        auto completion = std::make_shared<std::promise<bool>>();
        std::future<bool> result = completion->get_future();
        transaction->SetCompletion(completion);
        m_database.m_delayQueue->add(transaction.get());
        transaction.release();
        // The worker or its callbacks may need admission. Never wait holding it.
        m_admission.unlock();
        return result.get();
    }
    catch (...)
    {
        m_database.MarkOrderedPersistenceFault();
        if (m_admission.owns_lock()) m_admission.unlock();
        return false;
    }
}

std::unique_ptr<Database::OwnedTransaction> Database::BeginOwnedTransaction(std::vector<uint32> participants)
{
    if (IsOrderedExecutorThread() || RejectOwnedBuilder() || m_TransStorage->get() ||
        !m_orderedPersistence || !m_ownedTransactionsEnabled || !m_bAllowAsyncTransactions)
        return nullptr;
    std::sort(participants.begin(), participants.end());
    if (participants.empty() || !participants.front() ||
        std::adjacent_find(participants.begin(), participants.end()) != participants.end())
        return nullptr;
    std::unique_lock<std::mutex> admission(m_orderedAdmission);
    if (m_orderedFault || m_orderedStopping || m_openNormalBuilders->load() != 0)
        return nullptr;
    try
    {
        SqlTransaction* transaction = m_TransStorage->init(0, m_knownAbortRetries);
        transaction->SetParticipants(std::move(participants));
        return std::unique_ptr<OwnedTransaction>(new OwnedTransaction(*this, transaction, std::move(admission)));
    }
    catch (...)
    {
        m_TransStorage->reset();
        MarkOrderedPersistenceFault();
        return nullptr;
    }
}

bool Database::IsOwnedTransactionParticipant(uint32 guid)
{
    SqlTransaction* transaction = m_TransStorage->get();
    return transaction && transaction->IsOwned() && !transaction->HasAppendFailure() && transaction->HasParticipant(guid);
}

bool Database::InOwnedTransaction()
{
    SqlTransaction* transaction = m_TransStorage->get();
    return transaction && transaction->IsOwned();
}

bool Database::AddToDelayQueue(SqlOperation* op)
{
    std::unique_ptr<SqlOperation> operation(op);
    if (RejectOwnedBuilder() || !op)
        return false;
    std::unique_lock<std::mutex> admission(m_orderedAdmission, std::defer_lock);
    if (m_orderedPersistence)
    {
        admission.lock();
        if (m_orderedFault || m_orderedStopping)
            return false;
    }
    // Release a normal builder's lease before exposing it to the executor:
    // the worker may pop and destroy it immediately after add().
    operation->OnAdmission();
    try { m_delayQueue->add(operation.get()); }
    catch (const std::bad_alloc&) { MarkOrderedPersistenceFault(); return false; }
    operation.release();
    return true;
}

void Database::ExecuteDelayedOperation(SqlOperation* op, SqlConnection* connection)
{
    std::unique_ptr<SqlOperation> operation(op);
    Database* previousExecutor = orderedExecutor;
    if (m_orderedPersistence) orderedExecutor = this;
    bool result = false;
    try
    {
        // Closing admission during a healthy shutdown must not discard queued
        // logout saves. Drain them on the live worker; only a fault cancels.
        if (m_orderedPersistence && (m_orderedFault || !connection))
        {
            if (operation->IsWriteOperation()) MarkOrderedPersistenceFault();
            operation->Cancel();
        }
        else
        {
            result = operation->Execute(connection);
            if (!result && operation->IsWriteOperation()) MarkOrderedPersistenceFault();
        }
    }
    catch (...)
    {
        MarkOrderedPersistenceFault();
        sLog.outError("Exception in delayed database operation.");
        try { operation->Cancel(); }
        catch (...) { sLog.outError("Exception while canceling delayed database operation."); }
    }
    try
    {
        if (const auto& callback = operation->GetCallback()) (*callback)(result);
    }
    catch (...) { MarkOrderedPersistenceFault(); sLog.outError("Exception in delayed database callback."); }
    try { operation->Complete(result); }
    catch (...) { MarkOrderedPersistenceFault(); sLog.outError("Exception completing delayed database operation."); }
    orderedExecutor = previousExecutor;
}

//////////////////////////////////////////////////////////////////////////
SqlPreparedStatement * SqlConnection::CreateStatement( const std::string& fmt )
{
    return new SqlPlainPreparedStatement(fmt, *this);
}

void SqlConnection::FreePreparedStatements()
{
    SqlConnection::Lock guard(this);

    size_t nStmts = m_holder.size();
    for (size_t i = 0; i < nStmts; ++i)
        delete m_holder[i];

    m_holder.clear();
}

SqlPreparedStatement * SqlConnection::GetStmt( int nIndex )
{
    if(nIndex < 0)
        return nullptr;

    //resize stmt container
    if(m_holder.size() <= nIndex)
        m_holder.resize(nIndex + 1, nullptr);

    SqlPreparedStatement * pStmt = nullptr;

    //create stmt if needed
    if(m_holder[nIndex] == nullptr)
    {
        //obtain SQL request string
        std::string fmt = m_db.GetStmtString(nIndex);
        MANGOS_ASSERT(fmt.length());
        //allocate SQlPreparedStatement object
        pStmt = CreateStatement(fmt);
        //prepare statement
        if(!pStmt->prepare())
        {
            //MANGOS_ASSERT(false && "Unable to prepare SQL statement");
            sLog.outError("Can't prepare %s, statement not executed!", fmt.c_str());
            delete pStmt;
            return nullptr;
        }

        //save statement in internal registry
        m_holder[nIndex] = pStmt;
    }
    else
        pStmt = m_holder[nIndex];

    return pStmt;
}

bool SqlConnection::Initialize(const char *infoString)
{
    Tokens tokens = StrSplit(infoString, ";");

    Tokens::iterator iter;

    iter = tokens.begin();

    m_use_socket = false;
    if (iter != tokens.end())
    {
        m_host = *iter++;
        if (m_host == ".")
        {
            m_host = "localhost";
            m_use_socket = true;
        }
    }
    if (iter != tokens.end())
    {
        m_port_or_socket = *iter++;
        m_port = atoi(m_port_or_socket.c_str());
    }
    if (iter != tokens.end())
        m_user = *iter++;
    if (iter != tokens.end())
        m_password = *iter++;
    if (iter != tokens.end())
        m_database = *iter++;

    return OpenConnection(false);
}

bool SqlConnection::ExecuteStmt(int nIndex, const SqlStmtParameters& id )
{
    if(nIndex == -1)
        return false;

    //get prepared statement object
    if (SqlPreparedStatement * pStmt = GetStmt(nIndex))
    {
        //bind parameters
        pStmt->bind(id);
        //execute statement
        return pStmt->execute();
    }
    return false;
}

bool SqlConnection::ExecuteMigrationBatch(const std::vector<std::string>& statements)
{
    if (!BeginTransaction())
        return false;
    for (const auto& statement : statements)
    {
        if (!Execute(statement.c_str()))
        {
            RollbackTransaction();
            return false;
        }
    }
    if (CommitTransaction())
        return true;
    RollbackTransaction();
    return false;
}

//////////////////////////////////////////////////////////////////////////
Database::~Database()
{
    StopServer();
    delete m_delayQueue;
}

bool Database::Initialize(const char* name, const char *infoString, int nConns /*= 1*/, int nWorkers /*= 1*/)
{
    if (m_orderedPersistence)
        nWorkers = 1;
    // Enable logging of SQL commands (usually only GM commands)
    // (See method: PExecuteLog)
    m_logSQL = sConfig.GetBoolDefault("LogSQL", false);
    m_logsDir = sConfig.GetStringDefault("LogsDir","");
    if(!m_logsDir.empty())
    {
        if((m_logsDir.at(m_logsDir.length()-1)!='/') && (m_logsDir.at(m_logsDir.length()-1)!='\\'))
            m_logsDir.append("/");
    }

    m_pingIntervallms = sConfig.GetIntDefault ("MaxPingTime", 30) * (MINUTE * 1000);

    //create DB connections

    //setup connection pool size
    if(nConns < MIN_CONNECTION_POOL_SIZE)
        m_nQueryConnPoolSize = MIN_CONNECTION_POOL_SIZE;
    else if(nConns > MAX_CONNECTION_POOL_SIZE)
        m_nQueryConnPoolSize = MAX_CONNECTION_POOL_SIZE;
    else
        m_nQueryConnPoolSize = nConns;

    //create connection pool for sync requests
    for (int i = 0; i < m_nQueryConnPoolSize; ++i)
    {
        SqlConnection * pConn = CreateConnection();
        if(!pConn->Initialize(infoString))
        {
            delete pConn;
            return false;
        }

        m_pQueryConnections.push_back(pConn);
    }

    //create and initialize connection for async requests
    m_pResultQueue = new SqlResultQueue(name);
    m_pAsyncConn = CreateConnection();
    if(!m_pAsyncConn->Initialize(infoString))
        return false;

    m_numAsyncWorkers = nWorkers;

    for (int i = 0; i < nWorkers; ++i)
        if (!InitDelayThread(name,infoString))
            return false;

    return true;
}

void Database::StopServer()
{
    HaltDelayThread();

    /*Delete objects*/
    if (m_pResultQueue)
    {
        // Delete queued queries
        m_pResultQueue->CancelAll();
        delete m_pResultQueue;
        m_pResultQueue = nullptr;
    }

    if (m_pAsyncConn)
    {
        delete m_pAsyncConn;
        m_pAsyncConn = nullptr;
    }

    for (size_t i = 0; i < m_pQueryConnections.size(); ++i)
        delete m_pQueryConnections[i];

    m_pQueryConnections.clear();

}

bool Database::InitDelayThread(const char* Name, std::string const& infoString)
{
    //New delay thread for delay execute

    SqlConnection* threadConnection = CreateConnection();
    if(!threadConnection->Initialize(infoString.c_str()))
        return false;

    std::shared_ptr<SqlDelayThread> tbody = std::make_shared<SqlDelayThread>(Name, this, threadConnection);
    m_threadsBodies.emplace_back(tbody);
    m_delayThreads.emplace_back([tbody](){
        tbody->run();
    });

    return true;
}

void Database::HaltDelayThread()
{
    if (m_orderedPersistence)
    {
        std::lock_guard<std::mutex> admission(m_orderedAdmission);
        m_orderedStopping = true;
    }
    if (m_delayThreads.empty() || m_threadsBodies.empty())
    {
        if (m_orderedPersistence)
        {
            SqlOperation* operation = nullptr;
            while (m_delayQueue->next(operation)) ExecuteDelayedOperation(operation, nullptr);
        }
        return;
    }

    for (uint32 i = 0; i < m_numAsyncWorkers; ++i)
        m_threadsBodies[i]->Stop();

    for (uint32 i = 0; i < m_numAsyncWorkers; ++i)
        m_delayThreads[i].join();

    m_threadsBodies.clear();
    m_delayThreads.clear();

    m_numAsyncWorkers = 0;
    if (m_orderedPersistence)
    {
        SqlOperation* operation = nullptr;
        while (m_delayQueue->next(operation)) ExecuteDelayedOperation(operation, nullptr);
    }
}

void Database::ThreadStart()
{
}

void Database::ThreadEnd()
{
}

void Database::ProcessResultQueue(uint32 maxTime)
{
    if (m_pResultQueue)
        m_pResultQueue->Update(maxTime);
}

void Database::escape_string(std::string& str)
{
    if (str.empty())
        return;

    int bufSize = str.size() * 2 + 1;
    char* buf = new char[bufSize + 1];
    //we don't care what connection to use - escape string will be the same
    m_pQueryConnections[0]->escape_string(buf, str.c_str(), str.size());
    buf[bufSize] = 0;
    str = buf;
    delete[] buf;
}

SqlConnection * Database::getQueryConnection()
{
    int nCount = 0;

    if(m_nQueryCounter == long(1 << 31))
        m_nQueryCounter = 0;
    else
        nCount = ++m_nQueryCounter;

    return m_pQueryConnections[nCount % m_nQueryConnPoolSize];
}

void Database::Ping()
{
    if (m_orderedPersistence && m_bAllowAsyncTransactions)
    {
        delete Query("SELECT 1");
        return;
    }
    const char * sql = "SELECT 1";

    {
        SqlConnection::Lock guard(m_pAsyncConn);
        delete guard->Query(sql);
    }

    for (int i = 0; i < m_nQueryConnPoolSize; ++i)
    {
        SqlConnection::Lock guard(m_pQueryConnections[i]);
        delete guard->Query(sql);
    }
}

bool Database::PExecuteLog(const char * format,...)
{
    if (!format)
        return false;

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf( szQuery, MAX_QUERY_LEN, format, ap );
    va_end(ap);

    if(res < 0 || res >= MAX_QUERY_LEN)
    {
        MarkAppendFailure();
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return false;
    }

    if( m_logSQL )
    {
        time_t curr;
        tm local;
        time(&curr);                                        // get current time_t value
        local=*(localtime(&curr));                          // dereference and assign
        char fName[128];
        sprintf( fName, "%04d-%02d-%02d_logSQL.sql", local.tm_year+1900, local.tm_mon+1, local.tm_mday );

        FILE* log_file;
        std::string logsDir_fname = m_logsDir+fName;
        log_file = fopen(logsDir_fname.c_str(), "a");
        if (log_file)
        {
            fprintf(log_file, "%s;\n", szQuery);
            fclose(log_file);
        }
        else
        {
            // The file could not be opened
            sLog.outError("SQL-Logging is disabled - Log file for the SQL commands could not be openend: %s",fName);
        }
    }

    return Execute(szQuery);
}

QueryResult* Database::PQuery(const char *format,...)
{
    if(!format) { MarkAppendFailure(); return nullptr; }

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf( szQuery, MAX_QUERY_LEN, format, ap );
    va_end(ap);

    if(res < 0 || res >= MAX_QUERY_LEN)
    {
        MarkAppendFailure();
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return nullptr;
    }

    return Query(szQuery);
}

#ifdef ENABLE_ELUNA
std::shared_ptr<QueryNamedResult> Database::PQueryNamed(const char *format,...)
#else
QueryNamedResult* Database::PQueryNamed(const char *format,...)
#endif
{
    if(!format) { MarkAppendFailure(); return nullptr; }

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf( szQuery, MAX_QUERY_LEN, format, ap );
    va_end(ap);

    if(res < 0 || res >= MAX_QUERY_LEN)
    {
        MarkAppendFailure();
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return nullptr;
    }

    return QueryNamed(szQuery);
}

bool Database::Execute(const char *sql, bool multiline, std::function<void(bool)>* callback)
{
    if (!m_pAsyncConn || !sql || m_orderedFault || m_orderedStopping)
    {
        MarkAppendFailure();
        return false;
    }

    SqlTransaction * pTrans = m_TransStorage->get();
    if(pTrans)
    {
        if (pTrans->IsOwned() && callback)
        {
            pTrans->MarkAppendFailure();
            return false;
        }
        if (callback)
            pTrans->SetCallback(callback);


        //add SQL request to trans queue
        return pTrans->DelayExecute(multiline ? static_cast<SqlOperation*>(new SqlMultilineRequest(sql)) : new SqlPlainRequest(sql));
    }
    else
    {
        //if async execution is not available
        if(!m_bAllowAsyncTransactions)
            return DirectExecute(sql);

        auto sqlOperation = multiline ? static_cast<SqlOperation*>(new SqlMultilineRequest(sql)) : new SqlPlainRequest(sql);

        if (callback)
            sqlOperation->SetCallback(callback);

        // Simple sql statement
        return AddToDelayQueue(sqlOperation);
    }

    return true;
}

bool Database::PExecute(const char * format, ...)
{
    if (!format)
    {
        MarkAppendFailure();
        return false;
    }

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf( szQuery, MAX_QUERY_LEN, format, ap );
    va_end(ap);

    if(res < 0 || res >= MAX_QUERY_LEN)
    {
        MarkAppendFailure();
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return false;
    }

    return Execute(szQuery);
}

bool Database::PExecuteCallback(const char* format, std::function<void(bool)>* callback, ...)
{
    if (!format)
    {
        MarkAppendFailure();
        return false;
    }

    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, callback);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res < 0 || res >= MAX_QUERY_LEN)
    {
        MarkAppendFailure();
        sLog.outError("SQL Query truncated (and not execute) for format: %s", format);
        return false;
    }

    return Execute(szQuery, false, callback);
}

bool Database::DirectPExecute(const char * format,...)
{
    if (!format)
    {
        MarkAppendFailure();
        return false;
    }

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf( szQuery, MAX_QUERY_LEN, format, ap );
    va_end(ap);

    if(res < 0 || res >= MAX_QUERY_LEN)
    {
        MarkAppendFailure();
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return false;
    }

    return DirectExecute(szQuery);
}

bool Database::BeginTransaction(uint32 serialId)
{
    if (RejectOwnedBuilder() || m_orderedFault || m_orderedStopping)
        return false;
    if (!m_pAsyncConn)
    {
        sLog.outError("Cant begin transaction.");
        return false;
    }
    //ASSERT(!m_TransStorage->get());
    if (m_TransStorage->get())
    {
        sLog.outError("Cant begin transaction.");
        return false;
    }

    // Register before any SQL snapshot capture. Owned groups refuse admission
    // while an older normal builder is still staging, but legacy reads need not
    // hold this gate and can still use ordinary FIFO query barriers.
    std::unique_lock<std::mutex> admission(m_orderedAdmission, std::defer_lock);
    if (m_orderedPersistence && m_bAllowAsyncTransactions)
    {
        admission.lock();
        if (m_orderedFault || m_orderedStopping)
            return false;
    }
    SqlTransaction* transaction = m_TransStorage->init(serialId, m_knownAbortRetries);
    if (admission.owns_lock()) transaction->HoldBuilderLease(m_openNormalBuilders);
    return true;
}

bool Database::InTransaction()
{
    return m_TransStorage->get() != nullptr;
}

uint32 Database::GetTransactionSerialId()
{
    if (SqlTransaction *trans = m_TransStorage->get())
        return trans->GetSerialId();

    return 0;
}

bool Database::CommitTransaction(std::function<void(bool)>* callback)
{
    if (RejectOwnedBuilder())
        return false;
    if (!m_pAsyncConn)
    {
        sLog.outError("Cant commit transaction.");
        return false;
    }

    //check if we have pending transaction
    //ASSERT(m_TransStorage->get());
    if (!m_TransStorage->get())
    {
        sLog.outError("Cant commit transaction.");
        return false;
    }

    //if async execution is not available
    if (!m_bAllowAsyncTransactions)
    {
        bool res = CommitTransactionDirect();
        if (callback)
            (*callback)(res);
        return res;
    }

    //add SqlTransaction to the async queue
    // if serial ID > 0, add to the serial delay queue
    SqlTransaction *trans = m_TransStorage->detach();
    if (callback)
        trans->SetCallback(callback);

    if (trans->GetSerialId() > 0)
        return AddToSerialDelayQueue(trans);
    else
        return AddToDelayQueue(trans);
}

bool Database::CommitTransactionDirect()
{
    if (RejectOwnedBuilder() || !m_pAsyncConn)
        return false;

    //check if we have pending transaction
    if (!m_TransStorage->get())
        return false;

    //directly execute SqlTransaction
    SqlTransaction * pTrans = m_TransStorage->detach();
    if (m_orderedPersistence && m_bAllowAsyncTransactions)
        return RunOrderedSynchronously(std::unique_ptr<SqlOperation>(pTrans));
    bool res = pTrans->Execute(m_pAsyncConn);
    delete pTrans;

    return res;
}

bool Database::ExecuteMigrationBatch(const std::vector<std::string>& statements)
{
    if (RejectOwnedBuilder())
        return false;
    // The updater owns this connection only before runtime writers are enabled.
    if (!m_pAsyncConn || m_bAllowAsyncTransactions || InTransaction())
    {
        sLog.outError("Database migrations require startup with no pending runtime transaction.");
        return false;
    }
    SqlConnection::Lock guard(m_pAsyncConn);
    return guard->ExecuteMigrationBatch(statements);
}

bool Database::RollbackTransaction()
{
    if (RejectOwnedBuilder())
        return false;
    if (!m_pAsyncConn)
        return false;

    if(!m_TransStorage->get())
        return false;

    //remove scheduled transaction
    m_TransStorage->reset();

    return true;
}

bool Database::AddToSerialDelayQueue(int workerId, SqlOperation* op)
{
    if (m_orderedPersistence)
        return AddToDelayQueue(op);
    if (!op || workerId < 0 || size_t(workerId) >= m_threadsBodies.size())
    {
        delete op;
        return false;
    }
    m_threadsBodies[workerId]->addSerialOperation(op);
    return true;
}

bool Database::AddToSerialDelayQueue(SqlOperation *op)
{
    if (!op || m_orderedPersistence || op->GetSerialId() == 0 || m_numAsyncWorkers == 0)
    {
        return AddToDelayQueue(op);
    }

    // This is a very naive way of doing this. No load balancing.
    // TODO: Load balance, must maintain mapping of serial ID so queries are
    // executed sequentially, however
    int worker = op->GetSerialId() % m_numAsyncWorkers;
    return AddToSerialDelayQueue(worker, op);
}

bool Database::HasAsyncQuery()
{
    bool hasQuery = !m_delayQueue->empty_unsafe();

    for (uint32 i = 0; i < m_numAsyncWorkers && !hasQuery; ++i)
        hasQuery = m_threadsBodies[i]->HasAsyncQuery();

    return hasQuery;
}

bool Database::ExecuteStmt(const SqlStatementID& id, SqlStmtParameters * params)
{
    std::unique_ptr<SqlStmtParameters> owner(params);
    if (!m_pAsyncConn || !params || m_orderedFault || m_orderedStopping)
    {
        MarkAppendFailure();
        return false;
    }

    SqlTransaction * pTrans = m_TransStorage->get();
    if(pTrans)
    {
        //add SQL request to trans queue
        return pTrans->DelayExecute(new SqlPreparedRequest(id.ID(), owner.release()));
    }
    else
    {
        //if async execution is not available
        if(!m_bAllowAsyncTransactions)
            return DirectExecuteStmt(id, owner.release());

        // Simple sql statement
        return AddToDelayQueue(new SqlPreparedRequest(id.ID(), owner.release()));
    }

    return true;
}

bool Database::DirectExecuteStmt( const SqlStatementID& id, SqlStmtParameters * params )
{
    MANGOS_ASSERT(params);
    std::unique_ptr<SqlStmtParameters> p(params);
    if (RejectOwnedBuilder() || m_orderedFault || m_orderedStopping || !m_pAsyncConn)
        return false;
    if (m_orderedPersistence && m_bAllowAsyncTransactions)
        return RunOrderedSynchronously(std::unique_ptr<SqlOperation>(new SqlPreparedRequest(id.ID(), p.release())));
    //execute statement
    SqlConnection::Lock _guard(getAsyncConnection());
    return _guard->ExecuteStmt(id.ID(), *params);
}

SqlStatement Database::CreateStatement(SqlStatementID& index, const char * fmt )
{
    int nId = -1;
    //check if statement ID is initialized
    if(!index.initialized())
    {
        //convert to lower register
        std::string szFmt(fmt);
        //count input parameters
        int nParams = std::count(szFmt.begin(), szFmt.end(), '?');
        //find existing or add a new record in registry
        LOCK_GUARD _guard(m_stmtGuard);
        PreparedStmtRegistry::const_iterator iter = m_stmtRegistry.find(szFmt);
        if(iter == m_stmtRegistry.end())
        {
            nId = ++m_iStmtIndex;
            m_stmtRegistry[szFmt] = nId;
        }
        else
            nId = iter->second;

        //save initialized statement index info
        index.init(nId, nParams);
    }

    return SqlStatement(index, *this);
}

std::string Database::GetStmtString(const int stmtId) const
{
    LOCK_GUARD _guard(m_stmtGuard);

    if(stmtId == -1 || stmtId > m_iStmtIndex)
        return std::string();

    PreparedStmtRegistry::const_iterator iter_last = m_stmtRegistry.end();
    for(PreparedStmtRegistry::const_iterator iter = m_stmtRegistry.begin(); iter != iter_last; ++iter)
    {
        if(iter->second == stmtId)
            return iter->first;
    }

    return std::string();
}

//HELPER CLASSES AND FUNCTIONS
Database::TransHelper::~TransHelper()
{
    reset();
}

SqlTransaction * Database::TransHelper::init(uint32 serialId, uint32 knownAbortRetries)
{
    MANGOS_ASSERT(!m_pTrans);   //if we will get a nested transaction request - we MUST fix code!!!
    m_pTrans = new SqlTransaction(serialId, knownAbortRetries);

    return m_pTrans;
}

SqlTransaction * Database::TransHelper::detach()
{
    SqlTransaction * pRes = m_pTrans;
    m_pTrans = nullptr;
    return pRes;
}

void Database::TransHelper::reset()
{
    if(m_pTrans)
    {
        delete m_pTrans;
        m_pTrans = nullptr;
    }
}
