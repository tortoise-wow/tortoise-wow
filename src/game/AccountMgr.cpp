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

#include "AccountMgr.h"
#include "Database/DatabaseEnv.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "Policies/SingletonImp.h"
#include "Util.h"
#include "Auth/Sha1.h"
#include "World.h"
#include "Chat.h"
#include "WorldSession.h"
#include "Chat.h"
#include "Guild.h"
#include "MasterPlayer.h"
#include "Anticheat.h"

AccountMgr sAccountMgr;

AccountMgr::AccountMgr() : m_banlistUpdateTimer(0), m_fingerprintAutobanTimer(0), m_accountMailsResetTimer(0)
{}

AccountMgr::~AccountMgr()
{}

AccountOpResult AccountMgr::CreateAccount(std::string username, std::string password)
{
    if (utf8length(username) > MAX_ACCOUNT_STR)
        return AOR_NAME_TOO_LONG;                           // username's too long

    normalizeString(username);
    normalizeString(password);

    if (GetId(username))
    {
        return AOR_NAME_ALREDY_EXIST;                       // username does already exist
    }

    if (!LoginDatabase.PExecute("INSERT INTO account(username,sha_pass_hash,joindate) VALUES('%s','%s',NOW())", username.c_str(), CalculateShaPassHash(username, password).c_str()))
        return AOR_DB_INTERNAL_ERROR;                       // unexpected error
    LoginDatabase.Execute("REPLACE INTO realmcharacters (realmid, acctid, numchars) SELECT realmlist.id, account.id, 0 FROM realmlist,account LEFT JOIN realmcharacters ON acctid=account.id WHERE acctid IS NULL");

    return AOR_OK;                                          // everything's fine
}

AccountOpResult AccountMgr::DeleteAccount(uint32 accid)
{
    QueryResult *result = LoginDatabase.PQuery("SELECT 1 FROM account WHERE id='%u'", accid);
    if (!result)
        return AOR_NAME_NOT_EXIST;                          // account doesn't exist
    delete result;

    // existing characters list
    result = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE account='%u'", accid);
    if (result)
    {
        do
        {
            Field *fields = result->Fetch();
            uint32 guidlo = fields[0].GetUInt32();
            ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, guidlo);

            // kick if player currently
            ObjectAccessor::KickPlayer(guid);
            Player::DeleteFromDB(guid, accid, false);       // no need to update realm characters
        }
        while (result->NextRow());

        delete result;
    }

    // table realm specific but common for all characters of account for realm
    CharacterDatabase.PExecute("DELETE FROM character_tutorial WHERE account = '%u'", accid);

    LoginDatabase.BeginTransaction();

    bool res =
        LoginDatabase.PExecute("DELETE FROM account WHERE id='%u'", accid) &&
        LoginDatabase.PExecute("DELETE FROM realmcharacters WHERE acctid='%u'", accid);

    LoginDatabase.CommitTransaction();

    if (!res)
        return AOR_DB_INTERNAL_ERROR;                       // unexpected error;

    return AOR_OK;
}

//#DEPRECATED: Not used anywhere, should we delete?
AccountOpResult AccountMgr::ChangeUsername(uint32 accid, std::string new_uname, std::string new_passwd)
{
    QueryResult *result = LoginDatabase.PQuery("SELECT 1 FROM account WHERE id='%u'", accid);
    if (!result)
        return AOR_NAME_NOT_EXIST;                          // account doesn't exist
    delete result;

    if (utf8length(new_uname) > MAX_ACCOUNT_STR)
        return AOR_NAME_TOO_LONG;

    if (utf8length(new_passwd) > MAX_ACCOUNT_STR)
        return AOR_PASS_TOO_LONG;

    normalizeString(new_uname);
    normalizeString(new_passwd);

    std::string safe_new_uname = new_uname;
    LoginDatabase.escape_string(safe_new_uname);

    if (!LoginDatabase.PExecute("UPDATE account SET v='0',s='0',username='%s',sha_pass_hash='%s' WHERE id='%u'", safe_new_uname.c_str(),
                                CalculateShaPassHash(new_uname, new_passwd).c_str(), accid))
        return AOR_DB_INTERNAL_ERROR;                       // unexpected error

    return AOR_OK;
}

