# Transaction guard tests

Run on Windows with the MSVC C++ tools installed:

```powershell
./tests/storage/run-transaction-guard-tests.ps1 -OutputDirectory <test-output-directory>
```

The standalone executable directly compiles the production `DatabaseMysql.cpp`
and `SqlOperations.cpp` implementations and uses their real class declarations and prepared-statement base
header. A deterministic MySQL C API replaces network I/O; the small `stubs/`
headers replace logging, query-result wrappers, and unrelated server/worker
infrastructure. MSVC's test-only `include_alias` replaces unrelated `DatabaseImpl.h`
asynchronous templates with an empty header. No database connection, credentials, server build, or live runtime
is used. The stubs contain no transaction guard implementation.

These checks exercise failed BEGIN, transaction connection loss and no replay,
statement errors and server rollback, session changes, failed COMMIT without
retry, nested BEGIN, outside-transaction recovery, prepared prepare/bind/execute
failure, deferred prepared-cache destruction, and multiline errors/refusal.
An optional retry budget replays the entire operation vector only after a
statement reports ER_LOCK_DEADLOCK, the backend confirms the same live session,
and explicit rollback succeeds. The default budget is zero. Tests verify replay
order, bounded exhaustion, prepared statement replay, marker reset, and no retry
after BEGIN/COMMIT failures, other SQL errors, connection loss, or failed rollback.
Production callers must opt in only after verifying all involved tables use InnoDB.
Runtime transaction SQL is checked before submission: unsupported statements,
transaction control, top-level multiple statements, and executable version comments
are refused; ordinary DML/SELECT with quoted semicolons and trailing delimiters
remain allowed. String escaping follows the server's NO_BACKSLASH_ESCAPES status.
Backslash escapes within double-quoted text fail closed inside transactions because
ANSI_QUOTES changes their meaning without a corresponding server status bit.

Migration batch tests exercise the explicit startup migration path, including
CREATE/INSERT/ledger success, ledger retained until final COMMIT after DDL,
first-error stop, no replay after connection loss, refused invocation inside a
runtime transaction, and failed BEGIN/COMMIT. This path is deliberately non-atomic
across DDL: implicit commits may leave earlier changes durable. Its session is
always discarded so migration SET/autocommit state cannot leak into runtime work.

The stub `SqlConnection` owns cached prepared statements with the same lifetime
boundary as production, while using unique_ptr so a preparation failure does not
leak. Production `SqlConnection::ExecuteStmt` must be virtual for the MySQL
recovery entry point to dispatch through generic SQL operations. Compile the
production implementation with normal server headers as an additional check;
the standalone stubs cannot validate the broader inheritance/build environment.

The public `SqlTransaction` checks enqueue real plain/prepared operations and
assert BEGIN abort, stop after statement failure, COMMIT failure propagation,
and the empty transaction fast path. Sandbox integration
should also use InnoDB tables and a separate administrative connection to KILL
the transaction connection after an early write and before COMMIT, then verify
that none of the transaction writes persisted. A connection loss during COMMIT
has an unknown outcome and must never automatically replay the transaction.
