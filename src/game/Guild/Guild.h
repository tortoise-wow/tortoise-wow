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

#ifndef MANGOSSERVER_GUILD_H
#define MANGOSSERVER_GUILD_H

#include "Common.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"
#include "ObjectMgr.h"
#include "GuildBank/GuildBank.h"

class Item;
class Petition;

#define GUILD_RANKS_MIN_COUNT   5
#define GUILD_RANKS_MAX_COUNT   10
#ifdef ENABLE_ELUNA
#define GUILD_RANK_NONE         0xFF
#endif

enum
{
    GUILD_NOTE_MAX_LENGTH = 31,
    GUILD_INFO_MAX_LENGTH = 500,
    GUILD_MOTD_MAX_LENGTH = 128,
    GUILD_MEMBER_BLOCK_SIZE = (8 + 1 + (1 + MAX_INTERNAL_PLAYER_NAME) + 4 + 1 + 1 + 4 + (1 + GUILD_NOTE_MAX_LENGTH) + (1 + GUILD_NOTE_MAX_LENGTH)),
    GUILD_MEMBER_BLOCK_SIZE_WITHOUT_NOTE = (8 + 1 + (1 + MAX_INTERNAL_PLAYER_NAME) + 4 + 1 + 1 + 4 + (1 + 0) + (1 + 0)),
    // 0x8000 is client max accepted packet size.
    // Set a limit so that SMSG_GUILD_ROSTER size is never over this size.
    GUILD_MAX_MEMBERS_WITH_NOTE = (0x8000 - 10 - GUILD_RANKS_MAX_COUNT - GUILD_INFO_MAX_LENGTH - GUILD_MOTD_MAX_LENGTH) / GUILD_MEMBER_BLOCK_SIZE,
    GUILD_MAX_MEMBERS = (0x8000 - 10 - GUILD_RANKS_MAX_COUNT - GUILD_INFO_MAX_LENGTH - GUILD_MOTD_MAX_LENGTH) / GUILD_MEMBER_BLOCK_SIZE_WITHOUT_NOTE,
};

enum GuildDefaultRanks
{
    //these ranks can be modified, but they cannot be deleted
    GR_GUILDMASTER  = 0,
    GR_OFFICER      = 1,
    GR_VETERAN      = 2,
    GR_MEMBER       = 3,
    GR_INITIATE     = 4,
    //When promoting member server does: rank--;!
    //When demoting member server does: rank++;!
};

enum GuildRankRights
{
    GR_RIGHT_EMPTY              = 0x00000040,
    GR_RIGHT_GCHATLISTEN        = 0x00000041,
    GR_RIGHT_GCHATSPEAK         = 0x00000042,
    GR_RIGHT_OFFCHATLISTEN      = 0x00000044,
    GR_RIGHT_OFFCHATSPEAK       = 0x00000048,
    GR_RIGHT_PROMOTE            = 0x000000C0,
    GR_RIGHT_DEMOTE             = 0x00000140,
    GR_RIGHT_INVITE             = 0x00000050,
    GR_RIGHT_REMOVE             = 0x00000060,
    GR_RIGHT_SETMOTD            = 0x00001040,
    GR_RIGHT_EPNOTE             = 0x00002040,
    GR_RIGHT_VIEWOFFNOTE        = 0x00004040,
    GR_RIGHT_EOFFNOTE           = 0x00008040,
    // [-ZERO] tbc enumerations [?]
    GR_RIGHT_MODIFY_GUILD_INFO  = 0x00010040,
    GR_RIGHT_ALL                = 0x000FF1FF
};

enum Typecommand
{
    GUILD_CREATE_S  = 0x00,
    GUILD_INVITE_S  = 0x01,
    GUILD_QUIT_S    = 0x03,
    GUILD_FOUNDER_S = 0x0E,
    GUILD_UNK19     = 0x13,
    GUILD_UNK20     = 0x14
    // [ZERO] in SMSG_GUILD_COMMAND_RESULT: 2,4-13,15-18 no effect for no error
};

enum class GuildAddStatus
{
    OK,
    GUILD_FULL,
    ALREADY_IN_GUILD,
    UNKNOWN_PLAYER,
    PLAYER_DATA_ERROR
};

