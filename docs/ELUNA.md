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
  the seven markers. The copy is remade when a submodule file, a patch or
  `ElunaSources.cmake` changes. Each patch names its Eluna base commit; 0001,
  0003, 0004, 0005, 0006 and 0007 are candidates to upstream to Eluna, 0002 is
  VMaNGOS-specific (other cores get the check from trackable pointers).

  | Patch | Fixes |
  | --- | --- |
  | `0001-eventmgr-...` | An object that left a state and came back before the state's next cleanup got a new event processor that was destroyed at once while still listed (use after free). |
  | `0002-vmangos-refuse-objects-of-another-map` | A map state could use objects of another map (see Threading rules). |
  | `0003-vmangos-instance-data-...` | `Save`, `GetData` and `GetData64` of a Lua instance script did not override this core's non-const `InstanceData` methods: instance data was saved empty and lost at a restart, and C++ callers read 0. |
  | `0004-vmangos-creature-spellhit-...` | `CREATURE_EVENT_ON_HIT_BY_SPELL` never fired: the Lua AI's `SpellHit` did not override this core's `CreatureAI::SpellHit(WorldObject*, ...)`. |
  | `0005-template-name-and-decoding-table-...` | Data race: every Lua state stored the per-type `ElunaTemplate<T>::tname` (one static per type for the whole process) in `Register`, so map states made or reloaded on different map threads wrote it at once while other states read it in `Push`/`Check`; also the lazy build of the base-64 table used for instance data. Both are now set once (`std::call_once`, a function-local static). |
  | `0006-eventmgr-free-lua-references-...` | Lua function references of timers leaked: when an object left its map or was destroyed with a timer pending, its event processor was removed with the events set to `LUAEVENT_STATE_ERASE`, which skips `luaL_unref` (meant for a closed or reloaded state). Each such timer's function, and what its upvalues reach, stayed in the state's registry until the state closed. They are now unreferenced (`LUAEVENT_STATE_ABORT`) on the thread that updates the state. A reload still marks every pending event `LUAEVENT_STATE_ERASE` (its references belong to the closed state), now also those of processors replaced by 0001 and waiting for the next cleanup, which are not in the processor list; otherwise that cleanup would unreference the old state's reference numbers in the new state. |
  | `0007-creature-ai-no-state-falls-back-...` | Crash (null `Eluna*`): every hook of the Lua creature AI called `me->GetEluna()->...`, but `WorldObject::GetEluna` is null while the creature is not in the world, and `Creature::Update` can still run the AI then (seen on `.npc delete` of a scripted creature: SIGSEGV in `Eluna::UpdateAI`). A missing state now counts as not handled, so the native AI runs, as for a creature without a script. |
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
| Map state | The thread updating that map, or the world thread while no map is updating. |

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
- **Packet hooks** (`OnPacketSend`, `OnPacketReceive`) are not bridged: they
  would run on network threads.
- `Map` and `World` cache their `Eluna*`. Eluna's `ElunaMgr` container has no
  lock, so the core only touches it to make and destroy states, under one mutex
  (`ElunaStateLock`).
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
  (they are only pushed by hooks on their own map).
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
  gameobject's spawn event, and a check that every hook of the file is still
  in its state's registry after reloads. The header says what to do and what to expect.

## Known limits

- **Global functions that change server-wide data** (for example
  `AddTaxiPath`, which adds taxi nodes and paths to the core's tables) are
  callable in every state, as upstream. In a map state they run on that map's
  thread next to other maps: call them from the world state (server events),
  which only runs while no map updates.
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
- **Spell script events** (`SPELL_EVENT_*`): this Eluna pin has no
  `RegisterSpellEvent` for VMaNGOS, so they cannot be registered.

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
