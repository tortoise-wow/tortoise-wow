---
layout: default
title: Module Hook API
---

# Module Hook API

Modules live under `modules/<module-name>/` and are discovered when the module
has a `src/` directory. The module system lets a module register C++ scripts
without editing the core script loader directly.

Create a module skeleton with:

```sh
modules/create_module.sh mod-example
```

The generated module contains a `src/` file, a config template, SQL migration
folders, and a module CMake file. Only `src/` is required for discovery.

## Build Modes

Modules are controlled by the global `MODULES` option and optional per-module
cache variables.

```sh
cmake -S . -B build -DMODULES=static
cmake -S . -B build -DMODULES=dynamic
cmake -S . -B build -DMODULES=disabled
```

Per-module variables are generated from the module name. For example,
`mod-example` becomes:

```sh
cmake -S . -B build -DMODULE_MOD_EXAMPLE=static
```

Valid modes are `disabled`, `static`, `dynamic`, and `default`.

## Script Loader Function

Each module exposes a loader function named after the module:

```cpp
void Addmod_exampleScripts()
{
    new ExampleWorldScript();
}
```

The function name is `Add<module-name-sanitized>Scripts`. Characters other than
letters, numbers, and underscores are converted to underscores.

The generated CMake loader calls each enabled module loader through
`AddModulesScripts()`.

## Basic Hook Script

Most hook classes are declared in `src/game/ScriptObjects.h`. Include that file,
derive from the script type you need, override the relevant hook methods, and
instantiate the class in the module loader.

```cpp
#include "ScriptObjects.h"
#include "Log.h"

namespace
{
    class ExampleWorldScript : public WorldScript
    {
    public:
        ExampleWorldScript()
            : WorldScript("mod_example_world", { WORLDHOOK_ON_STARTUP })
        {
        }

        void OnStartup() override
        {
            sLog.outString("[mod-example] module loaded.");
        }
    };
}

void Addmod_exampleScripts()
{
    new ExampleWorldScript();
}
```

For `WorldScript`, `PlayerScript`, `UnitScript`, and `ServerScript`, passing an
empty hook list enables every hook for that script type. Passing explicit hook
IDs limits dispatch to only those hooks.

## World Hooks

Use `WorldScript` for server lifecycle and global world events.

| Hook ID | Method |
| --- | --- |
| `WORLDHOOK_ON_OPEN_STATE_CHANGE` | `OnOpenStateChange(bool open)` |
| `WORLDHOOK_ON_AFTER_CONFIG_LOAD` | `OnAfterConfigLoad(bool reload)` |
| `WORLDHOOK_ON_LOAD_CUSTOM_DATABASE_TABLE` | `OnLoadCustomDatabaseTable()` |
| `WORLDHOOK_ON_BEFORE_CONFIG_LOAD` | `OnBeforeConfigLoad(bool reload)` |
| `WORLDHOOK_ON_MOTD_CHANGE` | `OnMotdChange(std::string& newMotd)` |
| `WORLDHOOK_ON_SHUTDOWN_INITIATE` | `OnShutdownInitiate(uint32 shutdownMask, uint32 exitCode)` |
| `WORLDHOOK_ON_SHUTDOWN_CANCEL` | `OnShutdownCancel()` |
| `WORLDHOOK_ON_UPDATE` | `OnUpdate(uint32 diff)` |
| `WORLDHOOK_ON_STARTUP` | `OnStartup()` |
| `WORLDHOOK_ON_SHUTDOWN` | `OnShutdown()` |
| `WORLDHOOK_ON_AFTER_UNLOAD_ALL_MAPS` | `OnAfterUnloadAllMaps()` |
| `WORLDHOOK_ON_BEFORE_WORLD_INITIALIZED` | `OnBeforeWorldInitialized()` |

## Player Hooks

Use `PlayerScript` for player lifecycle, chat, rewards, movement, and spell
events.

