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

#ifndef DO_POSTGRESQL

#include <errmsg.h>
#include <mysqld_error.h>
#include "Util.h"
#include "Policies/SingletonImp.h"
#include "Platform/Define.h"
#include "DatabaseEnv.h"
#include "Timer.h"

size_t DatabaseMysql::db_count = 0;

namespace
{
// Fail closed for SQL that can end/restart a transaction before we can inspect
// the server status. This is a statement boundary check, not a SQL parser.
bool IsTransactionalStatement(const char* sql, bool noBackslashEscapes)
{
    if (!sql)
        return false;
    bool foundKeyword = false;
    bool ended = false;
    for (const char* p = sql; *p;)
    {
        if (static_cast<unsigned char>(*p) <= ' ') { ++p; continue; }
        if (*p == '#' || (p[0] == '-' && p[1] == '-' &&
            static_cast<unsigned char>(p[2]) <= ' '))
        {
            while (*p && *p != '\n' && *p != '\r') ++p;
            continue;
        }
        if (p[0] == '/' && p[1] == '*')
        {
            // Executable version comments can contain otherwise hidden SQL.
            if (p[2] == '!' || ((p[2] == 'M' || p[2] == 'm') && p[3] == '!'))
                return false;
            p += 2;
            while (*p && !(p[0] == '*' && p[1] == '/'))
            {
                if (p[0] == '/' && p[1] == '*') return false;
                ++p;
            }
            if (!*p) return false;
            p += 2;
            continue;
        }
        if (ended)
            return false;
        if (!foundKeyword)
        {
            std::string keyword;
            while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') ||
                (*p >= '0' && *p <= '9') || *p == '_' || *p == '$')
            {
                keyword += (*p >= 'a' && *p <= 'z') ? char(*p - 'a' + 'A') : *p;
                ++p;
            }
            if (keyword != "SELECT" && keyword != "INSERT" && keyword != "UPDATE" &&
                keyword != "DELETE" && keyword != "REPLACE")
                return false;
            foundKeyword = true;
            continue;
        }
        if (*p == ';') { ended = true; ++p; continue; }
        if (*p == '\'' || *p == '"' || *p == '`')
        {
            char quote = *p++;
            bool closed = false;
            while (*p)
            {
                if (*p == quote)
                {
                    if (p[1] == quote) { p += 2; continue; }
                    ++p; closed = true; break;
                }
                if (*p == '\\' && quote != '`' && !noBackslashEscapes)
                {
                    // Double quotes can denote ANSI identifiers; their escape
                    // rules are ambiguous without inspecting the SQL mode.
                    if (quote == '"' || !p[1]) return false;
                    p += 2;
                }
                else ++p;
            }
            if (!closed) return false;
            continue;
        }
        ++p;
    }
    return foundKeyword;
}
}

void DatabaseMysql::ThreadStart()
{
    mysql_thread_init();
}

void DatabaseMysql::ThreadEnd()
{
    mysql_thread_end();
}

DatabaseMysql::DatabaseMysql()
{
    // before first connection
    if (db_count++ == 0 )
    {
        // Mysql Library Init
        mysql_library_init(-1, nullptr, nullptr);

        if (!mysql_thread_safe())
        {
            sLog.outError("FATAL ERROR: Used MySQL library isn't thread-safe.");
            Log::WaitBeforeContinueIfNeed();
            exit(1);
        }
    }
}

DatabaseMysql::~DatabaseMysql()
{
    StopServer();

    //Free Mysql library pointers for last ~DB
    if(--db_count == 0)
        mysql_library_end();
}

SqlConnection * DatabaseMysql::CreateConnection()
{
    return new MySQLConnection(*this);
}

MySQLConnection::~MySQLConnection()
{
    CloseConnection();
}

void MySQLConnection::CloseConnection()
{
    FreePreparedStatements();
    if (mMysql)
        mysql_close(mMysql);
    mMysql = nullptr;
}

