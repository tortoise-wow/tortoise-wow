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

#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "Opcodes.h"
#include "ObjectMgr.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Chat.h"
#include "SocialMgr.h"
#include "Util.h"
#include "Language.h"
#include "World.h"
#include "Anticheat.h"
#include "ScriptObjects.h"
#include "Item.h"

#define MAX_UNCOMPRESSED_PACKET_SIZE 0x8000 

//// MemberSlot ////////////////////////////////////////////
void MemberSlot::SetMemberStats(Player* player)
{
    Name   = player->GetName();
    Level  = player->GetLevel();
    Class  = player->GetClass();
    ZoneId = player->GetCachedZoneId();
}

void MemberSlot::UpdateLogoutTime()
{
    LogoutTime = time(nullptr);
}

void MemberSlot::SetPublicNote(std::string const& publicNote)
{
    if (PublicNote == publicNote)
        return;

    PublicNote = publicNote;

    // pnote now can be used for encoding to DB
    CharacterDatabase.escape_string(PublicNote);
    CharacterDatabase.PExecute("UPDATE guild_member SET pnote = '%s' WHERE guid = '%u'", PublicNote.c_str(), guid.GetCounter());
}

void MemberSlot::SetOfficerNote(std::string const& officerNote)
{
    if (OfficerNote == officerNote)
        return;

    OfficerNote = officerNote;

    // offnote now can be used for encoding to DB
    CharacterDatabase.escape_string(OfficerNote);
    CharacterDatabase.PExecute("UPDATE guild_member SET offnote = '%s' WHERE guid = '%u'", OfficerNote.c_str(), guid.GetCounter());
}

void MemberSlot::ChangeRank(uint32 newRank)
{
    RankId = newRank;

    Player *player = sObjectMgr.GetPlayer(guid);
    // If player not online data in data field will be loaded from guild tabs no need to update it !!
    if (player)
        player->SetRank(newRank);

    CharacterDatabase.PExecute("UPDATE guild_member SET `rank`='%u' WHERE guid='%u'", newRank, guid.GetCounter());
}

//// Guild /////////////////////////////////////////////////

Guild::Guild() : m_Id(0), m_EmblemStyle(0), m_EmblemColor(0), m_BorderStyle(0), m_BorderColor(0), m_BackgroundColor(0), m_accountsNumber(0),
    m_CreatedYear(0), m_CreatedMonth(0), m_CreatedDay(0), m_GuildEventLogNextGuid(0), _Bank(nullptr), _InfernoBank(nullptr)
{
}

Guild::~Guild()
{
}

bool Guild::Create(Petition* petition, Player* leader)
{
    if (!Create(leader, petition->GetName()))
        return false;

    PetitionSignatureList signatures = petition->GetSignatureList();
    for (auto iter = signatures.cbegin(); iter != signatures.cend(); ++iter)
    {
        PetitionSignature* signature = *iter;

        if (signature->GetSignatureGuid().IsEmpty())
            continue;

        AddMember(signature->GetSignatureGuid(), GetLowestRank());
    }

    return true;
}

bool Guild::Create(Player* leader, std::string gname)
{
    if (sGuildMgr.GetGuildByName(gname))
        return false;

    WorldSession* lSession = leader->GetSession();
    if (!lSession)
        return false;

    m_LeaderGuid = leader->GetObjectGuid();
    m_Name = gname;
    m_info.clear();
    m_motd = "No message set.";
    m_Id = sObjectMgr.GenerateGuildId();

    // creating data
    time_t now = time(0);
    tm local = *(localtime(&now));                          // dereference and assign
    m_CreatedDay   = local.tm_mday;
    m_CreatedMonth = local.tm_mon + 1;
    m_CreatedYear  = local.tm_year + 1900;

    DEBUG_LOG("GUILD: creating guild %s to leader: %s", gname.c_str(), m_LeaderGuid.GetString().c_str());

    // gname already assigned to Guild::name, use it to encode string for DB
    CharacterDatabase.escape_string(gname);

    std::string dbGINFO = m_info;
    std::string dbMOTD = m_motd;
    CharacterDatabase.escape_string(dbGINFO);
    CharacterDatabase.escape_string(dbMOTD);

    CharacterDatabase.BeginTransaction();
    // CharacterDatabase.PExecute("DELETE FROM guild WHERE guildid='%u'", Id); - MAX(guildid)+1 not exist
    CharacterDatabase.PExecute("DELETE FROM guild_member WHERE guildid='%u'", m_Id);
    CharacterDatabase.PExecute("INSERT INTO guild (guildid,name,leaderguid,info,motd,createdate,EmblemStyle,EmblemColor,BorderStyle,BorderColor,BackgroundColor) "
                               "VALUES('%u','%s','%u', '%s', '%s','" UI64FMTD "','%u','%u','%u','%u','%u')",
                               m_Id, gname.c_str(), m_LeaderGuid.GetCounter(), dbGINFO.c_str(), dbMOTD.c_str(), uint64(now), m_EmblemStyle, m_EmblemColor, m_BorderStyle, m_BorderColor, m_BackgroundColor);
    CharacterDatabase.CommitTransaction();

    CreateDefaultGuildRanks(lSession->GetSessionDbLocaleIndex());

    _Bank = new GuildBank{ false };
	_Bank->SetGuild(this);

    _InfernoBank = new GuildBank{ true };
    _InfernoBank->SetGuild(this);

    GuildAddStatus status = AddMember(m_LeaderGuid, (uint32)GR_GUILDMASTER);
    if (status == GuildAddStatus::OK)
    {
        ScriptRegistry<GuildScript>::ForEach([&](GuildScript* script)
        {
            script->OnCreate(this, leader, m_Name);
        });
    }

    return status == GuildAddStatus::OK;
}

