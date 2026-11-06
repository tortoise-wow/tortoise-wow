-- Paired with the map-821 WorldMapArea patch produced by build_map_bounds.py.
-- Real positions stay in the native APIs; only existing map artwork is fitted
-- to its original geographic rectangle inside the expanded map boundary.
local state = nil
local tiles = {}
local mapScale = 694 / 870
local mapInset = (1 - mapScale) / 2
local expanded = false

-- Expanded geographic bounds expose space outside the original artwork.
-- Fill only that margin; otherwise the native frame's unrelated backing
-- textures show through as black blocks and fragments of an old map.
local margins = {}
local zoneLabel, continentLabel
local function FitMargins(show)
    if not show then
        for i = 1, 4 do if margins[i] then margins[i]:Hide() end end
        return
    end
    local w, h = WorldMapDetailFrame:GetWidth(), WorldMapDetailFrame:GetHeight()
    local x, y = w * mapInset, h * mapInset
    local rects = {{0,0,w,y},{0,-h+y,w,y},{0,-y,x,h-2*y},{w-x,-y,x,h-2*y}}
    for i = 1, 4 do
        if not margins[i] then
            margins[i] = WorldMapDetailFrame:CreateTexture("ManTechThornMapMargin" .. i, "ARTWORK")
            margins[i]:SetTexture("Interface\\QuestFrame\\QuestBG")
            margins[i]:SetTexCoord(0, 1, 0, 1)
        end
        local t, box = margins[i], rects[i]
        t:ClearAllPoints(); t:SetPoint("TOPLEFT", WorldMapDetailFrame, "TOPLEFT", box[1], box[2])
        t:SetWidth(box[3]); t:SetHeight(box[4]); t:Show()
    end
end
local function SetMapLabels(show)
    if show then
        if not zoneLabel and WorldMapZoneDropDownText then zoneLabel = WorldMapZoneDropDownText:GetText() or "" end
        if not continentLabel and WorldMapContinentDropDownText then continentLabel = WorldMapContinentDropDownText:GetText() or "" end
        if WorldMapZoneDropDownText then WorldMapZoneDropDownText:SetText("Thorn Gorge") end
        if WorldMapContinentDropDownText then WorldMapContinentDropDownText:SetText("Battleground") end
    else
        if zoneLabel and WorldMapZoneDropDownText and WorldMapZoneDropDownText:GetText() == "Thorn Gorge" then WorldMapZoneDropDownText:SetText(zoneLabel) end
        if continentLabel and WorldMapContinentDropDownText and WorldMapContinentDropDownText:GetText() == "Battleground" then WorldMapContinentDropDownText:SetText(continentLabel) end
        zoneLabel, continentLabel = nil, nil
    end
end

local function FitMap()
    local isThorn = GetMapInfo() == "ThornGorge"
    FitMargins(isThorn)
    SetMapLabels(isThorn)
    if isThorn then
        for i = 1, 12 do
            local tile = _G["WorldMapDetailTile" .. i]
            if tile then
                if not tiles[i] then
                    local point, relative, relativePoint, x, y = tile:GetPoint(1)
                    tiles[i] = {tile:GetWidth(), tile:GetHeight(), point, relative, relativePoint, x, y}
                    tiles[i].uv = {tile:GetTexCoord()}
                end
                tile:ClearAllPoints()
                local column = math.mod(i - 1, 4)
                local row = math.floor((i - 1) / 4)
                local width = math.min(256, 1002 - column * 256)
                local height = math.min(256, 668 - row * 256)
                tile:SetWidth(width * mapScale)
                tile:SetHeight(height * mapScale)
                tile:SetTexCoord(0, width / 256, 0, height / 256)
                tile:SetPoint("TOPLEFT", WorldMapDetailFrame, "TOPLEFT",
                    1002 * mapInset + column * 256 * mapScale,
                    -668 * mapInset - row * 256 * mapScale)
            end
        end
        -- Native Update has just set overlay coordinates against the old art.
        for i = 1, (NUM_WORLDMAP_OVERLAYS or 0) do
            local texture = _G["WorldMapOverlay" .. i]
            if texture and texture:IsShown() then
                local point, relative, relativePoint, x, y = texture:GetPoint(1)
                texture:SetWidth(texture:GetWidth() * mapScale)
                texture:SetHeight(texture:GetHeight() * mapScale)
                texture:ClearAllPoints()
                texture:SetPoint(point, relative, relativePoint,
                    1002 * mapInset + x * mapScale, -668 * mapInset + y * mapScale)
            end
        end
        expanded = true
    elseif expanded then
        for i = 1, 12 do
            local tile, saved = _G["WorldMapDetailTile" .. i], tiles[i]
            if tile and saved then
                tile:ClearAllPoints()
                tile:SetWidth(saved[1]); tile:SetHeight(saved[2])
                tile:SetTexCoord(unpack(saved.uv))
                tile:SetPoint(saved[3], saved[4], saved[5], saved[6], saved[7])
            end
        end
        expanded = false
    end