bool MySQLConnection::OpenConnection(bool reconnect)
{
    if (m_transactionActive || m_migrationActive)
    {
        RecordError(0);
        return false;
    }

    MYSQL * mysqlInit = mysql_init(nullptr);
    if (!mysqlInit)
    {
        sLog.outError( "Could not initialize Mysql connection" );
        return false;
    }

    mysql_options(mysqlInit, MYSQL_SET_CHARSET_NAME, "utf8");
    // A library reconnect would silently abandon the current transaction.
    my_bool automaticReconnect = 0;
    if (mysql_options(mysqlInit, MYSQL_OPT_RECONNECT, &automaticReconnect))
    {
        sLog.outError("Could not disable automatic MySQL reconnection");
        mysql_close(mysqlInit);
        return false;
    }

    if (m_use_socket)                                           // socket use option (Unix/Linux)
    {
        unsigned int opt = MYSQL_PROTOCOL_SOCKET;
        mysql_options(mysqlInit, MYSQL_OPT_PROTOCOL, (char const*)&opt);
    }

    unsigned long flags = CLIENT_MULTI_STATEMENTS;

    //multiple execs for auto updater.
    //We might have to add multiple result set support later on for this.


    mMysql = mysql_real_connect(mysqlInit, m_host.c_str(), m_user.c_str(),
        m_password.c_str(), m_database.c_str(), m_port, nullptr, flags);

    if (mMysql)
    {
        m_connectionLost = false;
        DETAIL_LOG( "Connected to MySQL database at %s",
            m_host.c_str());
        DETAIL_LOG( "MySQL client library: %s", mysql_get_client_info());
        DETAIL_LOG( "MySQL server ver: %s ", mysql_get_server_info(mMysql));

        /*----------SET AUTOCOMMIT ON---------*/
        // It seems mysql 5.0.x have enabled this feature
        // by default. In crash case you can lose data!!!
        // So better to turn this off
        // ---
        // This is wrong since mangos use transactions,
        // autocommit is turned of during it.
        // Setting it to on makes atomic updates work
        // ---
        // LEAVE 'AUTOCOMMIT' MODE ALWAYS ENABLED!!!
        // W/O IT EVEN 'SELECT' QUERIES WOULD REQUIRE TO BE WRAPPED INTO 'START TRANSACTION'<>'COMMIT' CLAUSES!!!
        if (mysql_autocommit(mMysql, 1))
        {
            sLog.outError("Could not enable MySQL autocommit: %s", mysql_error(mMysql));
            CloseConnection();
            return false;
        }
        DETAIL_LOG("AUTOCOMMIT SUCCESSFULLY SET TO 1");
        /*-------------------------------------*/

        // set connection properties to UTF8 to properly handle locales for different
        // server configs - core sends data in UTF8, so MySQL must expect UTF8 too
        // Initialization must not use Execute's normal reconnect/retry path.
        if (!_TransactionCmd("SET NAMES `utf8`") || !_TransactionCmd("SET CHARACTER SET `utf8`"))
        {
            CloseConnection();
            return false;
        }

        return true;
    }
    else
    {
        sLog.outError( "Could not connect to MySQL database at %s: %s\n",
            m_host.c_str(), mysql_error(mysqlInit));
        mysql_close(mysqlInit);
        return false;
    }
}

bool MySQLConnection::Reconnect()
{
    if (m_transactionActive || m_migrationActive)
    {
        RecordError(0);
        return false;
    }

    sLog.outString("Reconnection attempt to database %s (on %s)", m_database.c_str(), m_host.c_str());

    // mysql_close invalidates statement handles. Destroy their owners first,
    // and only reconnect between calls (never from a prepared statement).
    CloseConnection();
    m_connectionLost = false;

    if (OpenConnection(true))
    {
        sLog.outString("Successfully reconnected to %s @%s:%u.",
            m_database.c_str(), m_host.c_str(), m_port);

        return true;
    }

    return false; // Failed to reconnect
}

