--[[
Eluna loot smoke script (tortoise-wow dep/eluna-patches/0011).

Copy it into the server's Eluna.ScriptPath (default ./lua_scripts), build with
-DBUILD_ELUNA=ON. Every line it logs starts with "ELUNA_LOOT". Lines say
"PASS" or "FAIL" where there is something to check.

  load        every state: GetItemTemplate(2589) (Linen Cloth) has quality 1,
              Item:GetTemplate exists, GetStateMapId() is -1 in the world state
              and the map id in a map state
  map change  PLAYER_EVENT_ON_MAP_CHANGE (28), in the player's map state:
              Group:GetMembersOnMap lists the group members on this map (and
              Group:GetMembers is refused there), Player:IsBot (true for a
              TortoiseBots bot), Unit:GetCritterGUID (a guid with a companion
              out, nil after dismiss: run the command below to see it)
  command     ".lootsmoke" with a dead creature selected (world state): logs
              GetLoot money, unlooted count, loot type and the items; then
              ".lootsmoke take" takes the first normal item with SetItemLooted,
              sets coin and unlooted count to 0 (SetMoney, SetUnlootedCount)
              and calls AllLootRemovedFromCorpse. Skinning must start (skin a
              skinnable creature) and the corpse stops sparkling.
  critter     ".lootsmoke pet" logs GetCritterGUID of the player
  pet roll    Creature:StartGroupLootRoll(player) (patch 0011, map states only).
              ".lootsmoke startroll" with a dead creature selected, in the world
              state: the method must not exist there (PASS "refused"). In a
              group with group loot or need before greed, kill a creature that
              drops an item at or above the loot threshold: 1 s after the kill
              (PLAYER_EVENT_ON_KILL_CREATURE, 7, in the map state) it is called
              twice; the first call logs started=true and the group gets the
              roll windows, the second logs started=false (rolls already
              started: PASS). Solo, master loot or free for all it logs
              started=false for both calls.
  roll        PLAYER_EVENT_ON_GROUP_ROLL_REWARD_ITEM (56): in a group of two
              with group loot, a need or greed winner logs the item, count,
              vote type and the Roll getters
]]

local function log(text)
    PrintInfo("ELUNA_LOOT " .. text)
end

local state = "world"
if GetStateMapId then
    local id = GetStateMapId()
    state = id == -1 and "world" or ("map " .. id .. "/" .. tostring(GetStateInstanceId and GetStateInstanceId()))
end

do
    local ok, tpl = pcall(GetItemTemplate, 2589)
    if ok and tpl then
        log(("load state=%s item 2589 quality=%s name=%s %s"):format(state, tostring(tpl:GetQuality()),
            tostring(tpl:GetName()), tpl:GetQuality() == 1 and "PASS" or "FAIL"))
        log("load state=" .. state .. " stack=" .. tostring(tpl:GetMaxStackSize()) ..
            " flags=" .. tostring(tpl:GetFlags()) .. " sell=" .. tostring(tpl:GetSellPrice()))
    else
        log("load state=" .. state .. " GetItemTemplate FAIL " .. tostring(tpl))
    end
    log("load state=" .. state .. " Item.GetTemplate " .. (Item and Item.GetTemplate and "PASS" or "FAIL"))
end

