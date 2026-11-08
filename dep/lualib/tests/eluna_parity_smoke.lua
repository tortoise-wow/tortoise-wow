--[[
Smoke script for Eluna patch 0012 (VMaNGOS TrinityCore parity).

Copy it into Eluna.ScriptPath (default ./lua_scripts) with Eluna.Enabled = 1.
Every Lua state loads it. Each check logs a line starting "ELUNA_0012 <group>";
a line with "FAIL" is a failed check. Groups, what to do and what to expect:

  state    On load every state logs GetStateMapId(), GetStateInstanceId() and
           pcall(GetStateMap). The world state: mapId -1, GetStateMap refused.
           A map state: its map id, GetStateMap's map has the same id. On
           MAP_EVENT_ON_UPDATE every 30 s a map state compares the players of
           GetPlayersOnMap() that are not in GM mode with map:GetPlayerCount()
           (which skips GMs; GetPlayersOnMap lists them, as TrinityCore does) ("state players ok"); the world state
           checks that GetPlayersOnMap is refused. (GetStateMapId and
           GetStateInstanceId come with patch 0011: without it the load line says so.)

  spell    Cast SMOKE_SPELL (default 133, Fireball rank 1) at a creature. Expect,
           in this order: check_cast, before_cast, cast, effect_hit_target
           (effect 0, handled at launch), after_cast, then before_hit miss=0,
           effect_hit_target (effect 1), hit, after_hit, with the
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
           creature stays on you for 10 s even if another unit gets threat
           (the script waits 10 s, logs "victim is the player=true", then does
           ClearFixate), ClearThreat(player) -> GetThreat 0,
           FixateTarget(a unit not on the list) false.

  inflight (0008) ".elunasmoke inflight" spawns a temporary Kobold Vermin 28 yd
           in front of you and arms the check; cast SMOKE_SPELL (Fireball) at it.
           At launch it is unsummoned (deleted) while the projectile flies;
           300 ms later the caster's current generic spell (the Fireball, state
           5 = delayed) is asked for its target: "inflight after the delete:
           state=5 GetTarget=nil (nil ok)". Before 0008: the freed creature.
  trade    (0008) An enchant (SMOKE_TRADE_ENCHANT_SPELL, 7418) cast on the
           partner's item in the "will not be traded" slot of an open trade:
           "trade enchant check_cast (slot) / before_cast (slot) / cast (guid)
           / after_cast (guid): GetTarget=item E guid G (item ok)". Needs a real
           client: turtle-agent casts only at a unit or itself.

  spellget Logged in the cast hook for SMOKE_SPELL: IsTriggered, IsPositive,
           GetState, IsInterruptable, IsChannelActive, CalculateDamage(0),
           GetOriginalCaster (for Fireball: false, false, a state 1..5 (only
           logged), true, false, > 0, the caster). Cast Arcane Missiles (5143):
           IsChannelActive is true in its hooks.

  data     On each login / map change player:Data() counts "n". After teleports
           the value keeps counting, after ".reload eluna" it survives. A handle
           kept by the old map state shows the value it had when the player
           left that state ("kept handle ... (detached ok)" on the player's next
           visit of the state: go to another map and come back, without
           ".reload eluna" in between, which drops the kept handles). Login and
           its map change arrive in one state: "kept handle same visit" there. map:Data() has a counter per map. d:Set("me", d) then d:Get("me"):Get("me") is
           nil (a copy, no cycle) and "data copy ok". A plain table that holds itself
           (t.self = t, or a.b = b with b.a = a) given to Set is refused with an
           error ("selfref refused ok"); a 65-level nesting is refused and a
           64-level one stored ("depth limit ok"); a function inside a table is
           refused ("unsupported refused ok"). None of them crashes the server.

  cmd      ".paritysmoke" from a GM in game and from the console: IsConsole
           false / true, GetPlayer player / nil, GetSecurity, GetSelectedUnit
           name / nil, PSendSysMessage("x=%d", 5) reaches the caller. A SEC_PLAYER
           account (PlayerCommands on) is told "no access". A map state never sees
           the event. Before patch 0012 the handler argument is nil ("cmd FAIL
           no handler").
]]