end

local function ColourCarrier(prefix)
    prefix = prefix or "WorldMapFlag"
    if not state or state.status ~= 3 or state.flag ~= 1 or GetMapInfo() ~= "ThornGorge" then return end
    if GetTime() - state.received > 5 then return end
    local token = state.team == 0 and "AllianceFlag" or state.team == 1 and "HordeFlag" or nil
    if token then
        for i = 1, GetNumBattlefieldFlagPositions() do
            local texture = _G[prefix .. i .. "Texture"]
            if texture then texture:SetTexture("Interface\\WorldStateFrame\\" .. token) end
        end
    end
end

local oldUpdate = WorldMapFrame_Update
function WorldMapFrame_Update()
    oldUpdate()
    FitMap()
    ColourCarrier()
end
local oldButtonUpdate = WorldMapButton_OnUpdate
function WorldMapButton_OnUpdate(elapsed)
    oldButtonUpdate(elapsed)
    ColourCarrier()
end

-- Blizzard's battlefield map is load-on-demand. Restore its original tile
-- size before native Update derives overlay scaling, then fit the artwork.
local miniHooked, miniExpanded = false, false
local miniAnchors = {}
local function RestoreMini()
    if not miniExpanded then return end
    for i = 1, 12 do
        local tile, saved = _G["BattlefieldMinimap" .. i], miniAnchors[i]
        if tile and saved then
            tile:SetWidth(BattlefieldMinimap:GetWidth() / 4)
            tile:SetHeight(BattlefieldMinimap:GetWidth() / 4)
            tile:SetTexCoord(unpack(saved.uv))
            tile:ClearAllPoints()
            tile:SetPoint(saved[1], saved[2], saved[3], saved[4], saved[5])
        end
    end
    miniExpanded = false
end
local function FitMini()
    if GetMapInfo() ~= "ThornGorge" then return end
    local w, h = BattlefieldMinimap:GetWidth(), BattlefieldMinimap:GetHeight()
    local piece = w / 4
    for i = 1, 12 do
        local tile = _G["BattlefieldMinimap" .. i]
        if tile then
            if not miniAnchors[i] then
                miniAnchors[i] = {tile:GetPoint(1)}
                miniAnchors[i].uv = {tile:GetTexCoord()}
            end
            local column, row = math.mod(i - 1, 4), math.floor((i - 1) / 4)
            local width, height = math.min(piece, w - column * piece), math.min(piece, h - row * piece)
            tile:SetWidth(width * mapScale); tile:SetHeight(height * mapScale)
            tile:SetTexCoord(0, width / piece, 0, height / piece)
            tile:ClearAllPoints()
            tile:SetPoint("TOPLEFT", BattlefieldMinimap, "TOPLEFT",
                w * mapInset + math.mod(i - 1, 4) * piece * mapScale,
                -h * mapInset - math.floor((i - 1) / 4) * piece * mapScale)
        end
    end
    for i = 1, (NUM_BATTLEFIELDMAP_OVERLAYS or 0) do
        local texture = _G["BattlefieldMinimapOverlay" .. i]
        if texture and texture:IsShown() then
            local point, relative, relativePoint, x, y = texture:GetPoint(1)
            texture:SetWidth(texture:GetWidth() * mapScale)
            texture:SetHeight(texture:GetHeight() * mapScale)
            texture:ClearAllPoints()
            texture:SetPoint(point, relative, relativePoint,
                w * mapInset + x * mapScale, -h * mapInset + y * mapScale)
        end
    end
    miniExpanded = true