bool MySQLConnection::HandleMySQLError(uint32 errNo)
{
    RecordError(errNo, true);
    if (m_transactionActive || m_migrationActive)
        return false;

    switch (errNo)
    {
        case CR_SERVER_GONE_ERROR:
        case CR_SERVER_LOST:
#if !(MARIADB_VERSION_ID >= 100200)
        case CR_INVALID_CONN_HANDLE:
#endif
        case CR_SERVER_LOST_EXTENDED:
        {
            return Reconnect();
        }

        case ER_LOCK_DEADLOCK:
            return false;
        // Query related errors - skip query
        case ER_WRONG_VALUE_COUNT:
        case ER_DUP_ENTRY:
            return false;

        // Outdated table or database structure - terminate core
        case ER_BAD_FIELD_ERROR:
        case ER_NO_SUCH_TABLE:
        {
            sLog.outErrorDb("Your database structure is not up to date. Please make sure you have executed all the queries in the sql/updates folders.");
            ASSERT(false);
            //return false; // Why return when ASSERT is called before anyways?
        }
        case ER_PARSE_ERROR:
        {
            sLog.outErrorDb("Error while parsing SQL. Core fix required.");
            ASSERT(false);
            //return false; // Why return when ASSERT is called before anyways?
        }
        default:
            sLog.outErrorDb("Unhandled MySQL errno %u. Unexpected behaviour possible.", errNo);
            return false;
    }
}

void MySQLConnection::RecordError(uint32 errNo, bool statementFailure)
{
    if (statementFailure && m_transactionActive && errNo == ER_LOCK_DEADLOCK)
        m_transactionKnownAbort = true;
    if (m_transactionActive)
        m_transactionFailed = true;
    if (m_migrationActive)
        m_migrationFailed = true;

    switch (errNo)
    {
        case CR_SERVER_GONE_ERROR:
        case CR_SERVER_LOST:
#if !(MARIADB_VERSION_ID >= 100200)
        case CR_INVALID_CONN_HANDLE:
#endif
        case CR_SERVER_LOST_EXTENDED:
            m_connectionLost = true;
            break;
        default:
            break;
    }
}

bool MySQLConnection::CheckTransaction()
{
    if (m_migrationActive && (m_migrationFailed || !mMysql || m_connectionLost ||
        mysql_thread_id(mMysql) != m_migrationThreadId))
    {
        m_migrationFailed = true;
        return false;
    }
    if (!m_transactionActive)
        return true;

    if (m_transactionFailed || !mMysql || m_connectionLost ||
        mysql_thread_id(mMysql) != m_transactionThreadId ||
        !(mMysql->server_status & SERVER_STATUS_IN_TRANS))
    {
        m_transactionFailed = true;
        return false;
    }
    return true;
}

bool MySQLConnection::EnsureConnection()
{
    if (!CheckTransaction())
        return false;
    return (mMysql && !m_connectionLost) || Reconnect();
}

bool MySQLConnection::CheckTransactionSql(const char* sql)
{
    if (!m_transactionActive)
        return true;
    if (!CheckTransaction())
        return false;
    if (IsTransactionalStatement(sql, (mMysql->server_status & SERVER_STATUS_NO_BACKSLASH_ESCAPES) != 0))
        return true;

    m_transactionFailed = true;
    sLog.outErrorDb("Unsafe or unsupported SQL rejected inside a transaction");
    return false;
}

bool MySQLConnection::_Query(const char *sql, MYSQL_RES **pResult, MYSQL_FIELD **pFields, uint64* pRowCount, uint32* pFieldCount)
{
    if (!EnsureConnection() || !CheckTransactionSql(sql))
    {
        return false;
    }
        

    uint32 _s = WorldTimer::getMSTime();

    if (mysql_query(mMysql, sql))
    {
        uint32 lErrno = mysql_errno(mMysql);

        sLog.outErrorDb( "SQL: %s", sql);
        sLog.outErrorDb("[%u] %s", lErrno, mysql_error(mMysql));

        if (HandleMySQLError(lErrno)) // If error is handled, just try again
            return _Query(sql, pResult, pFields, pRowCount, pFieldCount);

        return false;
    }
    else
    {
        BASIC_FILTER_LOG(LOG_FILTER_SQL_TEXT, "[%u ms] SQL: %s", WorldTimer::getMSTimeDiff(_s,WorldTimer::getMSTime()), sql );
    }

    *pResult = mysql_store_result(mMysql);
    if (mysql_errno(mMysql))
    {
        RecordError(mysql_errno(mMysql), true);
        if (*pResult)
            mysql_free_result(*pResult);
        return false;
    }
    if (!CheckTransaction())
    {
        if (*pResult)
            mysql_free_result(*pResult);
        return false;
    }
    *pRowCount = mysql_affected_rows(mMysql);
    *pFieldCount = mysql_field_count(mMysql);

    if (!*pResult )
        return false;

    if (!*pRowCount)
    {
        mysql_free_result(*pResult);
        return false;
    }

    *pFields = mysql_fetch_fields(*pResult);
    return true;
}

