/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ReputationMgr.h"
#include "DBCStores.h"
#include "Player.h"
#include "ScriptObjects.h"
#include "WorldPacket.h"
#include "ObjectMgr.h"
#include <numeric>

const int32 ReputationMgr::PointsInRank[MAX_REPUTATION_RANK] = {36000, 3000, 3000, 3000, 6000, 12000, 21000, 1000};

int32 ReputationMgr::GetRepPointsToRank(ReputationRank rank)
{
    return std::accumulate(PointsInRank, PointsInRank + rank, 0) - 42000;
}

ReputationRank ReputationMgr::ReputationToRank(int32 standing)
{
    int32 limit = Reputation_Cap + 1;
    for (int i = MAX_REPUTATION_RANK - 1; i >= MIN_REPUTATION_RANK; --i)
    {
        limit -= PointsInRank[i];
        if (standing >= limit)
            return ReputationRank(i);
    }

    return MIN_REPUTATION_RANK;
}

FactionState const* ReputationMgr::GetState(FactionEntry const* factionEntry) const
{
    return (factionEntry && factionEntry->CanHaveReputation()) ? GetState(RepListID(factionEntry->reputationListID)) : nullptr;
}

FactionState const* ReputationMgr::GetState(RepListID id) const
{
    FactionStateList::const_iterator repItr = m_factions.find(id);
    return repItr != m_factions.end() ? &repItr->second : nullptr;
}

int32 ReputationMgr::GetReputation(uint32 faction_id) const
{
    FactionEntry const *factionEntry = sObjectMgr.GetFactionEntry(faction_id);

    if (!factionEntry)
    {
        sLog.outError("ReputationMgr::GetReputation: Can't get reputation of %s for unknown faction (faction id) #%u.", m_player->GetName(), faction_id);
        return 0;
    }

    return GetReputation(factionEntry);
}

int32 ReputationMgr::GetReputation(FactionEntry const* factionEntry) const
{
    if (!factionEntry)
        return 0;

    if (FactionState const* state = GetState(factionEntry))
        return GetBaseReputation(factionEntry) + state->Standing;

    return 0;
}

int32 ReputationMgr::GetBaseReputation(FactionEntry const* factionEntry) const
{
    if (!factionEntry)
        return 0;

    uint32 raceMask = m_player->GetRaceMask();
    uint32 classMask = m_player->GetClassMask();

    int idx = factionEntry->GetIndexFitTo(raceMask, classMask);

    return idx >= 0 ? factionEntry->BaseRepValue[idx] : 0;
}

bool ReputationMgr::IsAtWar(uint32 faction_id) const
{
    FactionEntry const* factionEntry = sObjectMgr.GetFactionEntry(faction_id);

    if (!factionEntry)
    {
        sLog.outError("ReputationMgr::IsAtWar: Can't get state of 'At War' flag of %s for unknown faction (faction id) #%u.", m_player->GetName(), faction_id);
        return false;
    }

    return IsAtWar(factionEntry);
}

bool ReputationMgr::IsAtWar(FactionEntry const* factionEntry) const
{
    if (!factionEntry)
        return false;

    if (FactionState const* state = GetState(factionEntry))
        return (state->Flags & FACTION_FLAG_AT_WAR) != 0;

    return false;
}

ReputationRank ReputationMgr::GetRank(FactionEntry const* factionEntry) const
{
    int32 reputation = GetReputation(factionEntry);
    return ReputationToRank(reputation);
}

ReputationRank ReputationMgr::GetBaseRank(FactionEntry const* factionEntry) const
{
    int32 reputation = GetBaseReputation(factionEntry);
    return ReputationToRank(reputation);
}

ReputationRank const* ReputationMgr::GetForcedRankIfAny(FactionTemplateEntry const* factionTemplateEntry) const
{
    if (!factionTemplateEntry)
        return nullptr;

    ForcedReactions::const_iterator forceItr = m_forcedReactions.find(factionTemplateEntry->faction);
    return forceItr != m_forcedReactions.end() ? &forceItr->second : nullptr;
}

void ReputationMgr::ApplyForceReaction(uint32 faction_id, ReputationRank rank, bool apply)
{
    if (apply)
        m_forcedReactions[faction_id] = rank;
    else
        m_forcedReactions.erase(faction_id);
}