void Guild::CreateDefaultGuildRanks(int locale_idx)
{
    CharacterDatabase.PExecute("DELETE FROM guild_rank WHERE guildid='%u'", m_Id);

    CreateRank(sObjectMgr.GetMangosString(LANG_GUILD_MASTER, locale_idx),   GR_RIGHT_ALL);
    CreateRank(sObjectMgr.GetMangosString(LANG_GUILD_OFFICER, locale_idx),  GR_RIGHT_ALL);
    CreateRank(sObjectMgr.GetMangosString(LANG_GUILD_VETERAN, locale_idx),  GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
    CreateRank(sObjectMgr.GetMangosString(LANG_GUILD_MEMBER, locale_idx),   GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
    CreateRank(sObjectMgr.GetMangosString(LANG_GUILD_INITIATE, locale_idx), GR_RIGHT_GCHATLISTEN | GR_RIGHT_GCHATSPEAK);
}

void Guild::Rename(std::string& newName)
{
    m_Name = newName;

    std::string escaped = m_Name;
    CharacterDatabase.escape_string(escaped);

    CharacterDatabase.PExecute("UPDATE guild SET name = '%s' WHERE guildid = '%u'", escaped.c_str(), m_Id);
}

GuildAddStatus Guild::AddMember(ObjectGuid plGuid, uint32 plRank)
{
    Player* pl = sObjectAccessor.FindPlayerNotInWorld(plGuid);
    if (pl)
    {
        if (pl->GetGuildId() != 0)
            return GuildAddStatus::ALREADY_IN_GUILD;
    }
    else
    {
        if (members.find(plGuid) != members.end())
            return GuildAddStatus::ALREADY_IN_GUILD;
    }

    // When joining a guild, remove this player from any petition that could have previously signed.
    if (PetitionSignature* signature = sGuildMgr.GetSignatureForPlayerGuid(plGuid))
    {
        signature->DeleteFromDB();
        signature->GetSignaturePetition()->DeleteSignature(signature);
    }

    uint32 lowguid = plGuid.GetCounter();

    // fill player data
    MemberSlot newmember;

    newmember.guid = plGuid;

    if (pl)
    {
        newmember.accountId = pl->GetSession()->GetAccountId();
        newmember.Name   = pl->GetName();
        newmember.Level  = pl->GetLevel();
        newmember.Class  = pl->GetClass();
        newmember.ZoneId = pl->GetCachedZoneId();
    }
    else
    {
        PlayerCacheData const* data = sObjectMgr.GetPlayerDataByGUID(lowguid);
        if (!data)
            return GuildAddStatus::UNKNOWN_PLAYER;

        if (data->sName.empty())
            return GuildAddStatus::UNKNOWN_PLAYER;

        newmember.Name   = data->sName;
        newmember.Level  = data->uiLevel;
        newmember.Class  = data->uiClass;
        newmember.ZoneId = data->uiZoneId;
        newmember.accountId = data->uiAccount;

        if (newmember.Level < 1 || newmember.Level > PLAYER_STRONG_MAX_LEVEL || !((1 << (newmember.Class - 1)) & CLASSMASK_ALL_PLAYABLE))
        {
            sLog.outError("%s has a broken data in field `characters` table, cannot add him to guild.", plGuid.GetString().c_str());
            return GuildAddStatus::PLAYER_DATA_ERROR;
        }
    }

    newmember.RankId  = plRank;
    if (pl)
    {
        uint8 rank = uint8(newmember.RankId);
        ScriptRegistry<GuildScript>::ForEach([&](GuildScript* script)
        {
            script->OnAddMember(this, pl, rank);
        });

        newmember.RankId = rank;
    }

    newmember.OfficerNote = (std::string)"";
    newmember.PublicNote   = (std::string)"";
    newmember.LogoutTime = time(nullptr);
    members[lowguid] = newmember;
    sGuildMgr.GuildMemberAdded(GetId(), lowguid);

    std::string dbPnote   = newmember.PublicNote;
    std::string dbOFFnote = newmember.OfficerNote;
    CharacterDatabase.escape_string(dbPnote);
    CharacterDatabase.escape_string(dbOFFnote);

    CharacterDatabase.PExecute("INSERT INTO guild_member (guildid,guid,`rank`,pnote,offnote) VALUES ('%u', '%u', '%u','%s','%s')",
                               m_Id, lowguid, newmember.RankId, dbPnote.c_str(), dbOFFnote.c_str());

    // If player not in game data in data field will be loaded from guild tables, no need to update it!!
    if (pl)
    {
        pl->SetInGuild(m_Id);
        pl->SetRank(newmember.RankId);
        pl->SetGuildIdInvited(0);
        AddToCache(lowguid);
    }

    UpdateAccountsNumber();

    return GuildAddStatus::OK;
}

void Guild::SetMOTD(std::string motd)
{
    m_motd = motd;

    // motd now can be used for encoding to DB
    CharacterDatabase.escape_string(motd);
    CharacterDatabase.PExecute("UPDATE guild SET motd='%s' WHERE guildid='%u'", motd.c_str(), m_Id);
#ifdef ENABLE_ELUNA
    ScriptRegistry<GuildScript>::ForEach([&](GuildScript* script)
    {
        script->OnMotdChanged(this, m_motd);
    });
#endif
}

void Guild::SetGINFO(std::string ginfo)
{
    m_info = ginfo;

    // ginfo now can be used for encoding to DB
    CharacterDatabase.escape_string(ginfo);
    CharacterDatabase.PExecute("UPDATE guild SET info='%s' WHERE guildid='%u'", ginfo.c_str(), m_Id);
#ifdef ENABLE_ELUNA
    ScriptRegistry<GuildScript>::ForEach([&](GuildScript* script)
    {
        script->OnInfoChanged(this, m_info);
    });
#endif
}

bool Guild::LoadGuildFromDB(QueryResult *guildDataResult)
{
    if (!guildDataResult)
        return false;

    Field *fields = guildDataResult->Fetch();

    m_Id              = fields[0].GetUInt32();
    m_Name            = fields[1].GetCppString();
    m_LeaderGuid      = ObjectGuid(HIGHGUID_PLAYER, fields[2].GetUInt32());
    m_EmblemStyle     = fields[3].GetUInt32();
    m_EmblemColor     = fields[4].GetUInt32();
    m_BorderStyle     = fields[5].GetUInt32();
    m_BorderColor     = fields[6].GetUInt32();
    m_BackgroundColor = fields[7].GetUInt32();
    m_info            = fields[8].GetCppString();
    m_motd            = fields[9].GetCppString();
    time_t time       = fields[10].GetUInt64();

    if (time > 0)
    {
        tm local       = *(localtime(&time));               // dereference and assign
        m_CreatedDay   = local.tm_mday;
        m_CreatedMonth = local.tm_mon + 1;
        m_CreatedYear  = local.tm_year + 1900;
    }

    return true;
}

bool Guild::CheckGuildStructure()
{
    // Repair the structure of guild
    // If the guildmaster doesn't exist or isn't the member of guild
    // attempt to promote another member
    int32 GM_rights = GetRank(m_LeaderGuid);
    if (GM_rights == -1)
    {
        if (DelMember(m_LeaderGuid))
            return false;                                   // guild will disbanded and deleted in caller
    }
    else if (GM_rights != GR_GUILDMASTER)
        SetLeader(m_LeaderGuid);

    // Allow only 1 guildmaster, set other to officer
    for (MemberList::iterator itr = members.begin(); itr != members.end(); ++itr)
        if (itr->second.RankId == GR_GUILDMASTER && m_LeaderGuid != itr->second.guid)
            itr->second.ChangeRank(GR_OFFICER);

    return true;
}

bool Guild::LoadRanksFromDB(QueryResult *guildRanksResult)
{
    if (!guildRanksResult)
    {
        sLog.outError("Guild %u has broken `guild_rank` data, creating new...", m_Id);
        CreateDefaultGuildRanks(0);
        return true;
    }

    Field *fields;
    bool broken_ranks = false;

    // GUILD RANKS are sequence starting from 0 = GUILD_MASTER (ALL PRIVILEGES) to max 9 (lowest privileges)
    // the lower rank id is considered higher rank - so promotion does rank-- and demotion does rank++
    // between ranks in sequence cannot be gaps - so 0,1,2,4 cannot be
    // min ranks count is 5 and max is 10.

    do
    {
        fields = guildRanksResult->Fetch();
        //condition that would be true when all ranks in QueryResult will be processed and guild without ranks is being processed
        if (!fields)
            break;

        uint32 guildId       = fields[0].GetUInt32();
        if (guildId < m_Id)
        {
            //there is in table guild_rank record which doesn't have guildid in guild table, report error
            sLog.outErrorDb("Guild %u does not exist but it has a record in guild_rank table, deleting it!", guildId);
            CharacterDatabase.PExecute("DELETE FROM guild_rank WHERE guildid = '%u'", guildId);
            continue;
        }

        if (guildId > m_Id)                                 //we loaded all ranks for this guild already, break cycle
            break;

        uint32 rankID        = fields[1].GetUInt32();
        std::string rankName = fields[2].GetCppString();
        uint32 rankRights    = fields[3].GetUInt32();

        if (rankID != m_Ranks.size())                       // guild_rank.ids are sequence 0,1,2,3..
            broken_ranks =  true;

        // first rank is guildmaster, prevent loss leader rights
        if (m_Ranks.empty())
            rankRights |= GR_RIGHT_ALL;

        AddRank(rankName, rankRights);
    }
    while (guildRanksResult->NextRow());

    if (m_Ranks.size() < GUILD_RANKS_MIN_COUNT)             // if too few ranks, renew them
    {
        m_Ranks.clear();
        sLog.outError("Guild %u has broken `guild_rank` data, creating new...", m_Id);
        CreateDefaultGuildRanks(0);                         // 0 is default locale_idx
        broken_ranks = false;
    }
    // guild_rank have wrong numbered ranks, repair
    if (broken_ranks)
    {
        sLog.outError("Guild %u has broken `guild_rank` data, repairing...", m_Id);
        CharacterDatabase.BeginTransaction();
        CharacterDatabase.PExecute("DELETE FROM guild_rank WHERE guildid='%u'", m_Id);
        for (size_t i = 0; i < m_Ranks.size(); ++i)
        {
            std::string name = m_Ranks[i].Name;
            uint32 rights = m_Ranks[i].Rights;
            CharacterDatabase.escape_string(name);
            CharacterDatabase.PExecute("INSERT INTO guild_rank (guildid,rid,rname,rights) VALUES ('%u', '%u', '%s', '%u')", m_Id, uint32(i), name.c_str(), rights);
        }
        CharacterDatabase.CommitTransaction();
    }

    return true;
}

bool Guild::LoadMembersFromDB(QueryResult *guildMembersResult)
{
    if (!guildMembersResult)
        return false;

    do
    {
        Field *fields = guildMembersResult->Fetch();
        // this condition will be true when all rows in QueryResult are processed and new guild without members is going to be loaded - prevent crash
        if (!fields)
            break;
        uint32 guildId       = fields[0].GetUInt32();
        if (guildId < m_Id)
        {
            // there is in table guild_member record which doesn't have guildid in guild table, report error
            sLog.outErrorDb("Guild %u does not exist but it has a record in guild_member table, deleting it!", guildId);
            CharacterDatabase.PExecute("DELETE FROM guild_member WHERE guildid = '%u'", guildId);
            continue;
        }

        if (guildId > m_Id)
            // we loaded all members for this guild already, break cycle
            break;

        MemberSlot newmember;
        uint32 lowguid = fields[1].GetUInt32();
        newmember.guid = ObjectGuid(HIGHGUID_PLAYER, lowguid);
        newmember.RankId = fields[2].GetUInt32();
        // don't allow member to have not existing rank!
        if (newmember.RankId >= m_Ranks.size())
            newmember.RankId = GetLowestRank();

        newmember.PublicNote                 = fields[3].GetCppString();
        newmember.OfficerNote                = fields[4].GetCppString();

        newmember.Name                  = fields[5].GetCppString();
        newmember.Level                 = fields[6].GetUInt8();
        newmember.Class                 = fields[7].GetUInt8();
        newmember.ZoneId                = fields[8].GetUInt32();
        newmember.LogoutTime            = fields[9].GetUInt64();
        newmember.accountId             = fields[10].GetInt32();

        // this code will remove not existing character guids from guild
        if (newmember.Level < 1 || newmember.Level > PLAYER_STRONG_MAX_LEVEL) // can be at broken `data` field
        {
            sLog.outError("%s has a broken data in field `characters`.`data`, deleting him from guild!", newmember.guid.GetString().c_str());
            CharacterDatabase.PExecute("DELETE FROM guild_member WHERE guid = '%u'", lowguid);
            continue;
        }
        if (!newmember.ZoneId)
        {
            sLog.outError("%s has broken zone-data", newmember.guid.GetString().c_str());
            // here it will also try the same, to get the zone from characters-table, but additional it tries to find
            // the zone through xy coords .. this is a bit redundant, but shouldn't be called often
            newmember.ZoneId = Player::GetZoneIdFromDB(newmember.guid);
        }
        if (!((1 << (newmember.Class - 1)) & CLASSMASK_ALL_PLAYABLE)) // can be at broken `class` field
        {
            sLog.outError("%s has a broken data in field `characters`.`class`, deleting him from guild!", newmember.guid.GetString().c_str());
            CharacterDatabase.PExecute("DELETE FROM guild_member WHERE guid = '%u'", lowguid);
            continue;
        }
        if (newmember.Name.empty()) // no deleted characters
        {
            sLog.outError("%s has no name, deleting him from guild!", newmember.guid.GetString().c_str());
            CharacterDatabase.PExecute("DELETE FROM guild_member WHERE guid = '%u'", lowguid);
            continue;
        }

        members[lowguid]      = newmember;
        sGuildMgr.GuildMemberAdded(GetId(), lowguid);

    }
    while (guildMembersResult->NextRow());

    if (members.empty())
        return false;

    UpdateAccountsNumber();

    return true;
}

void Guild::SetLeader(ObjectGuid guid)
{
    MemberSlot* slot = GetMemberSlot(guid);
    if (!slot)
        return;

    SetLeader(slot);
}

void Guild::SetLeader(MemberSlot* slot)
{
    m_LeaderGuid = slot->guid;
    slot->ChangeRank(GR_GUILDMASTER);

    CharacterDatabase.PExecute("UPDATE guild SET leaderguid='%u' WHERE guildid='%u'", slot->guid.GetCounter(), m_Id);
}

/**
 * Remove character from guild
 *
 * @param guid          Character that removed from guild
 * @param isDisbanding  Flag set if function called from Guild::Disband, so not need update DB in per-member mode only or leader update
 *
 * @return true, if guild need to be disband and erase (no members or can't setup leader)
 */
bool Guild::DelMember(ObjectGuid guid, bool isDisbanding)
{
    uint32 lowguid = guid.GetCounter();

    // guild master can be deleted when loading guild and guid doesn't exist in characters table
    // or when he is removed from guild by gm command
    if (m_LeaderGuid == guid && !isDisbanding)
    {
        MemberSlot* oldLeader = nullptr;
        MemberSlot* best = nullptr;
        if (!GetSuitableNewLeader(best, oldLeader))
            return true;

        SetNewLeader(best, oldLeader);

        // when leader non-exist (at guild load with deleted leader only) not send broadcasts
        if (oldLeader)
            BroadcastEvent(GE_LEFT, guid, oldLeader->Name.c_str());
    }

    members.erase(lowguid);
    RemoveFromCache(lowguid);
    sGuildMgr.GuildMemberRemoved(lowguid);

    Player *player = sObjectMgr.GetPlayer(guid);
    if (player)
    {
        ScriptRegistry<GuildScript>::ForEach([&](GuildScript* script)
        {
            script->OnRemoveMember(this, player, isDisbanding, false);
        });
    }

    // If player not online data in data field will be loaded from guild tabs no need to update it !!
    if (player)
    {
        player->SetInGuild(0);
        player->SetRank(0);
    }

    CharacterDatabase.PExecute("DELETE FROM guild_member WHERE guid = '%u'", lowguid);

    if (!isDisbanding)
        UpdateAccountsNumber();

    return members.empty();
}

void Guild::SetNewLeader(ObjectGuid newLeaderGuid)
{
    MemberSlot* newLeaderSlot = GetMemberSlot(newLeaderGuid);
    MemberSlot* oldLeaderSlot = GetMemberSlot(GetLeaderGuid());

    if (!newLeaderSlot)
    {
        sLog.outError("Guild::SetNewLeader - New leader slot not found!");
        return;
    }

    if (!oldLeaderSlot)
    {
        sLog.outError("Guild::SetNewLeader - Old leader slot not found!");
        return;
    }

    if (newLeaderSlot == oldLeaderSlot)
    {
        sLog.outError("Guild::SetNewLeader - Attempt to change leader to same player!");
        return;
    }

    SetNewLeader(newLeaderSlot, oldLeaderSlot);
}

void Guild::SetNewLeader(MemberSlot* newLeaderSlot, MemberSlot* oldLeaderSlot)
{
    SetLeader(newLeaderSlot);

    // when leader non-exist (at guild load with deleted leader only) not send broadcasts
    if (oldLeaderSlot)
    {
        oldLeaderSlot->ChangeRank(GR_OFFICER);
        BroadcastEvent(GE_LEADER_CHANGED, oldLeaderSlot->Name.c_str(), newLeaderSlot->Name.c_str());
    }
}

bool Guild::GetSuitableNewLeader(MemberSlot*& newLeaderSlot, MemberSlot*& oldLeaderSlot, bool preferOldestOfficer)
{
    newLeaderSlot = nullptr;
    oldLeaderSlot = nullptr;

    uint32 lowGuid = m_LeaderGuid.GetCounter();

    for (Guild::MemberList::iterator i = members.begin(); i != members.end(); ++i)
    {
        if (i->first == lowGuid)
        {
            oldLeaderSlot = &(i->second);
            continue;
        }

        if (!newLeaderSlot || newLeaderSlot->RankId > i->second.RankId)
            newLeaderSlot = &(i->second);
    }

    if (preferOldestOfficer)
    {
        for (GuildEventLogEntry const& event : m_GuildEventLog)
        {
            if (event.EventType != GUILD_EVENT_LOG_INVITE_PLAYER || event.PlayerGuid2 == lowGuid)
                continue;

            MemberSlot* slot = GetMemberSlot(ObjectGuid(HIGHGUID_PLAYER, event.PlayerGuid2));
            if (slot && slot->RankId == GR_OFFICER)
            {
                newLeaderSlot = slot;
                break;
            }
        }
    }

    return newLeaderSlot != nullptr;
}

void Guild::BroadcastToGuild(WorldSession *session, std::string const& msg, uint32 language)
{
    if (!session || session->IsFingerprintBanned())
        return;

    MasterPlayer* pPlayer = session->GetMasterPlayer();
    BroadcastToGuild(pPlayer, msg, language);
}

void Guild::BroadcastToGuild(MasterPlayer* pPlayer, std::string const& msg, uint32 language)
{
    if (!pPlayer || !HasRankRight(pPlayer->GetRank(), GR_RIGHT_GCHATSPEAK))
        return;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_GUILD, msg.c_str(), Language(language), pPlayer->GetChatTag(), pPlayer->GetObjectGuid(), pPlayer->GetName());

    if (IsMemberCacheEnabled())
    {
        for (auto const& guidLow : m_onlineMemberCache)
        {
            auto pl = ObjectAccessor::FindMasterPlayer(ObjectGuid(HIGHGUID_PLAYER, guidLow));
            if (!pl)
                continue;

            auto memberItr = members.find(guidLow);
            if (memberItr == members.end())
                continue;

            if (!HasRankRight(memberItr->second.RankId, GR_RIGHT_GCHATLISTEN))
                continue;
            
            if (pl && pl->GetSession() && !pl->GetSession()->PlayerLogout() &&
                pl->GetSocial() && !pl->GetSocial()->HasIgnore(pPlayer->GetObjectGuid()))
                pl->GetSession()->SendPacket(&data);
        }
    }
    else
    {
        for (MemberList::const_iterator itr = members.begin(); itr != members.end(); ++itr)
        {
            if (!HasRankRight(itr->second.RankId, GR_RIGHT_GCHATLISTEN))
                continue;

            MasterPlayer* pl = ObjectAccessor::FindMasterPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first));

            if (pl && pl->GetSession() && !pl->GetSession()->PlayerLogout() &&
                pl->GetSocial() && !pl->GetSocial()->HasIgnore(pPlayer->GetObjectGuid()))
                pl->GetSession()->SendPacket(&data);
        }
    }

    for (const auto& gmGuid : m_GmListeners)
    {
        if (Player* gm = sObjectAccessor.FindPlayer(gmGuid))
        {
            gm->GetSession()->SendPacket(&data);
        }
    }
}

