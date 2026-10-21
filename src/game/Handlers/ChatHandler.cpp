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

#include "Common.h"
#include "Log.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "World.h"
#include "Opcodes.h"
#include "ObjectMgr.h"
#include "Chat.h"
#include "Database/DatabaseEnv.h"
#include "ChannelMgr.h"
#include "Group.h"
#include "LFTMgr.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Player.h"
#include "ScriptObjects.h"
#include "SpellAuras.h"
#include "Language.h"
#include "Util.h"
#include "Unit.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Anticheat.h"
#include "AccountMgr.h"
#include "Config/Config.h"
#include "CustomMerchantMgr.h"
#include "BarbershopMgr.h"
#include "Database/DatabaseImpl.h"
#include "HonorMgr.h"
#include "Shop/ShopMgr.h"
#include "GMTicketMgr.h"


#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include <regex>
#include "re2/re2.h"

bool WorldSession::CheckChatMessageValidity(std::string& msg, uint32 lang, uint32 msgType)
{
    if (!IsLanguageAllowedForChatType(lang, msgType))
        return false;

    if (lang != LANG_ADDON)
    {
        // strip invisible characters for non-addon messages
        if (sWorld.getConfig(CONFIG_BOOL_CHAT_FAKE_MESSAGE_PREVENTING))
            stripLineInvisibleChars(msg);

        if (sWorld.getConfig(CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_SEVERITY) && !ChatHandler(this).isValidChatMessage(msg.c_str()))
        {
            sLog.outError("Player %s (GUID: %u) sent a chatmessage with an invalid link: %s", GetPlayer()->GetName(),
                GetPlayer()->GetGUIDLow(), msg.c_str());
            if (sWorld.getConfig(CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_KICK))
                KickPlayer();
            return false;
        }
    }

    return true;
}

bool WorldSession::ProcessChatMessageAfterSecurityCheck(std::string& msg, uint32& lang, uint32& msgType)
{
    if (!CheckChatMessageValidity(msg, lang, msgType))
        return false;

    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_BEFORE_SEND_CHAT_MESSAGE, [&](PlayerScript* script)
    {
        script->OnBeforeSendChatMessage(GetPlayer(), msgType, lang, msg);
    });

    ChatHandler handler(this);

    return !handler.ParseCommands(msg.c_str());
}

bool WorldSession::IsLanguageAllowedForChatType(uint32 lang, uint32 msgType)
{
    // Right now we'll just restrict addon language to the appropriate chat types
    // Anything else is OK (default previous behaviour)
    switch (lang)
    {
        case LANG_ADDON:
        {
            switch (msgType)
            {
                case CHAT_MSG_PARTY:
                case CHAT_MSG_GUILD:
                case CHAT_MSG_OFFICER:
                case CHAT_MSG_RAID:
                case CHAT_MSG_RAID_LEADER:
                case CHAT_MSG_RAID_WARNING:
                case CHAT_MSG_BATTLEGROUND:
                case CHAT_MSG_BATTLEGROUND_LEADER:
                case CHAT_MSG_CHANNEL:
                    return true;
                default:
                    return false;
            }
        }
        default:
            return true;
    }

    return true;
}

uint32_t WorldSession::ChatCooldown()
{
    ASSERT(GetPlayer());

    auto cooldown = sWorld.getConfig(CONFIG_UINT32_WORLD_CHAN_CD);
    const auto minLevel = sWorld.getConfig(CONFIG_UINT32_WORLD_CHAN_MIN_LEVEL);
    const auto cooldownMaxLvl = sWorld.getConfig(CONFIG_UINT32_WORLD_CHAN_CD_MAX_LEVEL);
    const auto cooldownScaling = sWorld.getConfig(CONFIG_UINT32_WORLD_CHAN_CD_SCALING);
    const auto cooldownUseAcctLvl = sWorld.getConfig(CONFIG_UINT32_WORLD_CHAN_CD_USE_ACCOUNT_MAX_LEVEL);
    const auto playerLevel = cooldownUseAcctLvl? GetAccountMaxLevel() : GetPlayer()->GetLevel();

    if (cooldown && cooldownMaxLvl > playerLevel)
    {
        const auto currTime = time(nullptr);
        const auto delta = currTime - GetLastPubChanMsgTime();

        if (cooldownScaling)
        {
            auto factor = static_cast<double>((cooldownMaxLvl - playerLevel)) / (cooldownMaxLvl - minLevel);
            cooldown *= factor;
        }

        if (delta < cooldown)
        {
            return cooldown - delta;
        }
    }

    return 0;
}

bool EnforceEnglish(WorldSession* session, const std::string& msg)
{
    if (!sWorld.getConfig(CONFIG_BOOL_ENFORCED_ENGLISH))
        return false;

    std::wstring w_normMsg;
    if (!Utf8toWStr(msg, w_normMsg))
    {
        ChatHandler(session).SendSysMessage("Don't use invalid characters in public chats!");
        return true;
    }
    if (!IsAsciiOnly(w_normMsg))
        return true;
    return false;
}