enum CommandErrors
{
    ERR_PLAYER_NO_MORE_IN_GUILD     = 0x00, // no message/error
    ERR_GUILD_INTERNAL              = 0x01,
    ERR_ALREADY_IN_GUILD            = 0x02,
    ERR_ALREADY_IN_GUILD_S          = 0x03,
    ERR_INVITED_TO_GUILD            = 0x04,
    ERR_ALREADY_INVITED_TO_GUILD_S  = 0x05,
    ERR_GUILD_NAME_INVALID          = 0x06,
    ERR_GUILD_NAME_EXISTS_S         = 0x07,
    ERR_GUILD_LEADER_LEAVE          = 0x08, // for Typecommand 0x03
    ERR_GUILD_PERMISSIONS           = 0x08, // for another Typecommand
    ERR_GUILD_PLAYER_NOT_IN_GUILD   = 0x09,
    ERR_GUILD_PLAYER_NOT_IN_GUILD_S = 0x0A,
    ERR_GUILD_PLAYER_NOT_FOUND_S    = 0x0B,
    ERR_GUILD_NOT_ALLIED            = 0x0C,
    ERR_GUILD_RANK_TOO_HIGH_S       = 0x0D,
    ERR_GUILD_RANK_TOO_LOW_S        = 0x0E,
    // [ZERO] 0x0F, 0x10 unused
    ERR_GUILD_RANKS_LOCKED          = 0x11,
    ERR_GUILD_RANK_IN_USE           = 0x12,
    ERR_GUILD_IGNORING_YOU_S        = 0x13,
    ERR_GUILD_UNK20                 = 0x14  // for Typecommand 0x05 only
};

enum GuildEvents
{
    GE_PROMOTION                    = 0x00,
    GE_DEMOTION                     = 0x01,
    GE_MOTD                         = 0x02,
    GE_JOINED                       = 0x03,
    GE_LEFT                         = 0x04,
    GE_REMOVED                      = 0x05,
    GE_LEADER_IS                    = 0x06,
    GE_LEADER_CHANGED               = 0x07,
    GE_DISBANDED                    = 0x08,
    GE_TABARDCHANGE                 = 0x09,
    GE_UNK1                         = 0x0A,                 // string, string EVENT_GUILD_ROSTER_UPDATE tab content change?
    GE_UNK2                         = 0x0B,                 // EVENT_GUILD_ROSTER_UPDATE
    GE_SIGNED_ON                    = 0x0C,                 // ERR_FRIEND_ONLINE_SS
    GE_SIGNED_OFF                   = 0x0D,                 // ERR_FRIEND_OFFLINE_S
};

enum PetitionSigns
{
    PETITION_SIGN_OK                = 0,
    PETITION_SIGN_ALREADY_SIGNED    = 1,
    PETITION_SIGN_ALREADY_IN_GUILD  = 2,
    PETITION_SIGN_CANT_SIGN_OWN     = 3,
    PETITION_SIGN_NEED_MORE         = 4,
    PETITION_SIGN_NOT_SERVER        = 5
};

enum GuildEventLogTypes
{
    GUILD_EVENT_LOG_INVITE_PLAYER     = 1,
    GUILD_EVENT_LOG_JOIN_GUILD        = 2,
    GUILD_EVENT_LOG_PROMOTE_PLAYER    = 3,
    GUILD_EVENT_LOG_DEMOTE_PLAYER     = 4,
    GUILD_EVENT_LOG_UNINVITE_PLAYER   = 5,
    GUILD_EVENT_LOG_LEAVE_GUILD       = 6,
};

enum GuildEmblem
{
    ERR_GUILDEMBLEM_SUCCESS               = 0,
    ERR_GUILDEMBLEM_INVALID_TABARD_COLORS = 1,
    ERR_GUILDEMBLEM_NOGUILD               = 2,
    ERR_GUILDEMBLEM_NOTGUILDMASTER        = 3,
    ERR_GUILDEMBLEM_NOTENOUGHMONEY        = 4,
    ERR_GUILDEMBLEM_FAIL_NO_MESSAGE       = 5
};

struct GuildEventLogEntry
{
    uint8  EventType;
    uint32 PlayerGuid1;
    uint32 PlayerGuid2;
    uint8  NewRank;
    uint64 TimeStamp;
};

struct MemberSlot
{
    void SetMemberStats(Player* player);
    void UpdateLogoutTime();
    void SetPublicNote(std::string const& publicNote);
    void SetOfficerNote(std::string const& officerNote);
    void ChangeRank(uint32 newRank);

