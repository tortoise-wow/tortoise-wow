# Prepared mail tests and ownership contract

Run `run-prepared-mail-tests.ps1` with MSVC x64 loaded. It compiles the same
production `PreparedMail.inc` body as Mail.cpp, with real Mail.h declarations and
small actor/loot/SQL boundary fixtures. It does not run a DB or game server and
uses no credentials or game assets. `run-ordered-persistence-tests.ps1` separately
exercises the real Database.cpp scheduler and now accepts a singleton positive
participant while still rejecting empty/zero/duplicate participant sets.

Owned reward scope:

- Root actor scope installs a MailPublicationCollector only while DB ownership is
  active. SendMailTo runs the actor hook once, prepares immutable metadata and
  copied SQL parameters, appends into the current owned transaction, then passes
  actor-owned PreparedMail to that collector. It never begins/commits/publishes
  independently, even when its legacy direct flag is true.
- Newly generated template items, cloned items and new body text are not saved by
  constructors/preparation. Body IDs are reserved, item_text/attachment/mail/link
  rows join the selected transaction, and new text cache/mailbox publication waits
  for the caller's acknowledged COMMIT. Existing body-ID overloads refer to text
  already owned/persisted by the caller and are not re-inserted.
- PreparedMail adopts detached attachments only. Inventory/update-queue/world or
  existing mailbox pointers fail preparation without deleting their actor owner.
  Draft-created clones/template objects are cleaned when abandoned. External
  AddItem pointers remain caller-owned until preparation accepts them.
- After ACK, Publish relooks up MasterPlayer by GUID, completes/transfers attachment
  RAM ownership and publishes a Mail copy. The prepared metadata getters remain
  valid afterward. If no master remains, only detached RAM objects are discarded;
  database rows are not deleted. No Player/Item/Mail pointer enters a DB worker.
- Online empty template rolls persist has_items=true (no repeated roll); offline
  template mail remains lazy. Existing-mail lazy generation uses the same append
  and publication boundary, preserving the original mail identity.

Legacy compatibility:

- A borrowed legacy transaction gets appends only: no nested Begin/Commit and no
  ownership theft, regardless of direct=true. Its historical optimistic mailbox
  publication remains an A06 limitation. Standalone sends begin before preparation,
  commit their own transaction, and honor direct versus async mode.
- Existing legacy attachment rows are not blindly re-saved, since MailHandler may
  already have queued a DB owner override while RAM still has the old owner.
  Generated NEW attachments are saved under the selected legacy transaction.
  Return mail serializes its item first, then appends its receiver owner override.
- Direct COMMIT failure and owned append/collector/ACK failure publish no mail.
  This does not provide durable-intent recovery for ambiguous outcomes or retrofit
  atomicity to unrelated legacy mail send/return/auction operations.

Coverage: owned direct flag, copied subject/body/item values, all statement-append
failure positions, failed outer ACK, missing/rejected collector, nested mail,
clone/body deferral, empty rolls, offline/online-master behavior, lazy generation,
borrowed/standalone/direct sends, returned owner override, detached/live pointer
ownership and SQL-free publication. Normal-header compilation of Mail.cpp validates
real gameplay declarations separately. Real InnoDB/native quest acceptance remains
parent-owned, including quest/full snapshot coupling and stock level-up mails.

Publication to ObjectMgr's global item-text cache also requires its normal Add/Get/
Create accessors to be synchronized across map actors; that narrow ObjectMgr change
is parent-owned. Caller pre-saves of attachments/CreateItemText in owned reward
paths must be removed by the Player serializer change, rather than hidden here.
