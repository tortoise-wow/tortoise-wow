// Compile this translation unit, not a copied model of the transaction code.
#include "stubs/TestEnvironment.h"
#include "../../src/shared/Database/DatabaseMysql.h"
#include "../../src/shared/Database/DatabaseMysql.cpp"
// DatabaseImpl supplies unrelated asynchronous templates; all transaction and
// operation declarations/definitions below are the unmodified production files.
#pragma include_alias("DatabaseImpl.h", "TestDatabaseImpl.h")
#include "../../src/shared/Database/SqlOperations.cpp"
#include <algorithm>
#include <iostream>

namespace Fake {
struct Failure { std::string sql; unsigned error; };
std::vector<Failure> failures;
std::vector<std::string> queries;
std::map<MYSQL*, unsigned> errors;
std::map<MYSQL_STMT*, MYSQL*> statements;
MYSQL* current = nullptr;
unsigned long sessions = 0;
unsigned executeError = 0, bindError = 0, prepareError = 0;
unsigned autocommitError = 0;
unsigned nextResultError = 0;
bool loseTransactionOnExecute = false;
bool changeThreadOnCommit = false, startHasTransaction = true;
bool ledgerObservedTransaction = false;
bool disableReconnectSeen = false;
unsigned stmtExecutions = 0, stmtPrepares = 0, stmtCloses = 0, connections = 0;
bool inStatementExecute = false;
unsigned consume(std::string const& sql) {
    queries.push_back(sql);
    if (failures.empty() || failures.front().sql != sql) return 0;
    unsigned error = failures.front().error;
    failures.erase(failures.begin());
    return error;
}
void reset() {
    assert(statements.empty()); assert(errors.empty());
    failures.clear(); queries.clear(); current = nullptr;
    executeError = bindError = prepareError = nextResultError = 0;
    autocommitError = 0;
    loseTransactionOnExecute = false; disableReconnectSeen = false;
    changeThreadOnCommit=false; startHasTransaction=true;
    ledgerObservedTransaction=false;
    stmtExecutions = stmtPrepares = stmtCloses = connections = 0;
}
unsigned count(std::string const& sql) {
    return unsigned(std::count(queries.begin(),queries.end(),sql));
}
}
extern "C" {
int STDCALL mysql_server_init(int,char**,char**) { return 0; }
void STDCALL mysql_server_end() {}
unsigned int STDCALL mysql_thread_safe() { return 1; }
my_bool STDCALL mysql_thread_init() { return 0; }
void STDCALL mysql_thread_end() {}
MYSQL* STDCALL mysql_init(MYSQL*) { auto p = new MYSQL{}; Fake::errors[p]=0; return p; }
MYSQL* STDCALL mysql_real_connect(MYSQL* p,const char*,const char*,const char*,const char*,unsigned int,const char*,unsigned long) {
    p->thread_id=++Fake::sessions; Fake::current=p; ++Fake::connections; return p;
}
void STDCALL mysql_close(MYSQL* p) {
    assert(!Fake::inStatementExecute);
    for (auto const& item : Fake::statements) assert(item.second != p);
    Fake::errors.erase(p);
    if (Fake::current == p) Fake::current = nullptr;
    delete p;
}
int STDCALL mysql_options(MYSQL*,enum mysql_option option,const void* value) {
    if (option==MYSQL_OPT_RECONNECT) { assert(!*static_cast<const my_bool*>(value)); Fake::disableReconnectSeen=true; }
    return 0;
}
my_bool STDCALL mysql_autocommit(MYSQL* p,my_bool) {
    Fake::errors[p]=Fake::autocommitError; Fake::autocommitError=0;
    return Fake::errors[p] ? 1 : 0;
}
const char* STDCALL mysql_get_client_info() { return "fake"; }
const char* STDCALL mysql_get_server_info(MYSQL*) { return "fake"; }
int STDCALL mysql_query(MYSQL* p,const char* text) {
    unsigned error=Fake::consume(text); Fake::errors[p]=error;
    if (error) return 1;
    if (strcmp(text,"START TRANSACTION")==0 && Fake::startHasTransaction) p->server_status |= SERVER_STATUS_IN_TRANS;
    if (strcmp(text,"COMMIT")==0 || strcmp(text,"ROLLBACK")==0) p->server_status &= ~SERVER_STATUS_IN_TRANS;
    if (strncmp(text,"CREATE ",7)==0 || strncmp(text,"ALTER ",6)==0 || strncmp(text,"DROP ",5)==0)
        p->server_status &= ~SERVER_STATUS_IN_TRANS;
    if (strcmp(text,"COMMIT")==0 && Fake::changeThreadOnCommit) ++p->thread_id;
    if (Fake::loseTransactionOnExecute && strcmp(text,"UPDATE test")==0) p->server_status &= ~SERVER_STATUS_IN_TRANS;
    if (strncmp(text,"INSERT INTO ledger",18)==0)
        Fake::ledgerObservedTransaction = (p->server_status & SERVER_STATUS_IN_TRANS) != 0;
    return 0;
}
unsigned int STDCALL mysql_errno(MYSQL* p) { return Fake::errors[p]; }
const char* STDCALL mysql_error(MYSQL*) { return "injected"; }
unsigned long STDCALL mysql_thread_id(MYSQL* p) { return p->thread_id; }
MYSQL_RES* STDCALL mysql_store_result(MYSQL*) { return nullptr; }
void STDCALL mysql_free_result(MYSQL_RES*) {}
my_ulonglong STDCALL mysql_affected_rows(MYSQL*) { return 0; }
unsigned int STDCALL mysql_field_count(MYSQL*) { return 0; }
MYSQL_FIELD* STDCALL mysql_fetch_fields(MYSQL_RES*) { return nullptr; }
int STDCALL mysql_next_result(MYSQL* p) {
    if (!Fake::nextResultError) return -1;
    Fake::errors[p]=Fake::nextResultError; Fake::nextResultError=0; return 1;
}
unsigned long STDCALL mysql_real_escape_string(MYSQL*,char*,const char*,unsigned long) { return 0; }
MYSQL_STMT* STDCALL mysql_stmt_init(MYSQL* p) {
    auto stmt = new MYSQL_STMT{}; Fake::statements[stmt]=p; return stmt;
}
int STDCALL mysql_stmt_prepare(MYSQL_STMT* p,const char*,unsigned long) {
    ++Fake::stmtPrepares;
    p->last_errno=Fake::prepareError; Fake::prepareError=0; return p->last_errno ? 1 : 0;
}
unsigned long STDCALL mysql_stmt_param_count(MYSQL_STMT*) { return 1; }
MYSQL_RES* STDCALL mysql_stmt_result_metadata(MYSQL_STMT*) { return nullptr; }
unsigned int STDCALL mysql_num_fields(MYSQL_RES*) { return 0; }
my_bool STDCALL mysql_stmt_bind_param(MYSQL_STMT* p,MYSQL_BIND*) {
    p->last_errno=Fake::bindError; Fake::bindError=0; return p->last_errno ? 1 : 0;
}
my_bool STDCALL mysql_stmt_close(MYSQL_STMT* p) {
    assert(!Fake::inStatementExecute); ++Fake::stmtCloses;
    assert(Fake::statements.erase(p)==1); delete p; return 0;
}
int STDCALL mysql_stmt_execute(MYSQL_STMT* p) {
    Fake::inStatementExecute=true; ++Fake::stmtExecutions;
    p->last_errno=Fake::executeError; Fake::executeError=0;
    Fake::inStatementExecute=false;
    return p->last_errno ? 1 : 0;
}
unsigned int STDCALL mysql_stmt_errno(MYSQL_STMT* p) { return p->last_errno; }
const char* STDCALL mysql_stmt_error(MYSQL_STMT*) { return "injected"; }
}

