--[[
Smoke script for the fixes made after the review of tortoise-wow#603.

Copy it into Eluna.ScriptPath (default ./lua_scripts) with Eluna.Enabled = 1.
Every Lua state loads it. Each check logs a line starting "ELUNA_603 <item>";
a line with "FAIL" is a failed check. What to do and what to expect, per item:

  1  timers across a teleport
     Log in, then teleport to another map and back a few times, with and
     without an instance in between (.tele, or a dungeon portal), and let an
     instance unload (leave it empty; MapUpdate unload timer). Every map
     change registers a repeating player timer in the new map's state.
     Expect: "1 timer" lines only from the player's current map state (the
     old state's timer stops at the teleport), no crash when the instance
     unloads while the player is elsewhere.

  2  objects of another map
     Two players on different maps. Every map state checks every player in
     the world each 30 s. Expect "2 refused" for players on other maps,
     "2 own" for players on the state's map, never "2 FAIL".

  3  instance data survives a restart
     SMOKE_INSTANCE_MAP must be a dungeon WITHOUT a C++ instance script; the
     default, 34 (The Stockade), has none in src/scripts (389, Ragefire
     Chasm, neither; 36, Deadmines, has one). Enter it,
     wait for "3 saved counter=N", restart mangosd within the instance's
     reset time, enter the same instance again.
     Expect "3 loaded counter=N" (not 0), and "3 saved counter=N+1".
     On a map WITH a C++ instance script the server logs
     "Eluna: map M has a C++ instance script" and no "3" line appears.

  4  reload while instances open
     Type ".reload eluna" repeatedly (or set SMOKE_RELOAD_EVERY_MS) while
     players or bots enter instances. Expect "Eluna: script reload queued"
     then "Eluna: script cache reloaded in N ms", a new "loaded" line per
     state, and no crash. A ThreadSanitizer build with SMOKE_RELOAD_EVERY_MS =
     20000 and players in instances must show no report in Eluna code (before
     dep/eluna-patches/0005: ElunaTemplate<T>::Register / Push, the per-type
     name stored by every state's reload on its own map thread).

  5  spell hit and gameobject spawn reach Lua
     Cast any spell on creature SMOKE_CREATURE_ENTRY (this script gives it Lua
     AI with CREATURE_EVENT_ON_HIT_BY_SPELL). Expect "5 creature hit by spell
     <id>". Let gameobject SMOKE_GO_ENTRY spawn or respawn (a grid loading,
     or loot a herb and wait for its respawn): expect "5 gameobject spawn".
]]

local SMOKE_INSTANCE_MAP = 34       -- The Stockade: no C++ instance script in src/scripts (nor Ragefire Chasm, 389)
local SMOKE_CREATURE_ENTRY = 6      -- Kobold Vermin, Northshire
local SMOKE_GO_ENTRY = 1618         -- Peacebloom
local SMOKE_RELOAD_EVERY_MS = 0     -- > 0: the world state reloads Eluna this often

local PLAYER_EVENT_ON_LOGIN = 3
local PLAYER_EVENT_ON_MAP_CHANGE = 28
local SERVER_EVENT_ON_MAP_UPDATE = 23
local WORLD_EVENT_ON_UPDATE = 13
local INSTANCE_EVENT_ON_INITIALIZE = 1
local INSTANCE_EVENT_ON_LOAD = 2
local INSTANCE_EVENT_ON_PLAYER_ENTER = 4
local CREATURE_EVENT_ON_HIT_BY_SPELL = 14
local GAMEOBJECT_EVENT_ON_SPAWN = 2

local state = "world"
local stateMapId = -1
local ticks = 0

PrintInfo("ELUNA_603 loaded")

local function log(item, text)
    PrintInfo("ELUNA_603 " .. item .. " " .. text .. " state=" .. state)
end

