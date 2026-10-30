// SPDX-License-Identifier: GPL-2.0-or-later
#include "Common.h"
#include <functional>
#include "Database/DatabaseEnv.h"
#include "Database/CharacterStorage.h"
#include "Database/DatabaseMysql.h"
#include "Database/QueryResult.h"
#include "Database/SqlOperations.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace
{
unsigned checks = 0;
void Check(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
    ++checks;
}

std::string Normalize(std::string value)
{
    std::replace(value.begin(), value.end(), '\\', '/');
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return char(std::tolower(c)); });
    while (!value.empty() && value.back() == '/') value.pop_back();
    return value;
}

void Guard(MySQLConnection& connection)
{
    std::unique_ptr<QueryResult> result(connection.Query("SELECT @@port, @@datadir, DATABASE()"));
    Check(bool(result), "Cannot verify sandbox identity");
    Field* fields = result->Fetch();
    Check(fields[0].GetUInt32() == 3307, "REFUSED: not private sandbox port");
    Check(Normalize(fields[1].GetCppString()) ==
        "d:/wow/storage-reliability/runtime/database/data", "REFUSED: wrong private datadir");
    Check(fields[2].GetCppString() == "storage_test_char", "REFUSED: wrong fixture database");
}

uint32 Value(MySQLConnection& connection, uint32 id)
{
    std::string sql = "SELECT value FROM storage_guard_fixture WHERE id=" + std::to_string(id);
    std::unique_ptr<QueryResult> result(connection.Query(sql.c_str()));
    Check(bool(result), "Fixture row missing");
    return result->Fetch()[0].GetUInt32();
}

std::string ConnectionInfo(const char* options)
{
    std::ifstream input(options);
    Check(bool(input), "Private options file unavailable");
    std::map<std::string, std::string> values;
    std::string line;
    while (std::getline(input, line))
    {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto equal = line.find('=');
        if (equal != std::string::npos) values[line.substr(0, equal)] = line.substr(equal + 1);
    }
    Check(values["host"] == "127.0.0.1" && values["port"] == "3307", "REFUSED: non-sandbox options");
    Check(values["password"].find(';') == std::string::npos, "Unsupported options delimiter");
    return "127.0.0.1;3307;" + values["user"] + ";" + values["password"] + ";storage_test_char";
}
}