AccountOpResult AccountMgr::ChangePassword(uint32 accid, std::string new_passwd, std::string username)
{
    if (username.empty())
    {
        if (!GetName(accid, username))
            return AOR_NAME_NOT_EXIST;
    }
    else
        normalizeString(username);                       // account doesn't exist

    if (utf8length(new_passwd) > MAX_ACCOUNT_STR)
        return AOR_PASS_TOO_LONG;

    normalizeString(new_passwd);

    // also reset s and v to force update at next realmd login
    if (!LoginDatabase.PExecute("UPDATE account SET v='0', s='0', sha_pass_hash='%s' WHERE id='%u'",
                                CalculateShaPassHash(username, new_passwd).c_str(), accid))
        return AOR_DB_INTERNAL_ERROR;                       // unexpected error

    return AOR_OK;
}

uint32 AccountMgr::GetId(std::string username)
{
    auto itr = m_accountNameToId.find(username);
    if (itr != m_accountNameToId.end())
        return itr->second;

    LoginDatabase.escape_string(username);
    QueryResult *result = LoginDatabase.PQuery("SELECT id FROM account WHERE username = '%s'", username.c_str());
    if (!result)
        return 0;
    else
    {
        uint32 id = (*result)[0].GetUInt32();
        delete result;
        return id;
    }
}

void AccountMgr::Load()
{
    LoadGmLevels();
    LoadAccountNames();
    LoadAccountBanList();
    LoadAccountWarnings();
    LoadAccountIP();
    LoadAccountForumName();
    LoadAccountEmail();
    LoadIPBanList();
    LoadFingerprintBanList();
    LoadAccountHighestCharLevel();
    LoadDonatorAccounts();
}

void AccountMgr::LoadGmLevels()
{
    m_accountSecurity.clear();

    std::unique_ptr<QueryResult> result(LoginDatabase.PQuery("SELECT `id`, `rank` FROM `account` WHERE `rank` > 0"));

    if (!result)
    {
        return;
    }

    Field* fields = nullptr;
    do
    {
        fields = result->Fetch();
        uint32 accountId = fields[0].GetUInt32();
        AccountTypes secu = AccountTypes(fields[1].GetUInt32());
        switch (secu)
        {
        case SEC_OBSERVER:
        case SEC_MODERATOR:
        case SEC_DEVELOPER:
        case SEC_ADMINISTRATOR:
        case SEC_SIGMACHAD:
            if (m_accountSecurity.find(accountId) == m_accountSecurity.end() ||
                m_accountSecurity[accountId] < secu)
                m_accountSecurity[accountId] = secu;
            break;
        }
    } while (result->NextRow());
}

void AccountMgr::LoadAccountNames()
{
    std::unique_ptr<QueryResult> result(LoginDatabase.PQuery("SELECT `id`, `username`, UNIX_TIMESTAMP(joindate) FROM `account`"));

    if (result)
    {
        m_accountNameToId.clear();
        do
        {
            Field* fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            std::string username = fields[1].GetCppString();
            if (username.empty())
                continue;

            m_accountNameToId.insert({ username, id });
            m_accountData[id].Username = std::move(username);

            uint64 timeCreated = fields[2].GetUInt64();

            m_accountData[id].CreatedAt = timeCreated;

        } while (result->NextRow());
    }
}

AccountTypes AccountMgr::GetSecurity(uint32 acc_id)
{
    std::map<uint32, AccountTypes>::const_iterator it = m_accountSecurity.find(acc_id);
    if (it == m_accountSecurity.end())
        return SEC_PLAYER;
    return it->second;
}