uint32 ReputationMgr::GetDefaultStateFlags(FactionEntry const* factionEntry) const
{
    if (!factionEntry)
        return 0;

    uint32 raceMask = m_player->GetRaceMask();
    uint32 classMask = m_player->GetClassMask();

    int idx = factionEntry->GetIndexFitTo(raceMask, classMask);

    return idx >= 0 ? factionEntry->ReputationFlags[idx] : 0;
}

void ReputationMgr::SendForceReactions()
{
    WorldPacket data;
    data.Initialize(SMSG_SET_FORCED_REACTIONS, 4 + m_forcedReactions.size() * (4 + 4));
    data << uint32(m_forcedReactions.size());

    for (const auto& itr : m_forcedReactions)
    {
        data << uint32(itr.first);  // faction_id (Faction.dbc)
        data << uint32(itr.second); // reputation rank
    }

    m_player->SendDirectMessage(&data);
}

void ReputationMgr::SendState(FactionState const* faction)
{
    if (!faction)
        return;

    uint32 count = 1;

    WorldPacket data(SMSG_SET_FACTION_STANDING, (16));      // last check 2.4.0
    size_t p_count = data.wpos();
    data << (uint32) count;                                 // placeholder

    data << (uint32) faction->ReputationListID;
    data << (uint32) faction->Standing;

    for (auto& m_faction : m_factions)
    {
        FactionState& subFaction = m_faction.second;
        if (subFaction.needSend)
        {
            subFaction.needSend = false;
            if (subFaction.ReputationListID != faction->ReputationListID)
            {
                data << (uint32)subFaction.ReputationListID;
                data << (uint32)subFaction.Standing;
                ++count;
            }
        }
    }

    data.put<uint32>(p_count, count);
    m_player->SendDirectMessage(&data);
}

void ReputationMgr::SendInitialReputations()
{
    uint8 factionCount = 64;
    WorldPacket data(SMSG_INITIALIZE_FACTIONS, 4 + (factionCount * 5));
    data << uint32(factionCount);

    RepListID a = 0;

    for (auto& m_faction : m_factions)
    {
        // fill in absent fields
        for (; a != m_faction.first; ++a)
        {
            data << uint8(0x00);
            data << uint32(0x00000000);
        }

        // fill in encountered data
        data << uint8(m_faction.second.Flags);
        data << uint32(m_faction.second.Standing);

        m_faction.second.needSend = false;

        ++a;
    }

    // fill in absent fields
    for (; a != /*factionCount*/69; a++)
    {
        data << uint8(0x00);
        data << uint32(0x00000000);
    }

    m_player->SendDirectMessage(&data);
}

void ReputationMgr::SendVisible(FactionState const* faction) const
{
    if (!faction)
        return;

    if (m_player->GetSession()->PlayerLoading())
        return;

    // make faction visible in reputation list at client
    WorldPacket data(SMSG_SET_FACTION_VISIBLE, 4);
    data << faction->ReputationListID;
    m_player->SendDirectMessage(&data);
}

void ReputationMgr::Initialize()
{
    m_factions.clear();

    for (auto const& itr : sObjectMgr.GetFactionMap())
    {
        FactionEntry const *factionEntry = &itr.second;

        if (factionEntry && factionEntry->CanHaveReputation())
        {
            FactionState newFaction;
            newFaction.ID = factionEntry->ID;
            newFaction.ReputationListID = factionEntry->reputationListID;
            newFaction.Standing = 0;
            newFaction.Flags = GetDefaultStateFlags(factionEntry);
            newFaction.needSend = true;
            newFaction.needSave = true;

            m_factions[newFaction.ReputationListID] = newFaction;
        }
    }
}