QueryResult* MySQLConnection::Query(const char *sql)
{
    MYSQL_RES *result = nullptr;
    MYSQL_FIELD *fields = nullptr;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;

    if(!_Query(sql,&result,&fields,&rowCount,&fieldCount))
        return nullptr;

    QueryResultMysql *queryResult = new QueryResultMysql(result, fields, rowCount, fieldCount);

    queryResult->NextRow();
    return queryResult;
}

QueryNamedResult* MySQLConnection::QueryNamed(const char *sql)
{
    MYSQL_RES *result = nullptr;
    MYSQL_FIELD *fields = nullptr;
    uint64 rowCount = 0;
    uint32 fieldCount = 0;

    if(!_Query(sql,&result,&fields,&rowCount,&fieldCount))
        return nullptr;

    QueryFieldNames names(fieldCount);
    for (uint32 i = 0; i < fieldCount; i++)
        names[i] = fields[i].name;

    QueryResultMysql *queryResult = new QueryResultMysql(result, fields, rowCount, fieldCount);

    queryResult->NextRow();
    return new QueryNamedResult(queryResult,names);
}

bool MySQLConnection::ExecuteMultiline(const char* sql)
{
    // A deadlock can roll back the transaction while later statements in the
    // same packet continue in autocommit mode. Queue individual statements.
    if (m_transactionActive)
    {
        m_transactionFailed = true;
        sLog.outErrorDb("Multiline requests cannot execute inside a transaction");
        return false;
    }
    if (!EnsureConnection())
        return false;

    uint32 _s = WorldTimer::getMSTime();

    if (mysql_query(mMysql, sql))
    {
        uint32 lErrno = mysql_errno(mMysql);

        std::string error = mysql_error(mMysql);
        sLog.outErrorDb("SQL: %s", sql);
        sLog.outErrorDb("[%u] %s", lErrno, error.c_str());

        if (HandleMySQLError(lErrno))
            return ExecuteMultiline(sql);
        sLog.out(LOG_AUTOUPDATER, "[FAIL] MySQL Error %u, message: %s.\nSQL: %s", lErrno, error.c_str(), sql);
        return false;
    }
    else
    {
        DEBUG_FILTER_LOG(LOG_FILTER_SQL_TEXT, "[%u ms] SQL: %s", WorldTimer::getMSTimeDiff(_s, WorldTimer::getMSTime()), sql);
    }

    //we have to drain the results from multiline queries otherwise the server will not be able to keep up.
    int status;
    do
    {
        MYSQL_RES* result = mysql_store_result(mMysql);
        if (result)
            mysql_free_result(result);
        else if (mysql_errno(mMysql))
        {
            RecordError(mysql_errno(mMysql));
            return false;
        }
        status = mysql_next_result(mMysql);
    }
    while (status == 0);
    if (status > 0)
    {
        RecordError(mysql_errno(mMysql));
        return false;
    }

    return CheckTransaction();
}

bool MySQLConnection::Execute(const char* sql)
{
    if (!EnsureConnection() || !CheckTransactionSql(sql))
        return false;

    uint32 _s = WorldTimer::getMSTime();

    if (mysql_query(mMysql, sql))
    {
        uint32 lErrno = mysql_errno(mMysql);

        sLog.outErrorDb( "SQL: %s", sql);
        sLog.outErrorDb("[%u] %s", lErrno, mysql_error(mMysql));

        if (HandleMySQLError(lErrno)) // If error is handled, just try again
            return Execute(sql);
        return false;
    }
    else
    {
        BASIC_FILTER_LOG(LOG_FILTER_SQL_TEXT, "[%u ms] SQL: %s", WorldTimer::getMSTimeDiff(_s,WorldTimer::getMSTime()), sql );
    }

    return CheckTransaction();
}

bool MySQLConnection::ExecuteStmt(int nIndex, const SqlStmtParameters& params)
{
    if (!EnsureConnection())
        return false;
    return SqlConnection::ExecuteStmt(nIndex, params);
}