RegisterPlayerEvent(28, function(event, player)
    local ok, err = pcall(function()
        local group = player:GetGroup()
        local guid = player:GetCritterGUID()
        log(("map change %s state=%s map=%d bot=%s critter=%s"):format(player:GetName(), state, player:GetMapId(),
            tostring(player:IsBot()), tostring(guid)))
        if group then
            local members = group:GetMembersOnMap()
            local names = {}
            for _, m in ipairs(members) do
                names[#names + 1] = m:GetName() .. (m:IsBot() and "(bot)" or "")
            end
            log("map change state=" .. state .. " GetMembersOnMap=" .. #members .. " " .. table.concat(names, ","))
            local refused = not pcall(group.GetMembers, group)
            log("map change state=" .. state .. " GetMembers refused=" .. tostring(refused) ..
                " (expected true in a map state) " .. ((refused == (state ~= "world")) and "PASS" or "FAIL"))
        else
            log("map change state=" .. state .. " no group")
        end
    end)
    if not ok then
        log("map change FAIL " .. tostring(err))
    end
end)

local function describe(items)
    local parts = {}
    for _, it in ipairs(items) do
        parts[#parts + 1] = ("[%d]=%dx%d%s%s%s"):format(it.index, it.id, it.count,
            it.is_looted and " looted" or "", it.is_blocked and " blocked" or "",
            it.roll_winner_guid and " waiting" or "")
    end
    return table.concat(parts, " ")
end

RegisterPlayerEvent(42, function(event, player, command)
    local verb = command:match("^lootsmoke%s*(%a*)$")
    if not verb then
        return
    end
    if not player then
        return false
    end
    local ok, err = pcall(function()
        if verb == "pet" then
            log("pet " .. player:GetName() .. " critter=" .. tostring(player:GetCritterGUID()))
            return
        end
        local corpse = player:GetSelection()
        if not corpse or not corpse.GetLoot then
            log("command: select a dead creature first")
            return
        end
        if verb == "startroll" then
            log("startroll in the world state " .. ((corpse.StartGroupLootRoll == nil) and "PASS refused" or "FAIL method exists"))
            return
        end
        local loot = corpse:GetLoot()
        log(("loot %s money=%d unlooted=%d type=%d items: %s quest: %s reward_distance=%s loot_distance=%s"):format(
            corpse:GetName(), loot:GetMoney(), loot:GetUnlootedCount(), loot:GetLootType(),
            describe(loot:GetItems()), describe(loot:GetQuestItems()),
            tostring(player:IsAtGroupRewardDistance(corpse)), tostring(player:IsAtLootRewardDistance(corpse))))
        if verb == "take" then
            local first = loot:GetItems()[1]
            if first then
                local matched = loot:SetItemLooted(first.id, first.count)
                log("take SetItemLooted(" .. first.id .. ") matched=" .. tostring(matched))
            end
            loot:SetMoney(0)
            loot:SetUnlootedCount(0)
            corpse:AllLootRemovedFromCorpse()
            local again = corpse:GetLoot()
            log(("take after: money=%d unlooted=%d %s"):format(again:GetMoney(), again:GetUnlootedCount(),
                (again:GetMoney() == 0 and again:GetUnlootedCount() == 0) and "PASS" or "FAIL"))
        end
    end)
    if not ok then
        log("command FAIL " .. tostring(err))
    end
    player:SendBroadcastMessage("ELUNA_LOOT command done")
    return false
end)

-- Map state: kill a creature, then start the group roll twice (second must be false).
RegisterPlayerEvent(7, function(event, player, killed)
    if not player or not killed or not killed.StartGroupLootRoll then
        return
    end
    local pguid = player:GetGUID()
    killed:RegisterEvent(function(eventId, delay, calls, corpse)
        local ok, err = pcall(function()
            local p = GetPlayerByGUID(pguid)
            if not p then
                return
            end
            local first = corpse:StartGroupLootRoll(p)
            local second = corpse:StartGroupLootRoll(p)
            log(("startroll %s started=%s again=%s %s"):format(corpse:GetName(), tostring(first), tostring(second),
                (second == false) and "PASS idempotent" or "FAIL started twice"))
        end)
        if not ok then
            log("startroll FAIL " .. tostring(err))
        end
    end, 1000, 1)
end)

RegisterPlayerEvent(56, function(event, player, item, count, voteType, roll)
    local ok, err = pcall(function()
        log(("roll winner %s item=%d x%d vote=%d (1 need, 2 greed) slot=%d rolling=%d need=%d greed=%d pass=%d mask=%d target=%s votes=%d"):format(
            player:GetName(), roll:GetItemId(), count, voteType, roll:GetItemSlot(), roll:GetTotalPlayersRolling(),
            roll:GetTotalNeed(), roll:GetTotalGreed(), roll:GetTotalPass(), roll:GetRollVoteMask(),
            tostring(roll:GetLootedTargetGUID()), #roll:GetPlayerVoteGUIDs()))
        log("roll winner item entry matches " .. ((item:GetEntry() == roll:GetItemId()) and "PASS" or "FAIL"))
    end)
    if not ok then
        log("roll FAIL " .. tostring(err))
    end
end)

log("loaded state=" .. state)