void WorldSession::HandleMessagechatOpcode(WorldPacket & recv_data)
{
    uint32 now = WorldTimer::getMSTime();
    uint32 type;
    uint32 lang;

    recv_data >> type;
    recv_data >> lang;

    if (type >= MAX_CHAT_MSG_TYPE)
    {
        sLog.outError("CHAT: Wrong message type received: %u", type);
        return;
    }

    DEBUG_LOG("CHAT: packet received. type %u, lang %u", type, lang);

    // prevent talking at unknown language (cheating)
    LanguageDesc const* langDesc = GetLanguageDescByID(lang);
    if (!langDesc)
    {
        SendNotification(LANG_UNKNOWN_LANGUAGE);
        return;
    }
    if (langDesc->skill_id != 0 && !_player->HasSkill(langDesc->skill_id))
    {
        SendNotification(LANG_NOT_LEARNED_LANGUAGE);
        return;
    }

    if (lang == LANG_ADDON)
    {
        // Disabled addon channel?
        if (!sWorld.getConfig(CONFIG_BOOL_ADDON_CHANNEL))
            return;
    }
    // LANG_ADDON should not be changed nor be affected by flood control
    else
    {

        // send in universal language if player in .gmon mode (ignore spell effects)
        if (_player->IsGameMaster())
            lang = LANG_UNIVERSAL;
        else
        {
            // Send message in universal language if crossfaction chat is enabled and player is using default faction
            // languages.
            if ((sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHAT)) && (lang == LANG_COMMON || lang == LANG_ORCISH))
                lang = LANG_UNIVERSAL;
            else
            {
                switch (type)
                {
                    case CHAT_MSG_PARTY:
                    case CHAT_MSG_RAID:
                    case CHAT_MSG_RAID_LEADER:
                    case CHAT_MSG_RAID_WARNING:
                        // allow two side chat at group channel if two side group allowed
                        if (sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GROUP))
                            lang = LANG_UNIVERSAL;
                        break;
                    case CHAT_MSG_GUILD:
                    case CHAT_MSG_OFFICER:
                        // allow two side chat at guild channel if two side guild allowed
                        if (sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GUILD))
                            lang = LANG_UNIVERSAL;
                        break;
                }
            }

            // but overwrite it by SPELL_AURA_MOD_LANGUAGE auras (only single case used)
            Unit::AuraList const& ModLangAuras = _player->GetAurasByType(SPELL_AURA_MOD_LANGUAGE);
            if (!ModLangAuras.empty())
                lang = ModLangAuras.front()->GetModifier()->m_miscvalue;
        }
    }

    if (type != CHAT_MSG_AFK &&
        type != CHAT_MSG_DND &&
        lang != LANG_ADDON)
    {
        _player->UpdateChatActivityTimer();
    }

    std::string msg, channel, to;
    // Message parsing
    switch (type)
    {
        case CHAT_MSG_CHANNEL:
        {
            recv_data >> channel;
            recv_data >> msg;

            if (lang == LANG_ADDON)
            {
                // Destination authorization happens in the handling switch below.
                // Do not parse addon payloads as server chat commands meanwhile.
                if (!CheckChatMessageValidity(msg, lang, type))
                    return;
            }
            else if (!ProcessChatMessageAfterSecurityCheck(msg, lang, type))
                return;

            if (msg.empty())
                return;
            break;
        }
        case CHAT_MSG_WHISPER:
        {
            recv_data >> to;
            // no break
        }
        case CHAT_MSG_SAY:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_YELL:
        case CHAT_MSG_PARTY:
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_LEADER:
        case CHAT_MSG_RAID_WARNING:
        case CHAT_MSG_BATTLEGROUND:
        case CHAT_MSG_BATTLEGROUND_LEADER:
        case CHAT_MSG_HARDCORE:
        {
            recv_data >> msg;

            if (!ProcessChatMessageAfterSecurityCheck(msg, lang, type))
                return;
            if (msg.empty())
                return;
            break;
        }
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND:
        {
            recv_data >> msg;
            if (!CheckChatMessageValidity(msg, lang, type))
                return;
            break;
        }
        default:
        {
            sLog.outError("CHAT: unknown message type %u, lang: %u", type, lang);
            return;
        }
    }


    //Move muted here to check if whispers are whispering a GM, let those pass.
    if (lang != LANG_ADDON)
    {
        if (type != CHAT_MSG_AFK && type != CHAT_MSG_DND)
        {
            time_t currTime = time(nullptr);

            bool skipMute = false;

            if (type == CHAT_MSG_WHISPER)
            {
                MasterPlayer* player = ObjectAccessor::FindMasterPlayer(to.c_str());
                if (player && player->GetSession()->GetSecurity() > SEC_PLAYER)
                    skipMute = true; // skip mute when whispering to GMs but allow GM to still .whisp off later on.
            }

            if (!_player->CanSpeak() && !skipMute) // Muted
            {
                std::string timeStr = "";

                if ((GetAccountFlags() & ACCOUNT_FLAG_MUTED_PAUSING) == ACCOUNT_FLAG_MUTED_PAUSING)
                    timeStr = secsToTimeString(m_muteTime / 1000);
                else
                    timeStr = secsToTimeString(m_muteTime - currTime);

                SendNotification(GetMangosString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
                return;
            }
            if (lang != LANG_ADDON && GetMasterPlayer())
                GetMasterPlayer()->UpdateSpeakTime(); // Anti chat flood
        }
    }

    //temp workaround to move RP addons to their correct LANG, the addons are broken rn.
    if (msg.find("MR:") != std::string::npos || msg.find("TR:") != std::string::npos)
        lang = LANG_ADDON;

    if (type == CHAT_MSG_CHANNEL)
    {
        if (sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK))
        {
            static uint64 lastAnnounce = 0;

            //CN network has an addon using the channel Twb that's spamming and causing a lot of server stress.
            //Temp throttle until this is fixed by the author.

            std::string chnLower = channel;
            std::transform(chnLower.begin(), chnLower.end(), chnLower.begin(), ::tolower);
            if (chnLower.find("twb") != std::string::npos)
            {
                auto timeNow = time(nullptr);

                if ((timeNow - lastAnnounce) < 10)
                    return;
                lastAnnounce = timeNow;
            }
        }
    }

    if (HandleTurtleAddonMessages(lang, type, msg))
    {
        // Message was a turtle addon message, no point to process further
        return;
    }

    {
        PlayerPointer playerPointer(GetPlayerPointer());
        if (lang != LANG_ADDON && playerPointer->GetLevel() < sWorld.GetMinChatLevel() && !HasHighLevelCharacter() && type != CHAT_MSG_PARTY)
        {
            ChatHandler(this).SendSysMessage("You cannot talk yet.");
            return;
        }
    }

    if (lang != LANG_ADDON)
    {
        if (sWorld.getConfig(CONFIG_BOOL_BLOCK_ALL_HANZI))
        {
            std::wstring w_normMsg;
            if (!Utf8toWStr(msg, w_normMsg))
                return;
            if (hasChinese(w_normMsg))
                return;
        }
    }

    // Message handling
    switch (type)
    {
        case CHAT_MSG_CHANNEL:
        {
            PlayerPointer playerPointer(GetPlayerPointer());
            ASSERT(playerPointer);
            ChannelMgr* cMgr = channelMgr(playerPointer->GetTeam());

            if (cMgr)
            {
                if (Channel *chn = cMgr->GetChannel(channel, playerPointer))
                {
                    // Level channels restrictions
                    if (chn->IsLevelRestricted() && playerPointer->GetLevel() < sWorld.getConfig(CONFIG_UINT32_WORLD_CHAN_MIN_LEVEL)
                        && GetAccountMaxLevel() < sWorld.getConfig(CONFIG_UINT32_PUB_CHANS_MUTE_VANISH_LEVEL))
                    {
                        ChatHandler(this).SendSysMessage("You cannot use this channel yet.");
                        return;
                    }

                    // Public channels restrictions
                    if (!chn->HasFlag(Channel::CHANNEL_FLAG_CUSTOM))
                    {
                        // GMs should not be able to use public channels
                        if (GetSecurity() > SEC_PLAYER && !sWorld.getConfig(CONFIG_BOOL_GMS_ALLOW_PUBLIC_CHANNELS))
                        {
                            ChatHandler(this).SendSysMessage("GMs can't use public channels.");
                            return;
                        }

                        if (chn->HasFlag(Channel::CHANNEL_FLAG_GENERAL))
                        {
                            if (EnforceEnglish(this, msg))
                                return;
                        }

                        if (uint32 cooldown = ChatCooldown())
                        {
                            ChatHandler(this).PSendSysMessage(
                                "Please wait %u seconds before sending another message.", cooldown
                            );
                            return;
                        }
                    }

                    bool bIsWorldChannel = channel == "World";

                    //There is a really nice arcane bug in Channel.dbc linux loading.
                    //It blocks world from properly being loaded and seen as first-class channel.
                    //Biggest hackfixes of the century
                    if (bIsWorldChannel)
                    {
                        if (EnforceEnglish(this, msg))
                            return;
                    }

                    if (sWorld.IsPvPRealm())
                    {
                        if (channel == "WorldH")
                        {
                            channelMgr(HORDE)->GetOrCreateChannel("World")->AsyncSay(playerPointer->GetObjectGuid(), msg.c_str(), LANG_UNIVERSAL, true);
                        }

                        if (channel == "WorldA")
                        {
                            channelMgr(ALLIANCE)->GetOrCreateChannel("World")->AsyncSay(playerPointer->GetObjectGuid(), msg.c_str(), LANG_UNIVERSAL, true);
                        }
                    }

                    AntispamInterface* pAntispam = sAnticheatLib->GetAntispam();
                    if (lang == LANG_ADDON || !pAntispam || pAntispam->AddMessage(msg, lang, type, GetPlayerPointer(), nullptr, chn, nullptr))
                    {
                        chn->AsyncSay(playerPointer->GetObjectGuid(), msg.c_str(), lang);


                        if (lang != LANG_ADDON && bIsWorldChannel)
                        {
                            ChannelMgr::AnnounceBothFactionsChannel("Global", playerPointer->GetObjectGuid(), string_format("|cff{}{}|r", playerPointer->GetTeam() == HORDE ? "ff0000" : "2773ff"
                                , msg.c_str()).c_str());
                        }

                        if (bIsWorldChannel && lang != LANG_ADDON)
                        {
                            std::string logChat = sWorld.FormatLoggedChat(this, "Chan", msg, nullptr, 0, channel.c_str());
                           // sWorld.SendDiscordMessage(1075224002013962250, logChat);
                        }
                    }

                    SetLastPubChanMsgTime(time(nullptr));
                }
            }

            if (lang != LANG_ADDON)
            {
                normalizePlayerName(channel);
                sWorld.LogChat(this, "Chan", msg, nullptr, 0, channel.c_str());
            }
            break;
        }

        case CHAT_MSG_SAY:
        {
            if (GetPlayer()->GetLevel() < sWorld.getConfig(CONFIG_UINT32_SAY_MIN_LEVEL)
                && GetAccountMaxLevel() < sWorld.getConfig(CONFIG_UINT32_PUB_CHANS_MUTE_VANISH_LEVEL))
            {
                ChatHandler(this).SendSysMessage("You cannot speak yet (too low level).");
                return;
            }

            if (!GetPlayer()->IsAlive())
                return;

            GetPlayer()->Say(msg, lang);

            // Speech names nobody, so at most one managed bot nearby looks up.
            if (lang != LANG_ADDON)
                ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_CHAT_SAY,
                    [&](PlayerScript* s) { s->OnChatSay(GetPlayer(),
                        sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_SAY), msg.c_str()); });

            if (lang != LANG_ADDON)
            {
                sWorld.LogChat(this, "Say", msg);
            }

            break;
        }
        case CHAT_MSG_EMOTE:
        {
            if (GetPlayer()->GetLevel() < sWorld.getConfig(CONFIG_UINT32_SAY_EMOTE_MIN_LEVEL)
                && GetAccountMaxLevel() < sWorld.getConfig(CONFIG_UINT32_PUB_CHANS_MUTE_VANISH_LEVEL))
            {
                ChatHandler(this).SendSysMessage("You cannot use emotes yet (too low level).");
                return;
            }

            if (!GetPlayer()->IsAlive())
                return;

            GetPlayer()->TextEmote(msg);

            if (lang != LANG_ADDON)
            {
                sWorld.LogChat(this, "Emote", msg);
            }

            break;
        }
        case CHAT_MSG_YELL:
        {
            if (GetPlayer()->GetLevel() < sWorld.getConfig(CONFIG_UINT32_YELL_MIN_LEVEL)
                && GetAccountMaxLevel() < sWorld.getConfig(CONFIG_UINT32_PUB_CHANS_MUTE_VANISH_LEVEL))
            {
                ChatHandler(this).SendSysMessage("You cannot yell yet (too low level).");
                return;
            }

            if (!GetPlayer()->IsAlive())
                return;

            if (EnforceEnglish(this, msg))
                return;

            GetPlayer()->Yell(msg, lang);

            if (lang != LANG_ADDON && !IsFingerprintBanned())
                ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_CHAT_YELL,
                    [&](PlayerScript* s) { s->OnChatYell(GetPlayer(),
                        GetPlayer()->GetYellRange(), msg.c_str()); });

            if (lang != LANG_ADDON)
            {
                sWorld.LogChat(this, "Yell", msg);
            }
            break;
        }

        case CHAT_MSG_WHISPER: // Master Side
        {
            if (!normalizePlayerName(to))
            {
                SendPlayerNotFoundNotice(to);
                break;
            }
            MasterPlayer* masterPlr = GetMasterPlayer();
            ASSERT(masterPlr);

            MasterPlayer *player = ObjectAccessor::FindMasterPlayer(to.c_str());
            uint32 tSecurity = GetSecurity();
            uint32 pSecurity = player ? player->GetSession()->GetSecurity() : SEC_PLAYER;
            if (!player || (tSecurity == SEC_PLAYER && pSecurity > SEC_PLAYER && !player->AcceptsWhispersFrom(masterPlr->GetObjectGuid())))
            {
                SendPlayerNotFoundNotice(to);
                return;
            }

            if (tSecurity == SEC_PLAYER && pSecurity == SEC_PLAYER)
            {
                if (!sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_CHAT) && GetPlayer()->GetTeam() != player->GetTeam())
                {
                    SendWrongFactionNotice();
                    return;
                }

                if (/*player->GetZoneId() != masterPlr->GetZoneId() && */masterPlr->GetLevel() < sWorld.getConfig(CONFIG_UINT32_WHISP_DIFF_ZONE_MIN_LEVEL))
                {
                    ChatHandler(this).SendSysMessage("You cannot whisper yet.");
                    return;
                }

                const auto maxLevel{ masterPlr->GetSession()->GetAccountMaxLevel() };
                auto& whisper_targets{ masterPlr->GetSession()->GetWhisperTargets() };
                Player* toPlayer{ player->GetSession()->GetPlayer() };

                if (toPlayer && !whisper_targets.can_whisper(toPlayer->GetObjectGuid(), maxLevel))
                {
                    ChatHandler(this).PSendSysMessage("You have whispered too many different players too quickly.");
                    ChatHandler(this).PSendSysMessage("Please wait a while before whispering any additional players.");
                    return;
                }
            }

            if (Player* toPlayer = player->GetSession()->GetPlayer())
            {
                bool allowIgnoreAntispam = toPlayer->IsAllowedWhisperFrom(masterPlr->GetObjectGuid());
                bool allowSendWhisper = allowIgnoreAntispam;
                if (!sWorld.getConfig(CONFIG_BOOL_WHISPER_RESTRICTION) || !toPlayer->IsEnabledWhisperRestriction())
                    allowSendWhisper = true;
                if (IsFingerprintBanned())
                    allowSendWhisper = false;
                if (player == masterPlr || masterPlr->IsGameMaster())
                    allowSendWhisper = true;

              //  AntispamInterface* pAntispam = sAnticheatLib->GetAntispam();
                //if (!allowSendWhisper || lang == LANG_ADDON || !pAntispam || pAntispam->AddMessage(msg, lang, type, GetPlayerPointer(), PlayerPointer(new PlayerWrapper<MasterPlayer>(player)), nullptr, nullptr))
                masterPlr->Whisper(msg, lang, player, allowSendWhisper);

                // A module listens to whispers and party chat for one thing only:
                // somebody calling an errand off. Everything else it is told
                // arrives through a channel.
                if (_player && allowSendWhisper)
                    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_CHAT_WHISPER,
                        [&](PlayerScript* s) { s->OnChatWhisper(_player, msg.c_str()); });

                if (lang != LANG_ADDON)
                {
                    sWorld.LogChat(this, "Whisp", msg, PlayerPointer(new PlayerWrapper<MasterPlayer>(player)));
                }
            }
            break;
        }

        case CHAT_MSG_PARTY: // Master Side: TODO
        {
            // if player is in battleground, he cannot say to battleground members by /p
            Group *group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = _player->GetGroup();
                if (!group || group->isBGGroup())
                    return;
            }

            if (lang == LANG_ADDON && _player && sScriptMgr.OnAddonMessage(_player, msg))
                return;

            if (_player && lang != LANG_ADDON)
                ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_CHAT_WHISPER,
                    [&](PlayerScript* s) { s->OnChatWhisper(_player, msg.c_str()); });

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, ChatMsg(type), msg.c_str(), Language(lang), _player->GetChatTag(), _player->GetObjectGuid(), _player->GetName());

            if (lang != LANG_ADDON)
                sWorld.LogChat(this, "Group", msg, nullptr, group->GetId());

            group->BroadcastPacket(&data, false, group->GetMemberGroup(GetPlayer()->GetObjectGuid()));
            break;
        }
        case CHAT_MSG_GUILD: // Master side
        {
            if (Guild* guild = sGuildMgr.GetGuildById(GetMasterPlayer()->GetGuildId()))
            {
                if (lang == LANG_ADDON && _player && sScriptMgr.OnAddonMessage(_player, msg))
                    return;

                guild->BroadcastToGuild(this, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);

                // Module hook: guild chat with our people in it answers back.
                if (_player && lang != LANG_ADDON)
                {
                    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_CHAT_GUILD,
                        [&](PlayerScript* s) { s->OnChatGuild(_player, msg.c_str()); });
                }
            }

            if (lang != LANG_ADDON)
                sWorld.LogChat(this, "Guild", msg, nullptr, GetMasterPlayer()->GetGuildId());

            break;
        }
        case CHAT_MSG_HARDCORE:
        {
            if ((GetPlayer()->IsHardcore() || GetPlayer()->IsHC60()) || GetPlayer()->GetSession()->GetSecurity() > SEC_PLAYER)
            {
                WorldPacket data;
                ChatHandler::BuildChatPacket(data, CHAT_MSG_HARDCORE, msg.c_str(), Language(lang), _player->GetChatTag(), _player->GetObjectGuid(), _player->GetName());

                if (EnforceEnglish(this, msg))
                    return;

                sWorld.SendHardcoreMessage(&data, _player->GetSession());
            }
            else
                GetPlayer()->ToPlayer()->GetSession()->SendNotification("You must be Hardcore to join this channel.");                
            break;
        }
        case CHAT_MSG_OFFICER: // Master side
        {
            if (GetMasterPlayer()->GetGuildId())
                if (Guild* guild = sGuildMgr.GetGuildById(GetMasterPlayer()->GetGuildId()))
                {
                    if (lang == LANG_ADDON && _player && sScriptMgr.OnAddonMessage(_player, msg))
                        return;
                    guild->BroadcastToOfficers(this, msg, lang == LANG_ADDON ? LANG_ADDON : LANG_UNIVERSAL);
                }

            if (lang != LANG_ADDON)
                sWorld.LogChat(this, "Officer", msg, nullptr, GetMasterPlayer()->GetGuildId());

            break;
        }
        case CHAT_MSG_RAID: // Master side: TODO
        {
            // if player is in battleground, he cannot say to battleground members by /ra
            Group *group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup())
                    return;
            }

            if (lang == LANG_ADDON && _player && sScriptMgr.OnAddonMessage(_player, msg))
                return;

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID, msg.c_str(), Language(lang), _player->GetChatTag(), _player->GetObjectGuid(), _player->GetName());
            group->BroadcastPacket(&data, false);

            if (lang != LANG_ADDON)
                sWorld.LogChat(this, "Raid", msg, nullptr, group->GetId());
            break;

        }
        case CHAT_MSG_RAID_LEADER: // Master side: TODO
        {
            // if player is in battleground, he cannot say to battleground members by /ra
            Group *group = GetPlayer()->GetOriginalGroup();
            if (!group)
            {
                group = GetPlayer()->GetGroup();
                if (!group || group->isBGGroup() || !group->isRaidGroup() || !group->IsLeader(_player->GetObjectGuid()))
                    return;
            }

            if (lang == LANG_ADDON && _player && sScriptMgr.OnAddonMessage(_player, msg))
                return;

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_LEADER, msg.c_str(), Language(lang), _player->GetChatTag(), _player->GetObjectGuid(), _player->GetName());
            group->BroadcastPacket(&data, false);

            if (lang != LANG_ADDON)
                sWorld.LogChat(this, "Raid", msg, nullptr, group->GetId());
            break;
        }

        case CHAT_MSG_RAID_WARNING: // Master side: TODO
        {
            Group *group = GetPlayer()->GetGroup();
            if (!group || !group->isRaidGroup() ||
                    !(group->IsLeader(GetPlayer()->GetObjectGuid()) || group->IsAssistant(GetPlayer()->GetObjectGuid())))
                return;

            if (lang == LANG_ADDON && _player && sScriptMgr.OnAddonMessage(_player, msg))
                return;

            WorldPacket data;
            //in battleground, raid warning is sent only to players in battleground - code is ok
            ChatHandler::BuildChatPacket(data, CHAT_MSG_RAID_WARNING, msg.c_str(), Language(lang), _player->GetChatTag(), _player->GetObjectGuid(), _player->GetName());
            group->BroadcastPacket(&data, false);

            if (lang != LANG_ADDON)
                sWorld.LogChat(this, "Raid", msg, nullptr, group->GetId());
            break;
        }

        case CHAT_MSG_BATTLEGROUND: // Node side
        {
            // battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group *group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup())
                return;

            if (lang == LANG_ADDON && _player && sScriptMgr.OnAddonMessage(_player, msg))
                return;

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_BATTLEGROUND, msg.c_str(), Language(lang), _player->GetChatTag(), _player->GetObjectGuid(), _player->GetName());
            group->BroadcastPacket(&data, false);

            if (lang != LANG_ADDON)
            {
                // let game masters listen in on bg chat
                if (Map* pMap = GetPlayer()->FindMap())
                    pMap->SendToAllGMsNotInGroup(&data, group);

                sWorld.LogChat(this, "BG", msg, nullptr, group->GetId());
            }

            break;
        }

        case CHAT_MSG_BATTLEGROUND_LEADER: // Node side
        {
            // battleground raid is always in Player->GetGroup(), never in GetOriginalGroup()
            Group *group = GetPlayer()->GetGroup();
            if (!group || !group->isBGGroup() || !group->IsLeader(GetPlayer()->GetObjectGuid()))
                return;

            if (lang == LANG_ADDON && _player && sScriptMgr.OnAddonMessage(_player, msg))
                return;

            WorldPacket data;
            ChatHandler::BuildChatPacket(data, CHAT_MSG_BATTLEGROUND_LEADER, msg.c_str(), Language(lang), _player->GetChatTag(), _player->GetObjectGuid(), _player->GetName());
            group->BroadcastPacket(&data, false);

            if (lang != LANG_ADDON)
            {
                // let game masters listen in on bg chat
                if (Map* pMap = GetPlayer()->FindMap())
                    pMap->SendToAllGMsNotInGroup(&data, group);

                sWorld.LogChat(this, "BG", msg, nullptr, group->GetId());
            }

            break;
        }

        case CHAT_MSG_AFK: // Node side (for combat Check)
        {
            if(_player && _player->IsInCombat())
                break;

            if(!msg.empty() || !_player->IsAFK())
            {
                if (MasterPlayer* masterPlr = GetMasterPlayer())
                    masterPlr->afkMsg = msg;
            }

            if(msg.empty() || !_player->IsAFK())
            {
                _player->ToggleAFK();

                if(_player->IsAFK() && _player->IsDND())
                    _player->ToggleDND();
            }
            break;
        }

        case CHAT_MSG_DND:
        {
            if(!msg.empty() || !_player->IsDND())
            {
                if (MasterPlayer* masterPlr = GetMasterPlayer())
                    masterPlr->dndMsg = msg;
            }

            if(msg.empty() || !_player->IsDND())
            {
                _player->ToggleDND();

                if(_player->IsDND() && _player->IsAFK())
                    _player->ToggleAFK();
            }
            break;
        }
    }
}

