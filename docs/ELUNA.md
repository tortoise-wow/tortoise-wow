# Eluna Lua engine (optional)

[Eluna](https://github.com/ElunaLuaEngine/Eluna) can be compiled into the core
as an option. It is **off by default**: without `-DBUILD_ELUNA=ON` none of it
is compiled, and every core change for it sits inside `#ifdef ENABLE_ELUNA`,
so `unifdef -UENABLE_ELUNA` on a changed file gives back the file without
Eluna.

The integration is built on the port by Nighthawk42 and Nightdawg in
[TuringKi/turtle-wow-custom](https://github.com/TuringKi/turtle-wow-custom)
(`aa0b2be`, `e649428`, `bdbe530`). Design discussion:
[tortoise-wow#602](https://github.com/tortoise-wow/tortoise-wow/issues/602).

## Build

```sh
git submodule update --init --recursive dep/Eluna
cmake -S . -B build -DBUILD_ELUNA=ON            # ELUNA_LUA_VERSION=lua52 by default
cmake --build build --target mangosd
```

- The engine is the `dep/Eluna` submodule, pinned to `1b06f28` (branch
  `cmangos-spell-update` of ElunaLuaEngine/Eluna). Eluna master at the time
  (`58d7652`) declares TrinityCore aura and proc types unguarded and does not
  build for this core.
- The submodule stays pristine. The fixes this core needs in Eluna are the
  patch files in `dep/eluna-patches`; `cmake/ElunaSources.cmake` copies
  `dep/Eluna` to `<build>/dep/eluna-src`, applies the patches in name order
  (`git apply`, or `patch`) and the build compiles that copy. `git apply` is
  kept from treating the copy as part of the core's checkout (a build directory
  inside it, `cmake -B build`, would otherwise make it skip every file and
  still succeed; a build path with a `:` in it breaks that guard, which then
  ends in a CMake error, not a silent pass). Without git, `patch -p1
  --fuzz=0` is used. After applying, CMake checks the copy: every patch's
  `ELUNA_TW_PATCH_<n>` marker is there, and a scratch copy of it with the
  patches taken out again, last to first (they may touch the same lines), is
  file for file the pristine submodule. CMake stops if a patch does not apply
  or is missing (after a submodule bump, refresh the patches); the checks run
  on every configure and remake a copy that fails them.
  `src/game/ElunaScriptBridge.cpp` also fails to compile (`#error`) without
  the markers of all patches. The copy is remade when a submodule file, a patch or
  `ElunaSources.cmake` changes. Each patch names its Eluna base commit; 0001,
  0003, 0004, 0005, 0006, 0007, 0009, 0010 and 0011 (all of it but `Player:IsBot` and
  the two `Group.cpp` call sites) are candidates to upstream to
  Eluna, 0012 partly (the `LuaVal` value semantics and `RegisterSpellEvent` are not
  VMaNGOS-specific; the hit unit argument and the `ChatHandler` are), 0002 and 0008 are VMaNGOS-specific (other cores get the check from
  trackable pointers, and track their spell targets).

  | Patch | Fixes |
  | --- | --- |
  | `0001-eventmgr-...` | An object that left a state and came back before the state's next cleanup got a new event processor that was destroyed at once while still listed (use after free). |
  | `0002-vmangos-refuse-objects-of-another-map` | A map state could use objects of another map (see Threading rules). |
  | `0003-vmangos-instance-data-...` | `Save`, `GetData` and `GetData64` of a Lua instance script did not override this core's non-const `InstanceData` methods: instance data was saved empty and lost at a restart, and C++ callers read 0. |
  | `0004-vmangos-creature-spellhit-...` | `CREATURE_EVENT_ON_HIT_BY_SPELL` never fired: the Lua AI's `SpellHit` did not override this core's `CreatureAI::SpellHit(WorldObject*, ...)`. |
  | `0005-template-name-and-decoding-table-...` | Data race: every Lua state stored the per-type `ElunaTemplate<T>::tname` (one static per type for the whole process) in `Register`, so map states made or reloaded on different map threads wrote it at once while other states read it in `Push`/`Check`; also the lazy build of the base-64 table used for instance data. Both are now set once (`std::call_once`, a function-local static). |
  | `0006-eventmgr-free-lua-references-...` | Lua function references of timers leaked: when an object left its map or was destroyed with a timer pending, its event processor was removed with the events set to `LUAEVENT_STATE_ERASE`, which skips `luaL_unref` (meant for a closed or reloaded state). Each such timer's function, and what its upvalues reach, stayed in the state's registry until the state closed. They are now unreferenced (`LUAEVENT_STATE_ABORT`) on the thread that updates the state. A reload still marks every pending event `LUAEVENT_STATE_ERASE` (its references belong to the closed state), now also those of processors replaced by 0001 and waiting for the next cleanup, which are not in the processor list; otherwise that cleanup would unreference the old state's reference numbers in the new state. |
  | `0007-creature-ai-no-state-falls-back-...` | Crash (null `Eluna*`): every hook of the Lua creature AI called `me->GetEluna()->...`, but `WorldObject::GetEluna` is null while the creature is not in the world, and `Creature::Update` can still run the AI then (seen on `.npc delete` of a scripted creature: SIGSEGV in `Eluna::UpdateAI`). A missing state now counts as not handled, so the native AI runs, as for a creature without a script. |
  | `0008-vmangos-spell-gettarget-by-guid` | `Spell:GetTarget()` returned the target pointers cached in `SpellCastTargets`, which are refreshed only while the spell is casting: a projectile in flight kept the pointer of a target deleted meanwhile (a dead hunter's pet), and Lua read freed memory. The target is now looked up by its guid on the caster's map (`ObjectAccessor::GetUnit`, `Map::GetGameObject`/`GetCorpse`), and an item target the way `SpellCastTargets::Update` finds it: the caster player's item by guid (`TARGET_FLAG_ITEM`), or the trade partner's offered item (`TARGET_FLAG_TRADE_ITEM`, a trade enchant), found by slot `0..TRADE_SLOT_COUNT-1` before `Spell::cast` and by the item's guid (`TradeData::GetTradeSlotForItem`) after it, because `Spell::cast` runs `updateTradeSlotItem` before the cast hook and replaces the slot with the guid. |
  | `0009-vmangos-world-only-global-methods` | Server-wide functions ran in map states too: `PerformIngameSpawn`, `SendMail`, `SaveAllPlayers`, `StartGameEvent`, `StopGameEvent`, `AddVendorItem`, `VendorRemoveItem`, `VendorRemoveAllItems`, `AddTaxiPath`, `Kick`, `Ban`, `Player:KickPlayer`, `Player:LogoutPlayer` and `Player:RemoveFromGroup` write other maps, sessions, accounts or the core's vendor/taxi tables, close a session's socket or delete a player synchronously, and a map state runs on that map's thread next to other maps. They are now flagged `METHOD_REG_WORLD`, the mechanism the backend already uses for the Group and Guild methods: a map state gets a closure that raises "attempt to call method '...' that does not exist for state: N" (see Threading rules and Known limits). |
  | `0010-vmangos-null-safe-method-chains` | Null dereferences in methods: `Aura:GetCasterLevel` read `aura->GetCaster()->GetLevel()`; the caster is a guid lookup that returns null once it despawned or logged out (a DoT from a dead add), and a player caster is found on any map (now `nil` when the caster is gone or on another map than the state's). `Item:GetItemLink` read `item->GetOwner()->GetLevel()`; an ownerless item (dropped, in a chest, in the mail) has no level (the link's level field is 0). The Lua instance AI's hooks and data methods (`Update`, `IsEncounterInProgress`, `OnPlayerEnter`, `OnObjectCreate`, `OnCreatureCreate`, `Initialize`, `Load`, `Save`, `GetData`/`SetData`, `GetData64`/`SetData64`) called `instance->GetEluna()->...` unchecked; a missing state now counts as "no script" (as the creature AI does since 0007), because `Map::GetEluna` refuses the state to a thread that does not update the map; `Save` returns NULL when the state is missing (wrong thread or no Lua instance script) and leaves `lastSaveData` alone: `InstanceData::SaveToDB` skips the write on NULL, so the stored data stays (the `DungeonPersistentState::SaveToDB` that stores an empty column for NULL runs for a new instance only), and `Load` keeps its data before the state check, so the map's next update reloads it. |
  | `0011-vmangos-loot-itemtemplate-roll` | Missing for loot scripts: no `Loot` type, `GetItemTemplate` pushed nil (this pin registered no `ItemTemplate` type for VMaNGOS), `Unit:GetCritterGUID` was a "not implemented" stub, `Group:GetMembers` is world-only so no map state could list a group, and nothing told a script who won a group loot roll. Adds the `Loot`, `ItemTemplate` and `Roll` types, `PLAYER_EVENT_ON_GROUP_ROLL_REWARD_ITEM` (56) and the methods listed under "Turtle custom methods". |
  | `0012-vmangos-trinitycore-parity` | The TrinityCore script API that scripts port with. Globals `GetStateMap` (map states) and `GetPlayersOnMap` (map states; `GetStateMapId` and `GetStateInstanceId` come with 0011). `RegisterSpellEvent` for the spell events the core fires, called from `Spell.cpp`: 1 cast (after the cast hooks), 10 check cast (end of `CheckCast`; a `SPELL_FAILED_*` return cancels the cast, any other value counts as OK), 11 before cast and 12 after cast (`Spell::cast`), 20 before hit, 22 hit and 23 after hit (`DoAllEffectOnTarget`; they get the hit unit as last argument; 22 fires exactly once per successful target hit, whether the hit healed, damaged, only procced or only applied an aura), 19 effect hit and 21 effect hit target (`HandleEffects`; `true` prevents the default effect, the hit unit is the last argument); any other event number is an argument error. `Creature:GetThreat`, `ClearThreat`, `ResetAllThreat`, `FixateTarget` and `ClearFixate` (the fixate is a field of the unit that `GetTauntTarget` checks first, cleared when the creature leaves combat or dies). `Spell:IsTriggered`, `GetOriginalCaster`, `IsPositive`, `GetState`, `IsInterruptable`, `IsChannelActive`, `CalculateDamage`. `WorldObject:Data()` and `Map:Data()`: the `LuaVal` the core keeps with the object, with value semantics (a `LuaVal` stored into another is deep-copied; nesting deeper than 64 levels raises "Data(): table nested deeper than 64 levels" instead of being cut) and a detached copy left behind when the object leaves its map (`WorldObject::ResetMap`). The `ChatHandler` type: the third argument of `PLAYER_EVENT_ON_COMMAND` (`World::OnElunaCommand(ChatHandler&)`), with `SendSysMessage`, `PSendSysMessage`, `IsConsole`, `GetPlayer`, `GetSecurity`, `IsAvailable`, `HasLowerSecurity`, `GetSelected*` and more, world-only. |
- The backend is `ELUNA_VMANGOS` with `ELUNA_EXPANSION=0`.
- Lua (5.2 by default; `ELUNA_LUA_VERSION` takes lua51..lua54) is downloaded
  from lua.org at configure time with a pinned SHA-256 and linked statically.
- `BUILD_ELUNA_TESTS=ON` adds a small Lua runtime check to `ctest`.
- The `ELUNA LUA ENGINE` block of `mangosd.conf.dist` comes from
  `src/mangosd/eluna.conf.dist` and is only added in an Eluna build.

## Runtime

`Eluna.Enabled` (default 1 in an Eluna build) is read once, at startup. With 0
no Lua state is made and the bridge scripts are not registered, so each hook
site costs one null check. Scripts go in `Eluna.ScriptPath` (`./lua_scripts`);
`.reload eluna [mapId]` reloads them (`Eluna.ReloadCommand`,
`Eluna.ReloadSecurityLevel`).

The core takes `.reload eluna` over from Eluna (`World::OnElunaCommand`).
Eluna reloads its script cache on a thread of its own, while map threads make
new states (a new instance) and reload states from the same cache. The core
queues the command and runs it in `World::Update` after every map update and
async task of the tick has finished, holding the map manager's lock and the
lock under which states are made (`ElunaStateLock`), and waits for the cache
to be rebuilt before the maps update again. Each state then runs the scripts
again on its next update, on the thread that updates its map: states of
maps updated in parallel (continents, instances) reload in parallel, each in
its own `lua_State`. What they share is process-wide: Eluna's script cache
(only read then, see above) and the per-type names of Eluna's userdata
types, which every new or reloaded state registered again (a race found by
ThreadSanitizer with `.reload eluna` every 20 s; fixed by
`dep/eluna-patches/0005`, see the patch table). The world update stalls for
the time the scripts take to compile (logged).

There is one **world state** and one **state per map and instance**
(`Eluna.OnlyOnMaps` limits which maps get one).

## Threading rules

A Lua state must only ever be entered by one thread at a time.

| State | Entered from |
| --- | --- |
| World state | The world thread (the main thread during startup). `World::GetEluna()` returns nullptr on any other thread, so a hook reached from a map, async or network thread is skipped (logged once). |
| Map state | The thread updating that map, or the world thread while that map is not being updated. `Map::GetEluna()` returns nullptr to any other thread, and to the world thread while a thread is inside that map's update (logged once per map), so a hook reached from another map's thread, a `WorldAsync` task or a network thread is skipped instead of entering the state concurrently. |

What the core does to keep to that, only while `Eluna.Enabled = 1`:

- **Continent sub-threads.** `Continents.MotionUpdate.Threads` is limited to 0,
  `MapUpdate.ObjectsUpdate.MaxThreads` and `MapUpdate.VisibilityUpdate.MaxThreads`
  to 1, and `MapUpdate.Continents.MTCells.Threads` to 0, so a continent's motion,
  object, visibility and cell work runs on the map's own thread. Logged once at
  startup. Parallel updates of different continents and instances are unchanged.
- **Chat** from player accounts is handled on the world thread, as a GM's
  already is. Otherwise say/yell/emote run on map threads and whisper, party,
  guild and channel chat on the async packet thread, next to map updates, and
  chat reaches `OnChat` and, for a typed command, `OnCommand`.
- **World update.** The world state's timers and `OnWorldUpdate` run after the
  `WorldAsync` tasks have finished.
- **Weather** events go to the weather's map state.
- **Spell events** (`RegisterSpellEvent`) run in the state of the caster's map, on its
  thread, and the world state never sees them: register them in a script every map state
  loads.
- **`Data()`** is C++ memory of the object, not a Lua table: it survives `.reload eluna`.
  A handle is not invalidated with the object, so one kept by a map state is detached
  (the object gets a deep copy when it leaves the map) rather than shared.
- **`ChatHandler`** exists only in the world state, on the world thread, during the
  hook. A command parsed on a map thread gets no state and is skipped.
- **Packet hooks** (`OnPacketSend`, `OnPacketReceive`) are not bridged: they
  would run on network threads.
- `Map` and `World` cache their `Eluna*`. Eluna's `ElunaMgr` container has no
  lock, so the core only touches it to make and destroy states, under one mutex
  (`ElunaStateLock`).
- **The map's update thread owns its state.** `Map::DoUpdate` stores
  `std::this_thread::get_id()` in an atomic for the length of the update, and
  `Map::GetEluna()` hands the state only to that thread, or to the world
  thread while the map is idle (empty owner): teleports, instance switches,
  commands and the Eluna reload reach map states from the world thread after
  `sMapMgr.Update` returned or while maps are not updating. Map lifecycle
  sites that need the state regardless (`Map`'s constructor and destructor)
  use the member. This is a tripwire, not a lock: it refuses a caller that
  would enter the state from a thread that does not own it, and logs it once
  per map, instead of corrupting the state silently.
- **Objects of another map.** A map state only uses objects on its own map
  (`dep/eluna-patches/0002`): a player, creature, gameobject, item (by its
  owner) or `Map` userdata of another map is refused when a method is called
  on it, with the Lua error "... got an object on another map ...: refused".
  `GetPlayerByGUID`, `GetPlayerByName` and `GetPlayersInWorld` still return such
  players; scripts can test them with `pcall`. The world state may use objects
  of any map: it only runs while no map updates. Not checked: groups and
  guilds are not bound to a map, so in a map state their methods can still
  reach players on other maps (`SendPacket` to the members, member and leader
  changes); `Spell`, `Aura` and `BattleGround` userdata are not checked either
  (they are only pushed by hooks on their own map; `Aura:GetCasterLevel` does
  return nil for a caster on another map, `dep/eluna-patches/0010`).
- **Timed events of objects** (`obj:RegisterEvent`). An object's events live in
  the event manager of the state that registered them, and only the state of
  the object's own map can register map events on it. The object never keeps a
  pointer into a state that can outlive it (it keeps a `weak_ptr` to the
  state's lifetime token, `Map/World::GetElunaLifetime`). Its map events are
  dropped when it leaves the map (teleport, despawn, logout), on the thread
  that owns that map, so a timer never runs for a player on another map and an
  instance can be unloaded while a player who was in it is elsewhere. World
  state events of an object destroyed on a map thread are dropped on the world
  thread before the world state's timers run. The Lua function of each
  pending timer is freed then as well (`dep/eluna-patches/0006`). A script
  that kept the object in a Lua variable gets an invalid object from it
  ("pointer to nonexisting (invalidated) object"), never the C++ pointer:
  in this core a userdata is only valid during the call that pushed it.
  `GAMEOBJECT_EVENT_ON_SPAWN` fires once the gameobject is in the world (in
  `GameObject::AddToWorld` after `Create`, and on respawn), so a timer can
  be registered in it. Ships and zeppelins
  (`MO_TRANSPORT`) cannot have map events (`RegisterEvent` returns nil): they
  move between the maps of a continent.

## Rules for script authors

1. Never keep a userdata (`Player`, `Creature`, `Spell`, `Aura`, `Map`, item)
   in a variable across a timer or a later hook: keep the guid (and the map
   and instance id) and find the object again; check `GetMapId()` and
   `GetInstanceId()` before you use it. A userdata is only valid during the
   call that pushed it, so a kept one reads as "pointer to nonexisting
   (invalidated) object".
2. A map state only touches objects of its own map. Wrap the result of
   `GetPlayerByGUID`, `GetPlayerByName` and `GetPlayersInWorld` in `pcall`:
   a method on an object of another map is refused ("... got an object on
   another map ...: refused").
3. Anything server-wide (a spawn on another map, mail, game events, vendors,
   taxi paths, bans, save-all, logout) belongs in the world state: server,
   command and world-update events, never creature, map, instance, player or
   item events. Since `dep/eluna-patches/0009` a map state is refused
   ("attempt to call method '...' that does not exist for state: N").
4. Kick and log out only from the world state: `Player:KickPlayer` and
   `Player:LogoutPlayer` (and `Player:RemoveFromGroup`, which cancels and counts the
   leaver's loot rolls) are world-only since `dep/eluna-patches/0009`
   (a map state is refused), and `LogoutPlayer` deletes the player
   synchronously even there, so save or kick instead. Treat
   `Aura:GetCasterLevel` as possibly nil (it is nil once the caster is gone,
   `dep/eluna-patches/0010`) and check `Aura:GetCaster()`.
5. Read `Spell:GetTarget()` only in the cast hook (`dep/eluna-patches/0008`
   makes it a guid lookup; treat nil as normal).
6. Wrap every hook and timer body in `pcall` (the toolkit's `TT.guard`): Lua is
   compiled as C here, so an error that escapes unwinds with `longjmp` through
   C++ frames and skips their destructors.
7. Do not run `.reload eluna` or Lua `ReloadEluna()` during a walk or a raid:
   a reload stalls the world tick while the scripts compile.
8. Put nothing but scripts in `lua_scripts`: no backups, no extra folders (a
   stray `tt.bak` once broke a toolkit load, and a copied state file next to
   the scripts is loaded again in every state).

9. In spell events (`RegisterSpellEvent`) only read, set data, add threat, or queue
   work with `RegisterEvent` / `CreateLuaEvent` (delay of 1 ms or more). Do not
   despawn, kill-and-remove, teleport or remove the caster or the target: the C++
   frames of the cast still hold them.
10. Don't keep `Data()` handles: call `obj:Data()` again in every hook and timer. A
    handle (or a table you got out of it with `Get`) kept in a variable is a detached
    copy once the object has left the map.
11. A Lua command handler (`PLAYER_EVENT_ON_COMMAND`, third argument) checks
    `handler:IsAvailable(level)` first, and answers through the handler
    (`handler:SendSysMessage`, `handler:PSendSysMessage`), which also reaches the
    console and SOAP. Only commands the core does not know reach Lua, and players only
    do when `PlayerCommands` is on.

## Turtle custom methods

Added by `dep/eluna-patches/0011` for loot scripts (the methods are in the
submodule copy; the core side is the two roll call sites in `Group.cpp`).

| Method | What it does |
| --- | --- |
| `Creature:GetLoot()` | the corpse's `Loot` |
| `Group:GetLootMethod()` | the group's loot method: 0 free for all, 1 round robin, 2 master loot, 3 group loot, 4 need before greed. Works in map states |
| `Group:GetLootThreshold()` | the group's loot quality threshold (0 poor to 6 artifact); before a roll `is_underthreshold` is false for every item, so compare the item's quality with this |
| `Creature:StartGroupLootRoll(player)` | map states only. Starts the group's Need/Greed (or Group Loot) rolls for the corpse the way the first loot window a member opens does (`Group::GroupLoot` / `NeedBeforeGreed`: roll popups, `is_blocked`), and sets the corpse's `lootForBody` flag so a later loot open never starts them twice. `player` must be in the group that owns the corpse's loot. Returns `true` when rolls were started (even if bots already ended them); `false`, with `lootForBody` untouched, for a live corpse, `CreatureAI::CanBeLooted()` false (a boss in a fake death), loot disabled on the map, pickpocket loot, rolls already started, another group, master loot / free for all / round robin, or when no item at or above the threshold has a member who can roll. The core then gives the item to the winner as usual |
| `Creature:AllLootRemovedFromCorpse()` | tells the corpse its loot is gone (stops the sparkle, starts the despawn timer) |
| `Loot:GetMoney()`, `SetMoney(copper)` | the coin; 0 closes it in open loot windows |
| `Loot:GetUnlootedCount()`, `SetUnlootedCount(n)` | items not looted yet (0..255); skinning needs 0 |
| `Loot:GetLootType()` | `LootType` (1 corpse, 6 skinning; `clear()` never resets it) |
| `Loot:GetItems()`, `GetQuestItems()` | tables of `{ id, index, count, needs_quest, is_looted, is_blocked, freeforall, is_underthreshold, roll_winner_guid }` (`index` is the client slot, quest items follow the normal ones; `roll_winner_guid` is nil unless a roll winner with full bags is waiting) |
| `Loot:SetItemLooted(itemId, count, looted = true)` | marks the first normal item with that entry (and count, 0 = any) and tells open windows; returns whether one matched. It does not change the unlooted count: keep it in step with `Loot:SetUnlootedCount`, or the corpse is never marked fully looted |
| `Loot:Clear()` | empties the loot (`Loot::clear()`) and empties the looter list, which cancels the running rolls' link to their loot. It does not close loot windows that are already open: a player who has one open still sees the old slots until they close it. A cancelled roll is not removed from the group at once: it stays in the group's roll list until the next vote on it, its timeout or the group disbanding, and then ends without a winner |
| `Player:IsAtGroupRewardDistance(obj)` | within the group reward distance (a dead player counts by corpse) |
| `Player:IsAtLootRewardDistance(obj)` | the same, and for a creature also an allowed looter |
| `Player:IsBot()` | headless session, or `ScriptMgr::IsBotManaged` (a hired TortoiseBots bot) |
| `Unit:GetCritterGUID()` | guid of the mini pet (companion), or nil |
| `Group:GetMembersOnMap()` | members in the world and on the state's map; works in a map state, where `Group:GetMembers` is refused |
| `GetStateMapId()`, `GetStateInstanceId()` | the calling state's map id (-1 = world state) and instance id |
| `GetItemTemplate(id)`, `Item:GetItemTemplate()`, `Item:GetTemplate()` | an `ItemTemplate` (`GetItemId`, `GetName([locale])`, `GetQuality`, `GetClass`, `GetSubClass`, `GetFlags`, `GetSellPrice`, `GetBuyPrice`, `GetBuyCount`, `GetInventoryType`, `GetItemLevel`, `GetRequiredLevel`, `GetBonding`, `GetMaxStackSize`, `GetDisplayId`, `GetAllowableClass`, `GetAllowableRace`) |
| event 56 `PLAYER_EVENT_ON_GROUP_ROLL_REWARD_ITEM` | `(event, player, item, count, voteType, roll)`: a need (1) or greed (2) winner got the item; fired at the end of the roll, after it left the group's roll list, so a handler cannot count it twice. Not fired when the winner's bags are full, on a single-looter roll or when all pass. `Roll`: `GetItemId`, `GetItemRandomPropId`, `GetItemRandomSuffix` (0), `GetItemCount`, `GetItemSlot`, `GetPlayerVote(guid)`, `GetPlayerVoteGUIDs`, `GetTotalPlayersRolling`, `GetTotalNeed`, `GetTotalGreed`, `GetTotalPass`, `GetRollVoteMask` (7), `GetLootedTargetGUID` |

Lifetime: a `Loot` or `Roll` is only valid during the call that pushed it (the
`Loot` lives in its creature, the `Roll` is deleted right after the hook). Never
keep one; call `corpse:GetLoot()` again every time. The roll event goes to the
winner's map state; a vote counted on the world thread while that map updates
finds no state and is skipped (see Threading rules).

## Where the hooks are

- `src/game/ElunaScriptBridge.cpp` registers Eluna with the core's `ScriptObject`
  registries (`WorldScript`, `PlayerScript`, `AllCreatureScript`, `UnitScript`,
  `AllGameObjectScript`, `AllItemScript`, `LootScript`,
  `AuctionHouseScript`, `AllBattlegroundScript`, `GroupScript`, `GuildScript`).
- `ScriptMgr.cpp` hands gossip, quest accept, dialog status, gameobject use,
  area triggers, dummy effects and creature AI to Lua after the C++ scripts.
- Direct `#ifdef ENABLE_ELUNA` sites for what the registries do not reach: the
  world and map state lifecycle (`World.cpp`, `WorldRunnable.cpp`, `Map.cpp`),
  instance data, chat commands, trade, mail, quest abandon, summons, item
  expiry, resurrection, skills, quest status, gameobject state, weather and the
  per-object Lua event processors (`Object.h/.cpp`), battleground start/end
  and game events (`GameEventMgr.cpp`; this core's `GameEventScript` registry
  is keyed by script id and also serves spell "send event" scripts).
- Spell events: `Spell.cpp` calls `OnCheckCast` (end of `CheckCast`), `OnBeforeCast`,
  `OnSpellCast` and `OnAfterCast` (`Spell::cast`), `OnBeforeSpellHit`, `OnSpellHit` and
  `OnAfterSpellHit` (`DoAllEffectOnTarget`) and `OnEffectHit` / `OnEffectHitTarget`
  (`HandleEffects`), on the caster's state.
- Commands: `ChatHandler::ExecuteCommand` (`Chat.cpp`) passes itself to
  `World::OnElunaCommand(ChatHandler&)`, which reaches the world state's
  `PLAYER_EVENT_ON_COMMAND` with the handler as third argument.
- `src/game/Eluna/CustomMethods.h` registers `SpellInfo` methods backed by
  `spell_template`.

## Instance scripts

A map's C++ instance script comes first, as C++ creature scripts come before
Lua creature AI. Lua instance events (`RegisterMapEvent`,
`RegisterInstanceEvent` with `INSTANCE_EVENT_*`) run on maps without a C++
instance script; on a map with one the server logs "Eluna: map M has a C++
instance script ...; its Lua map/instance events are not used." The C++ boss
and door scripts cast the map's instance data to their own instance class, so
a Lua instance script in its place would break them (and their casts would
read the wrong object).

Lua instance data is saved to the `instance` (or `world`) table through the
core's `InstanceData::SaveToDB` (`map:SaveInstanceData()`), and loaded with
the map. The Lua data and the instance's events are freed when the map is
destroyed, after its objects are unloaded.

## Sample scripts

- `dep/lualib/tests/eluna_port_smoke.lua` logs `ELUNA_SMOKE ...` lines for a
  login, a command (`.elunasmoke`), gossip on creature 823, map updates, timed
  events and `.reload eluna`.
- `dep/lualib/tests/eluna_review_603_smoke.lua` logs `ELUNA_603 ...` lines for
  the fixes after the #603 review: player timers across teleports and instance
  unloads, refusal of objects of another map, instance data across a restart,
  `.reload eluna` while instances open, `CREATURE_EVENT_ON_HIT_BY_SPELL` and
  `GAMEOBJECT_EVENT_ON_SPAWN` (on spawn and respawn), Lua references of
  despawned creatures returning to zero, a timer registered in a
  gameobject's spawn event, a check that every hook of the file is still
  in its state's registry after reloads, the refusal of the server-wide
  functions in a map state (`refused=<name>`, patch 0009) and
  `Aura:GetCasterLevel` of an aura whose caster despawned (nil, patch 0010).
  The header says what to do and what to expect.
- `dep/lualib/tests/eluna_lootpet_smoke.lua` logs `ELUNA_LOOT ...` lines for
  the loot methods of patch 0011 (`.lootsmoke` and the group roll event). The
  header says what to do and what to expect.
- `dep/lualib/tests/eluna_parity_smoke.lua` logs `ELUNA_0012 <group>` lines for the
  patch 0012 API: state identity and `GetPlayersOnMap`, spell events and their order,
  creature threat and fixate (`.elunasmoke threat`), the `Spell` getters, `Data()`
  across teleports and reloads, and a Lua command with the `ChatHandler`
  (`.paritysmoke`, in game and from the console). The header says what to do.

## Known limits

- **Global functions that change server-wide data are world-only**
  (`dep/eluna-patches/0009`): `PerformIngameSpawn`, `SendMail`,
  `SaveAllPlayers`, `StartGameEvent`, `StopGameEvent`, `AddVendorItem`,
  `VendorRemoveItem`, `VendorRemoveAllItems`, `AddTaxiPath`, `Kick`, `Ban`,
  `Player:KickPlayer`, `Player:LogoutPlayer` and `Player:RemoveFromGroup` write other maps, sessions,
  accounts or the core's vendor/taxi tables, close a session's socket or
  delete a player synchronously. A map state registers a refusal closure for
  them instead of the method: the call raises "attempt to call method '...'
  that does not exist for state: N". Call them from the world state (server,
  command and world-update events), which only runs on the world thread while
  no map updates. `Player:KickPlayer` and `Player:LogoutPlayer` are both
  world-only; `LogoutPlayer` deletes the player synchronously inside the
  calling hook, so scripts should kick or save instead of logging a player out.
- **`Group:GetMembers` is world-only** (it is flagged `METHOD_REG_WORLD`): in a
  map state use `Group:GetMembersOnMap()` (`dep/eluna-patches/0011`), which lists
  the members on the state's map.
- **`Aura:GetCasterLevel` can be nil** (`dep/eluna-patches/0010`): the caster
  is looked up by its guid, so it is gone once the caster despawned or logged
  out (a DoT from a dead add), and it is nil for a caster on another map than
  the state's. Read `Aura:GetCaster()` and check it, or treat nil as normal.
- **`Item:GetItemLink` of an ownerless item** (`dep/eluna-patches/0010`):
  an item without an owner (dropped, in a chest, in the mail) has no player
  level; the link's level field is 0.
- **ThreadSanitizer** (`.reload eluna` every 20 s and a console reload every
  60 s, four players teleporting between continents and instances, all map
  thread settings at 4; 17 min before 0005, 15 min with it): before 0005, 675 reports in
  `ElunaTemplate<T>::Register`/`Push` (one per type per parallel reload); with
  0005, none, and no report has Eluna's own code at a racing access (the
  remaining ones there are the core's `Log::outTimestamp`/`localtime` and
  `ThreadPool::status`, reached from `PrintInfo` and the map pool). Reports whose stacks pass through Lua
  are this core's races between instance threads (pet summon:
  `ObjectMgr::GeneratePetNumber`, `Pet::Pet`; waypoint groups:
  `CreatureGroup::SetLastReachedWaypoint`; terrain and navmesh loading:
  `TerrainInfo::GetGrid`, `dtNavMesh`), reached the same way without Lua. In
  those reports the Lua frames sit below the map thread's own
  `ThreadPool::worker::loop`, which no Lua call can run: they are left over
  from an earlier Lua error (a refused cross-map object inside `pcall`; Lua
  unwinds with `longjmp` through C++ frames), not callers.

- **Lua creature AI replaces the creature's AI.** A creature with any
  `RegisterCreatureEvent` gets Eluna's Lua AI (`ElunaCreatureAI`, built on
  `AggressorAI` through `src/game/AI/BasicAI.h`) instead of its EventAI,
  GuardAI or other database AI; C++ creature scripts still come first. Events
  the Lua script does not handle fall back to `AggressorAI`, not to the
  creature's own AI. Keeping the original AI as the fallback would mean two AI
  objects per creature with the core reaching only the outer one (combat
  movement, melee and spell-list state set by scripts through `AI()`), which is
  not safe without changes to the AI classes. Script the whole creature in Lua,
  or keep it on its database AI.
- **Spell script events** (`SPELL_EVENT_*`): `RegisterSpellEvent` accepts 1 (cast),
  10 (check cast), 11 (before cast), 12 (after cast), 19 (effect hit), 20 (before hit),
  21 (effect hit target), 22 (hit) and 23 (after hit). The aura events 2-9, target
  selection 13-15, effect launch 16/17 and calc absorb 18 are not fired on this core
  (a different aura model, no launch phase, no `DamageInfo`), so registering them is an
  argument error. Events are keyed by spell id and run in the caster's map state.
- **`Creature:FixateTarget` is emulated** through the taunt target: the creature
  attacks the fixated unit while it is a valid target on its threat list, without evading,
  until `ClearFixate`, or until it leaves combat or dies. A fixated unit that is not valid
  for a moment is skipped, not forgotten.

## Not done yet

- A soak test with every thread setting above 1 and 500 bots, with the
  negative control (thread limit removed, it must crash).
- A ThreadSanitizer run.
- Tick-time numbers: built without Eluna, Eluna disabled, Eluna enabled.
- A Windows/MSVC build.
- Patch application on Windows: the patch files are LF (`.gitattributes` keeps
  them LF in a checkout). The apply and verify commands were run (ported to
  PowerShell, CMake itself was not run) with Git for Windows 2.55 on CRLF
  sources: they work with the default `core.autocrlf=true` and with the
  `patch.exe` fallback. With `core.autocrlf=false` and CRLF sources, `git apply`
  cannot match the LF patches: configure stops with "does not apply" and the
  copy is removed, it never builds unpatched. Use `core.autocrlf=true` (the Git
  for Windows default) or check out `dep/Eluna` with LF endings.
