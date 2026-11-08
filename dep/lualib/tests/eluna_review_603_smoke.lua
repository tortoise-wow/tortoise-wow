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

  6  Lua references of objects that are gone
     Log in and stay put for SMOKE_REF_ROUNDS * 15 s (about a minute). The
     map state spawns SMOKE_REF_OBJECTS creatures (SMOKE_REF_CREATURE_ENTRY)
     around the player every 15 s, each with a 10-minute repeating timer, and
     despawns them after 3 s. A timer holds its Lua function in the state's
     registry (luaL_ref). The script counts its own timer functions still in
     the registry (debug.getregistry) at the start of the next round.
     Expect "6 baseline", then per round "6 round R refs freed: 0 of N"
     with N > 0. A round in which no timer could be registered is a FAIL
     (nothing was tested). Before dep/eluna-patches/0006 the count grows by
     N each round: "6 FAIL round R: K timer functions ... still in the
     registry". The "6 stale handle refused" line is information only: in
     this core a userdata is invalid as soon as the call that pushed it
     returns, despawned or not, so it does not test the despawn.

  7  timer registered in GAMEOBJECT_EVENT_ON_SPAWN
     Each item 6 round also summons gameobject SMOKE_GO_ENTRY next to the
     player, and every grid load or respawn of that entry spawns one. Its
     spawn handler registers a 1 s timer on it. Expect "7 registered timer
     in spawn of gameobject" and then "7 timer from spawn fired". Before the
     spawn event was moved into GameObject::AddToWorld it fired before the
     object was in the world: "7 FAIL RegisterEvent in spawn returned nil".

  8  hooks survive a reload (no old reference freed in the new state)
     Every hook function this file registers (Register*Event) is held by
     the state's registry. 5 s after the file is loaded, and every 30 s,
     each state checks that every one of them is still there:
     "8 hooks intact: N of N". Run it together with item 4 (reloads) and
     item 1 (players changing maps). A reference of the closed state
     unreferenced in the new one frees a slot of the new state, often one
     of these hooks: "8 FAIL hooks missing from the registry".
     What it guards: patch 0001 parks a processor that is replaced for the
     same object (it left the state and came back before the next cleanup)
     until that cleanup; patch 0006 has a reload mark the parked processor's
     events ERASE as well, or the cleanup after the reload would unref the
     closed state's reference numbers in the new state. This cannot be
     forced from Lua: the parked processor needs an object of the same guid
     to leave the state and register a timer in it again between two
     cleanups (one update of the state), and no Lua method removes an
     object and brings it back with its guid (players relog or teleport
     over several updates; GameObject:RemoveFromWorld deletes it; creature
     despawn keeps it in the map). So item 8 detects the damage when the
     timing happens, it does not make it happen.

  9  server-wide functions are refused in a map state
     Every map state, on its first update, calls each function the VMaNGOS
     backend now flags world-only (dep/eluna-patches/0009: PerformIngameSpawn,
     SendMail, SaveAllPlayers, StartGameEvent, StopGameEvent, AddVendorItem,
     VendorRemoveItem, VendorRemoveAllItems, AddTaxiPath, Ban, Kick) and the
     methods Player:LogoutPlayer and Player:KickPlayer on a player of its own
     map. Expect one "9 refused=<name>" per function and method,
     "9 world-only functions refused: 11 of 11" and
     "9 world-only methods refused: 2 of 2"; do not call them in the world
     state, where they still work. The arguments are deliberately incomplete:
     before 0009 the call reaches the C++ method instead, which fails on them
     (logged as FAIL, and it spawns, bans or mails nothing; SaveAllPlayers
     does save every player). Before 0009: "9 FAIL PerformIngameSpawn failed
     with an unexpected error: int expected, ...", Player:LogoutPlayer logs
     the player out, and Player:KickPlayer disconnects it.

  10 Aura:GetCasterLevel of a gone caster
     The first player to log in spawns two rabbits (entry 721): one casts
     Mark of the Wild (1126, a 30 min aura) on the other and despawns after
     3 s; 8 s later the target's timer asks for the aura and its caster
     level. Expect "10 GetCasterLevel of the gone caster is nil". Before
     dep/eluna-patches/0010 that read the null guid lookup of the caster and
     crashed the map's update (SIGSEGV); a successful read logs FAIL.
]]