| Hook ID | Method |
| --- | --- |
| `PLAYERHOOK_ON_PLAYER_JUST_DIED` | `OnPlayerJustDied(Player* player)` |
| `PLAYERHOOK_ON_PLAYER_RELEASED_GHOST` | `OnPlayerReleasedGhost(Player* player)` |
| `PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST` | `OnPlayerCompleteQuest(Player* player, Quest const* quest)` |
| `PLAYERHOOK_ON_PVP_KILL` | `OnPVPKill(Player* killer, Player* killed)` |
| `PLAYERHOOK_ON_CREATURE_KILL` | `OnCreatureKill(Player* killer, Creature* killed)` |
| `PLAYERHOOK_ON_LEVEL_CHANGED` | `OnLevelChanged(Player* player, uint8 oldLevel)` |
| `PLAYERHOOK_ON_TALENTS_RESET` | `OnTalentsReset(Player* player, bool noCost)` |
| `PLAYERHOOK_ON_BEFORE_UPDATE` | `OnBeforeUpdate(Player* player, uint32 diff)` |
| `PLAYERHOOK_ON_UPDATE` | `OnUpdate(Player* player, uint32 diff)` |
| `PLAYERHOOK_ON_MONEY_CHANGED` | `OnMoneyChanged(Player* player, int32& amount)` |
| `PLAYERHOOK_ON_GIVE_EXP` | `OnGiveXP(Player* player, uint32& amount, Unit* victim)` |
| `PLAYERHOOK_ON_REPUTATION_CHANGE` | `OnReputationChange(Player* player, uint32 factionId, int32& standing)` |
| `PLAYERHOOK_ON_LEARN_SPELL` | `OnLearnSpell(Player* player, uint32 spellId)` |
| `PLAYERHOOK_ON_FORGOT_SPELL` | `OnForgotSpell(Player* player, uint32 spellId)` |
| `PLAYERHOOK_ON_DUEL_REQUEST` | `OnDuelRequest(Player* target, Player* challenger)` |
| `PLAYERHOOK_ON_DUEL_START` | `OnDuelStart(Player* player1, Player* player2)` |
| `PLAYERHOOK_ON_DUEL_END` | `OnDuelEnd(Player* winner, Player* loser, uint32 type)` |
| `PLAYERHOOK_ON_BEFORE_SEND_CHAT_MESSAGE` | `OnBeforeSendChatMessage(Player* player, uint32& type, uint32& language, std::string& message)` |
| `PLAYERHOOK_ON_EMOTE` | `OnEmote(Player* player, uint32 emote)` |
| `PLAYERHOOK_ON_TEXT_EMOTE` | `OnTextEmote(Player* player, uint32 textEmote, uint32 emoteNum, ObjectGuid guid)` |
| `PLAYERHOOK_ON_SPELL_CAST` | `OnSpellCast(Player* player, Spell* spell, bool skipCheck)` |
| `PLAYERHOOK_ON_LOGIN` | `OnLogin(Player* player)` |
| `PLAYERHOOK_ON_BEFORE_LOGOUT` | `OnBeforeLogout(Player* player)` |
| `PLAYERHOOK_ON_LOGOUT` | `OnLogout(Player* player)` |
| `PLAYERHOOK_ON_CREATE` | `OnCreate(Player* player)` |
| `PLAYERHOOK_ON_DELETE` | `OnDelete(ObjectGuid guid, uint32 accountId)` |
| `PLAYERHOOK_ON_SAVE` | `OnSave(Player* player)` |
| `PLAYERHOOK_ON_UPDATE_ZONE` | `OnUpdateZone(Player* player, uint32 newZone, uint32 newArea)` |
| `PLAYERHOOK_ON_UPDATE_AREA` | `OnUpdateArea(Player* player, uint32 oldArea, uint32 newArea)` |
| `PLAYERHOOK_ON_MAP_CHANGED` | `OnMapChanged(Player* player)` |
| `PLAYERHOOK_ON_BEFORE_TELEPORT` | `OnBeforeTeleport(Player* player, uint32 mapId, float x, float y, float z, float orientation)` |
| `PLAYERHOOK_ON_LOOT_ITEM` | `OnLootItem(Player* player, Item* item, uint32 count, ObjectGuid lootGuid)` |

