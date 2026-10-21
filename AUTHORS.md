# AUTHORS

Authorship and copyright for individual files may belong to the people and projects that originally wrote or maintained those files.
This file is meant to make major upstream lineage explicit, not to replace file-level copyright notices or the project license.

## Current Project

- Tortoise WoW contributors

## Upstream Lineage

This codebase contains, derives from, or has historical lineage through work from the following projects and contributor communities:

- **[MaNGOS][1]**
- **[MaNGOS Zero][2]**
- **[vMaNGOS][3]**
- **[Elysium Project][4]**
- **Nostalrius**
- **[AzerothCore][5]**

## AzerothCore Module System

The module system includes files and behavior derived from or modeled after AzerothCore's module system.
Files with direct AzerothCore-derived content include their own file-level attribution and license notices.

Relevant AzerothCore sources:

- `src/cmake/macros/ConfigureModules.cmake`
- `modules/ModulesScriptLoader.h`
- `modules/ModulesLoader.cpp.in.cmake`

## vMaNGOS Spell and Aura Script System

The spell and aura scripting system is an implementation based on vMaNGOS's spell and aura script system.
This includes the `SpellScript` and `AuraScript` API, script lookup through `ScriptMgr`, and spell/aura hook call sites in the core spell runtime.

Files with direct vMaNGOS-derived spell/aura scripting content:

- `src/game/ScriptMgr.h`
- `src/game/ScriptMgr.cpp`
- `src/game/Spells/Spell.h`
- `src/game/Spells/Spell.cpp`
- `src/game/Spells/SpellAuras.h`
- `src/game/Spells/SpellAuras.cpp`
- `src/game/Spells/SpellEffects.cpp`

Additional integration call sites for this system exist in broader runtime files:

- `src/game/UnitAuraProcHandler.cpp`
- `src/game/Objects/Unit.cpp`
- `src/game/Threat/ThreatManager.cpp`
- `src/game/StatSystem.cpp`

## Eluna Lua Engine (optional, `BUILD_ELUNA`)

The optional Eluna integration is built on the port by **Nighthawk42** and **Nightdawg** in
[TuringKi/turtle-wow-custom][6] (commits `aa0b2be`, `e649428`, `bdbe530`): the build plumbing, the
`ElunaScriptBridge.cpp` hook bridge, the core hook sites and the `SpellInfo` compatibility layer in
`src/game/Eluna/CustomMethods.h`. The engine itself is [Eluna][7] (GPL-3.0), pinned as the
`dep/Eluna` submodule (with the core's fixes in `dep/eluna-patches`); Lua is MIT-licensed.

## Notes

Third-party dependencies may carry their own authorship, copyright, and license notices in their respective directories.


[1]: https://github.com/mangos
[2]: https://github.com/mangoszero/server
[3]: https://github.com/vmangos/core
[4]: https://github.com/elysium-project
[5]: https://github.com/azerothcore/azerothcore-wotlk
[6]: https://github.com/TuringKi/turtle-wow-custom
[7]: https://github.com/ElunaLuaEngine/Eluna