local SMOKE_INSTANCE_MAP = 34       -- The Stockade: no C++ instance script in src/scripts (nor Ragefire Chasm, 389)
local SMOKE_CREATURE_ENTRY = 6      -- Kobold Vermin, Northshire
local SMOKE_GO_ENTRY = 1618         -- Peacebloom
local SMOKE_RELOAD_EVERY_MS = 0     -- > 0: the world state reloads Eluna this often
local SMOKE_REF_CREATURE_ENTRY = 721 -- Rabbit: item 6 spawns these and lets them despawn
local SMOKE_REF_OBJECTS = 25        -- creatures per item 6 round
local SMOKE_REF_ROUNDS = 4          -- item 6 rounds, 15 s apart

local PLAYER_EVENT_ON_LOGIN = 3
local PLAYER_EVENT_ON_MAP_CHANGE = 28
local SERVER_EVENT_ON_MAP_UPDATE = 23
local WORLD_EVENT_ON_UPDATE = 13
local INSTANCE_EVENT_ON_INITIALIZE = 1
local INSTANCE_EVENT_ON_LOAD = 2
local INSTANCE_EVENT_ON_PLAYER_ENTER = 4
local CREATURE_EVENT_ON_HIT_BY_SPELL = 14
local GAMEOBJECT_EVENT_ON_SPAWN = 2
local TEMPSUMMON_TIMED_DESPAWN = 3

-- 8: every hook function registered through these globals is counted; a
-- weak-keyed set, so it never keeps a function alive itself.
local hookFns = setmetatable({}, { __mode = "k" })
local hookCount = 0
for _, name in ipairs({ "RegisterPlayerEvent", "RegisterServerEvent", "RegisterMapEvent",
        "RegisterCreatureEvent", "RegisterGameObjectEvent" }) do
    local register = _G[name]
    _G[name] = function(...)
        local args = { ... }
        local result = register(...)
        for i = 1, select("#", ...) do
            if type(args[i]) == "function" then
                hookFns[args[i]] = true
                hookCount = hookCount + 1
            end
        end
        return result
    end
end

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

-- 6: timers of objects that are gone free their Lua references. The timer
-- functions are kept as weak keys, so this table never keeps one alive; one
-- found in the registry is held by the registry alone.
local refTimers = setmetatable({}, { __mode = "k" })
local refLoopStarted = false

local function countRefTimers()
    local held, functions = 0, 0
    for _, value in pairs(debug.getregistry()) do
        if type(value) == "function" then
            functions = functions + 1
            if refTimers[value] then
                held = held + 1
            end
        end
    end
    return held, functions
end

local function startRefLoop(player)
    if refLoopStarted then
        return
    end
    refLoopStarted = true
    local round = 0
    local spawned = 0
    local roundSpawned = 0
    local staleHandle = nil
    player:RegisterEvent(function(eventId, delay, repeats, obj)
        local held, functions = countRefTimers()
        if round == 0 then
            log("6", "baseline: " .. held .. " item 6 timer functions, " .. functions .. " functions in the registry")
        elseif roundSpawned == 0 then
            log("6", "FAIL round " .. round .. ": no creature timer could be registered, nothing was tested")
        elseif held == 0 then
            log("6", "round " .. round .. " refs freed: 0 of " .. spawned .. " (" .. functions .. " functions in the registry)")
        else
            log("6", "FAIL round " .. round .. ": " .. held .. " timer functions of " .. spawned ..
                " despawned creatures still in the registry (" .. functions .. " functions in the registry)")
        end
        if staleHandle then
            local ok, err = pcall(function() return staleHandle:GetEntry() end)
            if ok then
                log("6", "FAIL a creature kept from round " .. round .. " is still usable")
            else
                log("6", "stale handle refused (info only): " .. tostring(err))
            end
            staleHandle = nil
        end
        if round >= SMOKE_REF_ROUNDS then
            return
        end
        round = round + 1
        roundSpawned = 0
        local x, y, z, o = obj:GetLocation()
        for i = 1, SMOKE_REF_OBJECTS do
            local angle = i * 2 * math.pi / SMOKE_REF_OBJECTS
            local creature = obj:SpawnCreature(SMOKE_REF_CREATURE_ENTRY, x + 6 * math.cos(angle), y + 6 * math.sin(angle), z, o,
                TEMPSUMMON_TIMED_DESPAWN, 3000)
            if creature then
                -- An upvalue per timer: Lua 5.2 may reuse one closure for
                -- a function without upvalues.
                local tag = round * 1000 + i
                local timer = function() return tag end
                if creature:RegisterEvent(timer, 600000, 0) then
                    refTimers[timer] = true
                    spawned = spawned + 1
                    roundSpawned = roundSpawned + 1
                end
                staleHandle = creature
            end
        end
        -- 7: a summoned gameobject goes through Create and Map::Add.
        obj:SummonGameObject(SMOKE_GO_ENTRY, x + 3, y, z, o, 10)
        log("6", "round " .. round .. " spawned " .. SMOKE_REF_OBJECTS .. " creatures, " .. spawned .. " timers so far")
    end, 15000, SMOKE_REF_ROUNDS + 1)