AccountTypes AccountMgr::GetSecurityFromDatabase(uint32 acc_id)
{
    std::unique_ptr<QueryResult> result(LoginDatabase.PQuery("SELECT `rank` FROM `account` WHERE `id` = '%u'", acc_id));
    if (!result)
        return SEC_PLAYER;

    AccountTypes const secu = AccountTypes(result->Fetch()[0].GetUInt32());
    switch (secu)
    {
    case SEC_OBSERVER:
    case SEC_MODERATOR:
    case SEC_DEVELOPER:
    case SEC_ADMINISTRATOR:
    case SEC_SIGMACHAD:
        return secu;
    default:
        // Same set LoadGmLevels accepts; any other value is a player.
        return SEC_PLAYER;
    }
}

bool AccountMgr::IsAccountBannedInDatabase(uint32 acc_id)
{
    std::unique_ptr<QueryResult> result(LoginDatabase.PQuery(
        "SELECT 1 FROM `account_banned` WHERE `id` = '%u' AND `active` = 1 AND (`unbandate` > UNIX_TIMESTAMP() OR `unbandate` = `bandate`) LIMIT 1", acc_id));
    return result != nullptr;
}

void AccountMgr::SetSecurity(uint32 accId, AccountTypes sec)
{
    m_accountSecurity[accId] = sec;
    LoginDatabase.PExecute("UPDATE `account` SET `rank` = '%u' WHERE (`id` = '%u')", sec, accId);
}

bool AccountMgr::GetName(uint32 acc_id, std::string &name)
{
    auto itr = m_accountData.find(acc_id);
    if (itr != m_accountData.end())
    {
        name = itr->second.Username;
        return true;
    }
    QueryResult *result = LoginDatabase.PQuery("SELECT username FROM account WHERE id = '%u'", acc_id);
    if (result)
    {
        name = (*result)[0].GetCppString();
        delete result;
        if (normalizeString(name))
            return true;
    }

    return false;
}

uint32 AccountMgr::GetCharactersCount(uint32 acc_id)
{
    // check character count
    QueryResult *result = CharacterDatabase.PQuery("SELECT COUNT(guid) FROM characters WHERE account = '%u'", acc_id);
    if (result)
    {
        Field *fields = result->Fetch();
        uint32 charcount = fields[0].GetUInt32();
        delete result;
        return charcount;
    }
    else
        return 0;
}

bool AccountMgr::CheckPassword(uint32 accid, std::string passwd, std::string username)
{
    if (username.empty())
    {
        if (!GetName(accid, username))
            return false;
    }
    else
        normalizeString(username);

    normalizeString(passwd);

    QueryResult *result = LoginDatabase.PQuery("SELECT 1 FROM account WHERE id='%u' AND sha_pass_hash='%s'", accid, CalculateShaPassHash(username, passwd).c_str());
    if (result)
    {
        delete result;
        return true;
    }

    return false;
}

bool AccountMgr::normalizeString(std::string& utf8str)
{
    std::wstring wstr_buf;
    if (!Utf8toWStr(utf8str, wstr_buf))
        return false;

    if (wstr_buf.size() > MAX_ACCOUNT_STR)
        return false;

    std::transform(wstr_buf.begin(), wstr_buf.end(), wstr_buf.begin(), wcharToUpperOnlyLatin);

    return WStrToUtf8(wstr_buf, utf8str);
}

std::string AccountMgr::CalculateShaPassHash(std::string& name, std::string& password)
{
    Sha1Hash sha;
    sha.Initialize();
    sha.UpdateData(name);
    sha.UpdateData(":");
    sha.UpdateData(password);
    sha.Finalize();

    std::string encoded;
    hexEncodeByteArray(sha.GetDigest(), sha.GetLength(), encoded);

    return encoded;
}