    ObjectGuid guid;
    uint32 accountId;
    std::string Name;
    uint32 RankId;
    uint8 Level;
    uint8 Class;
    uint32 ZoneId;
    uint64 LogoutTime;
    std::string PublicNote;
    std::string OfficerNote;
};

struct RankInfo
{
    RankInfo(std::string const& _name, uint32 _rights) : Name(_name), Rights(_rights)
    {
    }

    std::string Name;
    uint32 Rights;
};

constexpr uint32 OnlineMemberCache = 1000;
constexpr uint32 RosterCache = 1000;
constexpr uint32 CacheExpiryMs = 4000;

class Guild
{
    public:
        Guild();
        ~Guild();

        bool Create(Player* leader, std::string gname);
        bool Create(Petition* petition, Player* leader);
        void CreateDefaultGuildRanks(int locale_idx);
        void Disband();

        typedef std::unordered_map<uint32, MemberSlot> MemberList;
        typedef std::vector<RankInfo> RankList;

        uint32 GetId(){ return m_Id; }
        ObjectGuid GetLeaderGuid() const { return m_LeaderGuid; }
        std::string const& GetName() const { return m_Name; }
        std::string const& GetMOTD() const { return m_motd; }
        std::string const& GetInfo() const { return m_info; }
#ifdef ENABLE_ELUNA
        std::string const& GetGINFO() const { return m_info; }
#endif

        void Rename(std::string& newName);

        uint32 GetCreatedYear() const { return m_CreatedYear; }
        uint32 GetCreatedMonth() const { return m_CreatedMonth; }
        uint32 GetCreatedDay() const { return m_CreatedDay; }

        uint32 GetEmblemStyle() const { return m_EmblemStyle; }
        uint32 GetEmblemColor() const { return m_EmblemColor; }
        uint32 GetBorderStyle() const { return m_BorderStyle; }
        uint32 GetBorderColor() const { return m_BorderColor; }
        uint32 GetBackgroundColor() const { return m_BackgroundColor; }

        void SetLeader(ObjectGuid guid);
        void SetLeader(MemberSlot* slot);
        GuildAddStatus AddMember(ObjectGuid plGuid, uint32 plRank);
        bool DelMember(ObjectGuid guid, bool isDisbanding = false);
        //lowest rank is the count of ranks - 1 (the highest rank_id in table)
        uint32 GetLowestRank() const { return m_Ranks.size() - 1; }

        void SetMOTD(std::string motd);
        void SetGINFO(std::string ginfo);
        void SetEmblem(uint32 emblemStyle, uint32 emblemColor, uint32 borderStyle, uint32 borderColor, uint32 backgroundColor);

        bool AddGMListener(Player* gm);

        uint32 GetMemberSize() const { return members.size(); }
        uint32 GetAccountsNumber();

        bool LoadGuildFromDB(QueryResult *guildDataResult);
        bool CheckGuildStructure();
        bool LoadRanksFromDB(QueryResult *guildRanksResult);
        bool LoadMembersFromDB(QueryResult *guildMembersResult);

        void BroadcastToGuild(WorldSession *session, std::string const& msg, uint32 language = LANG_UNIVERSAL);
        void BroadcastToGuild(MasterPlayer* pPlayer, std::string const& msg, uint32 language = LANG_UNIVERSAL);
        void BroadcastToOfficers(WorldSession *session, std::string const& msg, uint32 language = LANG_UNIVERSAL);
        void BroadcastPacketToRank(WorldPacket *packet, uint32 rankId);
        void BroadcastPacket(WorldPacket *packet);

        void BroadcastEvent(GuildEvents event, ObjectGuid guid, char const* str1 = nullptr, char const* str2 = nullptr, char const* str3 = nullptr);
        void BroadcastEvent(GuildEvents event, char const* str1 = nullptr, char const* str2 = nullptr, char const* str3 = nullptr)
        {
            BroadcastEvent(event, ObjectGuid(), str1, str2, str3);
        }