end

RegisterPlayerEvent(PLAYER_EVENT_ON_LOGIN, function(event, player)
    startRefLoop(player)
end)

-- 7: a timer registered in the spawn event runs.
RegisterGameObjectEvent(SMOKE_GO_ENTRY, GAMEOBJECT_EVENT_ON_SPAWN, function(event, go)
    local name = go:GetEntry() .. "/" .. go:GetGUIDLow()
    local eventId = go:RegisterEvent(function(id, delay, repeats, obj)
        log("7", "timer from spawn fired on gameobject " .. name)
    end, 1000, 1)
    if eventId then
        log("7", "registered timer in spawn of gameobject " .. name)
    else
        log("7", "FAIL RegisterEvent in spawn returned nil for gameobject " .. name .. " (not in the world yet)")
    end
end)

-- 8: every hook function of this file is still in the registry.
local function checkHooks()
    local present = 0
    for _, value in pairs(debug.getregistry()) do
        if type(value) == "function" and hookFns[value] then
            present = present + 1
        end
    end
    if present == hookCount then
        log("8", "hooks intact: " .. present .. " of " .. hookCount)
    else
        log("8", "FAIL hooks missing from the registry: " .. present .. " of " .. hookCount)
    end
end

CreateLuaEvent(function()
    checkHooks()
    CreateLuaEvent(checkHooks, 30000, 0)
end, 5000, 1)

-- 9: server-wide functions are refused in a map state.
-- dep/eluna-patches/0009 flags them METHOD_REG_WORLD: the state registers
-- Eluna's MethodWrongState closure instead of the method, and a call raises
-- "attempt to call method '...' that does not exist for state: N". They run in
-- the world state only (server events, commands). The arguments here are
-- deliberately incomplete: before 0009 they reach the C++ method and fail on
-- the arguments (logged as FAIL, not "refused"), so the check does not spawn,
-- ban or mail anything while it reports the missing flag.
local WorldOnlyFunctions = {
    "PerformIngameSpawn", "SendMail", "SaveAllPlayers", "StartGameEvent", "StopGameEvent",
    "AddVendorItem", "VendorRemoveItem", "VendorRemoveAllItems", "AddTaxiPath", "Ban", "Kick",
}

local function refusedWorldOnly(name, ok, err)
    if ok then
        log("9", "FAIL " .. name .. " ran in a map state (not world-only)")
        return false
    end
    if tostring(err):find("does not exist for state", 1, true) then
        log("9", "refused=" .. name)
        return true
    end
    log("9", "FAIL " .. name .. " failed with an unexpected error: " .. tostring(err))
    return false
end

local function refusedWorldOnlyMethod(name, fn)
    local ok, err = pcall(fn)
    -- 0009 flags the entries world-only (not unsafe); the unsafe refusal is
    -- accepted too, in case an entry is ever flagged that way.
    if not ok and tostring(err):find("flagged as unsafe", 1, true) then
        log("9", "refused=" .. name .. " (unsafe method)")
        return true
    end
    return refusedWorldOnly(name, ok, err)
end

