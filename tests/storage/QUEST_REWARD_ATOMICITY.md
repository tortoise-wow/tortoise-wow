# Core quest reward and generated mail

`AtomicPlayerSaveScope` owns a synchronous character-database transaction on the
player's existing map actor. Ordinary quest rewards admit the scope before
consuming required items. An automatic reward can join a surrounding player trade;
the outer owner then captures each participant once, selecting the core player
snapshot instead of its inventory-only snapshot where needed.

The transaction includes the character row (gold, XP, level and challenge state),
inventory and required-item removal, quest status/reward choice, dirty skills and
spells, auras, reputation, honor, character variables and item logs. Native local
reward effects and character-title writes run before the snapshot is sealed.
Generated mail, attachments and body text join the same SQL batch.
Mail IDs are reserved in memory; reserving an ID is not publication or a durable
write. Item-text and mail ID/cache access is synchronized.

Prepared mail owns its detached attachments until acknowledged commit. It refuses
to adopt pointers still owned by a player's inventory or existing mailbox. After
commit, the same actor consumes saved dirty/deleted state, publishes the mail to
the online mailbox, and sends deferred reward-item, XP, level and quest-completion
notifications. No database worker invokes gameplay objects.

A busy transaction builder rejects a reward before mutation. Automatic flag
quests stay incomplete and retry through a deduplicated player-owned event. A
failed append, failed/ambiguous commit or abandoned admitted scope fences later
character writes and stops the realm. The process must not continue saving a
partially mutated in-memory player. An unknown commit is never replayed: after
operator recovery the database contains the complete old or complete new batch.

## Deliberate boundaries

This captures the core quest reward, not every side effect of `SaveToDB`. It does
not serialize pets, battleground team state or arbitrary quest scripts and world
effects. Native local teaching, skill, item, aura and honor rewards are captured
before commit; complex scripted casts/events remain a separate boundary. External
quest hooks run after the core commit. The level-60
slow-and-steady Login-database coin credit is also deferred until that commit; a
process failure between the two databases can still lose that separate credit.
A durable cross-database reward journal remains a follow-up.

Legacy mail callers retain their existing transaction ownership. In particular a
borrowed legacy transaction is not committed by `SendMailTo`. Its optimistic
publication remains outside this owned-scope guarantee. Offline template mail
keeps its existing lazy attachment generation behavior.

## Validation

The storage runners exercise the production serialization bodies, real
`AtomicPlayerSave.cpp`, mail preparation/publication and the database scheduler.
They verify retained dirty/deleted objects until ACK, refusal of inventory-owned
attachments, nested participant selection, no publication before ACK, busy and
failure behavior, actor-owned automatic retries and legacy mail compatibility.

Native acceptance uses separate cloned InnoDB schemas and ordinary test accounts.
It accepts and turns in real delivery quests through protocol packets, verifies
the inventory/gold/XP/quest/mail outcome, reads the mailbox and reconnects. A late
quest-status SQL trigger first proves that mail and its attachment are already
staged, holds the transaction while external MVCC reads must still show the old
state, then either releases or injects error 1644. Failure must leave the complete
old state and stop the test realm without successful reward notifications.

Each synthetic quest needs its own reward mail template: the normal loader
disables a template reused by multiple quests. Native coverage and its binary
hashes must be recorded separately; these design notes alone are not a claim
that a particular build passed acceptance.