bool ReputationMgr::SetReputation(FactionEntry const* factionEntry, int32 standing, bool incremental, bool noSpillover, bool noBase)
{
    if (!factionEntry)
        return false;

    if (!noSpillover)
    {
        // if spillover definition exists in DB
        if (RepSpilloverTemplate const* repTemplate = sObjectMgr.GetRepSpilloverTemplate(factionEntry->ID))
        {
            for (uint32 i = 0; i < MAX_SPILLOVER_FACTIONS; ++i)
            {
                if (repTemplate->faction[i])
                {
                    FactionEntry const* factionEntry = sObjectMgr.GetFactionEntry(repTemplate->faction[i]);
                    if (factionEntry && m_player->GetReputationRank(repTemplate->faction[i]) <= ReputationRank(repTemplate->faction_rank[i]))
                    {
                        // bonuses are already given, so just modify standing by rate
                        int32 spilloverRep = standing * repTemplate->faction_rate[i];
                        SetOneFactionReputation(factionEntry, spilloverRep, incremental, noBase);

                        // report spillover rep to client
                        FactionStateList::iterator faction = m_factions.find(RepListID(factionEntry->reputationListID));
                        if (faction != m_factions.end())
                            SendState(&faction->second);
                    }
                }
            }
        }
    }
    
    // spillover done, update faction itself
    bool res = false;
    FactionStateList::iterator faction = m_factions.find(RepListID(factionEntry->reputationListID));
    if (faction != m_factions.end())
    {
        res = SetOneFactionReputation(factionEntry, standing, incremental, noBase);
        SendState(&faction->second);
    }
        
    if ((factionEntry->ID == 576 || factionEntry->ID == 609 || factionEntry->ID == 59) &&
        m_player->HasEarnedTitle(TITLE_DIPLOMAT))
        m_player->AwardTitle(TITLE_DIPLOMAT);

    return res;
}

bool ReputationMgr::SetOneFactionReputation(FactionEntry const* factionEntry, int32 standing, bool incremental, bool noBase)
{
    if (!factionEntry)
        return false;

    FactionStateList::iterator itr = m_factions.find(RepListID(factionEntry->reputationListID));
    if (itr != m_factions.end())
    {
        FactionState& faction = itr->second;
        int32 BaseRep = GetBaseReputation(factionEntry);

        if (incremental)
            standing += faction.Standing + BaseRep;

        ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_REPUTATION_CHANGE, [&](PlayerScript* script)
        {
            script->OnReputationChange(m_player, factionEntry->ID, standing);
        });

        if (standing > Reputation_Cap)
            standing = Reputation_Cap;
        else if (standing < Reputation_Bottom)
            standing = Reputation_Bottom;

        ReputationRank rankOld = ReputationToRank(faction.Standing + BaseRep);
        ReputationRank rankNew = ReputationToRank(standing);

        if (noBase)
            faction.Standing = standing;
        else
            faction.Standing = standing - BaseRep;

        faction.needSend = true;
        faction.needSave = true;

        SetVisible(&faction);

        if (rankNew != rankOld)
        {
            // Server alters "At war" flag on two occasions:
            // When reputation dips to "Hostile": forced tick and now locked for manual changes
            if (rankNew < REP_UNFRIENDLY && rankNew < rankOld && rankOld > REP_HOSTILE)
                SetAtWar(&itr->second, true);
            // When reputation improves to "Neutral": untick by id, can be manually overriden for eligible factions
            else if (rankNew > REP_UNFRIENDLY && rankNew > rankOld && rankOld < REP_NEUTRAL)
                SetAtWar(RepListID(factionEntry->reputationListID), false);
        }

        m_player->ReputationChanged(factionEntry);

        if (rankNew > rankOld)
            return true;
    }

    return false;
}

void ReputationMgr::SetVisible(FactionTemplateEntry const* factionTemplateEntry)
{
    if (!factionTemplateEntry || !factionTemplateEntry->faction)
        return;

    if (FactionEntry const *factionEntry = sObjectMgr.GetFactionEntry(factionTemplateEntry->faction))
        SetVisible(factionEntry);
}

void ReputationMgr::SetVisible(FactionEntry const* factionEntry)
{
    if (!factionEntry || !factionEntry->CanHaveReputation())
        return;

    FactionStateList::iterator itr = m_factions.find(RepListID(factionEntry->reputationListID));
    if (itr == m_factions.end())
        return;

    SetVisible(&itr->second);
}

void ReputationMgr::SetVisible(FactionState* faction)
{
    if (!faction)
        return;

    // always invisible or hidden faction can't be make visible
    if (faction->Flags & (FACTION_FLAG_INVISIBLE_FORCED | FACTION_FLAG_HIDDEN))
        return;

    // already set
    if (faction->Flags & FACTION_FLAG_VISIBLE)
        return;

    faction->Flags |= FACTION_FLAG_VISIBLE;
    faction->needSend = true;
    faction->needSave = true;

    SendVisible(faction);
}

