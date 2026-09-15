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

#ifndef _ACCMGR_H
#define _ACCMGR_H

#include "Common.h"
#include "Policies/Singleton.h"
#include <string>
#include <unordered_set>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include "PerformanceMonitor.h"

enum AccountOpResult
{
    AOR_OK,
    AOR_NAME_TOO_LONG,
    AOR_PASS_TOO_LONG,
    AOR_NAME_ALREDY_EXIST,
    AOR_NAME_NOT_EXIST,
    AOR_DB_INTERNAL_ERROR
};


class WorldSession;
class ChatHandler;
class MasterPlayer;
class Player;

#define MAX_ACCOUNT_STR 16

class AccountPersistentData
{
/// WHISP FLOOD
public:
    struct WhisperData
    {
        WhisperData() : first_whisp(time(nullptr)), score(0), whispers_count(0) {}
        time_t first_whisp;
        uint32 score;
        uint32 whispers_count;
    };
    void WhisperedBy(MasterPlayer* whisperer);
    uint32 CountWhispersTo(MasterPlayer* from, MasterPlayer* player);
    bool CanWhisper(MasterPlayer* player) const;
    uint32 GetWhisperScore(MasterPlayer* from, MasterPlayer* player) const;
    uint32 CountDifferentWhispTargets() const { return m_whisperTargets.size(); }

    typedef std::map<uint32 /*lowguid*/, WhisperData> WhispersMap;
    WhispersMap m_whisperTargets;

/// MAIL FLOOD
public:
    void JustMailed(uint32 toAccount);
    bool CanMail(uint32 targetAccount);
protected:
    typedef std::map<uint32, time_t> MailsSentMap;
    MailsSentMap m_mailsSent;
};

struct AccountData
{
    uint32 Id = 0;
    std::string Username;
    std::string Email;
    std::string ForumName;
    std::string LastIP;
    std::string LastWarning;
    std::optional<uint32> BannedUntil;
    uint64      CreatedAt;
    uint32 SentMailCount = 0;
};


class AccountMgr
{
    public:
        AccountMgr();
        ~AccountMgr();

        AccountOpResult CreateAccount(std::string username, std::string password);
        AccountOpResult DeleteAccount(uint32 accid);
        AccountOpResult ChangeUsername(uint32 accid, std::string new_uname, std::string new_passwd);
        AccountOpResult ChangePassword(uint32 accid, std::string new_passwd, std::string username="");
        bool CheckPassword(uint32 accid, std::string passwd, std::string username="");

        uint32 GetId(std::string username);

        void Load();
        AccountTypes GetSecurity(uint32 acc_id);
        // Reads the rank from the account row instead of the cache LoadGmLevels
        // fills at startup: callers outside the game (SOAP) must see a row that
        // was inserted or changed by SQL while the world runs.
        AccountTypes GetSecurityFromDatabase(uint32 acc_id);
        // Reads account_banned with realmd's predicate (AuthSocket.cpp) instead of
        // the cache LoadAccountBanList fills at startup: a ban inserted by SQL or by
        // realmd's failed-login autoban is not in the cache.
        bool IsAccountBannedInDatabase(uint32 acc_id);
        void SetSecurity(uint32 accId, AccountTypes sec);

        void LoadGmLevels();

        bool GetName(uint32 acc_id, std::string &name);
        uint32 GetCharactersCount(uint32 acc_id);
        std::string CalculateShaPassHash(std::string& name, std::string& password);

        bool IsTraineeGM(uint32 accId) const
        {
            return m_traineeGms.find(accId) != m_traineeGms.end();
        }

        void ToggleTraineeGM(uint32 accId)
        {
            if (IsTraineeGM(accId))
                m_traineeGms.erase(accId);
            else
                m_traineeGms.insert(accId);
        }

        uint32 GetFlags(uint32 accountId);
        static bool normalizeString(std::string& utf8str);
        // Nostalrius
        void Update(uint32 diff);
        void LoadAccountNames();
        void LoadIPBanList(bool silent=false);
        void LoadAccountBanList(bool silent=false);
        void LoadFingerprintBanList(bool silent = false);
        void LoadAccountWarnings(bool silent = false);
        void LoadAccountIP();
        void LoadAccountEmail();
        void LoadAccountForumName();
        void LoadAccountHighestCharLevel();
        void LoadDonatorAccounts();
        void SendPlayerInfoInAddonMessage(char const* playerName, Player* pPlayer);
        void BanIP(std::string const& ip, uint32 unbandate) { std::unique_lock lock{ m_ipBannedMutex }; m_ipBanned[ip] = unbandate; }
        void UnbanIP(std::string const& ip) { std::unique_lock lock{ m_ipBannedMutex };  m_ipBanned.erase(ip); }
        void BanAccount(uint32 account, uint32 unbandate) { m_accountData[account].BannedUntil = unbandate; }
        void UnbanAccount(uint32 acc) { m_accountData[acc].BannedUntil = std::nullopt; }
        void WarnAccount(uint32 acc, std::string reason) { m_accountData[acc].LastWarning = reason; }