void AccountMgr::Update(uint32 diff)
{
    if (m_banlistUpdateTimer < diff)
    {
        m_banlistUpdateTimer = sWorld.getConfig(CONFIG_UINT32_BANLIST_RELOAD_TIMER) * 1000;
        LoadIPBanList(true);
    }
    else
        m_banlistUpdateTimer -= diff;

    if (m_fingerprintAutobanTimer < diff)
    {
        // sLog.outInfo("Auto banning %u fingerprints.", (uint32)m_fingerprintAutoban.size());
        m_fingerprintAutobanTimer = 1 * MINUTE * IN_MILLISECONDS;

        for (auto const& fingerprint : m_fingerprintAutoban)
        {
            BanAccountsWithFingerprint(fingerprint, 0, "Fingerprint Autoban", nullptr);
        }
    }
    else
        m_fingerprintAutobanTimer -= diff;

    if (m_accountMailsResetTimer < diff)
    {
        m_accountMailsResetTimer = 1 * HOUR * IN_MILLISECONDS;
        std::unique_lock lock(m_accountDataMutex);
        for (auto& itr : m_accountData)
        {
            itr.second.SentMailCount = 0;
        }
    }
    else
        m_accountMailsResetTimer -= diff;
}

void AccountMgr::LoadIPBanList(bool silent)
{
    std::unique_ptr<QueryResult> banresult(LoginDatabase.PQuery("SELECT `ip`, `unbandate`, `bandate` FROM `ip_banned` WHERE (`unbandate` > UNIX_TIMESTAMP() OR `bandate` = `unbandate`)"));
    
    if (!banresult)
    {
        return;
    }

    std::unique_lock lock(m_ipBannedMutex);
    m_ipBanned.clear();
    do
    {
        Field* fields = banresult->Fetch();
        uint32 unbandate = fields[1].GetUInt32();
        uint32 bandate = fields[2].GetUInt32();
        if (unbandate == bandate)
            unbandate = 0xFFFFFFFF;

        m_ipBanned[fields[0].GetString()] = unbandate;
    } while (banresult->NextRow());

}

void AccountMgr::LoadAccountBanList(bool silent)
{
    std::unique_ptr<QueryResult> banresult(LoginDatabase.PQuery("SELECT `id`, `unbandate`, `bandate` FROM `account_banned` WHERE `active` = 1 AND (`unbandate` > UNIX_TIMESTAMP() OR `bandate` = `unbandate`)"));
    
    if (!banresult)
    {
        return;
    }

    std::unique_lock lock(m_accountDataMutex);
    do
    {
        Field* fields = banresult->Fetch();
        uint32 unbandate = fields[1].GetUInt32();
        uint32 bandate = fields[2].GetUInt32();
        if (unbandate == bandate)
            unbandate = 0xFFFFFFFF;
        m_accountData[fields[0].GetUInt32()].BannedUntil = unbandate;
    } while (banresult->NextRow());
}

void AccountMgr::LoadAccountWarnings(bool silent)
{

    uint32 maxAge = sWorld.getConfig(CONFIG_UINT32_MAX_AGE_SHOW_WARNING);


    std::unique_ptr<QueryResult> banresult(LoginDatabase.PQuery("SELECT `id`, `banreason` FROM `account_banned` WHERE `active` = 0 AND (`banreason` LIKE \"WARN:%\") AND `bandate` > UNIX_TIMESTAMP(NOW() - INTERVAL %u DAY) ORDER BY `bandate`", maxAge));

    if (!banresult)
    {
        return;
    }

    std::unique_lock lock(m_accountDataMutex);
    do
    {
        Field* fields = banresult->Fetch();
        m_accountData[fields[0].GetUInt32()].LastWarning = fields[1].GetCppString().substr(5, fields[1].GetCppString().size() - 5);
    } while (banresult->NextRow());
}

void AccountMgr::LoadAccountIP()
{

    std::unique_ptr<QueryResult> banresult(LoginDatabase.Query("SELECT `id`, `last_ip` FROM `account` WHERE `last_ip` != '0.0.0.0'"));

    if (!banresult)
    {
        return;
    }

    std::unique_lock lock(m_accountDataMutex);
    do
    {
        Field* fields = banresult->Fetch();
        m_accountData[fields[0].GetUInt32()].LastIP = fields[1].GetCppString();
    } while (banresult->NextRow());
}