bool MySQLConnection::_TransactionCmd(const char *sql)
{
    if (!mMysql || m_connectionLost)
        return false;
    if (mysql_query(mMysql, sql))
    {
        RecordError(mysql_errno(mMysql));
        sLog.outError("SQL: %s", sql);
        sLog.outError("SQL ERROR: %s", mysql_error(mMysql));
        return false;
    }
    else
    {
        DEBUG_FILTER_LOG(LOG_FILTER_SQL_TEXT, "SQL: %s", sql);
    }
    return true;
}

bool MySQLConnection::BeginTransaction()
{
    m_transactionKnownAbort = false;
    // Nested START TRANSACTION would commit the outer transaction in MySQL.
    if (m_transactionActive || m_migrationActive)
    {
        RecordError(0);
        return false;
    }
    if (!EnsureConnection() || !_TransactionCmd("START TRANSACTION"))
        return false;

    m_transactionActive = true;
    m_transactionFailed = false;
    m_transactionThreadId = mysql_thread_id(mMysql);
    if (!CheckTransaction())
    {
        RollbackTransaction();
        return false;
    }
    return true;
}

bool MySQLConnection::CommitTransaction()
{
    // A COMMIT failure never proves that it is safe to replay the transaction.
    m_transactionKnownAbort = false;
    if (!m_transactionActive)
        return false;
    if (!CheckTransaction())
    {
        RollbackTransaction();
        return false;
    }

    bool committed = _TransactionCmd("COMMIT");
    if (committed && (mysql_thread_id(mMysql) != m_transactionThreadId ||
        (mMysql->server_status & SERVER_STATUS_IN_TRANS)))
        committed = false;
    // A failed COMMIT has an unknown outcome. Never replay it, and discard
    // the session before accepting unrelated work.
    if (!committed)
        m_connectionLost = true;
    m_transactionActive = false;
    m_transactionFailed = false;
    return committed;
}

bool MySQLConnection::RollbackTransaction()
{
    if (!m_transactionActive)
        return false;
    bool rolledBack = _TransactionCmd("ROLLBACK");
    if (rolledBack && (mysql_thread_id(mMysql) != m_transactionThreadId ||
        (mMysql->server_status & SERVER_STATUS_IN_TRANS)))
        rolledBack = false;
    if (!rolledBack)
        m_connectionLost = true;
    m_transactionActive = false;
    m_transactionFailed = false;
    m_transactionKnownAbort = false;
    return rolledBack;
}

bool MySQLConnection::CanRetryTransaction() const
{
    return m_transactionActive && m_transactionKnownAbort && !m_connectionLost &&
        mMysql && mysql_thread_id(mMysql) == m_transactionThreadId;
}

bool MySQLConnection::ExecuteMigrationBatch(const std::vector<std::string>& statements)
{
    SqlConnection::Lock guard(this);
    if (m_transactionActive || m_migrationActive)
    {
        RecordError(0);
        return false;
    }
    if (statements.empty())
        return true;
    if (!EnsureConnection())
        return false;

    // Startup migrations explicitly allow MySQL DDL's implicit commits. Pin
    // the session so a failure can never replay a partly applied migration.
    m_migrationActive = true;
    m_migrationFailed = false;
    m_migrationThreadId = mysql_thread_id(mMysql);
    bool succeeded = _TransactionCmd("START TRANSACTION") && CheckTransaction();
    for (const std::string& sql : statements)
    {
        if (!succeeded)
            break;
        // A preceding DDL/control statement may have committed the current
        // segment. Keep subsequent DML, especially the final ledger INSERT,
        // pending until an explicit commit rather than silently autocommitting.
        if (!(mMysql->server_status & SERVER_STATUS_IN_TRANS))
            succeeded = _TransactionCmd("START TRANSACTION") && CheckTransaction();
        if (!succeeded)
            break;
        succeeded = Execute(sql.c_str());
    }
    if (succeeded)
        succeeded = _TransactionCmd("COMMIT") && CheckTransaction() &&
            !(mMysql->server_status & SERVER_STATUS_IN_TRANS);
    if (!succeeded && mMysql && !m_connectionLost &&
        mysql_thread_id(mMysql) == m_migrationThreadId)
        _TransactionCmd("ROLLBACK");

    // DDL may already be durable on failure. Always discard session settings
    // (including SET/autocommit changes) before unrelated runtime work.
    CloseConnection();
    m_migrationActive = false;
    m_migrationFailed = false;
    m_connectionLost = false;
    return succeeded;
}

