--[[
Smoke script for Eluna patch 0012 (VMaNGOS TrinityCore parity).

Copy it into Eluna.ScriptPath (default ./lua_scripts) with Eluna.Enabled = 1.
Every Lua state loads it. Each check logs a line starting "ELUNA_0012 <group>";
a line with "FAIL" is a failed check. Groups, what to do and what to expect:

  state    On load every state logs GetStateMapId(), GetStateInstanceId() and
           pcall(GetStateMap). The world state: mapId -1, GetStateMap refused.
           A map state: its map id, GetStateMap's map has the same id. On
           MAP_EVENT_ON_UPDATE every 30 s a map state compares #GetPlayersOnMap()
           with map:GetPlayerCount() ("state players ok"); the world state
           checks that GetPlayersOnMap is refused. (GetStateMapId and
           GetStateInstanceId come with patch 0011: without it the load line says so.)

  spell    Cast SMOKE_SPELL (default 133, Fireball rank 1) at a creature. Expect,
           in this order: check_cast, before_cast, cast, after_cast, then
           before_hit miss=0, effect_hit_target, hit, after_hit, with the
           target's guid as hitUnit, and "spell order ok". ".elunasmoke blockcast"
           (world state) makes the next check_cast return SPELL_FAILED_NOT_READY:
           expect the client error and no cast line, then it unblocks itself.
           RegisterSpellEvent(x, 2, f) logs "spell unsupported refused". It also
           logs "hit once per target ok" (event 22 fired exactly once). Cast
           SMOKE_AURA_SPELL (1126, an aura-only spell that took none of the old
           heal/damage/proc fire sites) on yourself: "aura-only hit once ok".

  threat   ".elunasmoke threat" on a selected creature that is in combat with
           you: AddThreat(player, 1000), GetThreat >= 1000, ResetAllThreat -> 0
           with the player still in GetAITargets, FixateTarget(you) true and the
           creature stays on you for 10 s even if another unit gets threat,
           ClearFixate, ClearThreat(player) -> GetThreat 0,
           FixateTarget(a unit not on the list) false.

  spellget Logged in the cast hook for SMOKE_SPELL: IsTriggered, IsPositive,
           GetState, IsInterruptable, IsChannelActive, CalculateDamage(0),
           GetOriginalCaster (for Fireball: false, false, a state 1..5 (only
           logged), true, false, > 0, the caster). Cast Arcane Missiles (5143):
           IsChannelActive is true in its hooks.

  data     On each login / map change player:Data() counts "n". After teleports
           the value keeps counting, after ".reload eluna" it survives. A handle
           kept in a global by the old map state shows the old value after the
           player left it (logs "data detached ok"), the new state's value
           differs. map:Data() has a counter per map; pcall(Data) of another
           map's object is refused. d:Set("me", d) then d:Get("me"):Get("me") is
           nil (a copy, no cycle) and "data copy ok".

  cmd      ".paritysmoke" from a GM in game and from the console: IsConsole
           false / true, GetPlayer player / nil, GetSecurity, GetSelectedUnit
           name / nil, PSendSysMessage("x=%d", 5) reaches the caller. A SEC_PLAYER
           account (PlayerCommands on) is told "no access". A map state never sees
           the event. Before patch 0012 the handler argument is nil ("cmd FAIL
           no handler").
]]

local SMOKE_SPELL = 133 -- Fireball rank 1
local SMOKE_AURA_SPELL = 1126 -- Mark of the Wild rank 1 (aura only; cast it on a friend or yourself)

local PLAYER_EVENT_ON_LOGIN = 3
local PLAYER_EVENT_ON_MAP_CHANGE = 28
local PLAYER_EVENT_ON_COMMAND = 42
local SERVER_EVENT_ON_MAP_UPDATE = 23
local SPELL_FAILED_NOT_READY = 60 -- SpellDefines.h, SpellCastResult

local state = "world"
local function log(group, text)
    PrintInfo("ELUNA_0012 " .. group .. " [" .. state .. "] " .. text)
end

-- state --------------------------------------------------------------------
local mapId = GetStateMapId and GetStateMapId() or "n/a (0011)"
local instanceId = GetStateInstanceId and GetStateInstanceId() or "n/a (0011)"
if type(mapId) == "number" and mapId >= 0 then
    state = "map " .. mapId .. "/" .. tostring(instanceId)
end
local okMap, stateMap = pcall(GetStateMap)
if type(mapId) == "number" and mapId == -1 then
    log("state", "world: mapId=-1 GetStateMap " .. (okMap and "FAIL not refused" or "refused ok"))
else
    log("state", "mapId=" .. tostring(mapId) .. " instance=" .. tostring(instanceId) ..
        " GetStateMap=" .. (okMap and tostring(stateMap and stateMap:GetMapId()) or "refused"))
end

local lastCheck = 0
RegisterServerEvent(SERVER_EVENT_ON_MAP_UPDATE, function(event, map, diff)
    lastCheck = lastCheck + diff
    if lastCheck < 30000 then return end
    lastCheck = 0
    local players = GetPlayersOnMap()
    if #players == map:GetPlayerCount() then
        log("state", "players ok: " .. #players)
    else
        log("state", "FAIL GetPlayersOnMap=" .. #players .. " GetPlayerCount=" .. map:GetPlayerCount())
    end
    local d = map:Data()
    d:Set("ticks", (d:Get("ticks") or 0) + 1)
    log("data", "map counter ticks=" .. d:Get("ticks"))
end)
if type(mapId) == "number" and mapId == -1 then
    CreateLuaEvent(function()
        log("state", "world: GetPlayersOnMap " .. (pcall(GetPlayersOnMap) and "FAIL not refused" or "refused ok"))
    end, 30000, 1)
end

-- spell --------------------------------------------------------------------
local order = {}
local hitCount = 0
local function step(name, spell, ...)
    order[#order + 1] = name
    log("spell", name .. " " .. table.concat({ ... }, " "))
end

if type(mapId) == "number" and mapId >= 0 then
    RegisterSpellEvent(SMOKE_SPELL, 10, function(event, spell)
        step("check_cast", spell)
        -- armed by ".elunasmoke blockcast" through the data of the map (the command runs in the world state)
        local d = GetStateMap():Data()
        if d:Get("blockcast") then
            d:Set("blockcast", nil)
            return SPELL_FAILED_NOT_READY
        end
    end)
    RegisterSpellEvent(SMOKE_SPELL, 11, function(event, spell) order = {}; step("before_cast", spell) end)
    RegisterSpellEvent(SMOKE_SPELL, 1, function(event, spell, skipCheck)
        step("cast", spell, "skipCheck=" .. tostring(skipCheck))
        local caster = spell:GetOriginalCaster()
        log("spellget", "IsTriggered=" .. tostring(spell:IsTriggered()) ..
            " IsPositive=" .. tostring(spell:IsPositive()) ..
            " GetState=" .. tostring(spell:GetState()) ..
            " IsInterruptable=" .. tostring(spell:IsInterruptable()) ..
            " IsChannelActive=" .. tostring(spell:IsChannelActive()) ..
            " CalculateDamage(0)=" .. tostring(spell:CalculateDamage(0)) ..
            " originalCaster=" .. (caster and caster:GetName() or "nil"))
    end)
    RegisterSpellEvent(SMOKE_SPELL, 12, function(event, spell) step("after_cast", spell) end)
    RegisterSpellEvent(SMOKE_SPELL, 20, function(event, spell, miss, hitUnit)
        step("before_hit", spell, "miss=" .. tostring(miss), "hit=" .. (hitUnit and hitUnit:GetName() or "nil"))
    end)
    RegisterSpellEvent(SMOKE_SPELL, 21, function(event, spell, effIndex, mode, hitUnit)
        step("effect_hit_target", spell, "eff=" .. tostring(effIndex), "mode=" .. tostring(mode), "hit=" .. (hitUnit and hitUnit:GetName() or "nil"))
        return false
    end)
    RegisterSpellEvent(SMOKE_SPELL, 22, function(event, spell, hitUnit)
        hitCount = hitCount + 1
        step("hit", spell, "hit=" .. (hitUnit and hitUnit:GetName() or "nil"))
    end)
    RegisterSpellEvent(SMOKE_SPELL, 23, function(event, spell, hitUnit)
        step("after_hit", spell, "hit=" .. (hitUnit and hitUnit:GetName() or "nil"))
        local want = { "before_cast", "cast", "after_cast", "before_hit", "effect_hit_target", "hit", "after_hit" }
        -- check_cast comes before before_cast and is dropped from the list by before_cast resetting it
        local same = #order == #want
        for i = 1, #want do same = same and order[i] == want[i] end
        log("spell", same and "order ok" or ("FAIL order: " .. table.concat(order, ",")))
        log("spell", hitCount == 1 and "hit once per target ok" or ("FAIL hit fired " .. hitCount .. " times"))
        hitCount = 0
    end)
    -- an aura-only spell (no damage, no heal, no proc: none of the old fire sites) still fires ON_HIT once
    local auraHits = 0
    RegisterSpellEvent(SMOKE_AURA_SPELL, 22, function(event, spell, hitUnit)
        auraHits = auraHits + 1
    end)
    RegisterSpellEvent(SMOKE_AURA_SPELL, 23, function(event, spell, hitUnit)
        log("spell", auraHits == 1 and "aura-only hit once ok" or ("FAIL aura-only hit fired " .. auraHits .. " times"))
        auraHits = 0
    end)
    local okReg, err = pcall(RegisterSpellEvent, SMOKE_SPELL, 2, function() end)
    log("spell", okReg and "FAIL event 2 accepted" or ("unsupported refused: " .. tostring(err)))
end

-- data ---------------------------------------------------------------------
local keptHandle
local function bump(event, player)
    local d = player:Data()
    d:Set("n", (d:Get("n") or 0) + 1)
    log("data", player:GetName() .. " n=" .. d:Get("n"))
    if keptHandle and keptHandle.name == player:GetName() then
        -- a handle kept from an earlier visit of this state: detached once the player left
        log("data", "kept handle n=" .. tostring(keptHandle.d:Get("n")) .. " now n=" .. d:Get("n") ..
            (keptHandle.d:Get("n") ~= d:Get("n") and " (detached ok)" or " FAIL (shared)"))
    end
    keptHandle = { name = player:GetName(), d = d } -- deliberately kept: this is what the rule forbids
    -- value copy: no cycle
    d:Set("me", d)
    local inner = d:Get("me")
    log("data", (inner and inner:Get("me") == nil) and "copy ok" or "FAIL cycle")
    d:Set("me", nil)
end
RegisterPlayerEvent(PLAYER_EVENT_ON_LOGIN, bump)
RegisterPlayerEvent(PLAYER_EVENT_ON_MAP_CHANGE, bump)

-- commands (world state) ---------------------------------------------------
if type(mapId) == "number" and mapId == -1 then
    RegisterPlayerEvent(PLAYER_EVENT_ON_COMMAND, function(event, player, command, handler)
        if command == "elunasmoke blockcast" then
            if not player then return false end
            player:GetMap():Data():Set("blockcast", true)
            log("spell", "blockcast armed on map " .. player:GetMapId())
            return false
        elseif command == "elunasmoke threat" then
            if not handler then log("cmd", "FAIL no handler") return false end
            local creature = handler:GetSelectedCreature()
            if not creature or not player then handler:SendSysMessage("select a creature in combat with you") return false end
            creature:AddThreat(player, 1000)
            log("threat", "GetThreat=" .. creature:GetThreat(player))
            creature:ResetAllThreat()
            log("threat", "after ResetAllThreat GetThreat=" .. creature:GetThreat(player) .. " targets=" .. #creature:GetAITargets())
            log("threat", "FixateTarget(player)=" .. tostring(creature:FixateTarget(player)))
            creature:ClearFixate()
            creature:ClearThreat(player)
            log("threat", "after ClearThreat GetThreat=" .. creature:GetThreat(player))
            log("threat", "FixateTarget(not on list)=" .. tostring(creature:FixateTarget(creature)))
            return false
        elseif command == "paritysmoke" then
            if not handler then log("cmd", "FAIL no handler") return end
            if not handler:IsAvailable(2) then
                handler:SendSysMessage("ELUNA_0012 no access")
                return false
            end
            local unit = handler:GetSelectedUnit()
            handler:PSendSysMessage("ELUNA_0012 x=%d console=%s security=%d player=%s unit=%s", 5,
                tostring(handler:IsConsole()), handler:GetSecurity(),
                handler:GetPlayer() and handler:GetPlayer():GetName() or "nil",
                unit and unit:GetName() or "nil")
            log("cmd", "console=" .. tostring(handler:IsConsole()) .. " security=" .. handler:GetSecurity())
            return false
        end
    end)
end
