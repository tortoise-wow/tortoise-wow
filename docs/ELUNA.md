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
git submodule update --init --recursive src/modules/Eluna
cmake -S . -B build -DBUILD_ELUNA=ON            # ELUNA_LUA_VERSION=lua52 by default
cmake --build build --target mangosd
```

- The engine is the `src/modules/Eluna` submodule, pinned to `1b06f28` (branch
  `cmangos-spell-update` of ElunaLuaEngine/Eluna). Eluna master at the time
  (`58d7652`) declares TrinityCore aura and proc types unguarded and does not
  build for this core.
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
`.reload eluna` reloads them (`Eluna.ReloadCommand`, `Eluna.ReloadSecurityLevel`).

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
  lock, so the core only touches it to make and destroy states, under one mutex.

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

## Sample script

`dep/lualib/tests/eluna_port_smoke.lua` logs `ELUNA_SMOKE ...` lines for a
login, a command (`.elunasmoke`), gossip on creature 823, map updates, timed
events and `.reload eluna`.

## Not done yet

- A soak test with every thread setting above 1 and 500 bots, with the
  negative control (thread limit removed, it must crash).
- A ThreadSanitizer run.
- Tick-time numbers: built without Eluna, Eluna disabled, Eluna enabled.
- A Windows/MSVC build.