void Guild::BroadcastToOfficers(WorldSession *session, std::string const& msg, uint32 language)
{
    if (!session)
        return;

    MasterPlayer* pPlayer = session->GetMasterPlayer();
    if (!pPlayer || !HasRankRight(pPlayer->GetRank(), GR_RIGHT_OFFCHATSPEAK))
        return;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_OFFICER, msg.c_str(), Language(language), pPlayer->GetChatTag(), pPlayer->GetObjectGuid(), pPlayer->GetName());

    for (MemberList::const_iterator itr = members.begin(); itr != members.end(); ++itr)
    {
        if (!HasRankRight(itr->second.RankId, GR_RIGHT_OFFCHATLISTEN))
            continue;

        MasterPlayer *pl = ObjectAccessor::FindMasterPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first));

        if (pl && pl->GetSession() && !pl->GetSocial()->HasIgnore(session->GetMasterPlayer()->GetObjectGuid()))
            pl->GetSession()->SendPacket(&data);
    }

    for (const auto& gmGuid : m_GmListeners)
    {
        if (Player* gm = sObjectAccessor.FindPlayer(gmGuid))
        {
            gm->GetSession()->SendPacket(&data);
        }
    }
}

void Guild::BroadcastPacket(WorldPacket *packet)
{
    if (IsMemberCacheEnabled())
    {
        for (auto const& guidLow : m_onlineMemberCache)
        {
            auto pl = ObjectAccessor::FindMasterPlayer(ObjectGuid(HIGHGUID_PLAYER, guidLow));
            if (!pl)
                continue;

            pl->GetSession()->SendPacket(packet);
        }
    }
    else
    {
        for (MemberList::const_iterator itr = members.begin(); itr != members.end(); ++itr)
        {
            Player* player = ObjectAccessor::FindPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first));
            if (player)
                player->GetSession()->SendPacket(packet);
        }
    }

    for (const auto& gmGuid : m_GmListeners)
    {
        if (Player* gm = sObjectAccessor.FindPlayer(gmGuid))
        {
            gm->GetSession()->SendPacket(packet);
        }
    }
}