local SMOKE_SPELL = 133 -- Fireball rank 1
local SMOKE_AURA_SPELL = 1126 -- Mark of the Wild rank 1 (aura only; cast it on a friend or yourself)
local SMOKE_INFLIGHT_ENTRY = 6 -- Kobold Vermin: the temporary target of ".elunasmoke inflight"
local SMOKE_TRADE_ENCHANT_SPELL = 7418 -- Enchant Bracer - Minor Health (on the not-traded slot of a trade)

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
    -- GetPlayersOnMap lists every player on the map, GMs included (TrinityCore); Map:GetPlayerCount
    -- is GetPlayersCountExceptGMs, so compare the players that are not in GM mode.
    local players = GetPlayersOnMap()
    local notGM = 0
    for _, p in ipairs(players) do
        if not p:IsGM() then notGM = notGM + 1 end
    end
    if notGM == map:GetPlayerCount() and #GetPlayersOnMap(2, true) == #players - notGM then
        log("state", "players ok: " .. #players .. " (" .. notGM .. " not in GM mode)")
    else
        log("state", "FAIL GetPlayersOnMap=" .. #players .. " not-GM=" .. notGM .. " GetPlayerCount=" .. map:GetPlayerCount())
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
        -- The core's real order for Fireball (a delayed projectile): effect 0 is handled at launch,
        -- so its effect_hit_target comes before after_cast; before_hit, effect 1, hit, after_hit
        -- follow when the projectile lands.
        local want = { "before_cast", "cast", "effect_hit_target", "after_cast",
                       "before_hit", "effect_hit_target", "hit", "after_hit" }
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

    -- 0008, a projectile whose target is deleted in flight: ".elunasmoke inflight" (world state) spawned a
    -- temporary target and armed this through the map's data. At launch (after_cast) the target is
    -- unsummoned (deleted at the end of this map update) while the Fireball is SPELL_STATE_DELAYED in the
    -- caster's current generic spell; 300 ms later the caster's own timer asks that spell for its target.
    -- Before 0008 GetTarget returned the cached pointer of the deleted creature.
    RegisterSpellEvent(SMOKE_SPELL, 12, function(event, spell)
        local d = GetStateMap():Data()
        local armed = d:Get("inflight")
        if not armed then return end
        local target = spell:GetTarget()
        if not target or target:GetGUIDLow() ~= armed then
            log("spellget", "inflight: not the armed target (" .. tostring(target and target:GetGUIDLow()) .. " ~= " .. armed .. "), cast again at it")
            return
        end
        d:Set("inflight", nil)
        local caster = spell:GetCaster()
        log("spellget", "inflight launch: GetTarget=" .. target:GetName() .. " " .. armed .. " state=" .. tostring(spell:GetState()) .. ", unsummoning it")
        target:DespawnOrUnsummon(0)
        caster:RegisterEvent(function(id, delay, repeats, obj)
            local s = obj:GetCurrentSpell(1)   -- CURRENT_GENERIC_SPELL: the delayed projectile stays there until it lands
            if not s or s:GetEntry() ~= SMOKE_SPELL then
                log("spellget", "inflight: inconclusive, the Fireball already landed (cast from farther away)")
                return
            end
            local t = s:GetTarget()
            log("spellget", "inflight after the delete: state=" .. tostring(s:GetState()) .. " GetTarget=" ..
                tostring(t and t:GetGUIDLow()) .. ((s:GetState() == 5 and t == nil) and " (nil ok)" or " FAIL (expected nil in state 5)"))
        end, 300, 1)
    end)

    -- 0008, the trade-slot enchant: an enchant cast on the item in the other player's "will not be traded"
    -- trade slot targets TARGET_FLAG_TRADE_ITEM, by slot number before Spell::cast and by the item's guid
    -- after updateTradeSlotItem. GetTarget must return that item in both: check_cast / before_cast (slot)
    -- and cast / after_cast (guid). Needs a real client (turtle-agent sends no trade-item cast target).
    local function tradeTarget(name)
        return function(event, spell)
            local t = spell:GetTarget()
            local isItem = t ~= nil and t:GetTypeId() == 1   -- TYPEID_ITEM
            log("spellget", "trade enchant " .. name .. ": GetTarget=" .. (isItem and ("item " .. t:GetEntry() .. " guid " .. t:GetGUIDLow()) or tostring(t)) ..
                (isItem and " (item ok)" or " FAIL (expected the trade item)"))
        end
    end
    RegisterSpellEvent(SMOKE_TRADE_ENCHANT_SPELL, 10, tradeTarget("check_cast (slot)"))
    RegisterSpellEvent(SMOKE_TRADE_ENCHANT_SPELL, 11, tradeTarget("before_cast (slot)"))
    RegisterSpellEvent(SMOKE_TRADE_ENCHANT_SPELL, 1, tradeTarget("cast (guid)"))
    RegisterSpellEvent(SMOKE_TRADE_ENCHANT_SPELL, 12, tradeTarget("after_cast (guid)"))
end

-- data ---------------------------------------------------------------------
-- One kept handle per player and state. A handle taken in this state is checked on the player's next
-- bump here. Login and the map change of the same login both arrive in one state with no map left in
-- between: then the handle is still the live data ("same visit", shared on purpose). The detach is
-- only checked when the player left this state and came back: then n moved on by 2 or more (a bump in
-- each other state), and the kept handle must still show the n it had when the player left.
local keptHandles = {}
local function chain(n)
    local t = { leaf = true }
    for i = 2, n do t = { c = t } end
    return t
end
local function bump(event, player)
    local d = player:Data()
    d:Set("n", (d:Get("n") or 0) + 1)
    local n = d:Get("n")
    log("data", player:GetName() .. " n=" .. n)
    local kept = keptHandles[player:GetGUIDLow()]
    if kept then
        local keptNow = kept.d:Get("n")
        if n == kept.n + 1 then
            log("data", "kept handle same visit n=" .. tostring(keptNow) .. " now n=" .. n .. " (shared, no map left)")
        else
            log("data", "kept handle n=" .. tostring(keptNow) .. " now n=" .. n ..
                (keptNow == kept.n and " (detached ok)" or (" FAIL (shared: the kept handle should still say " .. kept.n .. ")")))
        end
    end
    keptHandles[player:GetGUIDLow()] = { n = n, d = d } -- deliberately kept: this is what the rule forbids
    -- value copy: no cycle
    d:Set("me", d)
    local inner = d:Get("me")
    log("data", (inner and inner:Get("me") == nil) and "copy ok" or "FAIL cycle")
    d:Set("me", nil)
    -- a table that holds itself is refused with a Lua error, not stored and not a crash (twice, and through
    -- another table); nesting of 65 levels is refused, 64 is stored
    local selfRef = {}
    selfRef.self = selfRef
    local a, b = {}, {}
    a.b = b
    b.a = a
    local okSet, errSet = pcall(d.Set, d, "selfref", selfRef)
    local okSet2 = pcall(d.Set, d, "selfref", selfRef)
    local okAB, errAB = pcall(d.Set, d, "ab", a)
    log("data", (not okSet and not okSet2 and not okAB and tostring(errSet):find("contains itself", 1, true)
            and tostring(errAB):find("contains itself", 1, true) and d:Get("selfref") == nil and d:Get("ab") == nil)
        and "selfref refused ok" or ("FAIL selfref ok=" .. tostring(okSet) .. " err=" .. tostring(errSet) ..
            " ab ok=" .. tostring(okAB) .. " err=" .. tostring(errAB)))
    local okDeep, errDeep = pcall(d.Set, d, "deep", chain(65))
    local okEdge = pcall(d.Set, d, "edge", chain(64))
    log("data", (not okDeep and tostring(errDeep):find("deeper than 64", 1, true) and d:Get("deep") == nil and okEdge)
        and "depth limit ok (65 refused, 64 stored)"
        or ("FAIL depth 65 ok=" .. tostring(okDeep) .. " err=" .. tostring(errDeep) .. " 64 ok=" .. tostring(okEdge)))
    d:Set("edge", nil)
    local okFn, errFn = pcall(d.Set, d, "fn", { inner = { function() end } })
    log("data", (not okFn and tostring(errFn):find("unsupported type", 1, true) and d:Get("fn") == nil)
        and "unsupported refused ok" or ("FAIL unsupported ok=" .. tostring(okFn) .. " err=" .. tostring(errFn)))
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
        elseif command == "elunasmoke inflight" then
            if not player then return false end
            local x, y, z, o = player:GetLocation()
            local target = player:SpawnCreature(SMOKE_INFLIGHT_ENTRY, x + 28 * math.cos(o), y + 28 * math.sin(o), z, o, 3, 120000)
            if not target then log("spellget", "FAIL inflight: no target spawned") return false end
            player:GetMap():Data():Set("inflight", target:GetGUIDLow())
            log("spellget", "inflight armed on map " .. player:GetMapId() .. ": cast Fireball at " .. target:GetName() ..
                " guid " .. target:GetGUIDLow())
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
            -- Keep the fixate for 10 s, then check the creature still has the player as its
            -- victim, and only then clear it. The userdata do not outlive this call: use guids.
            local creatureGuid, playerGuid = creature:GetGUID(), player:GetGUID()
            CreateLuaEvent(function()
                local p = GetPlayerByGUID(playerGuid)
                local o = p and p:GetMap():GetWorldObject(creatureGuid)
                local c = o and o:ToCreature()
                if not c then log("threat", "FAIL creature or player gone after 10 s") return end
                local victim = c:GetVictim()
                log("threat", "after 10 s victim is the player=" .. tostring(victim ~= nil and victim:GetGUID() == playerGuid))
                c:ClearFixate()
                c:ClearThreat(p)
                log("threat", "after ClearThreat GetThreat=" .. c:GetThreat(p))
                log("threat", "FixateTarget(not on list)=" .. tostring(c:FixateTarget(c)))
            end, 10000, 1)
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