end
local function HookMini()
    if miniHooked or not BattlefieldMinimap_Update or not BattlefieldMinimap_OnUpdate then return end
    miniHooked = true
    local update, tick = BattlefieldMinimap_Update, BattlefieldMinimap_OnUpdate
    function BattlefieldMinimap_Update()
        RestoreMini(); update(); FitMini()
        ColourCarrier("BattlefieldMinimapFlag")
    end
    function BattlefieldMinimap_OnUpdate(elapsed)
        if BattlefieldMinimap.resizing then
            RestoreMini(); tick(elapsed)
            -- Native resizing changes base tiles; refresh overlays from source.
            BattlefieldMinimap_Update()
        else tick(elapsed) end
        ColourCarrier("BattlefieldMinimapFlag")
    end
    BattlefieldMinimap_Update()
end
HookMini()

local frame = CreateFrame("Frame", "ManTechThornGorgeStatus", UIParent)
frame:SetWidth(360); frame:SetHeight(24)
frame:SetPoint("TOP", UIParent, "TOP", 0, -110)
local label = frame:CreateFontString(nil, "OVERLAY", "GameFontNormal")
label:SetAllPoints(frame)
frame:RegisterEvent("ADDON_LOADED")
frame:RegisterEvent("CHAT_MSG_ADDON")
frame:RegisterEvent("PLAYER_ENTERING_WORLD")
frame:RegisterEvent("ZONE_CHANGED_NEW_AREA")
frame:SetScript("OnEvent", function()
    if event == "ADDON_LOADED" then HookMini(); return end
    if event ~= "CHAT_MSG_ADDON" then state = nil; label:SetText(""); return end
    -- The native server sender is the receiving character, never another user.
    if arg1 ~= "MT_TG1" or arg3 ~= "GUILD" or arg4 ~= UnitName("player") or not arg2 or string.len(arg2) > 96 then return end
    local _, _, version, instance, status, flag, team, remaining =
        string.find(arg2, "^(%d+);(%d+);(%d+);(%d+);(%d+);(%d+)$")
    version, instance, status, flag, team, remaining = tonumber(version), tonumber(instance), tonumber(status), tonumber(flag), tonumber(team), tonumber(remaining)
    if version ~= 1 or not instance or instance <= 0 or not status or status > 4 or not flag or flag > 3 or not team or team > 2 or not remaining or remaining > 30000 then return end
    state = {instance=instance, status=status, flag=flag, team=team, remaining=remaining, received=GetTime()}
end)
frame:SetScript("OnUpdate", function()
    if not state or state.status ~= 3 or GetTime() - state.received > 5 then label:SetText(""); return end
    if state.flag == 2 or state.flag == 3 then
        local seconds = math.max(0, math.ceil(state.remaining / 1000 - (GetTime() - state.received)))
        label:SetText("Flag returns to centre: " .. seconds .. "s")
    elseif state.flag == 1 then
        label:SetText(state.team == 0 and "|cff4488ffAlliance carries the flag|r" or state.team == 1 and "|cffff4444Horde carries the flag|r" or "Flag carried")
    else
        label:SetText("Flag at centre")
    end
end)