void Guild::BroadcastPacketToRank(WorldPacket *packet, uint32 rankId)
{
    for (MemberList::const_iterator itr = members.begin(); itr != members.end(); ++itr)
    {
        if (itr->second.RankId == rankId)
        {
            Player *player = ObjectAccessor::FindPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first));
            if (player)
                player->GetSession()->SendPacket(packet);
        }
    }
}

void Guild::CreateRank(std::string name_, uint32 rights)
{
    if (m_Ranks.size() >= GUILD_RANKS_MAX_COUNT)
        return;

    // ranks are sequence 0,1,2,... where 0 means guildmaster
    uint32 new_rank_id = m_Ranks.size();

    AddRank(name_, rights);

    // name now can be used for encoding to DB
    CharacterDatabase.escape_string(name_);
    CharacterDatabase.PExecute("INSERT INTO guild_rank (guildid,rid,rname,rights) VALUES ('%u', '%u', '%s', '%u')", m_Id, new_rank_id, name_.c_str(), rights);
}

void Guild::AddRank(std::string const& name_, uint32 rights)
{
    m_Ranks.emplace_back(RankInfo(name_, rights));
}

void Guild::DelRank()
{
    // client won't allow to have less than GUILD_RANKS_MIN_COUNT ranks in guild
    if (m_Ranks.size() <= GUILD_RANKS_MIN_COUNT)
        return;

    // delete lowest guild_rank
    uint32 rank = GetLowestRank();
    CharacterDatabase.PExecute("DELETE FROM guild_rank WHERE rid>='%u' AND guildid='%u'", rank, m_Id);

	_Bank->UpdateMinranks(rank);
    _InfernoBank->UpdateMinranks(rank);

    m_Ranks.pop_back();
}

