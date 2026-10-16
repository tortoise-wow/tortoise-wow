#include "GameObjectAI.h"
#include "AccountMgr.h"
#include "Anticheat.h"
#include "AsyncCommandHandlers.h"
#include "AuraRemovalMgr.h"
#include "AutoBroadCastMgr.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "CellImpl.h"
#include "CharacterDatabaseCache.h"
#include "Chat.h"
#include "Common.h"
#include "Config/Config.h"
#include "Creature.h"
#include "CreatureAI.h"
#include "CreatureEventAIMgr.h"
#include "CreatureGroups.h"
#include "DBCStores.h"
#include "DBCStructure.h"
#include "Database/DatabaseEnv.h"
#include "Database/DatabaseImpl.h"
#include "Formulas.h"
#include "GMTicketMgr.h"
#include "GameEventMgr.h"
#include "GameObject.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "GuidObjectScaling.h"
#include "HardcodedEvents.h"
#include "HonorMgr.h"
#include "InstanceData.h"
#include "Item.h"
#include "ItemEnchantmentMgr.h"
#include "Language.h"
#include "Log.h"
#include "LootMgr.h"
#include "Mail.h"
#include "MapManager.h"
#include "MapPersistentStateMgr.h"
#include "MassMailMgr.h"
#include "ModelInstance.h"
#include "MoveMap.h"                                       
#include "MoveSpline.h"
#include "MoveSplineInit.h"
#include "Nostalrius.h"
#include "ObjectAccessor.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "PathFinder.h"                                    
#include "Pet.h"
#include "Player.h"
#include "ChatTranslator.hpp"
#include "PointMovementGenerator.h"
#include "QuestDef.h"
#include "ScriptMgr.h"
#include "SpellMgr.h"
#include "SpellModMgr.h"
#include "SQLStorages.h"
#include "SystemConfig.h"
#include "TargetedMovementGenerator.h"
#include "TemporarySummon.h"
#include "Totem.h"
#include "Util.h"
#include "VMapFactory.h"
#include "WaypointManager.h"
#include "WaypointMovementGenerator.h"
#include "Weather.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "GuidObjectScaling.h"
#include "PlayerDump.h"
#include "revision.h"
#include "Auth/base32.h"
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string.h>
#include <typeinfo>
#include <regex>
#include <iomanip>
#include <sstream>
#include <ctime>
#include "Anticheat/Anticheat.h"
#include <ace/OS_NS_dirent.h>
#include "SuspiciousStatisticMgr.h"
#include "ChannelMgr.h"
#include "CommandStream.h"
#include "DynamicVisibilityMgr.h"
#include "CommandStream.h"
#include "TransmogMgr.h"
#include "PerfStats.h"
#include "PerformanceMonitor.h"
#include "../scripts/miscellaneous/npc_loothelper.h"


#ifdef ENABLE_LSAN
#include <sanitizer/lsan_interface.h>
#endif

int32 GetTokenBalance(uint32 accountId)
{
    QueryResult* result = LoginDatabase.PQuery("SELECT `coins` FROM `shop_coins` WHERE `id` = '%u'", accountId);

    int32 coins = 0;

    if (result)
    {
        Field* fields = result->Fetch();
        coins = fields[0].GetInt32();
        delete result;
    }
    else
    {
        LoginDatabase.PExecute("INSERT INTO shop_coins (id, coins) VALUES ('%u', 0)", accountId);
        return coins;
    }
    
    return coins;
}

struct LookupPlayerHandler
{
    static void HandleLookupPlayerIpResult(QueryResult* result, std::pair<uint32, uint32> dataPair)
    {
        const auto& [accountId, lim] = dataPair;
        uint32 limit = lim;

        auto session = sWorld.FindSession(accountId);

        if (session)
            ChatHandler(session).LookupPlayerSearchCommand(result, &limit);
        else
            delete result;
    }

    static void HandleBanInfoCallback(QueryResult* result, std::pair<uint32, std::string> dataPair)
    {
        const auto& [accountId, accountname] = dataPair;
        auto session = sWorld.FindSession(accountId);

        if (!session)
        {
            delete result;
            return;
        }

        auto handler = ChatHandler(session);

        if (!result)
        {
            handler.PSendSysMessage(LANG_BANINFO_NOACCOUNTBAN, accountname.c_str());
            return;
        }

        handler.PSendSysMessage(LANG_BANINFO_BANHISTORY, accountname.c_str());
        do
        {
            Field* fields = result->Fetch();

            time_t unbandate = time_t(fields[3].GetUInt64());
            bool active = false;
            if (fields[2].GetBool() && (fields[1].GetUInt64() == (uint64)0 || unbandate >= time(nullptr)))
                active = true;
            bool permanent = (fields[1].GetUInt64() == (uint64)0);
            uint32 reqGmLevel = fields[7].GetUInt8();
            std::string banreason = fields[4].GetString();
            if (reqGmLevel > uint8(handler.GetAccessLevel()))
                banreason = "<hidden>";

            std::string authorName = fields[5].GetCppString() + " (" + fields[6].GetCppString() + ")";
            std::string bantime = permanent ? handler.GetMangosString(LANG_BANINFO_INFINITE) : secsToTimeString(fields[1].GetUInt64(), true);
            std::string banInfoHistory = handler.GetMangosString(LANG_BANINFO_HISTORYENTRY);

            std::ostringstream ss;
            ss << "ID " << fields[8].GetUInt64() << " | ";

            std::string finalOutput = ss.str() + banInfoHistory;

            handler.PSendSysMessage(finalOutput.c_str(),
                fields[0].GetString(), bantime.c_str(), active ? handler.GetMangosString(LANG_BANINFO_YES) : handler.GetMangosString(LANG_BANINFO_NO), banreason.c_str(), authorName.c_str());
        } while (result->NextRow());

        delete result;
    }

    static void HandleMuteHistoryCallback(QueryResult* result, std::pair<uint32, std::string> dataPair)
    {

        const auto& [accountId, accountname] = dataPair;
        auto session = sWorld.FindSession(accountId);

        if (!session)
        {
            delete result;
            return;
        }

        auto handler = ChatHandler(session);



        if (!result)
        {
            handler.PSendSysMessage("Account %s has never been muted", accountname.c_str());
            return;
        }

        handler.PSendSysMessage("Mute history for account %s:", accountname.c_str());
        do
        {
            Field* fields = result->Fetch();

            time_t unmutedate = time_t(fields[2].GetUInt64());
            std::string mutereason = fields[3].GetString();
            std::string authorName = fields[4].GetCppString();
            std::string mutetime = secsToTimeString(fields[1].GetUInt64(), true);

            handler.PSendSysMessage("Mute Date: %s Mutetime: %s Still active: %s  Reason: %s Set by: %s",
                fields[0].GetString(),
                mutetime.c_str(),
                unmutedate > time(nullptr) ? handler.GetMangosString(LANG_BANINFO_YES) : handler.GetMangosString(LANG_BANINFO_NO),
                mutereason.c_str(),
                authorName.c_str());
        } while (result->NextRow());

        delete result;
    }
};

bool ChatHandler::HandleReloadMangosStringCommand(char* /*args*/)
{
    sObjectMgr.LoadMangosStrings();
    SendSysMessage("DB table `mangos_string` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadModuleStringCommand(char* /*args*/)
{
    sObjectMgr.LoadModuleStrings();
    SendSysMessage("DB tables `module_string` and `module_string_locale` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadHousingCommand(char* /*args*/)
{
    sObjectMgr.LoadGuildHouses();
    SendSysMessage("DB table `guild_house` reloaded.");
    return true;
}
bool ChatHandler::HandleReloadConfigCommand(char* /*args*/)
{
    sWorld.LoadConfigSettings(true);
    sAnticheatLib->Reload();
    SendSysMessage("World config settings reloaded.");
    return true;
}

bool ChatHandler::HandleReloadQuestTemplateCommand(char* /*args*/)
{
    sObjectMgr.LoadQuests();
    SendSysMessage("DB table `quest_template` (quest definitions) reloaded.");

    /// dependent also from `gameobject` but this table not reloaded anyway
    sObjectMgr.LoadGameObjectForQuests();
    SendSysMessage("Data GameObjects for quests reloaded.");
    return true;
}

bool ChatHandler::HandleAccountSetGmLevelCommand(char* args)
{
    char* accountStr = ExtractOptNotLastArg(&args);

    std::string targetAccountName;
    Player* targetPlayer = nullptr;
    uint32 targetAccountId = ExtractAccountId(&accountStr, &targetAccountName, &targetPlayer);
    if (!targetAccountId)
        return false;

    /// only target player different from self allowed
    if (GetAccountId() == targetAccountId)
        return false;

    int32 gm;
    if (!ExtractInt32(&args, gm))
        return false;

    if (gm < SEC_PLAYER || gm > SEC_ADMINISTRATOR)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    /// can set security level only for target with less security and to less security that we have
    /// This is also reject self apply in fact
    if (HasLowerSecurityAccount(nullptr, targetAccountId, true))
        return false;

    /// account can't set security to greater level, need more power GM or console
    AccountTypes plSecurity = GetAccessLevel();
    if (AccountTypes(gm) > plSecurity)
    {
        SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
        SetSentErrorMessage(true);
        return false;
    }

    if (targetPlayer)
    {
        ChatHandler(targetPlayer).PSendSysMessage(LANG_YOURS_SECURITY_CHANGED, GetNameLink().c_str(), gm);
        targetPlayer->GetSession()->SetSecurity(AccountTypes(gm));
    }

    PSendSysMessage(LANG_YOU_CHANGE_SECURITY, targetAccountName.c_str(), gm);
    sAccountMgr.SetSecurity(targetAccountId, AccountTypes(gm));

    return true;
}

/// Set password for account
bool ChatHandler::HandleAccountSetPasswordCommand(char* args)
{
    ///- Get the command line arguments
    char* szAccountName = ExtractQuotedOrLiteralArg(&args);
    if (!szAccountName)
    {
        SendSysMessage("Expected an account name.");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 targetAccountId = sAccountMgr.GetId(szAccountName);
    if (!targetAccountId)
    {
        PSendSysMessage("Cannot find account named '%s'.", szAccountName);
        SetSentErrorMessage(true);
        return false;
    }

    // allow or quoted string with possible spaces or literal without spaces
    char *szPassword1 = ExtractQuotedOrLiteralArg(&args);
    char *szPassword2 = ExtractQuotedOrLiteralArg(&args);
    if (!szPassword1 || !szPassword2)
        return false;

    /// can set password only for target with less security
    /// This is also reject self apply in fact
    if (HasLowerSecurityAccount(nullptr, targetAccountId, true))
        return false;

    if (strcmp(szPassword1, szPassword2))
    {
        SendSysMessage(LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage(true);
        return false;
    }

    AccountOpResult result = sAccountMgr.ChangePassword(targetAccountId, szPassword1);

    switch (result)
    {
        case AOR_OK:
            SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_NAME_NOT_EXIST:
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, szAccountName);
            SetSentErrorMessage(true);
            return false;
        case AOR_PASS_TOO_LONG:
            SendSysMessage(LANG_PASSWORD_TOO_LONG);
            SetSentErrorMessage(true);
            return false;
        default:
            SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            SetSentErrorMessage(true);
            return false;
    }

    return true;
}

bool ChatHandler::HandleAccountFaCommand(char* args)
{
    const std::string possibleChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::string account_name;
    uint32 targetAccountId = ExtractAccountId(&args, &account_name);
    if (!targetAccountId)
        return false;

    auto res = std::unique_ptr<QueryResult>(LoginDatabase.PQuery("SELECT `security` FROM `account` WHERE `id` = '%u'", targetAccountId));

    if (res && !res->Fetch()[0].GetCppString().empty())
    {
        PSendSysMessage("Account %s already has 2FA enabled. Token: %s.", account_name.c_str(), res->Fetch()[0].GetCppString().c_str());
        return false;
    }

    std::string randTokBuff;
    randTokBuff.reserve(32);
    for (uint32 i = 0; i < 32; ++i)
    {
        randTokBuff += possibleChars[urand(0, possibleChars.length() - 1)];
    }

    PSendSysMessage("Token: %s", randTokBuff.c_str());
    LoginDatabase.PExecute("UPDATE `account` SET `security` = '%s', `locked` = 2 WHERE id = '%u'", randTokBuff.c_str(), targetAccountId);
    return true;
}

bool ChatHandler::HandleMaxSkillCommand(char* /*args*/)
{
    Player* SelectedPlayer = GetSelectedPlayer();
    if (!SelectedPlayer)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // each skills that have max skill value dependent from level seted to current level max skill value
    SelectedPlayer->UpdateSkillsToMaxSkillsForLevel();
    return true;
}

bool ChatHandler::HandleSetSkillCommand(char* args)
{
    Player * target = GetSelectedPlayer();
    if (!target)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hskill:skill_id|h[name]|h|r
    char* skill_p = ExtractKeyFromLink(&args, "Hskill");
    if (!skill_p)
        return false;

    int32 skill;
    if (!ExtractInt32(&skill_p, skill))
        return false;

    int32 level;
    if (!ExtractInt32(&args, level))
        return false;

    int32 maxskill;
    if (!ExtractOptInt32(&args, maxskill, target->GetSkillMaxPure(skill)))
        return false;

    if (skill <= 0)
    {
        PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
        SetSentErrorMessage(true);
        return false;
    }

    SkillLineEntry const* sl = sSkillLineStore.LookupEntry(skill);
    if (!sl)
    {
        PSendSysMessage(LANG_INVALID_SKILL_ID, skill);
        SetSentErrorMessage(true);
        return false;
    }

    std::string tNameLink = GetNameLink(target);

    if (!target->GetSkillValue(skill))
    {
        PSendSysMessage(LANG_SET_SKILL_ERROR, tNameLink.c_str(), skill, sl->name[GetSessionDbcLocale()]);
        SetSentErrorMessage(true);
        return false;
    }

    if (level <= 0 || level > maxskill || maxskill <= 0)
        return false;

    target->SetSkill(skill, level, maxskill);
    PSendSysMessage(LANG_SET_SKILL, skill, sl->name[GetSessionDbcLocale()], tNameLink.c_str(), level, maxskill);

    return true;
}

bool ChatHandler::HandleUnLearnCommand(char* args)
{
    if (!*args)
        return false;

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
    uint32 spell_id = ExtractSpellIdFromLink(&args);
    if (!spell_id)
        return false;

    bool allRanks = ExtractLiteralArg(&args, "all") != nullptr;
    if (!allRanks && *args)                                 // can be fail also at syntax error
        return false;

    Player* target = GetSelectedPlayer();
    if (!target)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (allRanks)
        spell_id = sSpellMgr.GetFirstSpellInChain(spell_id);

    if (target->HasSpell(spell_id))
    {
        target->RemoveSpell(spell_id, false, !allRanks);
        PSendSysMessage("Successfully unlearned spell ID %u from player %s.", spell_id, target->GetName());
    }
    else
        SendSysMessage(LANG_FORGET_SPELL);

    return true;
}

bool ChatHandler::HandleUnLearnOfflineCommand(char* args)
{
    if (!*args)
        return false;

    Player* target;
    ObjectGuid target_guid;
    std::string target_name;

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
    uint32 spell_id = ExtractSpellIdFromLink(&args);
    if (!spell_id)
        return false;

    bool allRanks = ExtractLiteralArg(&args, "all") != nullptr;
    if (!allRanks)                                 // can be fail also at syntax error
        return false;

    ExtractPlayerTarget(&args, &target, &target_guid, &target_name);

    if (target == NULL && target_guid)
    {
        QueryResult* result = CharacterDatabase.PQuery("SELECT * FROM `character_spell` WHERE `guid` = %u AND `spell` = %u", target_guid, spell_id);

        if (result)
        {
            CharacterDatabase.PExecute("DELETE FROM `character_spell` WHERE `guid` = %u AND `spell` = %u", target_guid, spell_id);
            PSendSysMessage("Spell %u removed from OFFLINE player %s", spell_id, target_name.c_str());
            delete result;
        }
        else
            SendSysMessage(LANG_FORGET_SPELL);
    }
    else if (target)
    {
        if (allRanks)
            spell_id = sSpellMgr.GetFirstSpellInChain(spell_id);

        if (target->HasSpell(spell_id))
        {
            target->RemoveSpell(spell_id, false, !allRanks);
            PSendSysMessage("Spell %u removed from player %s", spell_id, target->GetName());
        }
        else
            SendSysMessage(LANG_FORGET_SPELL);
    }

    return true;
}

bool ChatHandler::HandleCooldownCommand(char* args)
{
    Unit* target = GetSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    std::string tNameLink = target->ToPlayer() ? GetNameLink(target->ToPlayer()) : target->GetName();

    if (!*args)
    {
        target->RemoveAllSpellCooldown();
        PSendSysMessage(LANG_REMOVEALL_COOLDOWN, tNameLink.c_str());
    }
    else
    {
        // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
        uint32 spell_id = ExtractSpellIdFromLink(&args);
        if (!spell_id)
            return false;

        if (!sSpellMgr.GetSpellEntry(spell_id))
        {
            PSendSysMessage(LANG_UNKNOWN_SPELL, target == m_session->GetPlayer() ? GetMangosString(LANG_YOU) : tNameLink.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        target->RemoveSpellCooldown(spell_id, true);
        PSendSysMessage(LANG_REMOVE_COOLDOWN, spell_id, target == m_session->GetPlayer() ? GetMangosString(LANG_YOU) : tNameLink.c_str());
    }
    return true;
}

bool ChatHandler::HandleLearnAllMySpellsCommand(char* /*args*/)
{
    Player* pPlayer = m_session->GetPlayer();
    ChrClassesEntry const* clsEntry = sChrClassesStore.LookupEntry(pPlayer->GetClass());

    if (!clsEntry)
        return true;

    uint32 family = clsEntry->spellfamily;

    for (uint32 i = 0; i < sObjectMgr.GetMaxSkillLineAbilityId(); ++i)
    {
        SkillLineAbilityEntry const *entry = sObjectMgr.GetSkillLineAbility(i);
        if (!entry)
            continue;

        SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(entry->spellId);
        if (!spellInfo)
            continue;

        // skip server-side/triggered spells
        if (spellInfo->spellLevel == 0)
            continue;

        // skip wrong class/race skills
        if (!pPlayer->IsSpellFitByClassAndRace(spellInfo->Id))
            continue;

        // skip other spell families
        if (spellInfo->SpellFamilyName != family)
            continue;

        // skip spells with first rank learned as talent (and all talents then also)
        uint32 first_rank = sSpellMgr.GetFirstSpellInChain(spellInfo->Id);
        if (GetTalentSpellCost(first_rank) > 0)
            continue;

        // skip broken spells
        if (!SpellMgr::IsSpellValid(spellInfo, pPlayer, false))
            continue;

        pPlayer->LearnSpell(spellInfo->Id, false);
    }

    SendSysMessage(LANG_COMMAND_LEARN_CLASS_SPELLS);
    return true;
}

SkillLineEntry const* ChatHandler::FindSkillLineEntryFromProfessionName(char* args, std::string& nameOut)
{
    if (!*args)
        return nullptr;

    std::wstring wnamepart;

    if (!Utf8toWStr(args, wnamepart))
        return nullptr;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    SkillLineEntry const* targetSkillInfo = nullptr;
    for (uint32 i = 1; i < sSkillLineStore.GetNumRows(); ++i)
    {
        SkillLineEntry const* skillInfo = sSkillLineStore.LookupEntry(i);
        if (!skillInfo)
            continue;

        if (skillInfo->categoryId != SKILL_CATEGORY_PROFESSION && skillInfo->categoryId != SKILL_CATEGORY_SECONDARY)
            continue;

        int loc = GetSessionDbcLocale();
        nameOut = skillInfo->name[loc];
        if (nameOut.empty())
            continue;

        if (!Utf8FitTo(nameOut, wnamepart))
        {
            loc = 0;
            for (; loc < MAX_DBC_LOCALE; ++loc)
            {
                if (loc == GetSessionDbcLocale())
                    continue;

                nameOut = skillInfo->name[loc];
                if (nameOut.empty())
                    continue;

                if (Utf8FitTo(nameOut, wnamepart))
                    break;
            }
        }

        if (loc < MAX_DBC_LOCALE)
        {
            targetSkillInfo = skillInfo;
            break;
        }
    }

    return targetSkillInfo;
}

bool ChatHandler::HandleLearnAllRecipesCommand(char* args)
{
    //  Learns all recipes of specified profession and sets skill to max
    //  Example: .learn all_recipes enchanting

    Player* target = GetSelectedPlayer();
    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        return false;
    }

    std::string name;
    SkillLineEntry const* targetSkillInfo = FindSkillLineEntryFromProfessionName(args, name);
    if (!targetSkillInfo)
        return false;

    HandleLearnSkillRecipesHelper(target, targetSkillInfo->id);

    uint16 maxLevel = target->GetSkillMaxPure(targetSkillInfo->id);
    target->SetSkill(targetSkillInfo->id, maxLevel, maxLevel);
    PSendSysMessage(LANG_COMMAND_LEARN_ALL_RECIPES, name.c_str());
    return true;
}

bool ChatHandler::HandleLearnAllTrainerCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();
    
    uint32 trainerId;
    if (ExtractUInt32(&args, trainerId))
    {
        if (TrainerSpellData const* tSpells = sObjectMgr.GetNpcTrainerTemplateSpells(trainerId))
            HandleLearnTrainerHelper(pPlayer, tSpells);
        else
        {
            SendSysMessage("Trainer template not found!");
            SetSentErrorMessage(true);
            return false;
        }
    }
    else
    {
        std::set<uint32> checkedTrainerTemplates;
        for (auto const& itr : sObjectMgr.GetCreatureInfoMap())
        {
            CreatureInfo const* cInfo = itr.second.get();
            if (!cInfo)
                continue;

            if (!(cInfo->npc_flags & UNIT_NPC_FLAG_TRAINER))
                continue;

            switch (cInfo->trainer_type)
            {
                case TRAINER_TYPE_CLASS:
                {
                    if (cInfo->trainer_class != pPlayer->GetClass())
                        continue;
                    break;
                }
                case TRAINER_TYPE_PETS:
                {
                    if (pPlayer->GetClass() != CLASS_HUNTER)
                        continue;
                    break;
                }
            }

            if (TrainerSpellData const* cSpells = sObjectMgr.GetNpcTrainerSpells(itr.first))
                HandleLearnTrainerHelper(pPlayer, cSpells);

            if (trainerId = cInfo->trainer_id) // assignment
            {
                if (checkedTrainerTemplates.find(trainerId) != checkedTrainerTemplates.end())
                    continue;

                checkedTrainerTemplates.insert(trainerId);
                if (TrainerSpellData const* tSpells = sObjectMgr.GetNpcTrainerTemplateSpells(trainerId))
                    HandleLearnTrainerHelper(pPlayer, tSpells);
            }
        }
    }

    SendSysMessage("Learned all available spells from trainers.");
    return true;
}

bool ChatHandler::HandleLearnAllItemsCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();

    for (auto const& itr : sObjectMgr.GetItemPrototypeMap())
    {
        ItemPrototype const* pProto = &itr.second;
        if (!pProto)
            continue;

        if (pProto->ExtraFlags & ITEM_EXTRA_NOT_OBTAINABLE)
            continue;

        SpellEntry const* pLearnSpell = nullptr;
        for (uint32 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            if (!pProto->Spells[i].SpellId)
                continue;

            if (pProto->Spells[i].SpellTrigger != ITEM_SPELLTRIGGER_ON_USE)
                continue;

            SpellEntry const* pSpellEntry = sSpellMgr.GetSpellEntry(pProto->Spells[i].SpellId);
            if (pSpellEntry->HasEffect(SPELL_EFFECT_LEARN_SPELL))
                pLearnSpell = pSpellEntry;

            // items have only one on use effect
            break;
        }

        if (!pLearnSpell)
            continue;
        
        if (pPlayer->CanUseItem(pProto) != EQUIP_ERR_OK)
            continue;

        for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            uint32 spellId = pLearnSpell->EffectTriggerSpell[i];
            if (spellId && pLearnSpell->Effect[i] == SPELL_EFFECT_LEARN_SPELL)
            {
                SkillLineAbilityMapBounds bounds = sSpellMgr.GetSkillLineAbilityMapBoundsBySpellId(spellId);
                if (bounds.first == bounds.second)
                    continue;

                pPlayer->LearnSpell(spellId, false);
            }
        }
    }

    SendSysMessage("Learned all available spells from items.");
    return true;
}

void ChatHandler::HandleLearnTrainerHelper(Player* player, TrainerSpellData const* tSpells)
{
    // spells are not in rank order, so we need to do multiple loops to learn everything
    bool learnedAnything;
    do
    {
        learnedAnything = false;
        for (const auto& itr : tSpells->spellList)
        {
            TrainerSpell const* tSpell = &itr.second;

            TrainerSpellState state = player->GetTrainerSpellState(tSpell);
            if (state != TRAINER_SPELL_GREEN)
                continue;

            for (auto const& spellId : sSpellMgr.GetSpellEntry(tSpell->spell)->EffectTriggerSpell)
            {
                if (!spellId)
                    continue;

                if (sSpellMgr.IsPrimaryProfessionFirstRankSpell(spellId))
                    continue;

                if (!player->IsSpellFitByClassAndRace(spellId))
                    continue;

                player->LearnSpell(spellId, false);
                learnedAnything = true;
            }
        }
    } while (learnedAnything);
}

bool ChatHandler::HandleLearnCommand(char* args)
{
    Player* player = m_session->GetPlayer();
    Player* targetPlayer = GetSelectedPlayer();

    if (!targetPlayer)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = ExtractSpellIdFromLink(&args);
    if (!spell || !sSpellMgr.GetSpellEntry(spell))
        return false;

    bool allRanks = ExtractLiteralArg(&args, "all") != nullptr;
    if (!allRanks && *args)                                 // can be fail also at syntax error
        return false;

    bool force = ExtractLiteralArg(&args, "force") != nullptr;

    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spell);
    if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
        SetSentErrorMessage(true);
        return false;
    }


    bool learn = (spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL);

    if (!allRanks && targetPlayer->HasSpell(spell))
    {
        if (targetPlayer == player)
            SendSysMessage(LANG_YOU_KNOWN_SPELL);
        else
            PSendSysMessage(LANG_TARGET_KNOWN_SPELL, targetPlayer->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    if (player != targetPlayer && !force && learn)
    {
        SendSysMessage("Can't .learn a learn-type spell to a player. Use .learn <spellid> force if you want to override this behavior.");
        return true;
    }

    if (allRanks)
        targetPlayer->LearnSpellHighRank(spell);
    else
        targetPlayer->LearnSpell(spell, false);

    return true;
}


bool ChatHandler::HandleListAurasCommand(char* /*args*/)
{
    Unit* unit = GetSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    char const* talentStr = GetMangosString(LANG_TALENT);
    char const* passiveStr = GetMangosString(LANG_PASSIVE);

    Unit::SpellAuraHolderMap const& uAuras = unit->GetSpellAuraHolderMap();
    PSendSysMessage(LANG_COMMAND_TARGET_LISTAURAS, uAuras.size());
    for (Unit::SpellAuraHolderMap::const_iterator itr = uAuras.begin(); itr != uAuras.end(); ++itr)
    {
        bool talent = GetTalentSpellCost(itr->second->GetId()) > 0;

        SpellAuraHolder* holder = itr->second;
        std::string name = holder->GetSpellProto()->SpellName[GetSessionDbcLocale()];

        for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            Aura* aur = holder->GetAuraByEffectIndex(SpellEffectIndex(i));
            if (!aur)
                continue;

            if (m_session)
            {
                std::ostringstream ss_name;
                ss_name << "|cffffffff|Hspell:" << itr->second->GetId() << "|h[" << name << "]|h|r";

                PSendSysMessage(LANG_COMMAND_TARGET_AURADETAIL, holder->GetId(), aur->GetEffIndex(),
                    aur->GetModifier()->m_auraname, aur->GetAuraDuration(), aur->GetAuraMaxDuration(), aur->GetAuraPeriodicTimer(), aur->GetStackAmount(),
                    ss_name.str().c_str(),
                    (holder->IsPassive() ? passiveStr : ""), (talent ? talentStr : ""),
                    holder->GetCasterGuid().GetString().c_str());
            }
            else
            {
                PSendSysMessage(LANG_COMMAND_TARGET_AURADETAIL, holder->GetId(), aur->GetEffIndex(),
                    aur->GetModifier()->m_auraname, aur->GetAuraDuration(), aur->GetAuraMaxDuration(), aur->GetAuraPeriodicTimer(), aur->GetStackAmount(),
                    name.c_str(),           // std::string through a printf vararg is an error under clang
                    (holder->IsPassive() ? passiveStr : ""), (talent ? talentStr : ""),
                    holder->GetCasterGuid().GetString().c_str());
            }
        }
    }

    return true;
}

bool ChatHandler::HandleListCreatureCommand(char* args)
{
    // number or [name] Shift-click form |color|Hcreature_entry:creature_id|h[name]|h|r
    uint32 cr_id;
    if (!ExtractUint32KeyFromLink(&args, "Hcreature_entry", cr_id))
        return false;

    if (!cr_id)
    {
        PSendSysMessage(LANG_COMMAND_INVALIDCREATUREID, cr_id);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(cr_id);
    if (!cInfo)
    {
        PSendSysMessage(LANG_COMMAND_INVALIDCREATUREID, cr_id);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 count;
    if (!ExtractOptUInt32(&args, count, 10))
        return false;

    QueryResult* result;

    uint32 cr_count = 0;
    result = WorldDatabase.PQuery("SELECT COUNT(guid) FROM creature WHERE id='%u'", cr_id);
    if (result)
    {
        cr_count = (*result)[0].GetUInt32();
        delete result;
    }

    if (m_session)
    {
        Player* pl = m_session->GetPlayer();
        result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM creature WHERE id = '%u' ORDER BY order_ ASC LIMIT %u",
            pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), cr_id, uint32(count));
    }
    else
        result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map FROM creature WHERE id = '%u' LIMIT %u",
            cr_id, uint32(count));

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            float x = fields[1].GetFloat();
            float y = fields[2].GetFloat();
            float z = fields[3].GetFloat();
            int mapid = fields[4].GetUInt16();

            if (m_session)
                PSendSysMessage(LANG_CREATURE_LIST_CHAT, guid, PrepareStringNpcOrGoSpawnInformation<Creature>(guid).c_str(), guid, cInfo->name.c_str(), x, y, z, mapid);
            else
                PSendSysMessage(LANG_CREATURE_LIST_CONSOLE, guid, PrepareStringNpcOrGoSpawnInformation<Creature>(guid).c_str(), cInfo->name.c_str(), x, y, z, mapid);
        } while (result->NextRow());

        delete result;
    }

    PSendSysMessage(LANG_COMMAND_LISTCREATUREMESSAGE, cr_id, cr_count);
    return true;
}

bool ChatHandler::HandleListExploredAreasCommand(char* args)
{
    Player* pPlayer = GetSelectedPlayer();
    if (!pPlayer)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Listing explored areas by %s", pPlayer->GetName());
    for (auto itr = sAreaStorage.begin<AreaEntry>(); itr < sAreaStorage.end<AreaEntry>(); ++itr)
    {
        std::string name = itr->Name;
        sObjectMgr.GetAreaLocaleString(itr->Id, GetSessionDbLocaleIndex(), &name);

        if (!itr->ExploreFlag || itr->ExploreFlag == 0xffff)
            continue;;

        int offset = itr->ExploreFlag / 32;
        if (offset >= PLAYER_EXPLORED_ZONES_SIZE)
            continue;

        uint32 val = (uint32)(1 << (itr->ExploreFlag % 32));
        uint32 currFields = pPlayer->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
        if (currFields & val)
        {
            int locale = GetSessionDbLocaleIndex() + 1;
            // send area in "id - [name]" format
            std::ostringstream ss;
            if (m_session)
                ss << itr->Id << " - |cffffffff|Harea:" << itr->Id << "|h[" << name << " " << localeNames[locale] << "]|h|r";
            else
                ss << itr->Id << " - " << name << " " << localeNames[locale];

            SendSysMessage(ss.str().c_str());
        }
    }
    return true;
}

bool ChatHandler::HandleListObjectCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject_entry:go_id|h[name]|h|r
    uint32 go_id;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject_entry", go_id))
        return false;

    if (!go_id)
    {
        PSendSysMessage(LANG_COMMAND_LISTOBJINVALIDID, go_id);
        SetSentErrorMessage(true);
        return false;
    }

    GameObjectInfo const* gInfo = sObjectMgr.GetGameObjectInfo(go_id);
    if (!gInfo)
    {
        PSendSysMessage(LANG_COMMAND_LISTOBJINVALIDID, go_id);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 count;
    if (!ExtractOptUInt32(&args, count, 10))
        return false;

    QueryResult* result;

    uint32 obj_count = 0;
    result = WorldDatabase.PQuery("SELECT COUNT(guid) FROM gameobject WHERE id='%u'", go_id);
    if (result)
    {
        obj_count = (*result)[0].GetUInt32();
        delete result;
    }

    if (m_session)
    {
        Player* pl = m_session->GetPlayer();
        result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM gameobject WHERE id = '%u' ORDER BY order_ ASC LIMIT %u",
            pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), go_id, uint32(count));
    }
    else
        result = WorldDatabase.PQuery("SELECT guid, position_x, position_y, position_z, map FROM gameobject WHERE id = '%u' LIMIT %u",
            go_id, uint32(count));

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            float x = fields[1].GetFloat();
            float y = fields[2].GetFloat();
            float z = fields[3].GetFloat();
            int mapid = fields[4].GetUInt16();

            if (m_session)
                PSendSysMessage(LANG_GO_LIST_CHAT, guid, PrepareStringNpcOrGoSpawnInformation<GameObject>(guid).c_str(), guid, gInfo->name.c_str(), x, y, z, mapid);
            else
                PSendSysMessage(LANG_GO_LIST_CONSOLE, guid, PrepareStringNpcOrGoSpawnInformation<GameObject>(guid).c_str(), gInfo->name.c_str(), x, y, z, mapid);
        } while (result->NextRow());

        delete result;
    }

    PSendSysMessage(LANG_COMMAND_LISTOBJMESSAGE, go_id, obj_count);
    return true;
}

bool ChatHandler::HandleListDestroyedItemsCommand(char* args)
{
    std::string name = args;

    normalizePlayerName(name);
    ObjectGuid guid = sObjectMgr.GetPlayerGuidByName(name);
    if (guid.IsEmpty())
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT `item_entry`, `stack_count`, `time` FROM `character_destroyed_items` WHERE `player_guid`=%u ORDER BY `time` LIMIT 128", guid.GetCounter()));
    if (!result)
    {
        SendSysMessage(LANG_COMMAND_NOITEMFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Destroyed items for %s:\n", args);
    do
    {
        Field* fields = result->Fetch();
        uint32 itemId = fields[0].GetUInt32();
        uint32 stackCount = fields[1].GetUInt32();
        time_t destroyedTime = fields[2].GetUInt64();

        std::string stacksStr = stackCount > 1 ? " x " + std::to_string(stackCount) : "";
        std::string timeStr = secsToTimeString(sWorld.GetGameTime() - destroyedTime, true);

        ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(itemId);
        if (!pProto)
        {
            PSendSysMessage("%u - [UNKNOWN]%s (%s ago)", itemId, stacksStr.c_str(), timeStr.c_str());
            continue;
        }

        if (m_session)
        {
            uint32 color = ItemQualityColors[pProto->Quality];
            std::ostringstream itemStr;
            itemStr << "|c" << std::hex << color << "|Hitem:" << std::to_string(pProto->ItemId) << ":0:0:0:0:0:0:0|h[" << pProto->Name1 << "]|h|r";
            PSendSysMessage("%u - %s%s (%s ago)", itemId, itemStr.str().c_str(), stacksStr.c_str(), timeStr.c_str());
        }
        else
            PSendSysMessage("%u - %s%s (%s ago)", itemId, pProto->Name1.c_str(), stacksStr.c_str(), timeStr.c_str());

    } while (result->NextRow());

    return true;
}

bool ChatHandler::HandleListBuybackItemsCommand(char* args)
{
    std::string name = args;
    Player* target;
    ObjectGuid guid;
    std::string target_name;

    if (!ExtractPlayerTarget(&args, &target, &guid, &target_name, true))
        return false;

    if (guid.IsEmpty())
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT `item_template` FROM `character_inventory` WHERE `guid`=%u && `slot` BETWEEN %u and %u", guid.GetCounter(), BUYBACK_SLOT_START, BUYBACK_SLOT_END - 1));
    if (!result)
    {
        SendSysMessage(LANG_COMMAND_NOITEMFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Buyback items for %s:\n", args);
    do
    {
        Field* fields = result->Fetch();
        uint32 itemId = fields[0].GetUInt32();

        ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(itemId);
        if (!pProto)
        {
            PSendSysMessage("%u - [UNKNOWN]", itemId);
            continue;
        }

        if (m_session)
        {
            uint32 color = ItemQualityColors[pProto->Quality];
            std::ostringstream itemStr;
            itemStr << "|c" << std::hex << color << "|Hitem:" << std::to_string(pProto->ItemId) << ":0:0:0:0:0:0:0|h[" << pProto->Name1 << "]|h|r";
            PSendSysMessage(LANG_ITEM_LIST_CONSOLE, itemId, itemStr.str().c_str(), "");
        }
        else
            PSendSysMessage(LANG_ITEM_LIST_CONSOLE, itemId, pProto->Name1.c_str(), "");

    } while (result->NextRow());

    return true;
}

bool ChatHandler::HandleListHostileRefsCommand(char* /*args*/)
{
    Unit* pUnit = GetSelectedUnit();
    if (!pUnit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("List of hostile references for %s:", pUnit->GetObjectGuid().GetString().c_str());
    HostileReference* pReference = pUnit->GetHostileRefManager().getFirst();
    uint32 counter = 1;
    while (pReference)
    {
        if (Unit* pTarget = pReference->getSourceUnit())
            PSendSysMessage("%u. %s", counter++, pTarget->GetGuidStr().c_str());
        pReference = pReference->next();
    }

    return true;
}

bool ChatHandler::HandleListThreatCommand(char* /*args*/)
{
    Unit* pUnit = GetSelectedUnit();
    if (!pUnit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Threat list for %s:", pUnit->GetObjectGuid().GetString().c_str());
    ThreatList const& threatList = pUnit->GetThreatManager().getThreatList();
    for (auto const& itr : threatList)
    {
        PSendSysMessage("%g - %s", itr->getThreat(), itr->getUnitGuid().GetString().c_str());
    }

    return true;
}

bool ChatHandler::ListBattlegroundsCommand(char* args)
{
    SendSysMessage("List of Active Battlegrounds:");
    std::function<void(const BattleGround*)> appl = [this](const BattleGround* bg)
        {
            if (!bg->GetInstanceID())
                return; // template BG, don't list.

            auto plTargetGuid = bg->GetPlayers().size() > 0 ? bg->GetPlayers().begin()->first : ObjectGuid{};
            auto plTarget = sObjectAccessor.FindPlayer(plTargetGuid);

            std::ostringstream ss;
            ss << BattleGround::TypeToString(bg->GetTypeID()) << " | " << bg->GetPlayers().size() << " / " << bg->GetMaxPlayers() << " | Status: " << bg->GetStatus() << " | Duration: " <<
                secsToTimeString(bg->GetStartTime() / 1000);

            if (plTarget)
                ss << " | TP Target : " << playerLink(plTarget->GetName());
            PSendSysMessage("%s", ss.str().c_str());
        };

    sBattleGroundMgr.ApplyAllBattleGrounds(appl);
    return true;
}

bool ChatHandler::HandleListMapsCommand(char* args)
{
    const auto& allMaps = sMapMgr.Maps();

    uint32 continentMaps = 0, instanceMaps = 0, battlegroundMaps = 0;

    PSendSysMessage("All maps currently active:");
    for (const auto& [mapId, map] : allMaps)
    {
        auto mapEntry = sMapStorage.LookupEntry<MapEntry>(mapId.nMapId);
        if (mapEntry)
        {
            PSendSysMessage("(ID %u, instance %u) %s", mapId.nMapId, mapId.nInstanceId, mapEntry->name);

            if (mapEntry->IsBattleGround())
                ++battlegroundMaps;
            else if (mapEntry->IsRaid() || mapEntry->IsDungeon())
                ++instanceMaps;
            else
                ++continentMaps;
        }
    }

    PSendSysMessage("Continent maps: %u, battleground maps: %u, instance maps: %u", continentMaps, battlegroundMaps, instanceMaps);

    return true;
}

bool ChatHandler::HandleAddItemCommand(char* args)
{
    char* cId = ExtractKeyFromLink(&args, "Hitem");
    if (!cId)
        return false;

    uint32 itemId = 0;
    if (!ExtractUInt32(&cId, itemId))                       // [name] manual form
    {
        std::string itemName = cId;
        WorldDatabase.escape_string(itemName);
        QueryResult *result = WorldDatabase.PQuery("SELECT entry FROM item_template WHERE name = '%s'", itemName.c_str());
        if (!result)
        {
            PSendSysMessage(LANG_COMMAND_COULDNOTFIND, cId);
            SetSentErrorMessage(true);
            return false;
        }
        itemId = result->Fetch()->GetUInt16();
        delete result;
    }

    int32 count;
    if (!ExtractOptInt32(&args, count, 1))
        return false;

    Player* pl = m_session->GetPlayer();
    Player* plTarget = GetSelectedPlayer();
    if (!plTarget)
        plTarget = pl;

    DETAIL_LOG(GetMangosString(LANG_ADDITEM), itemId, count);

    ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(itemId);
    if (!pProto)
    {
        PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, itemId);
        SetSentErrorMessage(true);
        return false;
    }

    //Subtract
    if (count < 0)
    {
        // Is the item actually?
        if (!plTarget->HasItemCount(itemId, -count, true))
        {
            PSendSysMessage("Player has %ux%u. Can take away %u. Bank included.", itemId, plTarget->GetItemCount(itemId, true), -count);
            SetSentErrorMessage(true);
            return false;
        }
        plTarget->DestroyItemCount(itemId, -count, true, false);
        PSendSysMessage(LANG_REMOVEITEM, itemId, -count, GetNameLink(plTarget).c_str());
        return true;
    }

    //Adding items
    uint32 noSpaceForCount = 0;

    // check space and find places
    ItemPosCountVec dest;
    uint8 msg = plTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, count, &noSpaceForCount);
    if (msg != EQUIP_ERR_OK)                                // convert to possible store amount
        count -= noSpaceForCount;

    if (count == 0 || dest.empty())                         // can't add any
    {
        PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);
        SetSentErrorMessage(true);
        return false;
    }

    Item* item = plTarget->StoreNewItem(dest, itemId, true, Item::GenerateItemRandomPropertyId(itemId));

    // remove binding (let GM give it to another player later)
    if (pl == plTarget)
        for (const auto& itr : dest)
            if (Item* item1 = pl->GetItemByPos(itr.pos))
                item1->SetBinding(false);

    if (count > 0 && item)
    {
        pl->SendNewItem(item, count, false, true);
        if (pl != plTarget)
            plTarget->SendNewItem(item, count, true, false);
    }

    if (noSpaceForCount > 0)
        PSendSysMessage(LANG_ITEM_CANNOT_CREATE, itemId, noSpaceForCount);

    return true;
}

bool ChatHandler::HandleCleanInventoryCommand(char* args)
{
    auto player = GetPlayer();

    if (player)
    {
        player->ApplyForAllItems([player](Item* item)
            {
                if (item->IsBag())
                    return;

                if (item->IsEquipped())
                    return;
                player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
            });
    }
    return true;
}

bool ChatHandler::HandleShowBordersCommand(char* args)
{
    auto player = GetPlayer();
    uint32 mapId = player->GetMapId();
    auto map = player->GetMap();
    auto points = sMapMgr.GetBorderPoints(mapId);

    std::sort(points.begin(), points.end(), [pl = GetPlayer()](const auto& pt1, const auto& pt2)
        {
            return pl->GetDistance2d(Position(pt1.first, pt1.second, 0, 0)) < pl->GetDistance2d(Position(pt2.first, pt2.second, 0, 0));
        });

    //find 2 closest ones and draw a border between them.
    auto pt1 = points[0];
    auto pt2 = points[1];

    player->SummonCreature(1, pt1.first, pt1.second, map->GetHeight(pt1.first, pt1.second, MAX_HEIGHT), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
    player->SummonCreature(1, pt2.first, pt2.second, map->GetHeight(pt2.first, pt2.second, MAX_HEIGHT), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);

    auto coords = pt1;
    for (int i = 0; i < 20; ++i)
    {
        coords.first += (pt2.first - pt1.first) / 20;
        coords.second += (pt2.second - pt1.second) / 20;
        player->SummonCreature(1, coords.first, coords.second, map->GetHeight(coords.first, coords.second, MAX_HEIGHT), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
    }

    PSendSysMessage("Closest border point : X: %f, Y: %f", pt1.first, pt1.second);
    return true;
}

bool ChatHandler::HandleDeleteItemCommand(char* args)
{
    char* cId = ExtractKeyFromLink(&args, "Hitem");

    if (!cId)
    {
        return false;
    }

    uint32 itemId = 0;

    if (!ExtractUInt32(&cId, itemId))
    {
        std::string itemName = cId;
        WorldDatabase.escape_string(itemName);

        std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT entry FROM item_template WHERE name = '%s'", itemName.c_str()));

        if (!result)
        {
            PSendSysMessage(LANG_COMMAND_COULDNOTFIND, cId);
            SetSentErrorMessage(true);
            return false;
        }

        itemId = result->Fetch()->GetUInt16();
    }

    uint32 count;

    if (!ExtractOptUInt32(&args, count, 1))
    {
        return false;
    }

    uint32 countCopy = count;

    Player* player;
    ObjectGuid target_guid;
    std::string target_name;

    if (!ExtractPlayerTarget(&args, &player, &target_guid, &target_name))
    {
        return false;
    }

    DETAIL_LOG(GetMangosString(LANG_REMOVEITEM), itemId, count);

    if (player)
    {
        if (!player->HasItemCount(itemId, count, true))
        {
            PSendSysMessage("Cannot remove %u instances of %u - maximum value is %u", count, itemId, player->GetItemCount(itemId, true));
            SetSentErrorMessage(true);
            return false;
        }

        player->DestroyItemCount(itemId, count, true, false, true);
        player->SaveInventoryAndGoldToDB();
    }
    else
    {
        std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery(
            "SELECT SUM(count) AS item_count FROM item_instance ii WHERE itemEntry = %u and owner_guid = %u",
            itemId, target_guid.GetCounter()
        ));

        uint32 maxItemCount = 0;

        if (result)
        {
            auto fields = result->Fetch();
            maxItemCount = fields[0].GetUInt32();
        }

        if (count > maxItemCount)
        {
            PSendSysMessage("Cannot remove %u instances of %u - maximum value is %u", count, itemId, maxItemCount);
            SetSentErrorMessage(true);
            return false;
        }

        while (count)
        {
            result.reset(CharacterDatabase.PQuery(
                "SELECT guid, count FROM item_instance ii WHERE itemEntry = %u and owner_guid = %u ORDER BY count DESC",
                itemId, target_guid.GetCounter()
            ));

            if (!result)
            {
                SendSysMessage("Encountered an error while attempting to locate items to remove - race condition?");
                SetSentErrorMessage(true);
                return false;
            }

            auto fields = result->Fetch();
            auto guid = fields[0].GetUInt32();
            auto stackCount = fields[1].GetUInt32();

            if (stackCount > count) // make sure we don't delete more than requested
            {
                if (!CharacterDatabase.PExecute("UPDATE item_instance SET count = %u WHERE guid = %u",
                    stackCount - count, guid))
                {
                    SendSysMessage("Encountered an error while attempting to adjust item stack count");
                    SetSentErrorMessage(true);
                    return false;
                }

                break;
            }
            else
            {
                if (!CharacterDatabase.DirectPExecute("DELETE FROM item_instance WHERE guid = %u", guid))
                {
                    SendSysMessage("Encountered an error while attempting to remove item instance");
                    SetSentErrorMessage(true);
                    return false;
                }
                
                if (!CharacterDatabase.DirectPExecute("DELETE FROM character_inventory WHERE item = %u", guid))
                {
                    SendSysMessage("Encountered an error while attempting to remove item from inventory");
                    SetSentErrorMessage(true);
                    return false;
                }

                if (!CharacterDatabase.DirectPExecute("DELETE FROM character_gifts WHERE item_guid = %u", guid))
                {
                    SendSysMessage("Encountered an error while attempting to remove item from gifts");
                    SetSentErrorMessage(true);
                    return false;
                }

                if (!CharacterDatabase.DirectPExecute("DELETE FROM mail_items WHERE item_guid = %u", guid))
                {
                    SendSysMessage("Encountered an error while attempting to remove item from mail");
                    SetSentErrorMessage(true);
                    return false;
                }

                count -= stackCount;
            }
        }
    }

    PSendSysMessage(LANG_REMOVEITEM, itemId, countCopy, target_name.c_str());
    return true;
}

bool ChatHandler::HandleAddItemSetCommand(char* args)
{
    uint32 itemsetId;
    if (!ExtractUint32KeyFromLink(&args, "Hitemset", itemsetId))
        return false;

    // prevent generation all items with itemset field value '0'
    if (itemsetId == 0)
    {
        PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemsetId);
        SetSentErrorMessage(true);
        return false;
    }

    Player* pl = m_session->GetPlayer();
    Player* plTarget = GetSelectedPlayer();
    if (!plTarget)
        plTarget = pl;

    DETAIL_LOG(GetMangosString(LANG_ADDITEMSET), itemsetId);

    bool found = false;
    for (auto const& itr : sObjectMgr.GetItemPrototypeMap())
    {
        ItemPrototype const *pProto = &itr.second;
        if (!pProto)
            continue;

        if (pProto->ItemSet == itemsetId)
        {
            found = true;
            ItemPosCountVec dest;
            InventoryResult msg = plTarget->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, pProto->ItemId, 1);
            if (msg == EQUIP_ERR_OK)
            {
                Item* item = plTarget->StoreNewItem(dest, pProto->ItemId, true);

                // remove binding (let GM give it to another player later)
                if (pl == plTarget)
                    item->SetBinding(false);

                pl->SendNewItem(item, 1, false, true);
                if (pl != plTarget)
                    plTarget->SendNewItem(item, 1, true, false);
            }
            else
            {
                pl->SendEquipError(msg, nullptr, nullptr, pProto->ItemId);
                PSendSysMessage(LANG_ITEM_CANNOT_CREATE, pProto->ItemId, 1);
            }
        }
    }

    if (!found)
    {
        PSendSysMessage(LANG_NO_ITEMS_FROM_ITEMSET_FOUND, itemsetId);

        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

void ChatHandler::ShowItemListHelper(uint32 itemId, int loc_idx, Player* target /*=nullptr*/)
{
    ItemPrototype const *itemProto = sObjectMgr.GetItemPrototype(itemId);
    if (!itemProto)
        return;

    std::string name;

    if (ItemLocale const *il = loc_idx >= 0 ? sObjectMgr.GetItemLocale(itemProto->ItemId) : nullptr)
        name = il->Name[loc_idx];

    if (name.empty())
        name = itemProto->Name1;

    char const* usableStr = "";

    if (target)
    {
        if (target->CanUseItem(itemProto))
            usableStr = GetMangosString(LANG_COMMAND_ITEM_USABLE);
    }

    if (m_session)
    {
        uint32 color = ItemQualityColors[itemProto->Quality];
        std::ostringstream itemStr;
        itemStr << "|c" << std::hex << color << "|Hitem:" << std::to_string(itemProto->ItemId) << ":0:0:0:0:0:0:0|h[" << name << "]|h|r";
        PSendSysMessage(LANG_ITEM_LIST_CONSOLE, itemId, itemStr.str().c_str(), usableStr);
    }
    else
        PSendSysMessage(LANG_ITEM_LIST_CONSOLE, itemId, name.c_str(), usableStr);
}

bool ChatHandler::HandleLookupItemCommand(char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    Player* pl = m_session ? m_session->GetPlayer() : nullptr;

    uint32 counter = 0;

    // Search in `item_template`
    for (auto const& itr : sObjectMgr.GetItemPrototypeMap())
    {
        ItemPrototype const *pProto = &itr.second;
        if (!pProto)
            continue;

        int loc_idx = GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
        {
            ItemLocale const *il = sObjectMgr.GetItemLocale(pProto->ItemId);
            if (il)
            {
                if ((int32)il->Name.size() > loc_idx && !il->Name[loc_idx].empty())
                {
                    std::string name = il->Name[loc_idx];

                    if (Utf8FitTo(name, wnamepart))
                    {
                        ShowItemListHelper(pProto->ItemId, loc_idx, pl);
                        ++counter;
                        continue;
                    }
                }
            }
        }

        std::string name = pProto->Name1;
        if (name.empty())
            continue;

        if (Utf8FitTo(name, wnamepart))
        {
            ShowItemListHelper(pProto->ItemId, -1, pl);
            ++counter;
        }
    }

    if (counter == 0)
        SendSysMessage(LANG_COMMAND_NOITEMFOUND);

    return true;
}

bool ChatHandler::HandleLookupSkillCommand(char* args)
{
    if (!*args)
        return false;

    // can be nullptr in console call
    Player* target = GetSelectedPlayer();

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    uint32 counter = 0;                                     // Counter for figure out that we found smth.

    // Search in SkillLine.dbc
    for (uint32 id = 0; id < sSkillLineStore.GetNumRows(); id++)
    {
        SkillLineEntry const *skillInfo = sSkillLineStore.LookupEntry(id);
        if (skillInfo)
        {
            int loc = GetSessionDbcLocale();
            std::string name = skillInfo->name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < MAX_DBC_LOCALE; ++loc)
                {
                    if (loc == GetSessionDbcLocale())
                        continue;

                    name = skillInfo->name[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < MAX_DBC_LOCALE)
            {
                char valStr[50] = "";
                char const* knownStr = "";
                if (target && target->HasSkill(id))
                {
                    knownStr = GetMangosString(LANG_KNOWN);
                    uint32 curValue = target->GetSkillValuePure(id);
                    uint32 maxValue  = target->GetSkillMaxPure(id);
                    uint32 permValue = target->GetSkillBonusPermanent(id);
                    uint32 tempValue = target->GetSkillBonusTemporary(id);

                    char const* valFormat = GetMangosString(LANG_SKILL_VALUES);
                    snprintf(valStr, 50, valFormat, curValue, maxValue, permValue, tempValue);
                }

                // send skill in "id - [namedlink locale]" format
                if (m_session)
                    PSendSysMessage(LANG_SKILL_LIST_CHAT, id, id, name.c_str(), localeNames[loc], knownStr, valStr);
                else
                    PSendSysMessage(LANG_SKILL_LIST_CONSOLE, id, name.c_str(), localeNames[loc], knownStr, valStr);

                ++counter;
            }
        }
    }
    if (counter == 0)                                       // if counter == 0 then we found nth
        SendSysMessage(LANG_COMMAND_NOSKILLFOUND);
    return true;
}

void ChatHandler::ShowSpellListHelper(Player* target, SpellEntry const* spellInfo, LocaleConstant loc)
{
    uint32 id = spellInfo->Id;

    bool known = target && target->HasSpell(id);
    bool learn = (spellInfo->Effect[EFFECT_INDEX_0] == SPELL_EFFECT_LEARN_SPELL);

    uint32 talentCost = GetTalentSpellCost(id);

    bool talent = (talentCost > 0);
    bool passive = spellInfo->IsPassiveSpell();
    bool active = target && target->HasAura(id);

    // unit32 used to prevent interpreting uint8 as char at output
    // find rank of learned spell for learning spell, or talent rank
    uint32 rank = talentCost ? talentCost : sSpellMgr.GetSpellRank(learn ? spellInfo->EffectTriggerSpell[EFFECT_INDEX_0] : id);

    // send spell in "id - [name, rank N] [talent] [passive] [learn] [known]" format
    std::ostringstream ss;
    if (m_session)
        ss << id << " - |cffffffff|Hspell:" << id << "|h[" << spellInfo->SpellName[loc];
    else
        ss << id << " - " << spellInfo->SpellName[loc];

    // include rank in link name
    if (rank)
        ss << GetMangosString(LANG_SPELL_RANK) << rank;

    if (m_session)
        ss << " " << localeNames[loc] << "]|h|r";
    else
        ss << " " << localeNames[loc];

    if (talent)
        ss << GetMangosString(LANG_TALENT);
    if (passive)
        ss << GetMangosString(LANG_PASSIVE);
    if (learn)
        ss << GetMangosString(LANG_LEARN);
    if (known)
        ss << GetMangosString(LANG_KNOWN);
    if (active)
        ss << GetMangosString(LANG_ACTIVE);

    SendSysMessage(ss.str().c_str());
}

bool ChatHandler::HandleLookupSpellCommand(char* args)
{
    if (!*args)
        return false;

    // can be nullptr at console call
    Player* target = GetSelectedPlayer();

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    uint32 counter = 0;                                     // Counter for figure out that we found smth.

    // Search in Spell.dbc
    for (uint32 id = 0; id < sSpellMgr.GetMaxSpellId(); id++)
    {
        SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(id);
        if (spellInfo)
        {
            int loc = GetSessionDbcLocale();
            std::string name = spellInfo->SpellName[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < MAX_DBC_LOCALE; ++loc)
                {
                    if (loc == GetSessionDbcLocale())
                        continue;

                    name = spellInfo->SpellName[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < MAX_DBC_LOCALE)
            {
                ShowSpellListHelper(target, spellInfo, LocaleConstant(loc));
                ++counter;
            }
        }
    }
    if (counter == 0)                                       // if counter == 0 then we found nth
        SendSysMessage(LANG_COMMAND_NOSPELLFOUND);
    return true;
}

bool ChatHandler::HandleLookupItemSetCommand(char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    uint32 counter = 0;                                     // Counter for figure out that we found smth.

    // Search in ItemSet.dbc
    for (uint32 id = 0; id < sItemSetStore.GetNumRows(); id++)
    {
        ItemSetEntry const* set = sItemSetStore.LookupEntry(id);
        if (set)
        {
            int loc = GetSessionDbcLocale();
            std::string name = set->name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < MAX_DBC_LOCALE; ++loc)
                {
                    if (loc == GetSessionDbcLocale())
                        continue;

                    name = set->name[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < MAX_DBC_LOCALE)
            {
                // send item set in "id - [namedlink locale]" format
                if (m_session)
                    PSendSysMessage(LANG_ITEMSET_LIST_CHAT, id, id, name.c_str(), localeNames[loc]);
                else
                    PSendSysMessage(LANG_ITEMSET_LIST_CONSOLE, id, name.c_str(), localeNames[loc]);
                ++counter;
            }
        }
    }
    if (counter == 0)                                       // if counter == 0 then we found nth
        SendSysMessage(LANG_COMMAND_NOITEMSETFOUND);
    return true;
}

bool ChatHandler::HandleLookupEventCommand(char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    uint32 counter = 0;

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

    for (uint32 id = 1; id < events.size(); ++id)
    {
        if (!sGameEventMgr.IsValidEvent(id))
            continue;

        GameEventData const& eventData = events[id];

        std::string descr = eventData.description;
        if (descr.empty())
            continue;

        if (Utf8FitTo(descr, wnamepart))
        {
            char const* active = sGameEventMgr.IsActiveEvent(id) ? GetMangosString(LANG_ACTIVE) : "";

            if (m_session)
                PSendSysMessage(LANG_EVENT_ENTRY_LIST_CHAT, id, id, eventData.description.c_str(), active);
            else
                PSendSysMessage(LANG_EVENT_ENTRY_LIST_CONSOLE, id, eventData.description.c_str(), active);

            ++counter;
        }
    }

    if (counter == 0)
        SendSysMessage(LANG_NOEVENTFOUND);

    return true;
}

bool ChatHandler::HandleLookupHwPrintCommand(char* args)
{
    CommandStream stream{ args };

    uint64 extendedPrint;

    if (!(stream >> extendedPrint))
    {
        SendSysMessage("Wrongly formatted HWPrint.");
        return false;
    }


    SendSysMessage("Listing all accounts with hwprint:");

    const auto& accountDatas = sWorld.GetAllAccountData();

    for (const auto& data : accountDatas)
    {
        if (data.second.lastExtendedFingerprint == extendedPrint)
        {
            PSendSysMessage("Got match on account username %s", data.second.username.c_str());
        }
    }

    return true;
}

bool ChatHandler::HandleLookupGuildCommand(char* args)
{
    if (!args || !*args)
        return false;

    char* name = ExtractQuotedArg(&args);
    if (!name)
        return false;

    std::string nameStr(name);
    Guild* guild = sGuildMgr.GetGuildByName(nameStr);
    if (!guild)
    {
        SendSysMessage(LANG_GUILD_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Guild %s (ID %u):", guild->GetName().c_str(), guild->GetId());
    std::string leaderName;
    sObjectMgr.GetPlayerNameByGUID(guild->GetLeaderGuid(), leaderName);
    PSendSysMessage("- Leader: %s, created: %u-%u-%u", leaderName.c_str(),
                    guild->GetCreatedYear(), guild->GetCreatedMonth(),
                    guild->GetCreatedDay());
    PSendSysMessage("- Members: %u (%u accounts)", guild->GetMemberSize(), guild->GetAccountsNumber());
    PSendSysMessage("- MOTD: %s", guild->GetMOTD().c_str());
    PSendSysMessage("- INFO: %s", guild->GetInfo().c_str());

    return true;
}

void ChatHandler::ShowQuestListHelper(uint32 questId, int32 loc_idx, Player* target /*= nullptr*/)
{
    Quest const* qinfo = sObjectMgr.GetQuestTemplate(questId);
    if (!qinfo)
        return;

    std::string title;

    if (QuestLocale const *il = loc_idx >= 0 ? sObjectMgr.GetQuestLocale(qinfo->GetQuestId()) : nullptr)
        title = il->Title[loc_idx];

    if (title.empty())
        title = qinfo->GetTitle();

    char const* statusStr = "";

    if (target)
    {
        QuestStatus status = target->GetQuestStatus(qinfo->GetQuestId());

        if (status == QUEST_STATUS_COMPLETE)
        {
            if (target->GetQuestRewardStatus(qinfo->GetQuestId()))
                statusStr = GetMangosString(LANG_COMMAND_QUEST_REWARDED);
            else
                statusStr = GetMangosString(LANG_COMMAND_QUEST_COMPLETE);
        }
        else if (status == QUEST_STATUS_INCOMPLETE)
            statusStr = GetMangosString(LANG_COMMAND_QUEST_ACTIVE);
    }

    if (m_session)
        PSendSysMessage(LANG_QUEST_LIST_CHAT, qinfo->GetQuestId(), qinfo->GetQuestId(), qinfo->GetQuestLevel(), title.c_str(), statusStr);
    else
        PSendSysMessage(LANG_QUEST_LIST_CONSOLE, qinfo->GetQuestId(), title.c_str(), statusStr);
}

bool ChatHandler::HandleLookupQuestCommand(char* args)
{
    if (!*args)
        return false;

    // can be nullptr at console call
    Player* target = GetSelectedPlayer();

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    uint32 counter = 0 ;

    ObjectMgr::QuestMap const& qTemplates = sObjectMgr.GetQuestTemplates();
    for (const auto& itr : qTemplates)
    {
        const auto& qinfo = itr.second;

        int loc_idx = GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
        {
            QuestLocale const *il = sObjectMgr.GetQuestLocale(qinfo->GetQuestId());
            if (il)
            {
                if ((int32)il->Title.size() > loc_idx && !il->Title[loc_idx].empty())
                {
                    std::string title = il->Title[loc_idx];

                    if (Utf8FitTo(title, wnamepart))
                    {
                        ShowQuestListHelper(qinfo->GetQuestId(), loc_idx, target);
                        ++counter;
                        continue;
                    }
                }
            }
        }

        std::string title = qinfo->GetTitle();
        if (title.empty())
            continue;

        if (Utf8FitTo(title, wnamepart))
        {
            ShowQuestListHelper(qinfo->GetQuestId(), -1, target);
            ++counter;
        }
    }

    if (counter == 0)
        SendSysMessage(LANG_COMMAND_NOQUESTFOUND);

    return true;
}

bool ChatHandler::HandleLookupCreatureCommand(char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    uint32 counter = 0;

    for (auto const& itr : sObjectMgr.GetCreatureInfoMap())
    {
        uint32 id = itr.first;
        CreatureInfo const* cInfo = itr.second.get();
        if (!cInfo)
            continue;

        int loc_idx = GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
        {
            CreatureLocale const *cl = sObjectMgr.GetCreatureLocale(id);
            if (cl)
            {
                if ((int32)cl->Name.size() > loc_idx && !cl->Name[loc_idx].empty())
                {
                    std::string name = cl->Name[loc_idx];

                    if (Utf8FitTo(name, wnamepart))
                    {
                        if (m_session)
                            PSendSysMessage(LANG_CREATURE_ENTRY_LIST_CHAT, id, id, name.c_str());
                        else
                            PSendSysMessage(LANG_CREATURE_ENTRY_LIST_CONSOLE, id, name.c_str());
                        ++counter;
                        continue;
                    }
                }
            }
        }

        std::string name = cInfo->name;
        if (name.empty())
            continue;

        if (Utf8FitTo(name, wnamepart))
        {
            if (m_session)
                PSendSysMessage(LANG_CREATURE_ENTRY_LIST_CHAT, id, id, name.c_str());
            else
                PSendSysMessage(LANG_CREATURE_ENTRY_LIST_CONSOLE, id, name.c_str());
            ++counter;
        }
    }

    if (counter == 0)
        SendSysMessage(LANG_COMMAND_NOCREATUREFOUND);

    return true;
}


bool ChatHandler::HandleLookupObjectCommand(char* args)
{
    if (!*args)
        return false;

    std::string namepart = args;
    std::wstring wnamepart;

    // converting string that we try to find to lower case
    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    wstrToLower(wnamepart);

    uint32 counter = 0;

    for (auto const& itr : sObjectMgr.GetGameObjectInfoMap())
    {
        int loc_idx = GetSessionDbLocaleIndex();
        if (loc_idx >= 0)
        {
            GameObjectLocale const *gl = sObjectMgr.GetGameObjectLocale(itr.second.id);
            if (gl)
            {
                if ((int32)gl->Name.size() > loc_idx && !gl->Name[loc_idx].empty())
                {
                    std::string name = gl->Name[loc_idx];

                    if (Utf8FitTo(name, wnamepart))
                    {
                        if (m_session)
                            PSendSysMessage(LANG_GO_ENTRY_LIST_CHAT, itr.second.id, itr.second.id, name.c_str());
                        else
                            PSendSysMessage(LANG_GO_ENTRY_LIST_CONSOLE, itr.second.id, name.c_str());
                        ++counter;
                        continue;
                    }
                }
            }
        }

        std::string name = itr.second.name;
        if (name.empty())
            continue;

        if (Utf8FitTo(name, wnamepart))
        {
            if (m_session)
                PSendSysMessage(LANG_GO_ENTRY_LIST_CHAT, itr.second.id, itr.second.id, name.c_str());
            else
                PSendSysMessage(LANG_GO_ENTRY_LIST_CONSOLE, itr.second.id, name.c_str());
            ++counter;
        }
    }

    if (counter == 0)
        SendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);

    return true;
}

/** \brief GM command level 3 - Create a guild.
 *
 * This command allows a GM (level 3) to create a guild.
 *
 * The "args" parameter contains the name of the guild leader
 * and then the name of the guild.
 *
 */
bool ChatHandler::HandleGuildCreateCommand(char* args)
{
    // guildmaster name optional
    char* guildMasterStr = ExtractOptNotLastArg(&args);

    Player* target;
    if (!ExtractPlayerTarget(&guildMasterStr, &target))
        return false;

    char* guildStr = ExtractQuotedArg(&args);
    if (!guildStr)
        return false;

    std::string guildname = guildStr;

    if (target->GetGuildId())
    {
        SendSysMessage(LANG_PLAYER_IN_GUILD);
        return true;
    }

    Guild* guild = new Guild;
    if (!guild->Create(target, guildname))
    {
        delete guild;
        SendSysMessage(LANG_GUILD_NOT_CREATED);
        SetSentErrorMessage(true);
        return false;
    }

    sGuildMgr.AddGuild(guild);
    return true;
}

bool ChatHandler::HandleGuildInviteCommand(char *args)
{
    // player name optional
    char* nameStr = ExtractOptNotLastArg(&args);

    // if not guild name only (in "") then player name
    ObjectGuid target_guid;
    if (!ExtractPlayerTarget(&nameStr, nullptr, &target_guid))
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    char* guildStr = ExtractQuotedArg(&args);
    if (!guildStr)
        return false;

    std::string glName = guildStr;
    Guild* targetGuild = sGuildMgr.GetGuildByName(glName);
    if (!targetGuild)
    {
        SendSysMessage(LANG_GUILD_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    // player's guild membership checked in AddMember before add
    auto status = targetGuild->AddMember(target_guid, targetGuild->GetLowestRank());

    if (status != GuildAddStatus::OK)
    {
        std::string error;

        switch (status) // bad
        {
            case GuildAddStatus::ALREADY_IN_GUILD:
                error = "Player is already in a guild.";
                break;
            case GuildAddStatus::GUILD_FULL:
                error = "The target guild is full.";
                break;
            case GuildAddStatus::PLAYER_DATA_ERROR:
                error = "Player data appears to be corrupt - tell an administrator.";
                break;
            case GuildAddStatus::UNKNOWN_PLAYER:
                error = "Unable to find target player.";
                break;
            default:
                error = "Unhandled guild invite error.";
        }

        SendSysMessage(error.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Player added to %s.", glName.c_str());
    return true;
}

bool ChatHandler::HandleGuildUninviteCommand(char *args)
{
    Player* target;
    ObjectGuid target_guid;
    if (!ExtractPlayerTarget(&args, &target, &target_guid))
        return false;

    uint32 glId = target ? target->GetGuildId() : Player::GetGuildIdFromDB(target_guid);

    if (!glId)
        return false;

    Guild* targetGuild = sGuildMgr.GetGuildById(glId);
    if (!targetGuild)
    {
        SendSysMessage(LANG_GUILD_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (targetGuild->DelMember(target_guid))
    {
        targetGuild->Disband();
        delete targetGuild;
    }

    return true;
}

bool ChatHandler::HandleGuildRankCommand(char *args)
{
    char* nameStr = ExtractOptNotLastArg(&args);

    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&nameStr, &target, &target_guid, &target_name))
        return false;

    uint32 glId = target ? target->GetGuildId() : Player::GetGuildIdFromDB(target_guid);

    if (!glId)
    {
        SendSysMessage(LANG_GUILD_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    Guild* targetGuild = sGuildMgr.GetGuildById(glId);
    if (!targetGuild)
        return false;

    uint32 newrank;
    if (!ExtractUInt32(&args, newrank))
        return false;

    if (newrank > targetGuild->GetLowestRank())
        return false;

    MemberSlot* slot = targetGuild->GetMemberSlot(target_guid);
    if (!slot)
        return false;

    slot->ChangeRank(newrank);
    return true;
}

bool ChatHandler::HandleGuildDeleteCommand(char* args)
{
    if (!*args)
        return false;

    char* guildStr = ExtractQuotedArg(&args);
    if (!guildStr)
        return false;

    std::string gld = guildStr;

    Guild* targetGuild = sGuildMgr.GetGuildByName(gld);

    if (!targetGuild)
    {
        SendSysMessage(LANG_GUILD_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    targetGuild->Disband();
    delete targetGuild;

    return true;
}

bool ChatHandler::HandleGuildHouseCommand(char* args)
{
    if (!*args)
    {
        SendSysMessage("Syntax: .guild house on / off");
        SetSentErrorMessage(true);
        return false;
    }

    bool value;

    if (!ExtractOnOff(&args, value))
    {
        SetSentErrorMessage(true);
        return false;
    }

    Player* player = m_session->GetPlayer();

    if (!player)
        return false;

    uint32 guild_id = player->GetGuildId();

    if (!guild_id)
    {
        SendSysMessage("You must be in the guild to add a guild house.");
        return false;
    }

    if (value)
    {
        CharacterDatabase.PExecute("REPLACE INTO guild_house VALUES (%u, %u, %f, %f, %f, %f);",
            guild_id, player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
        PSendSysMessage("The guild house teleport for %s was created.", sGuildMgr.GetGuildNameById(guild_id).c_str());
    }
    else
    {
        CharacterDatabase.PExecute("DELETE FROM guild_house WHERE guild_id = %u", guild_id);
        SendSysMessage("The guild house teleport was removed.");
    }

    SendSysMessage("Type .reload housing to reload guild houses.");
    return true;
}

/**
 * Renames a guild if a guild could be found with the specified name.
 * Usage: .guild rename "name" "new name"
 * It is not possible to rename a guild to a name that is already in use.
 */
bool ChatHandler::HandleGuildRenameCommand(char* args)
{
    if (!args || !*args)
        return false;

    char* currentName = ExtractQuotedArg(&args);
    if (!currentName)
        return false;

    char* newName = ExtractQuotedArg(&args);
    if (!newName)
        return false;

    std::string newn(newName);

    Guild* target = sGuildMgr.GetGuildByName(currentName);
    if (!target)
    {
        SendSysMessage(LANG_GUILD_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (Guild* existing = sGuildMgr.GetGuildByName(newn))
    {
        PSendSysMessage("A guild with the name '%s' already exists", newName);
        SetSentErrorMessage(true);
        return false;
    }

    target->Rename(newn);
    PSendSysMessage("Guild '%s' successfully renamed to '%s'. Players must relog to see the changes", currentName, newName);
    return true;
}

bool ChatHandler::HandleGuildLeaderCommand(char* args)
{
    Player* pPlayer;
    ObjectGuid playerGuid;
    std::string playerName;
    if (!ExtractPlayerTarget(&args, &pPlayer, &playerGuid, &playerName))
        return false;

    Guild* pGuild;
    if (pPlayer)
        pGuild = sGuildMgr.GetGuildById(pPlayer->GetGuildId());
    else
        pGuild = sGuildMgr.GetPlayerGuild(playerGuid.GetCounter());
    
    if (!pGuild)
    {
        SendSysMessage("Player is not in a guild.");
        SetSentErrorMessage(true);
        return false;
    }

    if (pGuild->GetLeaderGuid() == playerGuid)
    {
        SendSysMessage("Player is already the guild leader.");
        SetSentErrorMessage(true);
        return false;
    }

    pGuild->SetNewLeader(playerGuid);
    PSendSysMessage("Leader of \"%s\" changed to %s.", pGuild->GetName().c_str(), playerName.c_str());
    return true;
}

bool ChatHandler::HandleGuildListenCommand(char* args)
{
    if (!args || !*args)
        return false;

    char* guildName = ExtractQuotedArg(&args);
    if (!guildName)
        return false;

    std::string nameStr = guildName;

    Guild* target = sGuildMgr.GetGuildByName(guildName);
    if (!target)
    {
        SendSysMessage(LANG_GUILD_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    return target->AddGMListener(GetSession()->GetPlayer());
}


bool ChatHandler::HandleGetDistanceCommand(char* args)
{
    WorldObject* obj = nullptr;

    if (*args)
    {
        if (ObjectGuid guid = ExtractGuidFromLink(&args))
            obj = (WorldObject*)m_session->GetPlayer()->GetObjectByTypeMask(guid, TYPEMASK_CREATURE_OR_GAMEOBJECT);

        if (!obj)
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }
    }
    else
    {
        obj = GetSelectedUnit();

        if (!obj)
        {
            SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            SetSentErrorMessage(true);
            return false;
        }
    }

    Player* player = m_session->GetPlayer();
    // Calculate point-to-point distance
    float dx, dy, dz;
    dx = player->GetPositionX() - obj->GetPositionX();
    dy = player->GetPositionY() - obj->GetPositionY();
    dz = player->GetPositionZ() - obj->GetPositionZ();

    PSendSysMessage(LANG_DISTANCE, player->GetDistance(obj), player->GetDistance2d(obj), sqrt(dx * dx + dy * dy + dz * dz));

    return true;
}

bool ChatHandler::HandleDieCommand(char* /*args*/)
{
    Unit* target = GetSelectedUnit();

    if (!target || !m_session->GetPlayer()->GetSelectionGuid())
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        if (HasLowerSecurity((Player*)target, ObjectGuid(), false))
            return false;
    }

    if (target->ToPlayer() && target->ToPlayer()->IsHardcore())
    {
        SendSysMessage("Cannot kill a Hardcore Character.");
        SetSentErrorMessage(true);
        return false;
    }

    if (target->IsAlive())
    {
        if (sWorld.getConfig(CONFIG_BOOL_DIE_COMMAND_CREDIT))
            m_session->GetPlayer()->DealDamage(target, target->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
        else
        {
            Creature* targetCreature = target->ToCreature();
            if (targetCreature)
                targetCreature->SetLootRecipient(nullptr);
            target->DealDamage(target, target->GetHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
        }
    }

    return true;
}

bool ChatHandler::HandleFearCommand(char* /*args*/)
{
    Unit* target = GetSelectedUnit();

    if (!target || !m_session->GetPlayer()->GetSelectionGuid())
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    const uint32 fearID = 26641;

    SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(fearID);
    if (!spellInfo)
        return false;

    if (target->ToPlayer() && target->ToPlayer()->IsHardcore())
    {
        SendSysMessage("Cannot fear a Hardcore Character.");
        SetSentErrorMessage(true);
        return false;
    }

    if (!spellInfo->IsSpellAppliesAura((1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)) && !spellInfo->HasEffect(SPELL_EFFECT_PERSISTENT_AREA_AURA))
    {
        PSendSysMessage(LANG_SPELL_NO_HAVE_AURAS, fearID);
        SetSentErrorMessage(true);
        return false;
    }

    SpellAuraHolder *holder = CreateSpellAuraHolder(spellInfo, target, m_session->GetPlayer(), m_session->GetPlayer());

    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        uint8 eff = spellInfo->Effect[i];
        if (eff >= TOTAL_SPELL_EFFECTS)
            continue;

        if (Spells::IsAreaAuraEffect(eff) || eff == SPELL_EFFECT_APPLY_AURA || eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
        {
            Aura *aur = CreateAura(spellInfo, SpellEffectIndex(i), nullptr, holder, target);
            holder->AddAura(aur, SpellEffectIndex(i));
        }
    }

    if (!target->AddSpellAuraHolder(holder))
        holder = nullptr;

    return true;
}

bool ChatHandler::HandleAoEDamageCommand(char* args)
{
    int32 damage_int = 1000;
    ExtractInt32(&args, damage_int);

    if (damage_int <= 0)
        return false;

    uint32 damage = uint32(damage_int);

    int32 max_range = 10;
    ExtractInt32(&args, max_range);

    if (max_range <= 0)
        return false;

    Player* pPlayer = m_session->GetPlayer();

    std::list<Unit*> targetsList;
    MaNGOS::AnyAoETargetUnitInObjectRangeCheck u_check(pPlayer, pPlayer, max_range);
    MaNGOS::UnitListSearcher<MaNGOS::AnyAoETargetUnitInObjectRangeCheck> searcher(targetsList, u_check);
    Cell::VisitAllObjects(pPlayer, searcher, max_range);

    for (Unit* pTarget : targetsList)
    {
        pPlayer->DealDamage(pTarget, damage, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);
        pPlayer->SendAttackStateUpdate(HITINFO_AFFECTS_VICTIM, pTarget, 0, SPELL_SCHOOL_MASK_NORMAL, damage, 0, 0, VICTIMSTATE_NORMAL, 0);
    }

    return true;
}

bool ChatHandler::HandleDamageCommand(char* args)
{
    if (!*args)
        return false;

    Unit* target = GetSelectedUnit();
    Player* player = m_session->GetPlayer();

    if (!player)
    {
        return true;
    }

    if (!target || !player->GetSelectionGuid())
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!target->IsAlive())
        return true;

    int32 damage_int;
    if (!ExtractInt32(&args, damage_int))
        return false;

    if (damage_int <= 0)
        return true;

    uint32 damage = uint32(damage_int);

    if (target->ToPlayer() && target->ToPlayer()->IsHardcore())
    {
        SendSysMessage("Cannot damage a Hardcore Character.");
        SetSentErrorMessage(true);
        return false;
    }

    // Apply creature scripting if required
    if (Creature* target_creature = target->ToCreature())
    {
        if (target_creature->AI())
        {
            target_creature->AI()->DamageTaken(player, damage);
        }
    }

    // flat melee damage without resistence/etc reduction
    if (!*args)
    {
        player->DealDamage(target, damage, nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, nullptr, false);

        if (target != player)
            player->SendAttackStateUpdate(HITINFO_AFFECTS_VICTIM, target, 1, SPELL_SCHOOL_MASK_NORMAL, damage, 0, 0, VICTIMSTATE_NORMAL, 0);

        return true;
    }

    uint32 school;
    if (!ExtractUInt32(&args, school))
        return false;

    if (school >= MAX_SPELL_SCHOOL)
        return false;

    SpellSchoolMask schoolmask = GetSchoolMask(school);

    if (schoolmask & SPELL_SCHOOL_MASK_NORMAL)
        damage = player->CalcArmorReducedDamage(target, damage);

    // melee damage by specific school
    if (!*args)
    {
        uint32 absorb = 0;
        int32 resist = 0;

        target->CalculateDamageAbsorbAndResist(player, schoolmask, SPELL_DIRECT_DAMAGE, damage, &absorb, &resist, nullptr);

        const uint32 bonus = (resist < 0 ? uint32(std::abs(resist)) : 0);
        damage += bonus;
        const uint32 malus = (resist > 0 ? (absorb + uint32(resist)) : absorb);

        if (damage <= malus)
            return true;

        damage -= malus;

        player->DealDamageMods(target, damage, &absorb);
        player->DealDamage(target, damage, nullptr, DIRECT_DAMAGE, schoolmask, nullptr, false);
        player->SendAttackStateUpdate(HITINFO_AFFECTS_VICTIM, target, 1, schoolmask, damage, absorb, resist, VICTIMSTATE_NORMAL, 0);
        return true;
    }

    // non-melee damage

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spellid = ExtractSpellIdFromLink(&args);
    if (!spellid || !sSpellMgr.GetSpellEntry(spellid))
        return false;

    player->SpellNonMeleeDamageLog(target, spellid, damage);
    return true;
}

bool ChatHandler::HandleReviveCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    if (!ExtractPlayerTarget(&args, &target, &target_guid))
        return false;

    if (target)
    {
        target->ResurrectPlayer(1.f);
        target->SpawnCorpseBones();
        PSendSysMessage(LANG_CHARACTER_REVIVED_ONLINE, playerLink(target->GetName()).c_str());
    }
    else
    {
        // will resurrected at login without corpse
        sObjectAccessor.ConvertCorpseForPlayer(target_guid);
        std::string playername;
        sObjectMgr.GetPlayerNameByGUID(target_guid, playername);
        PSendSysMessage(LANG_CHARACTER_REVIVED_OFFLINE, playerLink(playername).c_str());
    }

    return true;
}

bool ChatHandler::HandleAnonymousWhispers(char* args)
{
    Player* player = GetSession()->GetPlayer();
    bool value = player->HasOption(PLAYER_RANDOMIZE_WHISPER_NAMES);
    char* nameStr = ExtractQuotedOrLiteralArg(&args);
    std::string newName = nameStr;
    if (value)
    {
        player->RemoveOption(PLAYER_RANDOMIZE_WHISPER_NAMES);
        sObjectMgr.RemoveFakeName(player);
    }
    else
    {
        
        if (!nameStr)
        {
            //TODO generate name here.
            return false;
        }
        player->EnableOption(PLAYER_RANDOMIZE_WHISPER_NAMES);
        sObjectMgr.AddFakeName(player, newName);
    }

    value = !value;

    PSendSysMessage("Anonymous whispers are now %s", value ? "ON" : "OFF");


    return true;
}

bool ChatHandler::HandleAnonymousMail(char* args)
{
    Player* player = GetSession()->GetPlayer();
    bool value = player->HasOption(PLAYER_ANON_MAIL);

    if (value)
        player->RemoveOption(PLAYER_ANON_MAIL);
    else
        player->EnableOption(PLAYER_ANON_MAIL);

    value = !value;

    PSendSysMessage("Anonymous mails are now %s", value ? "ON" : "OFF");

    return true;
}

bool ChatHandler::HandleAuraHelper(uint32 spellId, int32 duration, Unit* unit)
{
    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellId);
    if (!spellInfo)
    {
        PSendSysMessage("Spell %u does not exist.", spellId);
        SetSentErrorMessage(true);
        return false;
    }

    if (!spellInfo->IsSpellAppliesAura((1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)) &&
        !spellInfo->HasEffect(SPELL_EFFECT_PERSISTENT_AREA_AURA))
    {
        PSendSysMessage(LANG_SPELL_NO_HAVE_AURAS, spellId);
        SetSentErrorMessage(true);
        return false;
    }

    SpellAuraHolder* holder = CreateSpellAuraHolder(spellInfo, unit, m_session->GetPlayer(), m_session->GetPlayer());

    if (duration > 0)
        holder->SetAuraDuration(duration * IN_MILLISECONDS);

    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        uint8 eff = spellInfo->Effect[i];
        if (eff >= TOTAL_SPELL_EFFECTS)
            continue;
        if (Spells::IsAreaAuraEffect(eff) ||
            eff == SPELL_EFFECT_APPLY_AURA ||
            eff == SPELL_EFFECT_PERSISTENT_AREA_AURA)
        {
            Aura* aur = CreateAura(spellInfo, SpellEffectIndex(i), nullptr, holder, unit);
            holder->AddAura(aur, SpellEffectIndex(i));
        }
    }
    if (!unit->AddSpellAuraHolder(holder))
        holder = nullptr;
    return true;
}

bool ChatHandler::HandleAuraCommand(char* args)
{
    Unit* target = GetSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // Only allow devs to use auras in other players
    if (GetSession()->GetSecurity() < SEC_DEVELOPER)
        target = GetSession()->GetPlayer();

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spellId = ExtractSpellIdFromLink(&args);
    // Aura duration in seconds
    int32 duration = 0;
    ExtractInt32(&args, duration);

    return HandleAuraHelper(spellId, duration, target);
}

bool ChatHandler::HandleUnAuraCommand(char* args)
{
    Unit *target = GetSelectedUnit();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    std::string argstr = args;
    if (argstr == "all")
    {
        target->RemoveAllAuras();
        return true;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spellID = ExtractSpellIdFromLink(&args);
    if (!spellID)
        return false;

    target->RemoveAurasDueToSpell(spellID);

    return true;
}

bool ChatHandler::HandleLinkGraveCommand(char* args)
{
    uint32 g_id;
    if (!ExtractUInt32(&args, g_id))
        return false;

    char* teamStr = ExtractLiteralArg(&args);

    Team g_team;
    if (!teamStr)
        g_team = TEAM_NONE;
    else if (strncmp(teamStr, "horde", strlen(teamStr)) == 0)
        g_team = HORDE;
    else if (strncmp(teamStr, "alliance", strlen(teamStr)) == 0)
        g_team = ALLIANCE;
    else
        return false;

    WorldSafeLocsEntry const* graveyard =  sWorldSafeLocsStore.LookupEntry(g_id);

    if (!graveyard)
    {
        PSendSysMessage(LANG_COMMAND_GRAVEYARDNOEXIST, g_id);
        SetSentErrorMessage(true);
        return false;
    }

    Player* player = m_session->GetPlayer();

    uint32 zoneId = player->GetZoneId();

    const auto *areaEntry = AreaEntry::GetById(zoneId);
    if (!areaEntry || !areaEntry->IsZone())
    {
        PSendSysMessage(LANG_COMMAND_GRAVEYARDWRONGZONE, g_id, zoneId);
        SetSentErrorMessage(true);
        return false;
    }

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (sObjectMgr.AddGraveYardLink(g_id, zoneId, g_team))
        PSendSysMessage(LANG_COMMAND_GRAVEYARDLINKED, g_id, zoneId);
    else
        PSendSysMessage(LANG_COMMAND_GRAVEYARDALRLINKED, g_id, zoneId);

    return true;
}

bool ChatHandler::HandleNearGraveCommand(char* args)
{
    Team g_team;

    size_t argslen = strlen(args);

    if (!*args)
        g_team = TEAM_NONE;
    else if (strncmp(args, "horde", argslen) == 0)
        g_team = HORDE;
    else if (strncmp(args, "alliance", argslen) == 0)
        g_team = ALLIANCE;
    else
        return false;

    Player* player = m_session->GetPlayer();
    uint32 zone_id = player->GetZoneId();

    WorldSafeLocsEntry const* graveyard = sObjectMgr.GetClosestGraveYard(
            player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), g_team);

    if (graveyard)
    {
        uint32 g_id = graveyard->ID;

        GraveYardData const* data = sObjectMgr.FindGraveYardData(g_id, zone_id);
        if (!data)
        {
            PSendSysMessage(LANG_COMMAND_GRAVEYARDERROR, g_id);
            SetSentErrorMessage(true);
            return false;
        }

        g_team = data->team;

        std::string team_name = GetMangosString(LANG_COMMAND_GRAVEYARD_NOTEAM);

        if (g_team == 0)
            team_name = GetMangosString(LANG_COMMAND_GRAVEYARD_ANY);
        else if (g_team == HORDE)
            team_name = GetMangosString(LANG_COMMAND_GRAVEYARD_HORDE);
        else if (g_team == ALLIANCE)
            team_name = GetMangosString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

        PSendSysMessage(LANG_COMMAND_GRAVEYARDNEAREST, g_id, team_name.c_str(), zone_id);
    }
    else
    {
        std::string team_name;

        if (g_team == 0)
            team_name = GetMangosString(LANG_COMMAND_GRAVEYARD_ANY);
        else if (g_team == HORDE)
            team_name = GetMangosString(LANG_COMMAND_GRAVEYARD_HORDE);
        else if (g_team == ALLIANCE)
            team_name = GetMangosString(LANG_COMMAND_GRAVEYARD_ALLIANCE);

        if (g_team == ~uint32(0))
            PSendSysMessage(LANG_COMMAND_ZONENOGRAVEYARDS, zone_id);
        else
            PSendSysMessage(LANG_COMMAND_ZONENOGRAFACTION, zone_id, team_name.c_str());
    }

    return true;
}

//-----------------------Npc Commands-----------------------

bool ChatHandler::HandleNpcSetWanderDistanceCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        return true;
    }

    float dist = 0.0f;

    if (!ExtractFloat(&args, dist))
        return false;

    MovementGeneratorType type = RANDOM_MOTION_TYPE;
    if (dist == 0.0f)
        type = IDLE_MOTION_TYPE;

    pCreature->SetWanderDistance(dist);
    pCreature->SetDefaultMovementType(type);
    pCreature->GetMotionMaster()->Initialize();
    if (pCreature->IsAlive())                         // Dead creature will reset movement generator at respawn
    {
        pCreature->SetDeathState(JUST_DIED);
        pCreature->Respawn();
    }
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    pCreature->SaveToDB();

    return true;
}

bool ChatHandler::HandleNpcInfoCommand(char* /*args*/)
{
    Creature* target = GetSelectedCreature();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 faction = target->GetFactionTemplateId();
    uint32 npcflags = target->GetUInt32Value(UNIT_NPC_FLAGS);
    uint32 displayid = target->GetDisplayId();
    uint32 nativeid = target->GetNativeDisplayId();
    uint32 Entry = target->GetEntry();
    CreatureInfo const* cInfo = target->GetCreatureInfo();

    time_t curRespawnDelay = target->GetRespawnTimeEx() - time(nullptr);
    if (curRespawnDelay < 0)
        curRespawnDelay = 0;
    std::string curRespawnDelayStr = secsToTimeString(curRespawnDelay, true);
    std::string defRespawnDelayStr = secsToTimeString(target->GetRespawnDelay(), true);

    PSendSysMessage(LANG_NPCINFO_CHAR, target->GetGuidStr().c_str(), faction, npcflags, Entry, displayid, nativeid);
    PSendSysMessage(LANG_NPCINFO_LEVEL, target->GetLevel());
    PSendSysMessage(LANG_NPCINFO_EQUIPMENT, target->GetCurrentEquipmentId());
    PSendSysMessage(LANG_NPCINFO_HEALTH, target->GetCreateHealth(), target->GetMaxHealth(), target->GetHealth());
    if (target->GetPowerType() == POWER_MANA)
        PSendSysMessage(LANG_NPCINFO_MANA, target->GetCreateMana(), target->GetMaxPower(POWER_MANA), target->GetPower(POWER_MANA));
    PSendSysMessage(LANG_NPCINFO_INHABIT_TYPE, cInfo->inhabit_type);
    PSendSysMessage(LANG_NPCINFO_FLAGS, target->GetUInt32Value(UNIT_FIELD_FLAGS), target->GetUInt32Value(UNIT_DYNAMIC_FLAGS), target->GetFactionTemplateId());
    PSendSysMessage(LANG_COMMAND_RAWPAWNTIMES, defRespawnDelayStr.c_str(), curRespawnDelayStr.c_str());
    PSendSysMessage(LANG_NPCINFO_LOOT, cInfo->loot_id, cInfo->pickpocket_loot_id, cInfo->skinning_loot_id);
    PSendSysMessage(LANG_NPCINFO_ARMOR, target->GetArmor());
    PSendSysMessage(LANG_NPCINFO_DUNGEON_ID, target->GetInstanceId());
    PSendSysMessage(LANG_NPCINFO_POSITION, float(target->GetPositionX()), float(target->GetPositionY()), float(target->GetPositionZ()));
    PSendSysMessage(LANG_NPCINFO_AIINFO, target->GetAIName().c_str(), target->GetScriptName().c_str());
    PSendSysMessage(LANG_NPCINFO_ACTIVE_VISIBILITY, target->isActiveObject(), target->GetVisibilityModifier());

    if ((npcflags & UNIT_NPC_FLAG_VENDOR))
        SendSysMessage(LANG_NPCINFO_VENDOR);

    if ((npcflags & UNIT_NPC_FLAG_TRAINER))
        SendSysMessage(LANG_NPCINFO_TRAINER);

    ShowNpcOrGoSpawnInformation<Creature>(target->GetGUIDLow());
    return true;
}

//play npc emote
bool ChatHandler::HandleNpcPlayEmoteCommand(char* args)
{
    uint32 emote = atoi(args);

    Creature* target = GetSelectedCreature();
    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    target->HandleEmote(emote);

    return true;
}

bool ChatHandler::HandleNpcAddWeaponCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        return true;
    }

    // server slot id, 0 - for clear
    uint32 uiSlotId = 0;

    if (!ExtractUInt32(&args, uiSlotId))
        return false;

    if (!uiSlotId)
    {
        for (uint8 i = 0; i < MAX_VIRTUAL_ITEM_SLOT; ++i)
            pCreature->SetVirtualItem(VirtualItemSlot(i), 0);
        return true;
    }

    uint32 uiItemId = 0;

    if (!ExtractUInt32(&args, uiItemId))
        return false;
    
    ItemPrototype const* pItemProto = sObjectMgr.GetItemPrototype(uiItemId);

    if (!pItemProto)
    {
        PSendSysMessage(LANG_ITEM_NOT_FOUND, uiItemId);
        return true;
    }

    // convert to client slot id
    uint32 uiSlotId_C = uiSlotId - 1;

    if (uiSlotId_C > MAX_VIRTUAL_ITEM_SLOT)
    {
        PSendSysMessage(LANG_ITEM_SLOT_NOT_EXIST, uiSlotId_C);
        return true;
    }

    pCreature->SetVirtualItem(VirtualItemSlot(uiSlotId_C), uiItemId);
    PSendSysMessage(LANG_ITEM_ADDED_TO_SLOT, uiItemId, pItemProto->Name1.c_str(), uiSlotId_C);

    return true;
}
//----------------------------------------------------------

bool ChatHandler::HandleExploreCheatCommand(char* args)
{
    if (!*args)
        return false;

    int flag = atoi(args);

    Player *chr = GetSelectedPlayer();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (flag != 0)
    {
        PSendSysMessage(LANG_YOU_SET_EXPLORE_ALL, GetNameLink(chr).c_str());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_EXPLORE_SET_ALL, GetNameLink().c_str());
    }
    else
    {
        PSendSysMessage(LANG_YOU_SET_EXPLORE_NOTHING, GetNameLink(chr).c_str());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_EXPLORE_SET_NOTHING, GetNameLink().c_str());
    }

    for (uint8 i = 0; i < PLAYER_EXPLORED_ZONES_SIZE; ++i)
    {
        if (flag != 0)
            m_session->GetPlayer()->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);
        else
            m_session->GetPlayer()->SetFlag(PLAYER_EXPLORED_ZONES_1 + i, 0);
    }

    return true;
}

bool ChatHandler::HandleHoverCommand(char* args)
{
    uint32 flag;
    if (!ExtractOptUInt32(&args, flag, 1))
        return false;

    m_session->GetPlayer()->SetHover(flag);

    if (flag)
        SendSysMessage(LANG_HOVER_ENABLED);
    else
        SendSysMessage(LANG_HOVER_DISABLED);

    return true;
}

void ChatHandler::HandleCharacterLevel(Player* player, ObjectGuid player_guid, uint32 oldlevel, uint32 newlevel)
{
    if (player)
    {
        player->GiveLevel(newlevel);
        player->InitTalentForLevel();
        player->SetUInt32Value(PLAYER_XP, 0);

        if (needReportToTarget(player))
        {
            if (oldlevel == newlevel)
                ChatHandler(player).PSendSysMessage(LANG_YOURS_LEVEL_PROGRESS_RESET, GetNameLink().c_str());
            else if (oldlevel < newlevel)
                ChatHandler(player).PSendSysMessage(LANG_YOURS_LEVEL_UP, GetNameLink().c_str(), newlevel);
            else                                                // if(oldlevel > newlevel)
                ChatHandler(player).PSendSysMessage(LANG_YOURS_LEVEL_DOWN, GetNameLink().c_str(), newlevel);
        }
    }
    else
    {
        // update level and XP at level, all other will be updated at loading
        CharacterDatabase.PExecute("UPDATE characters SET level = '%u', xp = 0 WHERE guid = '%u'", newlevel, player_guid.GetCounter());
    }
}

bool ChatHandler::HandleLevelUpCommand(char* args)
{
    int32 addlevel = 1;
    char* nameStr = nullptr;

    if (*args)
    {
        nameStr = ExtractOptNotLastArg(&args);

        // exception opt second arg: .levelup $name
        if (!ExtractInt32(&args, addlevel))
        {
            if (!nameStr)
                nameStr = ExtractArg(&args);
            else
                return false;
        }
    }

    if (Creature* pCreature = GetSelectedCreature())
    {
        int32 newlevel = pCreature->GetLevel() + addlevel;

        if (newlevel < 1)
            newlevel = 1;

        if (newlevel > PLAYER_STRONG_MAX_LEVEL)
            newlevel = PLAYER_STRONG_MAX_LEVEL;

        if (pCreature->IsPet())
            ((Pet*)pCreature)->GivePetLevel(newlevel);
        else
            pCreature->SetLevel(newlevel);

        PSendSysMessage(LANG_YOU_CHANGE_LVL, pCreature->GetName(), newlevel);
    }
    else
    {
        Player* target;
        ObjectGuid target_guid;
        std::string target_name;
        if (!ExtractPlayerTarget(&nameStr, &target, &target_guid, &target_name))
            return false;

        int32 oldlevel = target ? target->GetLevel() : Player::GetLevelFromDB(target_guid);
        int32 newlevel = oldlevel + addlevel;

        if (newlevel < 1)
            newlevel = 1;

        if (newlevel > PLAYER_STRONG_MAX_LEVEL)                        // hardcoded maximum level
            newlevel = PLAYER_STRONG_MAX_LEVEL;

        HandleCharacterLevel(target, target_guid, oldlevel, newlevel);

        if (!m_session || m_session->GetPlayer() != target)     // including chr==nullptr
        {
            std::string nameLink = playerLink(target_name);
            PSendSysMessage(LANG_YOU_CHANGE_LVL, nameLink.c_str(), newlevel);
        }
    }

    return true;
}

bool ChatHandler::HandleShowAreaCommand(char* args)
{
    if (!*args)
        return false;

    Player *chr = GetSelectedPlayer();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    int area = AreaEntry::GetFlagById(atoi(args));
    int offset = area / 32;
    uint32 val = (uint32)(1 << (area % 32));

    if (area < 0 || offset >= PLAYER_EXPLORED_ZONES_SIZE)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 currFields = chr->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
    chr->SetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset, (uint32)(currFields | val));

    SendSysMessage(LANG_EXPLORE_AREA);
    return true;
}

bool ChatHandler::HandleHideAreaCommand(char* args)
{
    if (!*args)
        return false;

    Player *chr = GetSelectedPlayer();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    int area = AreaEntry::GetFlagById(atoi(args));
    int offset = area / 32;
    uint32 val = (uint32)(1 << (area % 32));

    if (area < 0 || offset >= PLAYER_EXPLORED_ZONES_SIZE)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 currFields = chr->GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset);
    chr->SetUInt32Value(PLAYER_EXPLORED_ZONES_1 + offset, (uint32)(currFields ^ val));

    SendSysMessage(LANG_UNEXPLORE_AREA);
    return true;
}

bool ChatHandler::HandleBankCommand(char* /*args*/)
{
    m_session->SendShowBank(m_session->GetPlayer()->GetObjectGuid());

    return true;
}

bool ChatHandler::HandleChangeWeatherCommand(char* args)
{
    // Weather is OFF
    if (!sWorld.getConfig(CONFIG_BOOL_WEATHER))
    {
        SendSysMessage(LANG_WEATHER_DISABLED);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 type;
    if (!ExtractUInt32(&args, type))
        return false;

    // see enum WeatherType
    if (!Weather::IsValidWeatherType(type))
        return false;

    float grade;
    if (!ExtractFloat(&args, grade))
        return false;

    // clamp grade from 0 to 1
    if (grade < 0.0f)
        grade = 0.0f;
    else if (grade > 1.0f)
        grade = 1.0f;

    Player* player = m_session->GetPlayer();
    uint32 zoneId = player->GetZoneId();
    player->GetMap()->SetWeather(zoneId, (WeatherType)type, grade, false);
    return true;
}

bool ChatHandler::HandleTeleAddCommand(char* args)
{
    if (!*args)
        return false;

    Player *player = m_session->GetPlayer();
    if (!player)
        return false;

    std::string name = args;

    if (sObjectMgr.GetGameTele(name))
    {
        SendSysMessage(LANG_COMMAND_TP_ALREADYEXIST);
        SetSentErrorMessage(true);
        return false;
    }

    GameTele tele;
    tele.x = player->GetPositionX();
    tele.y = player->GetPositionY();
    tele.z = player->GetPositionZ();
    tele.o = player->GetOrientation();
    tele.mapId = player->GetMapId();
    tele.name = name;

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (sObjectMgr.AddGameTele(tele))
        SendSysMessage(LANG_COMMAND_TP_ADDED);
    else
    {
        SendSysMessage(LANG_COMMAND_TP_ADDEDERR);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleTeleDelCommand(char* args)
{
    if (!*args)
        return false;

    std::string name = args;

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (!sObjectMgr.DeleteGameTele(name))
    {
        SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    SendSysMessage(LANG_COMMAND_TP_DELETED);
    return true;
}

bool ChatHandler::HandleResetHonorCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    target->GetHonorMgr().Reset();
    return true;
}

static bool HandleResetStatsOrLevelHelper(Player* player)
{
    ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(player->GetClass());
    if (!cEntry)
    {
        sLog.outError("Class %u not found in DBC (Wrong DBC files?)", player->GetClass());
        return false;
    }

    uint8 powertype = cEntry->powerType;

    // reset m_form if no aura
    if (!player->HasAuraType(SPELL_AURA_MOD_SHAPESHIFT))
        player->SetShapeshiftForm(FORM_NONE);

    player->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, DEFAULT_WORLD_OBJECT_SIZE);
    player->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

    player->SetFactionForRace(player->GetRace());

    player->SetByteValue(UNIT_FIELD_BYTES_0, 3, powertype);

    // reset only if player not in some form;
    if (player->GetShapeshiftForm() == FORM_NONE)
        player->InitPlayerDisplayIds();

    // is it need, only in pre-2.x used and field byte removed later?
    if (powertype == POWER_RAGE || powertype == POWER_MANA)
        player->SetByteValue(UNIT_FIELD_BYTES_1, 1, 0xEE);

    player->SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_UNK3 | UNIT_BYTE2_FLAG_UNK5);

    player->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

    //-1 is default value
    player->SetInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);

    //player->SetUInt32Value(PLAYER_FIELD_BYTES, 0xEEE00000 );
    return true;
}

bool ChatHandler::HandleResetLevelCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    if (!HandleResetStatsOrLevelHelper(target))
        return false;

    // set starting level
    uint32 start_level = sWorld.getConfig(CONFIG_UINT32_START_PLAYER_LEVEL);

    target->SetLevel(start_level);
    target->InitStatsForLevel(true);
    target->InitTaxiNodes();
    target->InitTalentForLevel();
    target->SetUInt32Value(PLAYER_XP, 0);

    // reset level for pet
    if (Pet* pet = target->GetPet())
        pet->SynchronizeLevelWithOwner();

    return true;
}

bool ChatHandler::HandleResetStatsCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    if (!HandleResetStatsOrLevelHelper(target))
        return false;

    target->InitStatsForLevel(true);
    target->InitTalentForLevel();

    return true;
}

bool ChatHandler::HandleResetSpellsCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &target_guid, &target_name))
        return false;

    if (target)
    {
        target->ResetSpells();

        ChatHandler(target).SendSysMessage(LANG_RESET_SPELLS);
        if (!m_session || m_session->GetPlayer() != target)
            PSendSysMessage(LANG_RESET_SPELLS_ONLINE, GetNameLink(target).c_str());
    }
    else
    {
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE guid = '%u'", uint32(AT_LOGIN_RESET_SPELLS), target_guid.GetCounter());
        PSendSysMessage(LANG_RESET_SPELLS_OFFLINE, target_name.c_str());
    }

    return true;
}

bool ChatHandler::HandleResetTalentsCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &target_guid, &target_name))
        return false;

    if (target)
    {
        target->ResetTalents(true);

        ChatHandler(target).SendSysMessage(LANG_RESET_TALENTS);
        if (!m_session || m_session->GetPlayer() != target)
            PSendSysMessage(LANG_RESET_TALENTS_ONLINE, GetNameLink(target).c_str());
    }
    else if (target_guid)
    {
        uint32 at_flags = AT_LOGIN_RESET_TALENTS;
        CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE guid = '%u'", at_flags, target_guid.GetCounter());
        std::string nameLink = playerLink(target_name);
        PSendSysMessage(LANG_RESET_TALENTS_OFFLINE, nameLink.c_str());
    }

    return true;
}

bool ChatHandler::HandleResetAllCommand(char* args)
{
    if (!*args)
        return false;

    std::string casename = args;

    AtLoginFlags atLogin;

    // Command specially created as single command to prevent using short case names
    if (casename == "spells")
    {
        atLogin = AT_LOGIN_RESET_SPELLS;
        sWorld.SendWorldText(LANG_RESETALL_SPELLS);
        if (!m_session)
            SendSysMessage(LANG_RESETALL_SPELLS);
    }
    else if (casename == "talents")
    {
        atLogin = AT_LOGIN_RESET_TALENTS;
        sWorld.SendWorldText(LANG_RESETALL_TALENTS);
        if (!m_session)
            SendSysMessage(LANG_RESETALL_TALENTS);
    }
    else
    {
        PSendSysMessage(LANG_RESETALL_UNKNOWN_CASE, args);
        SetSentErrorMessage(true);
        return false;
    }

    CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '%u' WHERE (at_login & '%u') = '0'", atLogin, atLogin);
    HashMapHolder<Player>::MapType const& plist = sObjectAccessor.GetPlayers();
    for (const auto& itr : plist)
        itr.second->SetAtLoginFlag(atLogin);

    return true;
}

bool ChatHandler::HandleServerResetAllRaidCommand(char* /*args*/)
{
    SendSysMessage("Global raid instances reset, all players in raid instances will be teleported to homebind!");
    sMapPersistentStateMgr.GetScheduler().ResetAllRaid();
    return true;
}

bool ChatHandler::HandleServerShutDownCancelCommand(char* /*args*/)
{
    sWorld.ShutdownCancel();
    return true;
}

bool ChatHandler::HandleServerShutDownCommand(char* args)
{
    uint32 delay;
    if (!ExtractUInt32(&args, delay))
        return false;

    uint32 exitcode;
    if (!ExtractOptUInt32(&args, exitcode, SHUTDOWN_EXIT_CODE))
        return false;

    // Exit code should be in range of 0-125, 126-255 is used
    // in many shells for their own return codes and code > 255
    // is not supported in many others
    if (exitcode > 125)
        return false;

    sWorld.ShutdownServ(delay, 0, exitcode);
    return true;
}

bool ChatHandler::HandleServerRestartCommand(char* args)
{
    uint32 delay;
    if (!ExtractUInt32(&args, delay))
        return false;

    uint32 exitcode;
    if (!ExtractOptUInt32(&args, exitcode, RESTART_EXIT_CODE))
        return false;

    // Exit code should be in range of 0-125, 126-255 is used
    // in many shells for their own return codes and code > 255
    // is not supported in many others
    if (exitcode > 125)
        return false;

    sWorld.ShutdownServ(delay, SHUTDOWN_MASK_RESTART, exitcode);
    return true;
}

bool ChatHandler::HandleServerIdleRestartCommand(char* args)
{
    uint32 delay;
    if (!ExtractUInt32(&args, delay))
        return false;

    uint32 exitcode;
    if (!ExtractOptUInt32(&args, exitcode, RESTART_EXIT_CODE))
        return false;

    // Exit code should be in range of 0-125, 126-255 is used
    // in many shells for their own return codes and code > 255
    // is not supported in many others
    if (exitcode > 125)
        return false;

    sWorld.ShutdownServ(delay, SHUTDOWN_MASK_RESTART | SHUTDOWN_MASK_IDLE, exitcode);
    return true;
}

bool ChatHandler::HandleServerIdleShutDownCommand(char* args)
{
    uint32 delay;
    if (!ExtractUInt32(&args, delay))
        return false;

    uint32 exitcode;
    if (!ExtractOptUInt32(&args, exitcode, SHUTDOWN_EXIT_CODE))
        return false;

    // Exit code should be in range of 0-125, 126-255 is used
    // in many shells for their own return codes and code > 255
    // is not supported in many others
    if (exitcode > 125)
        return false;

    sWorld.ShutdownServ(delay, SHUTDOWN_MASK_IDLE, exitcode);
    return true;
}

bool ChatHandler::HandleQuestAddCommand(char* args)
{
    // .addquest #entry'
    // number or [name] Shift-click form |color|Hquest:quest_id:quest_level|h[name]|h|r
    uint32 entry;
    if (!ExtractUint32KeyFromLink(&args, "Hquest", entry))
        return false;

    Player* player;
    if (!ExtractPlayerTarget(&args, &player, nullptr, nullptr))
        return false;

    Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);
    if (!pQuest)
    {
        PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
        SetSentErrorMessage(true);
        return false;
    }

    // check item starting quest (it can work incorrectly if added without item in inventory)
    for (auto const& itr : sObjectMgr.GetItemPrototypeMap())
    {
        ItemPrototype const *pProto = &itr.second;
        if (!pProto)
            continue;

        if (pProto->StartQuest == entry)
        {
            PSendSysMessage(LANG_COMMAND_QUEST_STARTFROMITEM, entry, pProto->ItemId);
            SetSentErrorMessage(true);
            return false;
        }
    }

    // ok, normal (creature/GO starting) quest
    if (player->CanAddQuest(pQuest, true))
    {
        player->AddQuest(pQuest, nullptr);

        if (player->CanCompleteQuest(entry))
            player->CompleteQuest(entry);
    }

    PSendSysMessage("Quest %u added for %s.", entry, player->GetName());
    return true;
}

bool ChatHandler::HandleQuestRemoveCommand(char* args)
{
    // .removequest #entry'
    // number or [name] Shift-click form |color|Hquest:quest_id:quest_level|h[name]|h|r
    uint32 entry;
    if (!ExtractUint32KeyFromLink(&args, "Hquest", entry))
        return false;

    Player* player;
    if (!ExtractPlayerTarget(&args, &player, nullptr, nullptr))
        return false;

    Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

    if (!pQuest)
    {
        PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
        SetSentErrorMessage(true);
        return false;
    }

    player->RemoveQuest(entry);

    // set quest status to not started (will updated in DB at next save)
    player->SetQuestStatus(entry, QUEST_STATUS_NONE);

    // reset rewarded for restart repeatable quest
    player->getQuestStatusMap()[entry].m_rewarded = false;

    SendSysMessage(LANG_COMMAND_QUEST_REMOVED);
    return true;
}

// WTB std::optional
QuestStatusData HandleQuestStatusCommandHelper(uint32 quest_id, const Player* player,
    const ObjectGuid* guid)
{
    QuestStatusData data;
    data.m_status = QuestStatus::QUEST_STATUS_NONE;

    if (player)
    {
        auto tmpQuestData = player->GetQuestStatusData(quest_id);

        if (tmpQuestData)
        {
            data = *tmpQuestData;
        }
    }
    else
    {
        std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery(
            "SELECT status, rewarded, reward_choice FROM character_queststatus WHERE guid = %u AND quest = %u",
            guid->GetCounter(), quest_id
        ));

        if (!result || result->GetRowCount() == 0)
        {
            return data;
        }

        auto fields = result->Fetch();
        data.m_status = QuestStatus(fields[0].GetUInt32());
        data.m_rewarded = fields[1].GetBool();
        data.m_reward_choice = fields[2].GetUInt32();
    }

    return data;
}

bool ChatHandler::HandleQuestStatusCommand(char* args)
{
    // .quest complete #entry
    // number or [name] Shift-click form |color|Hquest:quest_id:quest_level|h[name]|h|r
    uint32 entry;
    if (!ExtractUint32KeyFromLink(&args, "Hquest", entry))
        return false;

    Player* player;
    std::string name;
    ObjectGuid guid;

    if (!ExtractPlayerTarget(&args, &player, &guid, &name))
        return false;

    Quest const* quest = sObjectMgr.GetQuestTemplate(entry);

    if (!quest)
    {
        PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
        SetSentErrorMessage(true);
        return false;
    }

    QuestStatusData questData = HandleQuestStatusCommandHelper(entry, player, &guid);

    std::stringstream rewardInfo;

    if (questData.m_rewarded && questData.m_reward_choice)
    {
        rewardInfo << "item: " << questData.m_reward_choice << " ";

        const auto itemProto = sObjectMgr.GetItemPrototype(questData.m_reward_choice);

        if (itemProto)
        {
            rewardInfo << "[" << itemProto->Name1 << "]";
        }
        else
        {
            rewardInfo << "[invalid item entry?]";
        }
    }
    else
    {
        rewardInfo << "no reward data recorded";
    }

    PSendSysMessage("Quest ID: %u - [%s] - Rewarded: %s (%s)", entry, QuestStatusToString(questData.m_status),
        questData.m_rewarded? "Yes" : "No", rewardInfo.str().c_str());

    Quest const* prevQuest = quest;

    uint8 pos = 1;
    while (prevQuest->GetPrevQuestId())
    {
        if (Quest const* quest = sObjectMgr.GetQuestTemplate(prevQuest->GetPrevQuestId()))
        {
            questData = HandleQuestStatusCommandHelper(quest->GetQuestId(), player, &guid);

            if (quest->GetNextQuestInChain() == prevQuest->GetQuestId())
            {
                uint32 id = quest->GetQuestId();
                PSendSysMessage("%u. [PreventInChain] Quest ID: %u - [%s]", pos, id, QuestStatusToString(questData.m_status));
                prevQuest = quest;
            }
            else
            {
                uint32 id = quest->GetQuestId();
                PSendSysMessage("%u. [Prevent] Quest ID: %u - [%s]", pos, id, QuestStatusToString(questData.m_status));
                prevQuest = quest;
            }
        } else break;
        ++pos;
    }
    return true;
}

bool ChatHandler::HandleQuestCompleteCommand(char* args)
{
    // .quest complete #entry
    // number or [name] Shift-click form |color|Hquest:quest_id:quest_level|h[name]|h|r
    uint32 entry;
    if (!ExtractUint32KeyFromLink(&args, "Hquest", entry))
        return false;

    Player* player;
    if (!ExtractPlayerTarget(&args, &player, nullptr, nullptr))
        return false;

    Quest const* pQuest = sObjectMgr.GetQuestTemplate(entry);

    // If player doesn't have the quest
    if (!pQuest || player->GetQuestStatus(entry) == QUEST_STATUS_NONE)
    {
        PSendSysMessage(LANG_COMMAND_QUEST_NOTFOUND, entry);
        SetSentErrorMessage(true);
        return false;
    }
    player->FullQuestComplete(entry);

    PSendSysMessage("Quest %u completed for %s.", entry, player->GetName());
    return true;
}


bool ChatHandler::HandleBanAccountCommand(char* args)
{
    return HandleBanHelper(BAN_ACCOUNT, args);
}

bool ChatHandler::HandleBanCharacterCommand(char* args)
{
    return HandleBanHelper(BAN_CHARACTER, args);
}

bool ChatHandler::HandleBanIPCommand(char* args)
{
    return HandleBanHelper(BAN_IP, args);
}

bool ChatHandler::HandleBanAllIPCommand(char* args)
{
    char* ipStr = ExtractQuotedOrLiteralArg(&args);
    if (!ipStr)
        return false;

    char const* reason = ExtractQuotedOrLiteralArg(&args);
    if (!reason)
        reason = "<no reason given>";

    uint32 maxLevel = 10;
    uint32 minId = GetAccessLevel() < SEC_ADMINISTRATOR ? 100 : 0; // Don't show GM accounts

    std::string ip = ipStr;
    LoginDatabase.escape_string(ip);
    QueryResult* result = LoginDatabase.PQuery("SELECT id, username FROM account WHERE id >= %u AND last_ip " _LIKE_ " " _CONCAT2_("'%s'", "'%%'"), minId, ip.c_str());
    if (!result)
    {
        PSendSysMessage("No account found on IP '%s'", ip.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    std::set<uint32> accountsToBan;
    std::map<uint32, std::string> accountsIdToName;
    std::stringstream allAccounts;
    do
    {
        Field* fields = result->Fetch();
        if (!allAccounts.str().empty())
            allAccounts << ",";

        allAccounts << fields[0].GetUInt32();
        accountsToBan.insert(fields[0].GetUInt32());
        accountsIdToName[fields[0].GetUInt32()] = fields[1].GetCppString();
    } while (result->NextRow());

    delete result;
    if (result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE account IN (%s) AND level > %u GROUP BY account", allAccounts.str().c_str(), maxLevel))
    {
        do
        {
            Field* fields = result->Fetch();
            accountsToBan.erase(fields[0].GetUInt32());
        } while (result->NextRow());
        delete result;
    }

    uint32 bannedCount = 0;
    for (const auto it : accountsToBan)
    {
        if (sAccountMgr.IsAccountBanned(it))
            continue;

        sWorld.BanAccount(BAN_ACCOUNT, accountsIdToName[it], 0, reason, m_session ? m_session->GetPlayerName() : "");
        PSendSysMessage("Account '%s' permanently banned.", accountsIdToName[it].c_str(), reason);
        ++bannedCount;
    }

    PSendSysMessage("%u accounts banned for %s (%u on this IP)", bannedCount, reason, accountsIdToName.size());
    return true;
}

bool ChatHandler::HandleBanHelper(BanMode mode, char* args)
{
    if (!*args)
        return false;

    char* cnameOrIP = ExtractArg(&args);
    if (!cnameOrIP)
        return false;

    std::string nameOrIP = cnameOrIP;

    char* duration = ExtractArg(&args);                     // time string
    if (!duration)
        return false;

    uint32 duration_secs = TimeStringToSecs(duration);

    char* cReason = ExtractArg(&args);
    if (!cReason)
        return false;

    std::string reason(cReason);

    switch (mode)
    {
        case BAN_ACCOUNT:
            if (!AccountMgr::normalizeString(nameOrIP))
            {
                PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, nameOrIP.c_str());
                SetSentErrorMessage(true);
                return false;
            }
            break;
        case BAN_CHARACTER:
            if (!normalizePlayerName(nameOrIP))
            {
                SendSysMessage(LANG_PLAYER_NOT_FOUND);
                SetSentErrorMessage(true);
                return false;
            }
            break;
        case BAN_IP:
            if (!IsIPAddress(nameOrIP.c_str()))
                return false;
            break;
    }

    sWorld.BanAccount(mode, nameOrIP, duration_secs, reason, m_session ? m_session->GetPlayerName() : "");

    return true;
}

bool ChatHandler::HandleBanFingerprintCommand(char* args)
{
    if (!*args)
        return false;

    uint32 fingerprint;
    if (!ExtractUInt32(&args, fingerprint))
        return false;

    char* duration = ExtractArg(&args);                     // time string
    if (!duration)
        return false;

    uint32 duration_secs = TimeStringToSecs(duration);

    char* cReason = ExtractArg(&args);
    if (!cReason)
        return false;

    std::string reason(cReason);

    LoginDatabase.escape_string(reason);
    std::string safe_author = GetSession() ? GetSession()->GetPlayerName() : "CONSOLE";
    LoginDatabase.escape_string(safe_author);
    LoginDatabase.PExecute("REPLACE INTO `fingerprint_banned` (`fingerprint`, `bandate`, `unbandate`, `bannedby`, `banreason`) VALUES (%u,UNIX_TIMESTAMP(),UNIX_TIMESTAMP()+%u,'%s','%s')", fingerprint, duration_secs, safe_author.c_str(), reason.c_str());
    sAccountMgr.BanFingerprint(fingerprint, sWorld.GetGameTime() + duration_secs);

    return sAccountMgr.BanAccountsWithFingerprint(fingerprint, duration_secs, reason, this);
}

void ChatHandler::SendBanResult(BanMode mode, BanReturn result, std::string& banTarget, uint32 duration_secs, std::string& reason)
{
    switch (result)
    {
        case BAN_SUCCESS:
            if (duration_secs > 0)
                PSendSysMessage(LANG_BAN_YOUBANNED, banTarget.c_str(), secsToTimeString(duration_secs, true).c_str(), reason.c_str());
            else
                PSendSysMessage(LANG_BAN_YOUPERMBANNED, banTarget.c_str(), reason.c_str());
            break;
        case BAN_SYNTAX_ERROR:
            return;
        case BAN_NOTFOUND:
            switch (mode)
            {
            default:
                PSendSysMessage(LANG_BAN_NOTFOUND, "account", banTarget.c_str());
                break;
            case BAN_CHARACTER:
                PSendSysMessage(LANG_BAN_NOTFOUND, "character", banTarget.c_str());
                break;
            case BAN_IP:
                PSendSysMessage(LANG_BAN_NOTFOUND, "ip", banTarget.c_str());
                break;
            }
            SetSentErrorMessage(true);
            return;
    }
}

bool ChatHandler::HandleUnBanAccountCommand(char* args)
{
    return HandleUnBanHelper(BAN_ACCOUNT, args);
}

bool ChatHandler::HandleUnBanCharacterCommand(char* args)
{
    return HandleUnBanHelper(BAN_CHARACTER, args);
}

bool ChatHandler::HandleUnBanIPCommand(char* args)
{
    return HandleUnBanHelper(BAN_IP, args);
}

bool ChatHandler::HandleUnBanFingerprintCommand(char* args)
{
    if (!*args)
        return false;

    uint32 fingerprint;
    if (!ExtractUInt32(&args, fingerprint))
        return false;

    LoginDatabase.PExecute("DELETE FROM `fingerprint_banned` WHERE `fingerprint`=%u", fingerprint);
    sAccountMgr.UnbanFingerprint(fingerprint);
    PSendSysMessage("Fingerprint %u unbanned.", fingerprint);

    return true;
}

bool ChatHandler::HandleUnBanHelper(BanMode mode, char* args)
{
    if (!*args)
        return false;

    char* cnameOrIP = ExtractArg(&args);
    if (!cnameOrIP)
        return false;

    std::string nameOrIP = cnameOrIP;

    char* message = ExtractQuotedOrLiteralArg(&args);
    if (!message)
        return false;

    std::string unbanMessage(message);

    switch (mode)
    {
        case BAN_ACCOUNT:
            if (!AccountMgr::normalizeString(nameOrIP))
            {
                PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, nameOrIP.c_str());
                SetSentErrorMessage(true);
                return false;
            }
            break;
        case BAN_CHARACTER:
            if (!normalizePlayerName(nameOrIP))
            {
                SendSysMessage(LANG_PLAYER_NOT_FOUND);
                SetSentErrorMessage(true);
                return false;
            }
            break;
        case BAN_IP:
            if (!IsIPAddress(nameOrIP.c_str()))
                return false;
            break;
    }

    std::string source = m_session ? m_session->GetPlayerName() : "CONSOLE";

    if (sWorld.RemoveBanAccount(mode, source, unbanMessage, nameOrIP))
        PSendSysMessage(LANG_UNBAN_UNBANNED, nameOrIP.c_str(), unbanMessage.c_str());
    else
        PSendSysMessage(LANG_UNBAN_ERROR, nameOrIP.c_str());

    return true;
}

bool ChatHandler::HandleBanInfoAccountCommand(char* args)
{
    if (!*args)
        return false;

    std::string accountName;
    uint32 accountId = ExtractAccountId(&args, &accountName, nullptr, false);
    if (!accountId)
        return false;

    bool banResult = HandleBanInfoHelper(accountId, accountName.c_str());
    bool muteResult = HandleMuteHistoryHelper(accountId, accountName.c_str());
    return banResult || muteResult;
}

bool ChatHandler::HandleBanInfoCharacterCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    if (!ExtractPlayerTarget(&args, &target, &target_guid,nullptr,true))
        return false;

    uint32 accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr.GetPlayerAccountIdByGUID(target_guid);

    std::string accountName;
    if (!sAccountMgr.GetName(accountId, accountName))
    {
        PSendSysMessage(LANG_BANINFO_NOCHARACTER);
        return true;
    }

    bool banResult = HandleBanInfoHelper(accountId, accountName.c_str());
    bool muteResult = HandleMuteHistoryHelper(accountId, accountName.c_str());
    return banResult || muteResult;
}

bool ChatHandler::HandleBanInfoHelper(uint32 accountId, char const* accountname)
{
    LoginDatabase.AsyncPQuery(&LookupPlayerHandler::HandleBanInfoCallback, std::make_pair(GetSession()->GetAccountId(), std::string(accountname)),
    "SELECT FROM_UNIXTIME(bandate), unbandate-bandate, active, unbandate,banreason,bannedby,COALESCE(name, \"NoRealm\") , gmlevel, banid "
    "FROM account_banned LEFT JOIN realmlist ON realmlist.id = realm "
    "WHERE account_banned.id = '%u' ORDER BY bandate ASC", accountId);
    return true;
}

bool ChatHandler::HandleMuteHistoryHelper(uint32 accountId, char const* accountname)
{
    LoginDatabase.AsyncPQuery(&LookupPlayerHandler::HandleMuteHistoryCallback, std::make_pair(GetSession()->GetAccountId(), std::string(accountname)),
        //      0                        1                    2           3           4
        "SELECT FROM_UNIXTIME(mutedate), unmutedate-mutedate, unmutedate, mutereason, mutedby "
        "FROM account_muted "
        "WHERE id = '%u' ORDER BY mutedate ASC", accountId);
    return true;
}

bool ChatHandler::HandleBanInfoIPCommand(char* args)
{
    if (!*args)
        return false;

    char* cIP = ExtractQuotedOrLiteralArg(&args);
    if (!cIP)
        return false;

    if (!IsIPAddress(cIP))
        return false;

    std::string IP = cIP;

    LoginDatabase.escape_string(IP);
    QueryResult *result = LoginDatabase.PQuery("SELECT ip, FROM_UNIXTIME(bandate), FROM_UNIXTIME(unbandate), unbandate-UNIX_TIMESTAMP(), banreason,bannedby,unbandate-bandate FROM ip_banned WHERE ip = '%s'", IP.c_str());
    if (!result)
    {
        PSendSysMessage(LANG_BANINFO_NOIP);
        return true;
    }

    Field *fields = result->Fetch();
    bool permanent = !fields[6].GetUInt64();
    PSendSysMessage(LANG_BANINFO_IPENTRY,
                    fields[0].GetString(), fields[1].GetString(), permanent ? GetMangosString(LANG_BANINFO_NEVER) : fields[2].GetString(),
                    permanent ? GetMangosString(LANG_BANINFO_INFINITE) : secsToTimeString(fields[3].GetUInt64(), true).c_str(), fields[4].GetString(), fields[5].GetString());
    delete result;
    return true;
}

bool ChatHandler::HandleBanListCharacterCommand(char* args)
{
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");

    char* cFilter = ExtractLiteralArg(&args);
    if (!cFilter)
        return false;

    std::string filter = cFilter;
    CharacterDatabase.escape_string(filter);
    QueryResult* result = CharacterDatabase.PQuery("SELECT account FROM characters WHERE name " _LIKE_ " " _CONCAT2_("'%s'", "'%%'"), filter.c_str());
    if (!result)
    {
        PSendSysMessage(LANG_BANLIST_NOCHARACTER);
        return true;
    }

    return HandleBanListHelper(result);
}

bool ChatHandler::HandleBanListAccountCommand(char* args)
{
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");

    char* cFilter = ExtractLiteralArg(&args);
    std::string filter = cFilter ? cFilter : "";
    LoginDatabase.escape_string(filter);

    QueryResult* result;

    if (filter.empty())
    {
        result = LoginDatabase.Query("SELECT account.id, username FROM account, account_banned"
                                     " WHERE account.id = account_banned.id AND active = 1 GROUP BY account.id");
    }
    else
    {
        result = LoginDatabase.PQuery("SELECT account.id, username FROM account, account_banned"
                                      " WHERE account.id = account_banned.id AND active = 1 AND username " _LIKE_ " " _CONCAT2_("'%s'", "'%%'") " GROUP BY account.id",
                                      filter.c_str());
    }

    if (!result)
    {
        PSendSysMessage(LANG_BANLIST_NOACCOUNT);
        return true;
    }

    return HandleBanListHelper(result);
}

bool ChatHandler::HandleBanListHelper(QueryResult* result)
{
    PSendSysMessage(LANG_BANLIST_MATCHINGACCOUNT);

    // Chat short output
    if (m_session)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 accountid = fields[0].GetUInt32();

            QueryResult* banresult = LoginDatabase.PQuery("SELECT account.username FROM account,account_banned WHERE account_banned.id='%u' AND account_banned.id=account.id", accountid);
            if (banresult)
            {
                Field* fields2 = banresult->Fetch();
                PSendSysMessage("%s", fields2[0].GetString());
                delete banresult;
            }
        }
        while (result->NextRow());
    }
    // Console wide output
    else
    {
        SendSysMessage(LANG_BANLIST_ACCOUNTS);
        SendSysMessage("===============================================================================");
        SendSysMessage(LANG_BANLIST_ACCOUNTS_HEADER);
        do
        {
            SendSysMessage("-------------------------------------------------------------------------------");
            Field *fields = result->Fetch();
            uint32 account_id = fields[0].GetUInt32();

            std::string account_name;

            // "account" case, name can be get in same query
            if (result->GetFieldCount() > 1)
                account_name = fields[1].GetCppString();
            // "character" case, name need extract from another DB
            else
                sAccountMgr.GetName(account_id, account_name);

            // No SQL injection. id is uint32.
            QueryResult *banInfo = LoginDatabase.PQuery("SELECT bandate,unbandate,bannedby,banreason FROM account_banned WHERE id = %u ORDER BY unbandate", account_id);
            if (banInfo)
            {
                Field *fields2 = banInfo->Fetch();
                do
                {
                    time_t t_ban = fields2[0].GetUInt64();
                    tm* aTm_ban = localtime(&t_ban);

                    if (fields2[0].GetUInt64() == fields2[1].GetUInt64())
                    {
                        PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|   permanent  |%-15.15s|%-15.15s|",
                                        account_name.c_str(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                        fields2[2].GetString(), fields2[3].GetString());
                    }
                    else
                    {
                        time_t t_unban = fields2[1].GetUInt64();
                        tm* aTm_unban = localtime(&t_unban);
                        PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|%02d-%02d-%02d %02d:%02d|%-15.15s|%-15.15s|",
                                        account_name.c_str(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                        aTm_unban->tm_year % 100, aTm_unban->tm_mon + 1, aTm_unban->tm_mday, aTm_unban->tm_hour, aTm_unban->tm_min,
                                        fields2[2].GetString(), fields2[3].GetString());
                    }
                }
                while (banInfo->NextRow());
                delete banInfo;
            }
        }
        while (result->NextRow());
        SendSysMessage("===============================================================================");
    }

    delete result;
    return true;
}

bool ChatHandler::HandleBanListIPCommand(char* args)
{
    LoginDatabase.Execute("DELETE FROM ip_banned WHERE unbandate<=UNIX_TIMESTAMP() AND unbandate<>bandate");

    char* cFilter = ExtractLiteralArg(&args);
    std::string filter = cFilter ? cFilter : "";
    LoginDatabase.escape_string(filter);

    QueryResult* result;

    if (filter.empty())
    {
        result = LoginDatabase.Query("SELECT ip,bandate,unbandate,bannedby,banreason FROM ip_banned"
                                     " WHERE (bandate=unbandate OR unbandate>UNIX_TIMESTAMP())"
                                     " ORDER BY unbandate");
    }
    else
    {
        result = LoginDatabase.PQuery("SELECT ip,bandate,unbandate,bannedby,banreason FROM ip_banned"
                                      " WHERE (bandate=unbandate OR unbandate>UNIX_TIMESTAMP()) AND ip " _LIKE_ " " _CONCAT2_("'%s'", "'%%'")
                                      " ORDER BY unbandate", filter.c_str());
    }

    if (!result)
    {
        PSendSysMessage(LANG_BANLIST_NOIP);
        return true;
    }

    PSendSysMessage(LANG_BANLIST_MATCHINGIP);
    // Chat short output
    if (m_session)
    {
        do
        {
            Field* fields = result->Fetch();
            PSendSysMessage("%s", fields[0].GetString());
        }
        while (result->NextRow());
    }
    // Console wide output
    else
    {
        SendSysMessage(LANG_BANLIST_IPS);
        SendSysMessage("===============================================================================");
        SendSysMessage(LANG_BANLIST_IPS_HEADER);
        do
        {
            SendSysMessage("-------------------------------------------------------------------------------");
            Field *fields = result->Fetch();
            time_t t_ban = fields[1].GetUInt64();
            tm* aTm_ban = localtime(&t_ban);
            if (fields[1].GetUInt64() == fields[2].GetUInt64())
            {
                PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|   permanent  |%-15.15s|%-15.15s|",
                                fields[0].GetString(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                fields[3].GetString(), fields[4].GetString());
            }
            else
            {
                time_t t_unban = fields[2].GetUInt64();
                tm* aTm_unban = localtime(&t_unban);
                PSendSysMessage("|%-15.15s|%02d-%02d-%02d %02d:%02d|%02d-%02d-%02d %02d:%02d|%-15.15s|%-15.15s|",
                                fields[0].GetString(), aTm_ban->tm_year % 100, aTm_ban->tm_mon + 1, aTm_ban->tm_mday, aTm_ban->tm_hour, aTm_ban->tm_min,
                                aTm_unban->tm_year % 100, aTm_unban->tm_mon + 1, aTm_unban->tm_mday, aTm_unban->tm_hour, aTm_unban->tm_min,
                                fields[3].GetString(), fields[4].GetString());
            }
        }
        while (result->NextRow());
        SendSysMessage("===============================================================================");
    }

    delete result;
    return true;
}

bool ChatHandler::HandleWarnCharacterCommand(char* args)
{
    Player* target;
    ObjectGuid playerGuid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &playerGuid, &target_name))
    {
        PSendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    PlayerCacheData const* playerData = sObjectMgr.GetPlayerDataByGUID(playerGuid.GetCounter());

    if (!playerData)
    {
        PSendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    std::string authorName = m_session ? m_session->GetPlayerName() : "Console";

    char* reason = ExtractQuotedOrLiteralArg(&args);

    if (!reason)
    {
        PSendSysMessage("You must provide a reason. Please try again.");
        SetSentErrorMessage(true);
        return false;
    }

    MangosStrings mstring = LANG_WARN_INFORM;
    sWorld.WarnAccount(playerData->uiAccount, authorName, reason, "WARN");
    sAccountMgr.WarnAccount(playerData->uiAccount, reason);

    if (target)
    {
        if (mstring == LANG_WARN_INFORM)
        {
            ChatHandler(target->GetSession()).PSendSysMessage(mstring, reason);
            target->GetSession()->SendNotification(mstring, reason);

            PSendSysMessage("|cFFFF1100 Account #%u (Character: %s) has been warned for: |r \n \"%s\"",
                playerData->uiAccount, playerData->sName.c_str(), reason);
        }
        else
        {
            ChatHandler(target->GetSession()).PSendSysMessage(("|cFFFF1100 " + (std::string)GetMangosString(mstring) + "|r").c_str());
            target->GetSession()->SendNotification(mstring);
            // full warn text for GM
            /*PSendSysMessage("|cFFFF1100 Account #%u (character %s) has been warned (warn id:%u) for |r \n \"%s\"",
                playerData->uiAccount, playerData->sName.c_str(), mstring_id_reason, (std::string("WARN: ") + (char*)GetMangosString(mstring_id_reason)).c_str());
            PSendSysMessage("|cFFFF1100 Account #%u (character %s): a note (warn id:%u) has been added |r \n \"%s\"",
                playerData->uiAccount, playerData->sName.c_str(), mstring_id_reason, (std::string("NOTE: ") + (char*)GetMangosString(mstring_id_reason)).c_str());*/
        }
    }

    PSendSysMessage("Player %s warned.", playerData->sName.c_str());

    return true;
}


bool ChatHandler::HandleRemoveWarnCommand(char* args)
{
    uint32 warningId = 0;
    if (!ExtractUInt32(&args, warningId))
    {
        SendSysMessage("Give a valid Warning ID.");
        return false;
    }

    sWorld.RemoveWarning(warningId);
    PSendSysMessage("Removed warning with ID: %u", warningId);

    return true;
}

bool ChatHandler::HandleRespawnCommand(char* /*args*/)
{
    Player* pl = m_session->GetPlayer();

    // accept only explicitly selected target (not implicitly self targeting case)
    Unit* target = GetSelectedUnit();
    if (pl->GetSelectionGuid() && target)
    {
        if (target->GetTypeId() != TYPEID_UNIT)
        {
            SendSysMessage(LANG_SELECT_CREATURE);
            SetSentErrorMessage(true);
            return false;
        }

        if (target->IsDead())
            ((Creature*)target)->Respawn();
        return true;
    }

    MaNGOS::RespawnDo u_do;
    MaNGOS::WorldObjectWorker<MaNGOS::RespawnDo> worker(u_do);
    Cell::VisitGridObjects(pl, worker, pl->GetMap()->GetVisibilityDistance());
    return true;
}

bool ChatHandler::HandleMovegensCommand(char* /*args*/)
{
    Unit* unit = GetSelectedUnit();
    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_MOVEGENS_LIST, (unit->GetTypeId() == TYPEID_PLAYER ? "Player" : "Creature"), unit->GetGUIDLow());

    MotionMaster* mm = unit->GetMotionMaster();
    float x, y, z;
    mm->GetDestination(x, y, z);
    for (MotionMaster::const_iterator itr = mm->begin(); itr != mm->end(); ++itr)
    {
        switch ((*itr)->GetMovementGeneratorType())
        {
            case IDLE_MOTION_TYPE:
                SendSysMessage(LANG_MOVEGENS_IDLE);
                break;
            case RANDOM_MOTION_TYPE:
                SendSysMessage(LANG_MOVEGENS_RANDOM);
                break;
            case WAYPOINT_MOTION_TYPE:
                SendSysMessage(LANG_MOVEGENS_WAYPOINT);
                break;
            case CONFUSED_MOTION_TYPE:
                SendSysMessage(LANG_MOVEGENS_CONFUSED);
                break;

            case CHASE_MOTION_TYPE:
            {
                Unit* target = nullptr;
                if (unit->GetTypeId() == TYPEID_PLAYER)
                    target = static_cast<ChaseMovementGenerator<Player> const*>(*itr)->GetTarget();
                else
                    target = static_cast<ChaseMovementGenerator<Creature> const*>(*itr)->GetTarget();

                if (!target)
                    SendSysMessage(LANG_MOVEGENS_CHASE_NULL);
                else if (target->GetTypeId() == TYPEID_PLAYER)
                    PSendSysMessage(LANG_MOVEGENS_CHASE_PLAYER, target->GetName(), target->GetGUIDLow());
                else
                    PSendSysMessage(LANG_MOVEGENS_CHASE_CREATURE, target->GetName(), target->GetGUIDLow());
                break;
            }
            case FOLLOW_MOTION_TYPE:
            {
                Unit* target = nullptr;
                if (unit->GetTypeId() == TYPEID_PLAYER)
                    target = static_cast<FollowMovementGenerator<Player> const*>(*itr)->GetTarget();
                else
                    target = static_cast<FollowMovementGenerator<Creature> const*>(*itr)->GetTarget();

                if (!target)
                    SendSysMessage(LANG_MOVEGENS_FOLLOW_NULL);
                else if (target->GetTypeId() == TYPEID_PLAYER)
                    PSendSysMessage(LANG_MOVEGENS_FOLLOW_PLAYER, target->GetName(), target->GetGUIDLow());
                else
                    PSendSysMessage(LANG_MOVEGENS_FOLLOW_CREATURE, target->GetName(), target->GetGUIDLow());
                break;
            }
            case HOME_MOTION_TYPE:
                if (unit->GetTypeId() == TYPEID_UNIT)
                    PSendSysMessage(LANG_MOVEGENS_HOME_CREATURE, x, y, z);
                else
                    SendSysMessage(LANG_MOVEGENS_HOME_PLAYER);
                break;
            case FLIGHT_MOTION_TYPE:
                SendSysMessage(LANG_MOVEGENS_FLIGHT);
                break;
            case POINT_MOTION_TYPE:
            {
                PSendSysMessage(LANG_MOVEGENS_POINT, x, y, z);
                break;
            }
            case FLEEING_MOTION_TYPE:
                SendSysMessage(LANG_MOVEGENS_FEAR);
                break;
            case DISTRACT_MOTION_TYPE:
                SendSysMessage(LANG_MOVEGENS_DISTRACT);
                break;
            default:
                PSendSysMessage(LANG_MOVEGENS_UNKNOWN, (*itr)->GetMovementGeneratorType());
                break;
        }
    }
    return true;
}

bool ChatHandler::HandleCastCommand(char* args)
{
    if (!*args)
        return false;

    Unit* target = GetSelectedUnit();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // Only allow admins to cast spells in other players
    if (GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
        target = GetSession()->GetPlayer();

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = ExtractSpellIdFromLink(&args);
    if (!spell)
        return false;

    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spell);
    if (!spellInfo)
        return false;

    if (!SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
        SetSentErrorMessage(true);
        return false;
    }

    bool triggered = ExtractLiteralArg(&args, "triggered") != nullptr;
    if (!triggered && *args)                                // can be fail also at syntax error
        return false;

    m_session->GetPlayer()->CastSpell(target, spell, triggered);

    return true;
}

bool ChatHandler::HandleCastTargetCommand(char* args)
{
    Creature* caster = GetSelectedCreature();

    if (!caster)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!caster->GetVictim())
    {
        SendSysMessage(LANG_SELECTED_TARGET_NOT_HAVE_VICTIM);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = ExtractSpellIdFromLink(&args);
    if (!spell || !sSpellMgr.GetSpellEntry(spell))
        return false;

    bool triggered = ExtractLiteralArg(&args, "triggered") != nullptr;
    if (!triggered && *args)                                // can be fail also at syntax error
        return false;

    caster->SetFacingToObject(m_session->GetPlayer());

    caster->CastSpell(caster->GetVictim(), spell, triggered);

    return true;
}

/*
ComeToMe command REQUIRED for 3rd party scripting library to have access to PointMovementGenerator
Without this function 3rd party scripting library will get linking errors (unresolved external)
when attempting to use the PointMovementGenerator
*/
bool ChatHandler::HandleComeToMeCommand(char *args)
{
    Creature* caster = GetSelectedCreature();

    if (!caster)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    Player* pl = m_session->GetPlayer();

    caster->GetMotionMaster()->MovePoint(0, pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), true);
    return true;
}

bool ChatHandler::HandleCastSelfCommand(char* args)
{
    if (!*args)
        return false;

    Unit* target = GetSelectedUnit();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = ExtractSpellIdFromLink(&args);
    if (!spell)
        return false;

    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spell);
    if (!spellInfo)
        return false;

    if (!SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
        SetSentErrorMessage(true);
        return false;
    }

    bool triggered = ExtractLiteralArg(&args, "triggered") != nullptr;
    if (!triggered && *args)                                // can be fail also at syntax error
        return false;

    target->CastSpell(target, spell, triggered);

    return true;
}

bool ChatHandler::HandleInstanceListBindsCommand(char* /*args*/)
{
    Player* player = GetSelectedPlayer();
    if (!player) player = m_session->GetPlayer();
    uint32 counter = 0;

    Player::BoundInstancesMap &binds = player->GetBoundInstances();
    for (const auto& bind : binds)
    {
        DungeonPersistentState* state = bind.second.state;
        std::string timeleft;
        if (!bind.second.perm)
            timeleft = secsToTimeString(state->GetResetTime() - time(nullptr), true);
        else
            timeleft = secsToTimeString(sMapPersistentStateMgr.GetScheduler().GetResetTimeFor(bind.first) - time(nullptr));

        if (MapEntry const* entry = sMapStorage.LookupEntry<MapEntry>(bind.first))
        {
            PSendSysMessage("map: %d (%s) inst: %d perm: %s canReset: %s TTR: %s",
                            bind.first, entry->name, state->GetInstanceId(), bind.second.perm ? "yes" : "no",
                            state->CanReset() ? "yes" : "no", timeleft.c_str());
        }
        else
            PSendSysMessage("bound for a nonexistent map %u", bind.first);
        counter++;
    }

    PSendSysMessage("player binds: %d", counter);
    counter = 0;

    if (Group *group = player->GetGroup())
    {
        Group::BoundInstancesMap &binds = group->GetBoundInstances();
        for (const auto& bind : binds)
        {
            DungeonPersistentState* state = bind.second.state;
            std::string timeleft;
            if (!bind.second.perm)
                timeleft = secsToTimeString(state->GetResetTime() - time(nullptr), true);
            else
                timeleft = secsToTimeString(sMapPersistentStateMgr.GetScheduler().GetResetTimeFor(bind.first) - time(nullptr));

            if (MapEntry const* entry = sMapStorage.LookupEntry<MapEntry>(bind.first))
            {
                PSendSysMessage("map: %d (%s) inst: %d perm: %s canReset: %s TTR: %s",
                                bind.first, entry->name, state->GetInstanceId(), bind.second.perm ? "yes" : "no",
                                state->CanReset() ? "yes" : "no", timeleft.c_str());
            }
            else
                PSendSysMessage("bound for a nonexistent map %u", bind.first);
            counter++;
        }
    }

    PSendSysMessage("group binds: %d", counter);

    return true;
}

void ChatHandler::HandleInstanceUnbindHelper(Player* player, bool got_map, uint32 mapid)
{
    if (!player || !player->IsInWorld())
        return;

    uint32 counter = 0;

    Player::BoundInstancesMap &binds = player->GetBoundInstances();
    for (Player::BoundInstancesMap::iterator itr = binds.begin(); itr != binds.end();)
    {
        if (got_map && mapid != itr->first)
        {
            ++itr;
            continue;
        }

        if (itr->first != player->GetMapId())
        {
            DungeonPersistentState *save = itr->second.state;
            std::string timeleft = secsToTimeString(save->GetResetTime() - time(nullptr), true);

            if (MapEntry const* entry = sMapStorage.LookupEntry<MapEntry>(itr->first))
            {
                ChatHandler(player).PSendSysMessage("unbinding map: %d (%s) inst: %d perm: %s canReset: %s TTR: %s",
                    itr->first, entry->name, save->GetInstanceId(), itr->second.perm ? "yes" : "no",
                    save->CanReset() ? "yes" : "no", timeleft.c_str());
            }
            else
                ChatHandler(player).PSendSysMessage("bound for a nonexistent map %u", itr->first);

            player->UnbindInstance(itr);
            counter++;
        }
        else
            ++itr;
    }

    ChatHandler(player).PSendSysMessage("instances unbound: %d", counter);
}

bool ChatHandler::HandleInstanceUnbindCommand(char* args)
{
    if (!*args)
        return false;

    Player* player = GetSelectedPlayer();
    if (!player || GetAccessLevel() < SEC_ADMINISTRATOR)
        player = m_session->GetPlayer();

    uint32 mapid = 0;
    bool got_map = false;

    if (strncmp(args, "all", strlen(args)) != 0)
    {
        if (!isNumeric(args[0]))
            return false;

        got_map = true;
        mapid = atoi(args);
    }

    HandleInstanceUnbindHelper(player, got_map, mapid);

    return true;
}

bool ChatHandler::HandleInstanceGroupUnbindCommand(char* args)
{
    if (!*args)
        return false;

    Player* player = GetSelectedPlayer();
    if (!player || player->InBattleGround())
        return false;

    uint32 mapid = 0;
    bool got_map = false;

    if (strncmp(args, "all", strlen(args)) != 0)
    {
        if (!isNumeric(args[0]))
            return false;

        got_map = true;
        mapid = atoi(args);
    }

    Group* pGroup = player->GetGroup();
    if (!pGroup)
    {
        std::string nameLink = GetNameLink(player);
        PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        if (Player* pMember = itr->getSource())
        {
            if (!pMember->IsInWorld())
                continue;

            HandleInstanceUnbindHelper(pMember, got_map, mapid);
        }
    }

    pGroup->Disband();

    SendSysMessage("Group unbound. Disbanding.");
    return true;
}

bool ChatHandler::HandleInstanceStatsCommand(char* /*args*/)
{
    PSendSysMessage("instances loaded: %d", sMapMgr.GetNumInstances());
    PSendSysMessage("players in instances: %d", sMapMgr.GetNumPlayersInInstances());

    uint32 numSaves, numBoundPlayers, numBoundGroups;
    sMapPersistentStateMgr.GetStatistics(numSaves, numBoundPlayers, numBoundGroups);
    PSendSysMessage("instance saves: %d", numSaves);
    PSendSysMessage("players bound: %d", numBoundPlayers);
    PSendSysMessage("groups bound: %d", numBoundGroups);
    return true;
}

/// Display the list of GMs
bool ChatHandler::HandleGMListFullCommand(char* /*args*/)
{
    ///- Get the accounts with GM Level >0
    QueryResult *result = LoginDatabase.Query("SELECT username, `rank` FROM account"
                          " WHERE rank > 0");
    if (result)
    {
        SendSysMessage(LANG_GMLIST);
        SendSysMessage("========================");
        SendSysMessage(LANG_GMLIST_HEADER);
        SendSysMessage("========================");

        ///- Circle through them. Display username and GM level
        do
        {
            Field *fields = result->Fetch();
            PSendSysMessage("|%15s|%6s|", fields[0].GetString(), fields[1].GetString());
        }
        while (result->NextRow());

        PSendSysMessage("========================");
        delete result;
    }
    else
        PSendSysMessage(LANG_GMLIST_EMPTY);
    return true;
}

bool ChatHandler::HandleGMOnlineListCommand(char* args)
{
    SendSysMessage("Online GMs:");
    SendSysMessage("========================");
    bool empty = true;
    const World::SessionMap& AllSessions = sWorld.GetAllSessions();
    for (const auto& [accId, session] : AllSessions)
    {
        if (!session || !session->GetPlayer() || session->GetSecurity() < SEC_OBSERVER)
            continue;

        if (empty)
            empty = false;

        std::string accName = "None";
        sAccountMgr.GetName(session->GetAccountId(), accName);
        PSendSysMessage("| Char: %15s | Level: %u | Acc: %15s|%s", session->GetPlayer()->GetName(), (uint32)session->GetSecurity(), accName.c_str(), sAccountMgr.IsTraineeGM(accId) ? " TRAINEE" : "");

    }

    if (empty)
        SendSysMessage("No GMs");
    else
        SendSysMessage("========================");

    return true;
}

/// Output list of character for account
bool ChatHandler::HandleAccountCharactersCommand(char* args)
{
    ///- Get the command line arguments
    std::string account_name;
    Player* target = nullptr;                                  // only for triggering use targeted player account
    uint32 account_id = ExtractAccountId(&args, &account_name, &target);
    if (!account_id)
        return false;

    ///- Get the characters for account id
    CharacterDatabase.AsyncPQuery(&PlayerSearchHandler::HandlePlayerCharacterLookupResult,
        GetAccountId(), 100u,
        "SELECT guid, name, race, class, level FROM characters WHERE account = %u",
        account_id);

    return true;
}

bool ChatHandler::HandleSendMailHelper(MailDraft& draft, char* args)
{
    // format: "subject text" "mail text"
    char* msgSubject = ExtractQuotedArg(&args);
    if (!msgSubject)
        return false;

    char* msgText = ExtractQuotedArg(&args);
    if (!msgText)
        return false;

    // msgSubject, msgText isn't NUL after prev. check
    draft.SetSubjectAndBody(msgSubject, msgText);

    return true;
}



bool ChatHandler::HandleSendItemsHelper(MailDraft& draft, char* args)
{
    // format: "subject text" "mail text" item1[:count1] item2[:count2] ... item12[:count12]
    char* msgSubject = ExtractQuotedArg(&args);
    if (!msgSubject)
        return false;

    char* msgText = ExtractQuotedArg(&args);
    if (!msgText)
        return false;

    // extract items
    typedef std::pair<uint32, uint32> ItemPair;
    std::vector<ItemPair> items;

    // get from tail next item str
    while (char* itemStr = ExtractArg(&args))
    {
        // parse item str
        uint32 item_id = 0;
        uint32 item_count = 1;
        if (sscanf(itemStr, "%u:%u", &item_id, &item_count) != 2)
            if (sscanf(itemStr, "%u", &item_id) != 1)
                return false;

        if (!item_id)
        {
            PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, item_id);
            SetSentErrorMessage(true);
            return false;
        }

        ItemPrototype const* item_proto = sObjectMgr.GetItemPrototype(item_id);
        if (!item_proto)
        {
            PSendSysMessage(LANG_COMMAND_ITEMIDINVALID, item_id);
            SetSentErrorMessage(true);
            return false;
        }

        if (item_count < 1 || (item_proto->MaxCount > 0 && item_count > uint32(item_proto->MaxCount)))
        {
            PSendSysMessage(LANG_COMMAND_INVALID_ITEM_COUNT, item_count, item_id);
            SetSentErrorMessage(true);
            return false;
        }

        while (item_count > item_proto->GetMaxStackSize())
        {
            items.push_back(ItemPair(item_id, item_proto->GetMaxStackSize()));
            item_count -= item_proto->GetMaxStackSize();
        }

        items.push_back(ItemPair(item_id, item_count));

        if (items.size() > MAX_MAIL_ITEMS)
        {
            PSendSysMessage(LANG_COMMAND_MAIL_ITEMS_LIMIT, MAX_MAIL_ITEMS);
            SetSentErrorMessage(true);
            return false;
        }
    }

    // fill mail
    draft.SetSubjectAndBody(msgSubject, msgText);

    for (const auto& itr : items)
    {
        if (Item* item = Item::CreateItem(itr.first, itr.second, m_session ? m_session->GetPlayer() : 0))
        {
            item->SaveToDB();                               // save for prevent lost at next mail load, if send fail then item will deleted
            draft.AddItem(item);
        }
    }

    return true;
}

bool ChatHandler::HandleSendItemsCommand(char* args)
{
    // format: name "subject text" "mail text" item1[:count1] item2[:count2] ... item12[:count12]
    Player* receiver;
    ObjectGuid receiver_guid;
    std::string receiver_name;
    if (!ExtractPlayerTarget(&args, &receiver, &receiver_guid, &receiver_name))
        return false;

    MailDraft draft;

    // fill mail
    if (!HandleSendItemsHelper(draft, args))
        return false;

    // from console show nonexistent sender
    MailSender sender(MAIL_NORMAL, m_session ? m_session->GetPlayer()->GetObjectGuid().GetCounter() : 0, MAIL_STATIONERY_GM);

    draft.SendMailTo(MailReceiver(receiver, receiver_guid), sender);

    std::string nameLink = playerLink(receiver_name);
    PSendSysMessage(LANG_MAIL_SENT, nameLink.c_str());
    return true;
}


bool ChatHandler::HandleSendMoneyHelper(MailDraft& draft, char* args)
{
    /// format: "subject text" "mail text" money

    char* msgSubject = ExtractQuotedArg(&args);
    if (!msgSubject)
        return false;

    char* msgText = ExtractQuotedArg(&args);
    if (!msgText)
        return false;

    uint32 money;
    if (!ExtractUInt32(&args, money))
        return false;

    if (money <= 0)
        return false;

    // msgSubject, msgText isn't NUL after prev. check
    draft.SetSubjectAndBody(msgSubject, msgText).SetMoney(money);

    return true;
}

bool ChatHandler::HandleSendMoneyCommand(char* args)
{
    /// format: name "subject text" "mail text" money

    Player* receiver;
    ObjectGuid receiver_guid;
    std::string receiver_name;
    if (!ExtractPlayerTarget(&args, &receiver, &receiver_guid, &receiver_name))
        return false;

    MailDraft draft;

    // fill mail
    if (!HandleSendMoneyHelper(draft, args))
        return false;

    // from console show nonexistent sender
    MailSender sender(MAIL_NORMAL, m_session ? m_session->GetPlayer()->GetObjectGuid().GetCounter() : 0, MAIL_STATIONERY_GM);

    draft.SendMailTo(MailReceiver(receiver, receiver_guid), sender);

    std::string nameLink = playerLink(receiver_name);
    PSendSysMessage(LANG_MAIL_SENT, nameLink.c_str());
    return true;
}


/// Send a message to a player in game
bool ChatHandler::HandleSendMessageCommand(char* args)
{
    ///- Find the player
    Player *rPlayer;
    if (!ExtractPlayerTarget(&args, &rPlayer))
        return false;

    ///- message
    if (!*args)
        return false;

    WorldSession* rPlayerSession = rPlayer->GetSession();

    ///- Check that he is not logging out.
    if (rPlayerSession->isLogingOut())
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    ///- Send the message
    //Use SendAreaTriggerMessage for fastest delivery.
    rPlayerSession->SendAreaTriggerMessage("%s", args);
    rPlayerSession->SendAreaTriggerMessage("|cffff0000[Message from administrator]:|r");

    //Confirmation message
    std::string nameLink = GetNameLink(rPlayer);
    PSendSysMessage(LANG_SENDMESSAGE, nameLink.c_str(), args);
    return true;
}

enum ServiceDeleteFlags
{
    SDF_NONE            = 0x00,
    SDF_MAX_LEVEL       = 0x01,
    SDF_MAX_MONEY       = 0x02,
    SDF_MAX_ITEMS       = 0x04,
    SDF_MAX_PLAYED_TIME = 0x08,
    SDF_PERM_BANNED     = 0x10,
    SDF_LAST_LOGIN      = 0x20,
    SDF_GM              = 0x40
};

bool ChatHandler::HandleServiceDeleteCharacters(char* args)
{
    uint32 flags = 0;
    if (!ExtractUInt32(&args, flags))
        return false;

    Tokens tokens = StrSplit(sConfig.GetStringDefault("LoginDatabase.Info", ""), ";");

    std::string dblogon;
    auto i = 0;
    for (auto& token : tokens)
    {
        if (i == 4)
        {
            dblogon = token;
            break;
        }
        else
            ++i;
    }

    std::ostringstream s;

    if (flags & SDF_PERM_BANNED)
    {
        s << "SELECT `guid`, `account` FROM `characters` WHERE `account` IN (SELECT `id` FROM `" << dblogon << "`.`account_banned` WHERE `bandate` = `unbandate` AND `active` = 1) ";

        uint32 maxLevel = 0;
        if (ExtractUInt32(&args, maxLevel))
            s << "AND `level` <= " << maxLevel << " ";
    }
    else if (flags & SDF_GM)
    {
        s << "SELECT `guid`, `account` FROM `characters` WHERE `account` IN (SELECT `id` FROM `" << dblogon << "`.`account` WHERE `gmlevel` > 0)";
    }
    else
    {
        s << "SELECT `characters`.`guid`,"
             "    `characters`.`account`,"
             "    `characters`.`level` AS `level`,"
             "    `characters`.`money` AS `money`,"
             "    `characters`.`totaltime` AS `totaltime`,"
             "    COUNT(`characters`.`guid`) AS `items`,"
             "    `characters`.`logout_time` AS `logout_time` "
             "FROM `characters` "
             "INNER JOIN `character_inventory` USING(`guid`) "
             "GROUP BY `characters`.`guid` HAVING ";

        bool _and = false;
        if (flags & SDF_MAX_LEVEL)
        {
            uint32 maxLevel = 0;
            if (!ExtractUInt32(&args, maxLevel))
                return false;

            s << (_and ? "AND " : "") << "`level` <= " << maxLevel << " ";
            _and = true;
        }

        if (flags & SDF_MAX_MONEY)
        {
            uint32 maxMoney = 0;
            if (!ExtractUInt32(&args, maxMoney))
                return false;

            s << (_and ? "AND " : "") << "`money` <= " << maxMoney << " ";
            _and = true;
        }

        if (flags & SDF_MAX_ITEMS)
        {
            uint32 maxItems = 0;
            if (!ExtractUInt32(&args, maxItems))
                return false;

            s << (_and ? "AND " : "") << "`items` <= " << maxItems << " ";
            _and = true;
        }

        if (flags & SDF_MAX_PLAYED_TIME)
        {
            uint32 maxPlayedTime = 0;
            if (!ExtractUInt32(&args, maxPlayedTime))
                return false;

            s << (_and ? "AND " : "") << "`totaltime` <= " << maxPlayedTime << " ";
            _and = true;
        }

        if (flags & SDF_LAST_LOGIN)
        {
            uint32 logoutTime = 0;
            if (!ExtractUInt32(&args, logoutTime))
                return false;

            s << (_and ? "AND " : "") << "`logout_time` <= " << logoutTime << " ";
            _and = true;
        }
    }

    QueryResult* result = CharacterDatabase.Query(s.str().c_str());
    uint32 count = 0;
    if (result)
    {
        count = result->GetRowCount();
        do
        {
            Field* fields = result->Fetch();
            uint32 lowGuid = fields[0].GetUInt32();
            uint32 accountId = fields[1].GetUInt32();

            ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, lowGuid);

            Player::DeleteFromDB(guid, accountId, true, true);
        } while (result->NextRow());

        delete result;
    }

    sLog.outString("Service: Removed %u characters", count);
    return true;
}

bool ChatHandler::HandleFastDebugCommand(char* /*args*/)
{
    if (m_session->GetPlayer()->SetupHardcoreMode())
        PSendSysMessage("Test hardcore mode succeeded");
    else
        PSendSysMessage("Test hardcore mode failed");

    /*uint32 item = 25;
    WorldPacket data(SMSG_ITEM_QUERY_SINGLE_RESPONSE, 4);
    data << uint32(item | 0x80000000);
    m_session->SendPacket(&data);*/

    return true;
}

bool ChatHandler::HandleDebugAnimCommand(char* args)
{
    uint32 emote_id;
    if (!ExtractUInt32(&args, emote_id))
        return false;

    m_session->GetPlayer()->HandleEmoteCommand(emote_id);
    return true;
}

bool ChatHandler::HandleDebugLootTableCommand(char* args)
{
    std::stringstream in(args);
    std::string tableName;
    int32 lootid = 0;
    int32 checkItem = 0;
    uint32 simCount = 0;
    in >> tableName >> lootid >> simCount >> checkItem;
    simCount = simCount ? simCount : 10000;
    SetSentErrorMessage(true);

    LootStore const* store = nullptr;
    if (tableName == "creature")
        store = &LootTemplates_Creature;
    else if (tableName == "reference")
        store = &LootTemplates_Reference;
    else if (tableName == "fishing")
        store = &LootTemplates_Fishing;
    else if (tableName == "gameobject")
        store = &LootTemplates_Gameobject;
    else if (tableName == "item")
        store = &LootTemplates_Item;
    else if (tableName == "mail")
        store = &LootTemplates_Mail;
    else if (tableName == "pickpocketing")
        store = &LootTemplates_Pickpocketing;
    else if (tableName == "skinning")
        store = &LootTemplates_Skinning;
    else if (tableName == "disenchant")
        store = &LootTemplates_Disenchant;
    else if (tableName == "enchant")
        return HandleDebugItemEnchantCommand(lootid, simCount);
    else
    {
        PSendSysMessage("Error: loot type \"%s\" unknown", tableName.c_str());
        return false;
    }

    LootTemplate const* tab = store->GetLootFor(lootid);
    if (!tab)
    {
        PSendSysMessage("Error: loot type \"%s\" has no lootid %u", tableName.c_str(), lootid);
        return false;
    }

    Player* lootOwner = GetSelectedPlayer();

    std::map<uint32, uint32> lootChances;
    if (checkItem)
        lootChances[checkItem] = 0;

    uint32 const MAX_TIME = 30;
    auto startTime = time(nullptr);

    for (uint32 i = 0; i < simCount; ++i)
    {
        Loot l(nullptr);
        if (lootOwner)
            l.SetTeam(lootOwner->GetTeam());

        tab->Process(l, *store, store->IsRatesAllowed());

        for (const auto& item : l.items)
            if (!lootOwner || !item.conditionId)
                lootChances[item.itemid]++;

        for (const auto& m_questItem : l.m_questItems)
            lootChances[m_questItem.itemid]++;

        if (lootOwner)
        {
            l.FillNotNormalLootFor(lootOwner);
            QuestItemMap::const_iterator itemsList = l.m_playerFFAItems.find(lootOwner->GetGUIDLow());

            if (itemsList != l.m_playerFFAItems.end())
                for (const auto& it : *itemsList->second)
                    lootChances[l.items[it.index].itemid]++;

            itemsList = l.m_playerQuestItems.find(lootOwner->GetGUIDLow());
            if (itemsList != l.m_playerQuestItems.end())
                for (const auto& it : *itemsList->second)
                    lootChances[l.m_questItems[it.index].itemid]++;

            itemsList = l.m_playerNonQuestNonFFAConditionalItems.find(lootOwner->GetGUIDLow());
            if (itemsList != l.m_playerNonQuestNonFFAConditionalItems.end())
                for (const auto& it : *itemsList->second)
                    lootChances[l.items[it.index].itemid]++;
        }

        if (i % 1000000 == 0) // check the time every million iterations
        {
            if (time(nullptr) - startTime > MAX_TIME)
            {
                PSendSysMessage("Error: Aborted loot simulation after %u runs for exceeding max allowed time of %us", i, MAX_TIME);
                simCount = i;
                break;
            }
        }
    }

    PSendSysMessage("%u items dropped after %u attempts for loot %s.%u", lootChances.size(), simCount, tableName.c_str(), lootid);
    for (const auto& itr : lootChances)
    {
        if (itr.first == checkItem || !checkItem)
        {
            ItemPrototype const* proto = sObjectMgr.GetItemPrototype(itr.first);
            if (!proto)
                continue;

            std::stringstream chance;
            chance << 100 * itr.second / float(simCount);
            chance << "%";

            if (m_session)
                PSendSysMessage(LANG_ITEM_LIST_CHAT, itr.first, itr.first, proto->Name1.c_str(), chance.str().c_str());
            else
                PSendSysMessage(LANG_ITEM_LIST_CONSOLE, itr.first, proto->Name1.c_str(), chance.str().c_str());
        }
    }

    return true;
}

bool ChatHandler::HandleLootHelperCommand(char* args)
{
    const auto activePlayer = m_session->GetPlayer();
    if (!activePlayer)
    {
        return false;
    }

    uint32_t entry;
    const auto target = GetSelectedCreature();
    if (target)
    {
        entry = target->GetEntry();
    }
    else
    {
        if (!ExtractUInt32(&args, entry))
        {
            PSendSysMessage("Error: no creature selected and no entry provided");
            return false;
        }
    }

    const auto position = activePlayer->GetPosition();
    const auto lootHelper = activePlayer->SummonCreature(1001000, position.x, position.y, position.z, position.o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 1000 * 60 * 5);
    npc_loothelperAI* loothelperAi = dynamic_cast<npc_loothelperAI*>(lootHelper->AI());
    if (!loothelperAi)
    {
        PSendSysMessage("System Error: failed to create loot helper");
        return true;
    }

    loothelperAi->SetCreatureEntry(entry);

    return true;
}

bool ChatHandler::HandleDebugItemEnchantCommand(int lootid, uint32 simCount)
{
    std::map<uint32, uint32> lootChances;
    uint32 const MAX_TIME = 30;
    auto startTime = time(nullptr);

    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(lootid);
    if (!proto)
    {
        PSendSysMessage("Error: invalid item id %u", lootid);
        return false;
    }

    if (!proto->RandomProperty)
    {
        PSendSysMessage("Error: item %u has no random enchantments", lootid);
        return false;
    }

    for (uint32 i = 0; i < simCount; ++i)
    {
        uint32 enchant = GetItemEnchantMod(proto->RandomProperty);
        lootChances[enchant]++;

        if (i % 1000000 == 0) // check the time every million iterations
        {
            if (time(nullptr) - startTime > MAX_TIME)
            {
                PSendSysMessage("Error: Aborted loot simulation after %u runs for exceeding max allowed time of %us", i, MAX_TIME);
                simCount = i;
                break;
            }
        }
    }

    PSendSysMessage("%u items dropped after %u attempts for item %s.", lootChances.size(), simCount, proto->Name1.c_str());
    for (const auto& itr : lootChances)
    {
        std::stringstream chance;
        chance << 100 * itr.second / float(simCount);
        chance << "%";

        ItemRandomPropertiesEntry const* randomProp = sItemRandomPropertiesStore.LookupEntry(itr.first);
        if (!randomProp)
            continue;

        if (m_session)
            PSendSysMessage(LANG_ITEM_LIST_CHAT, itr.first, lootid, randomProp->internalName, chance.str().c_str());
        else
            PSendSysMessage(LANG_ITEM_LIST_CONSOLE, itr.first, randomProp->internalName, chance.str().c_str());
    }

    return true;
}

bool ChatHandler::HandleGMTicketAssignToCommand(char* args)
{
    if (!*args)
        return false;

    char* ticketIdStr = strtok((char*)args, " ");
    uint32 ticketId = atoi(ticketIdStr);

    std::string target(GetSession() ? GetSession()->GetPlayerName() : "");
    char* targetStr = strtok(nullptr, " ");
    if (targetStr)
        target = targetStr;
    if (target.empty())
        return false;

    if (!normalizePlayerName(target))
        return false;

    GmTicket* ticket = sTicketMgr.GetTicket(ticketId);
    if (!ticket || ticket->IsClosed())
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }

    ObjectGuid targetGuid = sObjectMgr.GetPlayerGuidByName(target);

    uint32 accountId = sObjectMgr.GetPlayerAccountIdByGUID(targetGuid);

    // If already assigned, leave
    if (ticket->IsAssignedTo(targetGuid))
    {
        PSendSysMessage(LANG_COMMAND_TICKETASSIGNERROR_B, ticket->GetId());
        return true;
    }

    // If assigned to different player other than current, leave
    //! Console can override though
    Player* player = GetSession() ? GetSession()->GetPlayer() : nullptr;
    if (player && ticket->IsAssignedNotTo(player->GetGUID()))
    {
        PSendSysMessage(LANG_COMMAND_TICKETALREADYASSIGNED, ticket->GetId(), target.c_str());
        return true;
    }

    // Assign ticket
    ticket->SetAssignedTo(targetGuid, sAccountMgr.GetSecurity(accountId) == sWorld.getConfig(CONFIG_UINT32_GMTICKETS_ADMIN_SECURITY));
    ticket->SaveToDB();
    sTicketMgr.UpdateLastChange();

    std::string msg = ticket->FormatMessageString(*this, nullptr, target.c_str(), nullptr, nullptr, nullptr);
    sWorld.SendGMTicketText(msg.c_str());
    return true;
}

bool ChatHandler::HandleGMTicketUnAssignCommand(char* args)
{
    if (!*args)
        return false;

    uint32 ticketId = atoi(args);
    GmTicket* ticket = sTicketMgr.GetTicket(ticketId);
    if (!ticket || ticket->IsClosed())
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }
    // Ticket must be assigned
    if (!ticket->IsAssigned())
    {
        PSendSysMessage(LANG_COMMAND_TICKETNOTASSIGNED, ticket->GetId());
        return true;
    }

    // Get security level of player, whom this ticket is assigned to
    uint32 security = SEC_PLAYER;
    Player* assignedPlayer = ticket->GetAssignedPlayer();
    if (assignedPlayer)
        security = assignedPlayer->GetSession()->GetSecurity();
    else
    {
        ObjectGuid guid = ticket->GetAssignedToGUID();
        uint32 accountId = sObjectMgr.GetPlayerAccountIdByGUID(guid);
        security = sAccountMgr.GetSecurity(accountId);
    }

    // Check security
    //! If no m_session present it means we're issuing this command from the console
    uint32 mySecurity = GetSession() ? GetSession()->GetSecurity() : SEC_CONSOLE;
    if (security > mySecurity)
    {
        SendSysMessage(LANG_COMMAND_TICKETUNASSIGNSECURITY);
        return true;
    }

    std::string assignedTo = ticket->GetAssignedToName(); // copy assignedto name because we need it after the ticket has been unnassigned
    ticket->SetUnassigned();
    ticket->SaveToDB();
    sTicketMgr.UpdateLastChange();

    std::string msg = ticket->FormatMessageString(*this, nullptr, assignedTo.c_str(),
                                                  GetSession() ? GetSession()->GetPlayer()->GetName() : "Console", nullptr, nullptr);
    sWorld.SendGMTicketText(msg.c_str());

    return true;
}

bool ChatHandler::HandleGMTicketListCommand(char* args)
{
    static const std::unordered_map<std::string, uint8> categories
    {
        { "stuck", 1 },{ "behavior", 2 },{ "harassment", 2 },{ "guild", 3 },
        { "item", 4 },{ "environment", 5 },{ "world", 5 },{ "npc", 6 },
        { "creature", 6 },{ "creep", 6 },{ "quest", 7 },{ "questnpc", 7 },
        { "technical", 8 },{ "account", 9 },{ "billing", 9 },{ "character", 10 }
    };

    auto it = categories.find(args);

    if (it == categories.end())
        sTicketMgr.ShowList(*this, false);
    else
        sTicketMgr.ShowList(*this, false, it->second);

    return true;
}


bool ChatHandler::HandleGMTicketsOnlineListCommand(char* args)
{
    sTicketMgr.ShowList(*this, true);
    return true;
}

bool ChatHandler::ViewTicketByIdOrName(char* ticketId_c, char* name_c)
{
    GmTicket* ticket = nullptr;

    // By ticket id
    if (ticketId_c && *ticketId_c)
    {
        uint32 ticketId = atoi(ticketId_c);
        ticket = sTicketMgr.GetTicket(ticketId);
    }

    // By player name
    if (!ticket && name_c && *name_c)
    {
        std::string name(name_c);
        if (!normalizePlayerName(name))
            return false;

        // Detect target's GUID
        ObjectGuid guid;
        if (Player* player = ObjectAccessor::FindPlayerByName(name.c_str()))
            guid = player->GetGUID();
        else
            guid = sObjectMgr.GetPlayerGuidByName(name);

        // Target must exist
        if (!guid)
        {
            SendSysMessage(LANG_NO_PLAYERS_FOUND);
            return true;
        }
        ticket = sTicketMgr.GetTicketByPlayer(guid);
    }

    if (!ticket)
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }

    if (ticket->IsClosed() || ticket->IsCompleted())
    {
        SendSysMessage(LANG_COMMAND_TICKETARCHIVED);
        return true;
    }
    return ViewTicket(ticket);
}

bool ChatHandler::ViewTicket(GmTicket* ticket)
{
    ticket->SetViewed();
    if (Player* player = ticket->GetPlayer()) // Inform that ticket has been viewed
        sTicketMgr.SendTicket(player->GetSession(), ticket);
    ticket->SaveToDB();

    SendSysMessage(ticket->FormatMessageString(*this, true).c_str());
    return true;
}

bool ChatHandler::HandleGMTicketGetByIdOrNameCommand(char* args)
{
    return ViewTicketByIdOrName(args, args);
}


bool ChatHandler::HandleGMTicketCloseByIdCommand(char* args)
{
    if (!*args)
        return false;

    uint32 ticketId = atoi(args);
    GmTicket* ticket = sTicketMgr.GetTicket(ticketId);
    if (!ticket)
    {
        SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
        return true;
    }
    if (ticket->IsClosed() || ticket->IsCompleted())
    {
        SendSysMessage(LANG_COMMAND_TICKETARCHIVED);
        return true;
    }

    // Ticket should be assigned to the player who tries to close it.
    // Console can override though
    Player* player = GetSession() ? GetSession()->GetPlayer() : nullptr;
    if (player && ticket->IsAssignedNotTo(player->GetGUID()))
    {
        PSendSysMessage(LANG_COMMAND_TICKETCANNOTCLOSE, ticket->GetId());
        return true;
    }

    sTicketMgr.CloseTicket(ticket->GetId(), player ? player->GetObjectGuid() : ObjectGuid(uint64(-1)));
    sTicketMgr.UpdateLastChange();

    std::string msg = ticket->FormatMessageString(*this, player ? player->GetName() : "Console", nullptr, nullptr, nullptr, nullptr);
    sWorld.SendGMTicketText(msg.c_str());

    // Inform player, who submitted this ticket, that it is closed
    if (Player* submitter = ticket->GetPlayer())
    {
        WorldPacket data(SMSG_GMTICKET_DELETETICKET, 4);
        data << uint32(GMTICKET_RESPONSE_TICKET_DELETED);
        submitter->GetSession()->SendPacket(&data);
    }
    return true;
}

bool RegisterPlayerToBG(WorldSession* sess, BattleGroundTypeId bgid)
{
    Player* pPlayer = sess->GetPlayer();
    if (!pPlayer->GetBGAccessByLevel(bgid))
        return false;

    pPlayer->SetBattleGroundEntryPoint(pPlayer->GetMapId(), pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetOrientation());
    sess->SendBattleGroundList(pPlayer->GetObjectGuid(), bgid);

    return true;
}

bool ChatHandler::HandleHelpCommand(char* args)
{
    if (!*args)
    {
        ShowHelpForCommand(getCommandTable(), "help");
        ShowHelpForCommand(getCommandTable(), "");
    }
    else
    {
        if (!ShowHelpForCommand(getCommandTable(), args))
            SendSysMessage(LANG_NO_CMD);
    }

    return true;
}

bool ChatHandler::HandleCommandsCommand(char* /*args*/)
{
    ShowHelpForCommand(getCommandTable(), "");
    return true;
}

bool ChatHandler::HandleAccountCommand(char* args)
{
    // let show subcommands at unexpected data in args
    if (*args)
        return false;

    AccountTypes gmlevel = GetAccessLevel();
    PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmlevel));
    return true;
}

bool ChatHandler::HandleStartCommand(char* /*args*/)
{
    Player* chr = m_session->GetPlayer();

    if (chr->IsTaxiFlying())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    if (chr->IsInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }

    // cast spell Stuck
    chr->CastSpell(chr, 7355, false);
    return true;
}

bool ChatHandler::HandleUnstuckCommand(char* /*args*/)
{
    Player* pPlayer = m_session->GetPlayer();

    if (!pPlayer)
        return false;

    if (pPlayer->IsInCombat() || pPlayer->InBattleGround() || pPlayer->IsTaxiFlying() || pPlayer->HasSpellCooldown(20939) || (pPlayer->GetDeathState() == CORPSE) || (pPlayer->GetLevel() < 10))
    {
        SendSysMessage(LANG_UNSTUCK_UNAVAILABLE);
        return false;
    }

    if (pPlayer->IsAlive())
    {
        pPlayer->CastSpell(pPlayer, 20939, false);
        SendSysMessage(LANG_UNSTUCK_ALIVE);
    }
    else
    {
        pPlayer->AddAura(SPELL_ID_PASSIVE_RESURRECTION_SICKNESS); // Add Resurrection Sickness
        pPlayer->AddSpellCooldown(20939, 0, time(nullptr) + 3600000); // Trigger 1 Hour Cooldown
        // Get nearest graveyard.
        WorldSafeLocsEntry const* ClosestGrave = sObjectMgr.GetClosestGraveYard(pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ(), pPlayer->GetMapId(), pPlayer->GetTeam());

        if (!ClosestGrave) //No nearby graveyards (stuck in void?). Send ally to Westfall, Horde to Barrens.
            ClosestGrave = pPlayer->GetTeamId() ? sWorldSafeLocsStore.LookupEntry(9) : sWorldSafeLocsStore.LookupEntry(4);

        if (ClosestGrave)
            pPlayer->TeleportTo(ClosestGrave->map_id, ClosestGrave->x, ClosestGrave->y, ClosestGrave->z, sObjectMgr.GetWorldSafeLocFacing(ClosestGrave->ID), 0);

        SendSysMessage(LANG_UNSTUCK_DEAD);
    }

    sLog.outInfo("Player %s (guid %u) used unstuck command at map %u (%f, %f, %f).", pPlayer->GetName(), pPlayer->GetGUIDLow(), pPlayer->GetMapId(), pPlayer->GetPositionX(), pPlayer->GetPositionY(), pPlayer->GetPositionZ());
    return true;
}

bool ChatHandler::HandleServerInfoCommand(char* /*args*/)
{
    if (!m_session || m_session->GetSecurity() != SEC_PLAYER)
        SendSysMessage("Core revision: " REVISION_HASH " / " REVISION_DATE " / " _ENDIAN_PLATFORM);

    uint32 activeClientsNum = sWorld.GetActiveSessionCount();
    uint32 queuedClientsNum = sWorld.GetQueuedSessionCount();
    uint32 maxActiveClientsNum = sWorld.GetMaxActiveSessionCount();
    uint32 maxQueuedClientsNum = sWorld.GetMaxQueuedSessionCount();
    std::string str = secsToTimeString(sWorld.GetUptime());
    uint32 queuedRegionOnePlayers = sWorld.GetRegionalIndexQueueCount(0);
    uint32 queuedRegionTwoPlayers = sWorld.GetRegionalIndexQueueCount(1);

    PSendSysMessage("Players online: %i. Max online: %i.", activeClientsNum, maxActiveClientsNum);
    PSendSysMessage(LANG_UPTIME, str.c_str());


    if (GetSession() && GetSession()->GetSecurity() >= SEC_MODERATOR)
    {
        PSendSysMessage("Last server diff: %u ms", sWorld.GetLastDiff());
        PSendSysMessage("Average server diff: %u ms", sWorld.GetAverageDiff());
        PSendSysMessage("Remaining HC Threshold hits: %u", sWorld.GetThresholdFlags());
        PSendSysMessage("Current dynamic respawn rate: %f", sWorld.m_dynamicRespawnRatio);

        if (!sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK))
            PSendSysMessage("Queued region one : %u, region two : %u", queuedRegionOnePlayers, queuedRegionTwoPlayers);


        uint32 numHcs = 0;
        uint32 numCn = 0;
        uint32 numNonCn = 0;
        uint32 numAlliance = 0;
        uint32 numHorde = 0;
        const World::SessionMap& sess = sWorld.GetAllSessions();
        for (const auto& sessPair : sess)
        {
            auto session = sessPair.second;
            auto player = session->GetPlayer();
            if (!player || !player->IsInWorld())
                continue;

            if (player->IsHardcore())
                ++numHcs;

            if (player->GetSession()->sessionDbcLocaleRaw == LOCALE_zhCN)
                ++numCn;
            else
                ++numNonCn;

            if (player->GetTeam() == HORDE)
                ++numHorde;
            else
                ++numAlliance;
        }
        PSendSysMessage("Total amount of Hardcore characters logged in: %u", numHcs);
        PSendSysMessage("Total amount of Alliance characters logged in: %u", numAlliance);
        PSendSysMessage("Total amount of Horde characters logged in: %u", numHorde);
        if (GetSession()->GetSecurity() >= SEC_ADMINISTRATOR)
        {
            if (!sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK))
            {
                PSendSysMessage("Total amount of CN characters logged in: %u", numCn);
                PSendSysMessage("Total amount of regional characters logged in: %u", numNonCn);
                PSendSysMessage("Total amount of CN conn logged in: %u", sWorld.loggedNonRegionSessions.load());
                PSendSysMessage("Total amount of regional conn logged in: %u", sWorld.loggedRegionSessions.load());
            }
        }
    }

    std::tm* ptm = std::localtime(&sWorld.GetGameTime());
    char timeStr[32];
    std::strftime(timeStr, 32, "%a, %d.%m.%Y %H:%M:%S", ptm);
    PSendSysMessage("Server Time: %s", timeStr);

    return true;
}

bool ChatHandler::HandleDismountCommand(char* /*args*/)
{
    Player* pPlayer = m_session->GetPlayer();

    if (pPlayer->GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID) == 2490 || pPlayer->GetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID) == 10318) // Mirage Raceway cars
    {
        pPlayer->GetSession()->SendNotification("You can't dismount while racing!");
        SetSentErrorMessage(true);
        return false;
    }

    if (!pPlayer->IsMounted())
    {
        SendSysMessage(LANG_CHAR_NON_MOUNTED);
        SetSentErrorMessage(true);
        return false;
    }

    if (pPlayer->IsTaxiFlying())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    pPlayer->Unmount();
    pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    return true;
}

bool ChatHandler::HandleSaveCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();

    // save GM account without delay and output message (testing, etc)
    if (GetAccessLevel() > SEC_PLAYER)
    {
        player->SaveToDB();
        SendSysMessage(LANG_PLAYER_SAVED);
        return true;
    }

    // save or plan save after 20 sec (logout delay) if current next save time more this value and _not_ output any messages to prevent cheat planning
    uint32 save_interval = sWorld.getConfig(CONFIG_UINT32_INTERVAL_SAVE);
    if (save_interval == 0 || (save_interval > 20 * IN_MILLISECONDS && player->GetSaveTimer() <= save_interval - 20 * IN_MILLISECONDS))
        player->SaveToDB();
    SendSysMessage(LANG_PLAYER_SAVED);
    return true;
}

bool ChatHandler::HandleAccountPasswordCommand(char* args)
{
    // allow use from RA, but not from console (not have associated account id)
    if (!GetAccountId())
    {
        SendSysMessage(LANG_RA_ONLY_COMMAND);
        SetSentErrorMessage(true);
        return false;
    }

    // allow or quoted string with possible spaces or literal without spaces
    char* old_pass = ExtractQuotedOrLiteralArg(&args);
    char* new_pass = ExtractQuotedOrLiteralArg(&args);
    char* new_pass_c = ExtractQuotedOrLiteralArg(&args);

    if (!old_pass || !new_pass || !new_pass_c)
        return false;

    std::string password_old = old_pass;
    std::string password_new = new_pass;
    std::string password_new_c = new_pass_c;

    if (password_new != password_new_c)
    {
        SendSysMessage(LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage(true);
        return false;
    }

    if (!sAccountMgr.CheckPassword(GetAccountId(), password_old, m_session->GetUsername()))
    {
        SendSysMessage(LANG_COMMAND_WRONGOLDPASSWORD);
        SetSentErrorMessage(true);
        return false;
    }

    AccountOpResult result = sAccountMgr.ChangePassword(GetAccountId(), password_new, m_session->GetUsername());

    switch (result)
    {
        case AOR_OK:
        {
            SendSysMessage(LANG_COMMAND_PASSWORD);
            if (m_session->GetAccountFlags() & ACCOUNT_FLAG_PASSWORD_CHANGE_REWARD)
            {
                if (ItemPrototype const* itemProto = sObjectMgr.GetItemPrototype(sWorld.getConfig(CONFIG_UINT32_PASSWORD_CHANGE_REWARD_ITEM)))
                {
                    std::vector<PlayerCacheData*> characters;
                    sObjectMgr.GetPlayerDataForAccount(GetAccountId(), characters);
                    for (auto const& pChar : characters)
                    {
                        ObjectGuid playerGuid = ObjectGuid(HIGHGUID_PLAYER, pChar->uiGuid);
                        Player* pPlayer = (pChar->uiGuid == GetPlayer()->GetGUIDLow()) ? GetPlayer() : nullptr;
                        if (Item* restoredItem = Item::CreateItem(itemProto->ItemId, 1, pPlayer))
                        {
                            // save new item before send
                            restoredItem->SaveToDB();

                            // subject
                            std::string subject = itemProto->Name1;

                            // text
                            std::string body = "Thank you for taking account security seriously and changing your password. Here is your reward.";

                            MailDraft(subject, body)
                                .AddItem(restoredItem)
                                .SendMailTo(MailReceiver(playerGuid), MailSender(MAIL_NORMAL, playerGuid.GetCounter(), MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED, 0, 30 * DAY);
                        }
                    }
                    m_session->SetAccountFlags(m_session->GetAccountFlags() & ~ACCOUNT_FLAG_PASSWORD_CHANGE_REWARD);
                    LoginDatabase.PExecute("UPDATE account SET flags = flags & ~0x%x WHERE id = %u", ACCOUNT_FLAG_PASSWORD_CHANGE_REWARD, GetAccountId());
                }
            }
            break;
        }
        case AOR_PASS_TOO_LONG:
            SendSysMessage(LANG_PASSWORD_TOO_LONG);
            SetSentErrorMessage(true);
            return false;
        case AOR_NAME_NOT_EXIST:                            // not possible case, don't want get account name for output
        default:
            SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            SetSentErrorMessage(true);
            return false;
    }
    SetSentErrorMessage(true);   
    return false;
}

bool ChatHandler::HandleNpcSayCommand(char* args)
{
    if (!*args)
        return false;

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->MonsterSay(args, LANG_UNIVERSAL);

    return true;
}

bool ChatHandler::HandleNpcYellCommand(char* args)
{
    if (!*args)
        return false;

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->MonsterYell(args, LANG_UNIVERSAL);

    return true;
}


bool ChatHandler::HandleUnitSpeedInfoCommand(char* args)
{
    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Speed info for %s", pTarget->GetObjectGuid().GetString().c_str());
    PSendSysMessage("Walk: %g", pTarget->GetSpeed(MOVE_WALK));
    PSendSysMessage("Run: %g", pTarget->GetSpeed(MOVE_RUN));
    PSendSysMessage("Run Back: %g", pTarget->GetSpeed(MOVE_RUN_BACK));
    PSendSysMessage("Swim: %g", pTarget->GetSpeed(MOVE_SWIM));
    PSendSysMessage("Swim Back: %g", pTarget->GetSpeed(MOVE_SWIM_BACK));
    PSendSysMessage("Turn: %g", pTarget->GetSpeed(MOVE_TURN_RATE));

    return true;
}

//show text emote by creature in chat
bool ChatHandler::HandleNpcTextEmoteCommand(char* args)
{
    if (!*args)
        return false;

    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->MonsterTextEmote(args, 0, false);

    return true;
}
//----------------------------------------------------------

// global announce
bool ChatHandler::HandleAnnounceCommand(char* args)
{
    if (!*args)
        return false;

    sWorld.SendWorldText(LANG_SYSTEMMESSAGE, args);
    return true;
}

//notification player at the screen
bool ChatHandler::HandleNotifyCommand(char* args)
{
    if (!*args)
        return false;

    std::string str = GetMangosString(LANG_GLOBAL_NOTIFY);
    str += args;

    WorldPacket data(SMSG_NOTIFICATION, (str.size() + 1));
    data << str;
    sWorld.SendGlobalMessage(&data);

    return true;
}

//Enable\Dissable GM Mode
bool ChatHandler::HandleGMCommand(char* args)
{
    if (!*args)
    {
        if (m_session->GetPlayer()->IsGameMaster())
            m_session->SendNotification(LANG_GM_ON);
        else
            m_session->SendNotification(LANG_GM_OFF);
        return true;
    }

    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    if (value)
    {
        m_session->GetPlayer()->SetGameMaster(true);
        m_session->SendNotification(LANG_GM_ON);
    }
    else
    {
        m_session->GetPlayer()->SetGameMaster(false);
        m_session->SendNotification(LANG_GM_OFF);
    }

    return true;
}

//Enable\Disable Invisible mode
bool ChatHandler::HandleGMVisibleCommand(char* args)
{
    if (!*args)
    {
        bool visible = GetSession()->GetPlayer()->IsGMVisible();
        uint32 visibilityLevel = visible ? 0 : GetSession()->GetPlayer()->GetGMInvisibilityLevel();
        PSendSysMessage(LANG_YOU_ARE, visible ? GetMangosString(LANG_VISIBLE) : GetMangosString(LANG_INVISIBLE), visibilityLevel);
        return true;
    }

    bool value;
    uint8 accessLevel = GetAccessLevel();
    uint32 visibilityLevel = accessLevel + 1;

    if (ExtractUInt32(&args, visibilityLevel))
        value = (visibilityLevel == 0); // Make visible if level = 0 only
    else if (ExtractOnOff(&args, value))
        visibilityLevel = accessLevel;

    if (visibilityLevel > accessLevel)
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    if (value)
    {
        m_session->GetPlayer()->SetGMVisible(true);
        m_session->SendNotification(LANG_INVISIBLE_VISIBLE);
    }
    else
    {
        m_session->GetPlayer()->SetGMInvisibilityLevel(visibilityLevel);
        m_session->SendNotification(LANG_INVISIBLE_INVISIBLE, visibilityLevel);
        m_session->GetPlayer()->SetGMVisible(false);
    }

    return true;
}

bool ChatHandler::HandleGMSocialsCommand(char* args)
{
    auto player = GetSession()->GetPlayer();
    if (!player || !player->IsInWorld())
        return false;

    bool onOff = false;
    if (!ExtractOnOff(&args, onOff))
    {
        PSendSysMessage("GM Socials is %s", player->HasGMDisabledSocials() ? "OFF" : "ON");
        return true;
    }
    
    player->SetGMSocials(onOff);

    PSendSysMessage("GM Socials is now %s", onOff ? "ON" : "OFF");

    return true;
}

bool ChatHandler::HandleLoSCommand(char* args)
{
    Unit* target = GetSelectedUnit();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetPlayer()->IsWithinLOSInMap(target))
        SendSysMessage("Target is in line of sight.");
    else
        SendSysMessage("Target is NOT in line of sight.");

    return true;
}

bool ChatHandler::HandleGPSCommand(char* args)
{
    WorldObject* obj = nullptr;
    if (*args)
    {
        if (ObjectGuid guid = ExtractGuidFromLink(&args))
            obj = (WorldObject*)m_session->GetPlayer()->GetObjectByTypeMask(guid, TYPEMASK_CREATURE_OR_GAMEOBJECT);

        if (!obj)
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }
    }
    else
    {
        obj = GetSelectedUnit();

        if (!obj)
        {
            SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
            SetSentErrorMessage(true);
            return false;
        }
    }
    CellPair cell_val = MaNGOS::ComputeCellPair(obj->GetPositionX(), obj->GetPositionY());
    Cell cell(cell_val);

    uint32 zone_id, area_id;
    obj->GetZoneAndAreaId(zone_id, area_id);

    MapEntry const* mapEntry = sMapStorage.LookupEntry<MapEntry>(obj->GetMapId());
    const auto* zoneEntry = AreaEntry::GetById(zone_id);
    const auto* areaEntry = AreaEntry::GetById(area_id);

    float zone_x = obj->GetPositionX();
    float zone_y = obj->GetPositionY();

    if (!Map2ZoneCoordinates(zone_x, zone_y, zone_id))
    {
        zone_x = 0;
        zone_y = 0;
    }

    Map const* map = obj->GetMap();
    float ground_z = map->GetHeight(obj->GetPositionX(), obj->GetPositionY(), MAX_HEIGHT);
    float floor_z = map->GetHeight(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ());

    GridPair p = MaNGOS::ComputeGridPair(obj->GetPositionX(), obj->GetPositionY());

    int gx = 63 - p.x_coord;
    int gy = 63 - p.y_coord;

    uint32 have_map = GridMap::ExistMap(obj->GetMapId(), gx, gy) ? 1 : 0;
    uint32 have_vmap = GridMap::ExistVMap(obj->GetMapId(), gx, gy) ? 1 : 0;

    TerrainInfo const* terrain = obj->GetTerrain();
    if (have_vmap)
    {
        if (terrain->IsOutdoors(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ()))
            PSendSysMessage("You are OUTdoor");
        else
            PSendSysMessage("You are INdoor");
    }
    else PSendSysMessage("no VMAP available for area info");

    std::string zoneName = "<unknown>";
    std::string areaName = "<unknown>";

    if (zoneEntry)
    {
        zoneName = zoneEntry->Name;
        sObjectMgr.GetAreaLocaleString(zoneEntry->Id, GetSessionDbLocaleIndex(), &zoneName);
    }

    if (areaEntry)
    {
        areaName = areaEntry->Name;
        sObjectMgr.GetAreaLocaleString(areaEntry->Id, GetSessionDbLocaleIndex(), &areaName);
    }

    PSendSysMessage(LANG_MAP_POSITION,
        obj->GetMapId(), (mapEntry ? mapEntry->name : "<unknown>"),
        zone_id, zoneName.c_str(), area_id, areaName.c_str(),
        obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(),
        cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), obj->GetInstanceId(),
        zone_x, zone_y, ground_z, floor_z, have_map, have_vmap);

    PSendSysMessage("TileX: %d, TileY: %d", gx, gy);

    DEBUG_LOG("Player %s GPS call for %s '%s' (%s: %u):",
        m_session ? GetNameLink().c_str() : GetMangosString(LANG_CONSOLE_COMMAND),
        (obj->GetTypeId() == TYPEID_PLAYER ? "player" : "creature"), obj->GetName(),
        (obj->GetTypeId() == TYPEID_PLAYER ? "GUID" : "Entry"), (obj->GetTypeId() == TYPEID_PLAYER ? obj->GetGUIDLow() : obj->GetEntry()));

    if (zoneEntry)
        sObjectMgr.GetAreaLocaleString(zoneEntry->Id, sWorld.GetDefaultDbcLocale(), &zoneName);

    if (areaEntry)
        sObjectMgr.GetAreaLocaleString(areaEntry->Id, sWorld.GetDefaultDbcLocale(), &areaName);

    DEBUG_LOG(GetMangosString(LANG_MAP_POSITION),
        obj->GetMapId(), (mapEntry ? mapEntry->name : "<unknown>"),
        zone_id, zoneName.c_str(), area_id, areaName.c_str(),
        obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(),
        cell.GridX(), cell.GridY(), cell.CellX(), cell.CellY(), obj->GetInstanceId(),
        zone_x, zone_y, ground_z, floor_z, have_map, have_vmap);
   
     sLog.outString("XYZ: %f %f %f", obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ());

    GridMapLiquidData liquid_status;
    GridMapLiquidStatus res = terrain->getLiquidStatus(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), MAP_ALL_LIQUIDS, &liquid_status);
    if (res)
        PSendSysMessage(LANG_LIQUID_STATUS, liquid_status.level, liquid_status.depth_level, liquid_status.type_flags, res);

    if (obj->GetTransport())
        PSendSysMessage("Transport: %s (%u)", obj->GetTransport()->GetName(), obj->GetTransport()->GetEntry());

    return true;
}

//Summon Player
bool ChatHandler::HandleSummonCommand(char* args)
{
    Player* pTarget;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &pTarget, &target_guid, &target_name))
        return false;

    Player* pPlayer = m_session->GetPlayer();
    if (pTarget == pPlayer || target_guid == pPlayer->GetObjectGuid())
    {
        PSendSysMessage(LANG_CANT_TELEPORT_SELF);
        SetSentErrorMessage(true);
        return false;
    }

    if (pTarget)
    {
        std::string nameLink = playerLink(target_name);
        // check online security
        if (HasLowerSecurity(pTarget))
            return false;

        if (pTarget->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, nameLink.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        Map* pMap = pPlayer->GetMap();

        if (pMap->IsBattleGround())
        {
            pTarget->SetBattleGroundId(pPlayer->GetBattleGroundId(), pPlayer->GetBattleGroundTypeId(), PLAYER_MAX_BATTLEGROUND_QUEUES);
            // remember current position as entry point for return at bg end teleportation
            if (!pTarget->GetMap()->IsBattleGround())
                pTarget->SetBattleGroundEntryPoint();
        }
        PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), "");
        if (needReportToTarget(pTarget, true))
            ChatHandler(pTarget).PSendSysMessage(LANG_SUMMONED_BY, playerLink(pPlayer->GetName()).c_str());

        // stop flight if need
        if (pTarget->IsTaxiFlying())
        {
            pTarget->GetMotionMaster()->MovementExpired();
            pTarget->GetTaxi().ClearTaxiDestinations();
        }
        // save only in non-flight case
        else
            pTarget->SaveRecallPosition();

        // before GM
        float x, y, z;
        pPlayer->GetClosePoint(x, y, z, pTarget->GetObjectBoundingRadius());
        pTarget->TeleportTo(pPlayer->GetMapId(), x, y, z, pTarget->GetOrientation(), TELE_TO_NOT_LEAVE_COMBAT);
    }
    else
    {
        // check offline security
        if (HasLowerSecurity(nullptr, target_guid))
            return false;

        std::string nameLink = playerLink(target_name);

        PSendSysMessage(LANG_SUMMONING, nameLink.c_str(), GetMangosString(LANG_OFFLINE));

        // in point where GM stay
        Player::SavePositionInDB(target_guid, pPlayer->GetMapId(),
            pPlayer->GetPositionX(),
            pPlayer->GetPositionY(),
            pPlayer->GetPositionZ(),
            pPlayer->GetOrientation(),
            pPlayer->GetZoneId());
    }

    return true;
}

//Teleport to Player
bool ChatHandler::HandleGonameCommand(char* args)
{
    Player* pTarget;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &pTarget, &target_guid, &target_name))
        return false;

    Player* pPlayer = m_session->GetPlayer();
    if (pTarget == pPlayer || target_guid == pPlayer->GetObjectGuid())
    {
        SendSysMessage(LANG_CANT_TELEPORT_SELF);
        SetSentErrorMessage(true);
        return false;
    }

    if (pTarget)
    {
        const std::string chrNameLink = playerLink(target_name);
        Map const* cMap = pTarget->GetMap();
        uint32 instanceId = 0;
        uint32 teleFlags = TELE_TO_GM_MODE;
        InstancePlayerBind const* bind = pPlayer->GetBoundInstance(pTarget->GetMapId());

        if (bind)
        {
            instanceId = bind->state->GetInstanceId();
            pPlayer->UnbindInstance(pTarget->GetMapId());
        }

        if (cMap->IsBattleGround())
        {
            if (pPlayer->GetBattleGroundId() && pPlayer->GetBattleGroundId() != pTarget->GetBattleGroundId())
            {
                PSendSysMessage(LANG_CANNOT_GO_TO_BG_FROM_BG, chrNameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
            // all's well, set bg id
            // when porting out from the bg, it will be reset to 0
            if (pPlayer->GetBattleGroundId() != pTarget->GetBattleGroundId())
            {
                pPlayer->SetBattleGroundId(pTarget->GetBattleGroundId(), pTarget->GetBattleGroundTypeId(), PLAYER_MAX_BATTLEGROUND_QUEUES);
                teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
            }

            // remember current position as entry point for return at bg end teleportation
            if (!pPlayer->GetMap()->IsBattleGround())
                pPlayer->SetBattleGroundEntryPoint();
        }
        else if (cMap->IsDungeon())
        {
            // we have to go to instance, and can go to player only if:
            //   1) we are in his group (either as leader or as member)
            //   2) we are not bound to any group and have GM mode on
            if (pPlayer->GetGroup())
            {
                // we are in group, we can go only if we are in the player group
                if (pPlayer->GetGroup() != pTarget->GetGroup())
                {
                    PSendSysMessage(LANG_CANNOT_GO_TO_INST_PARTY, chrNameLink.c_str());
                    SetSentErrorMessage(true);
                    return false;
                }
            }

            // if the player or the player's group is bound to another instance
            // the player will not be bound to another one
            InstancePlayerBind const* pBind = pPlayer->GetBoundInstance(pTarget->GetMapId());

            if (!pBind)
            {
                Group* group = pPlayer->GetGroup();
                // if no bind exists, create a solo bind
                InstanceGroupBind const* gBind = group ? group->GetBoundInstance(pTarget->GetMapId()) : nullptr;
                // if no bind exists, create a solo bind
                if (!gBind)
                {
                    DungeonPersistentState* save = ((DungeonMap*)pTarget->GetMap())->GetPersistanceState();

                    // if player is group leader then we need add group bind
                    if (group && group->IsLeader(pPlayer->GetObjectGuid()))
                        group->BindToInstance(save, !save->CanReset());
                    else
                        pPlayer->BindToInstance(save, !save->CanReset());

                    if (instanceId && instanceId != save->GetInstanceId())
                    {
                        teleFlags |= TELE_TO_FORCE_MAP_CHANGE;
                    }
                }
            }
        }

        PSendSysMessage(LANG_APPEARING_AT_ONLINE, chrNameLink.c_str());
        if (needReportToTarget(pTarget, true))
            ChatHandler(pTarget).PSendSysMessage(LANG_APPEARING_TO, GetNameLink().c_str());

        // stop flight if need
        if (pPlayer->IsTaxiFlying())
        {
            pPlayer->GetMotionMaster()->MovementExpired();
            pPlayer->GetTaxi().ClearTaxiDestinations();
        }
        // save only in non-flight case
        else
            pPlayer->SaveRecallPosition();

        // to point to see at target with same orientation
        float x, y, z;
        pTarget->GetPosition(x, y, z);

        pPlayer->TeleportTo(pTarget->GetMapId(), x, y, z + 2.f, pPlayer->GetAngle(pTarget), teleFlags);
    }
    else
    {
        const std::string nameLink = playerLink(target_name);

        PSendSysMessage(LANG_APPEARING_AT_OFFLINE, nameLink.c_str());

        // to point where player stay (if loaded)
        float x, y, z, o;
        uint32 map;
        bool in_flight;
        if (!Player::LoadPositionFromDB(target_guid, map, x, y, z, o, in_flight))
            return false;

        return HandleGoHelper(pPlayer, map, x, y, &z);
    }

    return true;
}

// Teleport player to last position
bool ChatHandler::HandleRecallCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    // check online security
    if (HasLowerSecurity(target))
        return false;

    if (target->IsBeingTeleported())
    {
        PSendSysMessage(LANG_IS_TELEPORTED, GetNameLink(target).c_str());
        SetSentErrorMessage(true);
        return false;
    }

    uint32 mapId;
    float x, y, z, o;
    target->GetRecallPosition(mapId, x, y, z, o);
    return HandleGoHelper(target, mapId, x, y, &z, &o);
}

bool ChatHandler::HandleReplenishCommand(char* args)
{
    Unit* pUnit = GetSelectedUnit();
    if (!pUnit || !pUnit->IsAlive())
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pUnit->SetHealth(pUnit->GetMaxHealth());

    if (pUnit->GetPowerType() == POWER_MANA)
        pUnit->SetPower(POWER_MANA, pUnit->GetMaxPower(POWER_MANA));

    return true;
}

//Edit Player Faction
bool ChatHandler::HandleModifyFactionCommand(char* args)
{
    Creature* chr = GetSelectedCreature();
    if (!chr)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (!*args)
    {
        if (chr)
        {
            uint32 factionid = chr->GetFactionTemplateId();
            uint32 flag = chr->GetUInt32Value(UNIT_FIELD_FLAGS);
            uint32 npcflag = chr->GetUInt32Value(UNIT_NPC_FLAGS);
            uint32 dyflag = chr->GetUInt32Value(UNIT_DYNAMIC_FLAGS);
            PSendSysMessage(LANG_CURRENT_FACTION, chr->GetGUIDLow(), factionid, flag, npcflag, dyflag);
        }
        return true;
    }

    if (!chr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 factionid;
    if (!ExtractUint32KeyFromLink(&args, "Hfaction", factionid))
        return false;

    if (!sObjectMgr.GetFactionTemplateEntry(factionid))
    {
        PSendSysMessage(LANG_WRONG_FACTION, factionid);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 flag;
    if (!ExtractOptUInt32(&args, flag, chr->GetUInt32Value(UNIT_FIELD_FLAGS)))
        return false;

    uint32 npcflag;
    if (!ExtractOptUInt32(&args, npcflag, chr->GetUInt32Value(UNIT_NPC_FLAGS)))
        return false;

    uint32  dyflag;
    if (!ExtractOptUInt32(&args, dyflag, chr->GetUInt32Value(UNIT_DYNAMIC_FLAGS)))
        return false;

    PSendSysMessage(LANG_YOU_CHANGE_FACTION, chr->GetGUIDLow(), factionid, flag, npcflag, dyflag);

    chr->SetFactionTemplateId(factionid);
    chr->SetUInt32Value(UNIT_FIELD_FLAGS, flag);
    chr->SetUInt32Value(UNIT_NPC_FLAGS, npcflag);
    chr->SetUInt32Value(UNIT_DYNAMIC_FLAGS, dyflag);

    return true;
}

//Enable On\OFF all taxi paths
bool ChatHandler::HandleTaxiCheatCommand(char* args)
{
    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    Player* chr = GetSelectedPlayer();
    if (!chr)
        chr = m_session->GetPlayer();
    // check online security
    else if (HasLowerSecurity(chr))
        return false;

    if (value)
    {
        chr->SetTaxiCheater(true);
        PSendSysMessage(LANG_YOU_GIVE_TAXIS, GetNameLink(chr).c_str());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_TAXIS_ADDED, GetNameLink().c_str());
    }
    else
    {
        chr->SetTaxiCheater(false);
        PSendSysMessage(LANG_YOU_REMOVE_TAXIS, GetNameLink(chr).c_str());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_TAXIS_REMOVED, GetNameLink().c_str());
    }

    return true;
}

//Edit Player Aspeed
bool ChatHandler::HandleModifyASpeedCommand(char* args)
{
    if (!*args)
        return false;

    float modSpeed = (float)atof(args);

    if (modSpeed > 100 || modSpeed < 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Unit* chr = GetSelectedUnit();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    /*  if (HasLowerSecurity(chr))
            return false;

        std::string chrNameLink = GetNameLink(chr);*/
    std::string chrNameLink = (chr->GetTypeId() == TYPEID_PLAYER) ? ((Player*)chr)->GetName() : "<creature>";

    if (chr->IsTaxiFlying())
    {
        PSendSysMessage(LANG_CHAR_IN_FLIGHT, chrNameLink.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_YOU_CHANGE_ASPEED, modSpeed, chrNameLink.c_str());

    chr->UpdateSpeed(MOVE_WALK, false, modSpeed);
    chr->UpdateSpeed(MOVE_RUN, false, modSpeed);
    chr->UpdateSpeed(MOVE_SWIM, false, modSpeed);
    return true;
}

//Edit Player Scale
bool ChatHandler::HandleModifyScaleCommand(char* args)
{
    if (!*args)
        return false;


    float Scale;
    if (!ExtractFloat(&args, Scale))
        return false;

    //float Scale = (float)atof(args);

    bool permanent = false;
    bool permanentGiven = ExtractOnOff(&args, permanent);

    if (Scale > 100.0f || Scale <= 0.0f)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Unit* target = GetSelectedUnit();
    if (target == nullptr)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        // check online security
        if (HasLowerSecurity((Player*)target))
            return false;

        PSendSysMessage(LANG_YOU_CHANGE_SIZE, Scale, GetNameLink((Player*)target).c_str());
        if (needReportToTarget((Player*)target))
            ChatHandler((Player*)target).PSendSysMessage(LANG_YOURS_SIZE_CHANGED, GetNameLink().c_str(), Scale);
    }

    if (permanentGiven)
    {
        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        if (permanent)
            sGuidObjectScaling.AddOrEdit(target->GetGUID(), Scale);
        else
            sGuidObjectScaling.Remove(target->GetGUID());
    }

    target->SetObjectScale(Scale);
    target->UpdateModelData();

    return true;
}

bool ChatHandler::HandleModifyHPCommand(char* args)
{
    if (!*args)
        return false;

    int32 hp = 0;
    int32 hpm = 0;
    ExtractInt32(&args, hp);
    ExtractInt32(&args, hpm);

    if (hpm < hp)
        hpm = hp;

    if (hp <= 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Unit* chr = GetSelectedUnit();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (chr->GetTypeId() == TYPEID_PLAYER && HasLowerSecurity(chr->ToPlayer()))
        return false;

    PSendSysMessage(LANG_YOU_CHANGE_HP, chr->ToPlayer() ? GetNameLink(chr->ToPlayer()).c_str() : "<creature>", hp, hpm);
    if (chr->GetTypeId() == TYPEID_PLAYER && needReportToTarget(chr->ToPlayer()))
        ChatHandler(chr->ToPlayer()).PSendSysMessage(LANG_YOURS_HP_CHANGED, GetNameLink().c_str(), hp, hpm);

    chr->SetMaxHealth(hpm);
    chr->SetHealth(hp);

    return true;
}

bool ChatHandler::HandleModifyManaCommand(char* args)
{
    if (!*args)
        return false;

    int32 mana = 0;
    int32 manam = 0;
    ExtractInt32(&args, mana);
    ExtractInt32(&args, manam);

    if (manam < mana)
        manam = mana;

    if (mana <= 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Unit* chr = GetSelectedUnit();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (chr->GetTypeId() == TYPEID_PLAYER && HasLowerSecurity(chr->ToPlayer()))
        return false;

    PSendSysMessage(LANG_YOU_CHANGE_MANA, chr->ToPlayer() ? GetNameLink(chr->ToPlayer()).c_str() : "<creature>", mana, manam);
    if (chr->GetTypeId() == TYPEID_PLAYER && needReportToTarget(chr->ToPlayer()))
        ChatHandler(chr->ToPlayer()).PSendSysMessage(LANG_YOURS_MANA_CHANGED, GetNameLink().c_str(), mana, manam);

    auto powerType = chr->GetPowerType();
    chr->SetMaxPower(powerType, manam);
    chr->SetPower(powerType, mana);

    return true;
}

bool ChatHandler::HandleModifyEnergyCommand(char* args)
{
    Player* chr = GetSelectedPlayer();
    if (!chr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 energyMin;
    if (!ExtractUInt32(&args, energyMin))
        return false;

    uint32 energyMax;
    if (!ExtractUInt32(&args, energyMax))
        energyMax = std::max(chr->GetMaxPower(POWER_ENERGY), energyMin);

    if (energyMin < 0 || energyMax < 0 || energyMax < energyMin)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(chr))
        return false;

    PSendSysMessage(LANG_YOU_CHANGE_ENERGY, GetNameLink(chr).c_str(), energyMin, energyMax);
    if (needReportToTarget(chr))
        ChatHandler(chr).PSendSysMessage(LANG_YOURS_ENERGY_CHANGED, GetNameLink().c_str(), energyMin, energyMax);

    chr->SetMaxPower(POWER_ENERGY, energyMax);
    chr->SetPower(POWER_ENERGY, energyMin);

    DETAIL_LOG(GetMangosString(LANG_CURRENT_ENERGY), chr->GetMaxPower(POWER_ENERGY));

    return true;
}

bool ChatHandler::HandleModifyRageCommand(char* args)
{
    Player* chr = GetSelectedPlayer();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 rageMin;
    if (!ExtractUInt32(&args, rageMin))
        return false;

    uint32 rageMax;
    if (!ExtractUInt32(&args, rageMax))
        rageMax = std::max(chr->GetMaxPower(POWER_RAGE) / 10, rageMin);

    if (rageMin < 0 || rageMax < 0 || rageMax < rageMin)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    rageMin *= 10;
    rageMax *= 10;

    // check online security
    if (HasLowerSecurity(chr))
        return false;

    PSendSysMessage(LANG_YOU_CHANGE_RAGE, GetNameLink(chr).c_str(), rageMin / 10, rageMax / 10);
    if (needReportToTarget(chr))
        ChatHandler(chr).PSendSysMessage(LANG_YOURS_RAGE_CHANGED, GetNameLink().c_str(), rageMin / 10, rageMax / 10);

    chr->SetMaxPower(POWER_RAGE, rageMax);
    chr->SetPower(POWER_RAGE, rageMin);

    return true;
}

bool ChatHandler::HandleModifyGenderCommand(char* args)
{
    if (!*args)
        return false;

    Player* player = GetSelectedPlayer();

    if (!player)
    {
        PSendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    PlayerInfo const* info = sObjectMgr.GetPlayerInfo(player->GetRace(), player->GetClass());
    if (!info)
        return false;

    char* gender_str = args;
    int gender_len = strlen(gender_str);

    Gender gender;

    if (!strncmp(gender_str, "male", gender_len))            // MALE
    {
        if (player->GetGender() == GENDER_MALE)
            return true;

        gender = GENDER_MALE;
    }
    else if (!strncmp(gender_str, "female", gender_len))    // FEMALE
    {
        if (player->GetGender() == GENDER_FEMALE)
            return true;

        gender = GENDER_FEMALE;
    }
    else
    {
        SendSysMessage(LANG_MUST_MALE_OR_FEMALE);
        SetSentErrorMessage(true);
        return false;
    }

    // Set gender
    player->SetByteValue(UNIT_FIELD_BYTES_0, 2, gender);
    player->SetUInt16Value(PLAYER_BYTES_3, 0, uint16(gender) | (player->GetDrunkValue() & 0xFFFE));

    // Change display ID
    player->InitPlayerDisplayIds();

    char const* gender_full = gender ? "female" : "male";

    PSendSysMessage(LANG_YOU_CHANGE_GENDER, player->GetName(), gender_full);

    if (needReportToTarget(player))
        ChatHandler(player).PSendSysMessage(LANG_YOUR_GENDER_CHANGED, gender_full, GetNameLink().c_str());

    return true;
}

bool ChatHandler::HandleModifyStrengthCommand(char* args)
{
    if (!*args)
        return false;

    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        PSendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount;
    if (!ExtractInt32(&args, amount))
        return false;

    if (amount < 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    pTarget->SetModifierValue(UNIT_MOD_STAT_STRENGTH, BASE_VALUE, (float)amount);
    pTarget->UpdateAllStats();

    PSendSysMessage(LANG_YOU_CHANGE_STR, pTarget->GetName(), amount);

    if (needReportToTarget(pTarget->ToPlayer()))
        ChatHandler(pTarget->ToPlayer()).PSendSysMessage(LANG_YOURS_STR_CHANGED, GetNameLink().c_str(), amount);

    return true;
}

bool ChatHandler::HandleModifyAgilityCommand(char* args)
{
    if (!*args)
        return false;

    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        PSendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount;
    if (!ExtractInt32(&args, amount))
        return false;

    if (amount < 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    pTarget->SetModifierValue(UNIT_MOD_STAT_AGILITY, BASE_VALUE, (float)amount);
    pTarget->UpdateAllStats();

    PSendSysMessage(LANG_YOU_CHANGE_AGI, pTarget->GetName(), amount);

    if (needReportToTarget(pTarget->ToPlayer()))
        ChatHandler(pTarget->ToPlayer()).PSendSysMessage(LANG_YOURS_AGI_CHANGED, GetNameLink().c_str(), amount);

    return true;
}

bool ChatHandler::HandleModifyStaminaCommand(char* args)
{
    if (!*args)
        return false;

    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        PSendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount;
    if (!ExtractInt32(&args, amount))
        return false;

    if (amount < 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    pTarget->SetModifierValue(UNIT_MOD_STAT_STAMINA, BASE_VALUE, (float)amount);
    pTarget->UpdateAllStats();

    if (pTarget->IsAlive())
        pTarget->SetHealth(pTarget->GetMaxHealth());

    PSendSysMessage(LANG_YOU_CHANGE_STA, pTarget->GetName(), amount);

    if (needReportToTarget(pTarget->ToPlayer()))
        ChatHandler(pTarget->ToPlayer()).PSendSysMessage(LANG_YOURS_STA_CHANGED, GetNameLink().c_str(), amount);

    return true;
}

bool ChatHandler::HandleModifyIntellectCommand(char* args)
{
    if (!*args)
        return false;

    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        PSendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount;
    if (!ExtractInt32(&args, amount))
        return false;

    if (amount < 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    pTarget->SetModifierValue(UNIT_MOD_STAT_INTELLECT, BASE_VALUE, (float)amount);
    pTarget->UpdateAllStats();

    PSendSysMessage(LANG_YOU_CHANGE_INT, pTarget->GetName(), amount);

    if (needReportToTarget(pTarget->ToPlayer()))
        ChatHandler(pTarget->ToPlayer()).PSendSysMessage(LANG_YOURS_INT_CHANGED, GetNameLink().c_str(), amount);

    return true;
}

bool ChatHandler::HandleModifySpiritCommand(char* args)
{
    if (!*args)
        return false;

    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        PSendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount;
    if (!ExtractInt32(&args, amount))
        return false;

    if (amount < 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    pTarget->SetModifierValue(UNIT_MOD_STAT_SPIRIT, BASE_VALUE, (float)amount);
    pTarget->UpdateAllStats();

    PSendSysMessage(LANG_YOU_CHANGE_SPI, pTarget->GetName(), amount);

    if (needReportToTarget(pTarget->ToPlayer()))
        ChatHandler(pTarget->ToPlayer()).PSendSysMessage(LANG_YOURS_SPI_CHANGED, GetNameLink().c_str(), amount);

    return true;
}

bool ChatHandler::HandleModifyMeleeApCommand(char* args)
{
    if (!*args)
        return false;

    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        PSendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount;
    if (!ExtractInt32(&args, amount))
        return false;

    if (amount <= 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    pTarget->SetInt32Value(UNIT_FIELD_ATTACK_POWER, amount);
    pTarget->UpdateDamagePhysical(BASE_ATTACK);
    pTarget->UpdateDamagePhysical(OFF_ATTACK);

    PSendSysMessage(LANG_YOU_CHANGE_MELEEAP, pTarget->GetName(), amount);

    if (needReportToTarget(pTarget->ToPlayer()))
        ChatHandler(pTarget->ToPlayer()).PSendSysMessage(LANG_YOURS_MELEEAP_CHANGED, GetNameLink().c_str(), amount);

    return true;
}

bool ChatHandler::HandleModifySpellPowerCommand(char* args)
{
    if (!*args)
        return false;

    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        PSendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    int32 amount;
    if (!ExtractInt32(&args, amount))
        return false;

    if (amount < 0)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    // dunno where spell power is stored so using a custom spell
    pTarget->RemoveAurasDueToSpell(18058);
    pTarget->CastCustomSpell(pTarget, 18058, &amount, &amount, nullptr, true);

    PSendSysMessage(LANG_YOU_CHANGE_SP, pTarget->GetName(), amount);

    if (needReportToTarget(pTarget->ToPlayer()))
        ChatHandler(pTarget->ToPlayer()).PSendSysMessage(LANG_YOURS_SP_CHANGED, GetNameLink().c_str(), amount);

    return true;
}

bool ChatHandler::HandleModifyParryCommand(char* args)
{
    if (!*args)
        return false;

    Player* player = GetSelectedPlayer();

    if (!player)
    {
        PSendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    float amount;
    if (!ExtractFloat(&args, amount))
        return false;

    if (amount < 0 || amount > 100)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    player->SetStatFloatValue(PLAYER_PARRY_PERCENTAGE, amount);

    PSendSysMessage(LANG_YOU_CHANGE_PARRY, player->GetName(), amount);

    if (needReportToTarget(player))
        ChatHandler(player).PSendSysMessage(LANG_YOURS_PARRY_CHANGED, GetNameLink().c_str(), amount);

    return true;
}

bool ChatHandler::HandleTeleCommand(char* args)
{
    if (!*args)
        return false;

    Player* pPlayer = m_session->GetPlayer();

    // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
    GameTele const* tele = ExtractGameTeleFromLink(&args);

    if (!tele)
    {
        SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    return HandleGoHelper(pPlayer, tele->mapId, tele->x, tele->y, &tele->z, &tele->o);
}

//Enable\Disable accept whispers (for GM)
bool ChatHandler::HandleWhispersCommand(char* args)
{
    if (!*args)
    {
        PSendSysMessage(LANG_COMMAND_WHISPERACCEPTING, GetOnOffStr(m_session->GetMasterPlayer()->IsAcceptWhispers()));
        return true;
    }

    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    // whisper on
    if (value)
    {
        GetSession()->GetPlayer()->SetAcceptWhispers(true);
        SendSysMessage(LANG_COMMAND_WHISPERON);
    }
    // whisper off
    else
    {
        GetSession()->GetPlayer()->SetAcceptWhispers(false);
        GetSession()->GetMasterPlayer()->ClearAllowedWhisperers();
        SendSysMessage(LANG_COMMAND_WHISPEROFF);
    }

    return true;
}

//Save all players in the world
bool ChatHandler::HandleSaveAllCommand(char* /*args*/)
{
    sObjectAccessor.SaveAllPlayers();
    SendSysMessage(LANG_PLAYERS_SAVED);
    return true;
}

#ifndef USE_ANTICHEAT

bool ChatHandler::HandleAnticheatInfoCommand(char* args)
{
    SendSysMessage("Anticheat is not built with server!");
    return true;
}

#endif

//Send mail by command
bool ChatHandler::HandleSendMailCommand(char* args)
{
    // format: name "subject text" "mail text"
    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &target_guid, &target_name))
        return false;

    MailDraft draft;

    // fill draft
    if (!HandleSendMailHelper(draft, args))
        return false;

    // from console show nonexistent sender
    MailSender sender(MAIL_NORMAL, m_session ? m_session->GetPlayer()->GetObjectGuid().GetCounter() : 0, MAIL_STATIONERY_GM);

    draft.SendMailTo(MailReceiver(target, target_guid), sender);

    std::string nameLink = playerLink(target_name);
    PSendSysMessage(LANG_MAIL_SENT, nameLink.c_str());
    return true;
}

// teleport player to given game_tele.entry
bool ChatHandler::HandleTeleNameCommand(char* args)
{
    char* nameStr = ExtractOptNotLastArg(&args);

    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&nameStr, &target, &target_guid, &target_name))
        return false;

    // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
    GameTele const* tele = ExtractGameTeleFromLink(&args);
    if (!tele)
    {
        SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (target)
    {
        // check online security
        if (HasLowerSecurity(target))
            return false;

        std::string chrNameLink = playerLink(target_name);

        if (target->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, chrNameLink.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        PSendSysMessage(LANG_TELEPORTING_TO, chrNameLink.c_str(), "", tele->name.c_str());
        if (needReportToTarget(target, true))
            ChatHandler(target).PSendSysMessage(LANG_TELEPORTED_TO_BY, GetNameLink().c_str());

        return HandleGoHelper(target, tele->mapId, tele->x, tele->y, &tele->z, &tele->o);
    }
    else
    {
        // check offline security
        if (HasLowerSecurity(nullptr, target_guid))
            return false;

        std::string nameLink = playerLink(target_name);

        PSendSysMessage(LANG_TELEPORTING_TO, nameLink.c_str(), GetMangosString(LANG_OFFLINE), tele->name.c_str());
        Player::SavePositionInDB(target_guid, tele->mapId, tele->x, tele->y, tele->z, tele->o, sTerrainMgr.GetZoneId(tele->mapId, tele->x, tele->y, tele->z));
    }

    return true;
}

//Teleport group to given game_tele.entry
bool ChatHandler::HandleTeleGroupCommand(char* args)
{
    if (!*args)
        return false;

    Player* player = GetSelectedPlayer();
    if (!player)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(player))
        return false;

    // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
    GameTele const* tele = ExtractGameTeleFromLink(&args);
    if (!tele)
    {
        SendSysMessage(LANG_COMMAND_TELE_NOTFOUND);
        SetSentErrorMessage(true);
        return false;
    }

    std::string nameLink = GetNameLink(player);

    Group* grp = player->GetGroup();
    if (!grp)
    {
        PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* pl = itr->getSource();

        if (!pl || !pl->GetSession())
            continue;

        // check online security
        if (HasLowerSecurity(pl))
            return false;

        std::string plNameLink = GetNameLink(pl);

        if (pl->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, plNameLink.c_str());
            continue;
        }

        PSendSysMessage(LANG_TELEPORTING_TO, plNameLink.c_str(), "", tele->name.c_str());
        if (needReportToTarget(pl, true))
            ChatHandler(pl).PSendSysMessage(LANG_TELEPORTED_TO_BY, nameLink.c_str());

        // stop flight if need
        if (pl->IsTaxiFlying())
        {
            pl->GetMotionMaster()->MovementExpired();
            pl->GetTaxi().ClearTaxiDestinations();
        }
        // save only in non-flight case
        else
            pl->SaveRecallPosition();

        pl->TeleportTo(*tele);
    }

    return true;
}

//Summon group of player
bool ChatHandler::HandleGroupgoCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    // check online security
    if (HasLowerSecurity(target))
        return false;

    Group* grp = target->GetGroup();

    std::string nameLink = GetNameLink(target);

    if (!grp)
    {
        PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    Player* pPlayer = m_session->GetPlayer();
    Map* gmMap = pPlayer->GetMap();
    bool to_instance = gmMap->Instanceable();

    // we are in instance, and can summon only player in our group with us as lead
    if (to_instance && (
        !pPlayer->GetGroup() || (grp->GetLeaderGuid() != pPlayer->GetObjectGuid()) ||
        (pPlayer->GetGroup()->GetLeaderGuid() != pPlayer->GetObjectGuid())))
        // the last check is a bit excessive, but let it be, just in case
    {
        SendSysMessage(LANG_CANNOT_SUMMON_TO_INST);
        SetSentErrorMessage(true);
        return false;
    }

    for (GroupReference* itr = grp->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        Player* pl = itr->getSource();

        if (!pl || pl == pPlayer || !pl->GetSession())
            continue;

        // check online security
        if (HasLowerSecurity(pl))
            return false;

        std::string plNameLink = GetNameLink(pl);

        if (pl->IsBeingTeleported())
        {
            PSendSysMessage(LANG_IS_TELEPORTED, plNameLink.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        if (to_instance)
        {
            Map* plMap = pl->GetMap();

            if (plMap->Instanceable() && plMap->GetInstanceId() != gmMap->GetInstanceId())
            {
                // cannot summon from instance to instance
                PSendSysMessage(LANG_CANNOT_SUMMON_TO_INST, plNameLink.c_str());
                SetSentErrorMessage(true);
                return false;
            }
        }

        PSendSysMessage(LANG_SUMMONING, plNameLink.c_str(), "");
        if (needReportToTarget(pl, true))
            ChatHandler(pl).PSendSysMessage(LANG_SUMMONED_BY, nameLink.c_str());

        // stop flight if need
        if (pl->IsTaxiFlying())
        {
            pl->GetMotionMaster()->MovementExpired();
            pl->GetTaxi().ClearTaxiDestinations();
        }
        // save only in non-flight case
        else
            pl->SaveRecallPosition();

        // before GM
        float x, y, z;
        pPlayer->GetClosePoint(x, y, z, pl->GetObjectBoundingRadius());
        pl->TeleportTo(pPlayer->GetMapId(), x, y, z, pl->GetOrientation());
    }

    return true;
}

bool ChatHandler::HandleGoHelper(Player* player, uint32 mapid, float x, float y, float const* zPtr, float const* ortPtr)
{
    float z;
    float ort = player->GetOrientation();

    if (zPtr)
    {
        z = *zPtr;

        if (ortPtr)
            ort = *ortPtr;

        // check full provided coordinates
        if (!MapManager::IsValidMapCoord(mapid, x, y, z, ort))
        {
            PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
            SetSentErrorMessage(true);
            return false;
        }
    }
    else
    {
        // we need check x,y before ask Z or can crash at invalide coordinates
        if (!MapManager::IsValidMapCoord(mapid, x, y))
        {
            PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapid);
            SetSentErrorMessage(true);
            return false;
        }

        TerrainInfo const* map = sTerrainMgr.LoadTerrain(mapid);
        z = map->GetWaterOrGroundLevel(x, y, MAX_HEIGHT);
    }

    // stop flight if need
    if (player->IsTaxiFlying())
    {
        player->GetMotionMaster()->MovementExpired();
        player->GetTaxi().ClearTaxiDestinations();
    }
    // save only in non-flight case
    else
        player->SaveRecallPosition();

    player->TeleportTo(mapid, x, y, z, ort);

    return true;
}

bool ChatHandler::HandleGoCommand(char* args)
{
    if (!*args)
        return false;

    Player* pPlayer = m_session->GetPlayer();

    uint32 mapid;
    float x, y, z;

    // raw coordinates case
    if (ExtractFloat(&args, x))
    {
        if (!ExtractFloat(&args, y))
            return false;

        if (!ExtractFloat(&args, z))
            return false;

        if (!ExtractOptUInt32(&args, mapid, pPlayer->GetMapId()))
            return false;
    }
    // link case
    else if (!ExtractLocationFromLink(&args, mapid, x, y, z))
        return false;

    return HandleGoHelper(pPlayer, mapid, x, y, &z);
}

/**
 * \brief Teleports the player to their corpse.
 * \return true if the command was successful
 */
bool ChatHandler::HandleGoCorpseCommand(char*)
{
    const auto player = m_session->GetPlayer();
    if (player->IsAlive())
    {
        return false;
    }

    const auto corpse = player->GetCorpse();
    if (!corpse)
    {
        return false;
    }

    WorldLocation corpseLocation;
    corpse->GetPosition(corpseLocation);
    player->TeleportTo(corpseLocation);
    return true;
}

// teleports to a grave yard.
bool ChatHandler::HandleGoGraveyardCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();

    uint32 graveyardId;
    if (!ExtractUInt32(&args, graveyardId))
        return false;

    WorldSafeLocsEntry const* graveyard = sWorldSafeLocsStore.LookupEntry(graveyardId);
    if (!graveyard)
    {
        PSendSysMessage("Graveyard (WorldSafeLocs ID %u) not found.", graveyardId);
        SetSentErrorMessage(true);
        return false;
    }

    if (graveyard->x == 0.0f && graveyard->y == 0.0f && graveyard->z == 0.0f)
    {
        PSendSysMessage(LANG_INVALID_TARGET_COORD, graveyard->x, graveyard->y, graveyard->map_id);
        SetSentErrorMessage(true);
        return false;
    }

    return HandleGoHelper(pPlayer, graveyard->map_id, graveyard->x, graveyard->y, &graveyard->z);
}

//teleport at coordinates, including Z
bool ChatHandler::HandleGoXYZCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();

    float x;
    if (!ExtractFloat(&args, x))
        return false;

    float y;
    if (!ExtractFloat(&args, y))
        return false;

    float z;
    if (!ExtractFloat(&args, z))
        return false;

    uint32 mapid;
    if (!ExtractOptUInt32(&args, mapid, pPlayer->GetMapId()))
        return false;

    return HandleGoHelper(pPlayer, mapid, x, y, &z);
}

bool ChatHandler::HandleModifyDrunkCommand(char* args)
{
    if (!*args)    return false;

    uint32 drunklevel = (uint32)atoi(args);
    if (drunklevel > 100)
        drunklevel = 100;

    uint16 drunkMod = drunklevel * 0xFFFF / 100;

    m_session->GetPlayer()->SetDrunkValue(drunkMod);

    return true;
}

bool ChatHandler::HandleSetGMChatCommand(char* args)
{
    if (!m_session || !m_session->GetPlayer())
        return false;

    auto player = m_session->GetPlayer();

    player->SetGMChat(!player->IsGMChat());
    PSendSysMessage("GM Chat is now %s", player->IsGMChat() ? "on" : "off");
    return true;
}

bool ChatHandler::HandleSetHCChatCommand(char* args)
{
    if (!m_session || !m_session->GetPlayer())
        return false;

    auto player = m_session->GetPlayer();

    player->SetHCChat(!player->IsHCChat());
    PSendSysMessage("HC Chat is now %s", player->IsHCChat() ? "on" : "off");
    return true;
}

bool ChatHandler::HandleToggleIllusionsCommand(char* args)
{
    m_session->GetPlayer()->hasIllusionsDisabled = !m_session->GetPlayer()->hasIllusionsDisabled;
    PSendSysMessage("illusions are now %s", m_session->GetPlayer()->hasIllusionsDisabled ? "disabled" : "enabled");
    return true;
}

bool ChatHandler::HandleSetViewCommand(char* /*args*/)
{
    if (Unit* unit = GetSelectedUnit())
        m_session->GetPlayer()->GetCamera().SetView(unit);
    else
    {
        PSendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleSetTimeCommand(char* args)
{
    auto player = GetSession()->GetPlayer();
    if (!player || !player->IsInWorld())
        return false;

    uint32 hour;
    if (!ExtractUInt32(&args, hour))
        return false;

    WorldPacket data;
    data.Initialize(SMSG_GAMETIME_SET, 4 );
    
    tm* lt = localtime(&sWorld.GetGameTime());
    lt->tm_hour = hour;
    data << uint32(secsToTimeBitFields(mktime(lt)));
    GetSession()->SendPacket(&data);
    return true;
}

bool ChatHandler::HandleGoldRemoval(char* args)
{
    std::string error("Illformed gold removal command. Format is: name #g #s #c"); // move?

    std::string input(args);
    // I'm bad at regex - feel free to improve this
    std::regex pattern(R"(([a-zA-Z]{3,}) (\d{1,5})(g|s|c)\s?(\d{1,2})(g|s|c)\s?(\d{1,2})(g|s|c)\s?)");
    std::smatch matches;

    if (!std::regex_match(input, matches, pattern))
    {
        PSendSysMessage(error.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    unsigned long gold = 0;
    unsigned long silver = 0;
    unsigned long copper = 0;

    std::string name = matches[1];

    for (auto i = matches.begin() + 2; i != matches.end(); i += 2)
    {
        try
        {
            auto type = (i + 1)->str();

            if (type == "g" && !gold)
            {
                gold += std::stoul(*i);
            }
            else if (type == "s" && !silver)
            {
                silver += std::stoul(*i);
            }
            else if (type == "c" && !copper)
            {
                copper += std::stoul(*i);
            }
            else
            {
                PSendSysMessage(error.c_str());
                SetSentErrorMessage(true);
                return false;
            }
        }
        catch (std::runtime_error&)
        {
            PSendSysMessage(error.c_str());
            SetSentErrorMessage(true);
            return false;
        }
    }

    uint32_t prevMoney = 0;
    uint32_t newMoney = 0;

    Player* player = sObjectMgr.GetPlayer(name.c_str());

    uint32 removalAmount = (gold * GOLD) + (silver * SILVER) + copper;

    if (player)
    {
        prevMoney = player->GetMoney();
        player->ModifyMoney(-static_cast<int32>(removalAmount));
        newMoney = player->GetMoney();

        PSendSysMessage("Removed %ug %us %uc from %s", gold, silver, copper, name.c_str());
        PSendSysMessage("%s previously had %ug %us %uc", name.c_str(), prevMoney / GOLD, (prevMoney % GOLD) / SILVER, (prevMoney % GOLD) % SILVER);
        PSendSysMessage("%s now has %ug %us %uc", name.c_str(), newMoney / GOLD, (newMoney % GOLD) / SILVER, (newMoney % GOLD) % SILVER);
    }
    else
    {
        CharacterDatabase.escape_string(name);
        CharacterDatabase.AsyncPQueryUnsafe(&PlayerGoldRemovalHandler::HandleGoldLookupResult,
            GetAccountId(), removalAmount,
            "SELECT money, guid, name FROM characters WHERE name = '%s'",
            name.c_str());
    }
    return true;
}


static uint32 ReputationRankStrIndex[MAX_REPUTATION_RANK] =
{
    LANG_REP_HATED,    LANG_REP_HOSTILE, LANG_REP_UNFRIENDLY, LANG_REP_NEUTRAL,
    LANG_REP_FRIENDLY, LANG_REP_HONORED, LANG_REP_REVERED,    LANG_REP_EXALTED
};

bool ChatHandler::HandlePausingMuteCommand(char* args)
{
    char* nameStr = ExtractOptNotLastArg(&args);

    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&nameStr, &target, &target_guid, &target_name))
        return false;

    uint32 notspeaktime;
    if (!ExtractUInt32(&args, notspeaktime))
        return false;

    std::string givenReason;
    if (char* givenReasonC = ExtractQuotedOrLiteralArg(&args))
        givenReason = givenReasonC;



    uint32 account_id = target ? target->GetSession()->GetAccountId() : sObjectMgr.GetPlayerAccountIdByGUID(target_guid);


    // find only player from same account if any
    if (!target)
    {
        if (WorldSession* session = sWorld.FindSession(account_id))
            target = session->GetPlayer();
    }

    time_t mutetime = time(nullptr) + notspeaktime * 60;

    // pausing uses world ms diff so multiply by 1000 for ms instead of s and use ms left instead of current time in future.
    mutetime = notspeaktime * 60 * 1000;

    if (target)
        target->GetSession()->m_muteTime = mutetime;

    LoginDatabase.PExecute("UPDATE account SET mutetime = " UI64FMTD " WHERE id = '%u'", uint64(mutetime), account_id);

    LoginDatabase.PExecute("UPDATE account SET flags = flags | 0x%x WHERE id = %u", ACCOUNT_FLAG_MUTED_PAUSING, account_id);
    if (target)
        target->GetSession()->SetAccountFlags(target->GetSession()->GetAccountFlags() | ACCOUNT_FLAG_MUTED_PAUSING);


    if (target)
        ChatHandler(target).PSendSysMessage(LANG_YOUR_CHAT_DISABLED, notspeaktime);

    std::string nameLink = playerLink(target_name);

    PSendSysMessage(LANG_YOU_DISABLE_CHAT, nameLink.c_str(), notspeaktime);

    // Add warning to the account
    std::string authorName = m_session ? m_session->GetPlayerName() : "Console";
    PlayerCacheData const* playerData = sObjectMgr.GetPlayerDataByGUID(target_guid);
    ASSERT(playerData);
    std::stringstream reason;
    reason << playerData->sName << " muted " << notspeaktime << " minutes (sticky)";
    if (!givenReason.empty())
        reason << " for \"" << givenReason << "\"";
    return true;
}

//mute player for some times
bool ChatHandler::HandleMuteCommand(char* args)
{
    char* nameStr = ExtractOptNotLastArg(&args);

    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&nameStr, &target, &target_guid, &target_name))
        return false;

    uint32 minutes;
    if (!ExtractUInt32(&args, minutes))
        return false;

    std::string givenReason;
    if (char* givenReasonC = ExtractQuotedOrLiteralArg(&args))
        givenReason = givenReasonC;

    uint32 const accountId = target ? target->GetSession()->GetAccountId() : sObjectMgr.GetPlayerAccountIdByGUID(target_guid);

    // find only player from same account if any
    if (!target)
    {
        if (WorldSession* session = sWorld.FindSession(accountId))
            target = session->GetPlayer();
    }

    // must have strong lesser security level
    if (HasLowerSecurity(target, target_guid, true))
        return false;

    time_t muteTime = time(nullptr) + minutes * MINUTE;

    if (target)
        target->GetSession()->m_muteTime = muteTime;

    LoginDatabase.PExecute("UPDATE `account` SET `mutetime` = " UI64FMTD " WHERE `id` = '%u'", uint64(muteTime), accountId);

    if (target)
        ChatHandler(target).PSendSysMessage(LANG_YOUR_CHAT_DISABLED, minutes);

    PSendSysMessage(LANG_YOU_DISABLE_CHAT, playerLink(target_name).c_str(), minutes);

    // Save mute history.
    std::string authorName = m_session ? m_session->GetPlayerName() : "Console";

    LoginDatabase.escape_string(givenReason);
    LoginDatabase.PExecute("INSERT INTO `account_muted` (`id`, `mutedate`, `unmutedate`, `mutedby`, `mutereason`) VALUES (%u, UNIX_TIMESTAMP(), UNIX_TIMESTAMP()+%u, '%s', '%s')",
        accountId, minutes * MINUTE, authorName.c_str(), givenReason.c_str());

    return true;
}

//unmute player
bool ChatHandler::HandleUnmuteCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &target_guid, &target_name))
        return false;

    uint32 account_id = target ? target->GetSession()->GetAccountId() : sObjectMgr.GetPlayerAccountIdByGUID(target_guid);

    // find only player from same account if any
    if (!target)
    {
        if (WorldSession* session = sWorld.FindSession(account_id))
            target = session->GetPlayer();
    }

    // must have strong lesser security level
    if (HasLowerSecurity(target, target_guid, true))
        return false;

    if (target)
    {
        if (target->CanSpeak())
        {
            SendSysMessage(LANG_CHAT_ALREADY_ENABLED);
            SetSentErrorMessage(true);
            return false;
        }

        target->GetSession()->m_muteTime = 0;
    }

    uint32 accountFlags = 0;

    if (target)
        accountFlags = target->GetSession()->GetAccountFlags();
    else
    {
        auto result = LoginDatabase.PQuery("SELECT `flags` FROM `account` WHERE `id` = '%u'", account_id);

        if (result)
        {
            accountFlags = result->Fetch()[0].GetUInt32();
            delete result;
        }

    }

    if (accountFlags & ACCOUNT_FLAG_MUTED_PAUSING)
    {
        LoginDatabase.PExecute("UPDATE account SET flags = flags & ~0x%x WHERE id = '%u'", ACCOUNT_FLAG_MUTED_PAUSING, account_id);
        if (target)
            target->GetSession()->SetAccountFlags(target->GetSession()->GetAccountFlags() & ~ACCOUNT_FLAG_MUTED_PAUSING);
    }

    LoginDatabase.PExecute("UPDATE account SET mutetime = '0' WHERE id = '%u'", account_id);

    if (target)
        ChatHandler(target).PSendSysMessage(LANG_YOUR_CHAT_ENABLED);

    std::string nameLink = playerLink(target_name);

    PSendSysMessage(LANG_YOU_ENABLE_CHAT, nameLink.c_str());
    return true;
}

void ChatHandler::ShowTriggerTargetListHelper(uint32 id, AreaTriggerTeleport const* at, bool subpart /*= false*/)
{
    if (m_session)
    {
        char dist_buf[50];
        if (!subpart)
        {
            float dist = m_session->GetPlayer()->GetDistance2d(at->destination);
            snprintf(dist_buf, 50, GetMangosString(LANG_TRIGGER_DIST), dist);
        }
        else
            dist_buf[0] = '\0';

        PSendSysMessage(LANG_TRIGGER_TARGET_LIST_CHAT,
            subpart ? " -> " : "", id, id, at->destination.mapId, at->destination.x, at->destination.y, at->destination.z, dist_buf);
    }
    else
        PSendSysMessage(LANG_TRIGGER_TARGET_LIST_CONSOLE,
            subpart ? " -> " : "", id, at->destination.mapId, at->destination.x, at->destination.y, at->destination.z);
}

void ChatHandler::ShowTriggerListHelper(AreaTriggerEntry const* atEntry)
{

    char const* tavern = sObjectMgr.IsTavernAreaTrigger(atEntry->id) ? GetMangosString(LANG_TRIGGER_TAVERN) : "";
    char const* quest = sObjectMgr.GetQuestForAreaTrigger(atEntry->id) ? GetMangosString(LANG_TRIGGER_QUEST) : "";

    if (m_session)
    {
        float dist = m_session->GetPlayer()->GetDistance2d(atEntry->x, atEntry->y);
        char dist_buf[50];
        snprintf(dist_buf, 50, GetMangosString(LANG_TRIGGER_DIST), dist);

        PSendSysMessage(LANG_TRIGGER_LIST_CHAT,
            atEntry->id, atEntry->id, atEntry->mapid, atEntry->x, atEntry->y, atEntry->z, dist_buf, tavern, quest);
    }
    else
        PSendSysMessage(LANG_TRIGGER_LIST_CONSOLE,
            atEntry->id, atEntry->mapid, atEntry->x, atEntry->y, atEntry->z, tavern, quest);

    if (AreaTriggerTeleport const* at = sObjectMgr.GetAreaTriggerTeleport(atEntry->id))
        ShowTriggerTargetListHelper(atEntry->id, at, true);
}

bool ChatHandler::HandleTriggerCommand(char* args)
{
    AreaTriggerEntry const* atEntry = nullptr;

    Player* player = m_session ? m_session->GetPlayer() : nullptr;

    // select by args
    if (*args)
    {
        uint32 atId;
        if (!ExtractUint32KeyFromLink(&args, "Hareatrigger", atId))
            return false;

        if (!atId)
            return false;

        atEntry = sObjectMgr.GetAreaTrigger(atId);

        if (!atEntry)
        {
            PSendSysMessage(LANG_COMMAND_GOAREATRNOTFOUND, atId);
            SetSentErrorMessage(true);
            return false;
        }
    }
    // find nearest
    else
    {
        if (!m_session)
            return false;

        float dist2 = MAP_SIZE * MAP_SIZE;

        Player* player = m_session->GetPlayer();

        // Search triggers
        for (auto const& itr : sObjectMgr.GetAreaTriggersMap())
        {
            AreaTriggerEntry const* atTestEntry = &itr.second;
            if (!atTestEntry)
                continue;

            if (atTestEntry->mapid != m_session->GetPlayer()->GetMapId())
                continue;

            float dx = atTestEntry->x - player->GetPositionX();
            float dy = atTestEntry->y - player->GetPositionY();

            float test_dist2 = dx * dx + dy * dy;

            if (test_dist2 >= dist2)
                continue;

            dist2 = test_dist2;
            atEntry = atTestEntry;
        }

        if (!atEntry)
        {
            SendSysMessage(LANG_COMMAND_NOTRIGGERFOUND);
            SetSentErrorMessage(true);
            return false;
        }
    }

    ShowTriggerListHelper(atEntry);

    int loc_idx = GetSessionDbLocaleIndex();

    AreaTriggerTeleport const* at = sObjectMgr.GetAreaTriggerTeleport(atEntry->id);
    if (at)
        PSendSysMessage(LANG_TRIGGER_REQ_LEVEL, at->requiredLevel);

    if (uint32 quest_id = sObjectMgr.GetQuestForAreaTrigger(atEntry->id))
    {
        SendSysMessage(LANG_TRIGGER_EXPLORE_QUEST);
        ShowQuestListHelper(quest_id, loc_idx, player);
    }

    return true;
}

bool ChatHandler::HandleTriggerActiveCommand(char* /*args*/)
{
    uint32 counter = 0;                                     // Counter for figure out that we found smth.

    Player* player = m_session->GetPlayer();

    // Search in AreaTable.dbc
    for (auto const& itr : sObjectMgr.GetAreaTriggersMap())
    {
        AreaTriggerEntry const* atEntry = &itr.second;
        if (!atEntry)
            continue;

        if (!IsPointInAreaTriggerZone(atEntry, player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ()))
            continue;

        ShowTriggerListHelper(atEntry);

        ++counter;
    }

    if (counter == 0)                                      // if counter == 0 then we found nth
        SendSysMessage(LANG_COMMAND_NOTRIGGERFOUND);

    return true;
}

bool ChatHandler::HandleTriggerNearCommand(char* args)
{
    float distance = (!*args) ? 10.0f : (float)atof(args);
    float dist2 = distance * distance;
    uint32 counter = 0;                                     // Counter for figure out that we found smth.

    Player* player = m_session->GetPlayer();

    // Search triggers
    for (auto const& itr : sObjectMgr.GetAreaTriggersMap())
    {
        AreaTriggerEntry const* atEntry = &itr.second;
        if (!atEntry)
            continue;

        if (atEntry->mapid != m_session->GetPlayer()->GetMapId())
            continue;

        float dx = atEntry->x - player->GetPositionX();
        float dy = atEntry->y - player->GetPositionY();

        if (dx * dx + dy * dy > dist2)
            continue;

        ShowTriggerListHelper(atEntry);

        ++counter;
    }

    // Search trigger targets
    for (auto const& itr : sObjectMgr.GetAreaTriggersMap())
    {
        AreaTriggerEntry const* atEntry = &itr.second;
        if (!atEntry)
            continue;

        AreaTriggerTeleport const* at = sObjectMgr.GetAreaTriggerTeleport(atEntry->id);
        if (!at)
            continue;

        if (at->destination.mapId != m_session->GetPlayer()->GetMapId())
            continue;

        float dx = at->destination.x - player->GetPositionX();
        float dy = at->destination.y - player->GetPositionY();

        if (dx * dx + dy * dy > dist2)
            continue;

        ShowTriggerTargetListHelper(atEntry->id, at);

        ++counter;
    }

    if (counter == 0)                                      // if counter == 0 then we found nth
        SendSysMessage(LANG_COMMAND_NOTRIGGERFOUND);

    return true;
}

bool ChatHandler::HandleGoTargetCommand(char* /*args*/)
{
    Unit* pTarget = GetSelectedUnit();

    if (!pTarget || !m_session->GetPlayer()->GetSelectionGuid() || !m_session->GetPlayer()->IsInMap(pTarget))
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->NearTeleportTo(pTarget->GetPosition(), TELE_TO_GM_MODE);

    return true;
}

bool ChatHandler::HandleGoTaxinodeCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();

    uint32 nodeId;
    if (!ExtractUint32KeyFromLink(&args, "Htaxinode", nodeId))
        return false;

    TaxiNodesEntry const* node = sObjectMgr.GetTaxiNodeEntry(nodeId);
    if (!node)
    {
        PSendSysMessage(LANG_COMMAND_GOTAXINODENOTFOUND, nodeId);
        SetSentErrorMessage(true);
        return false;
    }

    if (node->x == 0.0f && node->y == 0.0f && node->z == 0.0f)
    {
        PSendSysMessage(LANG_INVALID_TARGET_COORD, node->x, node->y, node->map_id);
        SetSentErrorMessage(true);
        return false;
    }

    return HandleGoHelper(pPlayer, node->map_id, node->x, node->y, &node->z);
}

static char const* const areatriggerKeys[] =
{
    "Hareatrigger",
    "Hareatrigger_target",
    nullptr
};

bool ChatHandler::HandleGoTriggerCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();

    if (!*args)
        return false;

    char* atIdStr = ExtractKeyFromLink(&args, areatriggerKeys);
    if (!atIdStr)
        return false;

    uint32 atId;
    if (!ExtractUInt32(&atIdStr, atId))
        return false;

    if (!atId)
        return false;

    AreaTriggerEntry const* atEntry = sObjectMgr.GetAreaTrigger(atId);
    if (!atEntry)
    {
        PSendSysMessage(LANG_COMMAND_GOAREATRNOTFOUND, atId);
        SetSentErrorMessage(true);
        return false;
    }

    bool to_target = ExtractLiteralArg(&args, "target");
    if (!to_target && *args)                                // can be fail also at syntax error
        return false;

    if (to_target)
    {
        AreaTriggerTeleport const* at = sObjectMgr.GetAreaTriggerTeleport(atId);
        if (!at)
        {
            PSendSysMessage(LANG_AREATRIGER_NOT_HAS_TARGET, atId);
            SetSentErrorMessage(true);
            return false;
        }

        return HandleGoHelper(pPlayer, at->destination.mapId, at->destination.x, at->destination.y, &at->destination.z);
    }
    else
        return HandleGoHelper(pPlayer, atEntry->mapid, atEntry->x, atEntry->y, &atEntry->z);
}

enum CreatureLinkType
{
    CREATURE_LINK_RAW = -1,                   // non-link case
    CREATURE_LINK_GUID = 0,
    CREATURE_LINK_ENTRY = 1,
};

static char const* const creatureKeys[] =
{
    "Hcreature",
    "Hcreature_entry",
    nullptr
};

/** \brief Teleport the GM to the specified creature
*
* .go creature <GUID>     --> TP using creature.guid
* .go creature azuregos   --> TP player to the mob with this name
*                             Warning: If there is more than one mob with this name
*                                      you will be teleported to the first one that is found.
* .go creature id 6109    --> TP player to the mob, that has this creature_template.entry
*                             Warning: If there is more than one mob with this "id"
*                                      you will be teleported to the first one that is found.
*/
//teleport to creature
bool ChatHandler::HandleGoCreatureCommand(char* args)
{
    if (!*args)
        return false;

    Player* pPlayer = m_session->GetPlayer();

    // "id" or number or [name] Shift-click form |color|Hcreature:creature_id|h[name]|h|r
    int crType;
    char* pParam1 = ExtractKeyFromLink(&args, creatureKeys, &crType);
    if (!pParam1)
        return false;

    // User wants to teleport to the NPC's template entry
    if (crType == CREATURE_LINK_RAW && strcmp(pParam1, "id") == 0)
    {
        // number or [name] Shift-click form |color|Hcreature_entry:creature_id|h[name]|h|r
        pParam1 = ExtractKeyFromLink(&args, "Hcreature_entry");
        if (!pParam1)
            return false;

        crType = CREATURE_LINK_ENTRY;
    }

    CreatureData const* data = nullptr;

    uint32 lowguid = 0;
    switch (crType)
    {
        case CREATURE_LINK_ENTRY:
        {
            uint32 tEntry;
            if (!ExtractUInt32(&pParam1, tEntry))
                return false;

            if (!tEntry)
                return false;

            if (!sObjectMgr.GetCreatureTemplate(tEntry))
            {
                SendSysMessage(LANG_COMMAND_GOCREATNOTFOUND);
                SetSentErrorMessage(true);
                return false;
            }

            FindCreatureData worker(tEntry, m_session ? m_session->GetPlayer() : nullptr);

            sObjectMgr.DoCreatureData(worker);

            CreatureDataPair const* dataPair = worker.GetResult();
            if (!dataPair)
            {
                SendSysMessage(LANG_COMMAND_GOCREATNOTFOUND);
                SetSentErrorMessage(true);
                return false;
            }

            data = &dataPair->second;
            lowguid = dataPair->first;
            break;
        }
        case CREATURE_LINK_GUID:
        {
            if (!ExtractUInt32(&pParam1, lowguid))
                return false;

            data = sObjectMgr.GetCreatureData(lowguid);
            if (!data)
            {
                SendSysMessage(LANG_COMMAND_GOCREATNOTFOUND);
                SetSentErrorMessage(true);
                return false;
            }
            break;
        }
        case CREATURE_LINK_RAW:
        {
            if (ExtractUInt32(&pParam1, lowguid))
            {
                data = sObjectMgr.GetCreatureData(lowguid);
                if (!data)
                {
                    SendSysMessage(LANG_COMMAND_GOCREATNOTFOUND);
                    SetSentErrorMessage(true);
                    return false;
                }
            }
            // Number is invalid - maybe the user specified the mob's name
            else
            {
                std::string name = pParam1;
                WorldDatabase.escape_string(name);
                QueryResult* result = WorldDatabase.PQuery("SELECT guid FROM creature, creature_template WHERE creature.id = creature_template.entry AND creature_template.name " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'"), name.c_str());
                if (!result)
                {
                    SendSysMessage(LANG_COMMAND_GOCREATNOTFOUND);
                    SetSentErrorMessage(true);
                    return false;
                }

                FindCreatureData worker(0, m_session ? m_session->GetPlayer() : nullptr);

                do
                {
                    Field* fields = result->Fetch();
                    uint32 guid = fields[0].GetUInt32();

                    CreatureDataPair const* cr_data = sObjectMgr.GetCreatureDataPair(guid);
                    if (!cr_data)
                        continue;

                    worker(*cr_data);

                } while (result->NextRow());

                delete result;

                CreatureDataPair const* dataPair = worker.GetResult();
                if (!dataPair)
                {
                    SendSysMessage(LANG_COMMAND_GOCREATNOTFOUND);
                    SetSentErrorMessage(true);
                    return false;
                }

                data = &dataPair->second;
                lowguid = dataPair->first;
            }
            break;
        }
    }

    float x, y, z;
    x = data->position.x;
    y = data->position.y;
    z = data->position.z;
    if (Creature* creature = pPlayer->GetMap()->GetCreature(data->GetObjectGuid(lowguid)))
        creature->GetPosition(x, y, z);

    return HandleGoHelper(pPlayer, data->position.mapId, x, y, &z);
}

enum GameobjectLinkType
{
    GAMEOBJECT_LINK_RAW = -1,                   // non-link case
    GAMEOBJECT_LINK_GUID = 0,
    GAMEOBJECT_LINK_ENTRY = 1,
};

static char const* const gameobjectKeys[] =
{
    "Hgameobject",
    "Hgameobject_entry",
    nullptr
};

//teleport to gameobject
bool ChatHandler::HandleGoObjectCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();

    // number or [name] Shift-click form |color|Hgameobject:go_guid|h[name]|h|r
    int goType;
    char* pParam1 = ExtractKeyFromLink(&args, gameobjectKeys, &goType);
    if (!pParam1)
        return false;

    // User wants to teleport to the GO's template entry
    if (goType == GAMEOBJECT_LINK_RAW && strcmp(pParam1, "id") == 0)
    {
        // number or [name] Shift-click form |color|Hgameobject_entry:creature_id|h[name]|h|r
        pParam1 = ExtractKeyFromLink(&args, "Hgameobject_entry");
        if (!pParam1)
            return false;

        goType = GAMEOBJECT_LINK_ENTRY;
    }

    GameObjectData const* data = nullptr;

    switch (goType)
    {
        case CREATURE_LINK_ENTRY:
        {
            uint32 tEntry;
            if (!ExtractUInt32(&pParam1, tEntry))
                return false;

            if (!tEntry)
                return false;

            if (!sObjectMgr.GetGameObjectInfo(tEntry))
            {
                SendSysMessage(LANG_COMMAND_GOOBJNOTFOUND);
                SetSentErrorMessage(true);
                return false;
            }

            FindGOData worker(tEntry, m_session ? m_session->GetPlayer() : nullptr);

            sObjectMgr.DoGOData(worker);

            GameObjectDataPair const* dataPair = worker.GetResult();
            if (!dataPair)
            {
                SendSysMessage(LANG_COMMAND_GOOBJNOTFOUND);
                SetSentErrorMessage(true);
                return false;
            }

            data = &dataPair->second;
            break;
        }
        case GAMEOBJECT_LINK_GUID:
        {
            uint32 lowguid;
            if (!ExtractUInt32(&pParam1, lowguid))
                return false;

            // by DB guid
            data = sObjectMgr.GetGOData(lowguid);
            if (!data)
            {
                SendSysMessage(LANG_COMMAND_GOOBJNOTFOUND);
                SetSentErrorMessage(true);
                return false;
            }
            break;
        }
        case GAMEOBJECT_LINK_RAW:
        {
            uint32 lowguid;
            if (ExtractUInt32(&pParam1, lowguid))
            {
                // by DB guid
                data = sObjectMgr.GetGOData(lowguid);
                if (!data)
                {
                    SendSysMessage(LANG_COMMAND_GOOBJNOTFOUND);
                    SetSentErrorMessage(true);
                    return false;
                }
            }
            else
            {
                std::string name = pParam1;
                WorldDatabase.escape_string(name);
                QueryResult* result = WorldDatabase.PQuery("SELECT guid FROM gameobject, gameobject_template WHERE gameobject.id = gameobject_template.entry AND gameobject_template.name " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'"), name.c_str());
                if (!result)
                {
                    SendSysMessage(LANG_COMMAND_GOOBJNOTFOUND);
                    SetSentErrorMessage(true);
                    return false;
                }

                FindGOData worker(0, m_session ? m_session->GetPlayer() : nullptr);

                do
                {
                    Field* fields = result->Fetch();
                    uint32 guid = fields[0].GetUInt32();

                    GameObjectDataPair const* go_data = sObjectMgr.GetGODataPair(guid);
                    if (!go_data)
                        continue;

                    worker(*go_data);

                } while (result->NextRow());

                delete result;

                GameObjectDataPair const* dataPair = worker.GetResult();
                if (!dataPair)
                {
                    SendSysMessage(LANG_COMMAND_GOOBJNOTFOUND);
                    SetSentErrorMessage(true);
                    return false;
                }

                data = &dataPair->second;
            }
            break;
        }
    }

    return HandleGoHelper(pPlayer, data->position.mapId, data->position.x, data->position.y, &data->position.z);
}

bool ChatHandler::HandleGameObjectTargetCommand(char* args)
{
    Player* pl = m_session->GetPlayer();

    if (!pl)
        return false;

    QueryResult* result;
    GameEventMgr::ActiveEvents const& activeEventsList = sGameEventMgr.GetActiveEventList();
    if (*args)
    {
        // number or [name] Shift-click form |color|Hgameobject_entry:go_id|h[name]|h|r
        char* cId = ExtractKeyFromLink(&args, "Hgameobject_entry");
        if (!cId)
            return false;

        uint32 id;
        if (ExtractUInt32(&cId, id))
        {
            result = WorldDatabase.PQuery("SELECT guid, id, position_x, position_y, position_z, orientation, map, (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ FROM gameobject WHERE map = '%i' AND id = '%u' ORDER BY order_ ASC LIMIT 1",
                pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), pl->GetMapId(), id);
        }
        else
        {
            std::string name = cId;
            WorldDatabase.escape_string(name);
            result = WorldDatabase.PQuery(
                "SELECT guid, id, position_x, position_y, position_z, orientation, map, (POW(position_x - %f, 2) + POW(position_y - %f, 2) + POW(position_z - %f, 2)) AS order_ "
                "FROM gameobject,gameobject_template WHERE gameobject_template.entry = gameobject.id AND map = %i AND name " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'")" ORDER BY order_ ASC LIMIT 1",
                pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), pl->GetMapId(), name.c_str());
        }
    }
    else
    {
        std::ostringstream eventFilter;
        eventFilter << " AND (event IS NULL ";
        bool initString = true;

        for (const auto itr : activeEventsList)
        {
            if (initString)
            {
                eventFilter  <<  "OR event IN (" << itr;
                initString = false;
            }
            else
                eventFilter << "," << itr;
        }

        if (!initString)
            eventFilter << "))";
        else
            eventFilter << ")";

        result = WorldDatabase.PQuery("SELECT gameobject.guid, id, position_x, position_y, position_z, orientation, map, "
            "(POW(position_x - %f, 2) + POW(position_y - %f, 2) + POW(position_z - %f, 2)) AS order_ FROM gameobject "
            "LEFT OUTER JOIN game_event_gameobject on gameobject.guid=game_event_gameobject.guid WHERE map = '%i' %s ORDER BY order_ ASC LIMIT 10",
            pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), pl->GetMapId(), eventFilter.str().c_str());
    }

    if (!result)
    {
        SendSysMessage(LANG_COMMAND_TARGETOBJNOTFOUND);
        return true;
    }

    bool found = false;
    float x, y, z, o;
    uint32 lowguid, id;
    uint16 mapid, pool_id;

    do
    {
        Field* fields = result->Fetch();
        lowguid = fields[0].GetUInt32();
        id = fields[1].GetUInt32();
        x = fields[2].GetFloat();
        y = fields[3].GetFloat();
        z = fields[4].GetFloat();
        o = fields[5].GetFloat();
        mapid = fields[6].GetUInt16();
        pool_id = sPoolMgr.IsPartOfAPool<GameObject>(lowguid);
        if (!pool_id || pl->GetMap()->GetPersistentState()->IsSpawnedPoolObject<GameObject>(lowguid))
            found = true;
    }     while (result->NextRow() && (!found));

    delete result;

    if (!found)
    {
        PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, id);
        return false;
    }

    GameObjectInfo const* goI = sObjectMgr.GetGameObjectInfo(id);

    if (!goI)
    {
        PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, id);
        return false;
    }

    GameObject* target = m_session->GetPlayer()->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, id, lowguid));

    PSendSysMessage(LANG_GAMEOBJECT_DETAIL, lowguid, goI->name.c_str(), lowguid, id, x, y, z, mapid, o);

    if (target)
    {
        time_t curRespawnDelay = target->GetRespawnTimeEx() - time(nullptr);
        if (curRespawnDelay < 0)
            curRespawnDelay = 0;

        std::string curRespawnDelayStr = secsToTimeString(curRespawnDelay, true);
        std::string defRespawnDelayStr = secsToTimeString(target->GetRespawnDelay(), true);

        PSendSysMessage(LANG_COMMAND_RAWPAWNTIMES, defRespawnDelayStr.c_str(), curRespawnDelayStr.c_str());

        ShowNpcOrGoSpawnInformation<GameObject>(target->GetGUIDLow());
    }
    return true;
}

//delete object by selection or guid
bool ChatHandler::HandleGameObjectDeleteCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_guid|h[name]|h|r

    auto object = getSelectedGameObject();

    uint32 entry = 0;
    uint32 lowguid = 0;
    if (!object)
    {
        if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
            return false;

        if (!lowguid)
            return false;

        if (!ExtractUInt32(&args, entry))
            entry = 0;
    }

    auto player = GetSession()->GetPlayer();

    if (!player)
        return false;



    GameObject* obj = object;

    if (!obj)
    {
        if (!entry)
        {
            // by DB guid
            if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
                obj = GetGameObjectWithGuidGlobal(lowguid, go_data);
        }
        else
            obj = GetGameObjectWithGuid(lowguid, entry);
    }

    if (!obj)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    if (ObjectGuid ownerGuid = obj->GetOwnerGuid())
    {
        Unit* owner = ObjectAccessor::GetUnit(*m_session->GetPlayer(), ownerGuid);
        if (!owner || !ownerGuid.IsPlayer())
        {
            PSendSysMessage(LANG_COMMAND_DELOBJREFERCREATURE, obj->GetGUIDLow(), ownerGuid.GetString().c_str());
            SetSentErrorMessage(true);
            return false;
        }

        owner->RemoveGameObject(obj, false);
    }

    obj->SetRespawnTime(0);                                 // not save respawn time
    obj->Delete();
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    obj->DeleteFromDB();

    PSendSysMessage(LANG_COMMAND_DELOBJMESSAGE, obj->GetGUIDLow());

    return true;
}

//turn selected object
bool ChatHandler::HandleGameObjectTurnCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
    uint32 lowguid;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
        return false;

    if (!lowguid)
        return false;

    GameObject* obj = nullptr;

    // by DB guid
    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
        obj = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!obj)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    float o;
    if (!ExtractOptFloat(&args, o, m_session->GetPlayer()->GetOrientation()))
        return false;

    Map* map = obj->GetMap();
    map->Remove(obj, false);

    obj->Relocate(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), o);
    obj->SetFloatValue(GAMEOBJECT_FACING, o);
    obj->UpdateRotationFields();

    map->Add(obj);

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    obj->SaveToDB();
    obj->Refresh();

    PSendSysMessage(LANG_COMMAND_TURNOBJMESSAGE, obj->GetGUIDLow(), obj->GetGOInfo()->name.c_str(), obj->GetGUIDLow());

    return true;
}

bool ChatHandler::HandleGameObjectScaleCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
    uint32 lowguid;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
        return false;

    if (!lowguid)
        return false;

    GameObject* obj = nullptr;

    // by DB guid
    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
        obj = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!obj)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    float scale;
    if (!ExtractOptFloat(&args, scale, 1.0f))
        return false;

    if (scale < 0.1f || scale >= 50.1f)
    {
        ChatHandler(m_session->GetPlayer()).PSendSysMessage("Scale must be between 0.1 and 50.");
        return false;
    }

    Map* map = obj->GetMap();
    map->Remove(obj, false);

    obj->SetObjectScale(scale);
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sGuidObjectScaling.AddOrEdit(obj->GetGUID(), scale);
    obj->UpdateRotationFields();

    map->Add(obj);

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    obj->SaveToDB();
    obj->Refresh();

    ChatHandler(m_session->GetPlayer()).PSendSysMessage("%u, Entry %u %s -  scaled to %f", obj->GetGUIDLow(), obj->GetGUIDLow(), obj->GetGOInfo()->name.c_str(), scale);

    return true;
}

bool ChatHandler::HandleGameObjectInfoCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
    uint32 lowguid;
    GameObject* pGameObject = nullptr;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
    {
        pGameObject = getSelectedGameObject();
    }

    if (!lowguid && !pGameObject)
        return false;

    

    // by DB guid
    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid); !pGameObject && go_data)
        pGameObject = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!pGameObject)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Entry: %u, GUID: %u\nName: %s\nType: %u, Display Id: %u\nGO State: %u, Loot State: %u",
        pGameObject->GetEntry(),
        pGameObject->GetGUIDLow(),
        pGameObject->GetGOInfo()->name.c_str(),
        pGameObject->GetGoType(),
        pGameObject->GetDisplayId(),
        pGameObject->GetGoState(),
        pGameObject->getLootState());
    if (pGameObject->GetUInt32Value(GAMEOBJECT_FLAGS))
        PSendSysMessage("Flags: %s", FlagsToString(pGameObject->GetUInt32Value(GAMEOBJECT_FLAGS), GameObjectFlagToString).c_str());
    if (pGameObject->GetUInt32Value(GAMEOBJECT_DYN_FLAGS))
        PSendSysMessage("Dynamic Flags: %s", FlagsToString(pGameObject->GetUInt32Value(GAMEOBJECT_DYN_FLAGS), GameObjectDynamicFlagToString).c_str());
    if (pGameObject->GetVisibilityModifier())
        PSendSysMessage("Visibility Modifier: %g", pGameObject->GetVisibilityModifier());
    if (pGameObject->isActiveObject())
        SendSysMessage("Active Object.");
    if (pGameObject->isSpawned())
        SendSysMessage("Object is spawned.");
    else
    {
        time_t respawnTime = pGameObject->GetRespawnTime();
        std::tm* pTime = std::localtime(&respawnTime);
        PSendSysMessage("Not spawned. Respawns in %u seconds (%u:%u:%u).", pGameObject->GetRespawnDelay(), pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
    }

    return true;
}

//move selected object
bool ChatHandler::HandleGameObjectMoveCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_guid|h[name]|h|r
    uint32 lowguid;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
        return false;

    if (!lowguid)
        return false;

    GameObject* obj = nullptr;

    // by DB guid
    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
        obj = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!obj)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 unsuccessfulExtractions = 0;

    float x;
    if (!ExtractFloat(&args, x))
        ++unsuccessfulExtractions;

    float y;
    if (!ExtractFloat(&args, y))
        ++unsuccessfulExtractions;

    float z;
    if (!ExtractFloat(&args, z))
        ++unsuccessfulExtractions;


    Player* chr = m_session->GetPlayer();

    constexpr uint32 ExpectedFailedExtractions = 3; // X Y Z params, if not supplied, use chars pos.

    bool shouldCheckCoords = false;

    // Wrong input, we only continue if all coords are fine or none are fine.
    if (unsuccessfulExtractions != ExpectedFailedExtractions && unsuccessfulExtractions != 0)
        return false;


    if (unsuccessfulExtractions == ExpectedFailedExtractions) // no params, use chars pos.
    {
        x = chr->GetPositionX();
        y = chr->GetPositionY();
        z = chr->GetPositionZ();
    }
    else // just filled in coords, check valid coords too.
        shouldCheckCoords = true;

    if (shouldCheckCoords && !MapManager::IsValidMapCoord(obj->GetMapId(), x, y, z))
    {
        PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, obj->GetMapId());
        SetSentErrorMessage(true);
        return false;
    }

    Map* map = obj->GetMap();
    map->Remove(obj, false);

    obj->Relocate(x, y, z, obj->GetOrientation());
    obj->SetFloatValue(GAMEOBJECT_POS_X, x);
    obj->SetFloatValue(GAMEOBJECT_POS_Y, y);
    obj->SetFloatValue(GAMEOBJECT_POS_Z, z);
    obj->SetFloatValue(GAMEOBJECT_FACING, obj->GetOrientation());

    map->Add(obj);

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    obj->SaveToDB();
    obj->Refresh();

    PSendSysMessage(LANG_COMMAND_MOVEOBJMESSAGE, obj->GetGUIDLow(), obj->GetGOInfo()->name.c_str(), obj->GetGUIDLow());

    return true;
}

bool ChatHandler::HandleGameObjectRotateCommand(char* args)
{
    uint32 lowguid;

    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
        return false;

    if (!lowguid)
        return false;

    GameObject* obj = nullptr;

    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
        obj = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!obj || !args)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    float o;

    if (!ExtractFloat(&args, o))
        return false;

    if (o <= -10.0F || o >= 10.0F)
    {
        SendSysMessage("Can't be higher than 10 or lower than -10.");
        SetSentErrorMessage(true);
        return false;
    }

    if (!MapManager::IsValidMapCoord(obj->GetMapId(), obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ()))
    {
        PSendSysMessage(LANG_INVALID_TARGET_COORD, obj->GetPositionX(), obj->GetPositionY(), obj->GetMapId());
        SetSentErrorMessage(true);
        return false;
    }

    Map* map = obj->GetMap();
    map->Remove(obj, false);

    obj->Relocate(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), o);
    obj->SetFloatValue(GAMEOBJECT_FACING, o);
    obj->UpdateRotationFields(0.0F, 0.0F);
    map->Add(obj);
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    obj->SaveToDB();
    obj->Refresh();

    PSendSysMessage(LANG_COMMAND_MOVEOBJMESSAGE, obj->GetGUIDLow(), obj->GetGOInfo()->name.c_str(), obj->GetGUIDLow());
    return true;
}

//spawn go
bool ChatHandler::HandleGameObjectAddCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject_entry:go_id|h[name]|h|r
    uint32 id;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject_entry", id))
        return false;

    if (!id)
        return false;

    int32 spawntimeSecs;
    if (!ExtractOptInt32(&args, spawntimeSecs, 0))
        return false;

    const GameObjectInfo* gInfo = sObjectMgr.GetGameObjectInfo(id);

    if (!gInfo)
    {
        PSendSysMessage(LANG_GAMEOBJECT_NOT_EXIST, id);
        SetSentErrorMessage(true);
        return false;
    }

    if (gInfo->displayId && !sGameObjectDisplayInfoStore.LookupEntry(gInfo->displayId))
    {
        // report to DB errors log as in loading case
        sLog.outErrorDb("Gameobject (Entry %u GoType: %u) have invalid displayId (%u), not spawned.", id, gInfo->type, gInfo->displayId);
        PSendSysMessage(LANG_GAMEOBJECT_HAVE_INVALID_DATA, id);
        SetSentErrorMessage(true);
        return false;
    }

    Player* chr = m_session->GetPlayer();
    float x = float(chr->GetPositionX());
    float y = float(chr->GetPositionY());
    float z = float(chr->GetPositionZ());
    float o = float(chr->GetOrientation());
    Map* map = chr->GetMap();

    GameObject* pGameObj = new GameObject;

    // used guids from specially reserved range (can be 0 if no free values)
    uint32 db_lowGUID = sObjectMgr.GenerateStaticGameObjectLowGuid();
    if (!db_lowGUID)
    {
        SendSysMessage(LANG_NO_FREE_STATIC_GUID_FOR_SPAWN);
        SetSentErrorMessage(true);
        delete pGameObj;
        return false;
    }

    if (!pGameObj->Create(db_lowGUID, gInfo->id, map, x, y, z, o, 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
    {
        delete pGameObj;
        return false;
    }

    if (spawntimeSecs)
        pGameObj->SetRespawnTime(spawntimeSecs);
    pGameObj->SetRespawnTime(300); // Default 5 min.

    // fill the gameobject data and save to the db
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    pGameObj->SaveToDB(map->GetId());

    // this will generate a new guid if the object is in an instance
    if (!pGameObj->LoadFromDB(db_lowGUID, map))
    {
        delete pGameObj;
        return false;
    }

    DEBUG_LOG(GetMangosString(LANG_GAMEOBJECT_CURRENT), gInfo->name.c_str(), db_lowGUID, x, y, z, o);

    map->Add(pGameObj);

    sObjectMgr.AddGameobjectToGrid(db_lowGUID, sObjectMgr.GetGOData(db_lowGUID));
    PSendSysMessage(LANG_GAMEOBJECT_ADD, id, gInfo->name.c_str(), db_lowGUID, x, y, z);
    return true;
}

bool ChatHandler::HandleGameObjectNearCommand(char* args)
{
    float distance;
    if (!ExtractOptFloat(&args, distance, 10.0f))
        return false;

    uint32 count = 0;

    Player* pl = m_session->GetPlayer();
    /*QueryResult* result = WorldDatabase.PQuery("SELECT guid, id, position_x, position_y, position_z, map, "
        "(POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) AS order_ "
        "FROM gameobject WHERE map='%u' AND (POW(position_x - '%f', 2) + POW(position_y - '%f', 2) + POW(position_z - '%f', 2)) <= '%f' ORDER BY order_",
        pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(),
        pl->GetMapId(), pl->GetPositionX(), pl->GetPositionY(), pl->GetPositionZ(), distance * distance);

    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();
            float x = fields[2].GetFloat();
            float y = fields[3].GetFloat();
            float z = fields[4].GetFloat();
            int mapid = fields[5].GetUInt16();

            GameObjectInfo const* gInfo = sObjectMgr.GetGameObjectInfo(entry);

            if (!gInfo)
                continue;

            PSendSysMessage(LANG_GO_MIXED_LIST_CHAT, guid, PrepareStringNpcOrGoSpawnInformation<GameObject>(guid).c_str(), entry, guid, gInfo->name, x, y, z, mapid);

            ++count;
        }         while (result->NextRow());

        delete result;
    }*/

    MaNGOS::AllGameObjectsInRange check(pl, distance);
    std::list<GameObject*> gameObjects;
    MaNGOS::GameObjectListSearcher<MaNGOS::AllGameObjectsInRange> searcher(gameObjects, check);

    Cell::VisitGridObjects(pl, searcher, distance);

    for (const auto& go : gameObjects)
    {
        PSendSysMessage(LANG_GO_MIXED_LIST_CHAT, go->GetGUIDLow(), PrepareStringNpcOrGoSpawnInformation<GameObject>(go->GetGUIDLow()).c_str(), go->GetEntry(), go->GetGUIDLow()
            , go->GetName(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetMapId());
        ++count;
    }

    PSendSysMessage(LANG_COMMAND_NEAROBJMESSAGE, distance, count);
    return true;
}

class NearestGameObjectInObjectRangeCheck
{
public:
    NearestGameObjectInObjectRangeCheck(WorldObject const& obj, float range) : i_obj(obj), i_range(range) {}
    WorldObject const& GetFocusObject() const { return i_obj; }
    bool operator()(GameObject* go)
    {
        if (i_obj.IsWithinDistInMap(go, i_range))
        {
            i_range = i_obj.GetDistance(go);        // use found GO range as new range limit for next check
            return true;
        }
        return false;
    }
    float GetLastRange() const { return i_range; }
private:
    WorldObject const& i_obj;
    float  i_range;
};


bool ChatHandler::HandleGameObjectSelectCommand(char* args)
{
    float dist;
    if (!ExtractOptFloat(&args, dist, 10.0f))
        return false;

    Player* player = m_session->GetPlayer();

    GameObject* go = nullptr;
    CellPair pair(MaNGOS::ComputeCellPair(player->GetPositionX(), player->GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    NearestGameObjectInObjectRangeCheck go_check(*player, dist);
    MaNGOS::GameObjectLastSearcher<NearestGameObjectInObjectRangeCheck> searcher(go, go_check);

    TypeContainerVisitor<MaNGOS::GameObjectLastSearcher<NearestGameObjectInObjectRangeCheck>, GridTypeMapContainer> go_searcher(searcher);
    cell.Visit(pair, go_searcher, *(player->GetMap()), *player, dist);
    if (go)
    {
        player->SetSelectedGobj(go->GetObjectGuid());
        GameObjectInfo const* data = go->GetGOInfo();
        PSendSysMessage(LANG_GO_MIXED_LIST_CHAT, go->GetDBTableGUIDLow(), PrepareStringNpcOrGoSpawnInformation<GameObject>(go->GetObjectGuid()).c_str(), go->GetEntry(), go->GetDBTableGUIDLow(), data ? data->name.c_str() : go->GetName(), go->GetPositionX(), go->GetPositionY(), go->GetPositionZ(), go->GetMapId());
    }
    else
        SendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);
    return true;
}

GameObject* ChatHandler::getSelectedGameObject()
{
    Player* player = m_session->GetPlayer();
    return player->GetMap()->GetGameObject(player->GetSelectedGobj());
}

bool ChatHandler::HandleGameObjectDespawnCommand(char*)
{
    GameObject* go = getSelectedGameObject();
    if (!go)
    {
        SendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);
        return false;
    }
    go->Despawn();
    return true;
}

bool ChatHandler::HandleGameObjectToggleCommand(char* args)
{
    uint32 lowguid;
    GameObject* obj = nullptr;

    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
    {
        // no args passed, search for already selected GO
        obj = getSelectedGameObject();
        if (!obj)
        {
            PSendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);
            SetSentErrorMessage(true);
            return false;
        }
    }
    else
    {
        // lowguid arg found, search DB for GO
        if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
            obj = GetGameObjectWithGuid(lowguid, go_data->id);

        if (!obj)
        {
            PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if (obj->getLootState() == GO_READY || obj->getLootState() == GO_JUST_DEACTIVATED)
        obj->UseDoorOrButton();
    else
        obj->ResetDoorOrButton();

    return true;
}

bool ChatHandler::HandleGameObjectResetCommand(char*)
{
    GameObject* go = getSelectedGameObject();
    if (!go)
    {
        SendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);
        return false;
    }
    go->ResetDoorOrButton();
    return true;
}

bool ChatHandler::HandleGameObjectRespawnCommand(char*)
{
    GameObject* go = getSelectedGameObject();
    if (!go)
    {
        SendSysMessage(LANG_COMMAND_NOGAMEOBJECTFOUND);
        return false;
    }
    go->Respawn();
    return true;
}

bool ChatHandler::HandleGameObjectSetGoStateCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
    uint32 lowguid;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
        return false;

    if (!lowguid)
        return false;

    GameObject* pGameObject = nullptr;

    // by DB guid
    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
        pGameObject = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!pGameObject)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 uiGoState;
    if (!ExtractUInt32(&args, uiGoState))
        return false;

    if (uiGoState > GO_STATE_ACTIVE_ALTERNATIVE)
        return false;

    pGameObject->SetGoState(GOState(uiGoState));
    PSendSysMessage("You changed GO State of %s (GUID %u) to %u.", pGameObject->GetName(), pGameObject->GetGUIDLow(), uiGoState);

    return true;
}

bool ChatHandler::HandleGameObjectSetLootStateCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
    uint32 lowguid;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
        return false;

    if (!lowguid)
        return false;

    GameObject* pGameObject = nullptr;

    // by DB guid
    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
        pGameObject = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!pGameObject)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 uiLootState;
    if (!ExtractUInt32(&args, uiLootState))
        return false;

    if (uiLootState > GO_JUST_DEACTIVATED)
        return false;

    pGameObject->SetLootState(LootState(uiLootState));
    PSendSysMessage("You changed Loot State of %s (GUID %u) to %u.", pGameObject->GetName(), pGameObject->GetGUIDLow(), uiLootState);

    return true;
}

bool ChatHandler::HandleGameObjectSetRespawnTimeCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
    uint32 lowguid;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
        return false;

    if (!lowguid)
        return false;

    uint32 timeMin;
    if (!ExtractUInt32(&args, timeMin))
        return false;
    uint32 timeMax;
    if (!ExtractUInt32(&args, timeMax))
        timeMax = timeMin;

    GameObject* pGameObject = nullptr;

    // by DB guid
    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
        pGameObject = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!pGameObject)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    GameObjectData* pData = const_cast<GameObjectData*>(pGameObject->GetGOData());
    if (!pData)
    {
        SendSysMessage("This gameobject is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    pData->spawntimesecsmin = timeMin;
    pData->spawntimesecsmax = timeMax;
    pGameObject->SetRespawnDelay((timeMin + timeMax) / 2);
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `gameobject` SET `spawntimesecsmin`=%u, `spawntimesecsmax`=%u WHERE `guid`=%u", timeMin, timeMax, pGameObject->GetDBTableGUIDLow());
    PSendSysMessage("Respawn time for guid %u updated to %u-%u.", pGameObject->GetDBTableGUIDLow(), timeMin, timeMax);
    return true;
}

void ChatHandler::ShowFactionListHelper(FactionEntry const* factionEntry, LocaleConstant loc, FactionState const* repState /*= nullptr*/, Player* target /*= nullptr */)
{
    std::string name = factionEntry->name[loc];
    // send faction in "id - [faction] rank reputation [visible] [at war] [own team] [unknown] [invisible] [inactive]" format
    // or              "id - [faction] [no reputation]" format
    std::ostringstream ss;
    if (m_session)
        ss << factionEntry->ID << " - |cffffffff|Hfaction:" << factionEntry->ID << "|h[" << name << " " << localeNames[loc] << "]|h|r";
    else
        ss << factionEntry->ID << " - " << name << " " << localeNames[loc];

    if (repState)                               // and then target!=nullptr also
    {
        ReputationRank rank = target->GetReputationMgr().GetRank(factionEntry);
        std::string rankName = GetMangosString(ReputationRankStrIndex[rank]);

        ss << " " << rankName << "|h|r (" << target->GetReputationMgr().GetReputation(factionEntry) << ")";

        if (repState->Flags & FACTION_FLAG_VISIBLE)
            ss << GetMangosString(LANG_FACTION_VISIBLE);
        if (repState->Flags & FACTION_FLAG_AT_WAR)
            ss << GetMangosString(LANG_FACTION_ATWAR);
        if (repState->Flags & FACTION_FLAG_PEACE_FORCED)
            ss << GetMangosString(LANG_FACTION_PEACE_FORCED);
        if (repState->Flags & FACTION_FLAG_HIDDEN)
            ss << GetMangosString(LANG_FACTION_HIDDEN);
        if (repState->Flags & FACTION_FLAG_INVISIBLE_FORCED)
            ss << GetMangosString(LANG_FACTION_INVISIBLE_FORCED);
        if (repState->Flags & FACTION_FLAG_INACTIVE)
            ss << GetMangosString(LANG_FACTION_INACTIVE);
    }
    else if (target)
        ss << GetMangosString(LANG_FACTION_NOREPUTATION);

    SendSysMessage(ss.str().c_str());
}

bool ChatHandler::HandleLookupFactionCommand(char* args)
{
    if (!*args)
        return false;

    // Can be nullptr at console call
    Player* target = GetSelectedPlayer();

    std::string namepart = args;
    std::wstring wnamepart;

    if (!Utf8toWStr(namepart, wnamepart))
        return false;

    // converting string that we try to find to lower case
    wstrToLower(wnamepart);

    uint32 counter = 0;                                     // Counter for figure out that we found smth.

    for (auto const& itr : sObjectMgr.GetFactionMap())
    {
        FactionEntry const* factionEntry = &itr.second;
        if (factionEntry)
        {
            int loc = GetSessionDbcLocale();
            std::string name = factionEntry->name[loc];
            if (name.empty())
                continue;

            if (!Utf8FitTo(name, wnamepart))
            {
                loc = 0;
                for (; loc < MAX_DBC_LOCALE; ++loc)
                {
                    if (loc == GetSessionDbcLocale())
                        continue;

                    name = factionEntry->name[loc];
                    if (name.empty())
                        continue;

                    if (Utf8FitTo(name, wnamepart))
                        break;
                }
            }

            if (loc < MAX_DBC_LOCALE)
            {
                FactionState const* repState = target ? target->GetReputationMgr().GetState(factionEntry) : nullptr;
                ShowFactionListHelper(factionEntry, LocaleConstant(loc), repState, target);
                counter++;
            }
        }
    }

    if (counter == 0)                                       // if counter == 0 then we found nth
        SendSysMessage(LANG_COMMAND_FACTION_NOTFOUND);
    return true;
}



bool ChatHandler::HandleModifyRepCommand(char* args)
{
    if (!*args)
        return false;

    Player* target = nullptr;
    target = GetSelectedPlayer();

    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(target))
        return false;

    uint32 factionId;
    if (!ExtractUint32KeyFromLink(&args, "Hfaction", factionId))
        return false;

    if (!factionId)
        return false;

    int32 amount = 0;
    if (!ExtractInt32(&args, amount))
    {
        char* rankTxt = ExtractLiteralArg(&args);
        if (!rankTxt)
            return false;

        std::string rankStr = rankTxt;
        std::wstring wrankStr;
        if (!Utf8toWStr(rankStr, wrankStr))
            return false;
        wstrToLower(wrankStr);

        int r = 0;
        amount = -42000;
        for (; r < MAX_REPUTATION_RANK; ++r)
        {
            std::string rank = GetMangosString(ReputationRankStrIndex[r]);
            if (rank.empty())
                continue;

            std::wstring wrank;
            if (!Utf8toWStr(rank, wrank))
                continue;

            wstrToLower(wrank);

            if (wrank.substr(0, wrankStr.size()) == wrankStr)
            {
                int32 delta;
                if (!ExtractOptInt32(&args, delta, 0) || (delta < 0) || (delta > ReputationMgr::PointsInRank[r] - 1))
                {
                    PSendSysMessage(LANG_COMMAND_FACTION_DELTA, (ReputationMgr::PointsInRank[r] - 1));
                    SetSentErrorMessage(true);
                    return false;
                }
                amount += delta;
                break;
            }
            amount += ReputationMgr::PointsInRank[r];
        }
        if (r >= MAX_REPUTATION_RANK)
        {
            PSendSysMessage(LANG_COMMAND_FACTION_INVPARAM, rankTxt);
            SetSentErrorMessage(true);
            return false;
        }
    }

    FactionEntry const* factionEntry = sObjectMgr.GetFactionEntry(factionId);

    if (!factionEntry)
    {
        PSendSysMessage(LANG_COMMAND_FACTION_UNKNOWN, factionId);
        SetSentErrorMessage(true);
        return false;
    }

    if (factionEntry->reputationListID < 0)
    {
        PSendSysMessage(LANG_COMMAND_FACTION_NOREP_ERROR, factionEntry->name[GetSessionDbcLocale()].c_str(), factionId);
        SetSentErrorMessage(true);
        return false;
    }

    target->GetReputationMgr().SetReputation(factionEntry, amount);
    PSendSysMessage(LANG_COMMAND_MODIFY_REP, factionEntry->name[GetSessionDbcLocale()].c_str(), factionId,
        GetNameLink(target).c_str(), target->GetReputationMgr().GetReputation(factionEntry));
    return true;
}

//-----------------------Npc Commands-----------------------
//add spawn of creature
bool ChatHandler::HandleNpcAddCommand(char* args)
{
    if (!*args)
        return false;

    uint32 id;
    if (!ExtractUint32KeyFromLink(&args, "Hcreature_entry", id))
        return false;

    // Forbid manually spawning mobs which break scripts if they are duplicated.
    switch (id)
    {
        case 1842: // Highlord Taelan Fordring
            SendSysMessage("You are not allowed to manually spawn that creature. Talk to a developer.");
            SetSentErrorMessage(true);
            return false;
    }

    CreatureInfo const* cinfo = sObjectMgr.GetCreatureTemplate(id);
    if (!cinfo)
    {
        PSendSysMessage(LANG_COMMAND_INVALIDCREATUREID, id);
        SetSentErrorMessage(true);
        return false;
    }

    Player* chr = m_session->GetPlayer();
    CreatureCreatePos pos(chr, chr->GetOrientation());
    Map* map = chr->GetMap();

    Creature* pCreature = new Creature;

    // used guids from specially reserved range (can be 0 if no free values)
    uint32 lowguid = sObjectMgr.GenerateStaticCreatureLowGuid();
    if (!lowguid)
    {
        SendSysMessage(LANG_NO_FREE_STATIC_GUID_FOR_SPAWN);
        SetSentErrorMessage(true);
        delete pCreature;
        return false;
    }

    if (!pCreature->Create(lowguid, pos, cinfo, id))
    {
        delete pCreature;
        return false;
    }

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    pCreature->SaveToDB(map->GetId());

    uint32 db_guid = pCreature->GetGUIDLow();

    // To call _LoadGoods(); _LoadQuests(); CreateTrainerSpells();
    pCreature->LoadFromDB(db_guid, map);

    map->Add(pCreature);
    sObjectMgr.AddCreatureToGrid(db_guid, sObjectMgr.GetCreatureData(db_guid));
    pCreature->SetRespawnDelay(300);
    return true;
}

bool ChatHandler::HandleNpcSummonCommand(char* args)
{
    //.npc add but temp
    if (!*args)
        return false;

    uint32 id;
    if (!ExtractUint32KeyFromLink(&args, "Hcreature_entry", id))
        return false;

    auto position = GetSession()->GetPlayer()->GetPosition();

    GetSession()->GetPlayer()->SummonCreature(id, position.x, position.y, position.z, position.o);
    return true;
}

//add item in vendorlist
bool ChatHandler::HandleNpcAddVendorItemCommand(char* args)
{
    uint32 itemId;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemId))
    {
        SendSysMessage(LANG_COMMAND_NEEDITEMSEND);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 maxcount;
    if (!ExtractOptUInt32(&args, maxcount, 0))
        return false;

    uint32 incrtime;
    if (!ExtractOptUInt32(&args, incrtime, 0))
        return false;

    uint32 itemflags;
    if (!ExtractOptUInt32(&args, itemflags, 0))
        return false;

    Creature* vendor = GetSelectedCreature();

    uint32 vendor_entry = vendor ? vendor->GetEntry() : 0;

    if (!sObjectMgr.IsVendorItemValid(false, "npc_vendor", vendor_entry, itemId, maxcount, incrtime, 0, m_session->GetPlayer()))
    {
        SetSentErrorMessage(true);
        return false;
    }

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sObjectMgr.AddVendorItem(vendor_entry, itemId, maxcount, incrtime, itemflags);

    ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(itemId);

    PSendSysMessage(LANG_ITEM_ADDED_TO_LIST, itemId, pProto->Name1.c_str(), maxcount, incrtime);
    return true;
}

bool ChatHandler::HandleNpcScaleCommand(char* args)
{
    if (!*args)
        return false;

    float scale = (float)atof(args);

    if (scale > 100.0F || scale <= 0.0F)
    {
        SendSysMessage(LANG_BAD_VALUE);
        SetSentErrorMessage(true);
        return false;
    }

    Creature* npc = GetSelectedCreature();

    if (!npc)
        return false;

    uint32 npc_entry = npc ? npc->GetEntry() : 0;

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature_template` set `scale` = %f where entry = %u", scale, npc_entry);

    npc->SetObjectScale(scale);
    npc->UpdateModelData();

    return true;
}

//del item from vendor list
bool ChatHandler::HandleNpcDelVendorItemCommand(char* args)
{
    if (!*args)
        return false;

    Creature* vendor = GetSelectedCreature();
    if (!vendor || !vendor->IsVendor())
    {
        SendSysMessage(LANG_COMMAND_VENDORSELECTION);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 itemId;
    if (!ExtractUint32KeyFromLink(&args, "Hitem", itemId))
    {
        SendSysMessage(LANG_COMMAND_NEEDITEMSEND);
        SetSentErrorMessage(true);
        return false;
    }

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (!sObjectMgr.RemoveVendorItem(vendor->GetEntry(), itemId))
    {
        PSendSysMessage(LANG_ITEM_NOT_IN_LIST, itemId);
        SetSentErrorMessage(true);
        return false;
    }

    ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(itemId);

    PSendSysMessage(LANG_ITEM_DELETED_FROM_LIST, itemId, pProto->Name1.c_str());
    return true;
}

bool ChatHandler::HandleNpcEvadeCommand(char* /*args*/)
{
    Creature* pTarget = GetSelectedCreature();

    if (!pTarget)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pTarget->OnLeaveCombat();

    return true;
}

//show info about AI
bool ChatHandler::HandleNpcAIInfoCommand(char* /*args*/)
{
    Creature* pTarget = GetSelectedCreature();

    if (!pTarget || !pTarget->AI())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_NPC_AI_HEADER, pTarget->GetEntry());

    std::string strScript = pTarget->GetScriptName();
    std::string strAI = pTarget->GetAIName();
    char const* cstrAIClass = pTarget->AI() ? typeid(*pTarget->AI()).name() : " - ";

    PSendSysMessage(LANG_NPC_AI_NAMES,
        strAI.empty() ? " - " : strAI.c_str(),
        cstrAIClass ? cstrAIClass : " - ",
        strScript.empty() ? " - " : strScript.c_str());
    PSendSysMessage(LANG_NPC_AI_MOVE, GetOnOffStr(pTarget->AI()->IsCombatMovementEnabled()));
    PSendSysMessage(LANG_NPC_AI_ATTACK, GetOnOffStr(pTarget->AI()->IsMeleeAttackEnabled()));
    MovementGeneratorType moveType = pTarget->GetMotionMaster()->GetCurrentMovementGeneratorType();
    PSendSysMessage(LANG_NPC_MOTION_TYPE, MotionMaster::GetMovementGeneratorTypeName(moveType), moveType);
    if (!pTarget->movespline->Finalized())
        PSendSysMessage("Spline Origin: %s", pTarget->movespline->GetMovementOrigin());
    if (pTarget->IsTemporarySummon())
    {
        TempSummonType despawnType = static_cast<TemporarySummon*>(pTarget)->GetDespawnType();
        PSendSysMessage("Despawn Type: %s (%u)", TempSummonTypeToString(despawnType), despawnType);
    }
     if (CreatureGroup* pGroup = pTarget->GetCreatureGroup())
         PSendSysMessage("Group Leader: %s", pGroup->GetLeaderGuid().GetString().c_str());
    pTarget->AI()->GetAIInformation(*this);

    return true;
}

//set npc_flags of creature
bool ChatHandler::HandleNpcFlagCommand(char* args)
{
    if (!*args)
        return false;

    uint32 npcFlags = (uint32)atoi(args);

    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->SetUInt32Value(UNIT_NPC_FLAGS, npcFlags);

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE creature_template SET npc_flags = '%u' WHERE entry = '%u'", npcFlags, pCreature->GetEntry());

    SendSysMessage(LANG_VALUE_SAVED_REJOIN);

    return true;
}

bool ChatHandler::HandleNpcFollowCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    Creature* creature = GetSelectedCreature();

    if (!creature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // Follow player - Using pet's default dist and angle
    creature->GetMotionMaster()->MoveFollow(player, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);

    PSendSysMessage(LANG_CREATURE_FOLLOW_YOU_NOW, creature->GetName());
    return true;
}

bool ChatHandler::HandleNpcUnfollowCommand(char* /*args*/)
{
    Player* pPlayer = m_session->GetPlayer();
    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    MotionMaster* creatureMotion = pCreature->GetMotionMaster();
    if (creatureMotion->empty() ||
        creatureMotion->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
    {
        PSendSysMessage(LANG_CREATURE_NOT_FOLLOW_YOU, pCreature->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    FollowMovementGenerator<Creature> const* mgen
        = static_cast<FollowMovementGenerator<Creature> const*>((creatureMotion->top()));

    if (mgen->GetTarget() != pPlayer)
    {
        PSendSysMessage(LANG_CREATURE_NOT_FOLLOW_YOU, pCreature->GetName());
        SetSentErrorMessage(true);
        return false;
    }

    // reset movement
    creatureMotion->MovementExpired(true);

    PSendSysMessage(LANG_CREATURE_NOT_FOLLOW_YOU_NOW, pCreature->GetName());
    return true;
}

bool ChatHandler::HandleNpcDeleteCommand(char* args)
{
    Creature* unit = nullptr;

    if (*args)
    {
        // number or [name] Shift-click form |color|Hcreature:creature_guid|h[name]|h|r
        uint32 lowguid;
        if (!ExtractUint32KeyFromLink(&args, "Hcreature", lowguid))
            return false;

        if (!lowguid)
            return false;

        if (CreatureData const* data = sObjectMgr.GetCreatureData(lowguid))
            unit = m_session->GetPlayer()->GetMap()->GetCreature(data->GetObjectGuid(lowguid));
    }
    else
        unit = GetSelectedCreature();

    if (!unit)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // Forbid manually spawning mobs which break scripts if they are duplicated.
    switch (unit->GetEntry())
    {
        case 1842: // Highlord Taelan Fordring
            SendSysMessage("You are not allowed to manually delete that creature. Talk to a developer.");
            SetSentErrorMessage(true);
            return false;
    }

    switch (unit->GetSubtype())
    {
        case CREATURE_SUBTYPE_GENERIC:
        {
            unit->CombatStop();
            uint32 lowguid = unit->GetGUIDLow();
            if (CreatureData const* data = sObjectMgr.GetCreatureData(lowguid))
            {
                Creature::AddToRemoveListInMaps(lowguid, data);
                sWorld.GetMigration().SetAuthor(m_session->GetUsername());
                Creature::DeleteFromDB(lowguid, data);
            }
            else
                unit->AddObjectToRemoveList();
            break;
        }
        case CREATURE_SUBTYPE_PET:
            ((Pet*)unit)->Unsummon(PET_SAVE_AS_CURRENT);
            break;
        case CREATURE_SUBTYPE_TOTEM:
            ((Totem*)unit)->UnSummon();
            break;
        case CREATURE_SUBTYPE_TEMPORARY_SUMMON:
            ((TemporarySummon*)unit)->UnSummon();
            break;
        default:
            return false;
    }

    SendSysMessage(LANG_COMMAND_DELCREATMESSAGE);

    return true;
}

//move selected creature
bool ChatHandler::HandleNpcSpawnMoveCommand(char* args)
{
    return HandleNpcMoveHelperCommand(args, true);
}

bool ChatHandler::HandleNpcMoveCommand(char* args)
{
    return HandleNpcMoveHelperCommand(args, false);
}

bool ChatHandler::HandleNpcMoveHelperCommand(char* args, bool save)
{
    uint32 lowguid = 0;
    Player* pPlayer = m_session->GetPlayer();
    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        // number or [name] Shift-click form |color|Hcreature:creature_guid|h[name]|h|r
        if (!ExtractUint32KeyFromLink(&args, "Hcreature", lowguid))
            return false;

        CreatureData const* data = sObjectMgr.GetCreatureData(lowguid);
        if (!data)
        {
            PSendSysMessage(LANG_COMMAND_CREATGUIDNOTFOUND, lowguid);
            SetSentErrorMessage(true);
            return false;
        }

        if (pPlayer->GetMapId() != data->position.mapId)
        {
            PSendSysMessage(LANG_COMMAND_CREATUREATSAMEMAP, lowguid);
            SetSentErrorMessage(true);
            return false;
        }

        pCreature = pPlayer->GetMap()->GetCreature(data->GetObjectGuid(lowguid));
    }
    else
        lowguid = pCreature->GetGUIDLow();

    float x = pPlayer->GetPositionX();
    float y = pPlayer->GetPositionY();
    float z = pPlayer->GetPositionZ();
    float o = pPlayer->GetOrientation();

    if (pCreature)
    {
        pCreature->SetHomePosition(x, y, z, o);
        if (pCreature->IsAlive())                           // dead creature will reset movement generator at respawn
        {
            pCreature->SetDeathState(JUST_DIED);
            pCreature->Respawn();
        }
    }

    if (save)
    {
        if (CreatureData* pData = const_cast<CreatureData*>(sObjectMgr.GetCreatureData(lowguid)))
        {
            pData->position.x = x;
            pData->position.y = y;
            pData->position.z = z;
            pData->position.o = o;
        }
        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        sWorld.ExecuteUpdate("UPDATE `creature` SET `position_x` = %f, `position_y` = %f, `position_z` = %f, `orientation` = %f WHERE `guid` = %u", x, y, z, o, lowguid);
        SendSysMessage(LANG_COMMAND_CREATUREMOVED);
    }
    else
        SendSysMessage("Creature moved without saving.");
    
    return true;
}

bool ChatHandler::HandleNpcSpeedCommand(char* args)
{
    if (!*args)
        return false;

    float mod = (uint32)atof(args);
    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->UpdateSpeed(MOVE_WALK, false, mod);
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE creature_template SET speed_walk = %f WHERE entry = %u", mod, pCreature->GetEntry());
    return true;
}

//set faction of creature
bool ChatHandler::HandleNpcFactionIdCommand(char* args)
{
    if (!*args)
        return false;

    uint32 factionId = (uint32)atoi(args);

    if (!sObjectMgr.GetFactionTemplateEntry(factionId))
    {
        PSendSysMessage(LANG_WRONG_FACTION, factionId);
        SetSentErrorMessage(true);
        return false;
    }

    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->SetFactionTemplateId(factionId);

    // faction is set in creature_template - not inside creature

    // update in memory
    if (CreatureInfo const* cinfo = pCreature->GetCreatureInfo())
    {
        const_cast<CreatureInfo*>(cinfo)->faction = factionId;
    }

    // and DB
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE creature_template SET faction = '%u' WHERE entry = '%u'", factionId, pCreature->GetEntry());

    return true;
}

//npc deathstate handling
bool ChatHandler::HandleNpcSpawnSetDeathStateCommand(char* args)
{
    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature || !pCreature->HasStaticDBSpawnData())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 const guidLow = pCreature->GetGUIDLow();
    CreatureData* pData = const_cast<CreatureData*>(sObjectMgr.GetCreatureData(guidLow));
    if (value)
        pData->spawn_flags |= SPAWN_FLAG_DEAD;
    else
        pData->spawn_flags &= ~SPAWN_FLAG_DEAD;

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    pCreature->SetDeadByDefault(value);
    pCreature->SaveToDB();
    pCreature->Respawn();

    return true;
}

bool ChatHandler::HandleNpcNearCommand(char* args)
{
    float distance;
    if (!ExtractOptFloat(&args, distance, 10.0f))
        return false;

    uint32 count = 0;

    Player* pl = m_session->GetPlayer();

    MaNGOS::AllCreaturesInRange check(pl, distance);
    std::list<Creature*> creatures;
    MaNGOS::CreatureListSearcher<MaNGOS::AllCreaturesInRange> searcher(creatures, check);

    Cell::VisitGridObjects(pl, searcher, distance);

    for (const auto& creature : creatures)
    {
        PSendSysMessage("Creature: GuidLow %u, %s, entry %u, name %u, pos: %f, %f, %f, mapid: %u", creature->GetGUIDLow(), PrepareStringNpcOrGoSpawnInformation<Creature>(creature->GetGUIDLow()).c_str(),
            creature->GetEntry(), creature->GetName(), creature->GetPositionX(), creature->GetPositionY(), creature->GetPositionZ(), creature->GetMapId());
        ++count;
    }

    PSendSysMessage(LANG_COMMAND_NEAROBJMESSAGE, distance, count);
    return true;

    return true;
}

bool ChatHandler::HandleGUIDCommand(char* /*args*/)
{
    ObjectGuid guid = m_session->GetPlayer()->GetSelectionGuid();

    if (!guid)
    {
        SendSysMessage(LANG_NO_SELECTION);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_OBJECT_GUID, guid.GetString().c_str());
    return true;
}

//move item to other slot
bool ChatHandler::HandleItemMoveCommand(char* args)
{
    if (!*args)
        return false;
    uint8 srcslot, dstslot;

    char* pParam1 = strtok(args, " ");
    if (!pParam1)
        return false;

    char* pParam2 = strtok(nullptr, " ");
    if (!pParam2)
        return false;

    srcslot = (uint8)atoi(pParam1);
    dstslot = (uint8)atoi(pParam2);

    if (srcslot == dstslot)
        return true;

    Player* pPlayer = m_session->GetPlayer();
    if (!pPlayer->IsValidPos(INVENTORY_SLOT_BAG_0, srcslot, true))
        return false;

    // can be autostore pos
    if (!pPlayer->IsValidPos(INVENTORY_SLOT_BAG_0, dstslot, false))
        return false;

    uint16 src = ((INVENTORY_SLOT_BAG_0 << 8) | srcslot);
    uint16 dst = ((INVENTORY_SLOT_BAG_0 << 8) | dstslot);

    pPlayer->SwapItem(src, dst);

    return true;
}

//demorph player or unit
bool ChatHandler::HandleDeMorphCommand(char* /*args*/)
{
    Unit* target = GetSelectedUnit();
    if (!target)
        target = m_session->GetPlayer();


    // check online security
    else if (target->GetTypeId() == TYPEID_PLAYER && HasLowerSecurity((Player*)target))
        return false;

    target->DeMorph();

    return true;
}

#define WEEB_MORPH_1 19000
#define WEEB_MORPH_2 19023
#define WEEB_MORPH_3 19032

//morph creature or player
bool ChatHandler::HandleModifyMorphCommand(char* args)
{
    if (!*args)
        return false;

    uint32 display_id = (uint32)atoi(args);
    Unit* target = GetSelectedUnit();
    if (!target)
        target = m_session->GetPlayer();

    // check online security
    else if (target->GetTypeId() == TYPEID_PLAYER && HasLowerSecurity((Player*)target))
        return false;

    // Different actions for specific display_ids
    switch (display_id)
    {
    case WEEB_MORPH_1:
    case WEEB_MORPH_2:
    case WEEB_MORPH_3:
        // Prevent turning players or creatures into weird weeb stuff
        if (target->GetTypeId() != TYPEID_PLAYER || ((Player*)target)->GetSession()->GetSecurity() == SEC_PLAYER)
            return false;
    default:
        target->SetDisplayId(display_id);
    }

    return true;
}

bool ChatHandler::HandleGetSkillValueCommand(char* args)
{
    if (!*args) return false;

    uint32 skill_id = (uint32)atoi(args);
    uint32 skill_value = 0;

    Player* target = GetSelectedPlayer();
    if (!target) target = m_session->GetPlayer();

    skill_value = target->GetSkillValue(skill_id);
    PSendSysMessage("Skill %u Value %u", skill_id, skill_value);
    return true;
}

bool ChatHandler::HandleHCMessagesCommand(char* args)
{
    if (!*args)
    {
        SendSysMessage("Syntax: .hcmessages <minlevel>");
        return false;
    }

    
    int32 minLevel = -1;

    ExtractInt32(&args, minLevel);

    if (minLevel > 100 || minLevel <= 0)
    {
        SendSysMessage("Enter a valid level.");
        return false;
    }

    minLevel = floor(float(minLevel) / 10.f) * 10;

    if (minLevel == 0)
        minLevel = 1;

    Player* player = GetSession()->GetPlayer();

    if (player)
    {
        PSendSysMessage("Minimum level for Hardcore messages is now: %d", minLevel);
        player->SetPlayerVariable(PlayerVariables::HardcoreMessageLevel, std::to_string(minLevel));
    }
    return true;
}

bool ChatHandler::HandleMinChatLevelCommand(char* args)
{
    if (!*args)
    {
        SendSysMessage("Syntax: .minchatlevel <minlevel>");
        return false;
    }


    int32 minLevel = -1;

    ExtractInt32(&args, minLevel);

    if (minLevel > 100 || minLevel <= 0)
    {
        SendSysMessage("Enter a valid level.");
        return false;
    }

    sWorld.SetMinChatLevel((uint32)minLevel);
    return true;
}

bool ChatHandler::HandleCrashCommand(char* args)
{
    int* torta = (int*)0x42;
    *torta = 9001;
    return true;
}


//removes the item id from player from "discovered usable" transmog items in collection
//syntax : <itemId> playername (or select player and no 2nd arg)
bool ChatHandler::HandleDeleteTransmogCollectionCommand(char* args)
{
    uint32 itemId;
    if (!ExtractUInt32(&args, itemId))
        return false;


    Player* player;
    ObjectGuid playerGuid;
    if (!ExtractPlayerTarget(&args, &player, &playerGuid))
        return false;


    if (player)
        player->GetTransmogMgr()->RemoveFromCollection(itemId);

    CharacterDatabase.PExecute("DELETE FROM character_transmogs WHERE guid = %u AND itemId = %u", playerGuid.GetCounter(), itemId);

    PSendSysMessage("Deleted item %u from player's collection.", itemId);
    return true;
}

bool ChatHandler::ForceJoinChannelCommand(char* args)
{
    if (!*args)
        return false;

    Player* chr = GetSelectedPlayer();
    if (!chr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    char* channelnamestr = ExtractLiteralArg(&args);
    if (!channelnamestr)
    {
        return false;
    }

    std::string channelName = channelnamestr;

    if (channelName.empty())
        return false;

    ChannelMgr* cMgr = channelMgr(chr->GetTeam());

    if (cMgr)
    {
        if (Channel* chn = cMgr->GetOrCreateChannel(channelName))
            chn->Join(chr->GetObjectGuid(), "", false);
    }
    return true;
}

bool ChatHandler::HandleMarkSuspiciousCommand(char* args)
{
    if (!*args)
        return false;

    Player* chr = GetSelectedPlayer();
    if (!chr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    

    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    if (value)
        chr->GetSession()->MarkSuspicious();
    else
        chr->GetSession()->UnmarkSuspicious();

    PSendSysMessage("Player is now %s", chr->GetSession()->IsSuspicious() ? "SUSPICIOUS" : "NOT SUSPICIOUS");

    return true;
}

bool ChatHandler::HandleWhoCommand(char* args)
{
    uint32 areaId;
    AreaEntry const* pAreaEntry = nullptr;
    std::string areaName = args;

    if (!areaName.empty())
    {
        if (!(pAreaEntry = sObjectMgr.GetAreaEntryByName(areaName)))
        {
            SendSysMessage("Area not found.");
            SetSentErrorMessage(true);
            return false;
        }

        areaId = pAreaEntry->Id;
        PSendSysMessage("Players in %s:", pAreaEntry->Name);
    }
    else
    {
        areaId = 0;
        SendSysMessage("Top 100 longest online players:");
    }
        
    time_t now = sWorld.GetGameTime();
    std::multimap<time_t, Player*> playersByOnlineTime;
    HashMapHolder<Player>::MapType& m = sObjectAccessor.GetPlayers();
    for (const auto& itr : m)
    {
        if (!itr.second->IsInWorld())
            continue;

        if (areaId && areaId != itr.second->GetCachedAreaId() && areaId != itr.second->GetCachedZoneId())
            continue;

        if (itr.second->GetSession()->GetSecurity() > SEC_PLAYER)
            continue;

        playersByOnlineTime.insert({ itr.second->GetLoginTime(), itr.second });

        if (playersByOnlineTime.size() >= 100)
            break;
    }

    if (playersByOnlineTime.empty())
        SendSysMessage("- No players found.");
    else
    {
        for (auto const& itr : playersByOnlineTime)
        {
            PSendSysMessage("- %s - %u - %s - %s - Online for %s",
                GetNameLink(itr.second).c_str(),
                itr.second->GetLevel(),
                itr.second->GetSession()->GetUsername().c_str(),
                itr.second->GetSession()->GetEmail().c_str(),
                secsToTimeString(now - itr.first, true).c_str());
        }
    }

    return true;
}

bool ChatHandler::HandlePvPCommand(char* args)
{
    if (!*args)
        return false;

    Player* chr = GetSelectedPlayer();
    if (!chr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    chr->UpdatePvP(value, true);
    PSendSysMessage("PvP of character %s set to %s.", chr->GetName(), value ? "on" : "off");
    return true;
}

//Edit Player money
bool ChatHandler::HandleModifyMoneyCommand(char* args)
{
    if (!*args)
        return false;

    Player* chr = GetSelectedPlayer();
    if (chr == nullptr)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(chr))
        return false;

    int32 addmoney = strtol(args, nullptr, 10);
    if (addmoney == LONG_MIN || addmoney == LONG_MAX)
        return false;

    uint32 moneyuser = chr->GetMoney();

    if (addmoney < 0)
    {
        int32 newmoney = int32(moneyuser) + addmoney;

        DETAIL_LOG(GetMangosString(LANG_CURRENT_MONEY), moneyuser, addmoney, newmoney);
        if (newmoney <= 0)
        {
            PSendSysMessage(LANG_YOU_TAKE_ALL_MONEY, GetNameLink(chr).c_str());
            if (needReportToTarget(chr))
                ChatHandler(chr).PSendSysMessage(LANG_YOURS_ALL_MONEY_GONE, GetNameLink().c_str());

            chr->SetMoney(0);
        }
        else
        {
            if (newmoney > MAX_MONEY_AMOUNT)
                newmoney = MAX_MONEY_AMOUNT;

            PSendSysMessage(LANG_YOU_TAKE_MONEY, abs(addmoney), GetNameLink(chr).c_str());
            if (needReportToTarget(chr))
                ChatHandler(chr).PSendSysMessage(LANG_YOURS_MONEY_TAKEN, GetNameLink().c_str(), abs(addmoney));
            chr->SetMoney(newmoney);
        }
    }
    else
    {
        PSendSysMessage(LANG_YOU_GIVE_MONEY, addmoney, GetNameLink(chr).c_str());
        if (needReportToTarget(chr))
            ChatHandler(chr).PSendSysMessage(LANG_YOURS_MONEY_GIVEN, GetNameLink().c_str(), addmoney);

        if (addmoney >= MAX_MONEY_AMOUNT)
            chr->SetMoney(MAX_MONEY_AMOUNT);
        else
            chr->LogModifyMoney(addmoney, "GM", m_session->GetPlayer()->GetObjectGuid());
    }

    DETAIL_LOG(GetMangosString(LANG_NEW_MONEY), moneyuser, addmoney, chr->GetMoney());

    return true;
}

std::string MakeTimeString(time_t timestamp)
{
    auto tm = *std::localtime(&timestamp);
    
    std::ostringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

constexpr const char* ItemLogActionToString(LogItemAction action)
{
    switch (action)
    {
    case LogItemAction::Auctioned:
        return "Auctioned";
    case LogItemAction::Deleted:
        return "Deleted";
    case LogItemAction::Disenchanted:
        return "Disenchanted";
    case LogItemAction::Looted:
        return "Looted";
    case LogItemAction::Mailed:
        return "Mailed";
    case LogItemAction::MailReceived:
        return "Received in Mail";
    case LogItemAction::Sold:
        return "Sold";
    case LogItemAction::Traded:
        return "Traded";
    case LogItemAction::TradeReceived:
        return "Received from Trade";
    default:
        return "<>";
    }
}

bool ChatHandler::HandleItemLogCommand(char* args)
{
    constexpr uint32 limit = 100;

    Player* target;
    ObjectGuid playerGuid;
    if (!ExtractPlayerTarget(&args, &target, &playerGuid))
        return false;

    int32 itemId;
    if (!ExtractInt32(&args, itemId))
    {
        SendSysMessage("Supply item Id.");
        return false;
    }

    if (itemId < 0)
        itemId = 0;

    PSendSysMessage("List of item logs for item ID %i : ", itemId);

    if (target)
    {
        const auto& logs = target->GetItemLogs();
        const auto& entryLogs = logs.find((uint32)itemId);
        if (entryLogs != logs.end())
        {
            auto beginItr = entryLogs->second.begin();
            auto itr = entryLogs->second.size() >= limit ? beginItr + limit : --entryLogs->second.end();

            //now iterate backwards..
            for (; itr != beginItr; --itr)
            {
                PSendSysMessage("%s: Item entry %u, count %u, guid %u %s.", MakeTimeString(itr->timestamp).c_str(), itr->entry, itr->count, itr->guidLow, ItemLogActionToString(itr->action));
            }

            //one extra for beginitr
            PSendSysMessage("%s: Item entry %u, count %u, guid %u %s.", MakeTimeString(itr->timestamp).c_str(), itr->entry, itr->count, itr->guidLow, ItemLogActionToString(itr->action));
        }

    }
    else
    {
        //target is offline, use playerGuid
        std::unique_ptr<QueryResult> result = std::unique_ptr<QueryResult>{
            CharacterDatabase.PQuery("SELECT itemLowGuid, itemEntry, itemCount, action, timestamp FROM character_item_logs WHERE playerLowGuid = '%u' AND itemEntry = '%u' ORDER BY timestamp DESC LIMIT %u", playerGuid.GetCounter(), (uint32)itemId, limit)
        };

        std::vector<LogItemInfo> logs;

        if (result)
            do {
                Field* fields = result->Fetch();
                uint32 itemLowGuid = fields[0].GetUInt32();
                uint32 itemEntry = fields[1].GetUInt32();
                uint32 count = fields[2].GetUInt32();
                uint32 action = fields[3].GetUInt32();
                uint64 timestamp = fields[4].GetUInt64();

                logs.push_back({ itemLowGuid, itemEntry, timestamp, count, static_cast<LogItemAction>(action) });
            } while (result->NextRow());


            for (auto itr = logs.rbegin(); itr != logs.rend(); ++itr)
            {
                PSendSysMessage("%s: Item entry %u, count %u, guid %u |cff1c9c27%s|r.", MakeTimeString(itr->timestamp).c_str(), itr->entry, itr->count, itr->guidLow, ItemLogActionToString(itr->action));
            }
    }

    return true;
}

bool ChatHandler::HandleModifyHonorCommand(char* args)
{
    if (!*args)
        return false;

    Player* target = GetSelectedPlayer();
    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    char* field = ExtractLiteralArg(&args);
    if (!field)
        return false;

    int32 amount;
    if (!ExtractInt32(&args, amount))
        return false;

    // hack code
    if (hasStringAbbr(field, "currency"))
    {
        target->GetHonorMgr().ModifySpendableHonor(amount);
        PSendSysMessage("Honor currency of %s is now %u.", target->GetName(), target->GetHonorMgr().GetSpendableHonor());
        return true;
    }
    else if (hasStringAbbr(field, "points"))
    {
        if (amount < 0 || amount > 255)
            return false;
        // rank points is sent to client with same size of uint8(255) for each rank
        target->SetByteValue(PLAYER_FIELD_BYTES2, 0, amount);
    }
    else if (hasStringAbbr(field, "rank"))
    {
        if (amount < 0 || amount >= 20)
            return false;
        target->SetByteValue(PLAYER_BYTES_3, 3, amount);
    }
    else if (hasStringAbbr(field, "todaykills"))
        target->SetUInt16Value(PLAYER_FIELD_SESSION_KILLS, 0, (uint32)amount);
    else if (hasStringAbbr(field, "yesterdaykills"))
        target->SetUInt32Value(PLAYER_FIELD_YESTERDAY_KILLS, (uint32)amount);
    else if (hasStringAbbr(field, "yesterdayhonor"))
        target->SetUInt32Value(PLAYER_FIELD_YESTERDAY_CONTRIBUTION, (uint32)amount);
    else if (hasStringAbbr(field, "thisweekkills"))
        target->SetUInt32Value(PLAYER_FIELD_THIS_WEEK_KILLS, (uint32)amount);
    else if (hasStringAbbr(field, "thisweekhonor"))
        target->SetUInt32Value(PLAYER_FIELD_THIS_WEEK_CONTRIBUTION, (uint32)amount);
    else if (hasStringAbbr(field, "lastweekkills"))
        target->SetUInt32Value(PLAYER_FIELD_LAST_WEEK_KILLS, (uint32)amount);
    else if (hasStringAbbr(field, "lastweekhonor"))
        target->SetUInt32Value(PLAYER_FIELD_LAST_WEEK_CONTRIBUTION, (uint32)amount);
    else if (hasStringAbbr(field, "lastweekstanding"))
        target->SetUInt32Value(PLAYER_FIELD_LAST_WEEK_RANK, (uint32)amount);
    else if (hasStringAbbr(field, "lifetimedishonorablekills"))
        target->SetUInt32Value(PLAYER_FIELD_LIFETIME_DISHONORABLE_KILLS, (uint32)amount);
    else if (hasStringAbbr(field, "lifetimehonorablekills"))
        target->SetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS, (uint32)amount);

    PSendSysMessage(LANG_COMMAND_MODIFY_HONOR, field, target->GetName(), hasStringAbbr(field, "rank") ? amount : (uint32)amount);

    return true;
}

//kick player
bool ChatHandler::HandleKickPlayerCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    std::string extraArgs = "";
    if (*args)
        extraArgs = args;

    if (m_session && target == m_session->GetPlayer())
    {
        SendSysMessage(LANG_COMMAND_KICKSELF);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(target))
        return false;

    if (target->GetGUID() == 1)
    {
        SendSysMessage("Nice try, fish.");
        return false;
    }

    bool force = extraArgs.find("force") != std::string::npos;

    // [Hardcore] Prevent death caused by getting disconnected while in-fight
    if (target->IsHardcore() && !force)
    {
        SendSysMessage("Cannot kick a Hardcore Character.");
        SetSentErrorMessage(true);
        return false;
    }

    // send before target pointer invalidate
    PSendSysMessage(LANG_COMMAND_KICKMESSAGE, GetNameLink(target).c_str());
    WorldSession* targetSession = target->GetSession();
    if (targetSession->IsHeadless())
    {
        sWorld.StopHeadlessSession(target->GetObjectGuid(), true);
        return true;
    }

    // First kick: close socket but keep player online
    if (targetSession->IsConnected())
        targetSession->KickPlayer();
    else
        targetSession->KickDisconnectedFromWorld();

    return true;
}

bool ChatHandler::HandleGroupInfoCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &target_guid, &target_name))
        return false;

    if (!target)
    {
        PSendSysMessage(LANG_NO_CHAR_SELECTED);
        return false;
    }

    auto group = target->GetGroup();

    if (!group)
    {
        std::string nameLink = GetNameLink(target);
        PSendSysMessage(LANG_NOT_IN_GROUP, nameLink.c_str());
        return false;
    }

    std::vector<std::string> names;

    for (GroupReference* itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
    {
        if (Player* player = itr->getSource())
        {
            names.emplace_back(player->GetName());
        }
    }

    std::stringstream stream;

    for (std::size_t i = 0, j = names.size(); i != j; ++i)
    {
        stream << names[i];

        if (i + 1 != j)
        {
            stream << ", ";
        }
    }

    PSendSysMessage(LANG_GROUP_INFO, (group->isRaidGroup() ? "Raid" : "Party"),
        playerLink(std::to_string(group->GetId())).c_str(), playerLink(group->GetLeaderName()).c_str(),
        group->GetMembersCount(), stream.str().c_str());
    return true;
}

//show info of player
bool ChatHandler::HandlePInfoCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &target_guid, &target_name, true))
        return false;

    if (HasLowerSecurity(target, target ? ObjectGuid() : target_guid))
        return false;

    PInfoHandler::HandlePInfoCommand(m_session, target, target_guid, target_name);

    return true;
}
/// Helper function
inline Creature* Helper_CreateWaypointFor(Creature* wpOwner, WaypointPathOrigin wpOrigin, int32 pathId, uint32 wpId, WaypointNode const* wpNode, CreatureInfo const* waypointInfo)
{
    TemporarySummonWaypoint* wpCreature = new TemporarySummonWaypoint(wpOwner->GetObjectGuid(), wpId+1, pathId, (uint32)wpOrigin);

    CreatureCreatePos pos(wpOwner->GetMap(), wpNode->x, wpNode->y, wpNode->z, wpNode->orientation != 100.0f ? wpNode->orientation : 0.0f);

    if (!wpCreature->Create(wpOwner->GetMap()->GenerateLocalLowGuid(HIGHGUID_UNIT), pos, waypointInfo, waypointInfo->entry))
    {
        delete wpCreature;
        return nullptr;
    }

    wpCreature->SetVisibility(VISIBILITY_OFF);
    wpCreature->SetSummonPoint(pos);
    wpCreature->SetUInt32Value(UNIT_FIELD_LEVEL, wpId + 1);
    wpCreature->SetActiveObjectState(true);
    wpCreature->AddUnitMovementFlag(MOVEFLAG_FIXED_Z);

    wpCreature->Summon(TEMPSUMMON_TIMED_DESPAWN, 5 * MINUTE * IN_MILLISECONDS); // Also initializes the AI and MMGen
    return wpCreature;
}
inline void UnsummonVisualWaypoints(Player const* player, ObjectGuid ownerGuid)
{
    std::list<Creature*> waypoints;
    MaNGOS::AllCreaturesOfEntryInRange checkerForWaypoint(player, VISUAL_WAYPOINT, SIZE_OF_GRIDS);
    MaNGOS::CreatureListSearcher<MaNGOS::AllCreaturesOfEntryInRange> searcher(waypoints, checkerForWaypoint);
    Cell::VisitGridObjects(player, searcher, SIZE_OF_GRIDS);

    for (const auto& waypoint : waypoints)
    {
        if (waypoint->GetSubtype() != CREATURE_SUBTYPE_TEMPORARY_SUMMON)
            continue;

        TemporarySummonWaypoint* wpTarget = dynamic_cast<TemporarySummonWaypoint*>(waypoint);
        if (!wpTarget)
            continue;

        if (wpTarget->GetSummonerGuid() == ownerGuid)
            wpTarget->UnSummon();
    }
}

/** Add a waypoint to a creature
 * .wp add [dbGuid] [pathId] [source]
 *
 * The user can either select an npc or provide its dbGuid.
 * Also the user can specify pathId and source if wanted.
 *
 * The user can even select a visual waypoint - then the new waypoint
 * is placed *after* the selected one - this makes insertion of new
 * waypoints possible.
 *
 * .wp add [pathId] [source]
 * -> adds a waypoint to the currently selected creature, to path pathId in source-storage
 *
 * .wp add guid [pathId] [source]
 * -> if no npc is selected, expect the creature provided with guid argument
 *
 * @return true - command did succeed, false - something went wrong
 */
bool ChatHandler::HandleWpAddCommand(char* args)
{
    DEBUG_LOG("DEBUG: HandleWpAddCommand");

    CreatureInfo const* waypointInfo = sObjectMgr.GetCreatureTemplate(VISUAL_WAYPOINT);
    if (!waypointInfo || waypointInfo->GetHighGuid() != HIGHGUID_UNIT)
        return false;                                       // must exist as normal creature in mangos.sql 'creature_template'

    Creature* targetCreature = GetSelectedCreature();
    WaypointPathOrigin wpDestination = PATH_NO_PATH;        ///< into which storage
    int32 wpPathId = 0;                                     ///< along which path
    uint32 wpPointId = 0;                                   ///< pointId if a waypoint was selected, in this case insert after
    Creature* wpOwner;

    if (targetCreature)
    {
        // Check if the user did specify a visual waypoint
        if (targetCreature->GetEntry() == VISUAL_WAYPOINT && targetCreature->GetSubtype() == CREATURE_SUBTYPE_TEMPORARY_SUMMON)
        {
            TemporarySummonWaypoint* wpTarget = dynamic_cast<TemporarySummonWaypoint*>(targetCreature);
            if (!wpTarget)
            {
                PSendSysMessage(LANG_WAYPOINT_VP_SELECT);
                SetSentErrorMessage(true);
                return false;
            }

            // Who moves along this waypoint?
            wpOwner = targetCreature->GetMap()->GetAnyTypeCreature(wpTarget->GetSummonerGuid());
            if (!wpOwner)
            {
                PSendSysMessage(LANG_WAYPOINT_NOTFOUND_NPC, wpTarget->GetSummonerGuid().GetString().c_str());
                SetSentErrorMessage(true);
                return false;
            }
            wpDestination = (WaypointPathOrigin)wpTarget->GetPathOrigin();
            wpPathId = wpTarget->GetPathId();
            wpPointId = wpTarget->GetWaypointId() + 1;      // Insert as next waypoint
        }
        else // normal creature selected
            wpOwner = targetCreature;
    }
    else //!targetCreature - first argument must be dbGuid
    {
        uint32 dbGuid;
        if (!ExtractUInt32(&args, dbGuid))
        {
            PSendSysMessage(LANG_WAYPOINT_NOGUID);
            SetSentErrorMessage(true);
            return false;
        }

        CreatureData const* data = sObjectMgr.GetCreatureData(dbGuid);
        if (!data)
        {
            PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }

        if (m_session->GetPlayer()->GetMapId() != data->position.mapId)
        {
            PSendSysMessage(LANG_COMMAND_CREATUREATSAMEMAP, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }

        wpOwner = m_session->GetPlayer()->GetMap()->GetAnyTypeCreature(data->GetObjectGuid(dbGuid));
        if (!wpOwner)
        {
            PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if (wpDestination == PATH_NO_PATH)                      // No Waypoint selected, parse additional params
    {
        if (ExtractOptInt32(&args, wpPathId, 0))            // Fill path-id and source
        {
            uint32 src = (uint32)PATH_NO_PATH;
            if (ExtractOptUInt32(&args, src, src))
                wpDestination = (WaypointPathOrigin)src;
            else // pathId provided but no destination
            {
                if (wpPathId != 0)
                    wpDestination = PATH_FROM_ENTRY;        // Multiple Paths must only be assigned by entry
            }
        }

        if (wpDestination == PATH_NO_PATH)                  // No overwrite params. Do best estimate
        {
            if (wpOwner->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                if (WaypointMovementGenerator<Creature> const* wpMMGen = dynamic_cast<WaypointMovementGenerator<Creature> const*>(wpOwner->GetMotionMaster()->GetCurrent()))
                    wpMMGen->GetPathInformation(wpDestination);

            // Get information about default path if no current path. If no default path, prepare data dependendy on uniqueness
            if (wpDestination == PATH_NO_PATH && !sWaypointMgr.GetDefaultPath(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), &wpDestination))
            {
                wpDestination = PATH_FROM_ENTRY;                // Default place to store paths
                if (wpOwner->HasStaticDBSpawnData())
                {
                    QueryResult* result = WorldDatabase.PQuery("SELECT COUNT(id) FROM creature WHERE id = %u", wpOwner->GetEntry());
                    if (result && result->Fetch()[0].GetUInt32() != 1)
                        wpDestination = PATH_FROM_GUID;
                    delete result;
                }
            }
        }
    }

    // All arguments parsed
    // wpOwner will get a new waypoint inserted into wpPath = GetPathFromOrigin(wpOwner, wpDestination, wpPathId) at wpPointId

    float x, y, z;
    m_session->GetPlayer()->GetPosition(x, y, z);
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (!sWaypointMgr.AddNode(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpPointId, wpDestination, x, y, z))
    {
        PSendSysMessage(LANG_WAYPOINT_NOTCREATED, wpPointId, wpOwner->GetGuidStr().c_str(), wpPathId, WaypointManager::GetOriginString(wpDestination).c_str());
        SetSentErrorMessage(true);
        return false;
    }

    // Unsummon old visuals, summon new ones
    UnsummonVisualWaypoints(m_session->GetPlayer(), wpOwner->GetObjectGuid());
    WaypointPath const* wpPath = sWaypointMgr.GetPathFromOrigin(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpPathId, wpDestination);
    for (const auto& itr : *wpPath)
    {
        if (!Helper_CreateWaypointFor(wpOwner, wpDestination, wpPathId, itr.first, &itr.second, waypointInfo))
        {
            PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, VISUAL_WAYPOINT);
            SetSentErrorMessage(true);
            return false;
        }
    }

    PSendSysMessage(LANG_WAYPOINT_ADDED, wpPointId, wpOwner->GetGuidStr().c_str(), wpPathId, WaypointManager::GetOriginString(wpDestination).c_str());

    return true;
}                                                           // HandleWpAddCommand

/**
 * .wp modify waittime | scriptid | orientation | del | move [dbGuid, id] [value]
 *
 * waittime <Delay>
 *   User has selected a visual waypoint before.
 *   Delay <Delay> is added to this waypoint. Everytime the
 *   NPC comes to this waypoint, it will wait Delay millieseconds.
 *
 * waittime <DBGuid> <WPNUM> <Delay>
 *   User has not selected visual waypoint before.
 *   For the waypoint <WPNUM> for the NPC with <DBGuid>
 *   an delay Delay is added to this waypoint
 *   Everytime the NPC comes to this waypoint, it will wait Delay millieseconds.
 *
 * scriptid <scriptId>
 *   User has selected a visual waypoint before.
 *   <scriptId> is added to this waypoint. Everytime the
 *   NPC comes to this waypoint, the DBScript scriptId is executed.
 *
 * scriptid <DBGuid> <WPNUM> <scriptId>
 *   User has not selected visual waypoint before.
 *   For the waypoint <WPNUM> for the NPC with <DBGuid>
 *   an emote <scriptId> is added.
 *   Everytime the NPC comes to this waypoint, the DBScript scriptId is executed.
 *
 * orientation [DBGuid, WpNum] <Orientation>
 *   Set the orientation of the selected waypoint or waypoint given with DbGuid/ WpId
 *   to the value of <Orientation>.
 *
 * del [DBGuid, WpId]
 *   Remove the selected waypoint or waypoint given with DbGuid/ WpId.
 *
 * move [DBGuid, WpId]
 *   Move the selected waypoint or waypoint given with DbGuid/ WpId to player's current positiion.
 */
bool ChatHandler::HandleWpModifyCommand(char* args)
{
    DEBUG_LOG("DEBUG: HandleWpModifyCommand");

    if (!*args)
    {
        return false;
    }

    CreatureInfo const* waypointInfo = sObjectMgr.GetCreatureTemplate(VISUAL_WAYPOINT);
    if (!waypointInfo || waypointInfo->GetHighGuid() != HIGHGUID_UNIT)
    {
        return false;
    }                                       // must exist as normal creature in mangos.sql 'creature_template'

// first arg: add del text emote spell waittime move
    char* subCmd_str = ExtractLiteralArg(&args);
    if (!subCmd_str)
    {
        return false;
    }

    std::string subCmd = subCmd_str;
    // Check
    // Remember: "show" must also be the name of a column!
    if ((subCmd != "waittime") && (subCmd != "scriptid") && (subCmd != "orientation") && (subCmd != "del") && (subCmd != "move"))
    {
        return false;
    }

    // Next arg is: <GUID> <WPNUM> <ARGUMENT>

    // Did user provide a GUID or did the user select a creature?
    Creature* targetCreature = GetSelectedCreature();       // Expect a visual waypoint to be selected
    Creature* wpOwner;                               // Who moves along the waypoint
    uint32 wpId = 0;
    WaypointPathOrigin wpSource = PATH_NO_PATH;
    int32 wpPathId = 0;

    if (targetCreature)
    {
        DEBUG_LOG("DEBUG: HandleWpModifyCommand - User did select an NPC");

        // Check if the user did specify a visual waypoint
        if (targetCreature->GetEntry() != VISUAL_WAYPOINT || targetCreature->GetSubtype() != CREATURE_SUBTYPE_TEMPORARY_SUMMON)
        {
            PSendSysMessage(LANG_WAYPOINT_VP_SELECT);
            SetSentErrorMessage(true);
            return false;
        }
        TemporarySummonWaypoint* wpTarget = dynamic_cast<TemporarySummonWaypoint*>(targetCreature);
        if (!wpTarget)
        {
            PSendSysMessage(LANG_WAYPOINT_VP_SELECT);
            SetSentErrorMessage(true);
            return false;
        }

        // Who moves along this waypoint?
        wpOwner = targetCreature->GetMap()->GetAnyTypeCreature(wpTarget->GetSummonerGuid());
        if (!wpOwner)
        {
            PSendSysMessage(LANG_WAYPOINT_NOTFOUND_NPC, wpTarget->GetSummonerGuid().GetString().c_str());
            SetSentErrorMessage(true);
            return false;
        }
        wpId = wpTarget->GetWaypointId()-1;

        wpPathId = wpTarget->GetPathId();
        wpSource = (WaypointPathOrigin)wpTarget->GetPathOrigin();
    }
    else
    {
        uint32 dbGuid = 0;
        // User did provide <GUID> <WPNUM>
        if (!ExtractUInt32(&args, dbGuid))
        {
            SendSysMessage(LANG_WAYPOINT_NOGUID);
            SetSentErrorMessage(true);
            return false;
        }

        if (!ExtractUInt32(&args, wpId))
        {
            SendSysMessage(LANG_WAYPOINT_NOWAYPOINTGIVEN);
            SetSentErrorMessage(true);
            return false;
        }

        CreatureData const* data = sObjectMgr.GetCreatureData(dbGuid);
        if (!data)
        {
            PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }

        wpOwner = m_session->GetPlayer()->GetMap()->GetAnyTypeCreature(data->GetObjectGuid(dbGuid));
        if (!wpOwner)
        {
            PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if (wpSource == PATH_NO_PATH)                           // No waypoint selected
    {
        if (wpOwner->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
            if (WaypointMovementGenerator<Creature> const* wpMMGen = dynamic_cast<WaypointMovementGenerator<Creature> const*>(wpOwner->GetMotionMaster()->GetCurrent()))
                wpMMGen->GetPathInformation(wpSource);

        if (wpSource == PATH_NO_PATH)
            sWaypointMgr.GetDefaultPath(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), &wpSource);
    }

    WaypointPath const* wpPath = sWaypointMgr.GetPathFromOrigin(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpPathId, wpSource);
    if (!wpPath)
    {
        PSendSysMessage(LANG_WAYPOINT_NOTFOUNDPATH, wpOwner->GetGuidStr().c_str(), wpPathId, WaypointManager::GetOriginString(wpSource).c_str());
        SetSentErrorMessage(true);
        return false;
    }

    WaypointPath::const_iterator point = wpPath->find(wpId);
    if (point == wpPath->end())
    {
        PSendSysMessage(LANG_WAYPOINT_NOTFOUND, wpId, wpOwner->GetGuidStr().c_str(), wpPathId, WaypointManager::GetOriginString(wpSource).c_str());
        SetSentErrorMessage(true);
        return false;
    }

    // If no visual WP was selected, but we are not going to remove it
    if (!targetCreature && subCmd != "del")
    {
        targetCreature = Helper_CreateWaypointFor(wpOwner, wpSource, wpPathId, wpId, &(point->second), waypointInfo);
        if (!targetCreature)
        {
            PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, VISUAL_WAYPOINT);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if (subCmd == "del")                                    // Remove WP, no additional command required
    {
        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        sWaypointMgr.DeleteNode(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpId, wpPathId, wpSource);

        if (TemporarySummonWaypoint* wpCreature = dynamic_cast<TemporarySummonWaypoint*>(targetCreature))
            wpCreature->UnSummon();

        if (wpPath->empty())
        {
            wpOwner->SetDefaultMovementType(RANDOM_MOTION_TYPE);
            wpOwner->GetMotionMaster()->Initialize();
            if (wpOwner->IsAlive())                         // Dead creature will reset movement generator at respawn
            {
                wpOwner->SetDeathState(JUST_DIED);
                wpOwner->Respawn();
            }
            wpOwner->SaveToDB();
        }

        PSendSysMessage(LANG_WAYPOINT_REMOVED);
        return true;
    }
    else if (subCmd == "move")                              // Move to player position, no additional command required
    {
        float x, y, z;
        m_session->GetPlayer()->GetPosition(x, y, z);

        // Move visual waypoint
        targetCreature->NearTeleportTo(x, y, z, targetCreature->GetOrientation());

        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        sWaypointMgr.SetNodePosition(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpId, wpPathId, wpSource, x, y, z);

        PSendSysMessage(LANG_WAYPOINT_CHANGED);
        return true;
    }
    else if (subCmd == "waittime")
    {
        uint32 waittime;
        if (!ExtractUInt32(&args, waittime))
            return false;

        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        sWaypointMgr.SetNodeWaittime(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpId, wpPathId, wpSource, waittime);
    }
    else if (subCmd == "scriptid")
    {
        uint32 scriptId;
        if (!ExtractUInt32(&args, scriptId))
            return false;

        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        if (!sWaypointMgr.SetNodeScriptId(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpId, wpPathId, wpSource, scriptId))
            PSendSysMessage(LANG_WAYPOINT_INFO_UNK_SCRIPTID, scriptId);
    }
    else if (subCmd == "orientation")
    {
        float ori;
        if (!ExtractFloat(&args, ori))
            return false;

        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        sWaypointMgr.SetNodeOrientation(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpId, wpPathId, wpSource, ori);
    }

    PSendSysMessage(LANG_WAYPOINT_CHANGED_NO, subCmd_str);
    return true;
}

/**
 * .wp show info | on | off | first | last [dbGuid] [pathId [wpOrigin] ]
 *
 * info -> User has selected a visual waypoint before
 *
 * on -> User has selected an NPC; all visual waypoints for this
 *       NPC are added to the world
 *
 * on <dbGuid> -> User did not select an NPC - instead the dbGuid of the
 *              NPC is provided. All visual waypoints for this NPC
 *              are added from the world.
 *
 * off -> User has selected an NPC; all visual waypoints for this
 *        NPC are removed from the world.
 */
bool ChatHandler::HandleWpShowCommand(char* args)
{
    DEBUG_LOG("DEBUG: HandleWpShowCommand");

    if (!*args)
    {
        return false;
    }

    CreatureInfo const* waypointInfo = sObjectMgr.GetCreatureTemplate(VISUAL_WAYPOINT);
    if (!waypointInfo || waypointInfo->GetHighGuid() != HIGHGUID_UNIT)
    {
        return false;
    }                                       // must exist as normal creature in mangos.sql 'creature_template'

// first arg: info, on, off, first, last

    char* subCmd_str = ExtractLiteralArg(&args);
    if (!subCmd_str)
        return false;
    std::string subCmd = subCmd_str;                        ///< info, on, off, first, last

    uint32 dbGuid = 0;
    int32 wpPathId = 0;
    WaypointPathOrigin wpOrigin = PATH_NO_PATH;

    // User selected an npc?
    Creature* targetCreature = GetSelectedCreature();
    if (targetCreature)
    {
        if (ExtractOptInt32(&args, wpPathId, 0))            // Fill path-id and source
        {
            uint32 src;
            if (ExtractOptUInt32(&args, src, (uint32)PATH_NO_PATH))
                wpOrigin = (WaypointPathOrigin)src;
        }
    }
    else    // Guid must be provided
    {
        if (!ExtractUInt32(&args, dbGuid))                  // No creature selected and no dbGuid provided
            return false;

        if (ExtractOptInt32(&args, wpPathId, 0))            // Fill path-id and source
        {
            uint32 src = (uint32)PATH_NO_PATH;
            if (ExtractOptUInt32(&args, src, src))
                wpOrigin = (WaypointPathOrigin)src;
        }

        // Params now parsed, check them
        CreatureData const* data = sObjectMgr.GetCreatureData(dbGuid);
        if (!data)
        {
            PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }

        targetCreature = m_session->GetPlayer()->GetMap()->GetCreature(data->GetObjectGuid(dbGuid));
        if (!targetCreature)
        {
            PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }
    }

    Creature* wpOwner;                                      // Npc that is moving
    TemporarySummonWaypoint* wpTarget = nullptr;               // Define here for wp-info command

    // Show info for the selected waypoint (Step one: get moving npc)
    if (subCmd == "info")
    {
        // Check if the user did specify a visual waypoint
        if (targetCreature->GetEntry() != VISUAL_WAYPOINT || targetCreature->GetSubtype() != CREATURE_SUBTYPE_TEMPORARY_SUMMON)
        {
            PSendSysMessage(LANG_WAYPOINT_VP_SELECT);
            SetSentErrorMessage(true);
            return false;
        }
        wpTarget = dynamic_cast<TemporarySummonWaypoint*>(targetCreature);
        if (!wpTarget)
        {
            PSendSysMessage(LANG_WAYPOINT_VP_SELECT);
            SetSentErrorMessage(true);
            return false;
        }

        // Who moves along this waypoint?
        wpOwner = targetCreature->GetMap()->GetAnyTypeCreature(wpTarget->GetSummonerGuid());
        if (!wpOwner)
        {
            PSendSysMessage(LANG_WAYPOINT_NOTFOUND_NPC, wpTarget->GetSummonerGuid().GetString().c_str());
            SetSentErrorMessage(true);
            return false;
        }

        // Ignore params, use information of selected waypoint!
        wpOrigin = (WaypointPathOrigin)wpTarget->GetPathOrigin();
        wpPathId = wpTarget->GetPathId();
    }
    else
        wpOwner = targetCreature;

    // Get the path
    WaypointPath* wpPath = nullptr;
    if (wpOrigin != PATH_NO_PATH)                           // Might have been provided by param
        wpPath = sWaypointMgr.GetPathFromOrigin(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpPathId, wpOrigin);
    else
    {
        if (wpOwner->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
            if (WaypointMovementGenerator<Creature> const* wpMMGen = dynamic_cast<WaypointMovementGenerator<Creature> const*>(wpOwner->GetMotionMaster()->GetCurrent()))
            {
                wpMMGen->GetPathInformation(wpOrigin);
                wpPath = sWaypointMgr.GetPathFromOrigin(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpPathId, wpOrigin);
            }

        if (wpOrigin == PATH_NO_PATH)
            wpPath = sWaypointMgr.GetDefaultPath(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), &wpOrigin);
    }

    if (!wpPath || wpPath->empty())
    {
        PSendSysMessage(LANG_WAYPOINT_NOTFOUNDPATH, wpOwner->GetGuidStr().c_str(), wpPathId, WaypointManager::GetOriginString(wpOrigin).c_str());
        SetSentErrorMessage(true);
        return false;
    }

    // Show info for the selected waypoint (Step two: Show actual info)
    if (subCmd == "info")
    {
        // Find the waypoint
        WaypointPath::const_iterator point = wpPath->find(wpTarget->GetWaypointId());
        if (point == wpPath->end())
        {
            PSendSysMessage(LANG_WAYPOINT_NOTFOUND, wpTarget->GetWaypointId(), wpOwner->GetGuidStr().c_str(), wpPathId, WaypointManager::GetOriginString(wpOrigin).c_str());
            SetSentErrorMessage(true);
            return false;
        }

        PSendSysMessage(LANG_WAYPOINT_INFO_TITLE, wpTarget->GetWaypointId(), wpOwner->GetGuidStr().c_str(), wpPathId, WaypointManager::GetOriginString(wpOrigin).c_str());
        PSendSysMessage(LANG_WAYPOINT_INFO_WAITTIME, point->second.delay);
        PSendSysMessage(LANG_WAYPOINT_INFO_ORI, point->second.orientation);
        PSendSysMessage(LANG_WAYPOINT_INFO_SCRIPTID, point->second.script_id);
        if (wpOrigin == PATH_FROM_SPECIAL)
            PSendSysMessage(LANG_WAYPOINT_INFO_AISCRIPT, wpOwner->GetScriptName().c_str());

        return true;
    }

    if (subCmd == "on")
    {
        UnsummonVisualWaypoints(m_session->GetPlayer(), wpOwner->GetObjectGuid());

        for (const auto& itr : *wpPath)
        {
            if (!Helper_CreateWaypointFor(wpOwner, wpOrigin, wpPathId, itr.first, &(itr.second), waypointInfo))
            {
                printf("error %s wpPathId %i", wpOwner->GetName(), wpPathId);
                PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, VISUAL_WAYPOINT);
                SetSentErrorMessage(true);
                return false;
            }
        }

        return true;
    }

    if (subCmd == "first")
    {
        if (!Helper_CreateWaypointFor(wpOwner, wpOrigin, wpPathId, wpPath->begin()->first, &(wpPath->begin()->second), waypointInfo))
        {
            PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, VISUAL_WAYPOINT);
            SetSentErrorMessage(true);
            return false;
        }

        // player->PlayerTalkClass->SendPointOfInterest(x, y, 6, 6, 0, "First Waypoint");
        return true;
    }

    if (subCmd == "last")
    {
        if (!Helper_CreateWaypointFor(wpOwner, wpOrigin, wpPathId, wpPath->rbegin()->first, &(wpPath->rbegin()->second), waypointInfo))
        {
            PSendSysMessage(LANG_WAYPOINT_VP_NOTCREATED, VISUAL_WAYPOINT);
            SetSentErrorMessage(true);
            return false;
        }

        // player->PlayerTalkClass->SendPointOfInterest(x, y, 6, 6, 0, "Last Waypoint");
        return true;
    }

    if (subCmd == "off")
    {
        UnsummonVisualWaypoints(m_session->GetPlayer(), wpOwner->GetObjectGuid());
        PSendSysMessage(LANG_WAYPOINT_VP_ALLREMOVED);
        return true;
    }

    return false;
}                                                           // HandleWpShowCommand

                                                            /// [Guid if no selected unit] <filename> [pathId [wpOrigin] ]
bool ChatHandler::HandleWpExportCommand(char* args)
{
    if (!*args)
        return false;

    Creature* wpOwner;
    WaypointPathOrigin wpOrigin = PATH_NO_PATH;
    int32 wpPathId = 0;

    if (Creature* targetCreature = GetSelectedCreature())
    {
        // Check if the user did specify a visual waypoint
        if (targetCreature->GetEntry() == VISUAL_WAYPOINT && targetCreature->GetSubtype() == CREATURE_SUBTYPE_TEMPORARY_SUMMON)
        {
            TemporarySummonWaypoint* wpTarget = dynamic_cast<TemporarySummonWaypoint*>(targetCreature);
            if (!wpTarget)
            {
                PSendSysMessage(LANG_WAYPOINT_VP_SELECT);
                SetSentErrorMessage(true);
                return false;
            }

            // Who moves along this waypoint?
            wpOwner = targetCreature->GetMap()->GetAnyTypeCreature(wpTarget->GetSummonerGuid());
            if (!wpOwner)
            {
                PSendSysMessage(LANG_WAYPOINT_NOTFOUND_NPC, wpTarget->GetSummonerGuid().GetString().c_str());
                SetSentErrorMessage(true);
                return false;
            }
            wpOrigin = (WaypointPathOrigin)wpTarget->GetPathOrigin();
            wpPathId = wpTarget->GetPathId();
        }
        else // normal creature selected
            wpOwner = targetCreature;
    }
    else
    {
        uint32 dbGuid;
        if (!ExtractUInt32(&args, dbGuid))
        {
            PSendSysMessage(LANG_WAYPOINT_NOGUID);
            SetSentErrorMessage(true);
            return false;
        }

        CreatureData const* data = sObjectMgr.GetCreatureData(dbGuid);
        if (!data)
        {
            PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }

        if (m_session->GetPlayer()->GetMapId() != data->position.mapId)
        {
            PSendSysMessage(LANG_COMMAND_CREATUREATSAMEMAP, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }

        wpOwner = m_session->GetPlayer()->GetMap()->GetAnyTypeCreature(data->GetObjectGuid(dbGuid));
        if (!wpOwner)
        {
            PSendSysMessage(LANG_WAYPOINT_CREATNOTFOUND, dbGuid);
            SetSentErrorMessage(true);
            return false;
        }
    }

    // wpOwner is now known, in case of export by visual waypoint also the to be exported path
    char* export_str = ExtractLiteralArg(&args);
    if (!export_str)
    {
        PSendSysMessage(LANG_WAYPOINT_ARGUMENTREQ, "export");
        SetSentErrorMessage(true);
        return false;
    }

    if (wpOrigin == PATH_NO_PATH)                           // No WP selected, Extract optional arguments
    {
        if (ExtractOptInt32(&args, wpPathId, 0))            // Fill path-id and source
        {
            uint32 src = (uint32)PATH_NO_PATH;
            if (ExtractOptUInt32(&args, src, src))
                wpOrigin = (WaypointPathOrigin)src;
            else // pathId provided but no destination
            {
                if (wpPathId != 0)
                    wpOrigin = PATH_FROM_ENTRY;             // Multiple Paths must only be assigned by entry
            }
        }

        if (wpOrigin == PATH_NO_PATH)
        {
            if (wpOwner->GetMotionMaster()->GetCurrentMovementGeneratorType() == WAYPOINT_MOTION_TYPE)
                if (WaypointMovementGenerator<Creature> const* wpMMGen = dynamic_cast<WaypointMovementGenerator<Creature> const*>(wpOwner->GetMotionMaster()->GetCurrent()))
                    wpMMGen->GetPathInformation(wpOrigin);
            if (wpOrigin == PATH_NO_PATH)
                sWaypointMgr.GetDefaultPath(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), &wpOrigin);
        }
    }

    WaypointPath const* wpPath = sWaypointMgr.GetPathFromOrigin(wpOwner->GetEntry(), wpOwner->GetGUIDLow(), wpPathId, wpOrigin);
    if (!wpPath || wpPath->empty())
    {
        PSendSysMessage(LANG_WAYPOINT_NOTHINGTOEXPORT);
        SetSentErrorMessage(true);
        return false;
    }

    std::ofstream outfile;
    outfile.open(export_str);

    std::string table;
    char const* key_field;
    uint32 key;
    switch (wpOrigin)
    {
    case PATH_FROM_ENTRY: key = wpOwner->GetEntry();    key_field = "entry";    table = "creature_movement_template"; break;
    case PATH_FROM_GUID: key = wpOwner->GetGUIDLow();   key_field = "id";       table = "creature_movement"; break;
    case PATH_FROM_SPECIAL: key = wpOwner->GetEntry();  key_field = "id";    table = "creature_movement_special"; break;
    case PATH_NO_PATH:
        return false;
    }

    outfile << "DELETE FROM " << table << " WHERE " << key_field << "=" << key << ";\n";
    outfile << "INSERT INTO " << table << " (" << key_field << ", point, position_x, position_y, position_z, orientation, waittime, script_id) VALUES\n";

    WaypointPath::const_iterator itr = wpPath->begin();
    uint32 countDown = wpPath->size();
    for (; itr != wpPath->end(); ++itr, --countDown)
    {
        outfile << "(" << key << ",";
        outfile << itr->first << ",";
        outfile << itr->second.x << ",";
        outfile << itr->second.y << ",";
        outfile << itr->second.z << ",";
        outfile << itr->second.orientation << ",";
        outfile << itr->second.delay << ",";
        outfile << itr->second.script_id << ")";
        if (countDown > 1)
            outfile << ",\n";
        else
            outfile << ";\n";
    }

    PSendSysMessage(LANG_WAYPOINT_EXPORTED);
    outfile.close();

    return true;
}

//rename characters
bool ChatHandler::HandleCharacterRenameCommand(char* args)
{
    Player* target;
    ObjectGuid targetGuid;
    std::string targetName;
    if (!ExtractPlayerTarget(&args, &target, &targetGuid, &targetName))
        return false;

    char* secondArg = ExtractLiteralArg(&args);
    bool cancel = secondArg && strstr(secondArg, "cancel");

    if (cancel)
    {
        if (target)
        {
            if (target->HasAtLoginFlag(AT_LOGIN_RENAME))
            {
                target->RemoveAtLoginFlag(AT_LOGIN_RENAME, true);
                SendSysMessage("At login rename removed in DB and memory.");
            }
            else
                SendSysMessage("Player doens't have rename flag.");
        }
    }
    else if (secondArg)
    {
        std::string name = secondArg;
        CharacterDatabase.escape_string(name);

        if (QueryResult* pResult = CharacterDatabase.PQuery("SELECT guid FROM characters WHERE `name`='%s'", name.c_str()))
        {
            PSendSysMessage("Cannot rename player to '%s' because that name is already taken.", secondArg);
            delete pResult;
            SetSentErrorMessage(true);
            return false;
        }

        if (target)
            target->SetName(secondArg);

        CharacterDatabase.PExecute("UPDATE `characters` SET `name` = '%s' WHERE guid = '%u'", name.c_str(), targetGuid.GetCounter());
        sObjectMgr.ChangePlayerNameInCache(targetGuid.GetCounter(), targetName, secondArg);
        sWorld.InvalidatePlayerDataToAllClients(targetGuid);

        PSendSysMessage("Renamed player '%s' to '%s'.", targetName.c_str(), secondArg);
    }
    else
    {
        if (target)
        {
            // check online security
            if (HasLowerSecurity(target))
                return false;

            PSendSysMessage(LANG_RENAME_PLAYER, GetNameLink(target).c_str());
            target->SetAtLoginFlag(AT_LOGIN_RENAME);
            // Notify the player about the rename.
            ChatHandler(target).PSendSysMessage("Your character has been flagged for a rename. Please make sure to logout, read our rules, and select a new appropriate name.");
            CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '1' WHERE guid = '%u'", target->GetGUIDLow());
        }
        else
        {
            // check offline security
            if (HasLowerSecurity(nullptr, targetGuid))
                return false;

            std::string oldNameLink = playerLink(targetName);

            PSendSysMessage(LANG_RENAME_PLAYER_GUID, oldNameLink.c_str(), targetGuid.GetCounter());
            // If the player is offline, also forcefully set the current name to be just the character's guid.
            CharacterDatabase.PExecute("UPDATE characters SET name = guid, at_login = at_login | '1' WHERE guid = '%u'", targetGuid.GetCounter());
        }
    }
    

    return true;
}

bool ChatHandler::HandleCharacterReputationCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    LocaleConstant loc = GetSessionDbcLocale();

    FactionStateList const& targetFSL = target->GetReputationMgr().GetStateList();
    for (const auto& itr : targetFSL)
    {
        FactionEntry const* factionEntry = sObjectMgr.GetFactionEntry(itr.second.ID);

        ShowFactionListHelper(factionEntry, loc, &itr.second, target);
    }
    return true;
}

bool ChatHandler::HandleCharacterHasItemCommand(char* args)
{
    if (!*args)
        return false;

    uint32 itemId = 0;
    if (!ExtractUInt32(&args, itemId))
        return false;

    Player* plTarget;
    ObjectGuid target_guid;
    std::string target_name;

    if (!ExtractPlayerTarget(&args, &plTarget, &target_guid, &target_name))
        return false;

    ItemPrototype const* pItem = sObjectMgr.GetItemPrototype(itemId);

    if (!pItem)
    {
        PSendSysMessage("Unknown item %u", itemId);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 itemCount = 0;

    if (plTarget)
    {
        itemCount = plTarget->GetItemCount(itemId, true);
    }
    else
    {
        std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery(
            "SELECT SUM(count) AS item_count FROM item_instance ii WHERE itemEntry = %u and owner_guid = %u",
            itemId, target_guid.GetCounter()
        ));

        if (result)
        {
            auto fields = result->Fetch();
            itemCount = fields[0].GetUInt32();
        }
    }

    PSendSysMessage("%s's amount of %s (id %u) is: %u", target_name.c_str(), GetItemLink(pItem).c_str(), itemId, itemCount);
    return true;
}

bool ChatHandler::HandleHonorSetRPCommand(char* args)
{
    Player* target = GetSelectedPlayer();
    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    float value;
    if (!ExtractFloat(&args, value))
        return false;

    target->GetHonorMgr().SetRankPoints(value);
    target->GetHonorMgr().Update();
    PSendSysMessage("You have changed rank points of %s to %g.", target->GetName(), value);
    return true;
}

bool ChatHandler::HandleHonorShow(char* /*args*/)
{
    Player* target = GetSelectedPlayer();
    if (!target)
        target = m_session->GetPlayer();

    int8 highest_rank = target->GetHonorMgr().GetHighestRank().visualRank;
    uint32 dishonorable_kills = target->GetUInt32Value(PLAYER_FIELD_LIFETIME_DISHONORABLE_KILLS);
    uint32 honorable_kills = target->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS);
    uint32 today_honorable_kills = target->GetUInt16Value(PLAYER_FIELD_SESSION_KILLS, 0);
    uint32 today_dishonorable_kills = target->GetUInt16Value(PLAYER_FIELD_SESSION_KILLS, 1);
    uint32 yesterday_kills = target->GetUInt32Value(PLAYER_FIELD_YESTERDAY_KILLS);
    uint32 yesterday_honor = target->GetUInt32Value(PLAYER_FIELD_YESTERDAY_CONTRIBUTION);
    uint32 this_week_kills = target->GetUInt32Value(PLAYER_FIELD_THIS_WEEK_KILLS);
    uint32 this_week_honor = target->GetUInt32Value(PLAYER_FIELD_THIS_WEEK_CONTRIBUTION);
    uint32 last_week_kills = target->GetUInt32Value(PLAYER_FIELD_LAST_WEEK_KILLS);
    uint32 last_week_honor = target->GetUInt32Value(PLAYER_FIELD_LAST_WEEK_CONTRIBUTION);
    uint32 last_week_standing = target->GetUInt32Value(PLAYER_FIELD_LAST_WEEK_RANK);

    static int16 alliance_ranks[HONOR_RANK_COUNT] =
    {
        LANG_NO_RANK,
        LANG_RANK_PARIAH,
        LANG_RANK_OUTLAW,
        LANG_RANK_EXILED,
        LANG_RANK_DISHONORED,
        LANG_ALI_PRIVATE,
        LANG_ALI_CORPORAL,
        LANG_ALI_SERGEANT,
        LANG_ALI_MASTER_SERGEANT,
        LANG_ALI_SERGEANT_MAJOR,
        LANG_ALI_KNIGHT,
        LANG_ALI_KNIGHT_LIEUTENANT,
        LANG_ALI_KNIGHT_CAPTAIN,
        LANG_ALI_KNIGHT_CHAMPION,
        LANG_ALI_LIEUTENANT_COMMANDER,
        LANG_ALI_COMMANDER,
        LANG_ALI_MARSHAL,
        LANG_ALI_FIELD_MARSHAL,
        LANG_ALI_GRAND_MARSHAL,
        //LANG_GAME_MASTER
    };
    static int16 horde_ranks[HONOR_RANK_COUNT] =
    {
        LANG_NO_RANK,
        LANG_RANK_PARIAH,
        LANG_RANK_OUTLAW,
        LANG_RANK_EXILED,
        LANG_RANK_DISHONORED,
        LANG_HRD_SCOUT,
        LANG_HRD_GRUNT,
        LANG_HRD_SERGEANT,
        LANG_HRD_SENIOR_SERGEANT,
        LANG_HRD_FIRST_SERGEANT,
        LANG_HRD_STONE_GUARD,
        LANG_HRD_BLOOD_GUARD,
        LANG_HRD_LEGIONNARE,
        LANG_HRD_CENTURION,
        LANG_HRD_CHAMPION,
        LANG_HRD_LIEUTENANT_GENERAL,
        LANG_HRD_GENERAL,
        LANG_HRD_WARLORD,
        LANG_HRD_HIGH_WARLORD,
        //LANG_GAME_MASTER
    };
    char const* rank_name = nullptr;
    char const* hrank_name = nullptr;

    uint32 honor_rank = target->GetHonorMgr().GetRank().visualRank;

    if (honor_rank >= HONOR_RANK_COUNT || highest_rank >= HONOR_RANK_COUNT)
    {
        PSendSysMessage("CrashAlert : R%u et HR%u", honor_rank, highest_rank);
        return false;
    }

    if (target->GetTeam() == ALLIANCE)
    {
        rank_name = GetMangosString(alliance_ranks[honor_rank]);
        hrank_name = GetMangosString(alliance_ranks[highest_rank]);
    }
    else if (target->GetTeam() == HORDE)
    {
        rank_name = GetMangosString(horde_ranks[honor_rank]);
        hrank_name = GetMangosString(horde_ranks[highest_rank]);
    }
    else
    {
        rank_name = GetMangosString(LANG_NO_RANK);
        hrank_name = GetMangosString(LANG_NO_RANK);
    }

    PSendSysMessage(LANG_RANK, target->GetName(), rank_name, honor_rank);
    PSendSysMessage(LANG_HONOR_TODAY, today_honorable_kills, today_dishonorable_kills);
    PSendSysMessage(LANG_HONOR_YESTERDAY, yesterday_kills, yesterday_honor);
    PSendSysMessage(LANG_HONOR_THIS_WEEK, this_week_kills, this_week_honor);
    PSendSysMessage(LANG_HONOR_LAST_WEEK, last_week_kills, last_week_honor, last_week_standing);
    PSendSysMessage(LANG_HONOR_LIFE, target->GetHonorMgr().GetRankPoints(), honorable_kills, dishonorable_kills, highest_rank, hrank_name);

    return true;
}

bool ChatHandler::HandleEventListCommand(char* args)
{
    uint32 counter = 0;
    bool all = false;
    std::string arg = args;
    if (arg == "all")
        all = true;

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

    char const* active = GetMangosString(LANG_ACTIVE);
    char const* inactive = GetMangosString(LANG_FACTION_INACTIVE);
    char const* state = "";

    for (uint32 event_id = 0; event_id < events.size(); ++event_id)
    {
        if (!sGameEventMgr.IsValidEvent(event_id))
            continue;

        if (!sGameEventMgr.IsActiveEvent(event_id))
        {
            if (!all)
                continue;
            state = inactive;
        }
        else
            state = active;

        GameEventData const& eventData = events[event_id];

        if (m_session)
            PSendSysMessage(LANG_EVENT_ENTRY_LIST_CHAT, event_id, event_id, eventData.description.c_str(), state);
        else
            PSendSysMessage(LANG_EVENT_ENTRY_LIST_CONSOLE, event_id, eventData.description.c_str(), state);

        ++counter;
    }

    if (counter == 0)
        SendSysMessage(LANG_NOEVENTFOUND);

    return true;
}

bool ChatHandler::HandleEventInfoCommand(char* args)
{
    if (!*args)
        return false;

    // id or [name] Shift-click form |color|Hgameevent:id|h[name]|h|r
    uint32 event_id;
    if (!ExtractUint32KeyFromLink(&args, "Hgameevent", event_id))
        return false;

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

    if (!sGameEventMgr.IsValidEvent(event_id))
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    GameEventData const& eventData = events[event_id];

    char const* activeStr = sGameEventMgr.IsActiveEvent(event_id) ? GetMangosString(LANG_ACTIVE) : "";

    std::string startTimeStr = TimeToTimestampStr(eventData.start);
    std::string endTimeStr = TimeToTimestampStr(eventData.end);

    uint32 delay = sGameEventMgr.NextCheck(event_id);
    time_t nextTime = time(nullptr) + delay;
    std::string nextStr = nextTime >= eventData.start && nextTime < eventData.end ? TimeToTimestampStr(time(nullptr) + delay) : "-";

    std::string occurenceStr = secsToTimeString(eventData.occurence * MINUTE);
    std::string lengthStr = secsToTimeString(eventData.length * MINUTE);

    PSendSysMessage(LANG_EVENT_INFO, event_id, eventData.description.c_str(), activeStr,
        startTimeStr.c_str(), endTimeStr.c_str(), occurenceStr.c_str(), lengthStr.c_str(),
        nextStr.c_str());
    return true;
}

bool ChatHandler::HandleEventStartCommand(char* args)
{
    if (!*args)
        return false;

    // id or [name] Shift-click form |color|Hgameevent:id|h[name]|h|r
    uint32 event_id;
    if (!ExtractUint32KeyFromLink(&args, "Hgameevent", event_id))
        return false;

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

    if (!sGameEventMgr.IsValidEvent(event_id))
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    GameEventData const& eventData = events[event_id];
    if (!eventData.isValid())
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    if (sGameEventMgr.IsActiveEvent(event_id))
    {
        PSendSysMessage(LANG_EVENT_ALREADY_ACTIVE, event_id);
        SetSentErrorMessage(true);
        return false;
    }

    if (!sGameEventMgr.IsEnabled(event_id))
    {
        PSendSysMessage(LANG_EVENT_DISABLED, event_id);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_EVENT_STARTED, event_id, eventData.description.c_str());
    sGameEventMgr.StartEvent(event_id, true);
    return true;
}

bool ChatHandler::HandleEventStopCommand(char* args)
{
    if (!*args)
        return false;

    // id or [name] Shift-click form |color|Hgameevent:id|h[name]|h|r
    uint32 event_id;
    if (!ExtractUint32KeyFromLink(&args, "Hgameevent", event_id))
        return false;

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

    if (!sGameEventMgr.IsValidEvent(event_id))
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    GameEventData const& eventData = events[event_id];
    if (!eventData.isValid())
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    if (!sGameEventMgr.IsActiveEvent(event_id))
    {
        PSendSysMessage(LANG_EVENT_NOT_ACTIVE, event_id);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_EVENT_STOPPED, event_id, eventData.description.c_str());
    sGameEventMgr.StopEvent(event_id, true);
    return true;
}

bool ChatHandler::HandleEventEnableCommand(char* args)
{
    if (!*args)
        return false;

    // id or [name] Shift-click form |color|Hgameevent:id|h[name]|h|r
    uint32 event_id;
    if (!ExtractUint32KeyFromLink(&args, "Hgameevent", event_id))
        return false;

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

    if (!sGameEventMgr.IsValidEvent(event_id))
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    GameEventData const& eventData = events[event_id];
    if (!eventData.isValid())
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    if (sGameEventMgr.IsEnabled(event_id))
    {
        PSendSysMessage(LANG_EVENT_ALREADY_ENABLED, event_id);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_EVENT_ENABLED, event_id, eventData.description.c_str());
    sGameEventMgr.EnableEvent(event_id, true);
    return true;
}

bool ChatHandler::HandleEventDisableCommand(char* args)
{
    if (!*args)
        return false;

    // id or [name] Shift-click form |color|Hgameevent:id|h[name]|h|r
    uint32 event_id;
    if (!ExtractUint32KeyFromLink(&args, "Hgameevent", event_id))
        return false;

    GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();

    if (!sGameEventMgr.IsValidEvent(event_id))
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    GameEventData const& eventData = events[event_id];
    if (!eventData.isValid())
    {
        SendSysMessage(LANG_EVENT_NOT_EXIST);
        SetSentErrorMessage(true);
        return false;
    }

    if (!sGameEventMgr.IsEnabled(event_id))
    {
        PSendSysMessage(LANG_EVENT_ALREADY_DISABLED, event_id);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage(LANG_EVENT_DISABLED, event_id, eventData.description.c_str());
    sGameEventMgr.EnableEvent(event_id, false);
    return true;
}

bool ChatHandler::HandleCombatStopCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    // check online security
    if (HasLowerSecurity(target))
        return false;

    target->CombatStop();
    target->GetHostileRefManager().deleteReferences();
    return true;
}

void ChatHandler::HandleLearnSkillRecipesHelper(Player* player, uint32 skill_id)
{
    uint32 classmask = player->GetClassMask();

    for (uint32 j = 0; j < sObjectMgr.GetMaxSkillLineAbilityId(); ++j)
    {
        SkillLineAbilityEntry const* skillLine = sObjectMgr.GetSkillLineAbility(j);
        if (!skillLine)
            continue;

        // wrong skill
        if (skillLine->skillId != skill_id)
            continue;

        // not high rank
        if (skillLine->forward_spellid)
            continue;

        // skip racial skills
        if (skillLine->racemask != 0)
            continue;

        // skip wrong class skills
        if (skillLine->classmask && (skillLine->classmask & classmask) == 0)
            continue;

        SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(skillLine->spellId);
        if (!spellInfo || !SpellMgr::IsSpellValid(spellInfo, player, false))
            continue;

        player->LearnSpell(skillLine->spellId, false);
    }
}

bool ChatHandler::ShowAccountIpListHelper(char* args, bool onlineonly)
{
    char* ipStr = ExtractQuotedOrLiteralArg(&args);
    if (!ipStr)
        return false;

    uint32 limit;
    if (!ExtractOptUInt32(&args, limit, 100))
        return false;

    std::string ip = ipStr;
    LoginDatabase.escape_string(ip);

    const char* query = onlineonly
        ? "SELECT id, username, last_ip, 0, expansion FROM account WHERE online = 1 AND last_ip " _LIKE_ " " _CONCAT2_("'%s'", "'%%'") " LIMIT %u"
        : "SELECT id, username, last_ip, 0, expansion FROM account WHERE                last_ip " _LIKE_ " " _CONCAT2_("'%s'", "'%%'") " LIMIT %u";

    LoginDatabase.AsyncPQuery(AccountSearchHandler::HandleAccountLookupResult, GetAccountId(), limit, query, ip.c_str(), limit);

    return true;
}

bool ChatHandler::HandleLookupPlayerIpCommand(char* args)
{
    char* ipStr = ExtractQuotedOrLiteralArg(&args);
    if (!ipStr)
        return false;

    uint32 limit;
    if (!ExtractOptUInt32(&args, limit, 100))
        return false;

    QueryResult* result = nullptr;
    std::string ip = ipStr;
    LoginDatabase.escape_string(ip);
    LoginDatabase.AsyncPQuery(&LookupPlayerHandler::HandleLookupPlayerIpResult, std::make_pair(GetSession()->GetAccountId(), limit),
        "SELECT id, username FROM account WHERE last_ip " _LIKE_ " " _CONCAT2_("'%s'", "'%%'"), ip.c_str());

    return LookupPlayerSearchCommand(result, &limit);
}

bool ChatHandler::HandleLookupPlayerAccountCommand(char* args)
{
    char* accountStr = ExtractQuotedOrLiteralArg(&args);
    if (!accountStr)
        return false;

    uint32 limit;
    if (!ExtractOptUInt32(&args, limit, 100))
        return false;

    std::string account = accountStr;
    if (!AccountMgr::normalizeString(account))
        return false;

    LoginDatabase.escape_string(account);

    QueryResult* result = LoginDatabase.PQuery("SELECT id,username FROM account WHERE username " _LIKE_ " " _CONCAT2_("'%s'", "'%%'"), account.c_str());

    return LookupPlayerSearchCommand(result, &limit);
}

bool ChatHandler::HandleLookupPlayerEmailCommand(char* args)
{
    char* emailStr = ExtractQuotedOrLiteralArg(&args);
    if (!emailStr)
        return false;

    uint32 limit;
    if (!ExtractOptUInt32(&args, limit, 100))
        return false;

    std::string email = emailStr;
    LoginDatabase.escape_string(email);

    QueryResult* result = LoginDatabase.PQuery("SELECT id,username FROM account WHERE email " _LIKE_ " " _CONCAT2_("'%s'", "'%%'"), email.c_str());

    return LookupPlayerSearchCommand(result, &limit);
}

bool ChatHandler::HandleLookupPlayerNameCommand(char* args)
{
    char* nameStr = ExtractQuotedOrLiteralArg(&args);
    if (!nameStr)
        return false;

    uint32 limit;
    if (!ExtractOptUInt32(&args, limit, 100))
        return false;

    uint32 limit_original = limit;
    std::string name = nameStr;
    CharacterDatabase.escape_string(name);

    CharacterDatabase.AsyncPQuery(&PlayerSearchHandler::HandlePlayerCharacterLookupResult,
        GetAccountId(), limit_original,
        "SELECT guid, name, race, class, level FROM characters WHERE name " _LIKE_ " " _CONCAT3_("'%%'", "'%s'", "'%%'"),
        name.c_str());

    return true;
}

bool ChatHandler::HandleLookupPlayerCharacterCommand(char* args)
{
    char* nameStr = ExtractQuotedOrLiteralArg(&args);
    if (!nameStr)
        return false;

    uint32 limit;
    if (!ExtractOptUInt32(&args, limit, 100))
        return false;

    QueryResult* result = nullptr;
    std::string normalizedName = nameStr;
    if (normalizePlayerName(normalizedName))
        if (PlayerCacheData const* data = sObjectMgr.GetPlayerDataByName(normalizedName))
            if (result = LoginDatabase.PQuery("SELECT id, last_ip FROM account WHERE id = %u", data->uiAccount))
            {
                Field* fields = result->Fetch();
                uint32 id = fields[0].GetInt32();
                std::string ip = fields[1].GetCppString();
                delete result;

                AccountTypes security = sAccountMgr.GetSecurity(id);
                if (GetAccessLevel() < security || (GetAccessLevel() < SEC_ADMINISTRATOR && security > SEC_PLAYER))
                    return LookupPlayerSearchCommand(nullptr, &limit);

                LoginDatabase.escape_string(ip);
                result = LoginDatabase.PQuery("SELECT id, username FROM account WHERE last_ip = '%s'", ip.c_str());
            }

    return LookupPlayerSearchCommand(result, &limit);
}

bool ChatHandler::LookupPlayerSearchCommand(QueryResult* result, uint32* limit)
{
    if (!result)
    {
        PSendSysMessage(LANG_NO_PLAYERS_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 limit_original = limit ? *limit : 100;

    uint32 limit_local = limit_original;

    if (!limit)
        limit = &limit_local;

    PlayerSearchQueryHolder* holder = new PlayerSearchQueryHolder(m_session ? m_session->GetAccountId() : 0, limit_local);
    holder->SetSize(result->GetRowCount());
    uint32 count = 0;
    do
    {
        // Definitely can't stay below the limit if we've found beyond the limit in accounts
        if (limit && count == *limit)
            break;

        Field* fields = result->Fetch();
        uint32 acc_id = fields[0].GetUInt32();
        std::string acc_name = fields[1].GetCppString();

        holder->SetPQuery(count, "SELECT guid, name, race, class, level FROM characters WHERE account = %u", acc_id);
        holder->AddAccountInfo(count, acc_id, acc_name);

        ++count;
    }     while (result->NextRow());
    delete result;

    CharacterDatabase.DelayQueryHolder(&PlayerSearchHandler::HandlePlayerAccountSearchResult, holder, 0);
    return true;
}

void ChatHandler::ShowPoolListHelper(uint16 pool_id)
{
    PoolTemplateData const& pool_template = sPoolMgr.GetPoolTemplate(pool_id);
    if (m_session)
        PSendSysMessage(LANG_POOL_ENTRY_LIST_CHAT,
            pool_id, pool_id, pool_template.description.c_str(), pool_template.IsAutoSpawn() ? 1 : 0, pool_template.MaxLimit,
            sPoolMgr.GetPoolCreatures(pool_id).size(), sPoolMgr.GetPoolGameObjects(pool_id).size(), sPoolMgr.GetPoolPools(pool_id).size());
    else
        PSendSysMessage(LANG_POOL_ENTRY_LIST_CONSOLE,
            pool_id, pool_template.description.c_str(), pool_template.IsAutoSpawn() ? 1 : 0, pool_template.MaxLimit,
            sPoolMgr.GetPoolCreatures(pool_id).size(), sPoolMgr.GetPoolGameObjects(pool_id).size(), sPoolMgr.GetPoolPools(pool_id).size());
}

/// Triggering corpses expire check in world
bool ChatHandler::HandleServerCorpsesCommand(char* /*args*/)
{
    sObjectAccessor.RemoveOldCorpses();
    return true;
}

bool ChatHandler::HandleRemoveCorpsesCommand(char* args)
{
    float dist = 0.0f;

    if (!ExtractFloat(&args, dist))
        return false;

    std::list<Corpse*> corpses;
    auto player = GetPlayer();

    if (!player)
        return false;


    MaNGOS::CorpseInRangeCheck check{player, dist};
    MaNGOS::CorpseListSearcher<MaNGOS::CorpseInRangeCheck> searcher{ corpses, check };

    //Visit world objects first, for corpses.
    Cell::VisitWorldObjects(player, searcher, dist);
    
    //Visit Grid objects for corpse-bones.
    Cell::VisitGridObjects(player, searcher, dist);


    for (auto& corpse : corpses)
    {
        //mark all corpses expired and schedule a bone update so that bones are cleaned up too.
        corpse->MarkExpired();
    }

    auto map = player->GetMap();
    if (map)
        map->ScheduleCorpseRemoval();

    sObjectAccessor.RemoveOldCorpses();

    return true;
}


bool ChatHandler::HandleRepairitemsCommand(char* args)
{
    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    // check online security
    if (HasLowerSecurity(target))
        return false;

    // Repair items
    target->DurabilityRepairAll(false, 0);

    PSendSysMessage(LANG_YOU_REPAIR_ITEMS, GetNameLink(target).c_str());
    if (needReportToTarget(target))
        ChatHandler(target).PSendSysMessage(LANG_YOUR_ITEMS_REPAIRED, GetNameLink().c_str());
    return true;
}

bool ChatHandler::HandleWaterwalkCommand(char* args)
{
    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    Player* player = GetSelectedPlayer();

    if (!player)
    {
        PSendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    // check online security
    if (HasLowerSecurity(player))
        return false;

    if (value)
        player->SetWaterWalking(true);
    else
        player->SetWaterWalking(false);

    PSendSysMessage(LANG_YOU_SET_WATERWALK, args, GetNameLink(player).c_str());
    if (needReportToTarget(player))
        ChatHandler(player).PSendSysMessage(LANG_YOUR_WATERWALK_SET, args, GetNameLink().c_str());
    return true;
}

// Custom Nostalrius commands

#define MAX_SPELL_EFFECTS 3

bool ChatHandler::HandleReloadSpellDisabledCommand(char* args)
{
    sObjectMgr.LoadSpellDisabledEntrys();
    SendSysMessage("DB table `spell_disabed` reloaded.");
    return true;
}
bool ChatHandler::HandleReloadAutoBroadcastCommand(char* args)
{
    sAutoBroadCastMgr.Load();
    SendSysMessage("DB table `autobroadcast` reloaded.");
    return true;
}


bool ChatHandler::HandleReloadItemTemplate(char*)
{
    sObjectMgr.LoadItemPrototypes();
    SendSysMessage("Table `item_template` reloaded.");
    return true;
}

bool ChatHandler::HandleGameObjectTempAddCommand(char* args)
{
    uint32 id;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject_entry", id))
        return false;

    if (!id)
        return false;

    Player* chr = m_session->GetPlayer();

    char* spawntime = strtok(nullptr, " ");
    uint32 spawntm = 300;

    if (spawntime)
        spawntm = atoi((char*)spawntime);

    float x = chr->GetPositionX();
    float y = chr->GetPositionY();
    float z = chr->GetPositionZ();
    float ang = chr->GetOrientation();

    float rot2 = sin(ang / 2);
    float rot3 = cos(ang / 2);

    return chr->SummonGameObject(id, x, y, z, ang, 0, 0, rot2, rot3, spawntm) != nullptr;
}

bool ChatHandler::HandleUpdateWorldStateCommand(char* args)
{
    uint32 value = 0;
    uint32 index = 0;
    sscanf(args, "%u %u", &index, &value);
    if (index == 0)
    {
        SendSysMessage("Usage :");
        SendSysMessage(".ws update index [value=0]");
        SetSentErrorMessage(true);
        return false;
    }
    m_session->GetPlayer()->SendUpdateWorldState(index, value);
    PSendSysMessage("WorldState %u is %u for player.", index, value);
    return true;
}

bool ChatHandler::HandleVariableCommand(char* args)
{
    uint32 index = 0;
    uint32 value = 0;
    sscanf(args, "%u %u", &index, &value);
    if (!index)
        return false;

    if (value)
    {
        uint32 oldValue = sObjectMgr.GetSavedVariable(index, 666);
        sObjectMgr.SetSavedVariable(index, value, true);
        if (oldValue == 666) // La valeur par default
            PSendSysMessage("New variable %u set to %u", index, value);
        else
            PSendSysMessage("Variable %u changed from %u to %u", index, oldValue, value);
    }
    else
    {
        value = sObjectMgr.GetSavedVariable(index, 666);
        if (value == 666)
            PSendSysMessage("Not found variable %u", index);
        else
            PSendSysMessage("Variable %u value is %u", index, value);
    }
    return true;
}

bool ChatHandler::HandleReloadVariablesCommand(char*)
{
    sObjectMgr.LoadSavedVariable();
    SendSysMessage("Table `variables` has been reloaded.");
    return true;
}


bool ChatHandler::HandleGoForwardCommand(char* args)
{
    float x, y, z;
    float add = 10.0f;
    sscanf(args, "%f", &add);
    if (Player* pPlayer = m_session->GetPlayer())
    {
        pPlayer->GetRelativePositions(add, 0.0f, 0.0f, x, y, z);
        pPlayer->NearLandTo(x, y, z, pPlayer->GetOrientation());
    }
    return true;
}

bool ChatHandler::HandleGoUpCommand(char* args)
{
    float x, y, z;
    float add_z = 10.0f;
    sscanf(args, "%f", &add_z);
    if (Player* pPlayer = m_session->GetPlayer())
    {
        pPlayer->GetRelativePositions(0.0f, 0.0f, add_z, x, y, z);
        pPlayer->NearLandTo(x, y, z, pPlayer->GetOrientation());
    }
    return true;
}


/*
CREATE TABLE `characters_guid_delete` (
  `guid` int(11) default nullptr,
  UNIQUE KEY `guid` (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

CREATE TABLE `characters_item_delete` (
  `entry` int(11) default nullptr,
  UNIQUE KEY `entry` (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;
*/
bool ChatHandler::HandleCleanCharactersToDeleteCommand(char* args)
{
    QueryResult* toDeleteCharsResult = CharacterDatabase.Query("SELECT guid FROM characters_guid_delete;");
    if (!toDeleteCharsResult)
    {
        SendSysMessage("Table 'characters_guid_delete' is empty or does not exist.");
        SetSentErrorMessage(true);
        return false;
    }
    else
    {
        uint32 deleteCount = 0;
        Field* fields;
        do
        {
            fields = toDeleteCharsResult->Fetch();
            uint32 guid = fields[0].GetUInt32();

            Player::DeleteFromDB(guid,
                0, // AccountID
                false, // Update realm characters count
                true // Delete finally
            );

            ++deleteCount;
        } while (toDeleteCharsResult->NextRow());
        PSendSysMessage("%u characters have been deleted.", deleteCount);
        delete toDeleteCharsResult;
    }
    return true;
}


bool ChatHandler::HandleCleanCharactersItemsCommand(char* args)
{
    bool Real = false;
    if (m_session->GetSecurity() == SEC_CONSOLE)
        Real = true;

    QueryResult* listDeleteItems = CharacterDatabase.Query("SELECT entry FROM characters_item_delete;");
    if (!listDeleteItems)
    {
        SendSysMessage("Cannot find items to delete. Table 'characters_item_delete' is empty ?");
        SetSentErrorMessage(true);
        return false;
    }
    std::vector<uint32> lDeleteEntries;

    uint32 deleteCount = 0;
    Field* fields;
    do
    {
        fields = listDeleteItems->Fetch();
        uint32 entry = fields[0].GetUInt32();

        lDeleteEntries.push_back(entry);

        ++deleteCount;
    } while (listDeleteItems->NextRow());
    PSendSysMessage("%u items to delete.", lDeleteEntries.size());
    delete listDeleteItems;

    QueryResult* allPlayersItems = CharacterDatabase.Query("SELECT guid, itemEntry, owner_guid FROM item_instance;");
    if (!allPlayersItems)
    {
        SendSysMessage("Unable to retrieve player items list.");
        SetSentErrorMessage(true);
        return false;
    }
    deleteCount = 0;
    uint32 totalItems = 0;
    std::vector<uint32>::iterator itr;
    CharacterDatabase.BeginTransaction();
    do
    {
        ++totalItems;
        fields = allPlayersItems->Fetch();
        uint32 item_guid = fields[0].GetUInt32();
        uint32 item_entry = fields[1].GetUInt32();
        uint32 owner_guid = fields[2].GetUInt32();

        for (itr = lDeleteEntries.begin(); itr != lDeleteEntries.end(); ++itr)
        {
            if ((*itr) == item_entry)
            {
                if (Real)
                {
                    // InGame ?
                    if (Player* pPlayer = sObjectMgr.GetPlayer(owner_guid))
                        pPlayer->DestroyItemCount(item_entry, 255, true);
                    else
                        Item::DeleteAllFromDB(item_guid);
                }
                ++deleteCount;
                break;
            }
        }

    } while (allPlayersItems->NextRow());
    CharacterDatabase.CommitTransaction();

    SendSysMessage("==== Statistiques ====");
    PSendSysMessage("- %u items entries", totalItems);
    PSendSysMessage("- %u items deleted", deleteCount);
    if (!Real)
        SendSysMessage("-> Not executed. (for security purposes).");
    delete allPlayersItems;
    return true;
}

bool ChatHandler::HandleCharacterFillFlysCommand(char* args)
{
    if (Player* player = GetSelectedPlayer())
    {
        if (player->GetTeam() == ALLIANCE)
            player->GetTaxi().LoadTaxiMask("3456411898 2148078928 49991 0 0 0 0 0 ");
        else
            player->GetTaxi().LoadTaxiMask("561714688 282102432 52408 0 0 0 0 0 ");
        PSendSysMessage("Fly paths unlocked for %s.", player->GetName());
        return true;
    }
    return false;
}

bool ChatHandler::HandleMmapsNearCommand(char* args)
{
    MMAP::MMapManager* mmap = MMAP::MMapFactory::createOrGetMMapManager();
    MMAP::NavMeshQueryHandle queryHandle = mmap->AcquireNavMeshQuery(GetSession()->GetPlayer()->GetMapId());
    if (!queryHandle)
    {
        PSendSysMessage("NavMesh not loaded for current map.");
        return true;
    }

    float dist = 0.0f;

    if (!ExtractFloat(&args, dist))
        return false;

    auto playerpos = GetSession()->GetPlayer()->GetPosition();
    float extents[3] = { dist, 15.f, dist };
    float point[3] = { playerpos.y, playerpos.z, playerpos.x};

    dtQueryFilter filter;
    filter.setExcludeFlags(NAV_STEEP_SLOPES);
    std::vector<dtPolyRef> refs;
    refs.resize(4000);
    dtNavMeshQuery const* query = queryHandle.get();
    int polyCount = 0;
    if (dtStatusFailed(query->queryPolygons(point, extents, &filter, refs.data(), &polyCount, refs.size())))
        SendSysMessage("Error querying polygons.");

    refs.resize(polyCount);

    std::vector<G3D::Vector3> visualPoints;
    visualPoints.reserve(refs.size());
    for (const auto& poly : refs)
    {
        float pointRes[3] = {};
        bool res = false;
        if (dtStatusFailed(query->closestPointOnPoly(poly, point, pointRes, &res)))
            continue;

        visualPoints.emplace_back(pointRes[2], pointRes[0], pointRes[1]);
    }

    // Summoning can enter map/grid code; no Detour refs are needed anymore.
    queryHandle = MMAP::NavMeshQueryHandle();
    for (G3D::Vector3 const& visualPoint : visualPoints)
        GetSession()->GetPlayer()->SummonCreature(VISUAL_WAYPOINT, visualPoint.x, visualPoint.y, visualPoint.z, 0, TEMPSUMMON_TIMED_DESPAWN, 30000, false, 0, nullptr, false);

    return true;
}

bool ChatHandler::HandleFactionAtWarCommand(char* args)
{
    auto player = GetSelectedPlayer();

    if (!player)
    {
        SendSysMessage("No Player selected.");
        return false;
    }

    uint32 factionId;
    bool onOff;

    if (!ExtractUInt32(&args, factionId) || !ExtractOnOff(&args, onOff))
    {
        SendSysMessage("Wrong Syntax. Syntax: <factionId> on/off");
        return false;
    }

    if (!player->GetReputationMgr().SetAtWar(factionId, onOff))
    {
        SendSysMessage("Setting at war failed.");
        return false;
    }

    player->SendFactionAtWar(factionId, onOff);
    return true;

}


bool ChatHandler::HandleMmapsPathCommand(char* args)
{
    if (!MMAP::MMapFactory::createOrGetMMapManager()->IsNavMeshLoaded(GetSession()->GetPlayer()->GetMapId()))
    {
        PSendSysMessage("NavMesh not loaded for current map.");
        return true;
    }

    SendSysMessage("mmap path:");

    // units
    Player* player = GetSession()->GetPlayer();
    Unit* target = GetSelectedUnit();
    if (!player || !target)
    {
        PSendSysMessage("Invalid target/source selection.");
        return true;
    }

    char* para = strtok((char*)args, " ");

    bool useStraightPath = false;
    if (para && strcmp(para, "true") == 0)
        useStraightPath = true;

    bool useRaycast = false;
    if (para && (strcmp(para, "line") == 0 || strcmp(para, "ray") == 0 || strcmp(para, "raycast") == 0))
        useRaycast = true;

    // unit locations
    float x, y, z;
    player->GetPosition(x, y, z);


    // path
    PathFinder path(target);
    path.setUseStrightPath(useStraightPath);
    bool result = path.calculate(x, y, z, false);

    Movement::PointsArray const& pointPath = path.getPath();
    PSendSysMessage("%s's path to %s:", target->GetName(), player->GetName());
    PSendSysMessage("Building: %s", useStraightPath ? "StraightPath" : useRaycast ? "Raycast" : "SmoothPath");
    PSendSysMessage("Result: %s - Length: %zu - Type: %u", (result ? "true" : "false"), pointPath.size(), path.getPathType());

    G3D::Vector3 const& start = path.getStartPosition();
    G3D::Vector3 const& end = path.getEndPosition();
    G3D::Vector3 const& actualEnd = path.getActualEndPosition();

    PSendSysMessage("StartPosition     (%.3f, %.3f, %.3f)", start.x, start.y, start.z);
    PSendSysMessage("EndPosition       (%.3f, %.3f, %.3f)", end.x, end.y, end.z);
    PSendSysMessage("ActualEndPosition (%.3f, %.3f, %.3f)", actualEnd.x, actualEnd.y, actualEnd.z);

    if (!player->IsGameMaster())
        PSendSysMessage("Enable GM mode to see the path points.");

    for (uint32 i = 0; i < pointPath.size(); ++i)
        player->SummonCreature(VISUAL_WAYPOINT, pointPath[i].x, pointPath[i].y, pointPath[i].z, 0, TEMPSUMMON_TIMED_DESPAWN, 9000);

    return true;
}

bool ChatHandler::HandleShellcoinCommand(char* args)
{
    uint32 sellprice = sObjectMgr.GetShellCoinSellPrice();
    uint32 buyprice = sObjectMgr.GetShellCoinBuyPrice();

    PSendSysMessage("Shellcoin buy price : %uG,%uS,%uC", buyprice / GOLD, (buyprice % GOLD) / SILVER, (buyprice % GOLD) % SILVER);
    PSendSysMessage("Shellcoin sell price : %uG,%uS,%uC", sellprice / GOLD, (sellprice % GOLD) / SILVER, (sellprice % GOLD) % SILVER);
    return true;
}

bool ChatHandler::HandleNpcGroupAddCommand(char* args)
{
    if (!*args)
        return false;

    Creature* target = GetSelectedCreature();
    SetSentErrorMessage(true);

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        return false;
    }

    uint32 leaderGuidCounter = 0;
    uint32 options = OPTION_FORMATION_MOVE | OPTION_AGGRO_TOGETHER | OPTION_EVADE_TOGETHER | OPTION_RESPAWN_TOGETHER;
    if (!ExtractUInt32(&args, leaderGuidCounter))
        return false;
    ExtractUInt32(&args, options);
    Creature* leader = target->GetMap()->GetCreature(CreatureGroupsManager::ConvertDBGuid(leaderGuidCounter));
    if (!leader)
    {
        PSendSysMessage("Leader not found");
        return false;
    }
    if (target->GetCreatureGroup())
    {
        SendSysMessage("Selected creature is already member of a group.");
        return false;
    }

    bool dbsave = target->HasStaticDBSpawnData();
    Player* chr = m_session->GetPlayer();
    float angle = (chr->GetAngle(target) - target->GetOrientation()) + 2 * M_PI_F;
    float dist = sqrtf(pow(chr->GetPositionX() - target->GetPositionX(), int(2)) + pow(chr->GetPositionY() - target->GetPositionY(), int(2)));

    CreatureGroup* group = leader->GetCreatureGroup();
    if (!group)
        group = new CreatureGroup(leader->GetObjectGuid());
    group->AddMember(target->GetObjectGuid(), dist, angle, options);
    target->SetCreatureGroup(group);
    leader->SetCreatureGroup(group);
    target->GetMotionMaster()->Initialize();
    if (dbsave)
    {
        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        group->SaveToDb();
    } 
    PSendSysMessage("Group added for creature %u. Leader %u, Angle %f, Dist %f", target->GetGUIDLow(), leader->GetGUIDLow(), angle, dist);
    return true;
}

bool ChatHandler::HandleNpcGroupAddRelCommand(char* args)
{
    if (!*args)
        return false;

    Creature* target = GetSelectedCreature();
    SetSentErrorMessage(true);

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        return false;
    }

    uint32 leaderGuidCounter = 0;
    uint32 options = OPTION_FORMATION_MOVE | OPTION_AGGRO_TOGETHER | OPTION_EVADE_TOGETHER | OPTION_RESPAWN_TOGETHER;
    if (!ExtractUInt32(&args, leaderGuidCounter))
        return false;
    ExtractUInt32(&args, options);
    Creature* leader = target->GetMap()->GetCreature(CreatureGroupsManager::ConvertDBGuid(leaderGuidCounter));
    if (!leader)
    {
        PSendSysMessage("Leader not found");
        return false;
    }
    if (target->GetCreatureGroup())
    {
        SendSysMessage("Selected creature is already member of a group.");
        return false;
    }

    bool dbsave = target->HasStaticDBSpawnData();
    //Player *chr = m_session->GetPlayer();
    float angle = target->GetAngle(leader);//(chr->GetAngle(target) - target->GetOrientation()) + 2 * M_PI_F;
    float dist = sqrtf(pow(leader->GetPositionX() - target->GetPositionX(), int(2)) + pow(leader->GetPositionY() - target->GetPositionY(), int(2)));

    CreatureGroup* group = leader->GetCreatureGroup();
    if (!group)
        group = new CreatureGroup(leader->GetObjectGuid());
    group->AddMember(target->GetObjectGuid(), dist, angle, options);
    target->SetCreatureGroup(group);
    leader->SetCreatureGroup(group);
    target->GetMotionMaster()->Initialize();
    if (dbsave)
    {
        sWorld.GetMigration().SetAuthor(m_session->GetUsername());
        group->SaveToDb();
    }
    PSendSysMessage("Group added for creature %u. Leader %u, Angle %f, Dist %f", target->GetGUIDLow(), leader->GetGUIDLow(), angle, dist);
    return true;
}

bool ChatHandler::HandleNpcGroupDelCommand(char* args)
{
    Creature* target = GetSelectedCreature();
    SetSentErrorMessage(true);

    if (!target || !target->HasStaticDBSpawnData())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        return false;
    }

    CreatureGroup* g = target->GetCreatureGroup();
    if (!g)
    {
        PSendSysMessage("%s [GUID=%u] is not in a group.", target->GetName(), target->GetGUIDLow());
        return false;
    }

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (g->GetOriginalLeaderGuid() == target->GetObjectGuid())
    {
        g->DeleteFromDb();
        target->LeaveCreatureGroup(); // group is deleted
        g = nullptr;
    }
    else
    {
        target->LeaveCreatureGroup();
        g->SaveToDb();
    }

    target->GetMotionMaster()->Initialize();
    return true;
}

bool ChatHandler::HandleNpcGroupLinkCommand(char* args)
{
    if (!*args)
        return false;

    Creature* target = GetSelectedCreature();
    SetSentErrorMessage(true);

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        return false;
    }

    uint32 options;
    uint32 leaderGuidCounter = 0;
    if (!ExtractUInt32(&args, leaderGuidCounter))
        return false;

    ExtractUInt32(&args, options);

    Creature* leader = target->GetMap()->GetCreature(CreatureGroupsManager::ConvertDBGuid(leaderGuidCounter));
    if (!leader)
    {
        PSendSysMessage("Leader not found");
        return false;
    }

    WorldDatabase.PExecute("DELETE FROM `creature_linking` WHERE `guid`=%u", target->GetGUIDLow());
    WorldDatabase.PExecute("INSERT INTO `creature_linking` SET `guid`=%u, `master_guid`=%u, `flag`='%u'",
        target->GetGUIDLow(), leaderGuidCounter, options);

    PSendSysMessage("creature_link for creature %u. Leader %u", target->GetGUIDLow(), leader->GetGUIDLow());
    return true;
}

bool ChatHandler::HandleReloadCreatureGroupsCommand(char* args)
{
    sCreatureGroupsManager.Load();
    SendSysMessage("DB table `creature_groups` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAreaTriggerTeleportCommand(char* /*args*/)
{
    sObjectMgr.LoadAreaTriggerTeleports();
    SendSysMessage("DB table `areatrigger_teleport` reloaded.");
    return true;
}

bool ChatHandler::HandleSendSpellVisualCommand(char* args)
{
    Unit* pTarget = GetSelectedUnit();
    if (!pTarget)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 uiPlayId = 0;
    char* playIdStr = strtok(args, " ");
    if (playIdStr)
        uiPlayId = uint32(atoi(playIdStr));
    SpellEntry const* proto = sSpellMgr.GetSpellEntry(uiPlayId);
    if (!proto)
    {
        SendSysMessage(LANG_COMMAND_NOSPELLFOUND);
        SetSentErrorMessage(true);
        return false;
    }
    PSendSysMessage("Spell %u visual on target '%s'.", uiPlayId, pTarget->GetName());

    WorldPacket data(SMSG_PLAY_SPELL_VISUAL, 8 + 4);
    data << uint64(m_session->GetPlayer()->GetGUID());
    data << uint32(uiPlayId);                                // spell visual id?
    pTarget->SendMessageToSet(&data, true);
    m_session->GetPlayer()->SendSpellGo(pTarget, uiPlayId);

    // Channeled case
    if (proto->IsChanneledSpell())
    {
        m_session->GetPlayer()->SetUInt32Value(UNIT_CHANNEL_SPELL, uiPlayId);
        m_session->GetPlayer()->SetChannelObjectGuid(pTarget->GetObjectGuid());
    }
    return true;
}

bool ChatHandler::HandleCastCustomCommand(char* args)
{
    if (!*args)
        return false;

    Unit* target = GetSelectedUnit();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    // Only allow admins to cast spells in other players
    if (GetSession()->GetSecurity() < SEC_ADMINISTRATOR)
        target = GetSession()->GetPlayer();

    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r or Htalent form
    uint32 spell = ExtractSpellIdFromLink(&args);
    if (!spell)
        return false;

    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spell);
    if (!spellInfo)
        return false;

    if (!SpellMgr::IsSpellValid(spellInfo, m_session->GetPlayer()))
    {
        PSendSysMessage(LANG_COMMAND_SPELL_BROKEN, spell);
        SetSentErrorMessage(true);
        return false;
    }

    int32 basepoint0, basepoint1, basepoint2;
    if (!ExtractOptInt32(&args, basepoint0, -1) || !ExtractOptInt32(&args, basepoint1, -1) || !ExtractOptInt32(&args, basepoint2, -1))
    {
        PSendSysMessage("Bad syntax.");
        return false;
    }

    m_session->GetPlayer()->CastCustomSpell(target, spell, basepoint0 == -1 ? nullptr : &basepoint0, basepoint1 == -1 ? nullptr : &basepoint1, basepoint2 == -1 ? nullptr : &basepoint2, true);

    return true;
}

bool ChatHandler::HandleSendSpellImpactCommand(char* args)
{
    Unit* pTarget = GetSelectedUnit();
    if (!pTarget)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 uiPlayId = 0;
    char* playIdStr = strtok(args, " ");
    if (playIdStr)
        uiPlayId = uint32(atoi(playIdStr));
    PSendSysMessage("Spell %u impact on target '%s'.", uiPlayId, pTarget->GetName());

    WorldPacket data(SMSG_PLAY_SPELL_IMPACT, 8 + 4);
    data << uint64(pTarget->GetGUID());
    data << uint32(uiPlayId);                                // spell visual id?
    pTarget->SendMessageToSet(&data, true);
    return true;
}

// BG

#define COLOR_HORDE      "FF3300"
#define COLOR_ALLIANCE   "0066B3"
#define COLOR_BG         "D580FE"
#define COLOR_INFO       "FF9900"
#define COLOR_STATUS     "FECCBF"

//#define DO_LINK(b,a)     "|Hnostalrius:0|h"a"|h"
#define DO_COLOR(a, b)   "|cff" a "" b "|r"

typedef std::map<ObjectGuid, BattleGroundPlayer> BattleGroundPlayerMap;
bool ChatHandler::HandleBGStatusCommand(char* args)
{
    Player* chr = m_session->GetPlayer();
    ASSERT(chr);
    SendSysMessage(DO_COLOR(COLOR_INFO, "-- Currently running BGs"));
    uint8 i = 0;
    uint8 uiAllianceCount, uiHordeCount;
    for (int8 bgTypeId = BATTLEGROUND_AB; bgTypeId >= BATTLEGROUND_AV; --bgTypeId)
    {
        for (BattleGroundSet::const_iterator it = sBattleGroundMgr.GetBattleGroundsBegin(BattleGroundTypeId(bgTypeId)); it != sBattleGroundMgr.GetBattleGroundsEnd(BattleGroundTypeId(bgTypeId)); ++it)
        {
            // Pas un "vrai" BG, mais un "modele" de BG.
            if (!it->first)
                continue;

            ++i;
            uiAllianceCount = 0;
            uiHordeCount = 0;
            BattleGroundPlayerMap const& pPlayers = it->second->GetPlayers();
            std::string playerName;

            for (const auto& itr : pPlayers)
            {
                if (itr.second.PlayerTeam == HORDE)
                    uiHordeCount++;
                else
                    uiAllianceCount++;
                if (playerName.empty())
                    if (sObjectMgr.GetPlayerNameByGUID(itr.first, playerName))
                        playerName = playerLink(playerName);
            }

            std::string statusName;
            BattleGroundStatus status = it->second->GetStatus();
            switch (status)
            {
            case STATUS_WAIT_JOIN:
                statusName = "WaitJoin";
                break;
            case STATUS_IN_PROGRESS:
                statusName = "InProgress";
                break;
            case STATUS_WAIT_LEAVE:
                statusName = "WaitLeave";
                break;
            }

            PSendSysMessage(DO_COLOR(COLOR_BG, "[%s %2u]") " [%2u-%2u] "
                DO_COLOR(COLOR_STATUS, "[%s]")
                DO_COLOR(COLOR_ALLIANCE, "%2u") "vs" DO_COLOR(COLOR_HORDE, "%2u")
                " Player:%s %s",
                it->second->GetName(), it->first, it->second->GetMinLevel(), it->second->GetMaxLevel(), statusName.c_str(),
                uiAllianceCount, uiHordeCount,
                playerName.c_str(), secsToTimeString(it->second->GetStartTime() / 1000, true).c_str());
        }
    }
    if (!i)
        PSendSysMessage(DO_COLOR(COLOR_INFO, "(No battleground started)"));

    PSendSysMessage(DO_COLOR(COLOR_INFO, "-- Queues for your bracket"));
    i = 0;

    for (uint8 bgTypeId = BATTLEGROUND_AV; bgTypeId < MAX_BATTLEGROUND_TYPE_ID; ++bgTypeId)
    {
        // Skip disabled arenas.
        if (bgTypeId == BATTLEGROUND_BR)
            continue;

        ++i;
        uiAllianceCount = 0;
        uiHordeCount = 0;

        BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(BattleGroundTypeId(bgTypeId));
        // Doit etre une référence (&), sinon crash par la suite ...
        BattleGroundQueue& queue = sBattleGroundMgr.m_BattleGroundQueues[bgQueueTypeId];
        for (const auto& itr : queue.m_QueuedPlayers)
        {
            if (itr.second.GroupInfo->GroupTeam == HORDE)
                uiHordeCount++;
            else
                uiAllianceCount++;
        }

        BattleGround* bg_template = sBattleGroundMgr.GetBattleGroundTemplate(BattleGroundTypeId(bgTypeId));
        ASSERT(bg_template);

        PSendSysMessage(DO_COLOR(COLOR_BG, "[%s]" "   " DO_COLOR(COLOR_ALLIANCE, "[Alliance] : %2u") " - " DO_COLOR(COLOR_HORDE, "[Horde] : %2u")),
            bg_template->GetName(), uiAllianceCount, uiHordeCount);
    }
    if (!i)
        PSendSysMessage(DO_COLOR(COLOR_INFO, "(No player queued)"));
    return true;
}

bool ChatHandler::HandleBGStartCommand(char* args)
{
    Player* chr = m_session->GetPlayer();
    ASSERT(chr);
    BattleGround* pBg = chr->GetBattleGround();
    if (!pBg)
    {
        SendSysMessage("You must be in a battleground to use this command.");
        SetSentErrorMessage(true);
        return false;
    }
    pBg->SetStartDelayTime(0);
    PSendSysMessage("BG commence [%s][%u]", pBg->GetName(), pBg->GetInstanceID());
    return true;
}

bool ChatHandler::HandleBGStopCommand(char* args)
{
    Player* chr = m_session->GetPlayer();
    ASSERT(chr);
    BattleGround* pBg = chr->GetBattleGround();
    if (!pBg)
    {
        SendSysMessage("You are not in a battleground");
        SetSentErrorMessage(true);
        return false;
    }
    pBg->StopBattleGround();
    PSendSysMessage("Battleground stopped [%s][%u]", pBg->GetName(), pBg->GetInstanceID());
    return true;
}

bool ChatHandler::HandleBGCustomCommand(char* args)
{
    Player* chr = m_session->GetPlayer();
    ASSERT(chr);
    BattleGround* pBg = chr->GetBattleGround();
    if (!pBg)
    {
        SendSysMessage("You are not in a battleground");
        SetSentErrorMessage(true);
        return false;
    }
    pBg->HandleCommand(chr, this, args);
    return true;

}

bool ChatHandler::HandleGodCommand(char* args)
{
    Player* pPlayer = GetSelectedPlayer();
    if (!pPlayer)
        pPlayer = m_session->GetPlayer();

    if (*args)
    {
        bool value;
        if (!ExtractOnOff(&args, value))
        {
            SendSysMessage(LANG_USE_BOL);
            SetSentErrorMessage(true);
            return false;
        }
        pPlayer->SetInvincibilityHpThreshold(value ? 1 : 0);
    }
    if (pPlayer->GetInvincibilityHpThreshold())
        SendSysMessage("Mode GOD : [ON]");
    else
        SendSysMessage("Mode GOD : [OFF]");
    return true;
}

bool ChatHandler::HandleGMOptionsCommand(char* args)
{
    if (!args)
        return false;

    std::string sArgs(args);
    bool enable = true;
    uint32 flags = 0;
    if (sArgs.find("off") != std::string::npos)
        enable = false;
    if (sArgs.find("OFF") != std::string::npos)
        enable = false;
    if (sArgs.find("CD") != std::string::npos || sArgs.find("cd") != std::string::npos || sArgs.find("cooldown") != std::string::npos || sArgs.find("COOLDOWN") != std::string::npos)
        flags |= PLAYER_CHEAT_NO_COOLDOWN;
    if (sArgs.find("CAST") != std::string::npos || sArgs.find("cast") != std::string::npos)
        flags |= PLAYER_CHEAT_NO_CAST_TIME;
    if (sArgs.find("SPEED") != std::string::npos || sArgs.find("speed") != std::string::npos)
        flags |= PLAYER_CHEAT_NO_MOD_SPEED;
    if (sArgs.find("power") != std::string::npos || sArgs.find("POWER") != std::string::npos || sArgs.find("mana") != std::string::npos || sArgs.find("MANA") != std::string::npos)
        flags |= PLAYER_CHEAT_NO_POWER;
    if (sArgs.find("crit") != std::string::npos || sArgs.find("CRIT") != std::string::npos)
        flags |= PLAYER_CHEAT_ALWAYS_CRIT;
    if (sArgs.find("checkcast") != std::string::npos || sArgs.find("CHECKCAST") != std::string::npos || sArgs.find("check") != std::string::npos || sArgs.find("CHECK") != std::string::npos)
        flags |= PLAYER_CHEAT_NO_CHECK_CAST;
    if (sArgs.find("proc") != std::string::npos || sArgs.find("PROC") != std::string::npos)
        flags |= PLAYER_CHEAT_ALWAYS_PROC;
    if (sArgs.find("video") != std::string::npos || sArgs.find("VIDEO") != std::string::npos)
        flags |= PLAYER_VIDEO_MODE;

    if (sArgs.find("spellcr") != std::string::npos || sArgs.find("SPELLCR") != std::string::npos)
        flags |= PLAYER_CHEAT_ALWAYS_SPELL_CRIT;

    if (sArgs.find("nodmgrng") != std::string::npos || sArgs.find("NODMGRNG") != std::string::npos)
        flags |= PLAYER_CHEAT_NO_DAMAGE_RNG;

    Player* pTarget = GetSelectedPlayer();
    if (!pTarget)
        pTarget = m_session->GetPlayer();

    PSendSysMessage("%s flags 0x%x for `%s`.", enable ? "Adding" : "Removing", flags, pTarget->GetName());

    if (enable)
        pTarget->EnableOption(flags);
    else
        pTarget->RemoveOption(flags);

    if (sArgs.find("GOD") != std::string::npos || sArgs.find("god") != std::string::npos)
        pTarget->SetInvincibilityHpThreshold(enable ? 1 : 0);

    if (flags & PLAYER_VIDEO_MODE)
    {
        UpdateMask m;
        m.SetCount(UNIT_END);
        m.SetBit(UNIT_FIELD_FLAGS);
        pTarget->RefreshBitsForVisibleUnits(&m, TYPEMASK_UNIT);
        m.SetCount(PLAYER_END);
        m.SetBit(UNIT_FIELD_FLAGS);
        pTarget->RefreshBitsForVisibleUnits(&m, TYPEMASK_PLAYER);
    }
    return true;
}

bool ChatHandler::HandleFreezeCommand(char* args)
{
    Unit* pTarget = GetSelectedUnit();
    if (!pTarget || pTarget == GetPlayer())
    {
        Player* pPlayer = nullptr;
        if (!ExtractPlayerTarget(&args, &pPlayer))
            return false;

        PSendSysMessage("Freezing %s.", GetNameLink(pPlayer).c_str());
        pTarget = pPlayer;
    }

    Unit* pCaster = GetPlayer()->IsInMap(pTarget) ? GetPlayer() : pTarget;
    pCaster->CastSpell(pTarget, 9454, true);

    return true;
}

bool ChatHandler::HandleUnfreezeCommand(char* args)
{
    Unit* pTarget = GetSelectedUnit();
    if (!pTarget || pTarget == GetPlayer())
    {
        Player* pPlayer = nullptr;
        if (!ExtractPlayerTarget(&args, &pPlayer))
            return false;

        PSendSysMessage("Unfreezing %s.", GetNameLink(pPlayer).c_str());
        pTarget = pPlayer;
    }
    
    pTarget->RemoveAurasDueToSpell(9454);
    return true;
}


bool ChatHandler::HandleUnitStatCommand(char* args)
{
    Unit* pTarget = GetSelectedUnit();
    if (!pTarget)
        return false;
    uint32 unitStat = 0x0;
    for (int i = 1; i < UNIT_STAT_IGNORE_PATHFINDING; i *= 2)
        if (pTarget->HasUnitState(i))
            unitStat |= i;
    PSendSysMessage("UnitState = 0x%x (%u)", unitStat, unitStat);
    if (ExtractUInt32(&args, unitStat))
    {
        pTarget->ClearUnitState(UNIT_STAT_ALL_STATE);
        pTarget->AddUnitState(unitStat);
        PSendSysMessage("UnitState changed to 0x%x (%u)", unitStat, unitStat);
    }
    return true;
}

bool ChatHandler::HandleAnticheatCommand(char* args)
{
    Player* player = nullptr;
    if (!ExtractPlayerTarget(&args, &player) && m_session)
        player = m_session->GetPlayer();
    if (!player)
        return false;

    PSendSysMessage("Cheat report on player '%s' (GUID %u)", player->GetName(), player->GetGUIDLow());
    /*if (player->GetCheatData())
        player->GetCheatData()->HandleCommand(this);*/

    return true;
}

bool ChatHandler::HandleWardenCommand(char* args)
{
    /*Player* player = nullptr;
    if (!ExtractPlayerTarget(&args, &player) && m_session)
        player = m_session->GetPlayer();
    if (!player)
        return false;

    WardenInterface* warden = player->GetSession()->GetWarden();
    if (!warden)
    {
        PSendSysMessage("No Warden loaded for account %s", player->GetSession()->GetUsername().c_str());
        return true;
    }
    warden->HandleInfoCommand(this);*/
    return true;
}

/*class WardenCommandReadQuery: public WardenMemoryQuery
{
public:
    WardenCommandReadQuery(uint32 addr, uint32 length, uint32 accountId) : WardenMemoryQuery(addr, length), _accountId(accountId)
    {
    }
    void DataRead(uint8 const* data, WardenInterface* warden)
    {
        WorldSession* sess = sWorld.FindSession(_accountId);
        if (!sess)
            return;
        ChatHandler handler(sess);
        handler.PSendSysMessage("Warden read at 0x%x", GetAddress());
        if (!data)
        {
            handler.SendSysMessage("Read failed!");
            return;
        }
        if (GetLength() == 4)
        {
            handler.PSendSysMessage("Read (uint32): 0x%8x", *((uint32*)data));
            return;
        }
        std::stringstream readUint8;
        for (uint32 i = 0; i < GetLength(); ++i)
            readUint8 << uint32(data[i]) << " ";
        handler.PSendSysMessage("Read (uint8*): %s", readUint8.str().c_str());
        handler.PSendSysMessage("Read (string): %s", std::string((const char*)data, 0, GetLength()).c_str());
    }
protected:
    uint32 _accountId;
};*/

bool ChatHandler::HandleWardenReadCommand(char* args)
{
    /*Player* player = GetSelectedPlayer();
    if (!player)
        return false;

    uint32 addr, len = 4;
    if (!ExtractUInt32Base(&args, addr, 0x10))
        return false;
    ExtractUInt32(&args, len);
    WardenInterface* warden = player->GetSession()->GetWarden();
    if (!warden)
    {
        PSendSysMessage("No Warden loaded for account %s", player->GetSession()->GetUsername().c_str());
        return true;
    }
    warden->AddMemoryQuery(new WardenCommandReadQuery(addr, len, GetAccountId()));*/
    return true;
}

bool ChatHandler::HandleListAddonsCommand(char* args)
{
    Player* player = GetSelectedPlayer();
    if (!player)
        return false;

    std::set<std::string> const& addons = player->GetSession()->GetAddons();
    PSendSysMessage("%u addons on target.", addons.size());
    for (const auto& addon : addons)
        PSendSysMessage(">> %s", addon.c_str());

    return true;
}

bool ChatHandler::HandleClientInfosCommand(char* args)
{
    Player* player = nullptr;

    if (!ExtractPlayerTarget(&args, &player) && m_session)
        player = m_session->GetPlayer();

    if (!player)
        return false;

    PSendSysMessage("Account %s has %u client identifiers.", player->GetSession()->GetUsername().c_str(), player->GetSession()->GetClientIdentifiers().size());
    for (const auto& it : player->GetSession()->GetClientIdentifiers())
        PSendSysMessage("%u: %s", it.first, it.second.c_str());

    player->GetSession()->ComputeClientHash();
    PSendSysMessage("Hash is %s", playerLink(player->GetSession()->GetClientHash()).c_str());
    return true;
}

bool ChatHandler::HandleClientSearchCommand(char* args)
{
    ASSERT(args);
    std::string searchedHash = args;
    uint32 i = 0;
    const World::SessionMap& sessMap = sWorld.GetAllSessions();
    for (const auto& itr : sessMap)
    {
        if (!itr.second)
            continue;

        std::string currentHash = itr.second->GetClientHash();
        if (currentHash.find(searchedHash) != std::string::npos)
        {
            PSendSysMessage("%s on account %s, %s",
                playerLink(itr.second->GetPlayerName()).c_str(),
                playerLink(itr.second->GetUsername()).c_str(),
                playerLink(itr.second->GetRemoteAddress()).c_str());
            ++i;
        }
    }
    if (i == 0)
        PSendSysMessage("Not result for hash %s", playerLink(searchedHash).c_str());
    else
        PSendSysMessage("%u result(s) for %s", i, playerLink(searchedHash).c_str());
    return true;
}

bool IsSimilarItem(ItemPrototype const* proto1, ItemPrototype const* proto2)
{
    for (int i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        if (proto1->ItemStat[i].ItemStatType != proto2->ItemStat[i].ItemStatType ||
            proto1->ItemStat[i].ItemStatValue != proto2->ItemStat[i].ItemStatValue)
            return false;
    if (proto1->Quality != proto2->Quality)
        return false;
    if (proto1->Class != proto2->Class)
        return false;
    if (proto1->SubClass != proto2->SubClass)
        return false;
    if (proto1->InventoryType != proto2->InventoryType)
        return false;
    if (proto1->Armor != proto2->Armor)
        return false;
    if (proto1->AllowableClass != proto2->AllowableClass)
        return false;
    return true;
}

bool ChatHandler::HandleFactionChangeItemsCommand(char* c)
{
    for (auto const& itr : sObjectMgr.GetItemPrototypeMap())
    {
        uint32 id = itr.first;
        ItemPrototype const* proto1 = &itr.second;
        if (!proto1)
            continue;
        Races currMountRace;
        uint8 currRaceNum = 0;
        if (sObjectMgr.GetMountDataByEntry(id, currMountRace, currRaceNum))
            continue;
        if (proto1->Quality <= 1)
            continue;
        bool inDb = false;
        for (std::map<uint32, uint32>::const_iterator it2 = sObjectMgr.factionchange_items.begin(); it2 != sObjectMgr.factionchange_items.end(); ++it2)
        {
            if ((it2->first == id) || (it2->second == id))
            {
                inDb = true;
                break;
            }
        }
        if (inDb)
            continue;

        bool canEquip = true;
        if ((proto1->AllowableRace & RACEMASK_ALLIANCE) == 0 || (proto1->AllowableRace & RACEMASK_HORDE) == 0)
            canEquip = false;
        if (proto1->RequiredHonorRank != 0 &&
            (proto1->Class == ITEM_CLASS_WEAPON || proto1->Class == ITEM_CLASS_ARMOR))
            canEquip = false;

        if (!canEquip)
        {
            ItemPrototype const* similar = nullptr;
            for (auto const& itr2 : sObjectMgr.GetItemPrototypeMap())
                if (ItemPrototype const* proto2 = &itr2.second)
                    if (proto1 != proto2 && IsSimilarItem(proto1, proto2))
                    {
                        if (similar)
                        {
                            // Ambiguity. Other similar items.
                            similar = nullptr;
                            break;
                        }
                        similar = proto2;
                    }


            PSendSysMessage("Item %u not handled ! Similar item : %u", proto1->ItemId, similar ? similar->ItemId : 0);
        }
    }
    return true;
}


bool ChatHandler::HandlePetListCommand(char* args)
{
    std::string charName(args);
    normalizePlayerName(charName);
    ObjectGuid playerGuid = sObjectMgr.GetPlayerGuidByName(charName);
    if (!playerGuid)
    {
        SendSysMessage(LANG_NON_EXIST_CHARACTER);
        SetSentErrorMessage(true);
        return false;
    }

    CharPetMap const& petsMap = sCharacterDatabaseCache.GetCharPetsMap();
    CharPetMap::const_iterator charPets = petsMap.find(playerGuid.GetCounter());
    uint32 count = 0;
    if (charPets != petsMap.end())
    {
        for (const auto it : charPets->second)
        {
            PSendSysMessage("#%u: \"%s\" (%s)", it->id, it->name.c_str(), it->slot == PET_SAVE_AS_CURRENT ? "Current pet" : "In stable");
            ++count;
        }
    }

    PSendSysMessage("Found %u pets for character %s (#%u).", count, charName.c_str(), playerGuid.GetCounter());
    return true;
}

bool ChatHandler::HandlePetRenameCommand(char* args)
{
    uint32 petId;
    std::string newName;
    if (!ExtractUInt32(&args, petId))
        return false;
    newName = args;
    PetNameInvalidReason res = ObjectMgr::CheckPetName(newName);
    if (res != PET_NAME_SUCCESS)
    {
        PSendSysMessage("\"%s\" is not a valid pet name", newName.c_str());
        SetSentErrorMessage(true);
        return false;
    }

    std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT owner, name FROM character_pet WHERE id = %u", petId));

    if (!result)
    {
        PSendSysMessage("Pet #%u not found", petId);
        SetSentErrorMessage(true);
        return false;
    }

    auto fields = result->Fetch();
    auto owner_guid = fields[0].GetUInt32();
    auto pet_name = fields[1].GetString();

    PSendSysMessage("Pet #%u (\"%s\", owner #%u) renamed to \"%s\"", petId, pet_name, owner_guid, newName.c_str());
    CharacterDatabase.escape_string(newName);
    CharacterDatabase.PExecute("UPDATE character_pet SET name = \"%s\" WHERE id = %u", newName.c_str(), petId);

    CharacterPetCache* petData = sCharacterDatabaseCache.GetCharacterPetById(petId);

    if (petData)
    {
        petData->name = newName;
    }

    return true;
}

bool ChatHandler::HandlePetLoyaltyCommand(char* args)
{
    if (!*args)
        return false;

    Pet* pet = GetSelectedPet();
    if (!pet)
        return false;

    if (pet->getPetType() != HUNTER_PET)
        return false;

    int32 loyaltyPoints;
    if (!ExtractOptInt32(&args, loyaltyPoints, 1))
        return false;

    pet->ModifyLoyalty(loyaltyPoints);

    return true;
}

bool ChatHandler::HandlePetLearnSpellCommand(char* args)
{
    if (!*args)
        return false;

    Pet* pet = GetSelectedPet();
    if (!pet)
        return false;

    int32 spellId;
    if (!ExtractOptInt32(&args, spellId, 1))
        return false;

    if (pet->LearnSpell(spellId))
        PSendSysMessage("Added spell %u to pet %s.", spellId, pet->GetName());
    else
        PSendSysMessage("Failed to add spell %u to pet %s.", spellId, pet->GetName());

    return true;
}

bool ChatHandler::HandlePetUnlearnSpellCommand(char* args)
{
    if (!*args)
        return false;

    Pet* pet = GetSelectedPet();
    if (!pet)
        return false;

    int32 spellId;
    if (!ExtractOptInt32(&args, spellId, 1))
        return false;

    if (pet->unlearnSpell(spellId, false, true))
        PSendSysMessage("Removed spell %u from pet %s.", spellId, pet->GetName());
    else
        PSendSysMessage("Failed to remove spell %u from pet %s.", spellId, pet->GetName());

    return true;
}

bool ChatHandler::HandlePetDeleteCommand(char* args)
{
    uint32 petId;
    if (!ExtractUInt32(&args, petId))
        return false;
    CharacterPetCache* petData = sCharacterDatabaseCache.GetCharacterPetById(petId);
    if (!petData)
    {
        PSendSysMessage("Pet #%u not found", petId);
        SetSentErrorMessage(true);
        return false;
    }
    PSendSysMessage("Pet #%u (\"%s\", owner #%u) deleted.", petData->id, petData->name.c_str(), petData->owner);
    CharacterDatabase.PExecute("DELETE FROM character_pet WHERE id = %u", petId);
    sCharacterDatabaseCache.DeleteCharacterPetById(petId);
    return true;
}

bool ChatHandler::HandleInstanceContinentsCommand(char*)
{
    if (Player* target = GetSelectedPlayer())
        PSendSysMessage("Target: %s, map %u instance %u", target->GetName(), target->GetMapId(), target->GetInstanceId());

    auto pl = GetPlayer();

    for (int mapId = 0; mapId < 2; ++mapId)
    {
        PSendSysMessage("MAP %u", mapId);
        for (int i = 0; i < 20; ++i)
            if (Map* m = sMapMgr.FindMap(mapId, i))
            {
                Map::PlayerList const& players = m->GetPlayers();
                if (players.begin() == players.end())
                    continue;
                Map::PlayerList::const_iterator it = players.begin();
                int count = 0;
                while (it != players.end())
                {
                    ++count;
                    ++it;
                }
                PSendSysMessage("[Instance%2u] %u players, dist visible:%.1f activate:%.1f", i, count, m->GetVisibilityDistance(), m->GetGridActivationDistance());

                if (pl)
                {
                    auto dynVis = sDynamicVisMgr.GetDynamicVisibility(pl->GetAreaId());
                    if (dynVis)
                        PSendSysMessage("Current dynamic visibility for this area: %u.", dynVis.value());
                }
            }
    }
    return true;
}

bool ChatHandler::HandleReloadDynamicVisibilityCommand(char*)
{
    sDynamicVisMgr.LoadFromDB(true);
    SendSysMessage("Dynamic Visibility Templates Reloaded.");
    return true;
}

bool ChatHandler::HandleInstancePerfInfosCommand(char* args)
{
    Player* player = GetSession()->GetPlayer();
    Map* map = player->FindMap();
    if (!map)
        return false;
    map->PrintInfos(*this);
    uint32 playersInClient = 0, gobjsInClient = 0, unitsInClient = 0, corpsesInClient = 0;
    std::shared_lock<std::shared_mutex> lock(player->m_visibleGUIDs_lock);
    for (const ObjectGuid& itr : player->m_visibleGUIDs)
    {
        switch (itr.GetHigh())
        {
        case HIGHGUID_PLAYER: ++playersInClient; break;
        case HIGHGUID_GAMEOBJECT: ++gobjsInClient; break;
        case HIGHGUID_UNIT: ++unitsInClient; break;
        case HIGHGUID_CORPSE: ++corpsesInClient; break;
        }
    }
    PSendSysMessage("Units in client: %u pl, %u gobj, %u crea, %u corpses", playersInClient, gobjsInClient, unitsInClient, corpsesInClient);
    return true;
}


// Turtle WoW custom commands

class DemorphAfterTime : public BasicEvent {
public:
    explicit DemorphAfterTime(uint64 player_guid) : BasicEvent(), player_guid(player_guid) {}

    bool Execute(uint64 e_time, uint32 p_time) override {
        Player* player = ObjectAccessor::FindPlayer(player_guid);
        if (player) {
            player->DeMorph();
        }
        return false;
    }

private:
    uint64 player_guid;
};


bool ChatHandler::HandleReloadShopCommand(char* args)
{
    sObjectMgr.LoadShop();
    SendSysMessage("DB table `shop_categories` reloaded.");
    SendSysMessage("DB table `shop_entries` reloaded.");
    return true;
}

bool ChatHandler::HandleMailboxCommand(char* args)
{
    if (!m_session || !m_session->GetPlayer())
        return false;

    /*
    WorldPacket pckt;
    pckt << m_session->GetPlayer()->GetGUID();
    m_session->HandleGetMailList(pckt);
    */

    //asked in Discord, known to be correct
    constexpr uint32 spellIdToMarkShittyDespawnCode = 13358832;
    auto player = m_session->GetPlayer();

    uint32 allyMailboxEntry = 144131;
    uint32 hordeMailboxEntry = 173221;

    auto pos = player->GetPosition();
    auto gob = m_session->GetPlayer()->SummonGameObject(player->GetTeamId() == TEAM_ALLIANCE ? allyMailboxEntry : hordeMailboxEntry,
        pos.x + 1.0f, pos.y + 1.0f, pos.z + 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 60, true);

    
    return true;
}

bool ChatHandler::HandleSendMailsCommand(char* args)
{
    Player* receiver;
    ObjectGuid receiver_guid;
    std::string receiver_name;
    if (!ExtractPlayerTarget(&args, &receiver, &receiver_guid, &receiver_name))
        return false;
    uint32 count = atoi(args);
    while (count-- > 0)
    {
        if (Item* item = Item::CreateItem(25, 1, m_session ? m_session->GetPlayer() : 0))
        {
            item->SaveToDB();                               // save for prevent lost at next mail load, if send fail then item will deleted
            

            MailDraft(std::to_string(count), "TEST").AddItem(item).SendMailTo(MailReceiver(receiver, receiver_guid), MailSender());
        }
    }
    return false;
}

bool ChatHandler::HandleBalanceCommand(char* args)
{
    char* c_account_name = ExtractQuotedOrLiteralArg(&args);

    if (!c_account_name)
        return false;

    std::string account_name = c_account_name;

    if (!AccountMgr::normalizeString(account_name))
    {
        PSendSysMessage("Wrong account name.");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 account_id;
    account_id = ExtractAccountId(&c_account_name, &account_name, nullptr, false);
    int32 coinsArg = 0;
    ExtractInt32(&args, coinsArg);

    if (!account_id)
        return false;

    int32 currentCoins = GetTokenBalance(account_id);

    if (!coinsArg)
    {
        PSendSysMessage("Current coins on account %s is : %i", account_name.c_str(), currentCoins);
        return true;
    }

    int64 updated_balance = currentCoins + coinsArg;

    if (updated_balance < 0 || updated_balance > INT_MAX)
    {
        PSendSysMessage("Can't go below zero, the current balance is %i.", currentCoins);
        return false;
    }

    LoginDatabase.PExecute("UPDATE `shop_coins` SET `coins`=`coins`+%i WHERE `id`=%u", coinsArg, account_id);
    PSendSysMessage("You've successfully added %i coins to %s.", coinsArg, account_name.c_str());
    PSendSysMessage("Account %s now has %i coins.", account_name.c_str(), updated_balance);
    return true;

    return false;
}

bool ChatHandler::HandlePlayCommand(char* args)
{
    int32 id = (int32)atoi(args);

    if (!*args)
    {
        SendSysMessage("Syntax: .play sound_id");
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->PlayDirectMusic(id);
    return true;
}

bool ChatHandler::HandleRadioCommand(char* args)
{
    char* cmd = ExtractLiteralArg(&args);

    if (!cmd)
        return false;

    std::string cmd_str(cmd);

    if (cmd_str == "1")
    {
        m_session->GetPlayer()->PlayDirectMusic(60401, m_session->GetPlayer()); 
        return true;
    }
    else if (cmd_str == "2")
    {
        m_session->GetPlayer()->PlayDirectMusic(60423, m_session->GetPlayer());
        // m_session->GetPlayer()->PlayDirectMusic(60424, m_session->GetPlayer()); // static
        return true;
    }
    return false;
}

bool ChatHandler::HandleBgTestCommand(char* args)
{
    sBattleGroundMgr.ToggleTesting();
    return true;
}

bool ChatHandler::HandleModifyXpCommand(char* args)
{
    if (!*args)
    {
        SendSysMessage("Syntax: .givexp value, where [value] is experience.");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 XP = (uint32)atoi(args);
    Player* target = GetSelectedPlayer();

    if (!target)
        target = m_session->GetPlayer();

    target->GiveXP(XP, target);
    return true;
}

// This is the first id in the dbc and exists in all clients.
#define DISPLAY_ID_BOX 4

bool ChatHandler::HandleModifyHairStyleCommand(char* args)
{
    if (!*args)
        return false;

    uint8 hairstyle = (uint8)atoi(args);
    Player* target = GetSelectedPlayer();

    if (!target)
        target = m_session->GetPlayer();

    target->SetByteValue(PLAYER_BYTES, 2, hairstyle);
    target->UpdateAppearance();

    PSendSysMessage("Character's hair style has been changed to: %u", hairstyle);
    return true;
}

bool ChatHandler::HandleModifyHairColorCommand(char* args)
{
    if (!*args)
        return false;

    uint8 haircolor = (uint8)atoi(args);
    Player* target = GetSelectedPlayer();

    if (!target)
        target = m_session->GetPlayer();

    target->SetByteValue(PLAYER_BYTES, 3, haircolor);
    target->UpdateAppearance();

    PSendSysMessage("Character's hair color has been changed to: %u", haircolor);
    return true;
}

bool ChatHandler::HandleModifySkinColorCommand(char* args)
{
    if (!*args)
        return false;

    uint8 skincolor = (uint8)atoi(args);
    Player* target = GetSelectedPlayer();

    if (!target)
        target = m_session->GetPlayer();

    target->SetByteValue(PLAYER_BYTES, 0, skincolor);
    target->UpdateAppearance();

    PSendSysMessage("Character's skin color has been changed to: %u", skincolor);
    return true;
}

bool ChatHandler::HandleModifyFaceCommand(char* args)
{
    if (!*args)
        return false;

    uint8 face = (uint8)atoi(args);
    Player* target = GetSelectedPlayer();

    if (!target)
        target = m_session->GetPlayer();

    target->SetByteValue(PLAYER_BYTES, 1, face);
    target->UpdateAppearance();

    PSendSysMessage("Character's face has been changed to: %u", face);
    return true;
}

bool ChatHandler::HandleModifyAccessoriesCommand(char* args)
{
    if (!*args)
        return false;

    uint8 accessories = (uint8)atoi(args);
    Player* target = GetSelectedPlayer();

    if (!target)
        target = m_session->GetPlayer();

    target->SetByteValue(PLAYER_BYTES_2, 0, accessories);
    target->UpdateAppearance();

    PSendSysMessage("Character's facial hair / markings / hooves have been changed to: %u", accessories);
    return true;
}

bool ChatHandler::HandleModifyTitleCommand(char* args)
{
    // For humans, Protector of Stormwind
    // For orcs, Overlord of Orgrimmar
    // For dwarves, Thane of Ironforge
    // For night elves, High Sentinel of Darnassus
    // For undead, Deathlord of the Undercity
    // For taurens, Chieftain of Thunderbluff
    // For gnomes, Avenger of Gnomeregan
    // For trolls, Voodoo Boss of Sen'jin

    int32 titleId = (int32)atoi(args);
    Player* target = GetSelectedPlayer();

    if (!target)
    {
        PSendSysMessage("You must select a player.");
        return false;
    }

    target->ToPlayer()->AwardTitle(titleId);
    return true;
}

bool ChatHandler::HandleTransferCommand(char* args)
{
    if (!args || !*args)
        return false;

    char* rawPlName = ExtractArg(&args);
    char* rawAccountName = ExtractArg(&args);
    if (!rawPlName || !rawAccountName)
    {
        SendSysMessage("Syntax: .transfer player_name account_name");
        return false;
    }


    std::string playerName(rawPlName);
    std::string accountName(rawAccountName);

    auto playerCache = sObjectMgr.GetPlayerDataByName(playerName);

    CharacterDatabase.escape_string(playerName);
    CharacterDatabase.escape_string(accountName);

    // Player must be offline
    // This will only return the player if it's ONLINE
    if (ObjectAccessor::FindPlayerByName(playerName.c_str()))
    {
        SendSysMessage("Player must be offline.");
        return false;
    }
    else
    {
        QueryResult* result = CharacterDatabase.PQuery("SELECT `guid` FROM `characters` WHERE `name` = '%s'",
            playerName.c_str());

        if (!result)
        {
            SendSysMessage("Player not found.");
            return false;
        }
        else
        {
            Field* fields = result->Fetch();
            uint32 guid = fields[0].GetUInt32();

            if (guid == 1)
            {
                SendSysMessage("Nope, can't move Torta.");
                return false;
            }

            uint32 accountId = sAccountMgr.GetId(accountName);

            // Account must exist
            if (!accountId)
            {
                SendSysMessage("Account not found.");
                return false;
            }

            CharacterDatabase.PExecute("UPDATE characters SET account = %u WHERE guid = '%u'", accountId, guid);
            if (playerCache)
                playerCache->uiAccount = GetAccountId();

            PSendSysMessage("You have successfully moved character %s to account %s.", playerName.c_str(),
                accountName.c_str());
        }
    }
    return true;
}

bool ChatHandler::HandleQuestTestCommand(char* args)
{
    auto player = GetSession()->GetPlayer();
    player->SetByteValue(UNIT_FIELD_BYTES_0, 0, RACE_HUMAN);
    GetSession()->GetPlayer()->SetFactionForRace(RACE_HUMAN);
    player->ForceValuesUpdateAtIndex(UNIT_FIELD_FACTIONTEMPLATE);
    player->DirectSendPublicValueUpdate(UNIT_FIELD_FACTIONTEMPLATE);
    FactionStateList const& targetFSL = player->GetReputationMgr().GetStateList();
    for (auto& itr : targetFSL)
    {      
        uint32 count = 1;

        auto faction = &itr.second;

        WorldPacket data(SMSG_SET_FACTION_STANDING, (16));
        size_t p_count = data.wpos();
        data << (uint32)count;

        data << (uint32)faction->ReputationListID;
        data << (uint32)80000;

        data.put<uint32>(p_count, count);
        player->SendDirectMessage(&data);
        player->SendFactionAtWar(faction->ReputationListID, false);
    }


    PlayerInfo const* info = sObjectMgr.GetPlayerInfo(RACE_HUMAN, GetSession()->GetPlayer()->GetClass());
    if (!info)
        return false;

    uint8 gender = GetSession()->GetPlayer()->GetGender();

    switch (gender)
    {
    case GENDER_FEMALE:
        player->SetByteValue(PLAYER_BYTES, 0, 1);
        player->SetByteValue(PLAYER_BYTES, 1, 1);
        player->SetByteValue(PLAYER_BYTES, 2, 1);
        player->SetByteValue(PLAYER_BYTES, 3, 1);
        player->SetDisplayId(info->displayId_f);
        player->SetNativeDisplayId(info->displayId_f);

        break;
    case GENDER_MALE:
        player->SetByteValue(PLAYER_BYTES, 0, 1);
        player->SetByteValue(PLAYER_BYTES, 1, 1);
        player->SetByteValue(PLAYER_BYTES, 2, 1);
        player->SetByteValue(PLAYER_BYTES, 3, 1);
        player->SetDisplayId(info->displayId_m);
        player->SetNativeDisplayId(info->displayId_m);

        break;
    }
    //player->GetMap()->ExistingPlayerLogin(player);
    return true;
}

bool ChatHandler::HandleXPCommand(char* args)
{
    auto player = GetSession()->GetPlayer();
    if (!player || !player->IsInWorld())
        return false;

    bool onOff = false;
    if (!ExtractOnOff(&args, onOff))
    {
        PSendSysMessage("XP gain is %s", player->HasXPGainEnabled() ? "ON" : "OFF");
        return true;
    }


    player->SetXPGain(onOff);

    PSendSysMessage("XP gain is now %s", onOff ? "ON" : "OFF");

    return true;
}

bool ChatHandler::HandleReloadBgAmounts(char* args)
{
    sBattleGroundMgr.ReloadBGPlayerCounts();
    SendSysMessage("Player counts reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGmLevelsCommand(char* args)
{
    CommandStream stream{ args };
    std::string x;
    stream >> x;

    if (x.empty())
        sAccountMgr.LoadGmLevels();
    else
        sAccountMgr.Load();
    SendSysMessage("GM account levels reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGmTicketTemplatesCommand(char* args)
{
    sTicketMgr.LoadTicketTemplates();
    SendSysMessage("GM ticket templates reloaded.");
    return true;
}

bool ChatHandler::HandleMountCommand(char* /*args*/)
{
    Player* player = m_session->GetPlayer();
    Creature* target = GetSelectedCreature();

    uint32 modelid;

    if (player->IsTaxiFlying())
        return false;

    if (!target)
    {
        player->Unmount();
        player->UpdateSpeed(MOVE_RUN, false, 1.0F);
        PSendSysMessage("Nothing to ride on! Target any creature with mounting points.");
        SetSentErrorMessage(true);
        return false;
    }

    modelid = target->GetUInt32Value(UNIT_FIELD_DISPLAYID);
    player->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, modelid);
    player->UpdateSpeed(MOVE_RUN, false, 4.0F);
    return true;
}

bool ChatHandler::HandleTurtleCinematic(char* args)
{
    uint32 CinematicId = 0;
    if (!ExtractUInt32(&args, CinematicId))
    {
        PSendSysMessage("Usage: .cinematic $cinematicInt$");
        return false;
    }

    if (Player* CurrentPlayer = GetSession()->GetPlayer())
    {
        CurrentPlayer->SendCinematicStart(CinematicId);
    }

    return true;
}

bool ChatHandler::HandlePetNameCommand(char* args)
{
    if (Player* CurrentPlayer = GetSession()->GetPlayer())
    {
        if (CurrentPlayer->GetClass() != CLASS_HUNTER)
        {
            m_session->SendNotification("Only the Hunters can name their pets!");
            SetSentErrorMessage(true);
            return false;
        }

        if (!CurrentPlayer->HasItemCount(80555))
        {
            m_session->SendNotification("You don't have [Pet Name Change Token].");
            SetSentErrorMessage(true);
            return false;
        }

        Pet* pet = CurrentPlayer->GetPet();

        if (!pet)
        {
            m_session->SendNotification("Please summon your pet first.");
            SetSentErrorMessage(true);
            return false;
        }

        if (pet->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_RENAME))
        {
            m_session->SendNotification("Pet renaming is already enabled and do so by right-clicking on your portrait.");
            SetSentErrorMessage(true);
            return false;
        }

        pet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_RENAME);
        m_session->SendNotification("Success! Rename your pet by right-clicking on your portrait.");
        CurrentPlayer->DestroyItemCount(80555, 1, true, false, true);
        CurrentPlayer->SaveInventoryAndGoldToDB();
        return true;
    }
    return false;

    return false;
    // Manual way if you don't like flag solution:

    //if (Player* CurrentPlayer = GetSession()->GetPlayer())
    //{
    //    ObjectGuid playerGuid = CurrentPlayer->GetGUID();

    //    if (!CurrentPlayer->GetClass() == CLASS_HUNTER)
    //    {
    //        m_session->SendNotification("Only Hunters can name their pets.");
    //        SetSentErrorMessage(true);
    //        return false;
    //    }

    //    if (!CurrentPlayer->HasItemCount(80555))
    //    {
    //        m_session->SendNotification("You don't have [Pet Name Change Token].");
    //        SetSentErrorMessage(true);
    //        return false;
    //    }     

    //    CharPetMap const& petsMap = sCharacterDatabaseCache.GetCharPetsMap();
    //    CharPetMap::const_iterator charPets = petsMap.find(playerGuid.GetCounter());
    //    uint32 count = 0;
    //    if (charPets != petsMap.end())
    //    {
    //        for (const auto it : charPets->second)
    //        {
    //            if (it->slot == PET_SAVE_AS_CURRENT)
    //            {
    //                uint32 petId = it->id;

    //            // PSendSysMessage("DEBUG: PetID: %u.", petId);

    //                if (!petId)
    //                {
    //                    PSendSysMessage("Please summon your pet first.");
    //                    SetSentErrorMessage(true);
    //                    return false;
    //                }

    //                std::string newName;
    //                newName = args;

    //                PetNameInvalidReason res = ObjectMgr::CheckPetName(newName);
    //                if (res != PET_NAME_SUCCESS)
    //                {
    //                    PSendSysMessage("\"%s\" is not a valid pet name.", newName.c_str());
    //                    SetSentErrorMessage(true);
    //                    return false;
    //                }

    //                std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT owner, name FROM character_pet WHERE id = %u", petId));

    //                if (!result)
    //                {
    //                    PSendSysMessage("Something went wrong, please contact a Gamemaster!", petId);
    //                    SetSentErrorMessage(true);
    //                    return false;
    //                }

    //                auto fields = result->Fetch();
    //                auto pet_name = fields[1].GetString();

    //                PSendSysMessage("Pet %s renamed to %s.", pet_name, newName.c_str());
    //                CharacterDatabase.escape_string(newName);
    //                CharacterDatabase.PExecute("UPDATE character_pet SET name = \"%s\" WHERE id = %u", newName.c_str(), petId);

    //                CharacterPetCache* petData = sCharacterDatabaseCache.GetCharacterPetById(petId);

    //                if (petData)
    //                    petData->name = newName;

    //                Pet* pet = CurrentPlayer->GetPet();
    //                pet->SetName(newName);

    //                CurrentPlayer->DestroyItemCount(80555, 1, true, false, true);
    //                CurrentPlayer->SaveInventoryAndGoldToDB();

    //                return true;

    //            }
    //            else
    //                ++count;
    //        }
    //    }       
    //    return false;
    //}  

    return false;
}

bool ChatHandler::HandleGuildNameCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();

    if (!pPlayer)
        return false;

    if (!pPlayer->GetGuildId() || !pPlayer->GetRank() == GR_GUILDMASTER)
    {
        m_session->SendNotification("You must be a guild master of your guild to use this service!");
        return false;
    }

    if (!pPlayer->HasItemCount(80499))
    {
        m_session->SendNotification("You must purchase [Guild Name Change Token] first.");
        SetSentErrorMessage(true);
        return false;
    }

    if (!args || !*args)
        return false;

    char* new_name = ExtractQuotedArg(&args);

    if (!new_name)
        return false;

    std::string name_str(new_name);
    std::wstring name_wstr;
    Utf8toWStr(name_str, name_wstr);

    if (name_wstr.size() == 0)
    {
        PSendSysMessage("Guild name must be at least 1 character long.");
        return false;
    }

    auto invalidCharacter = false;
    if (sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK))
    {
        for (wchar_t chr : name_wstr)
        {
            if (std::iswalpha(chr) == 0 && chr != ' ')
            {
                invalidCharacter = true;
                break;
            }
        }
    }
    else
    {
        for (unsigned const char chr : name_str)
        {
            if (std::isalpha(chr) == 0 && chr != ' ')
            {
                invalidCharacter = true;
                break;
            }
        }
    }

    if (invalidCharacter)
    {
        PSendSysMessage("Guild name contains invalid characters.");
        return false;
    }

    Guild* guild = sGuildMgr.GetGuildById(pPlayer->GetGuildId());

    if (!guild)
    {
        SendSysMessage(LANG_GUILD_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (Guild* existing = sGuildMgr.GetGuildByName(name_str))
    {
        m_session->SendNotification("A guild with the name '%s' already exists.", new_name);
        SetSentErrorMessage(true);
        return false;
    }

    guild->Rename(name_str);
    PSendSysMessage("Your guild has been successfully renamed. Players must log out and log back in to see the changes.");

    pPlayer->DestroyItemCount(80499, 1, true, false, true);
    pPlayer->SaveInventoryAndGoldToDB();
    return true;
}

bool ChatHandler::HandleCopyCommand(char* args)
{
    if (!args || !*args)
    {
        PSendSysMessage("You must specify the name of the character you want to look like.");
        SetSentErrorMessage(true);
        return false;
    }

    if (Player* target = m_session->GetPlayer())
    {
        if (!target->HasItemCount(80699))
        {
            PSendSysMessage("You must purchase [Appearance Change Token] first.");
            SetSentErrorMessage(true);
            return false;
        }

        uint8 curr_race = target->GetRace();
        std::string plName = args;
        CharacterDatabase.escape_string(plName);

        std::wstring wPlName;

        if (!Utf8toWStr(plName, wPlName))
            return false;

        wstrToLower(wPlName);

        if (Utf8FitTo(target->GetName(), wPlName))
        {
            PSendSysMessage("You must specify a name of a character different than yourself!");
            SetSentErrorMessage(true);
            return false;
        }

        if (target->IsInCombat() || target->InBattleGround() || target->HasSpellCooldown(20939) || (target->GetDeathState() == CORPSE) || target->IsBeingTeleported())
        {
            PSendSysMessage("You can not change your appearance yet.");
            SetSentErrorMessage(true);
            return false;
        }

        auto result = std::unique_ptr<QueryResult>{ CharacterDatabase.PQuery("SELECT race, playerBytes, playerBytes2 & 0xFF, gender FROM characters WHERE name='%s'", plName.c_str()) };

        if (!result)
        {
            PSendSysMessage("You must specify the name of the character you want to look like.");
            SetSentErrorMessage(true);
            return false;
        }

        Field* fields = result->Fetch();
        uint8 race = fields[0].GetUInt8();
        uint32 bytes = fields[1].GetUInt32();
        uint32 bytes2 = fields[2].GetUInt32();
        uint8 gender = fields[3].GetUInt8();

        if (race != curr_race)
        {
            PSendSysMessage("It should be a character of the same race.");
            SetSentErrorMessage(true);
            return false;
        }
        else
        {
            bytes2 |= (target->GetUInt32Value(PLAYER_BYTES_2) & 0xFFFFFF00);
            target->SetUInt32Value(PLAYER_BYTES, bytes);
            target->SetUInt32Value(PLAYER_BYTES_2, bytes2);
            target->SetByteValue(UNIT_FIELD_BYTES_0, 2, gender);
            target->DestroyItemCount(80699, 1, true, false, true);
            target->SaveInventoryAndGoldToDB();
            target->SetDisplayId(15435); // Invisible
            target->m_Events.AddEvent(new DemorphAfterTime(target->GetGUID()), target->m_Events.CalculateTime(250));
            PSendSysMessage("Appearance successfully changed to player %s. Please log out and back in.", plName.c_str());
            return true;
        }
    }
    return false;
}

bool ChatHandler::HandleShopRefundCommand(char* args)
{
    uint32 shopId;

    if (!ExtractOptUInt32(&args, shopId, 1))
    {
        SendSysMessage("No Shop ID specified.");
        return false;
    }

    auto entry = sObjectMgr.GetShopLogEntry(shopId);

    if (!entry)
    {
        SendSysMessage("Could not find shop entry for given ID.");
        return false;
    }

    if (entry->refunded)
    {
        PSendSysMessage("ID %u is already refunded.", entry->id);
        return false;
    }



    entry->refunded = true;
    LoginDatabase.PExecute("UPDATE `shop_logs` SET `refunded` = 1 WHERE `id` = %u", shopId);
    PSendSysMessage("Shop ID %u for player %u marked as refunded. Do not forget to remove items or restore balance if applicable.", entry->id, entry->charGuid);
    return true;
}

bool ChatHandler::HandleToggleInfernoModeCommand(char* args)
{
    auto player = GetSelectedPlayer();

    if (!player)
        return false;


    bool isInferno = player->IsHC60();
    player->SetHardcoreStatus(isInferno ? (player->GetLevel() == 60 ? HARDCORE_MODE_STATUS_IMMORTAL : HARDCORE_MODE_STATUS_ALIVE) : HARDCORE_MODE_STATUS_HC60);

    PSendSysMessage("Inferno mode is now %s", isInferno ? "OFF" : "ON");
    return true;
}


bool ChatHandler::HandleTranslateCommand(char* args)
{
    char* accountStr = ExtractOptNotLastArg(&args);

    auto res = sChatTranslator->Translate("hehe", "ich liebe dich");
    return true;
}

bool ChatHandler::HandleStopApiServerCommand(char* args)
{
    sWorld.StopHttpApiServer();
    SendSysMessage("OK. HTTP API server stopped.");
    return true;
}


bool ChatHandler::HandleToggleTrainingCommand(char* args)
{

    //disable command for trainees.
    if (sAccountMgr.IsTraineeGM(GetAccountId()))
        return false;

    char* accountStr = ExtractOptNotLastArg(&args);

    std::string targetAccountName;
    Player* targetPlayer = nullptr;
    uint32 targetAccountId = ExtractAccountId(&accountStr, &targetAccountName, &targetPlayer);
    if (!targetAccountId)
        return false;

    bool isTrainee = sAccountMgr.IsTraineeGM(targetAccountId);

    if (targetPlayer)
    {
        if (isTrainee) // toggle off
            targetPlayer->GetSession()->SetSecurity(SEC_OBSERVER);
        else // toggle on
            targetPlayer->GetSession()->SetSecurity(SEC_DEVELOPER);
    }

    isTrainee = !isTrainee;
    sAccountMgr.ToggleTraineeGM(targetAccountId);

    PSendSysMessage("Account %s trainee is now : %s", targetAccountName.c_str(), isTrainee ? "ON" : "OFF");

    return true;
}

bool ChatHandler::HandleGetShopLogs(char* args)
{

    uint32 accountId = 0;
    std::string account_name = "";
    char* c_account_name = ExtractArg(&args);

    if (!c_account_name)
    {
        auto selectedPlayer = GetSelectedPlayer();
        if (selectedPlayer)
        {
            accountId = selectedPlayer->GetSession()->GetAccountId();
            account_name = selectedPlayer->GetSession()->GetUsername();
        }
        else
            return false;
    }

    
    if (!accountId)
    {
         account_name = c_account_name;

        if (!AccountMgr::normalizeString(account_name))
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            SetSentErrorMessage(true);
            return false;
        }

        accountId = sAccountMgr.GetId(account_name);
        if (!accountId)
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            SetSentErrorMessage(true);
            return false;
        }
    }

    PSendSysMessage("Current tokens on accouns %s : %i", account_name.c_str(), GetTokenBalance(accountId));
    PSendSysMessage("Payment history for account %s", account_name.c_str());

    auto& entries = sObjectMgr.GetShopLogEntries(accountId);

    auto session = GetSession();

    if (entries.empty())
    {
        ChatHandler(session).PSendSysMessage("No payment history for account %s (ID: %u)", account_name.c_str(), accountId);
        return false;
    }

    auto timeNow = time(nullptr);
    const uint32 refundWindow = HOUR * 48; // 48 hour refund window.

    for (const auto& elem : entries)
    {
        std::string charName = "Unknown";

        auto cachedPlayerData = sObjectMgr.GetPlayerDataByGUID(elem->charGuid);

        if (cachedPlayerData)
            charName = cachedPlayerData->sName;

        std::string colorString = "";

        if (elem->refunded)
            colorString = "|cff0cbecf";
        else if (timeNow - refundWindow > elem->dateUnix) // refund window passed.
            colorString = "|cffb81d0f";

        ChatHandler(session).PSendSysMessage("%s%s | (ID %u) | %s (GUID:%u) spent %u tokens on item %u %s%s", colorString.c_str(), elem->date.c_str(),
            elem->id, charName.c_str(), elem->charGuid, elem->itemPrice, elem->itemEntry, elem->refunded ? "([REFUNDED])" : "", colorString.empty() ? "" : "|r");
    }
    return true;
}

bool ChatHandler::HandleCreaturePathSetup(char* /*args*/)
{
    Creature* target = GetSelectedCreature();

    if (!target)
    {
        SendSysMessage("No creature selected.");
        return false;
    }

    uint32 guid = target->GetDBTableGUIDLow();

    if (!guid)
    {
        SendSysMessage("No static DB data for this creature.");
        return false;
    }

    target->SetDefaultMovementType(IDLE_MOTION_TYPE);
    if (target->IsAlive())                         // Dead creature will reset movement generator at respawn
    {
        target->SetDeathState(JUST_DIED);
        target->Respawn();
    }
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    target->SaveToDB();
    target->GetMotionMaster()->Initialize();
    sWaypointMgr.DeletePath(target->GetGUIDLow());
    
    uint32 pointId = 0;
    if (!sWaypointMgr.AddNode(0, target->GetGUIDLow(), pointId, PATH_FROM_GUID, target->GetHomePosition().x, target->GetHomePosition().y, target->GetHomePosition().z))
    {
        SendSysMessage("Internal error at create path. Contact with admin.");
        return false;
    }

    PSendSysMessage("%s%s%u%s%u%s|r", "|cff00ff00", "PathID: |r|cff00ffff", target->GetGUIDLow(), "|r|cff00ff00: Waypoint |r|cff00ffff", pointId, "|r|cff00ff00 created. ");
    return true;
}

bool ChatHandler::HandleCreaturePathAddPoint(char* /*args*/)
{
    Creature* target = GetSelectedCreature();

    if (!target)
    {
        SendSysMessage("No creature selected.");
        return false;
    }

    uint32 guid = target->GetDBTableGUIDLow();

    if (!guid)
    {
        SendSysMessage("No static DB data for this creature.");
        return false;
    }

    uint32 pointId = 0;
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (!sWaypointMgr.AddNode(0, target->GetGUIDLow(), pointId, PATH_FROM_GUID, m_session->GetPlayer()->GetPositionX(), m_session->GetPlayer()->GetPositionY(), m_session->GetPlayer()->GetPositionZ()))
    {
        SendSysMessage("Internal error at add waypoint. Please use .path setup for clear waypoints and setup again");
        return false;
    }

    PSendSysMessage("%s%s%u%s%u%s|r", "|cff00ff00", "PathID: |r|cff00ffff", target->GetGUIDLow(), "|r|cff00ff00: Waypoint |r|cff00ffff", pointId, "|r|cff00ff00 added. ");
    return true;
}

bool ChatHandler::HandleCreaturePathLaunch(char* /*args*/)
{
    Creature* target = GetSelectedCreature();

    if (!target)
    {
        SendSysMessage("No creature selected.");
        return false;
    }

    uint32 guid = target->GetDBTableGUIDLow();

    if (!guid)
    {
        SendSysMessage("No static DB data for this creature.");
        return false;
    }

    if (!target->IsStopped())
    {
        SendSysMessage("Can't launch waypoint path for this creature. Please use setup.");
        return false;
    }

    QueryResult* result = WorldDatabase.PQuery("SELECT MAX(point) FROM creature_movement WHERE id = %u", guid);

    if (!result)
    {
        SendSysMessage("No start waypoint data for this creature.");
        return false;
    }

    delete result;

    target->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    target->SaveToDB();
    target->GetMotionMaster()->Initialize();
    target->MonsterSay("Path loaded.", 0);
    return true;
}

bool ChatHandler::HandleSendPacketCommand(char* args)
{
    //sWorld.SendQuestStatsInvalidate(8, m_session);

    /*if (const Quest* pQuest = sObjectMgr.GetQuestTemplate(8))
    {
        std::string Title, Details, Objectives, EndText;
        std::string ObjectiveText[QUEST_OBJECTIVES_COUNT];
        Title = /*pQuest->GetTitle()"Hui";
        Details = /*pQuest->GetDetails()"Hui";
        Objectives = pQuest->GetObjectives();
        EndText = pQuest->GetEndText();
        for (int i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
            ObjectiveText[i] = pQuest->ObjectiveText[i];

        WorldPacket data(SMSG_QUEST_QUERY_RESPONSE, 100);       // guess size

        data << uint32(pQuest->GetQuestId());                   // quest id
        data << uint32(pQuest->GetQuestMethod());               // Accepted values: 0, 1 or 2. 0==IsAutoComplete() (skip objectives/details)
        data << uint32(pQuest->GetQuestLevel());                // may be 0, static data, in other cases must be used dynamic level: Player::GetQuestLevelForPlayer
        data << uint32(pQuest->GetZoneOrSort());                // zone or sort to display in quest log

        data << uint32(pQuest->GetType());
        //[-ZERO] data << uint32(pQuest->GetSuggestedPlayers());

        data << uint32(pQuest->GetRepObjectiveFaction());       // shown in quest log as part of quest objective
        data << uint32(pQuest->GetRepObjectiveValue());         // shown in quest log as part of quest objective

        data << uint32(0);                                      // RequiredOpositeRepFaction
        data << uint32(0);                                      // RequiredOpositeRepValue, required faction value with another (oposite) faction (objective)

        data << uint32(pQuest->GetNextQuestInChain());          // client will request this quest from NPC, if not 0

        if (pQuest->HasQuestFlag(QUEST_FLAGS_HIDDEN_REWARDS))
            data << uint32(0);                                  // Hide money rewarded
        else
            data << uint32(pQuest->GetRewOrReqMoney());

        data << uint32(pQuest->GetRewMoneyMaxLevel());          // used in XP calculation at client
        data << uint32(pQuest->GetRewSpell());                  // reward spell, this spell will display (icon) (casted if RewSpellCast==0)

        data << uint32(pQuest->GetSrcItemId());                 // source item id
        data << uint32(pQuest->GetQuestFlags());                // quest flags

        int iI;

        if (pQuest->HasQuestFlag(QUEST_FLAGS_HIDDEN_REWARDS))
        {
            for (iI = 0; iI < QUEST_REWARDS_COUNT; ++iI)
                data << uint32(0) << uint32(0);
            for (iI = 0; iI < QUEST_REWARD_CHOICES_COUNT; ++iI)
                data << uint32(0) << uint32(0);
        }
        else
        {
            for (iI = 0; iI < QUEST_REWARDS_COUNT; ++iI)
            {
                data << uint32(pQuest->RewItemId[iI]);
                data << uint32(pQuest->RewItemCount[iI]);
            }
            for (iI = 0; iI < QUEST_REWARD_CHOICES_COUNT; ++iI)
            {
                data << uint32(pQuest->RewChoiceItemId[iI]);
                data << uint32(pQuest->RewChoiceItemCount[iI]);
            }
        }

        data << pQuest->GetPointMapId();
        data << pQuest->GetPointX();
        data << pQuest->GetPointY();
        data << pQuest->GetPointOpt();

        data << Title;
        data << Objectives;
        data << Details;
        data << EndText;

        for (iI = 0; iI < QUEST_OBJECTIVES_COUNT; ++iI)
        {
            if (pQuest->ReqCreatureOrGOId[iI] < 0)
            {
                // client expected gameobject template id in form (id|0x80000000)
                data << uint32((pQuest->ReqCreatureOrGOId[iI] * (-1)) | 0x80000000);
            }
            else
                data << uint32(pQuest->ReqCreatureOrGOId[iI]);
            data << uint32(pQuest->ReqCreatureOrGOCount[iI]);
            data << uint32(pQuest->ReqItemId[iI]);
            data << uint32(pQuest->ReqItemCount[iI]);
        }

        for (iI = 0; iI < QUEST_OBJECTIVES_COUNT; ++iI)
            data << ObjectiveText[iI];

        m_session->SendPacket(&data);
    }*/

    /*ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(37);
    if (pProto)
    {
        WorldPacket data(SMSG_ITEM_QUERY_SINGLE_RESPONSE, 600);
        data << pProto->ItemId;
        data << pProto->Class;
        // client known only 0 subclass (and 1-2 obsolute subclasses)
        data << (pProto->Class == ITEM_CLASS_CONSUMABLE ? uint32(0) : pProto->SubClass);
        data << "Dildo";                              // max length of any of 4 names: 256 bytes
        data << uint8(0x00);                                //pProto->Name2; // blizz not send name there, just uint8(0x00); <-- \0 = empty string = empty name...
        data << uint8(0x00);                                //pProto->Name3; // blizz not send name there, just uint8(0x00);
        data << uint8(0x00);                                //pProto->Name4; // blizz not send name there, just uint8(0x00);
        data << pProto->DisplayInfoID;
        data << pProto->Quality;
        data << pProto->Flags;
        data << pProto->BuyPrice;
        data << pProto->SellPrice;
        data << pProto->InventoryType;
        data << pProto->AllowableClass;
        data << pProto->AllowableRace;
        data << pProto->ItemLevel;
        data << pProto->RequiredLevel;
        data << pProto->RequiredSkill;
        data << pProto->RequiredSkillRank;
        data << pProto->RequiredSpell;
        // Item de style insigne
        if (pProto->Spells[0].SpellId != 0)
            data << uint32(0);
        else
            data << pProto->RequiredHonorRank;

        data << pProto->RequiredCityRank;
        data << pProto->RequiredReputationFaction;
        data << (pProto->RequiredReputationFaction > 0 ? pProto->RequiredReputationRank : 0);   // send value only if reputation faction id setted ( needed for some items)
        data << pProto->MaxCount;
        data << pProto->Stackable;
        data << pProto->ContainerSlots;
        for (int i = 0; i < MAX_ITEM_PROTO_STATS; ++i)
        {
            data << pProto->ItemStat[i].ItemStatType;
            data << pProto->ItemStat[i].ItemStatValue;
        }
        for (int i = 0; i < MAX_ITEM_PROTO_DAMAGES; ++i)
        {
            data << pProto->Damage[i].DamageMin;
            data << pProto->Damage[i].DamageMax;
            data << pProto->Damage[i].DamageType;
        }

        // resistances (7)
        data << pProto->Armor;
        data << pProto->HolyRes;
        data << pProto->FireRes;
        data << pProto->NatureRes;
        data << pProto->FrostRes;
        data << pProto->ShadowRes;
        data << pProto->ArcaneRes;

        data << pProto->Delay;
        data << pProto->AmmoType;
        data << (float)pProto->RangedModRange;

        for (int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
        {
            // send DBC data for cooldowns in same way as it used in Spell::SendSpellCooldown
            // use `item_template` or if not set then only use spell cooldowns
            SpellEntry const* spell = sSpellMgr.GetSpellEntry(pProto->Spells[s].SpellId);
            if (spell)
            {
                bool db_data = pProto->Spells[s].SpellCooldown >= 0 || pProto->Spells[s].SpellCategoryCooldown >= 0;

                data << pProto->Spells[s].SpellId;
                data << pProto->Spells[s].SpellTrigger;

                // let the database control the sign here.  negative means that the item should be consumed once the charges are consumed.
                data << pProto->Spells[s].SpellCharges;

                if (db_data)
                {
                    data << uint32(pProto->Spells[s].SpellCooldown);
                    data << uint32(pProto->Spells[s].SpellCategory);
                    data << uint32(pProto->Spells[s].SpellCategoryCooldown);
                }
                else
                {
                    data << uint32(spell->RecoveryTime);
                    data << uint32(spell->Category);
                    data << uint32(spell->CategoryRecoveryTime);
                }
            }
            else
            {
                data << uint32(0);
                data << uint32(0);
                data << uint32(0);
                data << uint32(-1);
                data << uint32(0);
                data << uint32(-1);
            }
        }
        data << pProto->Bonding;
        data << pProto->Description;
        data << pProto->PageText;
        data << pProto->LanguageID;
        data << pProto->PageMaterial;
        data << pProto->StartQuest;
        data << pProto->LockID;
        data << pProto->Material;
        data << pProto->Sheath;
        data << pProto->RandomProperty;
        data << pProto->Block;
        data << pProto->ItemSet;
        data << pProto->MaxDurability;
        data << pProto->Area;
        data << pProto->Map;                                // Added in 1.12.x & 2.0.1 client branch
        data << pProto->BagFamily;

        m_session->SendPacket(&data);
    }*/

    CreatureInfo const* ci = sObjectMgr.GetCreatureTemplate(5688);
    CreatureInfo ci2 = *ci;

    ci2 = *ci;
    ci2.name = "Loh";

    sWorld.SendUpdateCreatureStats(ci2, m_session);

    return true;
}

bool ChatHandler::HandleGameObjectSendCustomAnimCommand(char* args)
{
    // number or [name] Shift-click form |color|Hgameobject:go_id|h[name]|h|r
    uint32 lowguid;
    if (!ExtractUint32KeyFromLink(&args, "Hgameobject", lowguid))
        return false;

    if (!lowguid)
        return false;

    GameObject* pGameObject = nullptr;

    // by DB guid
    if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowguid))
        pGameObject = GetGameObjectWithGuid(lowguid, go_data->id);

    if (!pGameObject)
    {
        PSendSysMessage(LANG_COMMAND_OBJNOTFOUND, lowguid);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 uiAnim = 0;
    ExtractUInt32(&args, uiAnim);

    pGameObject->SendGameObjectCustomAnim(uiAnim);
    PSendSysMessage("Playing custom anim %u for %s (GUID %u).", uiAnim, pGameObject->GetName(), pGameObject->GetGUIDLow());

    return true;
}

bool ChatHandler::HandleCartographerCommand(char* args)
{
    Player* pPlayer = m_session->GetPlayer();
    if (!pPlayer)
    {
        SendSysMessage(LANG_NO_CHAR_SELECTED);
        SetSentErrorMessage(true);
        return false;
    }

    uint32 count = 0;
    uint32 lastUnexploredFlag = 0;
    uint32 const* fullExploreMask = sObjectMgr.GetCartographerExplorationMask();
    for (uint8 i = 0; i < PLAYER_EXPLORED_ZONES_SIZE; ++i)
    {
        for (uint32 j = 0; j < 32; ++j)
        {
            uint32 flag = 1 << j;
            if ((fullExploreMask[i] & flag) && !pPlayer->HasFlag(PLAYER_EXPLORED_ZONES_1 + i, flag))
            {
                count++;
                lastUnexploredFlag = (i * 32) + j;
            }
        }
    }
    
    if (count)
    {
        PSendSysMessage("You have %u areas left to explore.", count);
        if (AreaEntry const* pAreaEntry = sObjectMgr.GetAreaEntryByExploreFlag(lastUnexploredFlag))
        {
            if (pAreaEntry->ZoneId)
            {
                if (AreaEntry const* pZoneEntry = AreaEntry::GetById(pAreaEntry->ZoneId))
                {
                    PSendSysMessage("Next: %s (%s)", pAreaEntry->Name, pZoneEntry->Name);
                    return true;
                }
            }

            PSendSysMessage("Next: %s", pAreaEntry->Name);
        }
    }
    else
        SendSysMessage("You have explored all areas.");
       
    return true;
}

bool ChatHandler::HandleCharacterMailListCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &target_guid, &target_name, true))
        return false;

    PSendSysMessage("Listing mail for %s:", target_name.c_str());
    if (target)
    {
        MasterPlayer* pl = target->GetSession()->GetMasterPlayer();
        for (PlayerMails::iterator itr = pl->GetMailBegin(); itr != pl->GetMailEnd(); ++itr)
        {
            if ((*itr)->state == MAIL_STATE_DELETED || sWorld.GetGameTime() > (*itr)->expire_time)
                continue;

            PSendSysMessage("|cff00ff00%u|r - Sender Guid %u Expires %s Subject: |cff00ff00[%s]|r", (*itr)->messageID, (*itr)->sender, secsToTimeString((*itr)->expire_time - sWorld.GetGameTime(), true).c_str(), (*itr)->subject.c_str());
        }
    }
    else if (!target_guid.IsEmpty())
    {
        //                                                                    0     1         2          3
        std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT `id`, `sender`, `subject`, `expire_time` FROM `mail` WHERE `receiver` = %u && `isDeleted` = 0 && `expire_time` > %u", target_guid.GetCounter(), sWorld.GetGameTime()));
        if (!result)
            return true;

        do
        {
            Field* fields = result->Fetch();

            uint32 mailId = fields[0].GetUInt32();
            uint32 senderGuid = fields[1].GetUInt32();
            std::string subject = fields[2].GetCppString();
            time_t expireTime = fields[3].GetUInt64();

            PSendSysMessage("|cff00ff00%u|r - Sender Guid %u Expires %s Subject: |cff00ff00[%s]|r", mailId, senderGuid, secsToTimeString(expireTime - sWorld.GetGameTime(), true).c_str(), subject.c_str());
        } while (result->NextRow());
    }
    
    return true;
}

bool ChatHandler::HandleCharacterMailDeleteCommand(char* args)
{
    Player* target;
    ObjectGuid target_guid;
    std::string target_name;
    if (!ExtractPlayerTarget(&args, &target, &target_guid, &target_name, true))
        return false;

    uint32 mailId;
    if (!ExtractUInt32(&args, mailId))
        return false;

    if (target)
    {
        MasterPlayer* pl = target->GetSession()->GetMasterPlayer();
        if (Mail* m = pl->GetMail(mailId))
        {
            pl->MarkMailsUpdated();
            m->state = MAIL_STATE_DELETED;
        }
        target->GetSession()->SendMailResult(mailId, MAIL_DELETED, MAIL_OK);
    }
    else
    {
        CharacterDatabase.PExecute("UPDATE `mail` SET `expire_time`=%u WHERE `id`=%u", sWorld.GetGameTime(), mailId);
    }

    PSendSysMessage("Mail %u deleted from %s.", mailId, target_name.c_str());
    return true;
}

bool ChatHandler::HandleNpcAddEntryCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();

    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        return true;
    }

    uint32 uiCreatureId = 0;

    if (!ExtractUInt32(&args, uiCreatureId))
        return false;

    if (!sObjectMgr.GetCreatureTemplate(uiCreatureId))
    {
        PSendSysMessage(LANG_COMMAND_INVALIDCREATUREID, uiCreatureId);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("Creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    for (int i = 0; i < MAX_CREATURE_IDS_PER_SPAWN; i++)
    {
        if (pData->creature_id[i] == uiCreatureId)
        {
            SendSysMessage("Creature spawn already includes this entry.");
            SetSentErrorMessage(true);
            return false;
        }
    }

    if (pData->GetCreatureIdCount() >= MAX_CREATURE_IDS_PER_SPAWN)
    {
        SendSysMessage("Creature spawn has the maximum amount of entries already.");
        SetSentErrorMessage(true);
        return false;
    }

    int count = 0;
    std::array<uint32, MAX_CREATURE_IDS_PER_SPAWN> creatureIds = pData->creature_id;
    for (int i = 0; i < MAX_CREATURE_IDS_PER_SPAWN; i++)
    {
        if (!creatureIds[i])
        {
            count = i + 1;
            creatureIds[i] = uiCreatureId;
            break;
        }
    }
    std::sort(creatureIds.begin(), creatureIds.begin() + count);
    pData->creature_id = creatureIds;

    sWorld.ExecuteUpdate("UPDATE `creature` SET `id`=%u, `id2`=%u, `id3`=%u, `id4`=%u WHERE `guid`=%u", creatureIds[0], creatureIds[1], creatureIds[2], creatureIds[3], pCreature->GetGUIDLow());
    PSendSysMessage("Creature entry %u added to guid %u.", uiCreatureId, pCreature->GetGUIDLow());
    return true;
}

bool ChatHandler::HandleNpcSpawnInfoCommand(char* /*args*/)
{
    Creature* target = GetSelectedCreature();

    if (!target)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData const* pData = target->GetCreatureData();
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Spawn info for %s", target->GetObjectGuid().GetString().c_str());
    std::string creatureIds;
    for (uint32 i = 0; i < MAX_CREATURE_IDS_PER_SPAWN; i++)
    {
        if (!pData->creature_id[i])
            break;

        if (!creatureIds.empty())
            creatureIds += ", ";

        creatureIds += std::to_string(pData->creature_id[i]);
    }
    PSendSysMessage("Creature Ids: %s", creatureIds.c_str());
    PSendSysMessage(LANG_NPCINFO_POSITION, float(target->GetPositionX()), float(target->GetPositionY()), float(target->GetPositionZ()));
    PSendSysMessage("Orientation: %g", pData->position.o);
    PSendSysMessage("Respawn Time Min: %s", secsToTimeString(pData->spawntimesecsmin, true).c_str());
    PSendSysMessage("Respawn Time Max: %s", secsToTimeString(pData->spawntimesecsmax, true).c_str());
    std::string movementType;
    switch (pData->movement_type)
    {
        case IDLE_MOTION_TYPE:
            movementType = "Idle";
            break;
        case RANDOM_MOTION_TYPE:
            movementType = "Random";
            break;
        case WAYPOINT_MOTION_TYPE:
            movementType = "Waypoints";
            break;
    }
    PSendSysMessage("Movement Type: %s", movementType.c_str());
    PSendSysMessage("Wander Distance: %g", pData->wander_distance);
    PSendSysMessage(LANG_NPCINFO_ACTIVE_VISIBILITY, target->isActiveObject(), target->GetVisibilityModifier());
    ShowNpcOrGoSpawnInformation<Creature>(target->GetGUIDLow());

    return true;
}

bool ChatHandler::HandleNpcSpawnSetEntryCommand(char* args)
{
    uint32 creatureId;
    if (!ExtractUInt32(&args, creatureId))
        return false;

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature || pCreature->IsPet())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    pData->creature_id[0] = creatureId;
    pCreature->UpdateEntry(creatureId);
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature` SET `id`=%u WHERE `guid`=%u", creatureId, pCreature->GetDBTableGUIDLow());

    PSendSysMessage("Entry for guid %u updated to %u.", pCreature->GetDBTableGUIDLow(), creatureId);
    return true;
}

bool ChatHandler::HandleNpcSpawnSetDisplayIdCommand(char* args)
{
    uint32 displayId;
    if (!ExtractUInt32(&args, displayId))
        return false;

    if (!sCreatureDisplayInfoStore.LookupEntry(displayId))
    {
        PSendSysMessage("Display Id %u does not exist.", displayId);
        return false;
    }

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature || pCreature->IsPet())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->SetDisplayId(displayId);
    pCreature->SetNativeDisplayId(displayId);

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (CreatureDataAddon const* pAddonEntry = pCreature->GetCreatureAddon())
    {
        const_cast<CreatureDataAddon*>(pAddonEntry)->display_id = displayId;
        sWorld.ExecuteUpdate("UPDATE `creature_addon` SET `display_id`=%u WHERE `guid`=%u", displayId, pCreature->GetDBTableGUIDLow());
    }
    else
        sWorld.ExecuteUpdate("REPLACE INTO `creature_addon` (`guid`, `display_id`) VALUES (%u, %u)", pCreature->GetDBTableGUIDLow(), displayId);

    PSendSysMessage("Display Id for guid %u updated to %u.", pCreature->GetDBTableGUIDLow(), displayId);
    return true;
}

bool ChatHandler::HandleNpcSpawnSetEmoteStateCommand(char* args)
{
    uint32 emoteId;
    if (!ExtractUInt32(&args, emoteId))
        return false;

    EmotesEntry const* pEmoteEntry = nullptr;
    if (emoteId)
    {
        pEmoteEntry = sEmotesStore.LookupEntry(emoteId);
        if (!pEmoteEntry)
        {
            PSendSysMessage("Emote Id %u does not exist.", emoteId);
            return false;
        }
    }

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature || pCreature->IsPet())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->SetUInt32Value(UNIT_NPC_EMOTESTATE, emoteId);

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (CreatureDataAddon const* pAddonEntry = pCreature->GetCreatureAddon())
    {
        const_cast<CreatureDataAddon*>(pAddonEntry)->emote_state = emoteId;
        sWorld.ExecuteUpdate("UPDATE `creature_addon` SET `emote_state`=%u WHERE `guid`=%u", emoteId, pCreature->GetDBTableGUIDLow());
    }
    else
        sWorld.ExecuteUpdate("REPLACE INTO `creature_addon` (`guid`, `emote_state`) VALUES (%u, %u)", pCreature->GetDBTableGUIDLow(), emoteId);

    PSendSysMessage("Emote state for guid %u updated to %s (%u).", pCreature->GetDBTableGUIDLow(), pEmoteEntry ? pEmoteEntry->Name : "None", emoteId);
    return true;
}

bool ChatHandler::HandleNpcSpawnSetStandStateCommand(char* args)
{
    uint32 standState;
    if (!ExtractUInt32(&args, standState))
        return false;

    if (standState >= MAX_UNIT_STAND_STATE)
    {
        PSendSysMessage("Invalid stand state %u.", standState);
        return false;
    }

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature || pCreature->IsPet())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->SetStandState(standState);

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (CreatureDataAddon const* pAddonEntry = pCreature->GetCreatureAddon())
    {
        const_cast<CreatureDataAddon*>(pAddonEntry)->stand_state = standState;
        sWorld.ExecuteUpdate("UPDATE `creature_addon` SET `stand_state`=%u WHERE `guid`=%u", standState, pCreature->GetDBTableGUIDLow());
    }
    else
        sWorld.ExecuteUpdate("REPLACE INTO `creature_addon` (`guid`, `stand_state`) VALUES (%u, %u)", pCreature->GetDBTableGUIDLow(), standState);

    PSendSysMessage("Stand state for guid %u updated to %s (%u).", pCreature->GetDBTableGUIDLow(), UnitStandStateToString(standState), standState);
    return true;
}

bool ChatHandler::HandleNpcSpawnSetSheathStateCommand(char* args)
{
    uint32 sheathState;
    if (!ExtractUInt32(&args, sheathState))
        return false;

    if (sheathState >= MAX_SHEATH_STATE)
    {
        PSendSysMessage("Invalid sheath state %u.", sheathState);
        return false;
    }

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature || pCreature->IsPet())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    pCreature->SetSheath(SheathState(sheathState));

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (CreatureDataAddon const* pAddonEntry = pCreature->GetCreatureAddon())
    {
        const_cast<CreatureDataAddon*>(pAddonEntry)->sheath_state = sheathState;
        sWorld.ExecuteUpdate("UPDATE `creature_addon` SET `sheath_state`=%u WHERE `guid`=%u", sheathState, pCreature->GetDBTableGUIDLow());
    }
    else
        sWorld.ExecuteUpdate("REPLACE INTO `creature_addon` (`guid`, `sheath_state`) VALUES (%u, %u)", pCreature->GetDBTableGUIDLow(), sheathState);

    PSendSysMessage("Sheath state for guid %u updated to %s (%u).", pCreature->GetDBTableGUIDLow(), SheathStateToString(sheathState), sheathState);
    return true;
}

bool ChatHandler::HandleNpcSpawnSetMoveTypeCommand(char* args)
{
    // 2 arguments:
    // stay|random|way (determines the kind of movement)
    // NODEL (optional - tells the system NOT to delete any waypoints)
    //        this is very handy if you want to do waypoints, that are
    //        later switched on/off according to special events (like escort
    //        quests, etc)

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    MovementGeneratorType move_type;
    char* type_str = ExtractLiteralArg(&args);
    if (!type_str)
        return false;

    if (strncmp(type_str, "idle", strlen(type_str)) == 0)
        move_type = IDLE_MOTION_TYPE;
    else if (strncmp(type_str, "random", strlen(type_str)) == 0)
        move_type = RANDOM_MOTION_TYPE;
    else if (strncmp(type_str, "waypoint", strlen(type_str)) == 0)
        move_type = WAYPOINT_MOTION_TYPE;
    else
        return false;

    bool doNotDelete = ExtractLiteralArg(&args, "NODEL") != nullptr;
    if (!doNotDelete && *args)                              // need fail if false in result wrong literal
        return false;

    // update movement type
    if (!doNotDelete)
        sWaypointMgr.DeletePath(pCreature->GetDBTableGUIDLow());

    pData->movement_type = move_type;
    pCreature->SetDefaultMovementType(move_type);
    pCreature->GetMotionMaster()->Initialize();

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature` SET `movement_type`=%u WHERE `guid`=%u", move_type, pCreature->GetDBTableGUIDLow());

    PSendSysMessage("Movement for guid %u updated to %s.", pCreature->GetDBTableGUIDLow(), type_str);
    return true;
}

bool ChatHandler::HandleNpcSpawnWanderDistCommand(char* args)
{
    float wanderDistance;
    if (!ExtractFloat(&args, wanderDistance))
        return false;

    if (wanderDistance < 0.0f)
    {
        SendSysMessage(LANG_BAD_VALUE);
        return false;
    }

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    pData->wander_distance = wanderDistance;
    pCreature->SetWanderDistance(wanderDistance);
    pCreature->GetMotionMaster()->Initialize();

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature` SET `wander_distance`=%f WHERE `guid`=%u", wanderDistance, pCreature->GetDBTableGUIDLow());
    PSendSysMessage("Wander distance for guid %u updated to %g.", pCreature->GetDBTableGUIDLow(), wanderDistance);
    return true;
}

bool ChatHandler::HandleNpcSpawnSetRespawnTimeCommand(char* args)
{
    uint32 timeMin;
    if (!ExtractUInt32(&args, timeMin))
        return false;
    uint32 timeMax;
    if (!ExtractUInt32(&args, timeMax))
        timeMax = timeMin;

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    pData->spawntimesecsmin = timeMin;
    pData->spawntimesecsmax = timeMax;
    pCreature->SetRespawnDelay((timeMin + timeMax) / 2);
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature` SET `spawntimesecsmin`=%u, `spawntimesecsmax`=%u WHERE `guid`=%u", timeMin, timeMax, pCreature->GetDBTableGUIDLow());

    PSendSysMessage("Respawn time for guid %u updated to %u-%u.", pCreature->GetDBTableGUIDLow(), timeMin, timeMax);
    return true;
}

bool ChatHandler::HandleNpcSpawnSetAurasCommand(char* args)
{
    Tokens auras = StrSplit(args, " ");
    for (auto const& token : auras)
    {
        if (!isNumeric(token))
        {
            PSendSysMessage("Invalid symbol %s. Expected list of spells separated by spaces.", token.c_str());
            return false;
        }

        uint32 spellId = atoi(token.c_str());
        if (!sSpellMgr.GetSpellEntry(spellId))
        {
            PSendSysMessage("Aura %u does not exist.", spellId);
            return false;
        }
    }

    Creature* pCreature = GetSelectedCreature();
    if (!pCreature || pCreature->IsPet())
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }
    CreatureData* pData = const_cast<CreatureData*>(pCreature->GetCreatureData());
    if (!pData)
    {
        SendSysMessage("This creature is not a permanent spawn.");
        SetSentErrorMessage(true);
        return false;
    }

    for (auto const& token : auras)
        HandleAuraHelper(atoi(token.c_str()), 0, pCreature);

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    if (CreatureDataAddon const* pAddonEntry = pCreature->GetCreatureAddon())
    {
        delete const_cast<CreatureDataAddon*>(pAddonEntry)->auras;
        const_cast<CreatureDataAddon*>(pAddonEntry)->auras = new uint32[auras.size() + 1];
        for (int i = 0; i < auras.size(); i++)
            const_cast<uint32*>(const_cast<CreatureDataAddon*>(pAddonEntry)->auras)[i] = atoi(auras[i].c_str());
        const_cast<uint32*>(const_cast<CreatureDataAddon*>(pAddonEntry)->auras)[auras.size()] = 0;
        sWorld.ExecuteUpdate("UPDATE `creature_addon` SET `auras`='%s' WHERE `guid`=%u", args, pCreature->GetDBTableGUIDLow());
    }
    else
        sWorld.ExecuteUpdate("REPLACE INTO `creature_addon` (`guid`, `auras`) VALUES (%u, '%s')", pCreature->GetDBTableGUIDLow(), args);

    PSendSysMessage("Auras for guid %u updated to '%s'.", pCreature->GetDBTableGUIDLow(), args);
    return true;
}


#ifndef USING_DISCORD_BOT
bool ChatHandler::HandleDiscBotStopCommand(char* args)
{
    return true;
}
#endif

bool ChatHandler::HandleNpcTemplateSetDisplayIdCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo* pTemplate = const_cast<CreatureInfo*>(pCreature->GetCreatureInfo());
    if (!pTemplate)
    {
        SendSysMessage("Can't find creature template.");
        SetSentErrorMessage(true);
        return false;
    }

    sWorld.GetMigration().SetAuthor(m_session->GetUsername());

    uint32 displayId;
    for (uint32 i = 1; i <= MAX_DISPLAY_IDS_PER_CREATURE && ExtractUInt32(&args, displayId); i++)
    {
        if (displayId && !sCreatureDisplayInfoStore.LookupEntry(displayId))
        {
            PSendSysMessage("Display Id %u does not exist.", displayId);
            return false;
        }

        pTemplate->display_id[i - 1] = displayId;
        sWorld.ExecuteUpdate("UPDATE `creature_template` SET `display_id%u`=%u WHERE `entry`=%u", i, displayId, pTemplate->entry);
        PSendSysMessage("Updated template display_id%u of %s to %u.", i, pCreature->GetName(), displayId);
    }
    
    return true;
}

bool ChatHandler::HandleNpcTemplateSetMountDisplayIdCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo* pTemplate = const_cast<CreatureInfo*>(pCreature->GetCreatureInfo());
    if (!pTemplate)
    {
        SendSysMessage("Can't find creature template.");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 mountDisplayId = 0;
    if (!ExtractUInt32(&args, mountDisplayId))
        return false;

    if (mountDisplayId && !sCreatureDisplayInfoStore.LookupEntry(mountDisplayId))
    {
        PSendSysMessage("Display Id %u does not exist.", mountDisplayId);
        return false;
    }

    pTemplate->mount_display_id = mountDisplayId;
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature_template` SET `mount_display_id`=%u WHERE `entry`=%u", mountDisplayId, pTemplate->entry);
    PSendSysMessage("Updated template mount_display_id of %s to %u.", pCreature->GetName(), mountDisplayId);

    return true;
}

bool ChatHandler::HandleNpcTemplateSetLevelCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo* pTemplate = const_cast<CreatureInfo*>(pCreature->GetCreatureInfo());
    if (!pTemplate)
    {
        SendSysMessage("Can't find creature template.");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 levelMin = 0;
    if (!ExtractUInt32(&args, levelMin))
        return false;

    if (levelMin <= 0 || levelMin >= MAX_LEVEL)
    {
        PSendSysMessage("Invalid level %u.", levelMin);
        return false;
    }

    uint32 levelMax = levelMin;
    if (ExtractUInt32(&args, levelMax))
    {
        if (levelMax <= 0 || levelMax >= MAX_LEVEL)
        {
            PSendSysMessage("Invalid level %u.", levelMax);
            return false;
        }
    }

    if (levelMin > levelMax)
        std::swap(levelMin, levelMax);

    pTemplate->level_min = levelMin;
    pTemplate->level_max = levelMax;
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature_template` SET `level_min`=%u, `level_max`=%u WHERE `entry`=%u", levelMin, levelMax, pTemplate->entry);
    PSendSysMessage("Updated template level range of %s to %u-%u.", pCreature->GetName(), levelMin, levelMax);

    return true;
}

bool ChatHandler::HandleNpcTemplateSetFactionCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo* pTemplate = const_cast<CreatureInfo*>(pCreature->GetCreatureInfo());
    if (!pTemplate)
    {
        SendSysMessage("Can't find creature template.");
        SetSentErrorMessage(true);
        return false;
    }

    uint32 factionTemplateId = 0;
    if (!ExtractUInt32(&args, factionTemplateId))
        return false;

    FactionTemplateEntry const* pFaction = sObjectMgr.GetFactionTemplateEntry(factionTemplateId);
    if (!pFaction)
    {
        PSendSysMessage("Faction Template Id %u does not exist.", factionTemplateId);
        return false;
    }

    pTemplate->faction = factionTemplateId;
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature_template` SET `faction`=%u WHERE `entry`=%u", factionTemplateId, pTemplate->entry);
    PSendSysMessage("Updated template faction of %s to %s (%u).", pCreature->GetName(), sObjectMgr.GetFactionEntry(pFaction->faction)->name[0].c_str(), factionTemplateId);

    return true;
}

bool ChatHandler::HandleNpcTemplateSetScaleCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo* pTemplate = const_cast<CreatureInfo*>(pCreature->GetCreatureInfo());
    if (!pTemplate)
    {
        SendSysMessage("Can't find creature template.");
        SetSentErrorMessage(true);
        return false;
    }

    float scale = 0;
    if (!ExtractFloat(&args, scale))
        return false;

    if (scale <= 0 || scale > 10)
    {
        PSendSysMessage("Invalid scale %g.", scale);
        return false;
    }

    pTemplate->scale = scale;
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature_template` SET `scale`=%g WHERE `entry`=%u", scale, pTemplate->entry);
    PSendSysMessage("Updated template scale of %s to %g.", pCreature->GetName(), scale);

    return true;
}

bool ChatHandler::HandleNpcTemplateSetDetectionRangeCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo* pTemplate = const_cast<CreatureInfo*>(pCreature->GetCreatureInfo());
    if (!pTemplate)
    {
        SendSysMessage("Can't find creature template.");
        SetSentErrorMessage(true);
        return false;
    }

    float range = 0;
    if (!ExtractFloat(&args, range))
        return false;

    if (range < 0 || range > MAX_VISIBILITY_DISTANCE)
    {
        PSendSysMessage("Invalid detection range %g.", range);
        return false;
    }

    pCreature->SetDetectionDistance(range); // update for current creature too

    pTemplate->detection_range = range;
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature_template` SET `detection_range`=%g WHERE `entry`=%u", range, pTemplate->entry);
    PSendSysMessage("Updated template detection_range of %s to %g.", pCreature->GetName(), range);

    return true;
}

bool ChatHandler::HandleNpcTemplateSetCallForHelpRangeCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo* pTemplate = const_cast<CreatureInfo*>(pCreature->GetCreatureInfo());
    if (!pTemplate)
    {
        SendSysMessage("Can't find creature template.");
        SetSentErrorMessage(true);
        return false;
    }

    float range = 0;
    if (!ExtractFloat(&args, range))
        return false;

    if (range < 0 || range > MAX_VISIBILITY_DISTANCE)
    {
        PSendSysMessage("Invalid call for help range %g.", range);
        return false;
    }

    pCreature->SetCallForHelpDist(range); // update for current creature too

    pTemplate->call_for_help_range = range;
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature_template` SET `call_for_help_range`=%g WHERE `entry`=%u", range, pTemplate->entry);
    PSendSysMessage("Updated template call_for_help_range of %s to %g.", pCreature->GetName(), range);

    return true;
}

bool ChatHandler::HandleNpcTemplateSetLeashRangeCommand(char* args)
{
    Creature* pCreature = GetSelectedCreature();
    if (!pCreature)
    {
        SendSysMessage(LANG_SELECT_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    CreatureInfo* pTemplate = const_cast<CreatureInfo*>(pCreature->GetCreatureInfo());
    if (!pTemplate)
    {
        SendSysMessage("Can't find creature template.");
        SetSentErrorMessage(true);
        return false;
    }

    float range = 0;
    if (!ExtractFloat(&args, range))
        return false;

    if (range < 0 || range > MAX_VISIBILITY_DISTANCE)
    {
        PSendSysMessage("Invalid leash range %g.", range);
        return false;
    }

    pCreature->SetLeashDistance(range); // update for current creature too

    pTemplate->leash_range = range;
    sWorld.GetMigration().SetAuthor(m_session->GetUsername());
    sWorld.ExecuteUpdate("UPDATE `creature_template` SET `leash_range`=%g WHERE `entry`=%u", range, pTemplate->entry);
    PSendSysMessage("Updated template leash_range of %s to %g.", pCreature->GetName(), range);

    return true;
}

bool ChatHandler::HandleNpcTopTarget(char* args)
{
	Creature* pCreature = GetSelectedCreature();
	if (!pCreature)
	{
		SendSysMessage(LANG_SELECT_CREATURE);
		SetSentErrorMessage(true);
		return false;
	}

    if (Unit* HostileTarget = pCreature->GetThreatManager().getHostileTarget())
    {
        PSendSysMessage("Target name %s, pos: x:%.02f, y:%.02f, z:%.02f mapID: %u", 
            HostileTarget->GetName(), 
            HostileTarget->GetPositionX(),
            HostileTarget->GetPositionY(),
            HostileTarget->GetPositionZ(),
            HostileTarget->GetMapId());
    }
    else
    {
        SendSysMessage("No target");
    }

    return true;
}

bool ChatHandler::HandleCharacterDiffItemsCommand(char* args)
{
    char* nameStr = ExtractLiteralArg(&args);
    if (!nameStr)
        return false;

    Player* target;
    ObjectGuid targetGuid;
    std::string targetName;
    if (!ExtractPlayerTarget(&nameStr, &target, &targetGuid, &targetName))
        return false;

    std::map<uint32 /*guid*/, uint32 /*entry*/> newItems;
    std::unique_ptr<QueryResult> result(CharacterDatabase.PQuery("SELECT `item`, `item_template` FROM `character_inventory` WHERE `guid`=%u", targetGuid.GetCounter()));
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 itemGuid = fields[0].GetUInt32();
            uint32 itemEntry = fields[1].GetUInt32();
            newItems.insert({ itemGuid, itemEntry });
        } while (result->NextRow());
    }

    std::map<uint32 /*guid*/, uint32 /*entry*/> oldItems;
    result.reset(CharacterDatabase.PQuery("SELECT `item`, `item_template` FROM `character_inventory_copy` WHERE `guid`=%u", targetGuid.GetCounter()));
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 itemGuid = fields[0].GetUInt32();
            uint32 itemEntry = fields[1].GetUInt32();
            oldItems.insert({ itemGuid, itemEntry });
        } while (result->NextRow());
    }

    PSendSysMessage("Newly obtained items for %s:", targetName.c_str());
    for (auto const& itr : newItems)
    {
        if (oldItems.find(itr.first) == oldItems.end())
        {
            ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(itr.second);
            if (!pProto)
            {
                PSendSysMessage("%u - [UNKNOWN]", itr.second);
                continue;
            }

            if (m_session)
            {
                uint32 color = ItemQualityColors[pProto->Quality];
                std::ostringstream itemStr;
                itemStr << "|c" << std::hex << color << "|Hitem:" << std::to_string(pProto->ItemId) << ":0:0:0:0:0:0:0|h[" << pProto->Name1 << "]|h|r";
                PSendSysMessage("%u - %s", itr.second, itemStr.str().c_str());
            }
            else
                PSendSysMessage("%u - %s", itr.second, pProto->Name1.c_str());
        }
    }

    PSendSysMessage("No longer owned items for %s:", targetName.c_str());
    for (auto const& itr : oldItems)
    {
        if (newItems.find(itr.first) == newItems.end())
        {
            ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(itr.second);
            if (!pProto)
            {
                PSendSysMessage("%u - [UNKNOWN]", itr.second);
                continue;
            }

            if (m_session)
            {
                uint32 color = ItemQualityColors[pProto->Quality];
                std::ostringstream itemStr;
                itemStr << "|c" << std::hex << color << "|Hitem:" << std::to_string(pProto->ItemId) << ":0:0:0:0:0:0:0|h[" << pProto->Name1 << "]|h|r";
                PSendSysMessage("%u - %s", itr.second, itemStr.str().c_str());
            }
            else
                PSendSysMessage("%u - %s", itr.second, pProto->Name1.c_str());
        }
    }

    return true;
}

bool ChatHandler::HandlePDumpListCommand(char* args)
{
    uint32 guidLow = 0;
    if (!ExtractUInt32(&args, guidLow))
        return false;

    char fileName[32] = {};
    sprintf(fileName, "Char%u-", guidLow);

    PSendSysMessage("Searching for pdumps for guid %u:", guidLow);
    if (ACE_DIR* dirp = ACE_OS::opendir(ACE_TEXT(sWorld.GetPDumpDirectory().c_str())))
    {
        ACE_DIRENT* dp;

        while (!!(dp = ACE_OS::readdir(dirp)))
            if (strstr(dp->d_name, fileName))
                PSendSysMessage("- %s", playerLink(dp->d_name).c_str());

#ifndef _WIN32
        // this causes a crash on Windows, so just accept a minor memory leak for now
        ACE_OS::closedir(dirp);
#endif
    }

    return true;
}

bool ChatHandler::HandlePDumpLoadCommand(char *args)
{
    char* file = ExtractQuotedOrLiteralArg(&args);
    if (!file)
        return false;

    std::string account_name;
    uint32 account_id = ExtractAccountId(&args, &account_name);
    if (!account_id)
        return false;

    char* name_str = ExtractLiteralArg(&args);

    uint32 lowguid = 0;
    std::string name;

    if (name_str)
    {
        name = name_str;
        // normalize the name if specified and check if it exists
        if (!normalizePlayerName(name))
        {
            PSendSysMessage(LANG_INVALID_CHARACTER_NAME);
            SetSentErrorMessage(true);
            return false;
        }

        if (ObjectMgr::CheckPlayerName(name, true) != CHAR_NAME_SUCCESS)
        {
            PSendSysMessage(LANG_INVALID_CHARACTER_NAME);
            SetSentErrorMessage(true);
            return false;
        }

        if (*args)
        {
            if (!ExtractUInt32(&args, lowguid))
                return false;

            if (!lowguid)
            {
                PSendSysMessage(LANG_INVALID_CHARACTER_GUID);
                SetSentErrorMessage(true);
                return false;
            }

            ObjectGuid guid = ObjectGuid(HIGHGUID_PLAYER, lowguid);

            if (sObjectMgr.GetPlayerAccountIdByGUID(guid))
            {
                PSendSysMessage(LANG_CHARACTER_GUID_IN_USE, lowguid);
                SetSentErrorMessage(true);
                return false;
            }
        }
    }

    switch (PlayerDumpReader().LoadDump(sWorld.GetPDumpDirectory() + "/" + file, account_id, name, lowguid))
    {
        case DUMP_SUCCESS:
            PSendSysMessage(LANG_COMMAND_IMPORT_SUCCESS);
            break;
        case DUMP_FILE_OPEN_ERROR:
            PSendSysMessage(LANG_FILE_OPEN_FAIL, file);
            SetSentErrorMessage(true);
            return false;
        case DUMP_FILE_BROKEN:
            PSendSysMessage(LANG_DUMP_BROKEN, file);
            SetSentErrorMessage(true);
            return false;
        case DUMP_TOO_MANY_CHARS:
            PSendSysMessage(LANG_ACCOUNT_CHARACTER_LIST_FULL, account_name.c_str(), account_id);
            SetSentErrorMessage(true);
            return false;
        default:
            PSendSysMessage(LANG_COMMAND_IMPORT_FAILED);
            SetSentErrorMessage(true);
            return false;
    }

    return true;
}

bool ChatHandler::HandlePDumpWriteCommand(char *args)
{
    if (!*args)
        return false;

    char* file = ExtractQuotedOrLiteralArg(&args);
    if (!file)
        return false;

    char* p2 = ExtractLiteralArg(&args);

    uint32 lowguid;
    ObjectGuid guid;
    // character name can't start from number
    if (!ExtractUInt32(&p2, lowguid))
    {
        std::string name = ExtractPlayerNameFromLink(&p2);
        if (name.empty())
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        guid = sObjectMgr.GetPlayerGuidByName(name);
        if (!guid)
        {
            PSendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        lowguid = guid.GetCounter();
    }
    else
        guid = ObjectGuid(HIGHGUID_PLAYER, lowguid);

    if (!sObjectMgr.GetPlayerAccountIdByGUID(guid))
    {
        PSendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    switch (PlayerDumpWriter().WriteDump(sWorld.GetPDumpDirectory() + "/" + file, lowguid))
    {
        case DUMP_SUCCESS:
            PSendSysMessage(LANG_COMMAND_EXPORT_SUCCESS);
            break;
        case DUMP_FILE_OPEN_ERROR:
            PSendSysMessage(LANG_FILE_OPEN_FAIL, file);
            SetSentErrorMessage(true);
            return false;
        default:
            PSendSysMessage(LANG_COMMAND_EXPORT_FAILED);
            SetSentErrorMessage(true);
            return false;
    }

    return true;
}

Object* ChatHandler::GetObjectHelper(CommandStream& stream, uint32& lowGuid, uint32& index)
{
    std::string target;

    if (!(stream >> target))
    {
        SendSysMessage("Wrongly formatted target. Try: player,object,creature");
        return nullptr;
    }

    Object* targetObject = nullptr;

    if (!(stream >> lowGuid))
    {
        SendSysMessage("Wrongly formatted low GUID.");
        return nullptr;
    }



    if (iequals(target, "player"))
        targetObject = sObjectAccessor.FindPlayer(ObjectGuid(HIGHGUID_PLAYER, 0, lowGuid));
    else if (iequals(target, "object"))
    {
        if (GameObjectData const* go_data = sObjectMgr.GetGOData(lowGuid))
            targetObject = GetGameObjectWithGuidGlobal(lowGuid, go_data);
    }
    else if (iequals(target, "creature"))
    {
        if (CreatureData const* data = sObjectMgr.GetCreatureData(lowGuid))
            targetObject = m_session->GetPlayer()->GetMap()->GetCreature(data->GetObjectGuid(lowGuid));
    }
    else if (iequals(target, "item"))
    {
        //Not yet.
    }


    if (!targetObject)
        PSendSysMessage("Couldn't find target with type :%s.", target.c_str());
    return targetObject;
}

bool ChatHandler::HandleUnitStatInfoCommand(char* args)
{
    Unit* pTarget = GetSelectedUnit();

    if (!pTarget)
    {
        SendSysMessage(LANG_SELECT_CHAR_OR_CREATURE);
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Stat info for %s", pTarget->GetObjectGuid().GetString().c_str());
    PSendSysMessage("Health: %u", pTarget->GetHealth());
    PSendSysMessage("Max Health: %u", pTarget->GetMaxHealth());
    PSendSysMessage("Power type: %hhu", pTarget->GetPowerType());
    PSendSysMessage("Mana: %u", pTarget->GetPower(POWER_MANA));
    PSendSysMessage("Max Mana: %u", pTarget->GetMaxPower(POWER_MANA));
    PSendSysMessage("Rage: %u", pTarget->GetPower(POWER_RAGE));
    PSendSysMessage("Max Rage: %u", pTarget->GetMaxPower(POWER_RAGE));
    PSendSysMessage("Focus: %u", pTarget->GetPower(POWER_FOCUS));
    PSendSysMessage("Max Focus: %u", pTarget->GetMaxPower(POWER_FOCUS));
    PSendSysMessage("Energy: %u", pTarget->GetPower(POWER_ENERGY));
    PSendSysMessage("Max Energy: %u", pTarget->GetMaxPower(POWER_ENERGY));
    PSendSysMessage("Happiness: %u", pTarget->GetPower(POWER_HAPPINESS));
    PSendSysMessage("Max Happiness: %u", pTarget->GetMaxPower(POWER_HAPPINESS));
    PSendSysMessage("Base attack time: %g", pTarget->GetFloatValue(UNIT_FIELD_BASEATTACKTIME));
    PSendSysMessage("Off hand attack time: %g", pTarget->GetFloatValue(UNIT_FIELD_BASEATTACKTIME + 1));
    PSendSysMessage("Ranged attack time: %g", pTarget->GetFloatValue(UNIT_FIELD_RANGEDATTACKTIME));
    PSendSysMessage("Min damage: %g", pTarget->GetFloatValue(UNIT_FIELD_MINDAMAGE));
    PSendSysMessage("Max damage: %g", pTarget->GetFloatValue(UNIT_FIELD_MAXDAMAGE));
    PSendSysMessage("Min off hand damage: %g", pTarget->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE));
    PSendSysMessage("Max off hand damage: %g", pTarget->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE));
    PSendSysMessage("Casting speed mod: %g", pTarget->GetFloatValue(UNIT_MOD_CAST_SPEED));
    PSendSysMessage("Casting speed mod: %i", pTarget->GetInt32Value(UNIT_MOD_CAST_SPEED));
    PSendSysMessage("Base strength: %g", pTarget->GetCreateStat(STAT_STRENGTH));
    PSendSysMessage("Base agility: %g", pTarget->GetCreateStat(STAT_AGILITY));
    PSendSysMessage("Base stamina: %g", pTarget->GetCreateStat(STAT_STAMINA));
    PSendSysMessage("Base intellect: %g", pTarget->GetCreateStat(STAT_INTELLECT));
    PSendSysMessage("Base spirit: %g", pTarget->GetCreateStat(STAT_SPIRIT));
    PSendSysMessage("Total strength: %g", pTarget->GetStat(STAT_STRENGTH));
    PSendSysMessage("Total agility: %g", pTarget->GetStat(STAT_AGILITY));
    PSendSysMessage("Total stamina: %g", pTarget->GetStat(STAT_STAMINA));
    PSendSysMessage("Total intellect: %g", pTarget->GetStat(STAT_INTELLECT));
    PSendSysMessage("Total spirit: %g", pTarget->GetStat(STAT_SPIRIT));
    PSendSysMessage("Base armor: %i", pTarget->GetCreateResistance(SPELL_SCHOOL_NORMAL));
    PSendSysMessage("Base holy resist: %i", pTarget->GetCreateResistance(SPELL_SCHOOL_HOLY));
    PSendSysMessage("Base fire resist: %i", pTarget->GetCreateResistance(SPELL_SCHOOL_FIRE));
    PSendSysMessage("Base nature resist: %i", pTarget->GetCreateResistance(SPELL_SCHOOL_NATURE));
    PSendSysMessage("Base frost resist: %i", pTarget->GetCreateResistance(SPELL_SCHOOL_FROST));
    PSendSysMessage("Base shadow resist: %i", pTarget->GetCreateResistance(SPELL_SCHOOL_SHADOW));
    PSendSysMessage("Base arcane resist: %i", pTarget->GetCreateResistance(SPELL_SCHOOL_ARCANE));
    PSendSysMessage("Total armor: %i", pTarget->GetResistance(SPELL_SCHOOL_NORMAL));
    PSendSysMessage("Total holy resist: %i", pTarget->GetResistance(SPELL_SCHOOL_HOLY));
    PSendSysMessage("Total fire resist: %i", pTarget->GetResistance(SPELL_SCHOOL_FIRE));
    PSendSysMessage("Total nature resist: %i", pTarget->GetResistance(SPELL_SCHOOL_NATURE));
    PSendSysMessage("Total frost resist: %i", pTarget->GetResistance(SPELL_SCHOOL_FROST));
    PSendSysMessage("Total shadow resist: %i", pTarget->GetResistance(SPELL_SCHOOL_SHADOW));
    PSendSysMessage("Total arcane resist: %i", pTarget->GetResistance(SPELL_SCHOOL_ARCANE));
    PSendSysMessage("Attack power: %u", pTarget->GetUInt32Value(UNIT_FIELD_ATTACK_POWER));
    PSendSysMessage("Attack power mods: %i / %i", pTarget->GetInt16Value(UNIT_FIELD_ATTACK_POWER_MODS, 0), pTarget->GetInt16Value(UNIT_FIELD_ATTACK_POWER_MODS, 1));
    PSendSysMessage("Attack power multiplier: %u", pTarget->GetFloatValue(UNIT_FIELD_ATTACK_POWER_MULTIPLIER));
    PSendSysMessage("Ranged attack power: %u", pTarget->GetUInt32Value(UNIT_FIELD_RANGED_ATTACK_POWER));
    PSendSysMessage("Ranged attack power mods: %i / %i", pTarget->GetInt16Value(UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 0), pTarget->GetInt16Value(UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 1));
    PSendSysMessage("Ranged attack power multiplier: %u", pTarget->GetFloatValue(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER));
    PSendSysMessage("Min ranged damage: %g", pTarget->GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE));
    PSendSysMessage("Max ranged damage: %g", pTarget->GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE));
    PSendSysMessage("Physical power cost modifier: %i", pTarget->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + SPELL_SCHOOL_NORMAL));
    PSendSysMessage("Holy power cost modifier: %i", pTarget->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + SPELL_SCHOOL_HOLY));
    PSendSysMessage("Fire power cost modifier: %i", pTarget->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + SPELL_SCHOOL_FIRE));
    PSendSysMessage("Nature power cost modifier: %i", pTarget->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + SPELL_SCHOOL_NATURE));
    PSendSysMessage("Frost power cost modifier: %i", pTarget->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + SPELL_SCHOOL_FROST));
    PSendSysMessage("Shadow power cost modifier: %i", pTarget->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + SPELL_SCHOOL_SHADOW));
    PSendSysMessage("Arcane power cost modifier: %i", pTarget->GetInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + SPELL_SCHOOL_ARCANE));
    PSendSysMessage("Physical power cost multiplier: %g", pTarget->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + SPELL_SCHOOL_NORMAL));
    PSendSysMessage("Holy power cost multiplier: %g", pTarget->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + SPELL_SCHOOL_HOLY));
    PSendSysMessage("Fire power cost multiplier: %g", pTarget->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + SPELL_SCHOOL_FIRE));
    PSendSysMessage("Nature power cost multiplier: %g", pTarget->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + SPELL_SCHOOL_NATURE));
    PSendSysMessage("Frost power cost multiplier: %g", pTarget->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + SPELL_SCHOOL_FROST));
    PSendSysMessage("Shadow power cost multiplier: %g", pTarget->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + SPELL_SCHOOL_SHADOW));
    PSendSysMessage("Arcane power cost multiplier: %g", pTarget->GetFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + SPELL_SCHOOL_ARCANE));

    Player* pPlayer = pTarget->ToPlayer();
    if (!pPlayer)
        return true;

    PSendSysMessage("Block chance: %g", pPlayer->GetFloatValue(PLAYER_BLOCK_PERCENTAGE));
    PSendSysMessage("Dodge chance: %g", pPlayer->GetFloatValue(PLAYER_DODGE_PERCENTAGE));
    PSendSysMessage("Parry chance: %g", pPlayer->GetFloatValue(PLAYER_PARRY_PERCENTAGE));
    PSendSysMessage("Crit chance: %g", pPlayer->GetFloatValue(PLAYER_CRIT_PERCENTAGE));
    PSendSysMessage("Ranged crit chance: %g", pPlayer->GetFloatValue(PLAYER_RANGED_CRIT_PERCENTAGE));
    PSendSysMessage("Physical spell crit chance: %g", pPlayer->GetSpellCritPercent(SPELL_SCHOOL_NORMAL));
    PSendSysMessage("Holy spell crit chance: %g", pPlayer->GetSpellCritPercent(SPELL_SCHOOL_HOLY));
    PSendSysMessage("Fire spell crit chance: %g", pPlayer->GetSpellCritPercent(SPELL_SCHOOL_FIRE));
    PSendSysMessage("Nature spell crit chance: %g", pPlayer->GetSpellCritPercent(SPELL_SCHOOL_NATURE));
    PSendSysMessage("Frost spell crit chance: %g", pPlayer->GetSpellCritPercent(SPELL_SCHOOL_FROST));
    PSendSysMessage("Shadow spell crit chance: %g", pPlayer->GetSpellCritPercent(SPELL_SCHOOL_SHADOW));
    PSendSysMessage("Arcane spell crit chance: %g", pPlayer->GetSpellCritPercent(SPELL_SCHOOL_ARCANE));
    PSendSysMessage("Melee hit chance: %g", pPlayer->m_modMeleeHitChance);
    PSendSysMessage("Ranged hit chance: %g", pPlayer->m_modRangedHitChance);
    PSendSysMessage("Spell hit chance: %g", pPlayer->m_modSpellHitChance);
    PSendSysMessage("Positive strength: %g", pPlayer->GetPosStat(STAT_STRENGTH));
    PSendSysMessage("Positive agility: %g", pPlayer->GetPosStat(STAT_AGILITY));
    PSendSysMessage("Positive stamina: %g", pPlayer->GetPosStat(STAT_STAMINA));
    PSendSysMessage("Positive intellect: %g", pPlayer->GetPosStat(STAT_INTELLECT));
    PSendSysMessage("Positive spirit: %g", pPlayer->GetPosStat(STAT_SPIRIT));
    PSendSysMessage("Negative strength: %g", pPlayer->GetNegStat(STAT_STRENGTH));
    PSendSysMessage("Negative agility: %g", pPlayer->GetNegStat(STAT_AGILITY));
    PSendSysMessage("Negative stamina: %g", pPlayer->GetNegStat(STAT_STAMINA));
    PSendSysMessage("Negative intellect: %g", pPlayer->GetNegStat(STAT_INTELLECT));
    PSendSysMessage("Negative spirit: %g", pPlayer->GetNegStat(STAT_SPIRIT));
    PSendSysMessage("Positive armor buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_NORMAL, true));
    PSendSysMessage("Positive holy resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_HOLY, true));
    PSendSysMessage("Positive fire resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_FIRE, true));
    PSendSysMessage("Positive nature resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_NATURE, true));
    PSendSysMessage("Positive frost resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_FROST, true));
    PSendSysMessage("Positive shadow resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_SHADOW, true));
    PSendSysMessage("Positive arcane resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_ARCANE, true));
    PSendSysMessage("Negative armor buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_NORMAL, false));
    PSendSysMessage("Negative holy resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_HOLY, false));
    PSendSysMessage("Negative fire resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_FIRE, false));
    PSendSysMessage("Negative nature resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_NATURE, false));
    PSendSysMessage("Negative frost resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_FROST, false));
    PSendSysMessage("Negative shadow resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_SHADOW, false));
    PSendSysMessage("Negative arcane resist buff mod: %g", pPlayer->GetResistanceBuffMods(SPELL_SCHOOL_ARCANE, false));
    PSendSysMessage("Positive physical damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_NORMAL));
    PSendSysMessage("Positive holy damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_HOLY));
    PSendSysMessage("Positive fire damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FIRE));
    PSendSysMessage("Positive nature damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_NATURE));
    PSendSysMessage("Positive frost damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_FROST));
    PSendSysMessage("Positive shadow damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW));
    PSendSysMessage("Positive arcane damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_ARCANE));
    PSendSysMessage("Negative physical damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_NORMAL));
    PSendSysMessage("Negative holy damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_HOLY));
    PSendSysMessage("Negative fire damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_FIRE));
    PSendSysMessage("Negative nature damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_NATURE));
    PSendSysMessage("Negative frost damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_FROST));
    PSendSysMessage("Negative shadow damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_SHADOW));
    PSendSysMessage("Negative arcane damage mod: %u", pPlayer->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_ARCANE));
    PSendSysMessage("Percent physical damage mod: %g", pPlayer->GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + SPELL_SCHOOL_NORMAL));
    PSendSysMessage("Percent holy damage mod: %g", pPlayer->GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + SPELL_SCHOOL_HOLY));
    PSendSysMessage("Percent fire damage mod: %g", pPlayer->GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + SPELL_SCHOOL_FIRE));
    PSendSysMessage("Percent nature damage mod: %g", pPlayer->GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + SPELL_SCHOOL_NATURE));
    PSendSysMessage("Percent frost damage mod: %g", pPlayer->GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + SPELL_SCHOOL_FROST));
    PSendSysMessage("Percent shadow damage mod: %g", pPlayer->GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + SPELL_SCHOOL_SHADOW));
    PSendSysMessage("Percent arcane damage mod: %g", pPlayer->GetFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + SPELL_SCHOOL_ARCANE));
    return true;
}





bool ChatHandler::HandleDebugFieldsShowCommand(char* args)
{
    CommandStream stream{ args };

    auto sourcePlayer = GetPlayer();

    if (!sourcePlayer)
        return false;


    uint32 lowGuid, index;


    Object* targetObject = GetSelectedUnit();

    if (!targetObject)
        targetObject = GetSelectedCreature();

    if (!targetObject)
        targetObject = GetObjectHelper(stream, lowGuid, index);

    if (!targetObject)
        return false;

    if (!(stream >> index))
    {
        SendSysMessage("Wrong index.");
        return false;
    }

    std::string type;

    if (!(stream >> type))
    {
        SendSysMessage("Bad type. Try: float,uint,int");
        return false;
    }

    strToLower(type);

    if (type == "float")
        SendSysMessage(string_format("Value of index {} : {}", index, targetObject->GetFloatValue(index)).c_str());
    else if (type == "int")
        SendSysMessage(string_format("Value of index {} : {}", index, targetObject->GetInt32Value(index)).c_str());
    else
        SendSysMessage(string_format("Value of index {} : {}", index, targetObject->GetUInt32Value(index)).c_str());

    return true;
}


bool ChatHandler::HandleDebugFieldsModifyCommand(char* args)
{
    CommandStream stream{ args };

    auto sourcePlayer = GetPlayer();

    if (!sourcePlayer)
        return false;


    uint32 lowGuid, index;

    Object* targetObject = GetSelectedUnit();

    if (!targetObject)
      targetObject = GetSelectedCreature();

    if (!targetObject)
        targetObject = GetObjectHelper(stream, lowGuid, index);

    if (!targetObject)
        return false;

    uint32 newValue;

    if (!(stream >> newValue))
    {
        SendSysMessage("No good mod value given.");
        return false;
    }

    targetObject->SetUInt32Value(index, newValue);

    PSendSysMessage("Set index %u to %u.", index, newValue);


    return true;
}


bool ChatHandler::HandleDebugConditionCommand(char* args)
{
    int32 conditionId;

    if (!ExtractInt32(&args, conditionId))
        return false;

    Unit* pSource = GetSelectedUnit();
    Unit* pTarget = m_session->GetPlayer();

    if (pSource && pTarget)
    {
        if (IsConditionSatisfied(conditionId, pTarget, pSource->GetMap(), pSource, CONDITION_FROM_DBSCRIPTS))
            SendSysMessage("Condition is satisfied.");
        else
            SendSysMessage("Condition is not satisfied.");
    }

    return true;
}

bool ChatHandler::HandleDebugSetInstanceDataCommand(char* args)
{
	if (WorldSession* session = GetSession())
	{
		if (Player* player = session->GetPlayer())
		{
			if (InstanceData* pInstData = player->GetInstanceData())
			{
				uint32 Param1;
				uint32 Param2;
				if (!ExtractUInt32(&args, Param1))
					return false;
				if (!ExtractUInt32(&args, Param2))
					return false;
				pInstData->SetData(Param1, Param2);
			}
		}
	}
	return true;
}

bool ChatHandler::HandleCharacterInactivityDataCommand(char* args)
{
	if (WorldSession* session = GetSession())
	{
		if (Player* player = session->GetPlayer())
		{
			if (Player* selectedPlayer = player->GetSelectedPlayer())
			{
				uint32 currentTime = WorldTimer::getMSTime();

				uint32 deltaSpellCasted = WorldTimer::getMSTimeDiff(selectedPlayer->GetLastSpellCastedTime(), currentTime);
				uint32 deltaMovement = WorldTimer::getMSTimeDiff(selectedPlayer->GetLastMovementTime(), currentTime);
				uint32 deltaMessageChat = WorldTimer::getMSTimeDiff(selectedPlayer->GetLastChatMessageTime(), currentTime);

				float flDeltaSpellCasted = 0.0f;
				float flDeltaMovement = 0.0f;
				float flDeltaMessageChat = 0.0f;

				auto SafeTransformFloat = [](uint32 deltaTime) -> float
				{
					if (deltaTime != 0)
					{
						return float(deltaTime) / 1000.0f;
					}

					return 0.0f;
				};

				flDeltaSpellCasted = SafeTransformFloat(deltaSpellCasted);
				flDeltaMovement = SafeTransformFloat(deltaMovement);
				flDeltaMessageChat = SafeTransformFloat(deltaMessageChat);

				float VelocityPerSecond = selectedPlayer->GetVelocityPerSecond();
				float VelocityPer3Min = selectedPlayer->GetVelocityPer3Min();

				bool bInactivityResult = selectedPlayer->IsBasicallyInactive();

				PSendSysMessage("Player \"%s\" Delta spell: %f; Delta Movement: %f; Delta Chat: %f; VelocityPerSecond: %f; VelocityPer3Min: %f; Is Basically Inactive: %s",
					selectedPlayer->GetName(), flDeltaSpellCasted, flDeltaMovement, flDeltaMessageChat, VelocityPerSecond, VelocityPer3Min, GetOnOffStr(bInactivityResult));

				return true;
			}
		}
	}

    PSendSysMessage("Select player which you want to inspect");

	return false;
}

#ifdef USE_ANTICHEAT

bool ChatHandler::HandleSuspiciousEnable(char* args)
{
	bool bEnable = true;
	if (!ExtractOnOff(&args, bEnable)) return false;

	sSuspiciousStatisticMgr.SetEnable(bEnable);
	PSendSysMessage("Suspicious statistic global detection is %s", ChatHandler::GetOnOffStr(bEnable));
	return true;
}

bool ChatHandler::HandleSuspiciousMovementEnable(char* args)
{
	bool bEnable = true;
	if (!ExtractOnOff(&args, bEnable)) return false;

	sSuspiciousStatisticMgr.SetMovementEnable(bEnable);
	PSendSysMessage("Suspicious statistic movement anomaly detection is %s", ChatHandler::GetOnOffStr(bEnable));
	return true;
}

bool ChatHandler::HandleSuspiciousMovementDetectValue(char* args)
{
	float MovementDetectValue = 100.0f;
    if (!ExtractFloat(&args, MovementDetectValue))
    {
        float CurrentDetectionValue = sSuspiciousStatisticMgr.GetMovementDetectValue();
		PSendSysMessage("Current detection value is %f", CurrentDetectionValue);

        return true;
    }

	sSuspiciousStatisticMgr.SetMovementDetectValue(MovementDetectValue);
	PSendSysMessage("Suspicious statistic movement detection value set to %f", MovementDetectValue);
	return true;
}

bool ChatHandler::HandleSuspiciousFishingEnable(char* args)
{
	bool bEnable = true;
	if (!ExtractOnOff(&args, bEnable)) return false;

	sSuspiciousStatisticMgr.SetFishingEnable(bEnable);
	PSendSysMessage("Suspicious statistic fishing detection is %s", ChatHandler::GetOnOffStr(bEnable));
	return true;
}

bool ChatHandler::HandleSuspiciousKilledNPC(char* args)
{
	bool bEnable = true;
	if (!ExtractOnOff(&args, bEnable)) return false;

	sSuspiciousStatisticMgr.SetNpcKilledEnable(bEnable);
	PSendSysMessage("Suspicious statistic NPC kill detection is %s", ChatHandler::GetOnOffStr(bEnable));
	return true;
}

bool ChatHandler::HandleSuspiciousFishers(char* args)
{
	if (WorldSession* session = GetSession())
	{
		if (Player* player = session->GetPlayer())
		{
			sSuspiciousStatisticMgr.PrintAllActiveFishers(player);
		}
	}
	return true;
}

bool ChatHandler::HandleSuspiciousNotify(char* args)
{
	if (WorldSession* session = GetSession())
	{
		if (Player* player = session->GetPlayer())
		{
			bool bEnable = true;
			if (!ExtractOnOff(&args, bEnable)) return false;

			uint32 flags = session->GetAccountFlags();

			if (bEnable)
			{
				session->SetAccountFlags(flags | ACCOUNT_FLAG_SHOW_SUSPICIOUS);
				LoginDatabase.PExecute("UPDATE account SET flags = flags | 0x%x WHERE id = %u",
					session->GetAccountId(), ACCOUNT_FLAG_SHOW_SUSPICIOUS);
				SendSysMessage("Suspicious statistic messages will be shown");
			}
			else
			{
				session->SetAccountFlags(flags & ~ACCOUNT_FLAG_SHOW_SUSPICIOUS);
				LoginDatabase.PExecute("UPDATE account SET flags = flags & ~0x%x WHERE id = %u",
					session->GetAccountId(), ACCOUNT_FLAG_SHOW_SUSPICIOUS);
				SendSysMessage("Suspicious statistic messages will be hidden");
			}
		}
	}
	return true;
}

bool ChatHandler::HandleSuspiciousWhitelist(char* args)
{
    if (WorldSession* session = GetSession())
    {
		char* nameStr = ExtractQuotedOrLiteralArg(&args);
        if (!nameStr)
        {
            //PSendSysMessage("Please specify name", name.c_str());
            PSendSysMessage("All players in list:");
            for (ObjectGuid PlayerGuid : sSuspiciousStatisticMgr.WhitelistedPlayers)
            {
                std::string PlayerName;
                if (sObjectMgr.GetPlayerNameByGUID(PlayerGuid, PlayerName))
                {
                    PSendSysMessage("%s", PlayerName.c_str());
                }
                else
                {
                    PSendSysMessage("Guid: %llu", PlayerGuid.GetRawValue());
                }
            }

			return true;
        }
        else
        {
            std::string PlayerName = nameStr;
            ObjectGuid PossiblePlayerGuid = sObjectMgr.GetPlayerGuidByName(PlayerName);
            if (!PossiblePlayerGuid.IsEmpty())
            {
                PSendSysMessage("Adding player %s to whitelist", nameStr);
                sSuspiciousStatisticMgr.WhitelistedPlayers.insert(PossiblePlayerGuid);
            }
            else
            {
                PSendSysMessage("Can't find guid for player %s", nameStr);
                return false;
            }
        }

    }
    return true;
}

#endif

bool ChatHandler::HandleBlockEggsCommand(char* args)
{
    std::string accountName;
    uint32 accountId = ExtractAccountId(&args, &accountName, nullptr, false);
    if (!accountId)
        return false;

    auto data = sAccountMgr.GetAccountData(accountId);

    auto session = sWorld.FindSession(accountId);

    if (session)
    {
        if ((session->GetAccountFlags() & ACCOUNT_FLAG_BLOCK_EGG_PURCHASES) == ACCOUNT_FLAG_BLOCK_EGG_PURCHASES)
        {
            session->_accountFlags &= ~ACCOUNT_FLAG_BLOCK_EGG_PURCHASES;
            SendSysMessage("Removed Egg block.");
        }
        else
        {
            session->_accountFlags |= ACCOUNT_FLAG_BLOCK_EGG_PURCHASES;
            SendSysMessage("Added Egg block.");
        }
    }
    else
    {
        LoginDatabase.PExecute("UPDATE account SET flags = flags | %u WHERE id = %u", ACCOUNT_FLAG_BLOCK_EGG_PURCHASES, accountId);
        SendSysMessage("Added Egg block. (offline)");
    }

    return true;
}

bool ChatHandler::HandleBlacklistNameCommand(char* args)
{
    if (!*args)
        return false;

    std::string name = args;
    sObjectMgr.AddReservedName(name);
    WorldDatabase.escape_string(name);
    WorldDatabase.PExecute("REPLACE INTO `reserved_name` (`name`) VALUES ('%s')", name.c_str());

    PSendSysMessage("Name %s has been blacklisted.", name.c_str());
    return true;
}

bool ChatHandler::HandleListClickToMoveCommand(char* args)
{
    uint32 levelMin;
    if (!ExtractUInt32(&args, levelMin))
        levelMin = 1;

    uint32 levelMax;
    if (!ExtractUInt32(&args, levelMax))
        levelMax = 60;

    if (levelMin > levelMax)
        std::swap(levelMin, levelMax);

    std::multimap<uint32, Player*> levelSortedList;
    HashMapHolder<Player>::MapType const& plist = sObjectAccessor.GetPlayers();
    for (auto const& itr : plist)
    {
        if (!itr.second->IsInWorld())
            continue;

        uint32 const level = itr.second->GetLevel();
        if (level < levelMin || level > levelMax)
            continue;

        if (itr.second->GetSession()->HasUsedClickToMove())
            levelSortedList.insert(std::make_pair(level, itr.second));
    }

    if (levelSortedList.empty())
    {
        SendSysMessage("No players found.");
        return true;
    }

    SendSysMessage("Listing players using click to move:");
    for (auto const& itr : levelSortedList)
    {
        PSendSysMessage("- Name %s IP %s Level %s", GetNameLink(itr.second).c_str(), playerLink(itr.second->GetSession()->GetRemoteAddress()).c_str(), playerLink(std::to_string(itr.first)).c_str());
    }

    return true;
}

bool ChatHandler::HandleReloadQuestAreaTriggersCommand(char* /*args*/)
{
    sObjectMgr.LoadQuestAreaTriggers();
    SendSysMessage("DB table `areatrigger_involvedrelation` (quest area triggers) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAreaTriggerTavernCommand(char* /*args*/)
{
    sObjectMgr.LoadTavernAreaTriggers();
    SendSysMessage("DB table `areatrigger_tavern` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCharacterPetCommand(char* args)
{
    uint32 petId = 0;
    if (!ExtractUInt32(&args, petId))
        return false;
    if (!petId)
        return false;
    sCharacterDatabaseCache.LoadAll(petId);
    PSendSysMessage(">> Pet #%u reloaded from database.", petId);
    return true;
}

bool ChatHandler::HandleReloadConditionsCommand(char* /*args*/)
{
    sObjectMgr.LoadConditions();
    SendSysMessage("DB table `conditions` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCreatureCommand(char* /*args*/)
{
    sObjectMgr.LoadCreatures(true);
    SendSysMessage("DB table `creature` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCreatureTemplateCommand(char* args)
{
    uint32 entry;
    if (ExtractUInt32(&args, entry))
    {
        sObjectMgr.LoadCreatureTemplate(entry);
        PSendSysMessage("Creature template %u reloaded.", entry);
    }
    else
    {
        sObjectMgr.LoadCreatureTemplates();
        SendSysMessage("DB table `creature_template` reloaded.");
    }
    return true;
}

// Do not add separate reload command for scripts!
// EventAI events must be loaded right after.
bool ChatHandler::HandleReloadEventAIEventsCommand(char* args)
{
    sEventAIMgr.ClearEventData();

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `creature_ai_scripts`...");

    sScriptMgr.LoadCreatureEventAIScripts();

    if (*args != 'a')
        SendSysMessage("DB table `creature_ai_scripts` reloaded.");

    sLog.outInfo("Re-Loading Events from `creature_ai_events`...");
    sEventAIMgr.LoadCreatureEventAI_Events();
    SendSysMessage("DB table `creature_ai_events` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCreatureQuestInvRelationsCommand(char* /*args*/)
{
    sObjectMgr.LoadCreatureInvolvedRelations();
    SendSysMessage("DB table `creature_involvedrelation` (creature quest takers) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesCreatureCommand(char* /*args*/)
{
    LoadLootTemplates_Creature();
    LootTemplates_Creature.CheckLootRefs();
    SendSysMessage("DB table `creature_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCreatureQuestRelationsCommand(char* /*args*/)
{
    sObjectMgr.LoadCreatureQuestRelations();
    SendSysMessage("DB table `creature_questrelation` (creature quest givers) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCreatureSpellsCommand(char* /*args*/)
{
    sObjectMgr.LoadCreatureSpells();
    SendSysMessage("DB table `creature_spells` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCreatureSpellScriptsCommand(char* args)
{
    if (sScriptMgr.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `creature_spells_scripts`...");

    sScriptMgr.LoadCreatureSpellScripts();

    if (*args != 'a')
        SendSysMessage("DB table `creature_spells_scripts` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadLootTemplatesDisenchantCommand(char* /*args*/)
{
    LoadLootTemplates_Disenchant();
    LootTemplates_Disenchant.CheckLootRefs();
    SendSysMessage("DB table `disenchant_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadEventScriptsCommand(char* args)
{
    if (sScriptMgr.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `event_scripts`...");

    sScriptMgr.LoadEventScripts();

    if (*args != 'a')
        SendSysMessage("DB table `event_scripts` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadLootTemplatesFishingCommand(char* /*args*/)
{
    LoadLootTemplates_Fishing();
    LootTemplates_Fishing.CheckLootRefs();
    SendSysMessage("DB table `fishing_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGameGraveyardZoneCommand(char* /*args*/)
{
    sObjectMgr.LoadGraveyardZones();
    SendSysMessage("DB table `game_graveyard_zone` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGameTeleCommand(char* /*args*/)
{
    sObjectMgr.LoadGameTele();
    SendSysMessage("DB table `game_tele` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadTaxiPathTransitionsCommand(char* /*args*/)
{
    sObjectMgr.LoadTaxiPathTransitions();
    SendSysMessage("DB table `taxi_path_transitions` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGameObjectCommand(char* /*args*/)
{
    sObjectMgr.LoadGameobjects(true);
    SendSysMessage("DB table `gameobject` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGOQuestInvRelationsCommand(char* /*args*/)
{
    sObjectMgr.LoadGameobjectInvolvedRelations();
    SendSysMessage("DB table `gameobject_involvedrelation` (gameobject quest takers) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesGameobjectCommand(char* /*args*/)
{
    LoadLootTemplates_Gameobject();
    LootTemplates_Gameobject.CheckLootRefs();
    SendSysMessage("DB table `gameobject_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGOQuestRelationsCommand(char* /*args*/)
{
    sObjectMgr.LoadGameobjectQuestRelations();
    SendSysMessage("DB table `gameobject_questrelation` (gameobject quest givers) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGORequirementsCommand(char* /*args*/)
{
    sObjectMgr.LoadGameobjectsRequirements();
    SendSysMessage("DB table `gameobject_requirement` (Game Objects requirements) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGameObjectScriptsCommand(char* args)
{
    if (sScriptMgr.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `gameobject_scripts`...");

    sScriptMgr.LoadGameObjectScripts();

    if (*args != 'a')
        SendSysMessage("DB table `gameobject_scripts` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadBattleEventCommand(char* /*args*/)
{
    sBattleGroundMgr.LoadBattleEventIndexes();
    SendSysMessage("DB table `gameobject_battleground` and `creature_battleground` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGossipMenuCommand(char* /*args*/)
{
    std::set<uint32> gossipScriptSet;
    for (const auto& itr : sGossipScripts)
        gossipScriptSet.insert(itr.first);
    sObjectMgr.LoadGossipMenu(gossipScriptSet);
    SendSysMessage("DB table `gossip_menu` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGossipMenuOptionCommand(char* /*args*/)
{
    std::set<uint32> gossipScriptSet;
    for (const auto& itr : sGossipScripts)
        gossipScriptSet.insert(itr.first);
    sObjectMgr.LoadGossipMenuItems(gossipScriptSet);
    SendSysMessage("DB table `gossip_menu_option` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGenericScriptsCommand(char* args)
{
    if (sScriptMgr.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `generic_scripts`...");

    sScriptMgr.LoadGenericScripts();

    if (*args != 'a')
        SendSysMessage("DB table `generic_scripts` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadGossipScriptsCommand(char* args)
{
    if (sScriptMgr.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `gossip_scripts`...");

    sScriptMgr.LoadGossipScripts();

    if (*args != 'a')
        SendSysMessage("DB table `gossip_scripts` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadItemEnchantementsCommand(char* /*args*/)
{
    LoadRandomEnchantmentsTable();
    SendSysMessage("DB table `item_enchantment_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesItemCommand(char* /*args*/)
{
    LoadLootTemplates_Item();
    LootTemplates_Item.CheckLootRefs();
    SendSysMessage("DB table `item_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadItemRequiredTragetCommand(char* /*args*/)
{
    sObjectMgr.LoadItemRequiredTarget();
    SendSysMessage("DB table `item_required_target` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesCreatureCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading Locales Creature ...");
    sObjectMgr.LoadCreatureLocales();
    SendSysMessage("DB table `locales_creature` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesGameobjectCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading Locales Gameobject ... ");
    sObjectMgr.LoadGameObjectLocales();
    SendSysMessage("DB table `locales_gameobject` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesGossipMenuOptionCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading Locales Gossip Menu Option ... ");
    sObjectMgr.LoadGossipMenuItemsLocales();
    SendSysMessage("DB table `locales_gossip_menu_option` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesItemCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading Locales Item ... ");
    sObjectMgr.LoadItemLocales();
    SendSysMessage("DB table `locales_item` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesPageTextCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading Locales Page Text ... ");
    sObjectMgr.LoadPageTextLocales();
    SendSysMessage("DB table `locales_page_text` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesPointsOfInterestCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading Locales Points Of Interest ... ");
    sObjectMgr.LoadPointOfInterestLocales();
    SendSysMessage("DB table `locales_points_of_interest` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLocalesQuestCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading Locales Quest ... ");
    sObjectMgr.LoadQuestLocales();
    SendSysMessage("DB table `locales_quest` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesMailCommand(char* /*args*/)
{
    LoadLootTemplates_Mail();
    LootTemplates_Mail.CheckLootRefs();
    SendSysMessage("DB table `mail_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadNpcGossipCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading `npc_gossip` Table!");
    sObjectMgr.LoadNpcGossips();
    SendSysMessage("DB table `npc_gossip` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadNpcTextCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading `npc_text` Table!");
    sObjectMgr.LoadNPCText();
    SendSysMessage("DB table `npc_text` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadNpcTrainerCommand(char* /*args*/)
{
    sLog.outInfo("Re-Loading `npc_trainer_template` Table!");
    sObjectMgr.LoadTrainerTemplates();
    SendSysMessage("DB table `npc_trainer_template` reloaded.");

    sLog.outInfo("Re-Loading `npc_trainer` Table!");
    sObjectMgr.LoadTrainers();
    SendSysMessage("DB table `npc_trainer` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadNpcVendorCommand(char* /*args*/)
{
    // not safe reload vendor template tables independent...
    sLog.outInfo("Re-Loading `npc_vendor_template` Table!");
    sObjectMgr.LoadVendorTemplates();
    SendSysMessage("DB table `npc_vendor_template` reloaded.");

    sLog.outInfo("Re-Loading `npc_vendor` Table!");
    sObjectMgr.LoadVendors();
    SendSysMessage("DB table `npc_vendor` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadPageTextsCommand(char* /*args*/)
{
    sObjectMgr.LoadPageTexts();
    SendSysMessage("DB table `page_texts` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesPickpocketingCommand(char* /*args*/)
{
    LoadLootTemplates_Pickpocketing();
    LootTemplates_Pickpocketing.CheckLootRefs();
    SendSysMessage("DB table `pickpocketing_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadPointsOfInterestCommand(char* /*args*/)
{
    sObjectMgr.LoadPointsOfInterest();
    SendSysMessage("DB table `points_of_interest` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadQuestEndScriptsCommand(char* args)
{
    if (sScriptMgr.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `quest_end_scripts`...");

    sScriptMgr.LoadQuestEndScripts();

    if (*args != 'a')
        SendSysMessage("DB table `quest_end_scripts` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadQuestStartScriptsCommand(char* args)
{
    if (sScriptMgr.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `quest_start_scripts`...");

    sScriptMgr.LoadQuestStartScripts();

    if (*args != 'a')
        SendSysMessage("DB table `quest_start_scripts` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadQuestGreetingCommand(char* /*args*/)
{
    sObjectMgr.LoadQuestGreetings();
    SendSysMessage("DB table `quest_greeting` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadTrainerGreetingCommand(char* /*args*/)
{
    sObjectMgr.LoadTrainerGreetings();
    SendSysMessage("DB table `npc_trainer_greeting` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesReferenceCommand(char* /*args*/)
{
    LootIdSet ids_set;
    LoadLootTemplates_Reference(ids_set);
    CheckLootTemplates_Reference(ids_set);
    SendSysMessage("DB table `reference_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadReservedNameCommand(char* /*args*/)
{
    sObjectMgr.LoadReservedPlayersNames();
    SendSysMessage("DB table `reserved_name` (player reserved names) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadReputationRewardRateCommand(char* /*args*/)
{
    sObjectMgr.LoadReputationRewardRate();
    SendSysMessage("DB table `reputation_reward_rate` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadReputationSpilloverTemplateCommand(char* /*args*/)
{
    sObjectMgr.LoadReputationSpilloverTemplate();
    SendSysMessage("DB table `reputation_spillover_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSkillFishingBaseLevelCommand(char* /*args*/)
{
    sObjectMgr.LoadFishingBaseSkillLevel();
    SendSysMessage("DB table `skill_fishing_base_level` (fishing base level for zone/subzone) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadLootTemplatesSkinningCommand(char* /*args*/)
{
    LoadLootTemplates_Skinning();
    LootTemplates_Skinning.CheckLootRefs();
    SendSysMessage("DB table `skinning_loot_template` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellAffectCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellAffects();
    SendSysMessage("DB table `spell_affect` (spell mods apply requirements) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellAreaCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellAreas();
    SendSysMessage("DB table `spell_area` (spell dependences from area/quest/auras state) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellChainCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellChains();
    SendSysMessage("DB table `spell_chain` (spell ranks) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellElixirCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellElixirs();
    SendSysMessage("DB table `spell_elixir` (spell elixir types) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellLearnSpellCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellLearnSpells();
    SendSysMessage("DB table `spell_learn_spell` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellPetAurasCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellPetAuras();
    SendSysMessage("DB table `spell_pet_auras` reloaded.");
    return true;
}


bool ChatHandler::HandleReloadSpellProcEventCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellProcEvents();
    SendSysMessage("DB table `spell_proc_event` (spell proc trigger requirements) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellProcItemEnchantCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellProcItemEnchant();
    SendSysMessage("DB table `spell_proc_item_enchant` (item enchantment ppm) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellScriptTargetCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellScriptTarget();
    SendSysMessage("DB table `spell_script_target` (spell targets selection in case specific creature/GO requirements) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellScriptsCommand(char* args)
{
    if (sScriptMgr.IsScriptScheduled())
    {
        SendSysMessage("DB scripts used currently, please attempt reload later.");
        SetSentErrorMessage(true);
        return false;
    }

    if (*args != 'a')
        sLog.outInfo("Re-Loading Scripts from `spell_scripts`...");

    sScriptMgr.LoadSpellScripts();

    if (*args != 'a')
        SendSysMessage("DB table `spell_scripts` reloaded.");

    return true;
}

bool ChatHandler::HandleReloadSpellTargetPositionCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellTargetPositions();
    SendSysMessage("DB table `spell_target_position` (destination coordinates for spell targets) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellThreatsCommand(char* /*args*/)
{
    sSpellMgr.LoadSpellThreats();
    SendSysMessage("DB table `spell_threat` (spell aggro definitions) reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellModsCommand(char* args)
{
    sSpellModMgr.LoadSpellMods();
    SendSysMessage("DB table `spell_mod` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadMapLootDisabledCommand(char* args)
{
    sObjectMgr.LoadMapLootDisabled();
    SendSysMessage("DB table `map_loot_disabled` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCinematicWaypointsCommand(char* args)
{
    sObjectMgr.LoadCinematicsWaypoints();
    SendSysMessage("DB table `cinematic_waypoints` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellGroupCommand(char*)
{
    sSpellMgr.LoadSpellGroups();
    SendSysMessage("Table `spell_group` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadSpellGroupStackRulesCommand(char*)
{
    sSpellMgr.LoadSpellGroupStackRules();
    SendSysMessage("Table `spell_group_stack_rules` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadExplorationBaseXp(char*)
{
    sObjectMgr.LoadExplorationBaseXP();
    SendSysMessage(">> Table `exploration_basexp` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadPetNameGeneration(char*)
{
    sObjectMgr.LoadPetNames();
    SendSysMessage(">> Table `pet_name_generation` reloaded.");
    return true;
}


bool ChatHandler::HandleReloadCreatureOnKillReputation(char*)
{
    sObjectMgr.LoadReputationOnKill();
    SendSysMessage(">> Table `creature_onkill_reputation` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadGameWeather(char*)
{
    sWeatherMgr.LoadWeatherZoneChances();
    SendSysMessage(">> Table `game_weather` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadFactionChangeReputations(char*)
{
    sObjectMgr.LoadFactionChangeReputations();
    SendSysMessage(">> Table `player_factionchange_reputations` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadFactionChangeSpells(char*)
{
    sObjectMgr.LoadFactionChangeSpells();
    SendSysMessage(">> Table `player_factionchange_spells` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadFactionChangeItems(char*)
{
    sObjectMgr.LoadFactionChangeItems();
    SendSysMessage(">> Table `player_factionchange_items` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadFactionChangeQuests(char*)
{
    sObjectMgr.LoadFactionChangeQuests();
    SendSysMessage(">> Table `player_factionchange_quests` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadFactionChangeMounts(char*)
{
    sObjectMgr.LoadFactionChangeMounts();
    SendSysMessage(">> Table `player_factionchange_mounts` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadCreatureDisplayInfoAddon(char*)
{
    sObjectMgr.LoadCreatureDisplayInfoAddon();
    SendSysMessage(">> Table `creature_display_info_addon` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadIPBanList(char*)
{
    sAccountMgr.LoadIPBanList();
    SendSysMessage(">> Table `ip_banned` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadAccountBanList(char*)
{
    sAccountMgr.LoadAccountBanList();
    SendSysMessage(">> Table `account_banned` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadInstanceBuffRemoval(char*)
{
    sAuraRemovalMgr.LoadFromDB();
    SendSysMessage(">> Table `instance_buff_removal` reloaded.");
    return true;
}

bool ChatHandler::HandleReloadPetitions(char*)
{
    sGuildMgr.LoadPetitions();
    SendSysMessage(">> Table `petition` reloaded.");
    return true;
}

bool ChatHandler::HandleQuestStatusesCommand(char* args)
{
    auto player = GetPlayer();

    const uint32 MaxMessageSize = 2000;
    const uint32 ReserveSize = 2048;

    std::string msg;
    msg.reserve(ReserveSize);
    for (const auto& elem : player->getQuestStatusMap())
    {
        if (elem.second.m_rewarded && elem.second.m_status != QUEST_STATUS_NONE)
            msg += std::to_string(elem.first) + " ";

        if (msg.length() >= MaxMessageSize)
        {
            player->SendAddonMessage("TWQUEST", std::move(msg));
            msg.clear();
        }
    }
    player->SendAddonMessage("TWQUEST", std::move(msg));

    return true;
}

bool ChatHandler::HandleAccountEmailCommand(char* args)
{
    char* oldEmail = ExtractLiteralArg(&args);
    char* newEmail = ExtractLiteralArg(&args);
    char* newEmail2 = ExtractLiteralArg(&args);

    if (!oldEmail || !newEmail || !newEmail2)
        return false;

    if (strcmp(oldEmail, newEmail) == NULL)
    {
        SendSysMessage("New email must be different than old.");
        SetSentErrorMessage(true);
        return false;
    }

    if (strcmp(newEmail, newEmail2) != NULL)
    {
        SendSysMessage("Email confirmation doesn't match.");
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetEmail() != oldEmail)
    {
        SendSysMessage("Old email doesn't match.");
        SetSentErrorMessage(true);
        return false;
    }

    if (strchr(newEmail, '@') == NULL)
    {
        SendSysMessage("New email is not valid.");
        SetSentErrorMessage(true);
        return false;
    }

    std::string newEmailStr = newEmail;
    LoginDatabase.escape_string(newEmailStr);
    LoginDatabase.PExecute("UPDATE `account` SET `email`='%s' WHERE `id`=%u", newEmailStr.c_str(), m_session->GetAccountId());
    m_session->SetEmail(newEmail);

    SendSysMessage("Email changed.");
    return true;
}

#include "events/event_wareffort.h"

bool ChatHandler::HandleWarEffortInfoCommand(char* args)
{
    sGameEventMgr.Update();

    uint32 stage = sObjectMgr.GetSavedVariable(VAR_WE_STAGE, WAR_EFFORT_STAGE_COLLECTION);
    PSendSysMessage("Stage: %s (%u)", WarEffortStageToString(stage), stage);

    uint32 lastStageTransitionTime = sObjectMgr.GetSavedVariable(VAR_WE_STAGE_TRANSITION_TIME, 0);
    PSendSysMessage("Last Transition Time: %s (%u)", TimeToTimestampStr(lastStageTransitionTime).c_str(), lastStageTransitionTime);

    uint32 gongRingTime = sObjectMgr.GetSavedVariable(VAR_WE_GONG_TIME, 0);
    PSendSysMessage("Gong Ring Time: %s (%u)", TimeToTimestampStr(gongRingTime).c_str(), gongRingTime);

    switch (stage)
    {
        case WAR_EFFORT_STAGE_COLLECTION:
        {
            uint32 lastAutoCompleteTime = sObjectMgr.GetSavedVariable(VAR_WE_AUTOCOMPLETE_TIME, 0);
            PSendSysMessage("Last Auto Complete Time: %s (%u)", TimeToTimestampStr(lastAutoCompleteTime).c_str(), lastAutoCompleteTime);

            uint32 nextAutoCompleteIn = sWorld.getConfig(CONFIG_UINT32_WAR_EFFORT_AUTOCOMPLETE_PERIOD) - (time(nullptr) - lastAutoCompleteTime);
            PSendSysMessage("Next Auto Complete In: %s", secsToTimeString(nextAutoCompleteIn).c_str());

            uint32 remainingResources = 0;

            // Check all totals. If we're at the limit, start the moving.
            for (int i = 0; i < NUM_SHARED_OBJECTIVES; ++i)
            {
                WarEffortStockInfo info;
                if (GetWarEffortStockInfo(SharedObjectives[i].itemId, info, TEAM_ALLIANCE))
                {
                    ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(SharedObjectives[i].itemId);

                    if (info.count < info.required)
                    {
                        ++remainingResources;
                        PSendSysMessage("Alliance %s: %u / %u", GetItemLink(pProto).c_str(), info.count, info.required);
                    }
                }

                if (GetWarEffortStockInfo(SharedObjectives[i].itemId, info, TEAM_HORDE))
                {
                    ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(SharedObjectives[i].itemId);

                    if (info.count < info.required)
                    {
                        ++remainingResources;
                        PSendSysMessage("Horde %s: %u / %u", GetItemLink(pProto).c_str(), info.count, info.required);
                    }
                }
            }

            for (int i = 0; i < NUM_FACTION_OBJECTIVES; ++i)
            {
                WarEffortStockInfo info;
                if (GetWarEffortStockInfo(AllianceObjectives[i].itemId, info))
                {
                    ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(AllianceObjectives[i].itemId);

                    if (info.count < info.required)
                    {
                        ++remainingResources;
                        PSendSysMessage("Alliance %s: %u / %u", GetItemLink(pProto).c_str(), info.count, info.required);
                    }
                }

                if (GetWarEffortStockInfo(HordeObjectives[i].itemId, info))
                {
                    ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(HordeObjectives[i].itemId);
                    
                    if (info.count < info.required)
                    {
                        ++remainingResources;
                        PSendSysMessage("Horde %s: %u / %u", GetItemLink(pProto).c_str(), info.count, info.required);
                    }
                }
            }

            PSendSysMessage("Total Remaining Resources: %u", remainingResources);

            break;
        }
        case WAR_EFFORT_STAGE_MOVE_1:
        case WAR_EFFORT_STAGE_MOVE_2:
        case WAR_EFFORT_STAGE_MOVE_3:
        case WAR_EFFORT_STAGE_MOVE_4:
        case WAR_EFFORT_STAGE_MOVE_5:
        {
            uint32 nextAutoCompleteIn = WAR_EFFORT_MOVE_TRANSITION_TIME - (time(nullptr) - lastStageTransitionTime);
            PSendSysMessage("Next Transition In: %s", secsToTimeString(nextAutoCompleteIn).c_str());
            break;
        }
        case WAR_EFFORT_STAGE_BATTLE:
        {
            uint32 nextTransitionIn = WAR_EFFORT_CH_ATTACK_TIME - (time(nullptr) - lastStageTransitionTime);
            PSendSysMessage("Next Transition In: %s", secsToTimeString(nextTransitionIn).c_str());
            break;
        }
        case WAR_EFFORT_STAGE_CH_ATTACK:
        {
            uint32 nextTransitionIn = WAR_EFFORT_FINAL_BATTLE_TIME - (time(nullptr) - lastStageTransitionTime);
            PSendSysMessage("Next Transition In: %s", secsToTimeString(nextTransitionIn).c_str());
            break;
        }
        case WAR_EFFORT_STAGE_FINALBATTLE:
        {
            uint32 nextTransitionIn = WAR_EFFORT_GONG_DURATION - (time(nullptr) - gongRingTime);
            PSendSysMessage("Next Transition In: %s", secsToTimeString(nextTransitionIn).c_str());
            break;
        }
    }

    return true;
}

bool ChatHandler::HandleWarEffortSetGongTimeCommand(char* args)
{
    uint32 gongTime;
    if (!ExtractUInt32(&args, gongTime))
        return false;

    sObjectMgr.SetSavedVariable(VAR_WE_GONG_TIME, gongTime, true);
    PSendSysMessage("War effort gong ring time set to '%s' (%u).", TimeToTimestampStr(gongTime).c_str(), gongTime);
    sGameEventMgr.Update();

    return true;
}

bool ChatHandler::HandleWarEffortSetStageCommand(char* args)
{
    uint32 stage;
    if (!ExtractUInt32(&args, stage))
        return false;

    sObjectMgr.SetSavedVariable(VAR_WE_STAGE, stage, true);
    sObjectMgr.SetSavedVariable(VAR_WE_STAGE_TRANSITION_TIME, time(nullptr), true);
    PSendSysMessage("War effort stage set to '%s' (%u).", WarEffortStageToString(stage), stage);
    sGameEventMgr.Update();

    return true;
}

bool ChatHandler::HandlePerfStatsCommand(char* args)
{
    SendSysMessage("Showing performance statistics:");
    PSendSysMessage("Total Units: %i", PerfStats::g_totalUnits);
    PSendSysMessage("Total Creatures: %i", PerfStats::g_totalCreatures);
    PSendSysMessage("Total Pets: %i", PerfStats::g_totalPets);
    PSendSysMessage("Total Players: %i", PerfStats::g_totalPlayers);
    PSendSysMessage("Total Corpses: %i", PerfStats::g_totalCorpses);
    PSendSysMessage("Total Items: %i", PerfStats::g_totalItems);
    PSendSysMessage("Total GameObjects: %i", PerfStats::g_totalGameObjects);
    PSendSysMessage("Total DynamicObjects: %i", PerfStats::g_totalDynamicObjects);
    PSendSysMessage("Total QueryResults: %i", PerfStats::g_totalQueryResults);
    PSendSysMessage("Total Maps: %i", PerfStats::g_totalMaps);
    PSendSysMessage("Slowest Map: %i (%i ms)", PerfStats::g_slowestMapId, PerfStats::g_slowestMapUpdateTime);

    return true;
}

bool ChatHandler::HandlePerfEnable(char* Args)
{
	bool bEnable = true;
	if (!ExtractOnOff(&Args, bEnable))
    {
        bool bIsEnabled = sWorld.getConfig(CONFIG_BOOL_PERFORMANCE_ENABLE);

        PSendSysMessage("Performance stats is %s", ChatHandler::GetOnOffStr(bIsEnabled));
        return true;
    }

    g_bEnableStatGather = bEnable;
    sWorld.setConfig(CONFIG_BOOL_PERFORMANCE_ENABLE, bEnable);
	PSendSysMessage("Performance stats now is %s", ChatHandler::GetOnOffStr(bEnable));

    return true;
}

bool ChatHandler::HandlePerfIntervalReport(char* Args)
{
    uint32 NewInterval = 0;
    if (!ExtractUInt32(&Args, NewInterval))
    {
        uint32 CurrentInterval = sWorld.getConfig(CONFIG_UINT32_PERFORMANCE_REPORT_INTERVAL);
		PSendSysMessage("Performance report interval is %u", CurrentInterval);

        return false;
    }

    sWorld.setConfig(CONFIG_UINT32_PERFORMANCE_REPORT_INTERVAL, NewInterval);
    sPerfMonitor.SetReportInterval(NewInterval);
    PSendSysMessage("Performance report interval now is %u", NewInterval);

    return true;
}

bool ChatHandler::HandlePerfReportCPU(char* Args)
{
    sPerfMonitor.ReportCPU(*this);
    return true;
}

bool ChatHandler::HandlePerfReportMemory(char* Args)
{
    sPerfMonitor.ReportMemory(*this);
    return true;
}


bool ChatHandler::HandleDebugLeakReportCommand(char* args)
{
#ifndef ENABLE_LSAN
    SendSysMessage("Can't generate leak report, core is not built with LSAN.");
    return true;
#endif

    SendSysMessage("Now snapshotting leaks, this might cause lag.");
    uint32 now = WorldTimer::getMSTime();
#ifdef ENABLE_LSAN
    __lsan_do_recoverable_leak_check();
#endif
    PSendSysMessage("Snapshot completed in %u ms. Check %s for data.", WorldTimer::getMSTimeDiffToNow(now), lsan_output_path);
    return true;
}

bool ChatHandler::HandleDebugCoeffsCommand(char* args)
{

    CommandStream stream{ args };

    uint32 spellId = 0;
    stream >> spellId;

    std::string damageHeal = "";

    auto spellProto = sSpellMgr.GetSpellEntry(spellId);

    if (!spellProto)
    {
        PSendSysMessage("Spell %u does not exist.", spellId);
        return false;
    }

    stream >> damageHeal;

    auto type = DIRECT_DAMAGE;

    if (damageHeal == "heal")
        type = HEAL;



    for (uint32 i = 0; i < MAX_SPELL_EFFECTS; ++i)
    {
        float coeff;

        // Check for table values
        if (spellProto->EffectBonusCoefficient[i] >= 0.0f)
            coeff = spellProto->EffectBonusCoefficient[i];
        // Calculate default coefficient
        else
            coeff = spellProto->CalculateDefaultCoefficient(type);

        // Calculate custom coefficient
        coeff = spellProto->CalculateCustomCoefficient(GetPlayer(), type, coeff, nullptr, true);

        PSendSysMessage("Coeff for spellIndex %u : %f", i, coeff);
    }

    return true;
}


bool ChatHandler::HandleDebugPacketStatsCommand(char* args)
{
    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    if (sWorld.m_collectPacketStats == value)
    {
        PSendSysMessage("Packet statistics collecting is already %s.", value ? "on" : "off");
        SetSentErrorMessage(true);
        return false;
    }

    if (value)
    {
        for (uint32 i = 0; i < NUM_MSG_TYPES; ++i)
        {
            sWorld.m_packetsCount[i] = 0;
            sWorld.m_packetsSize[i] = 0;
        }
        SendSysMessage("Beginning collection of packet statistics.");
    }

    sWorld.m_collectPacketStats = value;

    std::vector<std::pair<uint64 /*count*/, uint64 /*opcode*/>> opcodesByCount;
    std::vector<std::pair<uint64 /*size*/, uint64 /*opcode*/>> opcodesBySize;

    if (!value)
    {
        for (uint32 i = 0; i < NUM_MSG_TYPES; ++i)
        {
            opcodesByCount.push_back(std::make_pair<uint64, uint64>((uint64)sWorld.m_packetsCount[i], (uint64)i));
            opcodesBySize.push_back(std::make_pair<uint64, uint64>((uint64)sWorld.m_packetsSize[i], (uint64)i));
        }

        std::sort(opcodesByCount.begin(),
            opcodesByCount.end(),
            [](std::pair<uint64, uint64> a, std::pair<uint64, uint64> b)
            {return a.first > b.first; });

        std::sort(opcodesBySize.begin(),
            opcodesBySize.end(),
            [](std::pair<uint64, uint64> a, std::pair<uint64, uint64> b)
            {return a.first > b.first; });

        SendSysMessage("Top 5 by count:");
        for (uint32 i = 0; i < 5; ++i)
        {
            PSendSysMessage("%u. %s - %u", i + 1, LookupOpcodeName(opcodesByCount[i].second), opcodesByCount[i].first);
        }

        SendSysMessage("Top 5 by size:");
        for (uint32 i = 0; i < 5; ++i)
        {
            PSendSysMessage("%u. %s - %u", i + 1, LookupOpcodeName(opcodesBySize[i].second), opcodesBySize[i].first);
        }

        if (FILE* pFile = fopen("packetstats.txt", "w"))
        {
            SendSysMessage("Writing full statistics to file packetstats.txt");

            fprintf(pFile, "Packets by count:\n");
            for (auto const& itr : opcodesByCount)
            {
                if (itr.first != 0)
                    fprintf(pFile, "%s - %llu\n", LookupOpcodeName(itr.second), itr.first);
            }

            fprintf(pFile, "\nPackets by size:\n");
            for (auto const& itr : opcodesBySize)
            {
                if (itr.first != 0)
                    fprintf(pFile, "%s - %llu\n", LookupOpcodeName(itr.second), itr.first);
            }

            fclose(pFile);
        }
    }

    return true;
}

bool ChatHandler::HandleCharacterGetNameCommand(char* args)
{
    uint32 guidLow;
    if (!ExtractUInt32(&args, guidLow))
        return false;

    ObjectGuid guid(HIGHGUID_PLAYER, guidLow);

    std::string name;
    if (!sObjectMgr.GetPlayerNameByGUID(guid, name))
    {
        SendSysMessage("Player not found.");
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Guid %u is named '%s'.", guidLow, name.c_str());

    return true;
}

bool ChatHandler::HandleAccountGetNameCommand(char* args)
{
    uint32 accountId;
    if (!ExtractUInt32(&args, accountId))
        return false;

    AccountData* pData = sAccountMgr.GetAccountData(accountId);
    if (!pData)
    {
        SendSysMessage("Account not found.");
        SetSentErrorMessage(true);
        return false;
    }

    PSendSysMessage("Account %u is named '%s'.", accountId, pData->Username.c_str());

    return true;
}