local function checkWorldOnly(player)
    local refused = 0
    for _, name in ipairs(WorldOnlyFunctions) do
        local fn = _G[name]
        if type(fn) ~= "function" then
            log("9", "FAIL " .. name .. " is not a global function")
        elseif refusedWorldOnly(name, pcall(fn)) then
            refused = refused + 1
        end
    end
    log("9", "world-only functions refused: " .. refused .. " of " .. #WorldOnlyFunctions)

    -- Player:LogoutPlayer and Player:KickPlayer are methods: there has to be
    -- a player of this map state's map for them (one on another map is
    -- refused by item 2's check).
    if not player then
        log("9", "no player on this map; the LogoutPlayer and KickPlayer checks did not run")
        return
    end
    local refusedMethods = 0
    if refusedWorldOnlyMethod("LogoutPlayer", function() player:LogoutPlayer(false) end) then
        refusedMethods = refusedMethods + 1
    end
    if refusedWorldOnlyMethod("KickPlayer", function() player:KickPlayer() end) then
        refusedMethods = refusedMethods + 1
    end
    log("9", "world-only methods refused: " .. refusedMethods .. " of 2")
end

local function firstPlayerOnThisMap()
    for _, player in ipairs(GetPlayersInWorld() or {}) do
        local ok, mapId = pcall(function() return player:GetMapId() end)
        if ok and mapId == stateMapId then
            return player
        end
    end
    return nil
end

RegisterServerEvent(SERVER_EVENT_ON_MAP_UPDATE, function(event, map, diff)
    if ticks ~= 1 then
        return
    end
    checkWorldOnly(firstPlayerOnThisMap())
end)

-- 10: Aura:GetCasterLevel of an aura whose caster is gone returns nil.
-- A rabbit casts Mark of the Wild (a 30 min aura) on another rabbit and
-- despawns after 3 s; 8 s later the target's own timer asks for the aura and
-- its caster level. Before dep/eluna-patches/0010 that dereferences the null
-- GuidList lookup of the caster (SIGSEGV in the map's thread). The timer is
-- registered on the target and uses the object it is passed: a userdata kept
-- in an upvalue across a timer is invalid in this core.
local CASTER_TEST_SPELL = 1126           -- Mark of the Wild, Rank 1
local CASTER_TEST_ENTRY = 721            -- Rabbit
local CASTER_TEST_GONE_AFTER_MS = 8000   -- the caster is unsummoned at 3 s
local casterTestStarted = false

local function startCasterTest(player)
    if casterTestStarted then
        return
    end
    casterTestStarted = true

    local x, y, z, o = player:GetLocation()
    local target = player:SpawnCreature(CASTER_TEST_ENTRY, x + 4, y, z, o, TEMPSUMMON_TIMED_DESPAWN, 120000)
    local caster = player:SpawnCreature(CASTER_TEST_ENTRY, x + 8, y, z, o, TEMPSUMMON_TIMED_DESPAWN, 3000)
    if not target or not caster then
        log("10", "FAIL could not spawn the test creatures")
        return
    end

    if not target:AddAura(CASTER_TEST_SPELL, caster) then
        log("10", "FAIL AddAura returned nil")
        return
    end

    local names = target:GetGUIDLow() .. "/" .. caster:GetGUIDLow()
    local eventId = target:RegisterEvent(function(id, delay, repeats, obj)
        if not obj:HasAura(CASTER_TEST_SPELL) then
            log("10", "FAIL the aura left with its caster (" .. names .. ")")
            return
        end
        local aura = obj:GetAura(CASTER_TEST_SPELL)
        if not aura then
            log("10", "FAIL GetAura returned nil for an aura HasAura saw (" .. names .. ")")
            return
        end
        local level = aura:GetCasterLevel()
        if level == nil then
            log("10", "GetCasterLevel of the gone caster is nil (" .. names .. ")")
        else
            log("10", "FAIL GetCasterLevel returned " .. tostring(level) .. " for a caster that despawned (" .. names .. ")")
        end
    end, CASTER_TEST_GONE_AFTER_MS, 1)
    if not eventId then
        log("10", "FAIL RegisterEvent on the aura's target returned nil")
        return
    end

    log("10", "aura " .. CASTER_TEST_SPELL .. " on " .. names .. " cast by a creature that despawns in 3 s")
end

RegisterPlayerEvent(PLAYER_EVENT_ON_LOGIN, function(event, player)
    startCasterTest(player)
end)