std::string Guild::GetRankName(uint32 rankId)
{
    if (rankId >= m_Ranks.size())
        return "<unknown>";

    return m_Ranks[rankId].Name;
}

uint32 Guild::GetRankRights(uint32 rankId)
{
    if (rankId >= m_Ranks.size())
        return 0;

    return m_Ranks[rankId].Rights;
}

void Guild::SetRankName(uint32 rankId, std::string name_)
{
    if (rankId >= m_Ranks.size())
        return;

    m_Ranks[rankId].Name = name_;

    // name now can be used for encoding to DB
    CharacterDatabase.escape_string(name_);
    CharacterDatabase.PExecute("UPDATE guild_rank SET rname='%s' WHERE rid='%u' AND guildid='%u'", name_.c_str(), rankId, m_Id);
}

void Guild::SetRankRights(uint32 rankId, uint32 rights)
{
    if (rankId >= m_Ranks.size())
        return;

    m_Ranks[rankId].Rights = rights;

    CharacterDatabase.PExecute("UPDATE guild_rank SET rights='%u' WHERE rid='%u' AND guildid='%u'", rights, rankId, m_Id);
}

/**
 * Disband guild including cleanup structures and DB
 *
 * Note: guild object need deleted after this in caller code.
 */
void Guild::Disband()
{
    ScriptRegistry<GuildScript>::ForEach([&](GuildScript* script)
    {
        script->OnDisband(this);
    });

    BroadcastEvent(GE_DISBANDED);

    while (!members.empty())
    {
        MemberList::const_iterator itr = members.begin();
        DelMember(ObjectGuid(HIGHGUID_PLAYER, itr->first), true);
    }

    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("DELETE FROM guild WHERE guildid = '%u'", m_Id);
    CharacterDatabase.PExecute("DELETE FROM guild_rank WHERE guildid = '%u'", m_Id);
    CharacterDatabase.PExecute("DELETE FROM guild_eventlog WHERE guildid = '%u'", m_Id);

    CharacterDatabase.PExecute("DELETE FROM guild_bank WHERE guildid = '%u'", m_Id);
    CharacterDatabase.PExecute("DELETE FROM guild_bank_log WHERE guildid = '%u'", m_Id);
    CharacterDatabase.PExecute("DELETE FROM guild_bank_tabs WHERE guildid = '%u'", m_Id);
    CharacterDatabase.PExecute("DELETE FROM guild_bank_money WHERE guildid = '%u'", m_Id);
    CharacterDatabase.CommitTransaction();

    if (_Bank)
    {
        _Bank->DeleteFromDB();
        delete _Bank;
    }

    if (_InfernoBank)
    {
        _InfernoBank->DeleteFromDB();
        delete _InfernoBank;
    }

    sGuildMgr.RemoveGuild(m_Id);
}