## Unit Hooks

Use `UnitScript` for combat, aura, damage, and healing hooks.

| Hook ID | Method |
| --- | --- |
| `UNITHOOK_ON_HEAL` | `OnHeal(Unit* healer, Unit* receiver, uint32& gain)` |
| `UNITHOOK_ON_DAMAGE` | `OnDamage(Unit* attacker, Unit* victim, uint32& damage)` |
| `UNITHOOK_MODIFY_MELEE_DAMAGE` | `ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage)` |
| `UNITHOOK_MODIFY_SPELL_DAMAGE_TAKEN` | `ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage, SpellEntry const* spellInfo)` |
| `UNITHOOK_MODIFY_HEAL_RECEIVED` | `ModifyHealReceived(Unit* target, Unit* healer, uint32& heal, SpellEntry const* spellInfo)` |
| `UNITHOOK_ON_AURA_APPLY` | `OnAuraApply(Unit* unit, Aura* aura)` |
| `UNITHOOK_ON_AURA_REMOVE` | `OnAuraRemove(Unit* unit, Aura* aura)` |
| `UNITHOOK_ON_UNIT_UPDATE` | `OnUnitUpdate(Unit* unit, uint32 diff)` |
| `UNITHOOK_ON_UNIT_ENTER_COMBAT` | `OnUnitEnterCombat(Unit* unit, Unit* victim)` |
| `UNITHOOK_ON_UNIT_EXIT_COMBAT` | `OnUnitExitCombat(Unit* unit)` |
| `UNITHOOK_ON_UNIT_DEATH` | `OnUnitDeath(Unit* unit, Unit* killer)` |

## Server Hooks

Use `ServerScript` for network lifecycle and packet gates.

| Hook ID | Method |
| --- | --- |
| `SERVERHOOK_ON_NETWORK_START` | `OnNetworkStart()` |
| `SERVERHOOK_ON_NETWORK_STOP` | `OnNetworkStop()` |
| `SERVERHOOK_ON_SOCKET_OPEN` | `OnSocketOpen(WorldSocket* socket)` |
| `SERVERHOOK_ON_SOCKET_CLOSE` | `OnSocketClose(WorldSocket* socket)` |
| `SERVERHOOK_CAN_PACKET_SEND` | `CanPacketSend(WorldSession* session, WorldPacket const& packet)` |
| `SERVERHOOK_CAN_PACKET_RECEIVE` | `CanPacketReceive(WorldSession* session, WorldPacket const& packet)` |

## Database-Bound Scripts

Some script classes are bound to database script names. These are registered in
C++, but only become active for rows that reference the script name.

| Class | Common use |
| --- | --- |
| `CreatureScript` | Creature gossip, quests, dialog status, and creature AI. |
| `GameObjectScript` | GameObject gossip, quest interactions, state changes, and GameObject AI. |
| `ItemScript` | Item quest accept, use, spell use, remove, and expire behavior. |
| `SpellScriptLoader` | Creates `SpellScript` and `AuraScript` instances for spell hooks. |
| `AreaTriggerScript` | Handles scripted area trigger activation. |
| `ConditionScript` | Handles custom condition checks. |
| `GameEventScript` | Handles game event start, stop, and update. |
| `BattlegroundScript` | Creates battleground implementations. |
| `InstanceMapScript` | Creates instance data for scripted instance maps. |
| `TransportScript` | Handles transport passengers and relocation. |
| `WeatherScript` | Handles weather changes. |
| `OutdoorPvPScript` | Registers an outdoor PvP script name. |

