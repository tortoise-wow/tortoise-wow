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
#include "ArchitectureDiagnostics.h"
#include "DatabaseEnv.h"
#include "Config/Config.h"
#include "Database/SqlOperations.h"
#include "WorkMetrics.h"

#include <ctime>
#include <iostream>
#include <fstream>
#include <memory>

#define MIN_CONNECTION_POOL_SIZE 1
#define MAX_CONNECTION_POOL_SIZE 16

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
    SetStatementDeadlock(false);
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

//////////////////////////////////////////////////////////////////////////
Database::~Database()
{
    StopServer();
}

bool Database::Initialize(const char* name, const char *infoString, int nConns /*= 1*/, int nWorkers /*= 1*/)
{
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
    {
        delete threadConnection;
        return false;
    }

    std::shared_ptr<SqlDelayThread> tbody = std::make_shared<SqlDelayThread>(Name, this, threadConnection);
    m_threadsBodies.emplace_back(tbody);
    m_delayThreads.emplace_back([tbody](){
        tbody->run();
    });

    return true;
}

void Database::HaltDelayThread()
{
    // Initialization can fail after only part of the configured pool starts.
    // Stop/join the objects we own, not the requested worker count.
    for (auto const& worker : m_threadsBodies)
        worker->Stop();

    for (auto& thread : m_delayThreads)
        if (thread.joinable()) thread.join();

    // A final callback may have queued work to a worker that already exited.
    // Keep all worker/connection objects alive until that tail also drains.
    size_t drained;
    do
    {
        drained = 0;
        for (auto const& worker : m_threadsBodies)
            drained += worker->DrainRequests();
    } while (drained);

    m_threadsBodies.clear();
    m_delayThreads.clear();

    m_numAsyncWorkers = 0;
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

size_t Database::GetPendingResultCount() const
{
    return m_pResultQueue ? m_pResultQueue->PendingCount() : 0;
}

size_t Database::GetPendingAsyncOperationCount() const
{
    size_t pending = m_delayQueue ? m_delayQueue->size() : 0;
    for (auto const& worker : m_threadsBodies)
        pending += worker->PendingCount();
    return pending;
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

    if(res==-1)
    {
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

QueryResult* Database::Query(const char* sql)
{
    TurtleDiagnostics::Scope wait(TurtleDiagnostics::DatabaseWait);
    SqlConnection::Lock guard(getQueryConnection());
    wait.Finish();
    TurtleDiagnostics::Scope execution(TurtleDiagnostics::DatabaseRead);
    return guard->Query(sql);
}

QueryResult* Database::PQuery(const char *format,...)
{
    if(!format) return nullptr;

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf( szQuery, MAX_QUERY_LEN, format, ap );
    va_end(ap);

    if(res==-1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return nullptr;
    }

    return Query(szQuery);
}

std::shared_ptr<QueryNamedResult> Database::PQueryNamed(const char *format,...)
{
    if(!format) return nullptr;

    va_list ap;
    char szQuery [MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf( szQuery, MAX_QUERY_LEN, format, ap );
    va_end(ap);

    if(res==-1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s",format);
        return nullptr;
    }

    return QueryNamed(szQuery);
}

bool Database::Execute(const char *sql, bool multiline, std::function<void(bool)>* callback)
{
    if (!m_pAsyncConn)
        return false;

    SqlTransaction * pTrans = m_TransStorage->get();
    if(pTrans)
    {
        if (callback)
            pTrans->SetCallback(callback);


        //add SQL request to trans queue
        pTrans->DelayExecute(multiline ? static_cast<SqlOperation*>(new SqlMultilineRequest(sql)) : new SqlPlainRequest(sql));
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
        AddToDelayQueue(sqlOperation);
    }

    return true;
}

bool Database::PExecute(const char * format, ...)
{
    if (!format)
        return false;

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

    return Execute(szQuery);
}

bool Database::PExecuteCallback(const char* format, std::function<void(bool)>* callback, ...)
{
    if (!format)
        return false;

    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, callback);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == -1)
    {
        sLog.outError("SQL Query truncated (and not execute) for format: %s", format);
        return false;
    }

    return Execute(szQuery, false, callback);
}

bool Database::DirectPExecute(const char * format,...)
{
    if (!format)
        return false;

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

    return DirectExecute(szQuery);
}

bool Database::BeginTransaction(uint32 serialId, bool retryDeadlock)
{
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

    //initiate transaction on current thread
    m_TransStorage->init(serialId, retryDeadlock);
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
        AddToSerialDelayQueue(trans);
    else
        AddToDelayQueue(trans);
    return true;
}

bool Database::CommitTransactionDirect()
{
    if (!m_pAsyncConn)
        return false;

    //check if we have pending transaction
    ASSERT (m_TransStorage->get());

    //directly execute SqlTransaction
    SqlTransaction * pTrans = m_TransStorage->detach();
    bool res = pTrans->Execute(m_pAsyncConn);
    delete pTrans;

    return res;
}

bool Database::RollbackTransaction()
{
    if (!m_pAsyncConn)
        return false;

    if(!m_TransStorage->get())
        return false;

    //remove scheduled transaction
    m_TransStorage->reset();

    return true;
}

void Database::AddToDelayQueue(SqlOperation* op)
{
    // Unkeyed writes have one FIFO lane. Reads can use all workers. Keep
    // Turtle's explicit serial-ID affinity for keyed read-after-write chains.
    if (m_numAsyncWorkers && !op->IsReadOnly())
        m_threadsBodies[0]->addSerialOperation(op);
    else
        m_delayQueue->add(op);
}

void Database::AddToSerialDelayQueue(SqlOperation *op)
{
    if (op->GetSerialId() == 0 || m_numAsyncWorkers == 0)
    {
        AddToDelayQueue(op);
        return;
    }

    // This is a very naive way of doing this. No load balancing.
    // TODO: Load balance, must maintain mapping of serial ID so queries are
    // executed sequentially, however
    int worker = op->GetSerialId() % m_numAsyncWorkers;
    m_threadsBodies[worker]->addSerialOperation(op);
}

void Database::AddToPrioritySerialDelayQueue(SqlOperation* op)
{
    if (op->GetSerialId() == 0 || m_numAsyncWorkers == 0)
    {
        AddToPriorityDelayQueue(op);
        return;
    }

    int const worker = op->GetSerialId() % m_numAsyncWorkers;
    m_threadsBodies[worker]->addPrioritySerialOperation(op);
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
    if (!m_pAsyncConn)
        return false;

    SqlTransaction * pTrans = m_TransStorage->get();
    if(pTrans)
    {
        //add SQL request to trans queue
        pTrans->DelayExecute(new SqlPreparedRequest(id.ID(), params));
    }
    else
    {
        //if async execution is not available
        if(!m_bAllowAsyncTransactions)
            return DirectExecuteStmt(id, params);

        // Simple sql statement
        AddToDelayQueue(new SqlPreparedRequest(id.ID(), params));
    }

    return true;
}

bool Database::DirectExecuteStmt( const SqlStatementID& id, SqlStmtParameters * params )
{
    MANGOS_ASSERT(params);
    std::unique_ptr<SqlStmtParameters> p(params);
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

SqlTransaction * Database::TransHelper::init(uint32 serialId, bool retryDeadlock)
{
    MANGOS_ASSERT(!m_pTrans);   //if we will get a nested transaction request - we MUST fix code!!!
    m_pTrans = new SqlTransaction(serialId, retryDeadlock);

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