int main(int argc, char** argv)
{
    try
    {
        Check(argc == 2, "Supply private sandbox options file; never a production connection");
        std::string info = ConnectionInfo(argv[1]);
        DatabaseMysql database;
        MySQLConnection connection(database), killer(database);
        Check(connection.Initialize(info.c_str()), "Sandbox connection failed");
        Check(killer.Initialize(info.c_str()), "Sandbox fault connection failed");
        Guard(connection);
        Guard(killer);
        Check(HasTransactionalCharacterStorage(connection), "Migrated fixture should pass startup policy");
        Check(connection.Execute("CREATE TABLE storage_guard_myisam(id INT) ENGINE=MyISAM"), "Could not create policy fixture");
        Check(!HasTransactionalCharacterStorage(connection), "Startup policy accepted an active MyISAM table");
        Guard(connection);
        Check(connection.Execute("DROP TABLE storage_guard_myisam"), "Policy fixture cleanup failed");
        Check(HasTransactionalCharacterStorage(connection), "Startup policy did not recover after cleanup");
        Check(connection.ExecuteMigrationBatch({
            "DROP TABLE IF EXISTS storage_guard_fixture",
            "CREATE TABLE storage_guard_fixture(id INT PRIMARY KEY,value INT NOT NULL) ENGINE=InnoDB",
            "INSERT INTO storage_guard_fixture VALUES(1,10),(2,20)"}), "DDL migration batch failed");

        Guard(connection);
        SqlTransaction success(1);
        success.DelayExecute(new SqlPlainRequest("UPDATE storage_guard_fixture SET value=11 WHERE id=1"));
        Check(success.Execute(&connection), "Valid production transaction failed");
        Check(Value(connection, 1) == 11, "Valid transaction did not commit");

        Guard(connection);
        SqlTransaction aborted(1);
        aborted.DelayExecute(new SqlPlainRequest("UPDATE storage_guard_fixture SET value=99 WHERE id=1"));
        aborted.DelayExecute(new SqlPlainRequest("INSERT INTO storage_guard_fixture VALUES(1,99)"));
        aborted.DelayExecute(new SqlPlainRequest("UPDATE storage_guard_fixture SET value=99 WHERE id=2"));
        Check(!aborted.Execute(&connection), "Duplicate-key transaction incorrectly succeeded");
        Check(Value(connection, 1) == 11 && Value(connection, 2) == 20, "Failed transaction was only partly rolled back");

        Guard(connection);
        Check(connection.BeginTransaction(), "Could not begin fault test");
        Guard(connection);
        Check(connection.Execute("UPDATE storage_guard_fixture SET value=77 WHERE id=1"), "Fault setup failed");
        std::unique_ptr<QueryResult> identity(connection.Query("SELECT CONNECTION_ID()"));
        Check(bool(identity), "Missing owned connection id");
        uint32 ownedId = identity->Fetch()[0].GetUInt32();
        identity.reset();
        Guard(killer);
        std::string kill = "KILL CONNECTION " + std::to_string(ownedId);
        Check(killer.Execute(kill.c_str()), "Could not close owned sandbox connection");
        // Identity was verified before injection on this pinned, non-reconnecting transaction.
        Check(!connection.Execute("UPDATE storage_guard_fixture SET value=88 WHERE id=2"), "Lost transaction replayed DML");
        Check(!connection.CommitTransaction(), "Lost transaction reported commit success");
        Guard(connection); // Recovery is permitted only after the failed transaction has ended.
        Check(Value(connection, 1) == 11 && Value(connection, 2) == 20, "Connection loss left a partial save");

        Guard(connection);
        Check(connection.BeginTransaction(), "Could not begin DDL rejection test");
        Guard(connection);
        Check(connection.Execute("UPDATE storage_guard_fixture SET value=55 WHERE id=1"), "DDL setup failed");
        Guard(connection);
        Check(!connection.Execute("CREATE TABLE storage_guard_forbidden(id INT)"), "Runtime DDL was executed");
        Check(!connection.CommitTransaction(), "Rejected DDL transaction reported success");
        Check(Value(connection, 1) == 11, "DDL implicitly committed earlier changes");
        std::unique_ptr<QueryResult> forbidden(connection.Query("SHOW TABLES LIKE 'storage_guard_forbidden'"));
        Check(!forbidden, "Rejected runtime DDL created a table");

        SqlStatementID preparedId;
        database.CreateStatement(preparedId, "UPDATE storage_guard_fixture SET value=? WHERE id=?");
        SqlStmtParameters parameters(2);
        parameters.addParam(SqlStmtFieldData(uint32(31)));
        parameters.addParam(SqlStmtFieldData(uint32(2)));
        Guard(connection);
        Check(connection.BeginTransaction(), "Could not begin prepared test");
        Guard(connection);
        Check(connection.ExecuteStmt(preparedId.ID(), parameters), "Real prepared DML failed");
        Check(connection.CommitTransaction(), "Real prepared COMMIT failed");
        Check(Value(connection, 2) == 31, "Prepared value not stored");

        Guard(connection);
        Check(connection.ExecuteMigrationBatch({
            "CREATE TRIGGER storage_guard_retry BEFORE UPDATE ON storage_guard_fixture FOR EACH ROW "
            "BEGIN IF NEW.id=2 AND @storage_retry_once IS NULL THEN SET @storage_retry_once=1; "
            "SIGNAL SQLSTATE '40001' SET MYSQL_ERRNO=1213, MESSAGE_TEXT='isolated retry injection'; END IF; END"}),
            "Could not install private error injection");
        Guard(connection);
        SqlTransaction retry(1, 3);
        retry.DelayExecute(new SqlPlainRequest("UPDATE storage_guard_fixture SET value=value+1 WHERE id=1"));
        retry.DelayExecute(new SqlPlainRequest("UPDATE storage_guard_fixture SET value=32 WHERE id=2"));
        Check(retry.Execute(&connection), "Confirmed-abort whole-transaction retry failed");
        Check(Value(connection, 1) == 12 && Value(connection, 2) == 32,
            "Retry did not roll back and replay the complete prefix exactly once");
        Guard(connection);
        Check(connection.Execute("DROP TRIGGER storage_guard_retry"), "Error injection cleanup failed");

        Guard(connection);
        Check(!connection.ExecuteMigrationBatch({
            "CREATE TABLE storage_guard_ledger(id INT PRIMARY KEY) ENGINE=InnoDB",
            "INSERT INTO storage_guard_fixture VALUES(3,30)",
            "INSERT INTO storage_guard_fixture VALUES(1,999)",
            "INSERT INTO storage_guard_ledger VALUES(1)"}), "Failed migration reported success");
        Guard(connection);
        std::unique_ptr<QueryResult> pending(connection.Query("SELECT COUNT(*) FROM storage_guard_fixture WHERE id=3"));
        Check(bool(pending) && pending->Fetch()[0].GetUInt32() == 0, "Failed migration retained pending DML");
        pending.reset();
        std::unique_ptr<QueryResult> ledger(connection.Query("SELECT COUNT(*) FROM storage_guard_ledger"));
        Check(bool(ledger) && ledger->Fetch()[0].GetUInt32() == 0, "Failed migration wrote success ledger");
        ledger.reset();
        Guard(connection);
        Check(connection.Execute("DROP TABLE storage_guard_ledger"), "Migration fixture cleanup failed");

        Guard(connection);
        Check(connection.Execute("DROP TABLE storage_guard_fixture"), "Fixture cleanup failed");
        std::cout << "PASS " << checks << " real production C++ / private MariaDB checks\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