void AccountMgr::LoadAccountForumName()
{

    std::unique_ptr<QueryResult> banresult(LoginDatabase.Query("SELECT `id`, `forum_username` FROM `account` WHERE `forum_username` != ''"));

    if (!banresult)
    {
        return;
    }

    std::unique_lock lock(m_accountDataMutex);
    do
    {
        Field* fields = banresult->Fetch();
        m_accountData[fields[0].GetUInt32()].ForumName = fields[1].GetCppString();
    } while (banresult->NextRow());
}

void AccountMgr::LoadAccountEmail()
{

    std::unique_ptr<QueryResult> banresult(LoginDatabase.Query("SELECT `id`, `email` FROM `account` WHERE `email` != ''"));

    if (!banresult)
    {
        return;
    }

    std::unique_lock lock(m_accountDataMutex);
    do
    {
        Field* fields = banresult->Fetch();
        m_accountData[fields[0].GetUInt32()].Email = fields[1].GetCppString();
    } while (banresult->NextRow());
}

void AccountMgr::LoadAccountHighestCharLevel()
{
    std::unique_ptr<QueryResult> result(CharacterDatabase.Query("SELECT `account`, `level` FROM `characters`"));

    if (!result)
    {
        return;
    }

    m_accountHighestCharLevel.clear();
    do
    {
        Field* fields = result->Fetch();
        uint32 accountId = fields[0].GetUInt32();
        uint32 level = fields[1].GetUInt32();

        if (level > m_accountHighestCharLevel[accountId])
            m_accountHighestCharLevel[accountId] = level;

    } while (result->NextRow());
}

void AccountMgr::LoadDonatorAccounts()
{
    std::unique_ptr<QueryResult> result(LoginDatabase.Query("SELECT DISTINCT `account_id` FROM `shop_coins_history`"));

    if (!result)
    {
        return;
    }

    m_donatorAccounts.clear();
    do
    {
        Field* fields = result->Fetch();
        uint32 accountId = fields[0].GetUInt32();

        m_donatorAccounts.insert(accountId);

    } while (result->NextRow());
}

void AccountMgr::LoadFingerprintBanList(bool silent)
{
    std::unique_ptr<QueryResult> banresult(LoginDatabase.PQuery("SELECT `fingerprint`, `unbandate`, `bandate` FROM `fingerprint_banned` WHERE (`unbandate` > UNIX_TIMESTAMP() OR `bandate` = `unbandate`)"));

    if (banresult)
    {
        m_fingerprintBanned.clear();
        do
        {
            Field* fields = banresult->Fetch();
            uint32 unbandate = fields[1].GetUInt32();
            uint32 bandate = fields[2].GetUInt32();
            if (unbandate == bandate)
                unbandate = 0xFFFFFFFF;
            m_fingerprintBanned[fields[0].GetUInt32()] = unbandate;
        } while (banresult->NextRow());
    }

    banresult.reset(LoginDatabase.PQuery("SELECT `fingerprint` FROM `fingerprint_autoban`"));

    if (banresult)
    {
        m_fingerprintAutoban.clear();
        do
        {
            Field* fields = banresult->Fetch();
            uint32 fingerprint = fields[0].GetUInt32();
            m_fingerprintAutoban.insert(fingerprint);
        } while (banresult->NextRow());
    }
}