#define CHECK(x) do { if (!(x)) throw std::runtime_error(#x); } while (0)
int main() {
    std::cout << std::unitbuf;
    unsigned passed=0;
    {
        Fake::reset(); Database db; MySQLConnection conn(db);
        Fake::autocommitError=CR_SERVER_LOST;
        CHECK(!conn.OpenConnection(false)); CHECK(Fake::connections==1); CHECK(Fake::errors.empty());
        CHECK(Fake::count("SET NAMES `utf8`")==0); CHECK(Fake::count("SET CHARACTER SET `utf8`")==0);
        CHECK(conn.OpenConnection(false)); CHECK(Fake::connections==2);
        ++passed; std::cout << "PASS autocommit initialization failure closes session without retry\n";
    }
    for (const char* sql : {"SET NAMES `utf8`", "SET CHARACTER SET `utf8`"})
    {
        Fake::reset(); Database db; MySQLConnection conn(db);
        Fake::failures.push_back({sql,CR_SERVER_LOST});
        CHECK(!conn.OpenConnection(false)); CHECK(Fake::connections==1); CHECK(Fake::errors.empty());
        CHECK(Fake::count(sql)==1);
        if (strcmp(sql,"SET NAMES `utf8`")==0) CHECK(Fake::count("SET CHARACTER SET `utf8`")==0);
        CHECK(conn.OpenConnection(false)); CHECK(Fake::connections==2);
        ++passed; std::cout << "PASS charset initialization failure closes session without retry: " << sql << '\n';
    }
    auto run=[&](const char* name, auto body) {
        Fake::reset();
        { Database db; MySQLConnection conn(db); CHECK(conn.OpenConnection(false)); CHECK(Fake::disableReconnectSeen); body(conn); }
        ++passed; std::cout << "PASS " << name << '\n';
    };
    run("normal commit",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); CHECK(c.Execute("UPDATE test")); CHECK(c.CommitTransaction());
        CHECK(Fake::count("COMMIT")==1);
    });
    run("SqlTransaction failed BEGIN invokes no queued statement",[](MySQLConnection& c) {
        SqlTransaction transaction(0);
        transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE second"));
        Fake::failures.push_back({"START TRANSACTION",CR_SERVER_LOST});
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("UPDATE first")==0);
        CHECK(Fake::count("UPDATE second")==0); CHECK(Fake::count("COMMIT")==0);
    });
    run("SqlTransaction stops and rolls back after statement failure",[](MySQLConnection& c) {
        SqlTransaction transaction(0);
        transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE second"));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE third"));
        Fake::failures.push_back({"UPDATE second",ER_LOCK_DEADLOCK});
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("UPDATE first")==1);
        CHECK(Fake::count("UPDATE second")==1); CHECK(Fake::count("UPDATE third")==0);
        CHECK(Fake::count("ROLLBACK")==1); CHECK(Fake::count("COMMIT")==0);
    });
    run("SqlTransaction propagates COMMIT failure",[](MySQLConnection& c) {
        SqlTransaction transaction(0);
        transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        Fake::failures.push_back({"COMMIT",CR_SERVER_LOST});
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("UPDATE first")==1);
        CHECK(Fake::count("COMMIT")==1); CHECK(Fake::connections==1);
    });
    run("empty SqlTransaction executes nothing",[](MySQLConnection& c) {
        SqlTransaction transaction(0); CHECK(transaction.Execute(&c));
        CHECK(Fake::count("START TRANSACTION")==0); CHECK(Fake::count("COMMIT")==0);
    });
    run("SqlTransaction prepared loss stops before next operation",[](MySQLConnection& c) {
        SqlTransaction transaction(0);
        auto params = new SqlStmtParameters(1); params->addParam(SqlStmtFieldData(uint32(7)));
        transaction.DelayExecute(new SqlPreparedRequest(0,params));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE second"));
        Fake::executeError=CR_SERVER_LOST;
        CHECK(!transaction.Execute(&c)); CHECK(Fake::stmtExecutions==1);
        CHECK(Fake::count("UPDATE second")==0); CHECK(Fake::count("COMMIT")==0); CHECK(Fake::connections==1);
    });
    run("BEGIN failure does not establish transaction",[](MySQLConnection& c) {
        Fake::failures.push_back({"START TRANSACTION",CR_SERVER_LOST});
        CHECK(!c.BeginTransaction()); CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==0);
    });
    run("BEGIN with no server transaction cleans up its state",[](MySQLConnection& c) {
        Fake::startHasTransaction=false; CHECK(!c.BeginTransaction());
        Fake::startHasTransaction=true; CHECK(c.BeginTransaction()); CHECK(c.CommitTransaction());
    });
    run("plain transaction loss is never replayed",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::failures.push_back({"UPDATE test",CR_SERVER_LOST});
        CHECK(!c.Execute("UPDATE test")); CHECK(!c.Execute("UPDATE second")); CHECK(!c.CommitTransaction());
        CHECK(Fake::connections==1); CHECK(Fake::count("UPDATE test")==1); CHECK(Fake::count("UPDATE second")==0); CHECK(Fake::count("COMMIT")==0);
        CHECK(c.BeginTransaction()); CHECK(Fake::connections==2); CHECK(c.CommitTransaction());
    });
    run("statement error poisons transaction",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::failures.push_back({"UPDATE test",ER_LOCK_DEADLOCK});
        CHECK(!c.Execute("UPDATE test")); CHECK(!c.Execute("UPDATE second")); CHECK(!c.CommitTransaction());
        CHECK(Fake::count("ROLLBACK")==1); CHECK(Fake::count("COMMIT")==0);
    });
    run("failed COMMIT is never retried",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); CHECK(c.Execute("UPDATE test")); Fake::failures.push_back({"COMMIT",CR_SERVER_LOST});
        CHECK(!c.CommitTransaction()); CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==1); CHECK(Fake::connections==1);
        CHECK(c.Execute("UPDATE independent")); CHECK(Fake::connections==2); CHECK(Fake::count("COMMIT")==1);
    });
    run("changed session cannot commit",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); ++Fake::current->thread_id;
        CHECK(!c.Execute("UPDATE test")); CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==0);
    });
    run("changed session during COMMIT cannot report success",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::changeThreadOnCommit=true;
        CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==1);
        Fake::changeThreadOnCommit=false; CHECK(c.Execute("UPDATE independent")); CHECK(Fake::connections==2);
    });
    run("server rollback cannot report success",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::loseTransactionOnExecute=true;
        CHECK(!c.Execute("UPDATE test")); CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==0);
    });
    run("nested BEGIN cannot implicitly commit",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); CHECK(!c.BeginTransaction()); CHECK(!c.CommitTransaction()); CHECK(Fake::count("START TRANSACTION")==1);
    });
    run("outside transaction reconnect remains compatible",[](MySQLConnection& c) {
        Fake::failures.push_back({"UPDATE test",CR_SERVER_GONE_ERROR}); CHECK(c.Execute("UPDATE test"));
        CHECK(Fake::connections==2); CHECK(Fake::count("UPDATE test")==2);
    });
    run("failed rollback discards session before later work",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::failures.push_back({"ROLLBACK",CR_SERVER_LOST});
        CHECK(!c.RollbackTransaction()); CHECK(c.Execute("UPDATE independent")); CHECK(Fake::connections==2);
    });
    run("query loss inside transaction never reconnects or replays",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::failures.push_back({"SELECT test",CR_SERVER_LOST});
        CHECK(!c.Query("SELECT test")); CHECK(Fake::count("SELECT test")==1);
        CHECK(Fake::connections==1); CHECK(!c.CommitTransaction());
    });
    run("prepared loss does not reconnect inside execute",[](MySQLConnection& c) {
        SqlStmtParameters params(1); params.addParam(SqlStmtFieldData(uint32(7)));
        CHECK(c.BeginTransaction()); Fake::executeError=CR_SERVER_LOST;
        CHECK(!c.ExecuteStmt(0,params)); CHECK(Fake::connections==1); CHECK(Fake::stmtExecutions==1); CHECK(Fake::stmtCloses==0);
        CHECK(!c.CommitTransaction()); CHECK(c.ExecuteStmt(0,params));
        CHECK(Fake::connections==2); CHECK(Fake::stmtCloses==1); CHECK(Fake::stmtExecutions==2);
    });
    run("prepared bind failure never executes stale parameters",[](MySQLConnection& c) {
        SqlStmtParameters params(1); params.addParam(SqlStmtFieldData(uint32(7)));
        CHECK(c.BeginTransaction()); Fake::bindError=ER_WRONG_VALUE_COUNT;
        CHECK(!c.ExecuteStmt(0,params)); CHECK(Fake::stmtExecutions==0); CHECK(!c.CommitTransaction());
    });
    run("prepared prepare failure poisons transaction",[](MySQLConnection& c) {
        SqlStmtParameters params(1); params.addParam(SqlStmtFieldData(uint32(7)));
        CHECK(c.BeginTransaction()); Fake::prepareError=ER_WRONG_VALUE_COUNT;
        CHECK(!c.ExecuteStmt(0,params)); CHECK(!c.CommitTransaction()); CHECK(Fake::stmtExecutions==0);
    });
    run("multiline later-result failure propagates",[](MySQLConnection& c) {
        Fake::nextResultError=ER_LOCK_DEADLOCK;
        CHECK(!c.ExecuteMultiline("UPDATE test; UPDATE second"));
    });
    run("multiline transaction packet is refused before execution",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction());
        CHECK(!c.ExecuteMultiline("UPDATE test; UPDATE second")); CHECK(!c.CommitTransaction());
        CHECK(Fake::count("UPDATE test; UPDATE second")==0);
    });
    for (const char* sql : {"CREATE TABLE forbidden(id INT)", "ALTER TABLE test ADD value INT",
        "DROP TABLE test", "TRUNCATE TABLE test", "START TRANSACTION", "BEGIN", "COMMIT", "ROLLBACK",
        "SET autocommit=1", "CALL forbidden()", "LOCK TABLES test WRITE", "UNLOCK TABLES",
        "/* prefix */ COMMIT", "-- prefix\nCOMMIT", "# prefix\nCOMMIT",
        "/*! COMMIT */", "/*M! COMMIT */", "UPDATE test /*! SET value=1 */",
        "UPDATE test; COMMIT; UPDATE second", "UPDATE test; /* trailing */ COMMIT",
        "SELECT 1; -- trailing\nCOMMIT", "SELECT 1; # trailing\nCOMMIT", "UPDATE test;;",
        "UPDATE test SET value='unterminated", "UPDATE test /* unclosed", "UPDATE test /* nested /* */",
        "UPDATE test SET value=\"ambiguous\\\"; COMMIT; --\""})
    {
        run((std::string("reject transaction SQL: ") + sql).c_str(),[&](MySQLConnection& c) {
            CHECK(c.BeginTransaction()); CHECK(c.Execute("UPDATE prior"));
            unsigned before = Fake::count(sql);
            CHECK(!c.Execute(sql)); CHECK(Fake::count(sql)==before); CHECK(!c.CommitTransaction());
            CHECK(Fake::count("COMMIT")==0); CHECK(Fake::count("ROLLBACK")==1);
        });
    }
    for (const char* sql : {"SELECT ';'", "INSERT INTO test VALUES ('a;b')", "UPDATE test SET value='it\\'s;fine'",
        "DELETE FROM test WHERE value='a'';b'", "REPLACE INTO test VALUES (1);",
        "/* prefix ; COMMIT */ uPdAtE `a;``b` SET value='/*x*/;#--'; /* trailing */",
        "-- leading\nSELECT \"a;\"; # trailing", "SELECT 1 -- ; COMMIT\n;", "SELECT 1 # ; COMMIT\n;"})
    {
        run("single DML or SELECT with quoted semicolons remains allowed",[&](MySQLConnection& c) {
            CHECK(c.BeginTransaction()); CHECK(c.Execute(sql)); CHECK(Fake::count(sql)==1); CHECK(c.CommitTransaction());
        });
    }
    run("no-backslash SQL mode cannot hide a second statement",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::current->server_status |= SERVER_STATUS_NO_BACKSLASH_ESCAPES;
        const char* sql="UPDATE test SET value='x\\'; COMMIT; -- '";
        CHECK(!c.Execute(sql)); CHECK(Fake::count(sql)==0); CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==0);
    });
    run("no-backslash SQL mode allows literal backslashes",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::current->server_status |= SERVER_STATUS_NO_BACKSLASH_ESCAPES;
        CHECK(c.Execute("UPDATE test SET value='x\\';")); CHECK(c.CommitTransaction());
    });
    run("query entry cannot execute hidden transaction control",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); CHECK(!c.Query("SELECT 1; COMMIT"));
        CHECK(Fake::count("SELECT 1; COMMIT")==0); CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==0);
    });
    run("unsafe prepared format is refused before prepare or execute",[](MySQLConnection& c) {
        SqlStmtParameters params(1); params.addParam(SqlStmtFieldData(uint32(7)));
        c.DB().statement="COMMIT";
        CHECK(c.BeginTransaction()); CHECK(!c.ExecuteStmt(0,params));
        CHECK(Fake::stmtPrepares==0); CHECK(Fake::stmtExecutions==0); CHECK(!c.CommitTransaction());
    });
    run("cached prepared control cannot execute inside transaction",[](MySQLConnection& c) {
        SqlStmtParameters params(1); params.addParam(SqlStmtFieldData(uint32(7)));
        c.DB().statement="COMMIT"; CHECK(c.ExecuteStmt(0,params)); CHECK(Fake::stmtExecutions==1);
        CHECK(c.BeginTransaction()); CHECK(!c.ExecuteStmt(0,params));
        CHECK(Fake::stmtExecutions==1); CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==0);
    });
    run("outside transaction SQL compatibility remains unchanged",[](MySQLConnection& c) {
        CHECK(c.Execute("CREATE TABLE allowed(id INT)")); CHECK(c.Execute("SET autocommit=1"));
        CHECK(c.Execute("UPDATE first; COMMIT; UPDATE second"));
    });
    run("migration CREATE plus INSERT plus ledger succeeds on pinned session",[](MySQLConnection& c) {
        CHECK(c.ExecuteMigrationBatch({"CREATE TABLE migration_test(id INT)", "INSERT INTO migration_test VALUES (1)", "INSERT INTO ledger VALUES (1)"}));
        CHECK(Fake::count("CREATE TABLE migration_test(id INT)")==1);
        CHECK(Fake::count("INSERT INTO migration_test VALUES (1)")==1);
        CHECK(Fake::count("INSERT INTO ledger VALUES (1)")==1); CHECK(Fake::count("COMMIT")==1); CHECK(Fake::connections==1);
        CHECK(Fake::ledgerObservedTransaction); CHECK(Fake::count("START TRANSACTION")==2);
        CHECK(Fake::errors.empty()); CHECK(c.Execute("UPDATE independent")); CHECK(Fake::connections==2);
    });
    run("migration middle error prevents ledger and final commit",[](MySQLConnection& c) {
        Fake::failures.push_back({"INSERT INTO migration_test VALUES (1)",ER_DUP_ENTRY});
        CHECK(!c.ExecuteMigrationBatch({"CREATE TABLE migration_test(id INT)", "INSERT INTO migration_test VALUES (1)", "INSERT INTO ledger VALUES (1)"}));
        CHECK(Fake::count("INSERT INTO ledger VALUES (1)")==0); CHECK(Fake::count("COMMIT")==0);
        CHECK(Fake::count("ROLLBACK")==1); CHECK(Fake::connections==1); CHECK(Fake::errors.empty());
    });
    run("migration connection loss never reconnects or replays",[](MySQLConnection& c) {
        Fake::failures.push_back({"INSERT INTO migration_test VALUES (1)",CR_SERVER_LOST});
        CHECK(!c.ExecuteMigrationBatch({"CREATE TABLE migration_test(id INT)", "INSERT INTO migration_test VALUES (1)", "INSERT INTO ledger VALUES (1)"}));
        CHECK(Fake::count("INSERT INTO migration_test VALUES (1)")==1); CHECK(Fake::count("INSERT INTO ledger VALUES (1)")==0);
        CHECK(Fake::connections==1); CHECK(Fake::count("COMMIT")==0); CHECK(Fake::errors.empty());
    });
    run("migration invocation inside runtime transaction is refused",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); CHECK(c.Execute("UPDATE prior"));
        CHECK(!c.ExecuteMigrationBatch({"CREATE TABLE forbidden(id INT)", "INSERT INTO ledger VALUES (1)"}));
        CHECK(Fake::count("CREATE TABLE forbidden(id INT)")==0); CHECK(!c.CommitTransaction()); CHECK(Fake::count("COMMIT")==0);
    });
    run("failed migration COMMIT is never replayed",[](MySQLConnection& c) {
        Fake::failures.push_back({"COMMIT",CR_SERVER_LOST});
        CHECK(!c.ExecuteMigrationBatch({"CREATE TABLE migration_test(id INT)", "INSERT INTO ledger VALUES (1)"}));
        CHECK(Fake::count("COMMIT")==1); CHECK(Fake::connections==1); CHECK(Fake::errors.empty());
    });
    run("migration BEGIN failure invokes no statements",[](MySQLConnection& c) {
        Fake::failures.push_back({"START TRANSACTION",CR_SERVER_LOST});
        CHECK(!c.ExecuteMigrationBatch({"CREATE TABLE forbidden(id INT)", "INSERT INTO ledger VALUES (1)"}));
        CHECK(Fake::count("CREATE TABLE forbidden(id INT)")==0); CHECK(Fake::count("INSERT INTO ledger VALUES (1)")==0);
        CHECK(Fake::connections==1); CHECK(Fake::errors.empty());
    });
    run("known deadlock retries entire transaction from first statement",[](MySQLConnection& c) {
        SqlTransaction transaction(0,1);
        transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE second"));
        Fake::failures.push_back({"UPDATE first",ER_LOCK_DEADLOCK});
        size_t begin = Fake::queries.size();
        CHECK(transaction.Execute(&c));
        std::vector<std::string> observed(Fake::queries.begin()+begin,Fake::queries.end());
        CHECK(observed == std::vector<std::string>({"START TRANSACTION","UPDATE first","ROLLBACK",
            "START TRANSACTION","UPDATE first","UPDATE second","COMMIT"}));
        CHECK(!c.CanRetryTransaction()); CHECK(Fake::connections==1);
    });
    run("middle deadlock replays prior statement but submits following statement once",[](MySQLConnection& c) {
        SqlTransaction transaction(0,1);
        transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE second"));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE third"));
        Fake::failures.push_back({"UPDATE second",ER_LOCK_DEADLOCK});
        CHECK(transaction.Execute(&c)); CHECK(Fake::count("UPDATE first")==2);
        CHECK(Fake::count("UPDATE second")==2); CHECK(Fake::count("UPDATE third")==1); CHECK(Fake::count("COMMIT")==1);
    });
    run("known-deadlock retry budget is bounded and exhaustion fails",[](MySQLConnection& c) {
        SqlTransaction transaction(0,2);
        transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE second"));
        Fake::failures={{"UPDATE first",ER_LOCK_DEADLOCK},{"UPDATE first",ER_LOCK_DEADLOCK},{"UPDATE first",ER_LOCK_DEADLOCK}};
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("START TRANSACTION")==3);
        CHECK(Fake::count("UPDATE first")==3); CHECK(Fake::count("UPDATE second")==0);
        CHECK(Fake::count("ROLLBACK")==3); CHECK(Fake::count("COMMIT")==0); CHECK(!c.CanRetryTransaction());
    });
    run("opt-in retry does not replay non-deadlock errors",[](MySQLConnection& c) {
        SqlTransaction transaction(0,2); transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        Fake::failures.push_back({"UPDATE first",ER_DUP_ENTRY});
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("START TRANSACTION")==1); CHECK(Fake::count("UPDATE first")==1);
    });
    run("opt-in retry never replays failed COMMIT",[](MySQLConnection& c) {
        SqlTransaction transaction(0,2); transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        Fake::failures.push_back({"COMMIT",CR_SERVER_LOST});
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("START TRANSACTION")==1);
        CHECK(Fake::count("UPDATE first")==1); CHECK(Fake::count("COMMIT")==1); CHECK(!c.CanRetryTransaction());
    });
    run("deadlock reported during COMMIT is never replayed",[](MySQLConnection& c) {
        SqlTransaction transaction(0,2); transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        Fake::failures.push_back({"COMMIT",ER_LOCK_DEADLOCK});
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("START TRANSACTION")==1); CHECK(Fake::count("COMMIT")==1);
        CHECK(!c.CanRetryTransaction());
    });
    run("deadlock reported during BEGIN is never replayed",[](MySQLConnection& c) {
        SqlTransaction transaction(0,2); transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        Fake::failures.push_back({"START TRANSACTION",ER_LOCK_DEADLOCK});
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("START TRANSACTION")==1);
        CHECK(Fake::count("UPDATE first")==0); CHECK(!c.CanRetryTransaction());
    });
    run("known deadlock is not retried when explicit rollback fails",[](MySQLConnection& c) {
        SqlTransaction transaction(0,2); transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        Fake::failures={{"UPDATE first",ER_LOCK_DEADLOCK},{"ROLLBACK",CR_SERVER_LOST}};
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("START TRANSACTION")==1);
        CHECK(Fake::count("UPDATE first")==1); CHECK(Fake::count("ROLLBACK")==1);
    });
    run("connection loss is never retried even with opt-in budget",[](MySQLConnection& c) {
        SqlTransaction transaction(0,2); transaction.DelayExecute(new SqlPlainRequest("UPDATE first"));
        Fake::failures.push_back({"UPDATE first",CR_SERVER_LOST});
        CHECK(!transaction.Execute(&c)); CHECK(Fake::count("START TRANSACTION")==1);
        CHECK(Fake::count("UPDATE first")==1); CHECK(Fake::connections==1);
    });
    run("prepared statement deadlock retries whole transaction without reconnect",[](MySQLConnection& c) {
        SqlTransaction transaction(0,1);
        auto params = new SqlStmtParameters(1); params->addParam(SqlStmtFieldData(uint32(7)));
        transaction.DelayExecute(new SqlPreparedRequest(0,params));
        transaction.DelayExecute(new SqlPlainRequest("UPDATE second"));
        Fake::executeError=ER_LOCK_DEADLOCK;
        CHECK(transaction.Execute(&c)); CHECK(Fake::stmtExecutions==2);
        CHECK(Fake::count("UPDATE second")==1); CHECK(Fake::connections==1); CHECK(Fake::stmtCloses==0);
    });
    run("retry marker does not leak from prior aborted transaction",[](MySQLConnection& c) {
        CHECK(c.BeginTransaction()); Fake::failures.push_back({"UPDATE first",ER_LOCK_DEADLOCK});
        CHECK(!c.Execute("UPDATE first")); CHECK(c.CanRetryTransaction()); CHECK(c.RollbackTransaction());
        CHECK(!c.CanRetryTransaction()); CHECK(c.BeginTransaction()); CHECK(!c.CanRetryTransaction());
        Fake::failures.push_back({"UPDATE first",ER_DUP_ENTRY}); CHECK(!c.Execute("UPDATE first"));
        CHECK(!c.CanRetryTransaction()); CHECK(c.RollbackTransaction());
    });
    std::cout << passed << " transaction guard checks passed\n";
}