-- 1: a repeating player timer in the state of the map the player is on.
local function startPlayerTimer(player)
    local name = player:GetName()
    player:RegisterEvent(function(eventId, delay, repeats, obj)
        -- obj is the player; the timer runs in the state that registered it.
        -- A timer of the old map running for a player now on another map is
        -- the regression; with the cross-map refusal (item 2) using obj then
        -- raises "another map", so the error itself is reported as FAIL.
        local ok, mapId = pcall(function() return obj:GetMapId() end)
        if not ok then
            log("1", "FAIL timer of map " .. stateMapId .. " ran for a player it cannot use: " .. tostring(mapId))
            return
        end
        if stateMapId ~= -1 and mapId ~= stateMapId then
            log("1", "FAIL timer of map " .. stateMapId .. " ran for a player on map " .. mapId)
            return
        end
        log("1", "timer " .. name .. " on map " .. mapId)
    end, 5000, 0)
    log("1", "registered timer for " .. name)
end

RegisterPlayerEvent(PLAYER_EVENT_ON_LOGIN, function(event, player)
    startPlayerTimer(player)
end)

RegisterPlayerEvent(PLAYER_EVENT_ON_MAP_CHANGE, function(event, player)
    startPlayerTimer(player)
end)

-- 2: in a map state, objects of other maps are refused.
RegisterServerEvent(SERVER_EVENT_ON_MAP_UPDATE, function(event, map, diff)
    ticks = ticks + 1
    if ticks == 1 then
        stateMapId = map:GetMapId()
        state = "map " .. stateMapId .. "/" .. map:GetInstanceId()
        log("2", "first tick")
    end
    if ticks % 300 ~= 0 then
        return
    end
    for _, player in ipairs(GetPlayersInWorld() or {}) do
        local ok, result = pcall(function() return player:GetMapId() end)
        if ok then
            if result == stateMapId then
                log("2", "own player on map " .. result)
            else
                log("2", "FAIL used a player on map " .. tostring(result))
            end
        elseif tostring(result):find("another map", 1, true) then
            log("2", "refused a player of another map")
        else
            log("2", "FAIL unexpected error: " .. tostring(result))
        end
    end
end)

-- 3: instance data is saved with the Lua table and loaded after a restart.
local function bumpAndSave(map, data)
    data.counter = (data.counter or 0) + 1
    map:SaveInstanceData()
    log("3", "saved counter=" .. data.counter)
end

RegisterMapEvent(SMOKE_INSTANCE_MAP, INSTANCE_EVENT_ON_INITIALIZE, function(event, data, map)
    log("3", "initialize (new instance) counter=" .. tostring(data.counter))
end)

RegisterMapEvent(SMOKE_INSTANCE_MAP, INSTANCE_EVENT_ON_LOAD, function(event, data, map)
    log("3", "loaded counter=" .. tostring(data.counter))
end)

RegisterMapEvent(SMOKE_INSTANCE_MAP, INSTANCE_EVENT_ON_PLAYER_ENTER, function(event, data, map, player)
    bumpAndSave(map, data)
end)

-- 4: optional reload loop. WORLD_EVENT_ON_UPDATE only runs in the world state.
local reloadClock = 0
RegisterServerEvent(WORLD_EVENT_ON_UPDATE, function(event, diff)
    if SMOKE_RELOAD_EVERY_MS <= 0 then
        return
    end
    reloadClock = reloadClock + diff
    if reloadClock >= SMOKE_RELOAD_EVERY_MS then
        reloadClock = 0
        log("4", "reload requested")
        RunCommand("reload eluna")
    end
end)

-- 5: spell hit on a creature with Lua AI and gameobject spawn.
RegisterCreatureEvent(SMOKE_CREATURE_ENTRY, CREATURE_EVENT_ON_HIT_BY_SPELL, function(event, creature, caster, spellId)
    log("5", "creature hit by spell " .. tostring(spellId))
end)

RegisterGameObjectEvent(SMOKE_GO_ENTRY, GAMEOBJECT_EVENT_ON_SPAWN, function(event, go)
    log("5", "gameobject spawn " .. go:GetEntry())
end)