bool ReputationMgr::SetAtWar(RepListID repListID, bool on)
{
    FactionStateList::iterator itr = m_factions.find(repListID);
    if (itr == m_factions.end())
        return false;

    // always invisible or hidden faction can't change war state
    if (itr->second.Flags & (FACTION_FLAG_INVISIBLE_FORCED | FACTION_FLAG_HIDDEN))
        return false;

    return SetAtWar(&itr->second, on);
}

bool ReputationMgr::SetAtWar(FactionState* faction, bool atWar)
{
    if (!faction)
        return false;

    // already set
    if (((faction->Flags & FACTION_FLAG_AT_WAR) != 0) && atWar)
        return false;

    // not allow declare war to faction unless already hated or less
    if (atWar && (faction->Flags & FACTION_FLAG_PEACE_FORCED) != 0 && ReputationToRank(faction->Standing) > REP_HATED)
        return false;

    if (atWar)
        faction->Flags |= FACTION_FLAG_AT_WAR;
    else
        faction->Flags &= ~uint32(FACTION_FLAG_AT_WAR);

    faction->needSend = true;
    faction->needSave = true;

    return true;
}

void ReputationMgr::SetInactive(RepListID repListID, bool on)
{
    FactionStateList::iterator itr = m_factions.find(repListID);
    if (itr == m_factions.end())
        return;

    SetInactive(&itr->second, on);
}

void ReputationMgr::SetInactive(FactionState* faction, bool inactive)
{
    // always invisible or hidden faction can't be inactive
    if (inactive && ((faction->Flags & (FACTION_FLAG_INVISIBLE_FORCED | FACTION_FLAG_HIDDEN)) || !(faction->Flags & FACTION_FLAG_VISIBLE)))
        return;

    // already set
    if (((faction->Flags & FACTION_FLAG_INACTIVE) != 0) == inactive)
        return;

    if (inactive)
        faction->Flags |= FACTION_FLAG_INACTIVE;
    else
        faction->Flags &= ~FACTION_FLAG_INACTIVE;

    faction->needSend = true;
    faction->needSave = true;
}

void ReputationMgr::LoadFromDB(QueryResult *result)
{
    // Set initial reputations (so everything is nifty before DB data load)
    Initialize();

    if (result)
    {
        do
        {
            Field *fields = result->Fetch();

            FactionEntry const *factionEntry = sObjectMgr.GetFactionEntry(fields[0].GetUInt32());
            if (factionEntry && factionEntry->CanHaveReputation())
            {
                FactionState* faction = &m_factions[factionEntry->reputationListID];

                // update standing to current
                faction->Standing = int32(fields[1].GetUInt32());

                uint32 dbFactionFlags = fields[2].GetUInt32();

                if (dbFactionFlags & FACTION_FLAG_VISIBLE)
                    SetVisible(faction);                    // have internal checks for forced invisibility

                if (dbFactionFlags & FACTION_FLAG_INACTIVE)
                    SetInactive(faction, true);             // have internal checks for visibility requirement

                if (dbFactionFlags & FACTION_FLAG_AT_WAR)   // DB at war
                    SetAtWar(faction, true);                // have internal checks for FACTION_FLAG_PEACE_FORCED
                else                                        // DB not at war
                {
                    // allow remove if visible (and then not FACTION_FLAG_INVISIBLE_FORCED or FACTION_FLAG_HIDDEN)
                    if (faction->Flags & FACTION_FLAG_VISIBLE)
                        SetAtWar(faction, false);           // have internal checks for FACTION_FLAG_PEACE_FORCED
                }

                // set atWar for hostile
                ForcedReactions::const_iterator forceItr = m_forcedReactions.find(factionEntry->ID);
                if (forceItr != m_forcedReactions.end())
                {
                    if (forceItr->second <= REP_HOSTILE)
                        SetAtWar(faction, true);
                }
                else if (GetRank(factionEntry) <= REP_HOSTILE)
                    SetAtWar(faction, true);

                // reset changed flag if values similar to saved in DB
                if (faction->Flags == dbFactionFlags)
                {
                    faction->needSend = false;
                    faction->needSave = false;
                }
            }
        }
        while (result->NextRow());
    }
}

#include "ReputationSave.inc"