void WorldSession::HandleEmoteOpcode(WorldPacket & recv_data)
{
    if (!GetPlayer()->IsAlive() || GetPlayer()->HasUnitState(UNIT_STAT_FEIGN_DEATH))
        return;

    if (!GetPlayer()->CanSpeak())
    {


        std::string timeStr = "";

        if ((GetAccountFlags() & ACCOUNT_FLAG_MUTED_PAUSING) == ACCOUNT_FLAG_MUTED_PAUSING)
            timeStr = secsToTimeString(m_muteTime / 1000);
        else
            timeStr = secsToTimeString(m_muteTime - time(nullptr));
        SendNotification(GetMangosString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        return;
    }

    uint32 emote;
    recv_data >> emote;

    // Restrict to the only emotes hardcoded in client
    if (emote != EMOTE_ONESHOT_NONE && emote != EMOTE_ONESHOT_WAVE)
        return;

    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_EMOTE, [&](PlayerScript* script)
    {
        script->OnEmote(GetPlayer(), emote);
    });

    GetPlayer()->HandleEmoteCommand(emote);
}

namespace MaNGOS
{
class EmoteChatBuilder
{
public:
    EmoteChatBuilder(Player const& pl, uint32 text_emote, uint32 emote_num, Unit const* target)
        : i_player(pl), i_text_emote(text_emote), i_emote_num(emote_num), i_target(target) {}

