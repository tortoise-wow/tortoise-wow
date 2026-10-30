# Ordered CharacterDatabase tests

Run `run-ordered-persistence-tests.ps1` after loading the MSVC x64 environment and
ACE_ROOT. It compiles the current production Database.cpp, SqlOperations.cpp and
SqlDelayThread.cpp directly, linking the already-built support libraries. It
uses an in-process SqlConnection fake, never initializes a network connection,
and reads no credentials. Outputs stay in the selected evidence directory. The
runner fails after 15 seconds if the test process hangs.

The test covers global/GUID operations, owned participant batches, synchronous
DirectExecute/DirectExecuteStmt/CommitTransactionDirect, Query barriers and real
SqlQueryHolder dispatch. It checks exact order and real COMMIT acknowledgement,
held builder admission, prior normal-builder contention, rollback and TSS lease
release, append failure, post-detach allocation failure, accepted and rejected
completion lifetime, fault cancellation and healthy shutdown drain. Worker
reentrant synchronous calls fail before taking the admission mutex.

Normal builders register a shared RAII lease before capture. Owned admission
refuses while any normal builder is open; it does not wait while blocking that
builder's read/commit. Admission releases normal leases under the same gate,
BEFORE queue exposure, because the executor may immediately destroy an exposed
operation. Rollback/TSS cleanup only decrement shared counter state and hold no
raw Database callbacks. An owned staging gate blocks later normal Begin until
the pair is admitted. Arbitrary caller data captured outside a transaction
builder is only guaranteed FIFO by admission; gameplay actor ownership remains
required to prevent concurrent mutation of Player/Item objects.

All current ordered runtime write failures latch persistence and cancel dependent
work. Healthy shutdown closes new admission, drains already admitted logout
writes/read waiters on the live worker, then closes connections. Faulted shutdown
cancels dependent work and signals waiters. This containment is not a durable save
journal and does not resolve A06 after permanent/ambiguous failures.