        template<class Do>
        void BroadcastWorker(Do& _do, Player* except = nullptr)
        {
            for(MemberList::iterator itr = members.begin(); itr != members.end(); ++itr)
                if(Player *player = ObjectAccessor::FindPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first)))
                    if(player != except)
                        _do(player);
        }

        void CreateRank(std::string name,uint32 rights);
        void DelRank();
        std::string GetRankName(uint32 rankId);
        uint32 GetRankRights(uint32 rankId);
        uint32 GetRanksSize() const { return m_Ranks.size(); }

        void SetRankName(uint32 rankId, std::string name);
        void SetRankRights(uint32 rankId, uint32 rights);
        bool HasRankRight(uint32 rankId, uint32 right)
        {
            return ((GetRankRights(rankId) & right) != GR_RIGHT_EMPTY) ? true : false;
        }

        int32 GetRank(ObjectGuid guid)
        {
            MemberSlot* slot = GetMemberSlot(guid);
            return slot ? slot->RankId : -1;
        }

        MemberSlot* GetMemberSlot(ObjectGuid guid)
        {
            MemberList::iterator itr = members.find(guid.GetCounter());
            return itr != members.end() ? &itr->second : nullptr;
        }

        MemberSlot* GetMemberSlot(std::string const& name)
        {
            for (MemberList::iterator itr = members.begin(); itr != members.end(); ++itr)
                if (itr->second.Name == name)
                    return &itr->second;

            return nullptr;
        }

        void SendRoster(WorldSession* session = nullptr)
        {
                Roster(session);
        }

        
        void UpdateCaches(uint32 diff);
        WorldPacket BuildOnlineRosterPacket(bool sendOfficerNote);
        void Roster(WorldSession *session = nullptr);          // nullptr = broadcast
        void TempRosterOnline(WorldSession* session = nullptr);          // nullptr = broadcast
        void Query(WorldSession *session);

        // Guild EventLog
        void   LoadGuildEventLogFromDB();
        void   DisplayGuildEventLog(WorldSession *session);
        void   LogGuildEvent(uint8 EventType, ObjectGuid playerGuid1, ObjectGuid playerGuid2 = ObjectGuid(), uint8 newRank = 0);
        ObjectGuid GetGuildInviter(ObjectGuid playerGuid) const;
        bool GetSuitableNewLeader(MemberSlot*& newLeaderSlot, MemberSlot*& oldLeaderSlot, bool preferOldestOfficer = false);
        void SetNewLeader(ObjectGuid newLeaderGuid);
        void SetNewLeader(MemberSlot* newLeaderSlot, MemberSlot* oldLeaderSlot);

        void AddToCache(uint32 guidLow)
        {
            m_onlineMemberCache.insert(guidLow);
        }

        void RemoveFromCache(uint32 guidLow)
        {
            m_onlineMemberCache.erase(guidLow);
        }

        bool IsMemberCacheEnabled() const
        {
            return members.size() >= OnlineMemberCache;
        }

        bool IsRosterCacheEnabled() const
        {
            return members.size() >= RosterCache;
        }

    protected:
        void AddRank(std::string const& name,uint32 rights);

        uint32 m_Id;
        std::string m_Name;
        ObjectGuid m_LeaderGuid;
        std::string m_motd;
        std::string m_info;
        uint32 m_CreatedYear;
        uint32 m_CreatedMonth;
        uint32 m_CreatedDay;

        uint32 m_EmblemStyle;
        uint32 m_EmblemColor;
        uint32 m_BorderStyle;
        uint32 m_BorderColor;
        uint32 m_BackgroundColor;
        uint32 m_accountsNumber;                            // 0 used as marker for need lazy calculation at request

        RankList m_Ranks;

        std::unique_ptr<WorldPacket> m_cachedRosterPacket;
        std::unique_ptr<WorldPacket> m_cachedOfficerRosterPacket;
        uint32 m_cacheTimer = CacheExpiryMs;

        MemberList members;
        std::unordered_set<uint32> m_onlineMemberCache;

        /** These are actually ordered lists. The first element is the oldest entry.*/
        std::list<GuildEventLogEntry> m_GuildEventLog;

        std::unordered_set<ObjectGuid> m_GmListeners;

        uint32 m_GuildEventLogNextGuid;


    private:
        void UpdateAccountsNumber() { m_accountsNumber = 0;}// mark for lazy calculation at request in GetAccountsNumber

		// Guild Bank
	public:
		GuildBank* _Bank;
        GuildBank* _InfernoBank;
		// Guild Bank end;
};

#endif
