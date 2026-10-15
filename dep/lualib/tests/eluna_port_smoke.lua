--[[
Eluna port smoke script (tortoise-wow feat/eluna).

Copy it into the server's Eluna.ScriptPath (default ./lua_scripts). Every Lua
state (the world state and one per map) loads it, so most lines carry the
state they ran in: "world" until a map update names the state's map.

It covers, each with a log line starting "ELUNA_SMOKE":
  login        PLAYER_EVENT_ON_LOGIN (3) on the player's map state, plus a
               per-player timed event (3 shots, 2 s apart)
  command      PLAYER_EVENT_ON_COMMAND (42): ".elunasmoke" is answered and
               swallowed (world state)
  gossip       GOSSIP_EVENT_ON_HELLO / ON_SELECT on creature SMOKE_GOSSIP_ENTRY
  map update   MAP_EVENT_ON_UPDATE (23): first tick of each map state, then
               one line per SMOKE_MAP_LOG_EVERY ticks
  timed event  CreateLuaEvent: 3 shots, 5 s apart, in every state
  reload       the "loaded" line carries a load counter; ".reload eluna" makes
               every state run the file again
  game event   GAME_EVENT_START / GAME_EVENT_STOP (34/35): one line per event
               transition, always in the world state, also when a map script
               command started or stopped the event
  dialog       CREATURE_EVENT_ON_DIALOG_STATUS (35) on SMOKE_GOSSIP_ENTRY: a
               notification only at this Eluna pin (no status is returned)
]]

local SMOKE_GOSSIP_ENTRY = 823 -- Deputy Willem, Northshire; change it for another test spot
local SMOKE_MAP_LOG_EVERY = 600

local state = "world"
local mapTicks = 0

-- A plain global survives nothing: a reload builds a new lua_State. The
-- counter therefore says how many times this state ran the file since it was
-- made, which is 1 after a reload as well; the reload is visible as a second
-- "loaded" line for the same state.
ELUNA_SMOKE_LOADS = (ELUNA_SMOKE_LOADS or 0) + 1
PrintInfo("ELUNA_SMOKE loaded (load " .. ELUNA_SMOKE_LOADS .. ", core " .. tostring(GetCoreName()) .. ")")

RegisterServerEvent(14, function()
    PrintInfo("ELUNA_SMOKE world startup")
end)

RegisterServerEvent(23, function(event, map, diff)
    mapTicks = mapTicks + 1
    if mapTicks == 1 then
        state = "map " .. map:GetMapId() .. "/" .. map:GetInstanceId()
        PrintInfo("ELUNA_SMOKE map update first tick state=" .. state)
    elseif mapTicks % SMOKE_MAP_LOG_EVERY == 0 then
        PrintInfo("ELUNA_SMOKE map update tick " .. mapTicks .. " state=" .. state)
    end
end)

CreateLuaEvent(function(eventId, delay, repeats)
    PrintInfo("ELUNA_SMOKE timer state=" .. state .. " repeats_left=" .. tostring(repeats))
end, 5000, 3)

RegisterPlayerEvent(3, function(event, player)
    PrintInfo("ELUNA_SMOKE login " .. player:GetName() .. " map=" .. player:GetMapId() .. " state=" .. state)
    player:SendBroadcastMessage("ELUNA_SMOKE hello " .. player:GetName())
    player:RegisterEvent(function(eventId, delay, repeats, obj)
        PrintInfo("ELUNA_SMOKE player timer " .. obj:GetName() .. " repeats_left=" .. tostring(repeats) .. " state=" .. state)
    end, 2000, 3)
end)

RegisterPlayerEvent(42, function(event, player, command)
    if command ~= "elunasmoke" then
        return
    end
    local who = player and player:GetName() or "console"
    PrintInfo("ELUNA_SMOKE command from " .. who .. " state=" .. state)
    if player then
        player:SendBroadcastMessage("ELUNA_SMOKE command answered")
    end
    return false
end)

RegisterCreatureGossipEvent(SMOKE_GOSSIP_ENTRY, 1, function(event, player, creature)
    PrintInfo("ELUNA_SMOKE gossip hello " .. player:GetName() .. " -> " .. creature:GetName() .. " state=" .. state)
    player:GossipMenuAddItem(0, "ELUNA_SMOKE option", 1, 7)
    player:GossipSendMenu(1, creature)
    return true
end)

RegisterCreatureGossipEvent(SMOKE_GOSSIP_ENTRY, 2, function(event, player, creature, sender, intid, code)
    PrintInfo("ELUNA_SMOKE gossip select sender=" .. sender .. " intid=" .. intid .. " state=" .. state)
    player:SendBroadcastMessage("ELUNA_SMOKE gossip selected " .. intid)
    player:GossipComplete()
end)

RegisterServerEvent(34, function(event, gameEventId)
    PrintInfo("ELUNA_SMOKE game event start " .. gameEventId .. " state=" .. state)
end)

RegisterServerEvent(35, function(event, gameEventId)
    PrintInfo("ELUNA_SMOKE game event stop " .. gameEventId .. " state=" .. state)
end)

RegisterCreatureEvent(SMOKE_GOSSIP_ENTRY, 35, function(event, player, creature)
    PrintInfo("ELUNA_SMOKE dialog status " .. player:GetName() .. " -> " .. creature:GetName() .. " state=" .. state)
end)