    void operator()(WorldPacket& data, int32 loc_idx)
    {
        char const* nam = i_target ? i_target->GetNameForLocaleIdx(loc_idx) : nullptr;
        uint32 namlen = (nam ? strlen(nam) : 0) + 1;

        data.Initialize(SMSG_TEXT_EMOTE, (20 + namlen));
        data << ObjectGuid(i_player.GetObjectGuid());
        data << uint32(i_text_emote);
        data << uint32(i_emote_num);
        data << uint32(namlen);
        if (namlen > 1)
            data.append(nam, namlen);
        else
            data << uint8(0x00);
    }

private:
    Player const& i_player;
    uint32        i_text_emote;
    uint32        i_emote_num;
    Unit const*   i_target;
};
}                                                           // namespace MaNGOS

void WorldSession::HandleTextEmoteOpcode(WorldPacket & recv_data)
{
    if (!GetPlayer()->IsAlive())
        return;

    if (!GetPlayer()->CanSpeak())
    {
        std::string timeStr = "";

        if ((GetAccountFlags() & ACCOUNT_FLAG_MUTED_PAUSING) == ACCOUNT_FLAG_MUTED_PAUSING)
            timeStr = secsToTimeString(m_muteTime / 1000);
        else
            timeStr = secsToTimeString(m_muteTime - time(nullptr));

        SendNotification(GetMangosString(LANG_WAIT_BEFORE_SPEAKING), timeStr.c_str());
        return;
    }

    uint32 textEmote, emoteNum;
    ObjectGuid guid;

    recv_data >> textEmote;
    recv_data >> emoteNum;
    recv_data >> guid;

    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_TEXT_EMOTE, [&](PlayerScript* script)
    {
        script->OnTextEmote(GetPlayer(), textEmote, emoteNum, guid);
    });

    EmotesTextEntry const* em = sEmotesTextStore.LookupEntry(textEmote);
    if (!em)
        return;


    uint32 emoteId = em->textid;

    if (sWorld.IsAprilFools())
    {
        if (textEmote == TEXTEMOTE_PURR)
        {
            uint32 cats[] = { 5585, 9989, 18628 };
            GetPlayer()->SetDisplayId(cats[urand(0, 2)]);
        }
    }

    switch (emoteId)
    {
        case EMOTE_STATE_SLEEP:
        case EMOTE_STATE_SIT:
        case EMOTE_STATE_KNEEL:
        case EMOTE_ONESHOT_NONE:
            break;
        default:
        {
            //GetPlayer()->InterruptSpellsWithChannelFlags(AURA_INTERRUPT_ANIM_CANCELS);
            //GetPlayer()->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_ANIM_CANCELS);
            GetPlayer()->HandleEmote(emoteId);
            break;
        }
    }

    Unit* unit = GetPlayer()->GetMap()->GetUnit(guid);

    MaNGOS::EmoteChatBuilder emote_builder(*GetPlayer(), textEmote, emoteNum, unit);
    MaNGOS::LocalizedPacketDo<MaNGOS::EmoteChatBuilder > emote_do(emote_builder);
    MaNGOS::CameraDistWorker<MaNGOS::LocalizedPacketDo<MaNGOS::EmoteChatBuilder > > emote_worker(GetPlayer(), sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE), emote_do);
    Cell::VisitWorldObjects(GetPlayer(), emote_worker,  sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE));

    //Send scripted event call
    if (unit && unit->IsCreature() && ((Creature*)unit)->AI())
        ((Creature*)unit)->AI()->ReceiveEmote(GetPlayer(), textEmote);

    // And the same courtesy for managed bots. The packet carried its target, so
    // this is the one place in the game where who was addressed is not a
    // guess -- no distance heuristic, no name parsing, no language involved.
    if (unit && GetPlayer()->IsWithinDistInMap(unit,
        sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE)))
        ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_TEXT_EMOTE_HEARD,
            [&](PlayerScript* s) { s->OnTextEmoteHeard(GetPlayer(), textEmote, guid); });
}