bool AccountMgr::BanAccountsWithFingerprint(uint32 fingerprint, uint32 duration_secs, std::string reason, ChatHandler* chatHandler)
{
    // add fingerprint to reason
    reason = std::to_string(fingerprint) + " - " + reason;

    auto accountNames = sWorld.GetAccountNamesByFingerprint(fingerprint);
    for (auto it = accountNames.begin(); it != accountNames.end(); )
    {
        uint32 id = GetId(*it);
        if (IsAccountBanned(id))
        {
            it = accountNames.erase(it);
        }
        else
        {
            ++it;
        }
    }

    std::unique_ptr<QueryResult> result(LoginDatabase.PQuery("SELECT `id`, `username` FROM `account` WHERE `id` IN (SELECT `account` FROM `system_fingerprint_usage` WHERE `fingerprint`=%u)", fingerprint));

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 accountId = fields[0].GetUInt32();
            if (IsAccountBanned(accountId))
                continue;

            std::string username = fields[1].GetCppString();
            if (!AccountMgr::normalizeString(username))
                continue;

            accountNames.insert(username);

        } while (result->NextRow());
    }

    if (accountNames.empty())
    {
        if (chatHandler)
            chatHandler->SendSysMessage("No accounts with that fingerprint found.");

        return false;
    }

    for (const auto& accountName : accountNames)
    {
        if (chatHandler)
            chatHandler->PSendSysMessage("Banning account %s...", accountName.c_str());
        
        sWorld.BanAccount(BAN_ACCOUNT, accountName, duration_secs, reason, chatHandler && chatHandler->GetSession() ? chatHandler->GetSession()->GetPlayerName() : "");
    }

    return true;
}

bool AccountMgr::IsIPBanned(std::string const& ip) const
{
    std::shared_lock lock{ m_ipBannedMutex };
    auto it = m_ipBanned.find(ip);
    return !(it == m_ipBanned.end() || it->second < time(nullptr));
}

bool AccountMgr::IsAccountBanned(uint32 acc) const
{
    std::shared_lock lock(m_accountDataMutex);
    auto it = m_accountData.find(acc);
    return (it != m_accountData.end() && it->second.BannedUntil > time(nullptr));
}

bool AccountMgr::IsFingerprintBanned(uint32 fingerprint) const
{
    std::map<uint32, uint32>::const_iterator it = m_fingerprintBanned.find(fingerprint);
    return !(it == m_fingerprintBanned.end() || it->second < time(nullptr));
}

bool AccountMgr::CheckInstanceCount(uint32 accountId, uint32 instanceId, uint32 maxCount)
{
    AccountInstanceEnterTimesMap::iterator it = m_instanceEnterTimes.find(accountId);
    if (it == m_instanceEnterTimes.end())
        return true;

    InstanceEnterTimesMap& enterTimes = it->second;
    InstanceEnterTimesMap::iterator it2 = enterTimes.find(instanceId);

    if (it2 != enterTimes.end())
        return true;

    if (enterTimes.size() < maxCount)
        return true;

    time_t now = time(nullptr);

    for (it2 = enterTimes.begin(); it2 != enterTimes.end(); ++it2)
    {
        if (it2->second + 3600 < now)
        {
            enterTimes.erase(it2);
            return true;
        }
    }

    return false;
}

void AccountMgr::AddInstanceEnterTime(uint32 accountId, uint32 instanceId, time_t enterTime)
{
    AccountInstanceEnterTimesMap::iterator it = m_instanceEnterTimes.find(accountId);
    if (it == m_instanceEnterTimes.end())
    {
        InstanceEnterTimesMap resetTimes;
        resetTimes[instanceId] = enterTime;
        m_instanceEnterTimes[accountId] = resetTimes;
        return;
    }

    it->second[instanceId] = enterTime;
}

uint32 AccountMgr::GetFlags(uint32 accountId)
{
    QueryResult* result = LoginDatabase.PQuery("SELECT flags FROM account WHERE id='%u'", accountId);
    if (result)
    {
        uint32 val = result->Fetch()[0].GetUInt32();
        delete result;
        return val;
    }
    return 0;
}

bool AccountMgr::IsPlayerAccount(uint32 gmlevel)
{
    return gmlevel == SEC_PLAYER;
}