Database-bound scripts return `true` from `IsDatabaseBound()`. After the
database loads, the registry resolves the script object's name through
`sScriptMgr.GetScriptId()`. If the name is not present in the database script
name list, the server logs an error and does not install that script object.

## Global Script Classes

These script classes do not use explicit hook ID enums, but can still be
registered by modules.

| Class | Hooks |
| --- | --- |
| `CommandScript` | `GetCommands()` |
| `ModuleScript` | Marker script class for module registration. |
| `AccountScript` | Account login, failed login, email change, and password change events. |
| `AllCommandScript` | `CanExecuteCommand()` |
| `AllMapScript` | Map create, destroy, player enter/leave, and map update events. |
| `WorldMapScript` | Map-specific create, destroy, grid, player, and update hooks. |
| `AllCreatureScript` | All-creature update, add/remove world, gossip gate, and AI override hooks. |
| `AllGameObjectScript` | All-gameobject add/remove world, update, gossip gates, and AI override hooks. |
| `AllItemScript` | Item use, item quest accept, and item remove gates. |
| `AllSpellScript` | Spell prepare, cast, and cancel hooks. |
| `DatabaseScript` | `OnAfterDatabasesLoaded(uint32 updateFlags)` |
| `GlobalScript` | Item delete, spell custom attributes, and instance id removal hooks. |
| `MiscScript` | Object/player construction, item creation, and soulbound checks. |
| `FormulaScript` | Honor, gray level, XP gain, and group rate calculations. |
| `LootScript` | Money loot events, mutating items before they are added, and taking over loot generation via `OnBeforeLootGenerated`. |
| `AuctionHouseScript` | Auction add, remove, success, and expire events. |
| `AllBattlegroundScript` | Battleground start and end events. |
| `GroupScript` | Group member, leader, and disband events. |
| `GuildScript` | Guild member, create, and disband events. |
| `MailScript` | Mail draft send hook. |
| `MovementHandlerScript` | Player movement packet handling hook. |
| `PetScript` | Pet add/remove world events. |
| `TicketScript` | Ticket create, close, and resolve events. |

## Spell and Aura Scripts

`SpellScriptLoader` creates per-use `SpellScript` and `AuraScript` objects:

```cpp
class ExampleSpellLoader : public SpellScriptLoader
{
public:
    ExampleSpellLoader() : SpellScriptLoader("spell_example") {}

    SpellScript* GetSpellScript() const override
    {
        return new ExampleSpellScript();
    }
};
```

`SpellScript` hooks cover cast checks, target filtering, power cost, proc
handling, hit handling, effect execution, summon handling, dispels, totems, and
spell finish events.

`AuraScript` hooks cover aura initialization, value and duration calculation,
apply/remove, proc checks, absorb handling, periodic ticks, periodic amount
calculation, threat, mana regeneration, and area aura target checks.

## Config and SQL

Module config templates live in:

```text
modules/<module-name>/conf/<module-name>.conf.dist
```

Enabled module config templates are installed as `.conf.dist` files. Copy or
move the template to the same name without `.dist` before expecting the server
to load it.

Module SQL migrations live in:

```text
modules/<module-name>/data/sql/auth/
modules/<module-name>/data/sql/character/
modules/<module-name>/data/sql/world/
```

The database updater processes module migrations when allowed by
`Database.AutoUpdate.AllowedModules` in `mangosd.conf`.

Module-owned localized strings should use:

```sql
module_string(module, id, content_default)
module_string_locale(module, id, locale, content)
```

Read module strings from C++ with:

```cpp
sObjectMgr.GetModuleString("mod-example", id, localeIndex);
```

## Source References

- `modules/README.md`
- `modules/create_module.sh`
- `modules/templates/basic/src/module.cpp.in`
- `src/game/ScriptObjects.h`
- `src/game/ScriptMgr.h`
- `cmake/ConfigureModules.cmake`