WorldPacket Guild::BuildOnlineRosterPacket(bool sendOfficerNote)
{
    struct TempMemberInfo
    {
        ObjectGuid Guid;
        Player* Member = nullptr;
        MemberSlot* Slot = nullptr;
    };

    size_t onlineMembers = 0;
    std::vector<TempMemberInfo> onlineMemberCache;

    uint32 totalSize = 0;
    totalSize += sizeof(uint32); // count
    totalSize += m_motd.length() + 1 + m_info.length() + 1;
    totalSize += sizeof(uint32); // m_ranks.size()
    totalSize += sizeof(uint32) * m_Ranks.size(); // all ranks


    for (auto itr = members.begin(); itr != members.end(); ++itr)
    {
        TempMemberInfo info;
        info.Guid = ObjectGuid(HIGHGUID_PLAYER, itr->first);
        info.Member = ObjectAccessor::FindPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first));
        info.Slot = &itr->second;

        if (info.Member && !info.Member->HasGMDisabledSocials())
        {
            totalSize += GUILD_MEMBER_BLOCK_SIZE_WITHOUT_NOTE;
            totalSize += info.Slot->PublicNote.length();
            if (sendOfficerNote)
                totalSize += info.Slot->OfficerNote.length();

            onlineMemberCache.emplace_back(info);
            ++onlineMembers;
        }
    }

    const bool inPacketCap = totalSize < MAX_UNCOMPRESSED_PACKET_SIZE;

    auto writeMemberData = [inPacketCap, sendOfficerNote](WorldPacket& data, TempMemberInfo const& member) -> bool
    {
        if (!inPacketCap)
        {
            // if the packet is expected to be bigger than cap we filter otherwise we might send too much.
            if (data.size() + GUILD_MEMBER_BLOCK_SIZE >= MAX_UNCOMPRESSED_PACKET_SIZE)
                return false;
        }

        data << member.Guid;
        data << uint8(1); // only online
        data << member.Member->GetName();
        data << uint32(member.Slot->RankId);
        data << uint8(member.Member->GetLevel());
        data << uint8(member.Member->GetClass());
        data << uint32(member.Member->GetCachedZoneId());
        data << member.Slot->PublicNote;
        data << (sendOfficerNote ? member.Slot->OfficerNote : "");
        return true;
    };

    // we can only guess size
    WorldPacket data(SMSG_GUILD_ROSTER, (4 + m_motd.length() + 1 + m_info.length() + 1 + 4 + m_Ranks.size() * 4 + onlineMembers * GUILD_MEMBER_BLOCK_SIZE_WITHOUT_NOTE));

    uint32 countPos = data.wpos();

    data << uint32(onlineMembers);
    data << m_motd;
    data << m_info;

    data << uint32(m_Ranks.size());
    for (RankList::const_iterator ritr = m_Ranks.begin(); ritr != m_Ranks.end(); ++ritr)
        data << uint32(ritr->Rights);

    //sort the members from highest to lowest rank if over limit.
    if (!inPacketCap)
    {
        std::sort(onlineMemberCache.begin(), onlineMemberCache.end(), [](const TempMemberInfo& a, const TempMemberInfo& b)
            {
                return a.Slot->RankId < b.Slot->RankId; // lowest ranks first, lowest rank ids -> highest actual rank
            });
    }

    uint32 finalCount = 0;

    for (const auto& member : onlineMemberCache)
    {
        if (!member.Member)
            continue;

        if (writeMemberData(data, member))
            ++finalCount;
        else
            break;
    }

    data.put<uint32>(countPos, finalCount);
    return data;
}

void Guild::TempRosterOnline(WorldSession* session /*= nullptr*/)
{
    //This is for public guilds that are huge.
    //We will send cached results and only update on world ticks every x ms.
    const bool sendOfficerNote = session && session->GetPlayer() ? HasRankRight(session->GetPlayer()->GetRank(), GR_RIGHT_VIEWOFFNOTE) : false;
    const bool canSendCache = (sendOfficerNote && m_cachedOfficerRosterPacket) || (!sendOfficerNote && m_cachedRosterPacket);

    if (IsRosterCacheEnabled() && canSendCache)
    {
        if (session)
            session->SendPacket(sendOfficerNote ? m_cachedOfficerRosterPacket.get() :  m_cachedRosterPacket.get());
        else
            BroadcastPacket(sendOfficerNote ? m_cachedOfficerRosterPacket.get() : m_cachedRosterPacket.get());
        return;
    }

    auto data = BuildOnlineRosterPacket(sendOfficerNote);

    if (session)
        session->SendPacket(&data);
    else
        BroadcastPacket(&data);

    DEBUG_LOG("WORLD: Sent (SMSG_GUILD_ROSTER) (ONLY FOR ONLINE)");
}

void Guild::UpdateCaches(uint32 diff)
{
    if (!IsMemberCacheEnabled())
        return;

    //do the timer check in Guild instead of Mgr to allow for dynamic cache expiry in future per guild.

    if (m_cacheTimer < diff)
    {
        //refresh cache
        auto nonOfficerData = BuildOnlineRosterPacket(false);
        auto officerData = BuildOnlineRosterPacket(true);

        m_cachedOfficerRosterPacket = std::make_unique<WorldPacket>(std::move(officerData));
        m_cachedRosterPacket = std::make_unique<WorldPacket>(std::move(nonOfficerData));
        m_cacheTimer = CacheExpiryMs;
    }
    else
        m_cacheTimer -= diff;
}