unsigned long MySQLConnection::escape_string(char *to, const char *from, unsigned long length)
{
    if (!mMysql || !to || !from || !length)
        return 0;

    return mysql_real_escape_string(mMysql, to, from, length);
}

//////////////////////////////////////////////////////////////////////////
SqlPreparedStatement * MySQLConnection::CreateStatement( const std::string& fmt )
{
    return new MySqlPreparedStatement(fmt, *this, mMysql);
}


//////////////////////////////////////////////////////////////////////////
MySqlPreparedStatement::MySqlPreparedStatement( const std::string& fmt, SqlConnection& conn, MYSQL * mysql ) : SqlPreparedStatement(fmt, conn),
    m_pMySQLConn(mysql), m_stmt(nullptr), m_pInputArgs(nullptr), m_pResult(nullptr), m_pResultMetadata(nullptr), m_bindFailed(false)
{
}

MySqlPreparedStatement::~MySqlPreparedStatement()
{
    RemoveBinds();
}

bool MySqlPreparedStatement::prepare()
{
    if (!static_cast<MySQLConnection&>(m_pConn).CheckTransactionSql(m_szFmt.c_str()))
        return false;
    if(isPrepared())
        return true;

    //remove old binds
    RemoveBinds();

    //create statement object
    m_stmt = mysql_stmt_init(m_pMySQLConn);
    if (!m_stmt)
    {
        static_cast<MySQLConnection&>(m_pConn).RecordError(mysql_errno(m_pMySQLConn));
        sLog.outError("SQL: mysql_stmt_init()() failed ");
        return false;
    }

    //prepare statement
    if (mysql_stmt_prepare(m_stmt, m_szFmt.c_str(), m_szFmt.length()))
    {
        static_cast<MySQLConnection&>(m_pConn).RecordError(mysql_stmt_errno(m_stmt));
        sLog.outError("SQL: mysql_stmt_prepare() failed for '%s'", m_szFmt.c_str());
        sLog.outError("SQL ERROR: %s", mysql_stmt_error(m_stmt));
        return false;
    }

    /* Get the parameter count from the statement */
    m_nParams = mysql_stmt_param_count(m_stmt);

    /* Fetch result set meta information */
    m_pResultMetadata = mysql_stmt_result_metadata(m_stmt);
    //if we do not have result metadata
    if (!m_pResultMetadata && strnicmp(m_szFmt.c_str(), "select", 6) == 0)
    {
        static_cast<MySQLConnection&>(m_pConn).RecordError(mysql_stmt_errno(m_stmt));
        sLog.outError("SQL: no meta information for '%s'", m_szFmt.c_str());
        sLog.outError("SQL ERROR: %s", mysql_stmt_error(m_stmt));
        return false;
    }

    //bind input buffers
    if(m_nParams)
    {
        m_pInputArgs = new MYSQL_BIND[m_nParams];
        memset(m_pInputArgs, 0, sizeof(MYSQL_BIND) * m_nParams);
    }

    //check if we have a statement which returns result sets
    if(m_pResultMetadata)
    {
        //our statement is query
        m_bIsQuery = true;
        /* Get total columns in the query */
        m_nColumns = mysql_num_fields(m_pResultMetadata);

        //bind output buffers
    }

    m_bPrepared = true;
    return true;
}

