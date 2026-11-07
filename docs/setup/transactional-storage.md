---
layout: default
title: Transactional Character Storage
---

# Transactional Character Storage

Character saves can update gold, inventory, item instances, quest progress, mail
and other tables in one transaction. Every table participating in that transaction
must use a transactional engine for a failed save to roll back completely.

Fresh installations from `sql/create_databases.sql` use InnoDB for all active
character tables. The 54 formerly MyISAM tables also specify `ROW_FORMAT=DYNAMIC`;
InnoDB cannot inherit MyISAM's `FIXED` row format. Their columns, defaults, indexes,
character sets and collations remain unchanged. This change does not modify the
authentication, world or logging database schemas.

## Startup requirement

The default setting in `mangosd.conf` is:

```ini
CharacterDatabase.RequireTransactionalTables = 1
```

For MariaDB/MySQL, the world server checks character storage after schema updates
and before admitting players and runtime writers. Startup fails unless:

- There is at least one active character base table.
- Every active character base table uses InnoDB.
- `innodb_flush_log_at_trx_commit` is `1`.
- `innodb_force_recovery` is `0`.

After this check passes, character transactions allow up to three retries of an
entire immutable batch after a statement reports `ER_LOCK_DEADLOCK` and explicit
rollback succeeds. Other errors, failed BEGIN, lost connections and ambiguous
COMMIT never trigger replay. Other databases retain their default zero retry
budget. This does not supply durable save admission or recover a discarded
permanent/ambiguous failure; those remain separate persistence work.

The check includes newly added base tables automatically. Five historical copies
are excluded: `character_bck`, `character_duplicate_account`,
`character_reputation_broken`, `character_skills_copy`, and
`characters_namecleanup2020_2`. They retain their original engines and must remain
read-only archives outside runtime save transactions. New archive exceptions
require an explicit code and ownership review.

Setting `CharacterDatabase.RequireTransactionalTables = 0` skips this startup
check; it does not make MyISAM transactional or provide rollback protection. Keep
the default enabled for a realm that accepts players. A rejected startup requires
an offline storage/configuration correction before reopening the realm.

## Existing installations

Importing the fresh-install script over an existing database is destructive. Use
the reviewed manual candidate in
[`sql/wip_updates/character_storage_innodb.sql`](../../sql/wip_updates/character_storage_innodb.sql)
for the 54-table engine conversion instead. It is intentionally outside the
automatic startup update sequence and contains no database-selection directive.

1. Stop and drain every writer, including world/realm processes, SOAP, administrative
   sessions, imports and batch jobs. Keep writers stopped through verification and
   any recovery decision.
2. Take a complete consistent backup. Mixed MyISAM/InnoDB databases need a quiesced
   snapshot; `--single-transaction` alone does not protect concurrent MyISAM writes.
   Verify a full restore into a separate private database before changing engines.
3. Check source and restored metadata and rows for unsupported FULLTEXT/prefix
   indexes, zero dates, duplicate unique keys, corruption, and sufficient disk space.
   Rehearse the exact candidate against the restored rows. Preserve column/index
   definitions, row counts, logical checksums, AUTO_INCREMENT values and application
   integrity results.
4. Select the intended character database explicitly, apply the reviewed candidate
   while writers remain stopped, and stop on the first error. Do not use a client's
   `--force` option. DDL auto-commits, so an explicit `ROLLBACK` cannot reverse engine
   conversion or substitute for recovery from the verified backup.
5. Compare the final metadata, counts, checksums and integrity results with the
   quiesced baseline. Run `CHECK TABLE`, confirm the startup requirement passes, and
   exercise transaction failures through the application before reopening writers.

InnoDB and the startup check provide storage prerequisites. Correct application
transaction boundaries, failure propagation, memory-state handling and coordinated
recovery still require their own validation. This document does not establish that
every gameplay operation is atomic.