void Guild::Roster(WorldSession *session /*= nullptr*/)
{

    struct TempMemberInfo
    {
        ObjectGuid Guid;
        Player* Member = nullptr;
        MemberSlot* Slot = nullptr;
    };

    size_t onlineMembers = 0;
    size_t offlineMembers = 0;
    std::vector<TempMemberInfo> onlineMemberCache;
    std::vector<TempMemberInfo> offlineMemberCache;
    onlineMemberCache.reserve(members.size() / 2);
    offlineMemberCache.reserve(members.size() / 2);

    uint32 totalSize = 0;
    totalSize += sizeof(uint32); // count
    totalSize += m_motd.length() + 1 + m_info.length() + 1;
    totalSize += sizeof(uint32); // m_ranks.size()
    totalSize += sizeof(uint32) * m_Ranks.size(); // all ranks

    for (auto itr = members.begin(); itr != members.end(); ++itr)
    {
        TempMemberInfo info;
        info.Guid = ObjectGuid(HIGHGUID_PLAYER, itr->first);
        info.Member = ObjectAccessor::FindPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first));
        info.Slot = &itr->second;

        if (info.Member && !info.Member->HasGMDisabledSocials())
        {
            onlineMemberCache.emplace_back(info);
            ++onlineMembers;
        }
        else
        {
            offlineMemberCache.emplace_back(info);
            ++offlineMembers;
        }


        totalSize += GUILD_MEMBER_BLOCK_SIZE_WITHOUT_NOTE;
        totalSize += info.Slot->PublicNote.length() + 1 + info.Slot->OfficerNote.length() + 1;
    }

    size_t count = onlineMembers + offlineMembers;

    count = std::min(count, size_t(GUILD_MAX_MEMBERS));
    onlineMembers = std::min(onlineMembers, size_t(GUILD_MAX_MEMBERS));
    offlineMembers = std::min(offlineMembers, size_t(GUILD_MAX_MEMBERS) - onlineMembers);

    const bool inPacketCap = totalSize < MAX_UNCOMPRESSED_PACKET_SIZE;
    auto sendOfficerNote = session && session->GetPlayer() ? HasRankRight(session->GetPlayer()->GetRank(), GR_RIGHT_VIEWOFFNOTE) : false;

    if (!inPacketCap)
    {
        // figure out how many we can send before having to apply pagination (soon tm).
        // guild with notes is too big to show in one packet. We could for later add some UI to paginate the member list.
        // For now just prefer online members, then ranks, then the rest.
        // this if block is intentionally left empty.
    }

    auto writeMemberData = [inPacketCap, sendOfficerNote](WorldPacket& data, TempMemberInfo const& member) -> bool
    {
        if (!inPacketCap)
        {
            // if the packet is expected to be bigger than cap we filter otherwise we might send too much.
            if (data.size() + GUILD_MEMBER_BLOCK_SIZE >= MAX_UNCOMPRESSED_PACKET_SIZE)
                return false;
        }

        bool online = member.Member != nullptr && !member.Member->HasGMDisabledSocials();

        data << member.Guid;
        data << uint8((online)); // online / offline state, send 0 for GMS without socials enabled too
        if (member.Member)
        {
            data << member.Member->GetName();
            data << uint32(member.Slot->RankId);
            data << uint8(member.Member->GetLevel());
            data << uint8(member.Member->GetClass());
            data << uint32(member.Member->GetCachedZoneId());
        }
        else
        {
            data << member.Slot->Name;
            data << uint32(member.Slot->RankId);
            data << uint8(member.Slot->Level);
            data << uint8(member.Slot->Class);
            data << uint32(member.Slot->ZoneId);
            data << float(float(time(nullptr) - member.Slot->LogoutTime) / DAY);
        }
        data << member.Slot->PublicNote;
        data << (sendOfficerNote ? member.Slot->OfficerNote : "");
        return true;
    };

    // we can only guess size
    WorldPacket data(SMSG_GUILD_ROSTER, (4 + m_motd.length() + 1 + m_info.length() + 1 + 4 + m_Ranks.size() * 4 + count * GUILD_MEMBER_BLOCK_SIZE_WITHOUT_NOTE));

    uint32 countPos = data.wpos();

    data << uint32(count);
    data << m_motd;
    data << m_info;

    data << uint32(m_Ranks.size());
    for (RankList::const_iterator ritr = m_Ranks.begin(); ritr != m_Ranks.end(); ++ritr)
        data << uint32(ritr->Rights);


    //sort the members from highest to lowest rank if over limit.
    if (!inPacketCap)
    {
        std::sort(onlineMemberCache.begin(), onlineMemberCache.end(), [](const TempMemberInfo& a, const TempMemberInfo& b)
        {
            return a.Slot->RankId < b.Slot->RankId; // lowest ranks first, lowest rank ids -> highest actual rank
        });

        std::sort(offlineMemberCache.begin(), offlineMemberCache.end(), [](const TempMemberInfo& a, const TempMemberInfo& b)
        {
            return a.Slot->RankId < b.Slot->RankId; // lowest ranks first, lowest rank ids -> highest actual rank
        });
    }

    //cull caches if they're too big.
    if (onlineMembers < onlineMemberCache.size())
        onlineMemberCache.resize(onlineMemberCache.size() - (onlineMemberCache.size() - onlineMembers));

    if (offlineMembers < offlineMemberCache.size())
        offlineMemberCache.resize(offlineMemberCache.size() - (offlineMemberCache.size() - offlineMembers));

    uint32 finalCount = 0;

    for (const auto& member : onlineMemberCache)
    {
        if (!member.Member)
            continue;

        if (writeMemberData(data, member))
            ++finalCount;
        else
            break;
    }

    for (const auto& member : offlineMemberCache)
    {
        if (member.Member && !member.Member->HasGMDisabledSocials())
            continue;

        // skip deleted characters with no name
        // they should be removed from guild but somehow it happens on live
        if (member.Slot->Name.empty())
            continue;

        if (writeMemberData(data, member))
            ++finalCount;
        else
            break;
    }

    data.put<uint32>(countPos, finalCount);

    if (session)
        session->SendPacket(&data);
    else
        BroadcastPacket(&data);
    DEBUG_LOG("WORLD: Sent (SMSG_GUILD_ROSTER)");
}

void Guild::Query(WorldSession *session)
{
    WorldPacket data(SMSG_GUILD_QUERY_RESPONSE, (4 + 48 + 10 * 32 + 5 * 4)); // guess size; max: name(96), rankname(64)

    data << uint32(m_Id);
    data << m_Name;

    for (size_t i = 0 ; i < GUILD_RANKS_MAX_COUNT; ++i)     // show always 10 ranks
    {
        if (i < m_Ranks.size())
            data << m_Ranks[i].Name;
        else
            data << uint8(0);                               // nullptr string
    }

    data << uint32(m_EmblemStyle);
    data << uint32(m_EmblemColor);
    data << uint32(m_BorderStyle);
    data << uint32(m_BorderColor);
    data << uint32(m_BackgroundColor);

    session->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent (SMSG_GUILD_QUERY_RESPONSE)");
}