        char const* GetWarningText(uint32 acc) const
        {
            char const* warningText = nullptr;
            if (auto itr = m_accountData.find(acc); itr != m_accountData.end() && !itr->second.LastWarning.empty())
                warningText = itr->second.LastWarning.c_str();
            return warningText;
        }

        std::string GetForumName(uint32 acc) const
        {
            if (auto itr = m_accountData.find(acc); itr != m_accountData.end())
                return itr->second.ForumName;
            return "none";
        }

        std::string GetAccountEmail(uint32 acc) const
        {
            if (auto itr = m_accountData.find(acc); itr != m_accountData.end())
                return itr->second.Email;
            return "none@none";
        }

        std::string GetAccountIP(uint32 acc) const
        {
            if (auto itr = m_accountData.find(acc); itr != m_accountData.end())
                return itr->second.LastIP;
            return "0.0.0.0";
        }

        // returns true if previous ip was different
        bool UpdateAccountIP(uint32 acc, std::string const& ip)
        {
            auto itr = m_accountData.find(acc);
            if (itr != m_accountData.end())
            {
                if (ip != itr->second.LastIP)
                {
                    itr->second.LastIP = ip;
                    return true;
                }
                return false;
            }

            m_accountData[acc].LastIP = ip;
            return true;
        }

        uint32 GetMailCount(uint32 accountId) const
        {
            std::shared_lock lock(m_accountDataMutex);
            if (auto itr = m_accountData.find(accountId); itr != m_accountData.end())
                return itr->second.SentMailCount;
            return 0;
        }

        void IncreaseMailCount(uint32 accountId)
        {
            std::unique_lock lock(m_accountDataMutex);
            m_accountData[accountId].SentMailCount++;
        }

        uint32 GetHighestCharLevel(uint32 accountId)
        {
            auto itr = m_accountHighestCharLevel.find(accountId);
            if (itr != m_accountHighestCharLevel.end())
                return itr->second;
            return 0;
        }

        bool IsDonator(uint32 accountId)
        {
            return m_donatorAccounts.find(accountId) != m_donatorAccounts.end();
        }

        void BanFingerprint(uint32 fingerprint, uint32 unbandate) { m_fingerprintBanned[fingerprint] = unbandate; }
        bool BanAccountsWithFingerprint(uint32 fingerprint, uint32 duration_secs, std::string reason, ChatHandler* chatHandler);
        void UnbanFingerprint(uint32 fingerprint) { m_fingerprintBanned.erase(fingerprint); }
        bool IsIPBanned(std::string const& ip) const;
        bool IsAccountBanned(uint32 acc) const;
        bool IsFingerprintBanned(uint32 fingerprint) const;
        void AddAutobanFingerprint(uint32 fingerprint) { m_fingerprintAutoban.insert(fingerprint); }
        void RemoveAutobanFingerprint(uint32 fingerprint) { m_fingerprintAutoban.erase(fingerprint); }
        bool ShouldAutoBanFingerprint(uint32 fingerprint) const { return m_fingerprintAutoban.find(fingerprint) != m_fingerprintAutoban.end(); };

        // Max instance reset per account per hour
        bool CheckInstanceCount(uint32 accountId, uint32 instanceId, uint32 maxCount);
        void AddInstanceEnterTime(uint32 accountId, uint32 instanceId, time_t enterTime);

        // TW - Nolin
        bool IsPlayerAccount(uint32 gmlevel);
        bool IsGMAccount(uint32 gmlevel);
        bool IsAdminAccount(uint32 gmlevel);
        bool IsConsoleAccount(uint32 gmlevel);

        AccountPersistentData& GetAccountPersistentData(uint32 accountId) { return m_accountPersistentData[accountId]; }

        AccountData* GetAccountData(uint32 accountId) 
        {
            if (auto itr = m_accountData.find(accountId); 
                itr != m_accountData.end()) 
                return &itr->second; 
            return nullptr; 
        }
    protected:

        turtle_unordered_map<uint32, AccountData, Category_AccountStorage> m_accountData;
        turtle_unordered_map<std::string, uint32, Category_AccountStorage> m_accountNameToId;
        std::map<uint32, uint32> m_accountHighestCharLevel;
        std::map<uint32, AccountTypes> m_accountSecurity;
        uint32 m_banlistUpdateTimer;
        uint32 m_fingerprintAutobanTimer;
        uint32 m_accountMailsResetTimer;
        turtle_unordered_map<std::string, uint32, Category_AccountStorage> m_ipBanned;
        std::map<uint32, uint32> m_fingerprintBanned;
        std::unordered_set<uint32> m_traineeGms;
        std::unordered_set<uint32> m_donatorAccounts;
        std::set<uint32> m_fingerprintAutoban;
        typedef std::map<uint32 /* instanceId */, time_t /* enter time */> InstanceEnterTimesMap;
        typedef std::map<uint32 /* accountId */, InstanceEnterTimesMap> AccountInstanceEnterTimesMap;
        AccountInstanceEnterTimesMap m_instanceEnterTimes;
        std::map<uint32, AccountPersistentData> m_accountPersistentData;
        mutable std::shared_mutex m_ipBannedMutex;
        mutable std::shared_mutex m_accountDataMutex;
};

extern AccountMgr sAccountMgr;

#endif