bool AccountMgr::IsGMAccount(uint32 gmlevel)
{
    return gmlevel >= SEC_OBSERVER && gmlevel <= SEC_CONSOLE;
}

bool AccountMgr::IsAdminAccount(uint32 gmlevel)
{
    return gmlevel == SEC_ADMINISTRATOR || gmlevel == SEC_CONSOLE || gmlevel == SEC_SIGMACHAD;
}

bool AccountMgr::IsConsoleAccount(uint32 gmlevel)
{
    return gmlevel == SEC_CONSOLE;
}

// Anticheat
void AccountPersistentData::WhisperedBy(MasterPlayer* whisperer)
{
    WhispersMap::iterator it = m_whisperTargets.find(whisperer->GetGUIDLow());
    if (it != m_whisperTargets.end())
        it->second.score = 0;
}

uint32 AccountPersistentData::CountWhispersTo(MasterPlayer* from, MasterPlayer* player)
{
    sAccountMgr.GetAccountPersistentData(player->GetSession()->GetAccountId()).WhisperedBy(from);
    WhisperData& data = m_whisperTargets[player->GetGUIDLow()];
    ++data.whispers_count;
    if (data.whispers_count == 1)
        data.score = GetWhisperScore(from, player);

    return data.whispers_count-1;
}

bool AccountPersistentData::CanWhisper(MasterPlayer* player) const
{
    return true;
   // return sAnticheatLib->CanWhisper(*this, player);
}

uint32 AccountPersistentData::GetWhisperScore(MasterPlayer* from, MasterPlayer* target) const
{
    uint32 score = 3;
    Player* playerFrom = from->GetSession()->GetPlayer();
    Player* playerTo = target->GetSession()->GetPlayer();
    if (playerFrom->GetGuildId() && playerFrom->GetGuildId() == playerTo->GetGuildId())
        score = 1;
    if (from->GetAreaId() == target->GetAreaId())
        score = 1;
    if (playerTo->GetGroup() && playerTo->GetGroup() == playerFrom->GetGroup())
        score = 1;
    return score;
}

void AccountPersistentData::JustMailed(uint32 toAccount)
{
    m_mailsSent[toAccount] = time(nullptr);
}

bool AccountPersistentData::CanMail(uint32 targetAccount)
{
    // Already sent a mail to this guy => OK
    if (m_mailsSent.find(targetAccount) != m_mailsSent.end())
        return true;

    uint32 totalScore = 0;
    time_t lastNonExpired = time(nullptr) - sWorld.getConfig(CONFIG_UINT32_MAILSPAM_EXPIRE_SECS);
    for (const auto& it : m_mailsSent)
        if (it.second >= lastNonExpired)
            totalScore++;

    uint32 allowedScore = sWorld.getConfig(CONFIG_UINT32_MAILSPAM_MAX_MAILS);
    return totalScore < allowedScore;
}

void AccountMgr::SendPlayerInfoInAddonMessage(char const* playerName, Player* pPlayer)
{
    ObjectGuid guid = sObjectMgr.GetPlayerGuidByName(playerName);
    if (guid.IsEmpty())
        return;

    PlayerCacheData const* pPlayerCache = sObjectMgr.GetPlayerDataByGUID(guid);
    if (!pPlayerCache)
        return;

    std::stringstream ss;

    // playerinfo;;guid;;account;;ip;;level;;email;;forumusername;;race;;class

    ss << "playerinfo;;" << guid.GetCounter() << ";;";

    uint32 accId = pPlayerCache->uiAccount;
    std::string accName;
    GetName(accId, accName);
    ss << accName << ";;"
       << GetAccountIP(accId) << ";;"
       << pPlayerCache->uiLevel << ";;"
       << GetAccountEmail(accId) << ";;"
       << GetForumName(accId) << ";;"
       << pPlayerCache->uiRace << ";;"
       << pPlayerCache->uiClass;

    pPlayer->SendAddonMessage("GM_ADDON", ss.str());
}