void Guild::SetEmblem(uint32 emblemStyle, uint32 emblemColor, uint32 borderStyle, uint32 borderColor, uint32 backgroundColor)
{
    m_EmblemStyle = emblemStyle;
    m_EmblemColor = emblemColor;
    m_BorderStyle = borderStyle;
    m_BorderColor = borderColor;
    m_BackgroundColor = backgroundColor;

    CharacterDatabase.PExecute("UPDATE guild SET EmblemStyle=%u, EmblemColor=%u, BorderStyle=%u, BorderColor=%u, BackgroundColor=%u WHERE guildid = %u", m_EmblemStyle, m_EmblemColor, m_BorderStyle, m_BorderColor, m_BackgroundColor, m_Id);
}

bool Guild::AddGMListener(Player* gm)
{
    if (m_GmListeners.find(gm->GetObjectGuid()) == m_GmListeners.end())
    {
        m_GmListeners.insert(gm->GetObjectGuid());
        return true;
    }
    return false;
}

/**
 * Return the number of accounts that are in the guild after possible update if required
 * A player may have many characters in the guild, but with the same account
 */
uint32 Guild::GetAccountsNumber()
{
    // not need recalculation
    if (m_accountsNumber)
        return m_accountsNumber;

    //We use a set to be sure each element will be unique
    std::set<uint32> accountsIdSet;
    for (MemberList::const_iterator itr = members.begin(); itr != members.end(); ++itr)
        accountsIdSet.insert(itr->second.accountId);

    m_accountsNumber = accountsIdSet.size();

    return m_accountsNumber;
}

// *************************************************
// Guild Eventlog part
// *************************************************
// Display guild eventlog
void Guild::DisplayGuildEventLog(WorldSession *session)
{
    // NOSTALRIUS: Inexistant packet.
}

// Load guild eventlog from DB
void Guild::LoadGuildEventLogFromDB()
{
    //                                                     0        1          2            3            4        5
    QueryResult *result = CharacterDatabase.PQuery("SELECT LogGuid, EventType, PlayerGuid1, PlayerGuid2, NewRank, TimeStamp FROM guild_eventlog WHERE guildid=%u ORDER BY TimeStamp DESC,LogGuid DESC LIMIT %u", m_Id, GUILD_EVENTLOG_MAX_RECORDS);
    if (!result)
        return;
    bool isNextLogGuidSet = false;
    //uint32 configCount = sWorld.getConfig(CONFIG_UINT32_GUILD_EVENT_LOG_COUNT);
    // First event in list will be the oldest and the latest event is last event in list
    do
    {
        Field *fields = result->Fetch();
        if (!isNextLogGuidSet)
        {
            m_GuildEventLogNextGuid = fields[0].GetUInt32();
            isNextLogGuidSet = true;
        }
        // Fill entry
        GuildEventLogEntry NewEvent;
        NewEvent.EventType = fields[1].GetUInt8();
        NewEvent.PlayerGuid1 = fields[2].GetUInt32();
        NewEvent.PlayerGuid2 = fields[3].GetUInt32();
        NewEvent.NewRank = fields[4].GetUInt8();
        NewEvent.TimeStamp = fields[5].GetUInt64();

        // There can be a problem if more events have same TimeStamp the ORDER can be broken when fields[0].GetUInt32() == configCount, but
        // events with same timestamp can appear when there is lag, and we naively suppose that mangos isn't laggy
        // but if problem appears, player will see set of guild events that have same timestamp in bad order

        // Add entry to list
        m_GuildEventLog.push_front(NewEvent);

    }
    while (result->NextRow());
    delete result;
}

// Add entry to guild eventlog
void Guild::LogGuildEvent(uint8 EventType, ObjectGuid playerGuid1, ObjectGuid playerGuid2, uint8 newRank)
{
    GuildEventLogEntry NewEvent;
    // Create event
    NewEvent.EventType = EventType;
    NewEvent.PlayerGuid1 = playerGuid1.GetCounter();
    NewEvent.PlayerGuid2 = playerGuid2.GetCounter();
    NewEvent.NewRank = newRank;
    NewEvent.TimeStamp = uint32(time(nullptr));
    // Count new LogGuid
    m_GuildEventLogNextGuid = (m_GuildEventLogNextGuid + 1) % sWorld.getConfig(CONFIG_UINT32_GUILD_EVENT_LOG_COUNT);
    // Check max records limit
    if (m_GuildEventLog.size() >= GUILD_EVENTLOG_MAX_RECORDS)
        m_GuildEventLog.pop_front();
    // Add event to list
    m_GuildEventLog.push_back(NewEvent);
    // Save event to DB
    CharacterDatabase.PExecute("DELETE FROM guild_eventlog WHERE guildid='%u' AND LogGuid='%u'", m_Id, m_GuildEventLogNextGuid);
    CharacterDatabase.PExecute("INSERT INTO guild_eventlog (guildid, LogGuid, EventType, PlayerGuid1, PlayerGuid2, NewRank, TimeStamp) VALUES ('%u','%u','%u','%u','%u','%u','" UI64FMTD "')",
                               m_Id, m_GuildEventLogNextGuid, uint32(NewEvent.EventType), NewEvent.PlayerGuid1, NewEvent.PlayerGuid2, uint32(NewEvent.NewRank), NewEvent.TimeStamp);
}

ObjectGuid Guild::GetGuildInviter(ObjectGuid playerGuid) const
{
    for (auto const& itr : m_GuildEventLog)
    {
        if (itr.EventType == GUILD_EVENT_LOG_INVITE_PLAYER &&
            itr.PlayerGuid2 == playerGuid)
            return itr.PlayerGuid1;
    }
    return ObjectGuid();
}

void Guild::BroadcastEvent(GuildEvents event, ObjectGuid guid, char const* str1 /*=nullptr*/, char const* str2 /*=nullptr*/, char const* str3 /*=nullptr*/)
{
    uint8 strCount = !str1 ? 0 : (!str2 ? 1 : (!str3 ? 2 : 3));

    WorldPacket data(SMSG_GUILD_EVENT, 1 + 1 + 1 * strCount + (guid.IsEmpty() ? 0 : 8));
    data << uint8(event);
    data << uint8(strCount);

    if (str3)
    {
        data << str1;
        data << str2;
        data << str3;
    }
    else if (str2)
    {
        data << str1;
        data << str2;
    }
    else if (str1)
        data << str1;

    if (!guid.IsEmpty())
        data << guid;

    BroadcastPacket(&data);

    DEBUG_LOG("WORLD: Sent SMSG_GUILD_EVENT");
}