void WorldSession::HandleChatIgnoredOpcode(WorldPacket& recv_data)
{
    ObjectGuid iguid;
    //DEBUG_LOG("WORLD: Received CMSG_CHAT_IGNORED");

    recv_data >> iguid;

    Player *player = sObjectMgr.GetPlayer(iguid);
    if (!player || !player->GetSession())
        return;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_IGNORED, _player->GetName(), LANG_UNIVERSAL, CHAT_TAG_NONE, _player->GetObjectGuid());
    player->GetSession()->SendPacket(&data);
}

void WorldSession::SendPlayerNotFoundNotice(std::string const& name)
{
    WorldPacket data(SMSG_CHAT_PLAYER_NOT_FOUND, name.size() + 1);
    data << name;
    SendPacket(&data);
}

void WorldSession::SendWrongFactionNotice()
{
    WorldPacket data(SMSG_CHAT_WRONG_FACTION, 0);
    SendPacket(&data);
}

void WorldSession::SendChatRestrictedNotice()
{
    WorldPacket data(SMSG_CHAT_RESTRICTED, 0);
    SendPacket(&data);
}

bool WorldSession::HandleTurtleAddonMessages(uint32 lang, uint32 type, std::string& msg)
{
    if (lang != LANG_ADDON || msg.empty())
    {
        return false;
    }

    if (sLFTMgr.HandleAddonMessage(_player, type, msg))
        return true;

    if (sCustomMerchantMgr.HandleAddonMessage(this, _player, type, msg))
        return true;

    if (sBarbershopMgr.HandleAddonMessage(_player, type, msg))
        return true;

    if (type == CHAT_MSG_GUILD && msg == "TW_HONOR\tC2S_HONOR_REQUEST")
    {
        _player->GetHonorMgr().SendHonorCurrencyUpdate();
        return true;
    }

    //guild bank
    // no type == CHAT_MSG_GUILD on this, to protecc fraudulent messages
    if (strstr(msg.c_str(), "TW_GUILDBANK"))
    {
        const bool isInferno = _player->IsHC60();
        // HC check
        // no Newcommers check since we dont have anyone in Newcommers to unlock the feature
        if (_player->IsHardcore() && !isInferno)
        {
            //_player->GetSession()->SendNotification("HC No Guild Bank");
            _player->SendAddonMessage("TW_GUILDBANK", "Access:Error:HC");
            return true;
        }

        if (GetMasterPlayer()->GetGuildId())
        {
            Guild* guild = sGuildMgr.GetGuildById(GetMasterPlayer()->GetGuildId());
            if (!guild)
                return true;

            if (!guild->_Bank)
            {
                sLog.outInfo("cant get guild bank");
                return true;
            }

            if (isInferno)
                guild->_InfernoBank->HandleAddonMessages(msg, _player);
            else
                guild->_Bank->HandleAddonMessages(msg, _player);
        }
        else
        {
            _player->SendAddonMessage("TW_GUILDBANK", "Player:Unguilded");
        }

        return true;
    }

    static const std::string LFTPrefix = "LFT";
    if (msg.find(LFTPrefix) != std::string::npos)
    {
        //check if a HC sent this message, if so, just don't broadcast. HCs can block the normal groups looking for dungeons.
        if (GetPlayer() && GetPlayer()->IsHardcore())
            return true;
    }


    // TWT Threat: Guid and UnitDetailedThreatSituation Request
    if (type == CHAT_MSG_PARTY || type == CHAT_MSG_RAID)
    {
        // UnitDetailedThreatSituation
        if (strstr(msg.c_str(), "TWT_UDTSv4"))
        {
            if (_player->IsGameMaster())
                return true;

            if (!_player->GetSelectedCreature())
                return true;

            // CanHaveThreatList checks IsAlive too.
            if (!_player->GetSelectedCreature()->CanHaveThreatList())
                return true;

            int limit = 4;                                 // default low limit
            bool tankMode = strstr(msg.c_str(), "_TM");       // tank mode

            if (msg.find("limit=") != std::string::npos)       // limit= key found
            {
                msg = msg.substr(msg.find("limit=") + 6, 2);   // 6 = "limit=".length(), 2 = read only 2 digits

                std::string limitString = msg;

                static const re2::RE2 limitPattern = "[^0-9]*([0-9]+).*";
                re2::RE2::GlobalReplace(&limitString, limitPattern, R"(\1)");

                // std::string limitString = std::regex_replace(msg.c_str(), std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));

                if (limitString.empty() || limitString.length() > 2)
                    return true;

                try
                {
                    limit = std::stoi(limitString);
                }
                catch (...)
                {
                    return true;
                }
            }


            if (limit <= 0 || limit > 15) // 1-99, in practice 4-11
                return true;

            ThreatManager::UnitDetailedThreatSituation(
                _player->GetSelectedCreature(),
                _player,
                limit,
                tankMode);

            return true;
        }
    }

    if (type == CHAT_MSG_GUILD)
    {
        if (strstr(msg.c_str(), "TW_TRANSMOG")) // Transmog System Comms
        {
            if (_player->FindNearestInteractableNpcWithFlag(UNIT_NPC_FLAG_TRANSMOG))
                _player->_transmogMgr->HandleAddonMessages(msg);
            return true;
        }
        else if (strstr(msg.c_str(), "TW_CHAT_MSG_WHISPER")) // ghetto CHAT_MSG_WHISPER via CHAT_MSG_GUILD
        {
            // syntax: SendAddonMessage("TW_CHAT_MSG_WHISPER<ToName>", "message", "GUILD")
            // returns: TW_CHAT_MSG_WHISPER message (event has arg4 = from )
            std::string to;
            std::string message;

            Tokenizer params(msg, '>', 2);
            if (params.size() != 2)
            {
                // wrong syntax
                _player->SendAddonMessage("TW_CHAT_MSG_WHISPER", "SyntaxError:WrongDestination");
                return true;
            }
            message = params[1];

            Tokenizer dest(params[0], '<', 2);
            if (dest.size() != 2)
            {
                // wrong syntax
                _player->SendAddonMessage("TW_CHAT_MSG_WHISPER", "SyntaxError:WrongDestination");
                return true;
            }
            to = dest[1];

            if (!normalizePlayerName(to))
                return true;

            LoginDatabase.escape_string(to);

            if (Player* pTargetPlayer = sObjectMgr.GetPlayer(to.c_str())) {
                pTargetPlayer->SendAddonMessage("TW_CHAT_MSG_WHISPER", message, _player);
            }
            else
            {
                // wrong syntax
                _player->SendAddonMessage("TW_CHAT_MSG_WHISPER", "Error:CantFindPlayer:" + to);
                return true;
            }
            return true;
        }
        else if (strstr(msg.c_str(), "TW_BUFF")) // buff/debuff system
        {
            static const std::string prefix = "TW_BUFF";

            if (!_player->GetSelectedCreature())
                return true;

            Creature* target = _player->GetSelectedCreature();

            _player->SendAddonMessage(prefix, target->GetDebuffs());
            return true;
        }
        else if (strstr(msg.c_str(), "TW_SHOP")) // Shop Addon Coms
        {
            static const std::string prefix = "TW_SHOP";

            if (strstr(msg.c_str(), "Balance"))
            {
                if (!sShopMgr.RequestBalance(GetAccountId()))
                {
                    SendNotification("Balance query in progress. Please wait.");
                }
                return true;
            }

            if (strstr(msg.c_str(), "Categories"))
            {
                std::string categories = "Categories:";

                for (auto& itr : sObjectMgr.GetShopCategoriesList())
                    if (sWorld.getConfig(CONFIG_BOOL_SEA_NETWORK))
                        categories += std::to_string(itr.first) + "=0=" + itr.second.Name_loc4 + "=" + itr.second.Icon + ";"; // TODO: parent_id
                    else
                        categories += std::to_string(itr.first) + "=0=" + itr.second.Name + "=" + itr.second.Icon + ";"; // TODO: parent_id

                _player->SendAddonMessage(prefix, categories);
                return true;
            }

            if (strstr(msg.c_str(), "Entries:"))
            {
                static const re2::RE2 shopEntriesPattern = "[^0-9]*([0-9]+).*";
                std::string categoryIDString = msg;
                re2::RE2::GlobalReplace(&categoryIDString, shopEntriesPattern, R"(\1)");

                //std::string categoryIDString = std::regex_replace(msg.c_str(), std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));
                uint8 categoryID = 0;

                if (categoryIDString.empty() || categoryIDString.length() > 3)
                    return true;

                try
                {
                    categoryID = std::stoi(categoryIDString);
                }
                catch (...)
                {
                    return true;
                }

                _player->SendAddonMessage(prefix, "Entries:" + categoryIDString + "=start");

                const ShopCategoriesMap& ShopCategories = sObjectMgr.GetShopCategoriesList();
                ShopCategoriesMap::const_iterator ShopIter = ShopCategories.find(categoryID);
                if (ShopIter != ShopCategories.cend())
                {
                    const ShopCategory& ShopCat = ShopIter->second;

                    for (const std::string& EntryStr : ShopCat.CachedItemEntries)
                    {
                        _player->SendAddonMessage(prefix, EntryStr);
                    }
                }

                _player->SendAddonMessage(prefix, "Entries:" + categoryIDString + "=end");
                return true;
            }

            if (strstr(msg.c_str(), "Buy:"))
            {
                static const re2::RE2 shopBuyPattern = "[^0-9]*([0-9]+).*";
                std::string itemIDString = msg;
                re2::RE2::GlobalReplace(&itemIDString, shopBuyPattern, R"(\1)");

                //std::string itemIDString = std::regex_replace(msg.c_str(), std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));
                uint32 itemId = 0;

                if (itemIDString.empty() || itemIDString.length() > 6)
                    return true;

                try
                {
                    itemId = std::stoi(itemIDString);
                }
                catch (...)
                {
                    return true;
                }
                if (!sShopMgr.RequestPurchase(GetAccountId(), _player->GetGUIDLow(), itemId))
                    SendNotification("Purchase in progress. Please wait.");

                return true;
            }

            return true;
        }
        else if (strstr(msg.c_str(), "TW_BGQueue")) // Minimap Battleground Queue System
        {
            if (_player->IsInCombat())
            {
                _player->GetSession()->SendNotification("You cannot to queue for battlegrounds while in combat.");
                return true;
            }
            else if (_player->InBattleGround())
            {
                _player->GetSession()->SendNotification("You cannot to queue for battlegrounds while in a battleground.");
                return true;
            }
            else if (_player->GetDeathState() == CORPSE || _player->GetDeathState() == DEAD)
            {
                _player->GetSession()->SendNotification("You cannot queue for battlegrounds while dead.");
                return true;
            }
            else if (_player->IsBeingTeleported())
            {
                ChatHandler(_player).PSendSysMessage("You do not meet the conditions to queue for battlegrounds.");
                return true;
            }
            else if (strstr(msg.c_str(), "Warsong") || strstr(msg.c_str(), "Arathi") || strstr(msg.c_str(), "Alterac") || strstr(msg.c_str(), "Sunnyglade") || strstr(msg.c_str(), "Arena"))
            {
                _player->SetBattleGroundEntryPoint();

                if (strstr(msg.c_str(), "Warsong"))
                    _player->GetSession()->SendBattleGroundList(_player->GetObjectGuid(), BATTLEGROUND_WS);
                if (strstr(msg.c_str(), "Arathi"))
                    _player->GetSession()->SendBattleGroundList(_player->GetObjectGuid(), BATTLEGROUND_AB);
                if (strstr(msg.c_str(), "Alterac"))
                    _player->GetSession()->SendBattleGroundList(_player->GetObjectGuid(), BATTLEGROUND_AV);
                if (strstr(msg.c_str(), "Sunnyglade"))
                    _player->GetSession()->SendBattleGroundList(_player->GetObjectGuid(), BATTLEGROUND_SV);
                if (strstr(msg.c_str(), "Arena"))
                    _player->GetSession()->SendBattleGroundList(_player->GetObjectGuid(), BATTLEGROUND_BR);

                _player->SetBGQueueAllowed(true);
            }
            return true;
        }
        else if (strstr(msg.c_str(), "TW_TITLES")) // Title System Comms
        {
            // syntax: ChangeTitle:#
            if (strstr(msg.c_str(), "ChangeTitle:"))
            {
                std::string titleIDstring = msg;

                static const re2::RE2 changeTitlePattern = "[^0-9]*([0-9]+).*";

                re2::RE2::GlobalReplace(&titleIDstring, changeTitlePattern, R"(\1)");
                // std::string titleIDstring = std::regex_replace(msg.c_str(), std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));
                int titleID = 0;

                if (titleIDstring.empty() || titleIDstring.length() > 3)
                    return true;

                try
                {
                    titleID = std::stoi(titleIDstring);
                }
                catch (...)
                {
                    return true;
                }

                _player->ChangeTitle(titleID);
                return true;
            }

            // syntax: ChangeTitle:#
            if (strstr(msg.c_str(), "Titles:List"))
            {
                _player->SendEarnedTitles();
                return true;
            }

            return true;
        }
        // GM Addon
        else if (GetSecurity() > SEC_PLAYER)
        {
            if (strstr(msg.c_str(), "GM_ADDON")) // prefix
            {
                if (strstr(msg.c_str(), "GET_TICKETS"))
                    sTicketMgr.SendTicketsInAddonMessage(_player);
                else if (strstr(msg.c_str(), "GET_TEMPLATES"))
                    sTicketMgr.SendTicketTemplatesInAddonMessage(_player);
                else if (char const* pSubString = strstr(msg.c_str(), "PLAYER_INFO:"))
                    sAccountMgr.SendPlayerInfoInAddonMessage(pSubString + strlen("PLAYER_INFO:"), _player);
                return true;
            }
        }
    }

    return false;
}
