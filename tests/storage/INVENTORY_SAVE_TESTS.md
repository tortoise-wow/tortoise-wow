# Inventory save tests

Run `./tests/storage/run-inventory-save-tests.ps1 -OutputDirectory <isolated-output>`.
This is a standalone MSVC C++17 test, with no database or server runtime.

The test includes the same production `ItemSave.inc`, `ItemUpdateQueue.inc` and
`PlayerInventorySave.inc` bodies compiled by Item.cpp/Player.cpp. The real
serialization, dirty consumption, NEW-to-REMOVED deletion, and queue methods are
not copied into the test. Small boundary declarations replace unrelated game
object fields, loot accessors, logging, transmog cache publication and SQL I/O.
The normal server build must separately verify these methods against actual
headers and gameplay types.

Coverage includes both players' gold/item SQL; new/changed/removed item lifetime;
every statement-enqueue failure; wrapped gift owner/deletion, item text, loot
gold/items/deletion and transmogrification deletion; nonempty removed bags;
buyback normalization including previously queued items; enchant duration dirty
queueing; stashed items; duplicate/stale queue entries and invalid queue positions;
disabled/faulted/unregistered admission; and legacy consume APIs called from an
owned transaction, including an unregistered third owner. SQL-free completion
cleans/deletes only after the caller's acknowledged-commit branch. Real queue
lifecycle also covers unsaved new item transfer, new-item merge disappearance,
and a tombstone after a previously completed new insert.

These tests do not implement or prove DB-worker scheduling, actual COMMIT
acknowledgement, real InnoDB rollback, full trade movement/spell execution, or
crash recovery. Those require the separate production database tests, a normal
server compile and isolated native two-session tests. No durable-intent or
receipt recovery claim is made.
