# Core reward snapshot tests

Run `./tests/storage/run-player-core-save-tests.ps1 -OutputDirectory <isolated-output>`.
This standalone MSVC C++17 test uses no database or server runtime.

The test compiles the production `PlayerCoreSave.inc`, `ReputationSave.inc`,
`PlayerInventorySave.inc`, `ItemSave.inc` and `ItemUpdateQueue.inc` implementations.
The test environment supplies unrelated object fields, accessors and SQL transport.
The actual dirty-map serializer/completion logic is not copied into the test.

Coverage includes the required core save participant APIs; new/changed/deleted
quests, skills and spells; dependent-spell filtering; reputation dirty flags and
standing/flags binds; variables/item logs; every SQL-admission failure; owner,
teleport and disabled/faulted admission checks; one inventory serialization per
full snapshot; no dirty clearing, map erasure, cache invalidation or removed-item
deletion before the acknowledged branch; SQL-free completion; and compatibility
with ordinary map serializers.

`PlayerCharacterSave.inc` is deliberately a transport boundary in this unit test.
The exact production character SQL/bind block was extracted without changes and
is reused by normal SaveToDB. The normal server build verifies real types/binds;
native quest tests must verify XP, level, money and custom flags against real
character rows. This unit test does not claim to validate those binds, DB COMMIT,
mail visibility, actor scheduling, nested reward callbacks, or crash recovery.

The atomic reward snapshot includes character-row, inventory, quest, skill,
spell, reputation, checked aura/honor, variable and item-log state. It is not
identical to normal SaveToDB: cooldown/BG/pet/stats/tutorial/login-database work
remains outside this helper. Aura and honor internals are transport boundaries
in this orchestration test; honor has its own production-body tests. External
scripts/world effects and cross-database rewards retain their documented boundary.

Run `./tests/storage/run-auto-quest-retry-tests.ps1` for the actor-owned automatic
quest retry checks. This compiles production `PlayerQuestCompletion.inc` and the
real framework `EventProcessor.cpp`; only the outer atomic group and unrelated
reward/hook APIs are boundary fakes. It checks busy deduplication, one-next-tick
rescheduling, recovery when admission becomes available, deferred completion-hook
delivery, abandonment/fault/stopped cancellation, Player event destruction without
callback execution, and unchanged ordinary nonautomatic quest completion. It does
not replace native nested-auto-reward/mail/COMMIT tests.

`run-reward-packet-tests.ps1` compiles production `PlayerRewardPackets.inc` and
checks packet bytes remain value-owned and unsent until confirmed publication,
failed-scope discard, actor lookup at delivery, group broadcast/fallback, and
ordinary immediate behavior. SendNewItem, level-up info and XP gain serialize
their packet before calling this helper. No Item, Unit, Session or Group pointer
is retained by the queued publication.

The PlayerSpell test declaration uses the real enum bitfield width and bool
bitfields. This reproduces MSVC's forwarding restriction and verifies the saved
state is materialized as a separate value before emplacing its token.

`run-native-quest-reward-tests.ps1` compiles production
`PlayerNativeQuestRewards.inc`. It checks local effect and target classification,
data-cast-time teaching completion before snapshot, unfinished/failing dispatch,
item/aura/honor updates before seal, title/aura adjustments, excluded world/pet/
trigger/event/channel/CC/area cases, and instant single-effect far teleport policy.
The spell engine is a boundary fake: this test verifies orchestration, not real
Spell effects or SQL durability. Native teaching/honor/far-teleport fixtures and
the normal build must validate those against the actual engine and database.
