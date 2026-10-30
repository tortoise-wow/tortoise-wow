---
layout: default
title: Atomic Player Trades
---

# Atomic Player Trades

Saving each participant separately can leave one player's new wallet and inventory
committed while the other player's save fails. Core trades now append both players'
inventory and final gold balances to one owned SQL transaction. The server sends
`TRADE_COMPLETE` only after that transaction's COMMIT is acknowledged.

## Prerequisites and ordering

The implemented and tested backend is MariaDB/MySQL. Active character tables must
use InnoDB, flush-at-commit must be 1 and force-recovery must be 0. Existing realms
must complete the offline migration described in
[transactional-storage.md](transactional-storage.md) first.

```ini
CharacterDatabase.RequireTransactionalTables = 1
CharacterDatabase.OrderedPersistence = 1
```

Ordered persistence is configured before database workers start. It uses one
executor and one FIFO for character writes, synchronous/direct operations, queries
and login holders. It overrides CharacterDatabase.WorkerThreads; other databases
retain their configured workers. This reduces character-save parallelism and adds
SQL round-trip latency to completing a trade. Large-realm throughput has not been
established by the local functional tests.

Normal runtime transaction builders register a lease before capturing SQL values.
An owned trade refuses admission while an earlier builder is open, leaving items
and money untouched so the players can retry. Lease release and queue admission
share a gate. Once an owned builder starts, later normal builders cannot capture
state until the pair is admitted. This prevents an older staged save from being
submitted after the trade. The pair records both nonzero, distinct player GUIDs;
it does not select a queue by hashing that pair.

Owned scopes cannot be committed or rolled back by legacy nested helpers. Direct
SQL and database queries inside the owned builder fail closed rather than escaping
the transaction or waiting on their own admission gate. The owned handle releases
that gate before waiting for the executor's primitive completion result.

## Inventory lifetime and actor ownership

Both players must remain on the same map. The map session handler serializes the
transfer, final balances and standard synchronous trade enchants/reagent changes.
Inventory serializers append SQL without clearing dirty state, invalidating caches
or deleting removed items. Only the same actor thread, after acknowledged COMMIT,
consumes the captured queues. Database workers hold owned SQL values and a completion
promise; they never retain Player/Item pointers or invoke inventory recovery code.

Captured queues are validated for ownership, slots, duplicate/stale pointers,
buyback items and unsafe nonempty removed bags. Enchantment durations, wrapped gifts,
item text, dirty loot and transmogrification deletion are included in the existing
serializers. Unrelated dirty inventory work is included for both players too.

Shop delivery previously mutated inventories through a World async session task,
which could merge into an ACK-waiting stack. Delivery now queues immutable account,
character and item identities in WorldPlayerTasks. One sequential world-thread batch
resolves the current session and runs before map updates. Tasks queued during a
batch wait for the next tick. Other asynchronous session/list queries remain on
their existing paths. The producer and consumer share one queue across translation
units; queued work never owns a raw player pointer.

## Failure and shutdown

Any failed admitted character write in ordered mode fences subsequent writes and
login/read work. Waiters are completed or canceled; the realm stops with an error
instead of continuing to save speculative memory. An ambiguous COMMIT is never
automatically replayed. The operator must review the database error before restart.
After restart, a trade transaction is either fully committed or fully absent.

Healthy shutdown closes new admission and drains already admitted operations,
including final logout saves, before closing connections. Faulted shutdown cancels
dependent work. Builder abandonment, serialization failure after an irreversible
RAM transfer, or invalid completion also fences persistence and stops the realm.

This is not a durable admission journal. It cannot recover every unsaved gameplay
mutation after a permanent failure or process termination, and it does not supply
receipts/deduplication for arbitrary unknown outcomes. Quest status, crafting skill,
reputation, scripted effects and reward mail can have persistence outside the core
inventory/gold boundary. Auto-rewarded quest consequences and external hooks must
not be described as an atomic quest completion. Reward-mail and general dirty-state
recovery remain separate work. The validated server build disables Eluna.

## Verification

Run the production-source transaction guard and ordered queue tests, the inventory
serialization/lifetime tests and the two-translation-unit world-task tests under
`tests/storage/`. Their PowerShell runners keep outputs outside the source tree.
Compile the full server against normal headers as well: standalone boundary fakes
do not validate all gameplay types.

Real protocol testing uses two newly created private accounts and a separate copy
of all four databases, never live player identities. A controlled trigger blocks
the second participant's late gold update: independent MVCC reads must still show
the old complete pair and neither client may receive COMPLETE. Releasing a healthy
gate must commit both sides before COMPLETE; a permanent SQL error must roll back
both, fence later saves and stop the private realm. Fresh login then verifies the
authoritative result. Repeat for both acceptance orders, stack merge/split, wrapped
gifts and standard nontraded enchants; verify a normal shutdown drains logout saves.