void MySqlPreparedStatement::bind( const SqlStmtParameters& holder )
{
    m_bindFailed = false;
    if(!isPrepared())
    {
        MANGOS_ASSERT(false);
        return;
    }

    //finalize adding params
    if(!m_pInputArgs)
        return;

    //verify if we bound all needed input parameters
    if(m_nParams != holder.boundParams())
    {
        MANGOS_ASSERT(false);
        return;
    }

    int nIndex = 0;
    SqlStmtParameters::ParameterContainer const& _args = holder.params();

    SqlStmtParameters::ParameterContainer::const_iterator iter_last = _args.end();
    for (SqlStmtParameters::ParameterContainer::const_iterator iter = _args.begin(); iter != iter_last; ++iter)
    {
        //bind parameter
        addParam(nIndex++, (*iter));
    }

    //bind input arguments
    if (mysql_stmt_bind_param(m_stmt, m_pInputArgs))
    {
        m_bindFailed = true;
        static_cast<MySQLConnection&>(m_pConn).RecordError(mysql_stmt_errno(m_stmt));
        sLog.outError("SQL ERROR: mysql_stmt_bind_param() failed\n");
        sLog.outError("SQL ERROR: %s", mysql_stmt_error(m_stmt));
    }
}

void MySqlPreparedStatement::addParam( int nIndex, const SqlStmtFieldData& data )
{
    MANGOS_ASSERT(m_pInputArgs);
    MANGOS_ASSERT(nIndex < static_cast <int32> (m_nParams));

    MYSQL_BIND& pData = m_pInputArgs[nIndex];

    my_bool bUnsigned = 0;
    enum_field_types dataType = ToMySQLType(data, bUnsigned);

    //setup MYSQL_BIND structure
    pData.buffer_type = dataType;
    pData.is_unsigned = bUnsigned;
    pData.buffer = data.buff();
    pData.length = 0;
    pData.buffer_length = data.type() == FIELD_STRING ? data.size() : 0;
}

void MySqlPreparedStatement::RemoveBinds()
{
    if(!m_stmt)
        return;

    delete [] m_pInputArgs;
    delete [] m_pResult;

    mysql_free_result(m_pResultMetadata);
    mysql_stmt_close(m_stmt);

    m_stmt = nullptr;
    m_pResultMetadata = nullptr;
    m_pResult = nullptr;
    m_pInputArgs = nullptr;

    m_bPrepared = false;
}

bool MySqlPreparedStatement::execute()
{
    MySQLConnection& conn = static_cast<MySQLConnection&>(m_pConn);
    if(!isPrepared() || m_bindFailed || !conn.CheckTransactionSql(m_szFmt.c_str()))
        return false;

    if(mysql_stmt_execute(m_stmt))
    {
        // Recovery is deferred to the next connection operation: reconnecting
        // here would delete this cached statement while it is executing.
        conn.RecordError(mysql_stmt_errno(m_stmt), true);
        sLog.outError("SQL: cannot execute '%s'", m_szFmt.c_str());
        sLog.outError("SQL ERROR: %s", mysql_stmt_error(m_stmt));
        return false;
    }

    return conn.CheckTransaction();
}

enum_field_types MySqlPreparedStatement::ToMySQLType( const SqlStmtFieldData &data, my_bool &bUnsigned )
{
    bUnsigned = 0;
    enum_field_types dataType = MYSQL_TYPE_NULL;

    switch (data.type())
    {
    case FIELD_NONE:    dataType = MYSQL_TYPE_NULL;                     break;
    case FIELD_BOOL:    dataType = MYSQL_TYPE_BIT;      bUnsigned = 1;  break;
    case FIELD_I8:      dataType = MYSQL_TYPE_TINY;                     break;
    case FIELD_UI8:     dataType = MYSQL_TYPE_TINY;     bUnsigned = 1;  break;
    case FIELD_I16:     dataType = MYSQL_TYPE_SHORT;                    break;
    case FIELD_UI16:    dataType = MYSQL_TYPE_SHORT;    bUnsigned = 1;  break;
    case FIELD_I32:     dataType = MYSQL_TYPE_LONG;                     break;
    case FIELD_UI32:    dataType = MYSQL_TYPE_LONG;     bUnsigned = 1;  break;
    case FIELD_I64:     dataType = MYSQL_TYPE_LONGLONG;                 break;
    case FIELD_UI64:    dataType = MYSQL_TYPE_LONGLONG; bUnsigned = 1;  break;
    case FIELD_FLOAT:   dataType = MYSQL_TYPE_FLOAT;                    break;
    case FIELD_DOUBLE:  dataType = MYSQL_TYPE_DOUBLE;                   break;
    case FIELD_STRING:  dataType = MYSQL_TYPE_STRING;                   break;
    }

    return dataType;
}
#endif
