#pragma once
// Replace unrelated server infrastructure; exercise the real MySQL implementation
// and real prepared-statement declarations against a deterministic C API below.
#include <winsock2.h>
#include <mysql.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <cstdarg>
#include <functional>
#include <optional>
#include <mutex>
#define MANGOSSERVER_COMMON_H
#define DATABASE_H
#define DATABASEENV_H
#define MANGOS_ASSERT(x) assert(x)
#define ASSERT(x) assert(x)
#define DETAIL_LOG(...) ((void)0)
#define BASIC_FILTER_LOG(...) ((void)0)
#define DEBUG_FILTER_LOG(...) ((void)0)
#define LOG_AUTOUPDATER 0
#define LOG_PERFORMANCE 0
#define SIZEFMTD "%zu"
#define ATTR_PRINTF(...)
#define MAX_QUERY_LEN (32*1024)
#define __SQLDELAYTHREAD_H
#define strnicmp _strnicmp
using uint8 = uint8_t; using int8 = int8_t;
using uint16 = uint16_t; using int16 = int16_t;
using uint32 = uint32_t; using int32 = int32_t;
using uint64 = uint64_t; using int64 = int64_t;
#include "../../../src/shared/Database/SqlPreparedStatement.h"
struct Log {
    template<class... T> void outError(T...) {}
    template<class... T> void outErrorDb(T...) {}
    template<class... T> void outString(T...) {}
    template<class... T> void out(T...) {}
    static void WaitBeforeContinueIfNeed() {}
};
inline Log sLog;
class SqlConnection;
class SqlOperation;
inline char* mangos_strdup(const char* text) {
    auto p = new char[std::strlen(text) + 1]; std::strcpy(p,text); return p;
}
class Database {
public:
    virtual ~Database() = default;
    virtual void ThreadStart() {}
    virtual void ThreadEnd() {}
    void StopServer() {}
    void AddToSerialDelayQueue(SqlOperation*) {}
    std::string statement = "UPDATE storage_test SET value=?";
protected:
    virtual SqlConnection* CreateConnection() { return nullptr; }
};
class QueryResult {
public:
    virtual ~QueryResult() = default;
    void NextRow() {}
};
class QueryResultMysql : public QueryResult {
public:
    QueryResultMysql(MYSQL_RES*, MYSQL_FIELD*, uint64, uint32) {}
};
using QueryFieldNames = std::vector<std::string>;
class QueryNamedResult {
public:
    QueryNamedResult(QueryResult*, const QueryFieldNames&) {}
};
class SqlConnection {
public:
    explicit SqlConnection(Database& db) : m_db(db) {}
    Database& DB() { return m_db; }
    virtual ~SqlConnection() = default;
    virtual bool OpenConnection(bool) = 0;
    virtual QueryResult* Query(const char*) = 0;
    virtual QueryNamedResult* QueryNamed(const char*) = 0;
    virtual bool Execute(const char*) = 0;
    virtual bool ExecuteMultiline(const char*) = 0;
    virtual unsigned long escape_string(char*,const char*,unsigned long) = 0;
    virtual bool BeginTransaction() = 0;
    virtual bool CommitTransaction() = 0;
    virtual bool RollbackTransaction() = 0;
    virtual bool CanRetryTransaction() const { return false; }
    virtual bool ExecuteMigrationBatch(const std::vector<std::string>&) { return false; }
    virtual bool ExecuteStmt(int index, const SqlStmtParameters& params) {
        if (!statements.count(index)) {
            std::unique_ptr<SqlPreparedStatement> stmt(CreateStatement(m_db.statement));
            if (!stmt->prepare()) return false;
            statements[index] = std::move(stmt);
        }
        auto& stmt = statements[index];
        stmt->bind(params);
        return stmt->execute();
    }
    class Lock { public: explicit Lock(SqlConnection*) {} };
protected:
    virtual SqlPreparedStatement* CreateStatement(const std::string&) = 0;
    void FreePreparedStatements() { statements.clear(); }
    Database& m_db;
    std::string m_host = "test", m_user, m_password, m_database;
    int m_port = 0;
    bool m_use_socket = false;
private:
    std::map<int,std::unique_ptr<SqlPreparedStatement>> statements;
};
inline SqlStmtParameters::SqlStmtParameters(int count) { m_params.reserve(count); }
