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

#include "Chat.h"
#include "Language.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "ObjectMgr.h"
#include "ObjectGuid.h"
#include "Player.h"
#include "UpdateMask.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "AccountMgr.h"
#include "SpellMgr.h"
#include "PoolManager.h"
#include "GameEventMgr.h"
#include "HardcodedEvents.h"
#include "ScriptObjects.h"

#include <vector>

ChatCommand * ChatHandler::getCommandTable()
{
#ifdef USE_ANTICHEAT
    //Private table, must be in module
#include "Anticheat/AnticheatChatCommands.h"
#else
    //Public table
    static ChatCommand anticheatCommandTable[] =
    {
        { "info",       SEC_ADMINISTRATOR,        true,  &ChatHandler::HandleAnticheatInfoCommand,  "", nullptr },
        { nullptr,      0,                        false, nullptr,                                   "", nullptr }
    };

#endif

    static ChatCommand accountSetCommandTable[] =
    {
        { "gmlevel",        SEC_ADMINISTRATOR,        true,  &ChatHandler::HandleAccountSetGmLevelCommand,   "", nullptr },
        { "password",       SEC_DEVELOPER,           true,  &ChatHandler::HandleAccountSetPasswordCommand,  "", nullptr },
        { nullptr,          0,                        false, nullptr,                                        "", nullptr }
    };

    static ChatCommand accountCommandTable[] =
    {
        { "characters",     SEC_DEVELOPER,      true,  &ChatHandler::HandleAccountCharactersCommand,   "", nullptr },
        { "create",         SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleAccountCreateCommand,       "", nullptr },
        { "delete",         SEC_CONSOLE,        true,  &ChatHandler::HandleAccountDeleteCommand,       "", nullptr },
        { "email",          SEC_PLAYER,         false, &ChatHandler::HandleAccountEmailCommand,        "", nullptr },
        { "getname",       SEC_OBSERVER,        true,  &ChatHandler::HandleAccountGetNameCommand,      "", nullptr },
        { "set",            SEC_DEVELOPER,      true,  nullptr,                                        "", accountSetCommandTable },
        { "password",       SEC_PLAYER,         true,  &ChatHandler::HandleAccountPasswordCommand,     "", nullptr },
        { "twofa",          SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleAccountFaCommand,           "", nullptr },
        { "",               SEC_PLAYER,         true,  &ChatHandler::HandleAccountCommand,             "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand banCommandTable[] =
    {
        { "account",        SEC_MODERATOR,      true,  &ChatHandler::HandleBanAccountCommand,          "", nullptr },
        { "allip",          SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleBanAllIPCommand,            "", nullptr },
        { "character",      SEC_MODERATOR,      true,  &ChatHandler::HandleBanCharacterCommand,        "", nullptr },
        { "fingerprint",    SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleBanFingerprintCommand,      "", nullptr },
        { "ip",             SEC_MODERATOR,      true,  &ChatHandler::HandleBanIPCommand,               "", nullptr },
        { "warn",           SEC_MODERATOR,      true,  &ChatHandler::HandleWarnCharacterCommand,       "", nullptr },
        { "removewarn",     SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleRemoveWarnCommand,          "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand baninfoCommandTable[] =
    {
        { "account",        SEC_MODERATOR,      true,  &ChatHandler::HandleBanInfoAccountCommand,      "", nullptr },
        { "character",      SEC_MODERATOR,      true,  &ChatHandler::HandleBanInfoCharacterCommand,    "", nullptr },
        { "ip",             SEC_MODERATOR,      true,  &ChatHandler::HandleBanInfoIPCommand,           "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand banlistCommandTable[] =
    {
        { "account",        SEC_DEVELOPER,     true,  &ChatHandler::HandleBanListAccountCommand,      "", nullptr },
        { "character",      SEC_DEVELOPER,     true,  &ChatHandler::HandleBanListCharacterCommand,    "", nullptr },
        { "ip",             SEC_DEVELOPER,     true,  &ChatHandler::HandleBanListIPCommand,           "", nullptr },
        { nullptr,          0,                 false, nullptr,                                        "", nullptr }
    };

    static ChatCommand castCommandTable[] =
    {
        { "self",           SEC_DEVELOPER,     false, &ChatHandler::HandleCastSelfCommand,               "", nullptr },
        { "target",         SEC_DEVELOPER,     false, &ChatHandler::HandleCastTargetCommand,             "", nullptr },
        { "",               SEC_DEVELOPER,     false, &ChatHandler::HandleCastCommand,                   "", nullptr },
        { nullptr,          0,                 false, nullptr,                                           "", nullptr }
    };

    static ChatCommand characterDeletedListCommandTable[] =
    {
        { "name",            SEC_DEVELOPER,  true, &ChatHandler::HandleCharacterDeletedListNameCommand,    "", nullptr },
        { nullptr,           0,              false, nullptr,                                               "", nullptr }
    };

    static ChatCommand characterDeletedCommandTable[] =
    {
        { "list",           SEC_DEVELOPER,     true,  nullptr                                          ,  "", characterDeletedListCommandTable },
        { "restore",        SEC_DEVELOPER,    true,  &ChatHandler::HandleCharacterDeletedRestoreCommand, "", nullptr },
        { nullptr,          0,                  false, nullptr,                                            "", nullptr }
    };

    static ChatCommand characterCleanCommandTable[] =
    {
        { "todelete",       SEC_CONSOLE,  true,  &ChatHandler::HandleCleanCharactersToDeleteCommand,   "", nullptr },
        { "items",          SEC_CONSOLE,  true,  &ChatHandler::HandleCleanCharactersItemsCommand,      "", nullptr },
        { nullptr,          0,                   false, nullptr,                                       "", nullptr }
    };

    static ChatCommand characterMailCommandTable[] =
    {
        { "list",           SEC_DEVELOPER,       true,  &ChatHandler::HandleCharacterMailListCommand,     "", nullptr },
        { "delete",         SEC_DEVELOPER,       true,  &ChatHandler::HandleCharacterMailDeleteCommand,   "", nullptr },
        { nullptr,          0,                   false, nullptr,                                          "", nullptr }
    };

    static ChatCommand characterCommandTable[] =
    {
        { "deleted",        SEC_DEVELOPER,     true, nullptr,                                         "", characterDeletedCommandTable },
        { "erase",          SEC_CONSOLE,       true,  &ChatHandler::HandleCharacterEraseCommand,      "", nullptr },
        { "getname",        SEC_OBSERVER,      true,  &ChatHandler::HandleCharacterGetNameCommand,    "", nullptr },
        { "diffitems",      SEC_OBSERVER,      true,  &ChatHandler::HandleCharacterDiffItemsCommand,  "", nullptr },
        { "reputation",     SEC_DEVELOPER,     true,  &ChatHandler::HandleCharacterReputationCommand, "", nullptr },
        { "hasitem",        SEC_DEVELOPER,     true,  &ChatHandler::HandleCharacterHasItemCommand,    "", nullptr },
        { "fillflys",       SEC_DEVELOPER,     true,  &ChatHandler::HandleCharacterFillFlysCommand,   "", nullptr },
        { "clean",          SEC_CONSOLE,       true,  nullptr,                                        "", characterCleanCommandTable },
        { "itemlog",        SEC_DEVELOPER,     true,  &ChatHandler::HandleItemLogCommand,             "", nullptr },
        { "mail",           SEC_DEVELOPER,     true,  nullptr,                                        "", characterMailCommandTable },
        { "inactivity",     SEC_OBSERVER,     false,  &ChatHandler::HandleCharacterInactivityDataCommand,      "", nullptr },
        { nullptr,          0,                 false, nullptr,                                        "", nullptr }
    };


    static ChatCommand debugFieldsCommandTable[] =
    {
        {"show",              SEC_DEVELOPER,     false, &ChatHandler::HandleDebugFieldsShowCommand,          "", nullptr},
        {"modify",            SEC_DEVELOPER,     false, &ChatHandler::HandleDebugFieldsModifyCommand,        "", nullptr},
        {}
    };
    

    static ChatCommand debugCommandTable[] =
    {
        { "condition",      SEC_DEVELOPER,     false, &ChatHandler::HandleDebugConditionCommand,           "", nullptr },
        { "instancedata",   SEC_DEVELOPER,     false, &ChatHandler::HandleDebugSetInstanceDataCommand,     "", nullptr },
        { "packetstats",    SEC_DEVELOPER,     true,  &ChatHandler::HandleDebugPacketStatsCommand,         "", nullptr },
        { "fields",         SEC_DEVELOPER,     false, nullptr,                                             "", debugFieldsCommandTable},
        { "leakreport",     SEC_ADMINISTRATOR, true,  &ChatHandler::HandleDebugLeakReportCommand,          "", nullptr},
        { "coeffs",         SEC_DEVELOPER,     true,  &ChatHandler::HandleDebugCoeffsCommand,              "", nullptr},
        { nullptr,          0,                 false, nullptr,                                             "", nullptr }
    };

    static ChatCommand warEffortCommandTable[] =
    {
        { "info",           SEC_DEVELOPER,     true, &ChatHandler::HandleWarEffortInfoCommand,             "", nullptr },
        { "setgongtime",    SEC_DEVELOPER,     true, &ChatHandler::HandleWarEffortSetGongTimeCommand,      "", nullptr },
        { "setstage",       SEC_DEVELOPER,     true, &ChatHandler::HandleWarEffortSetStageCommand,         "", nullptr },
        { nullptr,          0,                 false, nullptr,                                             "", nullptr }
    };

    static ChatCommand suspiciousCommandTable[] =
    {
#ifdef USE_ANTICHEAT
        { "enable",               SEC_DEVELOPER,      true,  &ChatHandler::HandleSuspiciousEnable,               "", nullptr },
        { "movementenable",       SEC_DEVELOPER,      true,  &ChatHandler::HandleSuspiciousMovementEnable,       "", nullptr },
        { "movementdetectvalue",  SEC_DEVELOPER,      true,  &ChatHandler::HandleSuspiciousMovementDetectValue,  "", nullptr },
        { "fishingenable",        SEC_DEVELOPER,      true,  &ChatHandler::HandleSuspiciousFishingEnable,        "", nullptr },
        { "killednpcenabled",     SEC_DEVELOPER,      true,  &ChatHandler::HandleSuspiciousKilledNPC,            "", nullptr },
        { "whitelist",            SEC_DEVELOPER,      true,  &ChatHandler::HandleSuspiciousWhitelist,            "", nullptr },
        { "notify",               SEC_MODERATOR,      true,  &ChatHandler::HandleSuspiciousNotify,               "", nullptr },
        { "fishers",              SEC_OBSERVER,       false, &ChatHandler::HandleSuspiciousFishers,              "", nullptr },
#endif
        { nullptr,                0,                      false, nullptr,                                       "", nullptr }
    };

    static ChatCommand eventCommandTable[] =
    {
        { "list",           SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleEventListCommand,           "", nullptr },
        { "start",          SEC_ADMINISTRATOR,    true,  &ChatHandler::HandleEventStartCommand,          "", nullptr },
        { "stop",           SEC_ADMINISTRATOR,    true,  &ChatHandler::HandleEventStopCommand,           "", nullptr },
        { "enable",         SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleEventEnableCommand,         "", nullptr },
        { "disable",        SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleEventDisableCommand,        "", nullptr },
        { "",               SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleEventInfoCommand,           "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand gmCommandTable[] =
    {
        { "list",           SEC_DEVELOPER,  true,  &ChatHandler::HandleGMListFullCommand,          "", nullptr },
        { "ingame",         SEC_MODERATOR,  true,  &ChatHandler::HandleGMOnlineListCommand,        "", nullptr },
        { "visible",        SEC_MODERATOR, false, &ChatHandler::HandleGMVisibleCommand,           "", nullptr },
        { "options",        SEC_ADMINISTRATOR,     false, &ChatHandler::HandleGMOptionsCommand,           "", nullptr },
        { "socials",        SEC_MODERATOR,    false, &ChatHandler::HandleGMSocialsCommand,                "", nullptr},
        { "setview",        SEC_DEVELOPER, false, &ChatHandler::HandleSetViewCommand,             "", nullptr },
        { "chat",           SEC_ADMINISTRATOR, false, &ChatHandler::HandleSetGMChatCommand,          "", nullptr},
        { "",               SEC_MODERATOR, false, &ChatHandler::HandleGMCommand,                  "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand goCommandTable[] =
    {
        { "creature",       SEC_OBSERVER, false, &ChatHandler::HandleGoCreatureCommand,          "", nullptr },
        { "object",         SEC_OBSERVER, false, &ChatHandler::HandleGoObjectCommand,            "", nullptr },
        { "target",         SEC_OBSERVER, false, &ChatHandler::HandleGoTargetCommand,            "", nullptr },
        { "taxinode",       SEC_OBSERVER, false, &ChatHandler::HandleGoTaxinodeCommand,          "", nullptr },
        { "trigger",        SEC_OBSERVER, false, &ChatHandler::HandleGoTriggerCommand,           "", nullptr },
        { "xyz",            SEC_OBSERVER, false, &ChatHandler::HandleGoXYZCommand,               "", nullptr },
        { "corpse",         SEC_DEVELOPER, false, &ChatHandler::HandleGoCorpseCommand, "Teleports the user to their corpse.", nullptr },
        { "graveyard",      SEC_OBSERVER, false, &ChatHandler::HandleGoGraveyardCommand,         "", nullptr },
        { "forward",        SEC_OBSERVER, false, &ChatHandler::HandleGoForwardCommand,           "", nullptr },
        { "up",             SEC_OBSERVER, false, &ChatHandler::HandleGoUpCommand,                "", nullptr },
        { "",               SEC_DEVELOPER, false, &ChatHandler::HandleGoCommand,                  "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand gobjectSetCommandTable[] =
    {
        { "gostate",     SEC_DEVELOPER,         false, &ChatHandler::HandleGameObjectSetGoStateCommand,      "", nullptr },
        { "lootstate",   SEC_DEVELOPER,         false, &ChatHandler::HandleGameObjectSetLootStateCommand,    "", nullptr },
        { "respawntime", SEC_DEVELOPER,         false, &ChatHandler::HandleGameObjectSetRespawnTimeCommand,  "", nullptr },
        { nullptr,          0,                  false, nullptr,                                              "", nullptr }
    };

    static ChatCommand gobjectCommandTable[] =
    {
        { "add",            SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectAddCommand,       "", nullptr },
        { "tmpadd",         SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectTempAddCommand,   "", nullptr },
        { "delete",         SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectDeleteCommand,    "", nullptr },
        { "move",           SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectMoveCommand,      "", nullptr },
        { "rotate",         SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectRotateCommand,    "", nullptr },
        { "near",           SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectNearCommand,      "", nullptr },
        { "target",         SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectTargetCommand,    "", nullptr },
        { "turn",           SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectTurnCommand,      "", nullptr },
        { "scale",          SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectScaleCommand,     "", nullptr },
        { "info",           SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectInfoCommand,      "", nullptr },
        { "select",         SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectSelectCommand,    "", nullptr },
        { "despawn",        SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectDespawnCommand,   "", nullptr },
        { "toggle",         SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectToggleCommand,    "", nullptr },
        { "reset",          SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectResetCommand,     "", nullptr },
        { "respawn",        SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectRespawnCommand,   "", nullptr },
        { "set",            SEC_DEVELOPER,     false, nullptr,                                       "", gobjectSetCommandTable },
        { "customanim",     SEC_DEVELOPER,     false, &ChatHandler::HandleGameObjectSendCustomAnimCommand,"", nullptr },
        { nullptr,          0,                 false, nullptr,                                       "", nullptr }
    };

    static ChatCommand guildCommandTable[] =
    {
        { "create",         SEC_DEVELOPER,     true,  &ChatHandler::HandleGuildCreateCommand,         "", nullptr },
        { "delete",         SEC_DEVELOPER,     true,  &ChatHandler::HandleGuildDeleteCommand,         "", nullptr },
        { "invite",         SEC_MODERATOR,     true,  &ChatHandler::HandleGuildInviteCommand,         "", nullptr },
        { "uninvite",       SEC_MODERATOR,     true,  &ChatHandler::HandleGuildUninviteCommand,       "", nullptr },
        { "rank",           SEC_DEVELOPER,     true,  &ChatHandler::HandleGuildRankCommand,           "", nullptr },
        { "rename",         SEC_DEVELOPER,     true,  &ChatHandler::HandleGuildRenameCommand,         "", nullptr },
        { "leader",         SEC_DEVELOPER,     false, &ChatHandler::HandleGuildLeaderCommand,         "", nullptr },
        { "listen",         SEC_DEVELOPER,     false, &ChatHandler::HandleGuildListenCommand,         "", nullptr },
        { "house",          SEC_DEVELOPER,     false, &ChatHandler::HandleGuildHouseCommand,          "", nullptr },
        { nullptr,          0,                 false, nullptr,                                        "", nullptr }
    };

    static ChatCommand honorCommandTable[] =
    {
        { "show",           SEC_DEVELOPER, false, &ChatHandler::HandleHonorShow,                  "", nullptr },
        { "setrp",          SEC_DEVELOPER, false, &ChatHandler::HandleHonorSetRPCommand,          "", nullptr },
        { nullptr,          0,             false, nullptr,                                        "", nullptr }
    };

    static ChatCommand instanceCommandTable[] =
    {
        { "continents",     SEC_DEVELOPER,    true,  &ChatHandler::HandleInstanceContinentsCommand,  "", nullptr },
        { "listbinds",      SEC_DEVELOPER,     false, &ChatHandler::HandleInstanceListBindsCommand,   "", nullptr },
        { "unbind",         SEC_DEVELOPER,     false, &ChatHandler::HandleInstanceUnbindCommand,      "", nullptr },
        { "groupunbind",    SEC_DEVELOPER,    false, &ChatHandler::HandleInstanceGroupUnbindCommand, "", nullptr },
        { "stats",          SEC_DEVELOPER,    true,  &ChatHandler::HandleInstanceStatsCommand,       "", nullptr },
        { "perfinfos",      SEC_DEVELOPER,    false, &ChatHandler::HandleInstancePerfInfosCommand,   "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand learnCommandTable[] =
    {
        { "all_myspells",   SEC_DEVELOPER,     false, &ChatHandler::HandleLearnAllMySpellsCommand,    "", nullptr },
        { "all_recipes",    SEC_DEVELOPER,     false, &ChatHandler::HandleLearnAllRecipesCommand,     "", nullptr },
        { "all_trainer",    SEC_DEVELOPER,     false, &ChatHandler::HandleLearnAllTrainerCommand,     "", nullptr },
        { "all_items",      SEC_DEVELOPER,     false, &ChatHandler::HandleLearnAllItemsCommand,       "", nullptr },
        { "",               SEC_DEVELOPER,     false, &ChatHandler::HandleLearnCommand,               "", nullptr },
        { nullptr,          0,                  false, nullptr,                                       "", nullptr }
    };

    static ChatCommand listCommandTable[] =
    {
        { "auras",          SEC_DEVELOPER,  false, &ChatHandler::HandleListAurasCommand,           "", nullptr },
        { "creature",       SEC_DEVELOPER,  true,  &ChatHandler::HandleListCreatureCommand,        "", nullptr },
        { "clicktomove",    SEC_DEVELOPER,  true,  &ChatHandler::HandleListClickToMoveCommand,     "", nullptr },
        { "exploredareas",  SEC_DEVELOPER,  false, &ChatHandler::HandleListExploredAreasCommand,   "", nullptr },
        { "object",         SEC_DEVELOPER,  true,  &ChatHandler::HandleListObjectCommand,          "", nullptr },
        { "buybackitems",   SEC_OBSERVER,   true,  &ChatHandler::HandleListBuybackItemsCommand,    "", nullptr },
        { "destroyeditems", SEC_OBSERVER,   true,  &ChatHandler::HandleListDestroyedItemsCommand,  "", nullptr },
        { "hostilerefs",    SEC_OBSERVER,   false, &ChatHandler::HandleListHostileRefsCommand,     "", nullptr },
        { "threat",         SEC_OBSERVER,   false, &ChatHandler::HandleListThreatCommand,          "", nullptr },
        { "battlegrounds",  SEC_DEVELOPER,  true, &ChatHandler::ListBattlegroundsCommand,         "", nullptr },
        { "maps",           SEC_DEVELOPER,  true, &ChatHandler::HandleListMapsCommand,            "", nullptr },
        { nullptr,          0,              false, nullptr,                                        "", nullptr }
    };


    // TODO: Add console output support to async player lookups
    static ChatCommand lookupPlayerCommandTable[] =
    {
        { "account",        SEC_MODERATOR,     false,  &ChatHandler::HandleLookupPlayerAccountCommand,   "", nullptr },
        { "email",          SEC_MODERATOR,    false,  &ChatHandler::HandleLookupPlayerEmailCommand,     "", nullptr },
        { "ip",             SEC_MODERATOR,    false,  &ChatHandler::HandleLookupPlayerIpCommand,        "", nullptr },
        { "name",           SEC_MODERATOR, false,  &ChatHandler::HandleLookupPlayerNameCommand,      "", nullptr },
        { "character",      SEC_MODERATOR, false,  &ChatHandler::HandleLookupPlayerCharacterCommand, "", nullptr },
        { nullptr,          0,                  false,  nullptr,                                          "", nullptr }
    };

    static ChatCommand lookupCommandTable[] =
    {
        { "creature",       SEC_MODERATOR, true,  &ChatHandler::HandleLookupCreatureCommand,      "", nullptr },
        { "faction",        SEC_MODERATOR, true,  &ChatHandler::HandleLookupFactionCommand,       "", nullptr },
        { "item",           SEC_MODERATOR, true,  &ChatHandler::HandleLookupItemCommand,          "", nullptr },
        { "object",         SEC_MODERATOR, true,  &ChatHandler::HandleLookupObjectCommand,        "", nullptr },
        { "quest",          SEC_MODERATOR, true,  &ChatHandler::HandleLookupQuestCommand,         "", nullptr },
        { "player",         SEC_MODERATOR, true,  nullptr,                                        "", lookupPlayerCommandTable },
        { "skill",          SEC_MODERATOR, true,  &ChatHandler::HandleLookupSkillCommand,         "", nullptr },
        { "spell",          SEC_MODERATOR, true,  &ChatHandler::HandleLookupSpellCommand,         "", nullptr },
        { "itemset",        SEC_MODERATOR, true,  &ChatHandler::HandleLookupItemSetCommand,       "", nullptr },
        { "event",          SEC_MODERATOR, true,  &ChatHandler::HandleLookupEventCommand,         "", nullptr },
        { "guild",          SEC_MODERATOR, true,  &ChatHandler::HandleLookupGuildCommand,         "", nullptr },
        { "hwprint",        SEC_DEVELOPER, false, &ChatHandler::HandleLookupHwPrintCommand,       "", nullptr },
		{ nullptr,          0,                  false, nullptr,                                    "", nullptr }
    };

    static ChatCommand modifyCommandTable[] =
    {
        { "hp",             SEC_DEVELOPER,     false, &ChatHandler::HandleModifyHPCommand,            "", nullptr },
        { "mana",           SEC_DEVELOPER,     false, &ChatHandler::HandleModifyManaCommand,          "", nullptr },
        { "rage",           SEC_DEVELOPER,     false, &ChatHandler::HandleModifyRageCommand,          "", nullptr },
        { "energy",         SEC_DEVELOPER,     false, &ChatHandler::HandleModifyEnergyCommand,        "", nullptr },
        { "money",          SEC_DEVELOPER,     false, &ChatHandler::HandleModifyMoneyCommand,         "", nullptr },
        { "aspeed",         SEC_DEVELOPER,     false, &ChatHandler::HandleModifyASpeedCommand,        "", nullptr },
        { "speed",          SEC_DEVELOPER,     false, &ChatHandler::HandleModifyASpeedCommand,        "", nullptr },
        { "scale",          SEC_DEVELOPER,     false, &ChatHandler::HandleModifyScaleCommand,         "", nullptr },
        { "faction",        SEC_DEVELOPER,     false, &ChatHandler::HandleModifyFactionCommand,       "", nullptr },
        { "rep",            SEC_DEVELOPER,     false, &ChatHandler::HandleModifyRepCommand,           "", nullptr },
        { "drunk",          SEC_DEVELOPER,     false, &ChatHandler::HandleModifyDrunkCommand,         "", nullptr },
        { "morph",          SEC_DEVELOPER,     false, &ChatHandler::HandleModifyMorphCommand,         "", nullptr },
        { "gender",         SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifyGenderCommand,        "", nullptr },
        { "strength",       SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifyStrengthCommand,      "", nullptr },
        { "agility",        SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifyAgilityCommand,       "", nullptr },
        { "stamina",        SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifyStaminaCommand,       "", nullptr },
        { "intellect",      SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifyIntellectCommand,     "", nullptr },
        { "spirit",         SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifySpiritCommand,        "", nullptr },
        { "ap",             SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifyMeleeApCommand,       "", nullptr },
        { "spellpower",     SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifySpellPowerCommand,    "", nullptr },
        { "parry",          SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifyParryCommand,         "", nullptr },
        { "honor",          SEC_ADMINISTRATOR, false, &ChatHandler::HandleModifyHonorCommand,         "", nullptr },
        { "xp",             SEC_DEVELOPER,     false, &ChatHandler::HandleModifyXpCommand,            "", nullptr },
        { "hairstyle",      SEC_DEVELOPER,     false, &ChatHandler::HandleModifyHairStyleCommand,     "", nullptr },
        { "haircolor",      SEC_DEVELOPER,     false, &ChatHandler::HandleModifyHairColorCommand,     "", nullptr },
        { "skincolor",      SEC_DEVELOPER,     false, &ChatHandler::HandleModifySkinColorCommand,     "", nullptr },
        { "face",           SEC_DEVELOPER,     false, &ChatHandler::HandleModifyFaceCommand,          "", nullptr },
        { "accessories",    SEC_DEVELOPER,     false, &ChatHandler::HandleModifyAccessoriesCommand,   "", nullptr },
        { "title",          SEC_DEVELOPER,     false, &ChatHandler::HandleModifyTitleCommand,         "", nullptr },
        { nullptr,          0,                 false, nullptr,                                        "", nullptr }
    };

    static ChatCommand mmapsCommandTable[] =
    {
        { "path",            SEC_DEVELOPER,     false, &ChatHandler::HandleMmapsPathCommand,         "", nullptr },//HandleMmapsNearCommand
        { "near",            SEC_DEVELOPER,     false, &ChatHandler::HandleMmapsNearCommand,         "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand factionCommandTable[] =
    {
        { "atwar",            SEC_DEVELOPER,     false, &ChatHandler::HandleFactionAtWarCommand,         "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };


    static ChatCommand creatureGroupsCommandTable[] =
    {
        { "add",            SEC_DEVELOPER,     false, &ChatHandler::HandleNpcGroupAddCommand,         "", nullptr },
        { "addrel",         SEC_DEVELOPER,     false, &ChatHandler::HandleNpcGroupAddRelCommand,      "", nullptr },
        { "del",            SEC_DEVELOPER,     false, &ChatHandler::HandleNpcGroupDelCommand,         "", nullptr },
        { "link",           SEC_DEVELOPER,     false, &ChatHandler::HandleNpcGroupLinkCommand,        "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand creatureSpawnsSetCommandTable[] =
    {
        { "entry",          SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetEntryCommand,       "", nullptr },
        { "displayid",      SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetDisplayIdCommand,   "", nullptr },
        { "emotestate",     SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetEmoteStateCommand,  "", nullptr },
        { "standstate",     SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetStandStateCommand,  "", nullptr },
        { "sheathstate",    SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetSheathStateCommand, "", nullptr },
        { "movetype",       SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetMoveTypeCommand,    "", nullptr },
        { "wanderdistance", SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnWanderDistCommand,     "", nullptr },
        { "respawntime",    SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetRespawnTimeCommand, "", nullptr },
        { "deathstate",     SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetDeathStateCommand,  "", nullptr },
        { "auras",          SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnSetAurasCommand,       "", nullptr },
        { nullptr,          0,                  false, nullptr,                                           "", nullptr }
    };

    static ChatCommand creatureSpawnsCommandTable[] =
    {
        { "add",            SEC_DEVELOPER,      false, &ChatHandler::HandleNpcAddCommand,              "", nullptr },
        { "addentry",       SEC_DEVELOPER,      false, &ChatHandler::HandleNpcAddEntryCommand,         "", nullptr },
        { "delete",         SEC_DEVELOPER,      false, &ChatHandler::HandleNpcDeleteCommand,           "", nullptr },
        { "info",           SEC_MODERATOR,      false, &ChatHandler::HandleNpcSpawnInfoCommand,        "", nullptr },
        { "set",            SEC_DEVELOPER,      false, nullptr,                                        "", creatureSpawnsSetCommandTable },
        { "move",           SEC_DEVELOPER,      false, &ChatHandler::HandleNpcSpawnMoveCommand,        "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand creatureTemplateSetCommandTable[] =
    {
        { "displayid",        SEC_DEVELOPER,    false, &ChatHandler::HandleNpcTemplateSetDisplayIdCommand,        "", nullptr },
        { "mountdisplayid",   SEC_DEVELOPER,    false, &ChatHandler::HandleNpcTemplateSetMountDisplayIdCommand,   "", nullptr },
        { "level",            SEC_DEVELOPER,    false, &ChatHandler::HandleNpcTemplateSetLevelCommand,            "", nullptr },
        { "faction",          SEC_DEVELOPER,    false, &ChatHandler::HandleNpcTemplateSetFactionCommand,          "", nullptr },
        { "scale",            SEC_DEVELOPER,    false, &ChatHandler::HandleNpcTemplateSetScaleCommand,            "", nullptr },
        { "detectionrange",   SEC_DEVELOPER,    false, &ChatHandler::HandleNpcTemplateSetDetectionRangeCommand,   "", nullptr },
        { "callforhelprange", SEC_DEVELOPER,    false, &ChatHandler::HandleNpcTemplateSetCallForHelpRangeCommand, "", nullptr },
        { "leashrange",       SEC_DEVELOPER,    false, &ChatHandler::HandleNpcTemplateSetLeashRangeCommand,       "", nullptr },
        { nullptr,          0,                  false, nullptr,                                                   "", nullptr }
    };

    static ChatCommand creatureTemplateCommandTable[] =
    {
        { "set",            SEC_DEVELOPER,      false, nullptr,                                        "", creatureTemplateSetCommandTable },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand npcCommandTable[] =
    {
        { "add",            SEC_DEVELOPER,     false, &ChatHandler::HandleNpcAddCommand,              "", nullptr },
        { "summon",         SEC_DEVELOPER,     false, &ChatHandler::HandleNpcSummonCommand,           "", nullptr},
        { "additem",        SEC_DEVELOPER,     false, &ChatHandler::HandleNpcAddVendorItemCommand,    "", nullptr },
        { "scale",          SEC_DEVELOPER,     false, &ChatHandler::HandleNpcScaleCommand,    "", nullptr },
        { "addweapon",      SEC_DEVELOPER,     false, &ChatHandler::HandleNpcAddWeaponCommand,        "", nullptr },
        { "aiinfo",         SEC_ADMINISTRATOR, false, &ChatHandler::HandleNpcAIInfoCommand,           "", nullptr },
        { "wander",         SEC_DEVELOPER,     false, &ChatHandler::HandleNpcSetWanderDistanceCommand,    "", nullptr },
        { "delete",         SEC_DEVELOPER,     false, &ChatHandler::HandleNpcDeleteCommand,           "", nullptr },
        { "delitem",        SEC_DEVELOPER,     false, &ChatHandler::HandleNpcDelVendorItemCommand,    "", nullptr },
        { "evade",          SEC_DEVELOPER,     false, &ChatHandler::HandleNpcEvadeCommand,            "", nullptr },
        { "factionid",      SEC_DEVELOPER,     false, &ChatHandler::HandleNpcFactionIdCommand,        "", nullptr },
        { "flag",           SEC_DEVELOPER,     false, &ChatHandler::HandleNpcFlagCommand,             "", nullptr },
        { "follow",         SEC_DEVELOPER,     false, &ChatHandler::HandleNpcFollowCommand,           "", nullptr },
        { "unfollow",       SEC_DEVELOPER,     false, &ChatHandler::HandleNpcUnfollowCommand,         "", nullptr },
        { "info",           SEC_DEVELOPER,     false, &ChatHandler::HandleNpcInfoCommand,             "", nullptr },
        { "move",           SEC_DEVELOPER,     false, &ChatHandler::HandleNpcMoveCommand,             "", nullptr },
        { "playemote",      SEC_DEVELOPER,     false, &ChatHandler::HandleNpcPlayEmoteCommand,        "", nullptr },
        { "say",            SEC_DEVELOPER,     false, &ChatHandler::HandleNpcSayCommand,              "", nullptr },
        { "speed",          SEC_DEVELOPER,     false, &ChatHandler::HandleNpcSpeedCommand,              "", nullptr },
        { "textemote",      SEC_DEVELOPER,     false, &ChatHandler::HandleNpcTextEmoteCommand,        "", nullptr },
        { "yell",           SEC_DEVELOPER,     false, &ChatHandler::HandleNpcYellCommand,             "", nullptr },
        { "toptarget",      SEC_DEVELOPER,     false, &ChatHandler::HandleNpcTopTarget,             "", nullptr },
        { "near",           SEC_DEVELOPER,     false, &ChatHandler::HandleNpcNearCommand,                "", nullptr},
        { "group",          SEC_ADMINISTRATOR, false, nullptr,                                        "", creatureGroupsCommandTable },
        { "spawn",          SEC_MODERATOR,     false, nullptr,                                        "", creatureSpawnsCommandTable },
        { "template",       SEC_DEVELOPER,     false, nullptr,                                        "", creatureTemplateCommandTable },
        { nullptr,          0,                 false, nullptr,                                        "", nullptr }
    };

    static ChatCommand unitCommandTable[] =
    {
        { "speedinfo",      SEC_DEVELOPER,     false, &ChatHandler::HandleUnitSpeedInfoCommand,       "", nullptr },
        { "statinfo",       SEC_DEVELOPER,     false, &ChatHandler::HandleUnitStatInfoCommand,        "", nullptr},
        { nullptr,          0,                 false, nullptr,                                        "", nullptr }
    };

    static ChatCommand pdumpCommandTable[] =
    {
        { "list",           SEC_ADMINISTRATOR,  true,  &ChatHandler::HandlePDumpListCommand,           "", nullptr },
        { "load",           SEC_ADMINISTRATOR,  true,  &ChatHandler::HandlePDumpLoadCommand,           "", nullptr },
        { "write",          SEC_ADMINISTRATOR,  true,  &ChatHandler::HandlePDumpWriteCommand,          "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand questCommandTable[] =
    {
        { "add",            SEC_DEVELOPER, false, &ChatHandler::HandleQuestAddCommand,            "", nullptr },
        { "complete",       SEC_DEVELOPER, false, &ChatHandler::HandleQuestCompleteCommand,       "", nullptr },
        { "status",         SEC_MODERATOR,  false, &ChatHandler::HandleQuestStatusCommand,         "", nullptr },
        { "remove",         SEC_DEVELOPER,  false, &ChatHandler::HandleQuestRemoveCommand,         "", nullptr },
        { "test",           SEC_DEVELOPER,  false, &ChatHandler::HandleQuestTestCommand,           "", nullptr },
        { nullptr,          0,               false, nullptr,                                        "", nullptr }
    };

    static ChatCommand petCommandTable[] =
    {
        { "learnspell",     SEC_DEVELOPER,     false, &ChatHandler::HandlePetLearnSpellCommand,        "", nullptr },
        { "unlearnspell",   SEC_DEVELOPER,     false, &ChatHandler::HandlePetUnlearnSpellCommand,      "", nullptr },
        { "list",           SEC_DEVELOPER,     true,  &ChatHandler::HandlePetListCommand,              "", nullptr },
        { "rename",         SEC_DEVELOPER,     true,  &ChatHandler::HandlePetRenameCommand,            "", nullptr },
        { "delete",         SEC_DEVELOPER,     true,  &ChatHandler::HandlePetDeleteCommand,            "", nullptr },
        { "loyalty",        SEC_DEVELOPER,     false, &ChatHandler::HandlePetLoyaltyCommand,           "", nullptr },
        { nullptr,          0,                 false, nullptr,                                         "", nullptr }
    };

    static ChatCommand reloadCommandTable[] =
    { 
        // turtle
        { "shop",                         SEC_ADMINISTRATOR,    true,  &ChatHandler::HandleReloadShopCommand,                    "", nullptr },
        { "bgplayers",                    SEC_ADMINISTRATOR,    true,  &ChatHandler::HandleReloadBgAmounts,                      "", nullptr},
        { "gmlevels",                     SEC_DEVELOPER,        true,  &ChatHandler::HandleReloadGmLevelsCommand,                "", nullptr},
        { "gm_ticket_template",           SEC_ADMINISTRATOR,    true,  &ChatHandler::HandleReloadGmTicketTemplatesCommand,       "", nullptr},
        { "housing",                      SEC_DEVELOPER,        true,  &ChatHandler::HandleReloadHousingCommand,                 "", nullptr},
        { "visibilities",                 SEC_DEVELOPER,        true,  &ChatHandler::HandleReloadDynamicVisibilityCommand,        "", nullptr},

        // vmangos
        { "areatrigger_involvedrelation",SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadQuestAreaTriggersCommand,       "", nullptr },
        { "areatrigger_tavern",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadAreaTriggerTavernCommand,       "", nullptr },
        { "areatrigger_teleport",        SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadAreaTriggerTeleportCommand,     "", nullptr },
        { "character_pet",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCharacterPetCommand,            "", nullptr },
        { "conditions",                  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadConditionsCommand,              "", nullptr },
        { "config",                      SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadConfigCommand,                  "", nullptr },
        { "creature",                    SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureCommand,                "", nullptr },
        { "creature_ai_events",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadEventAIEventsCommand,           "", nullptr },
        { "creature_battleground",       SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadBattleEventCommand,             "", nullptr },
        { "creature_involvedrelation",   SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureQuestInvRelationsCommand, "", nullptr },
        { "creature_loot_template",      SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesCreatureCommand,   "", nullptr },
        { "creature_questrelation",      SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureQuestRelationsCommand,  "", nullptr },
        { "creature_spells",             SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureSpellsCommand,          "", nullptr },
        { "creature_spells_scripts",     SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureSpellScriptsCommand,    "", nullptr },
        { "creature_template",           SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureTemplateCommand,        "", nullptr },
        { "disenchant_loot_template",    SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesDisenchantCommand, "", nullptr },
        { "event_scripts",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadEventScriptsCommand,            "", nullptr },
        { "fishing_loot_template",       SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesFishingCommand,    "", nullptr },
        { "game_graveyard_zone",         SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGameGraveyardZoneCommand,       "", nullptr },
        { "game_tele",                   SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGameTeleCommand,                "", nullptr },
        { "taxi_path_transitions",       SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadTaxiPathTransitionsCommand,     "", nullptr },
        { "gameobject",                  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGameObjectCommand,              "", nullptr },
        { "gameobject_involvedrelation", SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGOQuestInvRelationsCommand,     "", nullptr },
        { "gameobject_loot_template",    SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesGameobjectCommand, "", nullptr },
        { "gameobject_questrelation",    SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGOQuestRelationsCommand,        "", nullptr },
        { "gameobject_requirement",      SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGORequirementsCommand,          "", nullptr },
        { "gameobject_scripts",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGameObjectScriptsCommand,       "", nullptr },
        { "gameobject_battleground",     SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadBattleEventCommand,             "", nullptr },
        { "gossip_menu",                 SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGossipMenuCommand,              "", nullptr },
        { "gossip_menu_option",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGossipMenuOptionCommand,        "", nullptr },
        { "generic_scripts",             SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGenericScriptsCommand,          "", nullptr },
        { "gossip_scripts",              SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGossipScriptsCommand,           "", nullptr },
        { "item_enchantment_template",   SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadItemEnchantementsCommand,       "", nullptr },
        { "item_loot_template",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesItemCommand,       "", nullptr },
        { "item_required_target",        SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadItemRequiredTragetCommand,      "", nullptr },
        { "item_template",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadItemTemplate,                   "", nullptr },
        { "locales_creature",            SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLocalesCreatureCommand,         "", nullptr },
        { "locales_gameobject",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLocalesGameobjectCommand,       "", nullptr },
        { "locales_gossip_menu_option",  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLocalesGossipMenuOptionCommand, "", nullptr },
        { "locales_item",                SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLocalesItemCommand,             "", nullptr },
        { "locales_page_text",           SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLocalesPageTextCommand,         "", nullptr },
        { "locales_points_of_interest",  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLocalesPointsOfInterestCommand, "", nullptr },
        { "locales_quest",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLocalesQuestCommand,            "", nullptr },
        { "mail_loot_template",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesMailCommand,       "", nullptr },
        { "mangos_string",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadMangosStringCommand,            "", nullptr },
        { "module_string",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadModuleStringCommand,            "", nullptr },
        { "module_string_locale",        SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadModuleStringCommand,            "", nullptr },
        { "npc_gossip",                  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadNpcGossipCommand,               "", nullptr },
        { "npc_text",                    SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadNpcTextCommand,                 "", nullptr },
        { "npc_trainer",                 SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadNpcTrainerCommand,              "", nullptr },
        { "npc_vendor",                  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadNpcVendorCommand,               "", nullptr },
        { "page_text",                   SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadPageTextsCommand,               "", nullptr },
        { "pickpocketing_loot_template", SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesPickpocketingCommand, "", nullptr},
        { "points_of_interest",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadPointsOfInterestCommand,        "", nullptr },
        { "quest_end_scripts",           SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadQuestEndScriptsCommand,         "", nullptr },
        { "quest_start_scripts",         SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadQuestStartScriptsCommand,       "", nullptr },
        { "quest_template",              SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadQuestTemplateCommand,           "", nullptr },
        { "quest_greeting",              SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadQuestGreetingCommand,           "", nullptr },
        { "trainer_greeting",            SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadTrainerGreetingCommand,         "", nullptr },
        { "reference_loot_template",     SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesReferenceCommand,  "", nullptr },
        { "reserved_name",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadReservedNameCommand,            "", nullptr },
        { "reputation_reward_rate",      SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadReputationRewardRateCommand,    "", nullptr },
        { "reputation_spillover_template", SEC_ADMINISTRATOR, true, &ChatHandler::HandleReloadReputationSpilloverTemplateCommand, "", nullptr },
        { "skill_fishing_base_level",    SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSkillFishingBaseLevelCommand,   "", nullptr },
        { "skinning_loot_template",      SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadLootTemplatesSkinningCommand,   "", nullptr },
        { "spell_affect",                SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellAffectCommand,             "", nullptr },
        { "spell_area",                  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellAreaCommand,               "", nullptr },
        { "spell_chain",                 SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellChainCommand,              "", nullptr },
        { "spell_elixir",                SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellElixirCommand,             "", nullptr },
        { "spell_learn_spell",           SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellLearnSpellCommand,         "", nullptr },
        { "spell_pet_auras",             SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellPetAurasCommand,           "", nullptr },
        { "spell_proc_event",            SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellProcEventCommand,          "", nullptr },
        { "spell_proc_item_enchant",     SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellProcItemEnchantCommand,    "", nullptr },
        { "spell_script_target",         SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellScriptTargetCommand,       "", nullptr },
        { "spell_scripts",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellScriptsCommand,            "", nullptr },
        { "spell_target_position",       SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellTargetPositionCommand,     "", nullptr },
        { "spell_threats",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellThreatsCommand,            "", nullptr },
        { "spell_disabled",              SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellDisabledCommand,           "", nullptr },
        { "autobroadcast",               SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadAutoBroadcastCommand,           "", nullptr },
        { "spell_mod",                   SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellModsCommand,               "", nullptr },
        { "map_loot_disabled",           SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadMapLootDisabledCommand,         "", nullptr },
        { "cinematic_waypoints",         SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCinematicWaypointsCommand,      "", nullptr },
        { "variables",                   SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadVariablesCommand,               "", nullptr },
        { "spell_group",                 SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellGroupCommand,              "", nullptr },
        { "spell_group_stack_rules",     SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadSpellGroupStackRulesCommand,    "", nullptr },
        { "creature_groups",             SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureGroupsCommand,          "", nullptr },
        { "exploration_basexp",          SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadExplorationBaseXp,              "", nullptr },
        { "pet_name_generation",         SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadPetNameGeneration,              "", nullptr },
        { "creature_onkill_reputation",  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureOnKillReputation,       "", nullptr },
        { "game_weather",                SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadGameWeather,                    "", nullptr },
        { "player_factionchange_reputations", SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadFactionChangeReputations,  "", nullptr },
        { "player_factionchange_spells", SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadFactionChangeSpells,            "", nullptr },
        { "player_factionchange_items",  SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadFactionChangeItems,             "", nullptr },
        { "player_factionchange_quests", SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadFactionChangeQuests,            "", nullptr },
        { "player_factionchange_mounts", SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadFactionChangeMounts,            "", nullptr },
        { "creature_display_info_addon", SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadCreatureDisplayInfoAddon,       "", nullptr },
        { "ip_banned",                   SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadIPBanList,                      "", nullptr },
        { "account_banned",              SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadAccountBanList,                 "", nullptr },
        { "instance_buff_removal",       SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadInstanceBuffRemoval,            "", nullptr },
        { "petitions",                   SEC_ADMINISTRATOR, true,  &ChatHandler::HandleReloadPetitions,                      "", nullptr },
        { nullptr,                       0,                     false, nullptr,                                              "", nullptr }
    };

    static ChatCommand resetCommandTable[] =
    {
        { "honor",          SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleResetHonorCommand,          "", nullptr },
        { "level",          SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleResetLevelCommand,          "", nullptr },
        { "spells",         SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleResetSpellsCommand,         "", nullptr },
        { "stats",          SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleResetStatsCommand,          "", nullptr },
        { "talents",        SEC_DEVELOPER,        true,  &ChatHandler::HandleResetTalentsCommand,        "", nullptr },
        { "all",            SEC_CONSOLE,           true,  &ChatHandler::HandleResetAllCommand,            "", nullptr },
        { nullptr,          0,                     false, nullptr,                                        "", nullptr }
    };

    static ChatCommand sendCommandTable[] =
    {
        { "items",          SEC_DEVELOPER,  true,  &ChatHandler::HandleSendItemsCommand,           "", nullptr },
        { "mail",           SEC_MODERATOR,      true,  &ChatHandler::HandleSendMailCommand,            "", nullptr },
        { "message",        SEC_DEVELOPER,  true,  &ChatHandler::HandleSendMessageCommand,         "", nullptr },
        { "money",          SEC_DEVELOPER,  true,  &ChatHandler::HandleSendMoneyCommand,           "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand serverIdleRestartCommandTable[] =
    {
        { "cancel",         SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerShutDownCancelCommand, "", nullptr },
        { ""   ,            SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerIdleRestartCommand,    "", nullptr },
        { nullptr,          0,                  false, nullptr,                                         "", nullptr }
    };

    static ChatCommand serverIdleShutdownCommandTable[] =
    {
        { "cancel",         SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerShutDownCancelCommand, "", nullptr },
        { ""   ,            SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerIdleShutDownCommand,   "", nullptr },
        { nullptr,          0,                  false, nullptr,                                         "", nullptr }
    };

    static ChatCommand serverRestartCommandTable[] =
    {
        { "cancel",         SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerShutDownCancelCommand, "", nullptr },
        { ""   ,            SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerRestartCommand,        "", nullptr },
        { nullptr,          0,                  false, nullptr,                                         "", nullptr }
    };

    static ChatCommand serverShutdownCommandTable[] =
    {
        { "cancel",         SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerShutDownCancelCommand, "", nullptr },
        { ""   ,            SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerShutDownCommand,       "", nullptr },
        { nullptr,          0,                  false, nullptr,                                         "", nullptr }
    };
       

    static ChatCommand serverCommandTable[] =
    {
        { "corpses",        SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerCorpsesCommand,     "", nullptr },
        { "exit",           SEC_CONSOLE,        true,  &ChatHandler::HandleServerExitCommand,        "", nullptr },
        { "idlerestart",    SEC_ADMINISTRATOR,  true,  nullptr,                                      "", serverIdleRestartCommandTable },
        { "idleshutdown",   SEC_ADMINISTRATOR,  true,  nullptr,                                      "", serverIdleShutdownCommandTable },
        { "info",           SEC_PLAYER,         true,  &ChatHandler::HandleServerInfoCommand,        "", nullptr },
        { "resetallraids",  SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleServerResetAllRaidCommand, "", nullptr },
        { "restart",        SEC_ADMINISTRATOR,  true,  nullptr,                                       "", serverRestartCommandTable },
        { "shutdown",       SEC_ADMINISTRATOR,  true,  nullptr,                                       "", serverShutdownCommandTable },
        { nullptr,          0,                  false, nullptr,                                       "", nullptr }
    };

    static ChatCommand teleCommandTable[] =
    {
        { "add",            SEC_DEVELOPER,     false, &ChatHandler::HandleTeleAddCommand,             "", nullptr },
        { "del",            SEC_DEVELOPER,     true,  &ChatHandler::HandleTeleDelCommand,             "", nullptr },
        { "name",           SEC_DEVELOPER,     true,  &ChatHandler::HandleTeleNameCommand,            "", nullptr },
        { "group",          SEC_DEVELOPER,     false, &ChatHandler::HandleTeleGroupCommand,           "", nullptr },
        { "",               SEC_OBSERVER,      false, &ChatHandler::HandleTeleCommand,                "", nullptr },
        { nullptr,          0,                 false, nullptr,                                        "", nullptr }
    };

    static ChatCommand triggerCommandTable[] =
    {
        { "active",         SEC_OBSERVER,   false, &ChatHandler::HandleTriggerActiveCommand,       "", nullptr },
        { "near",           SEC_OBSERVER,   false, &ChatHandler::HandleTriggerNearCommand,         "", nullptr },
        { "",               SEC_OBSERVER,   true,  &ChatHandler::HandleTriggerCommand,             "", nullptr },
        { nullptr,          0,              false, nullptr,                                        "", nullptr }
    };

    static ChatCommand unbanCommandTable[] =
    {
        { "account",        SEC_DEVELOPER,  true,  &ChatHandler::HandleUnBanAccountCommand,      "", nullptr },
        { "character",      SEC_DEVELOPER,  true,  &ChatHandler::HandleUnBanCharacterCommand,    "", nullptr },
        { "ip",             SEC_DEVELOPER,  true,  &ChatHandler::HandleUnBanIPCommand,           "", nullptr },
        { "fingerprint",    SEC_DEVELOPER,  true,  &ChatHandler::HandleUnBanFingerprintCommand,  "", nullptr },
        { nullptr,          0,                  false, nullptr,                                      "", nullptr }
    };

    static ChatCommand wpCommandTable[] =
    {
        { "show",           SEC_ADMINISTRATOR, false, &ChatHandler::HandleWpShowCommand,              "", nullptr },
        { "add",            SEC_ADMINISTRATOR,     false, &ChatHandler::HandleWpAddCommand,               "", nullptr },
        { "modify",         SEC_ADMINISTRATOR,     false, &ChatHandler::HandleWpModifyCommand,            "", nullptr },
        { "export",         SEC_ADMINISTRATOR,  false, &ChatHandler::HandleWpExportCommand,            "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand worldStateCommandTable[] =
    {
        { "test",              SEC_ADMINISTRATOR,        false, &ChatHandler::HandleUpdateWorldStateCommand, "", nullptr},
        { nullptr,             0,                        false, nullptr,                                     "", nullptr}
    };
    static ChatCommand bgCommandTable[] =
    {
        { "status",            SEC_ADMINISTRATOR,           false, &ChatHandler::HandleBGStatusCommand,         "", nullptr},
        { "start",             SEC_ADMINISTRATOR,           false, &ChatHandler::HandleBGStartCommand,          "", nullptr},
        { "stop",              SEC_ADMINISTRATOR,           false, &ChatHandler::HandleBGStopCommand,           "", nullptr},
        { "",                  SEC_ADMINISTRATOR,           false, &ChatHandler::HandleBGCustomCommand,         "", nullptr },
        { nullptr,             0,                        false, nullptr,                                     "", nullptr}
    };

    static ChatCommand anticheatClientCommandTable[] =
    {
        { "find",           SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleClientSearchCommand,        "", nullptr },
        { "",               SEC_ADMINISTRATOR,     true,  &ChatHandler::HandleClientInfosCommand,         "", nullptr },
        { nullptr,          0,                  false, nullptr,                                        "", nullptr }
    };

    static ChatCommand anticheatWardenCommandTable[] =
    {
        { "read",          SEC_ADMINISTRATOR,    false, &ChatHandler::HandleWardenReadCommand,           "", nullptr },
        { "",              SEC_ADMINISTRATOR,    true,  &ChatHandler::HandleWardenCommand,               "", nullptr },
        { nullptr,         0,                  false, nullptr,                                         "", nullptr }
    };

    static ChatCommand ticketCommandTable[] =
    {
        { "assign",        SEC_MODERATOR,  true,  &ChatHandler::HandleGMTicketAssignToCommand,         "", nullptr },
        { "unassign",      SEC_MODERATOR,  true,  &ChatHandler::HandleGMTicketUnAssignCommand,         "", nullptr },
        { "close",         SEC_MODERATOR,  true,  &ChatHandler::HandleGMTicketCloseByIdCommand,        "", nullptr },
        { "list",          SEC_MODERATOR,  true,  &ChatHandler::HandleGMTicketListCommand,             "", nullptr },
        { "onlinelist",    SEC_MODERATOR,  true,  &ChatHandler::HandleGMTicketsOnlineListCommand,      "", nullptr},
        { "",              SEC_MODERATOR,  true,  &ChatHandler::HandleGMTicketGetByIdOrNameCommand,    "", nullptr },
        { nullptr,         0,                   false, nullptr,                                             "", nullptr }
    };
    
    static ChatCommand serviceCommandTable[] =
    {
        { "del_characters",     SEC_ADMINISTRATOR,    true,  &ChatHandler::HandleServiceDeleteCharacters,   "", nullptr },
        { nullptr,              0,                    false, nullptr,                                       "", nullptr }
    };

    static ChatCommand goldCommandTable[] =
    {
        { "remove",            SEC_DEVELOPER,        false, &ChatHandler::HandleGoldRemoval,               "", nullptr },
        { nullptr,             0,                     false, nullptr,                                        "", nullptr }
    };

    static ChatCommand shopCommandTable[] =
    {
        { "log",               SEC_DEVELOPER,        false, &ChatHandler::HandleGetShopLogs,               "", nullptr },
        { "refund",           SEC_DEVELOPER,        false, &ChatHandler::HandleShopRefundCommand,          "", nullptr },
        { nullptr,             0,                     false, nullptr,                                        "", nullptr }
    };

    static ChatCommand pathCommandTable[] =
    {
        { "setup",             SEC_DEVELOPER,        false, &ChatHandler::HandleCreaturePathSetup,         "", nullptr },
        { "add",               SEC_DEVELOPER,        false, &ChatHandler::HandleCreaturePathAddPoint,      "", nullptr },
        { "start",             SEC_DEVELOPER,        false, &ChatHandler::HandleCreaturePathLaunch,        "", nullptr },
        { nullptr,             0,                     false, nullptr,                                       "", nullptr }
    };

    static ChatCommand anonymousCommandTable[] =
    {
        { "whispers",          SEC_DEVELOPER,       false, &ChatHandler::HandleAnonymousWhispers,         "", nullptr},
        { "mail",              SEC_DEVELOPER,       false, &ChatHandler::HandleAnonymousMail,         "", nullptr},
        { nullptr,             0,                     false, nullptr,                                       "", nullptr }
    };

    static ChatCommand discBotCommandTable[] =
    {
        { "stop",             SEC_ADMINISTRATOR,       true, &ChatHandler::HandleDiscBotStopCommand,         "", nullptr},
        { nullptr,             0,                     false, nullptr,                                       "", nullptr }
    };

    static ChatCommand blacklistCommandTable[] =
    {
        { "name",              SEC_DEVELOPER,       true, &ChatHandler::HandleBlacklistNameCommand,     "", nullptr},
        { nullptr,             0,                     false, nullptr,                                       "", nullptr }
    };

	static ChatCommand perfCommandTable[] =
	{
		{ "enable",            SEC_ADMINISTRATOR,       true, &ChatHandler::HandlePerfEnable,           "", nullptr},
		{ "intervalreport",    SEC_ADMINISTRATOR,       true, &ChatHandler::HandlePerfIntervalReport,   "", nullptr},
		{ "resources",         SEC_DEVELOPER,           true, &ChatHandler::HandlePerfStatsCommand,     "", nullptr},
		{ "cpu",               SEC_DEVELOPER,           true, &ChatHandler::HandlePerfReportCPU,        "", nullptr},
		{ "memory",            SEC_DEVELOPER,           true, &ChatHandler::HandlePerfReportMemory,     "", nullptr},
		{ nullptr,             0,                     false, nullptr,                                       "", nullptr }
	};

    static ChatCommand commandTable[] =
    {
        { "account",        SEC_PLAYER,          true,  nullptr,                                        "", accountCommandTable  },
        { "blacklist",      SEC_ADMINISTRATOR,   true,  nullptr,                                        "", blacklistCommandTable  },
        { "cast",           SEC_DEVELOPER,       false, nullptr,                                        "", castCommandTable     },
        { "castcustom",     SEC_DEVELOPER,       false, &ChatHandler::HandleCastCustomCommand,          "", nullptr },
        { "character",      SEC_DEVELOPER,       true,  nullptr,                                        "", characterCommandTable},
        { "rename",         SEC_MODERATOR,       true,  &ChatHandler::HandleCharacterRenameCommand,     "", nullptr},
        { "event",          SEC_DEVELOPER,       false, nullptr,                                        "", eventCommandTable    },
        { "gm",             SEC_OBSERVER,        true,  nullptr,                                        "", gmCommandTable       },
        { "honor",          SEC_ADMINISTRATOR,   false, nullptr,                                        "", honorCommandTable    },
        { "go",             SEC_OBSERVER,        false, nullptr,                                        "", goCommandTable       },
        { "gobject",        SEC_DEVELOPER,       false, nullptr,                                        "", gobjectCommandTable  },
        { "guild",          SEC_DEVELOPER,       true,  nullptr,                                        "", guildCommandTable    },
        { "instance",       SEC_ADMINISTRATOR,   true,  nullptr,                                        "", instanceCommandTable },
        { "learn",          SEC_DEVELOPER,       false, nullptr,                                        "", learnCommandTable    },
        { "list",           SEC_ADMINISTRATOR,   true,  nullptr,                                        "", listCommandTable     },
        { "lookup",         SEC_MODERATOR,       true,  nullptr,                                        "", lookupCommandTable   },
        { "modify",         SEC_DEVELOPER,       false, nullptr,                                        "", modifyCommandTable   },
        { "npc",            SEC_DEVELOPER,       false, nullptr,                                        "", npcCommandTable      },
        { "unit",           SEC_DEVELOPER,       false, nullptr,                                        "", unitCommandTable      },
        { "pdump",          SEC_ADMINISTRATOR,   true,  nullptr,                                        "", pdumpCommandTable    },
        { "quest",          SEC_DEVELOPER,       false, nullptr,                                        "", questCommandTable    },
        { "reload",         SEC_ADMINISTRATOR,   true,  nullptr,                                        "", reloadCommandTable   },
        { "reset",          SEC_DEVELOPER,       true,  nullptr,                                        "", resetCommandTable    },
        { "server",         SEC_PLAYER,          true,  nullptr,                                        "", serverCommandTable   },
        { "tele",           SEC_OBSERVER,        true,  nullptr,                                        "", teleCommandTable     },
        { "trigger",        SEC_OBSERVER,        false, nullptr,                                        "", triggerCommandTable  },
        { "wp",             SEC_ADMINISTRATOR,   false, nullptr,                                        "", wpCommandTable       },
        { "service",        SEC_ADMINISTRATOR,   true,  nullptr,                                        "", serviceCommandTable  },
        { "worldstate",     SEC_ADMINISTRATOR,   false, nullptr,                                        "", worldStateCommandTable},
        { "bg",             SEC_ADMINISTRATOR,   false, nullptr,                                        "", bgCommandTable},
        { "debug",          SEC_DEVELOPER,       false, nullptr,                                        "", debugCommandTable},
        { "wareffort",      SEC_DEVELOPER,       true, nullptr,                                         "", warEffortCommandTable},
        { "sus",            SEC_DEVELOPER,       false, nullptr,                                        "", suspiciousCommandTable},
        { "cartographer",   SEC_PLAYER,          false, &ChatHandler::HandleCartographerCommand,        "", nullptr},
        { "variable",       SEC_ADMINISTRATOR,   true,  &ChatHandler::HandleVariableCommand,            "", nullptr},
        { "god",            SEC_DEVELOPER,       false, &ChatHandler::HandleGodCommand,                 "", nullptr },
        { "aura",           SEC_DEVELOPER,       false, &ChatHandler::HandleAuraCommand,                "", nullptr },
        { "unaura",         SEC_DEVELOPER,       false, &ChatHandler::HandleUnAuraCommand,              "", nullptr },
        { "broadcast",      SEC_ADMINISTRATOR,  true,  &ChatHandler::HandleAnnounceCommand,            "", nullptr },
        { "notify",         SEC_ADMINISTRATOR,   true,  &ChatHandler::HandleNotifyCommand,              "", nullptr },
        { "goname",         SEC_OBSERVER,        false, &ChatHandler::HandleGonameCommand,              "", nullptr },
        { "summon",         SEC_MODERATOR,       false, &ChatHandler::HandleSummonCommand,              "", nullptr },
        { "groupgo",        SEC_MODERATOR,       false, &ChatHandler::HandleGroupgoCommand,             "", nullptr },
        { "appear",         SEC_OBSERVER,        false, &ChatHandler::HandleGonameCommand,              "", nullptr },
        { "groupsummon",    SEC_MODERATOR,       false, &ChatHandler::HandleGroupgoCommand,             "", nullptr },
        { "commands",       SEC_PLAYER,          true,  &ChatHandler::HandleCommandsCommand,            "", nullptr },
        { "demorph",        SEC_MODERATOR,       false, &ChatHandler::HandleDeMorphCommand,             "", nullptr },
        { "die",            SEC_DEVELOPER,       false, &ChatHandler::HandleDieCommand,                 "", nullptr },
        { "fear",           SEC_DEVELOPER,       false, &ChatHandler::HandleFearCommand,                "", nullptr },
        { "revive",         SEC_DEVELOPER,       true,  &ChatHandler::HandleReviveCommand,              "", nullptr },
        { "replenish",      SEC_DEVELOPER,       false, &ChatHandler::HandleReplenishCommand,           "", nullptr },
        { "dismount",       SEC_DEVELOPER,       false, &ChatHandler::HandleDismountCommand,            "", nullptr },
        { "los",            SEC_OBSERVER,        false, &ChatHandler::HandleLoSCommand,                 "", nullptr },
        { "gps",            SEC_OBSERVER,        false, &ChatHandler::HandleGPSCommand,                 "", nullptr },
        { "guid",           SEC_MODERATOR,       false, &ChatHandler::HandleGUIDCommand,                "", nullptr },
        { "help",           SEC_OBSERVER,        true,  &ChatHandler::HandleHelpCommand,                "", nullptr },
        { "itemmove",       SEC_DEVELOPER,       false, &ChatHandler::HandleItemMoveCommand,            "", nullptr },
        { "cooldown",       SEC_DEVELOPER,       false, &ChatHandler::HandleCooldownCommand,            "", nullptr },
        { "unlearn",        SEC_DEVELOPER,       false, &ChatHandler::HandleUnLearnCommand,             "", nullptr },
        { "unlearnoffline", SEC_DEVELOPER,       false, &ChatHandler::HandleUnLearnOfflineCommand,      "", nullptr },
        { "distance",       SEC_DEVELOPER,       false, &ChatHandler::HandleGetDistanceCommand,         "", nullptr },
        { "recall",         SEC_DEVELOPER,        false, &ChatHandler::HandleRecallCommand,              "", nullptr },
        { "save",           SEC_DEVELOPER,       false, &ChatHandler::HandleSaveCommand,                "", nullptr },
        { "wareffortget",   SEC_ADMINISTRATOR,   true,  &ChatHandler::HandleGetWarEffortResource,       "", nullptr },
        { "wareffortset",   SEC_ADMINISTRATOR,   true,  &ChatHandler::HandleSetWarEffortResource,       "", nullptr },
        { "saveall",        SEC_ADMINISTRATOR,   true,  &ChatHandler::HandleSaveAllCommand,             "", nullptr },
        { "kick",           SEC_MODERATOR,       true,  &ChatHandler::HandleKickPlayerCommand,          "", nullptr },
        { "ban",            SEC_MODERATOR,       true,  nullptr,                                        "", banCommandTable      },
        { "unban",          SEC_DEVELOPER,       true,  nullptr,                                        "", unbanCommandTable    },
        { "baninfo",        SEC_MODERATOR,       false, nullptr,                                        "", baninfoCommandTable  },
        { "banlist",        SEC_DEVELOPER,       true,  nullptr,                                        "", banlistCommandTable  },
        { "start",          SEC_DEVELOPER,       false, &ChatHandler::HandleStartCommand,               "", nullptr },
        { "unstuck",        SEC_MODERATOR,       false, &ChatHandler::HandleUnstuckCommand,             "", nullptr },
        { "taxicheat",      SEC_DEVELOPER,       false, &ChatHandler::HandleTaxiCheatCommand,           "", nullptr },
        { "linkgrave",      SEC_ADMINISTRATOR,   false, &ChatHandler::HandleLinkGraveCommand,           "", nullptr },
        { "neargrave",      SEC_DEVELOPER,       false, &ChatHandler::HandleNearGraveCommand,           "", nullptr },
        { "explorecheat",   SEC_DEVELOPER,       false, &ChatHandler::HandleExploreCheatCommand,        "", nullptr },
        { "hover",          SEC_ADMINISTRATOR,   false, &ChatHandler::HandleHoverCommand,               "", nullptr },
        { "levelup",        SEC_DEVELOPER,       false, &ChatHandler::HandleLevelUpCommand,             "", nullptr },
        { "showarea",       SEC_ADMINISTRATOR,   false, &ChatHandler::HandleShowAreaCommand,            "", nullptr },
        { "hidearea",       SEC_ADMINISTRATOR,   false, &ChatHandler::HandleHideAreaCommand,            "", nullptr },
        { "additem",        SEC_DEVELOPER,       false, &ChatHandler::HandleAddItemCommand,             "", nullptr },
        { "deleteitem",     SEC_DEVELOPER,       false, &ChatHandler::HandleDeleteItemCommand,          "", nullptr },
        { "additemset",     SEC_DEVELOPER,       false, &ChatHandler::HandleAddItemSetCommand,          "", nullptr },
        { "bank",           SEC_DEVELOPER,       false, &ChatHandler::HandleBankCommand,                "", nullptr },
        { "wchange",        SEC_DEVELOPER,       false, &ChatHandler::HandleChangeWeatherCommand,       "", nullptr },
        { "ticket",         SEC_MODERATOR,       true,  nullptr,                                        "",ticketCommandTable },
        { "maxskill",       SEC_DEVELOPER,       false, &ChatHandler::HandleMaxSkillCommand,            "", nullptr },
        { "setskill",       SEC_DEVELOPER,       false, &ChatHandler::HandleSetSkillCommand,            "", nullptr },
        { "whispers",       SEC_MODERATOR,       false, &ChatHandler::HandleWhispersCommand,            "", nullptr },
        { "pinfo",          SEC_MODERATOR,       true, &ChatHandler::HandlePInfoCommand,               "", nullptr },
        { "groupinfo",      SEC_DEVELOPER,       true,  &ChatHandler::HandleGroupInfoCommand,           "", nullptr },
        { "addons",         SEC_DEVELOPER,       false, &ChatHandler::HandleListAddonsCommand,          "", nullptr },
        { "respawn",        SEC_DEVELOPER,       false, &ChatHandler::HandleRespawnCommand,             "", nullptr },
        { "send",           SEC_DEVELOPER,       true,  nullptr,                                        "", sendCommandTable     },
        { "mute",           SEC_MODERATOR,       true,  &ChatHandler::HandleMuteCommand,                "", nullptr },
        { "pausingmute",    SEC_MODERATOR,       true,  &ChatHandler::HandlePausingMuteCommand,         "", nullptr },
        { "unmute",         SEC_MODERATOR,       true,  &ChatHandler::HandleUnmuteCommand,              "", nullptr },
        { "movegens",       SEC_DEVELOPER,       false, &ChatHandler::HandleMovegensCommand,            "", nullptr },
        { "cometome",       SEC_DEVELOPER,       false, &ChatHandler::HandleComeToMeCommand,            "", nullptr },
        { "aoedamage",      SEC_DEVELOPER,       false, &ChatHandler::HandleAoEDamageCommand,              "", nullptr },
        { "damage",         SEC_DEVELOPER,       false, &ChatHandler::HandleDamageCommand,              "", nullptr },
        { "combatstop",     SEC_MODERATOR,       false, &ChatHandler::HandleCombatStopCommand,          "", nullptr },
        { "repairitems",    SEC_DEVELOPER,       true,  &ChatHandler::HandleRepairitemsCommand,         "", nullptr },
        { "waterwalk",      SEC_DEVELOPER,       false, &ChatHandler::HandleWaterwalkCommand,           "", nullptr },
        { "quit",           SEC_CONSOLE,         true,  &ChatHandler::HandleQuitCommand,                "", nullptr },
        { "freeze",         SEC_MODERATOR,       false, &ChatHandler::HandleFreezeCommand,              "", nullptr },
        { "unfreeze",       SEC_MODERATOR,       false, &ChatHandler::HandleUnfreezeCommand,            "", nullptr },
        { "anticheat",      SEC_DEVELOPER,       false, nullptr,                                        "", anticheatCommandTable },
        { "pet",            SEC_DEVELOPER,       true,  nullptr,                                        "", petCommandTable},
        { "gold",           SEC_DEVELOPER,       true,  nullptr,                                        "", goldCommandTable },
        { "balance",        SEC_DEVELOPER,       true,  &ChatHandler::HandleBalanceCommand,             "", nullptr },
        { "mailbox",        SEC_DEVELOPER,       false, &ChatHandler::HandleMailboxCommand,             "", nullptr },
        { "mount",          SEC_DEVELOPER,       false, &ChatHandler::HandleMountCommand,               "", nullptr },
        { "transfer",       SEC_DEVELOPER,       false, &ChatHandler::HandleTransferCommand,            "", nullptr },
        { "bgtest",         SEC_ADMINISTRATOR,   false, &ChatHandler::HandleBgTestCommand,              "", nullptr },
        { "cinematic",      SEC_ADMINISTRATOR,   false, &ChatHandler::HandleTurtleCinematic,            "", nullptr },
        { "dbghard",        SEC_ADMINISTRATOR,   false, &ChatHandler::HandleFastDebugCommand,           "", nullptr },
        { "debugloot",      SEC_DEVELOPER,       true,  &ChatHandler::HandleDebugLootTableCommand,      "", nullptr },
        { "loothelper",     SEC_DEVELOPER,       false, &ChatHandler::HandleLootHelperCommand,          "", nullptr },
        { "anim",           SEC_DEVELOPER,       false, &ChatHandler::HandleDebugAnimCommand,           "", nullptr },
        { "copy",           SEC_PLAYER,          false, &ChatHandler::HandleCopyCommand,                "", nullptr },
        { "guildname",      SEC_PLAYER,          false, &ChatHandler::HandleGuildNameCommand,           "", nullptr },
        { "petname",        SEC_PLAYER,          false, &ChatHandler::HandlePetNameCommand,             "", nullptr },
        { "sendpacket",     SEC_ADMINISTRATOR,   false, &ChatHandler::HandleSendPacketCommand,          "", nullptr },
        { "shop",           SEC_DEVELOPER,       true,  nullptr,                                        "", shopCommandTable },
        { "path",           SEC_DEVELOPER,       true,  nullptr,                                        "", pathCommandTable },
        { "xp",             SEC_PLAYER,          false, &ChatHandler::HandleXPCommand,                  "", nullptr },
        { "settime",        SEC_DEVELOPER,       false, &ChatHandler::HandleSetTimeCommand,             "", nullptr},
        { "play",           SEC_DEVELOPER,       false, &ChatHandler::HandlePlayCommand,                "", nullptr},
        { "radio",          SEC_PLAYER,          false, &ChatHandler::HandleRadioCommand,               "", nullptr},
        { "getskillvalue",  SEC_DEVELOPER,       false, &ChatHandler::HandleGetSkillValueCommand,       "", nullptr},
        { "anonymous",      SEC_DEVELOPER,       false, nullptr,                                        "", anonymousCommandTable},
        { "hcmessages",     SEC_PLAYER,          false, &ChatHandler::HandleHCMessagesCommand,          "", nullptr },
        { "discbot",        SEC_ADMINISTRATOR,   true,  nullptr,                                        "", discBotCommandTable},
        { "minchatlevel",   SEC_ADMINISTRATOR,   true,  &ChatHandler::HandleMinChatLevelCommand,        "", nullptr },
        { "pvp",            SEC_DEVELOPER,       false, &ChatHandler::HandlePvPCommand,                 "", nullptr},
        { "mmaps",          SEC_DEVELOPER,       false, nullptr,                                        "", mmapsCommandTable },
        { "crash",          SEC_CONSOLE,         true, &ChatHandler::HandleCrashCommand,                "", nullptr},
        { "who",            SEC_OBSERVER,        true, &ChatHandler::HandleWhoCommand,                  "", nullptr },
        { "shellcoin",      SEC_PLAYER,          true, &ChatHandler::HandleShellcoinCommand,            "", nullptr},
        { "removecorpses",  SEC_ADMINISTRATOR,   false, &ChatHandler::HandleRemoveCorpsesCommand,       "", nullptr },
        { "faction",        SEC_DEVELOPER,       false, nullptr,                                        "", factionCommandTable },
#ifdef USE_ANTICHEAT
        { "banprint",       SEC_ADMINISTRATOR,   false, &ChatHandler::HandleAnticheatFingerprintAutobanAddCommand,      "", nullptr },
        { "unbanprint",     SEC_ADMINISTRATOR,   false, &ChatHandler::HandleAnticheatFingerprintAutobanRemoveCommand,      "", nullptr },
#endif
        { "bw",             SEC_MODERATOR,       false, &ChatHandler::HandleWarnCharacterCommand,       "", nullptr },
        { "bi",             SEC_MODERATOR,       false, &ChatHandler::HandleBanInfoAccountCommand,      "", nullptr },
        { "marksuspicious", SEC_MODERATOR,       false, &ChatHandler::HandleMarkSuspiciousCommand,      "", nullptr },
        { "forcejoinchannel", SEC_ADMINISTRATOR, false, &ChatHandler::ForceJoinChannelCommand,          "", nullptr},
        { "toggletrainee",  SEC_DEVELOPER,       false, &ChatHandler::HandleToggleTrainingCommand,      "", nullptr },
        { "toggleinferno",  SEC_DEVELOPER,       false, &ChatHandler::HandleToggleInfernoModeCommand,   "", nullptr },
        { "translate",      SEC_DEVELOPER,       false, &ChatHandler::HandleTranslateCommand,           "", nullptr },
        { "stopapi",        SEC_ADMINISTRATOR,   true,  &ChatHandler::HandleStopApiServerCommand,       "", nullptr},
        { "hcchat",         SEC_PLAYER,          false, &ChatHandler::HandleSetHCChatCommand,           "", nullptr },
        { "toggleillusions", SEC_DEVELOPER,      false, &ChatHandler::HandleToggleIllusionsCommand,     "", nullptr},
        { "tmogdelete",     SEC_DEVELOPER,       false, &ChatHandler::HandleDeleteTransmogCollectionCommand, "", nullptr},
        { "perf",           SEC_ADMINISTRATOR,   true, nullptr,                                         "", perfCommandTable },
        { "blockegg",       SEC_DEVELOPER,       false, &ChatHandler::HandleBlockEggsCommand,           "", nullptr},
        { "cleaninventory", SEC_DEVELOPER,       false, &ChatHandler::HandleCleanInventoryCommand,      "", nullptr},
        { "showborders",    SEC_DEVELOPER,       false, &ChatHandler::HandleShowBordersCommand,         "", nullptr },
        { "queststatuses",  SEC_PLAYER,           false, &ChatHandler::HandleQuestStatusesCommand,       "", nullptr},
        // Bot module commands. .rndbot is SEC_PLAYER so a single human can
        // manage their own random-bot pool without keeping a GM alt logged in;
        // a server operator who wants tighter control can raise it.
        // Match the CMaNGOS AHBot command family: these operations can reload
        // server configuration or rebuild the whole market, so they remain
        // administrator-only.
        // Optional module command scripts register their own diagnostics.

        { nullptr,          0,                   false, nullptr,                                        "", nullptr }
    };

    static std::vector<ChatCommand> scriptCommandTable;
    static size_t loadedCommandScriptCount = 0;
    static bool loaded = false;
    size_t const currentCommandScriptCount = ScriptRegistry<CommandScript>::ScriptPointerList.size();

    if (!loaded || loadedCommandScriptCount != currentCommandScriptCount)
    {
        loaded = true;
        loadedCommandScriptCount = currentCommandScriptCount;
        scriptCommandTable.clear();

        for (uint32 i = 0; commandTable[i].Name != nullptr; ++i)
            scriptCommandTable.push_back(commandTable[i]);

        ScriptRegistry<CommandScript>::ForEach([&](CommandScript* script)
        {
            std::vector<ChatCommand> commands = script->GetCommands();

            for (ChatCommand& command : commands)
            {
                if (command.Name)
                    scriptCommandTable.push_back(command);
            }
        });

        scriptCommandTable.push_back({ nullptr, 0, false, nullptr, "", nullptr });
        FillFullCommandsName(scriptCommandTable.data(), "");
    }

    return scriptCommandTable.data();
}

std::map<uint32 /*Permission Id*/, std::string /*Permission Name*/> ChatHandler::m_rbacPermissionNames;
std::map<uint32 /*Account Id*/, uint32 /*Permission Mask*/> ChatHandler::m_rbacAccountGrantedPermissions;
std::map<uint32 /*Account Id*/, uint32 /*Permission Mask*/> ChatHandler::m_rbacAccountBannedPermissions;

void ChatHandler::LoadRbacPermissions()
{
    std::unique_ptr<QueryResult> result(LoginDatabase.Query("SELECT `id`, `name` FROM `rbac_permissions`"));
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 id = fields[0].GetUInt32();
            std::string name = fields[1].GetCppString();

            if (id >= 32)
            {
                sLog.outErrorDb("RBAC permission ids are stored as an uint32 and cannot exceed id 31!");
                continue;
            }

            if (name.empty())
            {
                sLog.outErrorDb("RBAC permission names should not be empty!");
                continue;
            }

            m_rbacPermissionNames.emplace(std::make_pair(id, name));

        } while (result->NextRow());
    }

    result.reset(LoginDatabase.Query("SELECT `account_id`, `permission_id`, `granted` FROM `rbac_account_permissions`"));
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 accountId = fields[0].GetUInt32();
            uint32 permissionId = fields[1].GetUInt32();
            bool granted = fields[2].GetBool();

            if (m_rbacPermissionNames.find(permissionId) == m_rbacPermissionNames.end())
            {
                sLog.outErrorDb("Unknown RBAC permission id %u assigned to account %u!", permissionId, accountId);
                continue;
            }

            if (granted)
                m_rbacAccountGrantedPermissions[accountId] |= 1 << permissionId;
            else
                m_rbacAccountBannedPermissions[accountId] |= 1 << permissionId;

        } while (result->NextRow());
    }

    ChatCommand* commandTable = getCommandTable();

    result.reset(LoginDatabase.Query("SELECT `command`, `permission_id` FROM `rbac_command_permissions`"));
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            std::string command = fields[0].GetCppString();
            uint32 permissionId = fields[1].GetUInt32();

            if (m_rbacPermissionNames.find(permissionId) == m_rbacPermissionNames.end())
            {
                sLog.outErrorDb("Unknown RBAC permission id %u assigned to command '%s'!", permissionId, command.c_str());
                continue;
            }
            
            SetPermissionMaskForCommandInTable(commandTable, command.c_str(), permissionId);

        } while (result->NextRow());
    }
}

bool ChatHandler::SetPermissionMaskForCommandInTable(ChatCommand* commandTable, const char* text, uint32 permissionId)
{
    std::string fullcommand = text;                         // original `text` can't be used. It content destroyed in command code processing.

    ChatCommand* command = nullptr;
    std::string cmdName;

    ChatCommandSearchResult res = FindCommand(commandTable, text, command, nullptr, &cmdName, true, true);

    switch (res)
    {
        case CHAT_COMMAND_OK:
        {
            command->PermissionMask |= (1 << permissionId);
            return true;
        }
        case CHAT_COMMAND_UNKNOWN_SUBCOMMAND:
        {
            // command have subcommands, but not '' subcommand and then any data in `command` useless for it.
            if (cmdName.empty())
                sLog.outErrorDb("Table `command` have command '%s' that only used with some subcommand selection, it can't have help or overwritten access level, skip.", cmdName.c_str());
            else
                sLog.outErrorDb("Table `command` have unexpected subcommand '%s' in command '%s', skip.", cmdName.c_str(), fullcommand.c_str());
            return false;
        }
        case CHAT_COMMAND_UNKNOWN:
        {
            sLog.outErrorDb("Table `command` have nonexistent command '%s', skip.", cmdName.c_str());
            return false;
        }
    }

    return false;
}

ChatHandler::ChatHandler(WorldSession* session) :
    m_session(session), sentErrorMessage(false)
{
}

ChatHandler::ChatHandler(Player* player) : ChatHandler(player->GetSession()) {}

ChatHandler::~ChatHandler() {}

const char *ChatHandler::GetMangosString(int32 entry) const
{
    return m_session->GetMangosString(entry);
}

const char *ChatHandler::GetOnOffStr(bool value) const
{
    return value ?  GetMangosString(LANG_ON) : GetMangosString(LANG_OFF);
}

uint32 ChatHandler::GetAccountId() const
{
    return m_session->GetAccountId();
}

AccountTypes ChatHandler::GetAccessLevel() const
{
    return m_session->GetSecurity();
}

bool ChatHandler::isAvailable(ChatCommand const& cmd) const
{
    // check security level only for simple  command (without child commands)
    if (GetAccessLevel() >= (AccountTypes)cmd.SecurityLevel)
    {
        auto itrBanned = m_rbacAccountBannedPermissions.find(m_session->GetAccountId());
        if (itrBanned != m_rbacAccountBannedPermissions.end())
        {
            if (cmd.PermissionMask & itrBanned->second)
                return false;
        }

        return true;
    }
    else
    {
        auto itrGranted = m_rbacAccountGrantedPermissions.find(m_session->GetAccountId());
        if (itrGranted != m_rbacAccountGrantedPermissions.end())
        {
            if (cmd.PermissionMask & itrGranted->second)
                return true;
        }

        return false;
    }
}

std::string ChatHandler::GetNameLink() const
{
    return GetNameLink(m_session->GetPlayer());
}

bool ChatHandler::HasLowerSecurity(Player* target, ObjectGuid guid, bool strong)
{
    WorldSession* target_session = nullptr;
    uint32 target_account = 0;

    if (target)
        target_session = target->GetSession();
    else if (guid)
        target_account = sObjectMgr.GetPlayerAccountIdByGUID(guid);

    if (!target_session && !target_account)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return true;
    }

    return HasLowerSecurityAccount(target_session, target_account, strong);
}

bool ChatHandler::HasLowerSecurityAccount(WorldSession* target, uint32 target_account, bool strong)
{
    AccountTypes target_sec;

    // ignore only for non-players for non strong checks (when allow apply command at least to same sec level)
    if (GetAccessLevel() > SEC_PLAYER && !strong && !sWorld.getConfig(CONFIG_BOOL_GM_LOWER_SECURITY))
        return false;

    if (target)
        target_sec = target->GetSecurity();
    else if (target_account)
        target_sec = sAccountMgr.GetSecurity(target_account);
    else
        return true;                                        // caller must report error for (target==nullptr && target_account==0)

    if (GetAccessLevel() < target_sec || (strong && GetAccessLevel() <= target_sec))
    {
        SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
        SetSentErrorMessage(true);
        return true;
    }

    return false;
}

bool ChatHandler::hasStringAbbr(const char* name, const char* part)
{
    // non "" command
    if (*name)
    {
        // "" part from non-"" command
        if (!*part)
            return false;

        for (;;)
        {
            if (!*part)
                return true;
            else if (!*name)
                return false;
            else if (tolower(*name) != tolower(*part))
                return false;
            ++name;
            ++part;
        }
    }
    // allow with any for ""

    return true;
}

void ChatHandler::SendSysMessage(const char *str)
{
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = mangos_strdup(str);
    char* pos = buf;

    while (char* line = LineFromMessage(pos))
    {
        ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, line);
        m_session->SendPacket(&data);
    }

    delete [] buf;
    SetSentErrorMessage(true);
}

void ChatHandler::SendGlobalSysMessage(const char *str)
{
    // Chat output
    WorldPacket data;

    // need copy to prevent corruption by strtok call in LineFromMessage original string
    char* buf = mangos_strdup(str);
    char* pos = buf;

    while (char* line = LineFromMessage(pos))
    {
        ChatHandler::BuildChatPacket(data, CHAT_MSG_SYSTEM, line);
        sWorld.SendGlobalMessage(&data);
    }

    delete [] buf;
}

void ChatHandler::SendSysMessage(int32 entry)
{
    SendSysMessage(GetMangosString(entry));
}

void ChatHandler::PSendSysMessage(int32 entry, ...)
{
    const char *format = GetMangosString(entry);
    va_list ap;
    char str [2048];
    va_start(ap, entry);
    vsnprintf(str, 2048, format, ap);
    va_end(ap);
    SendSysMessage(str);
}

void ChatHandler::PSendSysMessage(const char *format, ...)
{
    va_list ap;
    char str [2048];
    va_start(ap, format);
    vsnprintf(str, 2048, format, ap);
    va_end(ap);
    SendSysMessage(str);
}

std::string ChatHandler::PGetParseString(int32 entry, ...)
{
    const char *format = GetMangosString(entry);
    va_list ap;
    char str [2048];
    va_start(ap, entry);
    vsnprintf(str, 2048, format, ap);
    va_end(ap);
    return std::string(str);
}

void ChatHandler::FillFullCommandsName(ChatCommand* table, std::string prefix)
{
    for (uint32 i = 0; table[i].Name != nullptr; ++i)
    {
        std::string newPrefix = prefix;
        if (!newPrefix.empty())
            newPrefix += " ";

        newPrefix += table[i].Name;
        table[i].FullName = newPrefix;

        if (table[i].ChildCommands)
            FillFullCommandsName(table[i].ChildCommands, newPrefix);

    }
}

void ChatHandler::CheckIntegrity(ChatCommand *table, ChatCommand *parentCommand)
{
    for (uint32 i = 0; table[i].Name != nullptr; ++i)
    {
        ChatCommand* command = &table[i];

        if (parentCommand && command->SecurityLevel < parentCommand->SecurityLevel)
            sLog.outError("Subcommand '%s' of command '%s' have less access level (%hhu) that parent (%hhu)",
                          command->Name, parentCommand->Name, command->SecurityLevel, parentCommand->SecurityLevel);

        if (!parentCommand && strlen(command->Name) == 0)
            sLog.outError("Subcommand '' at top level");

        if (command->ChildCommands)
        {
            if (command->Handler || command->ModuleHandler)
            {
                if (parentCommand)
                    sLog.outError("Subcommand '%s' of command '%s' have handler and subcommands in same time, must be used '' subcommand for handler instead.",
                                  command->Name, parentCommand->Name);
                else
                    sLog.outError("First level command '%s' have handler and subcommands in same time, must be used '' subcommand for handler instead.",
                                  command->Name);
            }

            if (parentCommand && strlen(command->Name) == 0)
                sLog.outError("Subcommand '' of command '%s' have subcommands", parentCommand->Name);

            CheckIntegrity(command->ChildCommands, command);
        }
        else if (!command->Handler && !command->ModuleHandler)
        {
            if (parentCommand)
                sLog.outError("Subcommand '%s' of command '%s' not have handler and subcommands in same time. Must have some from its!",
                              command->Name, parentCommand->Name);
            else
                sLog.outError("First level command '%s' not have handler and subcommands in same time. Must have some from its!",
                              command->Name);
        }
    }
}

/**
 * Search (sub)command for command line available for chat handler access level
 *
 * @param text  Command line string that will parsed for (sub)command search
 *
 * @return Pointer to found command structure or nullptr if appropriate command not found
 */
ChatCommand const* ChatHandler::FindCommand(char const* text)
{
    ChatCommand* command = nullptr;
    char const* textPtr = text;

    return FindCommand(getCommandTable(), textPtr, command) == CHAT_COMMAND_OK ? command : nullptr;
}

/**
 * Search (sub)command for command line available for chat handler access level with options and fail case additional info
 *
 * @param table         Pointer to command C-style array first level command where will be searched
 * @param text          Command line string that will parsed for (sub)command search,
 *                      it modified at return from function and pointed to not parsed tail
 * @param command       At success this is found command, at other cases this is last found parent command
 *                      before subcommand search fail
 * @param parentCommand Output arg for optional return parent command for command arg.
 * @param cmdNamePtr    Output arg for optional return last parsed command name.
 * @param allAvailable  Optional arg (with false default value) control use command access level checks while command search.
 * @param exactlyName   Optional arg (with false default value) control use exactly name in checks while command search.
 *
 * @return one from enum value of ChatCommandSearchResult. Output args return values highly dependent from this return result:
 *
 *      CHAT_COMMAND_OK       - Command found!
 *                              text point to non parsed tail with possible command specific data, command store found command pointer,
 *                              parentCommand have parent of found command or nullptr if command found in table array directly
 *                              cmdNamePtr store found command name in original form from command line
 *      CHAT_COMMAND_UNKNOWN  - Command not found in table directly
 *                              text only skip possible whitespaces,
 *                              command is nullptr
 *                              parentCommand is nullptr
 *                              cmdNamePtr store command name that not found as it extracted from command line
 *      CHAT_COMMAND_UNKNOWN_SUBCOMMAND - Subcommand not found in some deed subcomand lists
 *                              text point to non parsed tail including not found command name in command line,
 *                              command store last found parent command if any
 *                              parentCommand have parent of command in command arg or nullptr
 *                              cmdNamePtr store command name that not found as it extracted from command line
 */
ChatCommandSearchResult ChatHandler::FindCommand(ChatCommand* table, char const* &text, ChatCommand*& command, ChatCommand** parentCommand /*= nullptr*/, std::string* cmdNamePtr /*= nullptr*/, bool allAvailable /*= false*/, bool exactlyName /*= false*/)
{
    std::string cmd;

    // skip whitespaces
    while (*text != ' ' && *text != '\0')
    {
        cmd += *text;
        ++text;
    }

    while (*text == ' ') ++text;

    // search first level command in table
    for (uint32 i = 0; table[i].Name != nullptr; ++i)
    {
        if (exactlyName)
        {
            size_t len = strlen(table[i].Name);
            if (strncmp(table[i].Name, cmd.c_str(), len + 1) != 0)
                continue;
        }
        else
        {
            if (!hasStringAbbr(table[i].Name, cmd.c_str()))
                continue;
        }
        // select subcommand from child commands list
        if (table[i].ChildCommands != nullptr)
        {
            char const* oldchildtext = text;
            ChatCommand* parentSubcommand = nullptr;
            ChatCommandSearchResult res = FindCommand(table[i].ChildCommands, text, command, &parentSubcommand, cmdNamePtr, allAvailable, exactlyName);

            switch (res)
            {
                case CHAT_COMMAND_OK:
                {
                    // if subcommand success search not return parent command, then this parent command is owner of child commands
                    if (parentCommand)
                        *parentCommand = parentSubcommand ? parentSubcommand : &table[i];

                    // Name == "" is special case: restore original command text for next level "" (where parentSubcommand==nullptr)
                    if (strlen(command->Name) == 0 && !parentSubcommand)
                        text = oldchildtext;

                    return CHAT_COMMAND_OK;
                }
                case CHAT_COMMAND_UNKNOWN:
                {
                    // command not found directly in child command list, return child command list owner
                    command = &table[i];
                    if (parentCommand)
                        *parentCommand = nullptr;              // we don't known parent of table list at this point

                    text = oldchildtext;                    // restore text to stated just after parse found parent command
                    return CHAT_COMMAND_UNKNOWN_SUBCOMMAND; // we not found subcommand for table[i]
                }
                case CHAT_COMMAND_UNKNOWN_SUBCOMMAND:
                default:
                {
                    // some deep subcommand not found, if this second level subcommand then parentCommand can be nullptr, use known value for it
                    if (parentCommand)
                        *parentCommand = parentSubcommand ? parentSubcommand : &table[i];
                    return res;
                }
            }
        }

        // must be have handler is explicitly selected
        if (!table[i].Handler && !table[i].ModuleHandler)
            continue;

        // command found directly in to table
        command = &table[i];

        // unknown table owner at this point
        if (parentCommand)
            *parentCommand = nullptr;

        if (cmdNamePtr)
            *cmdNamePtr = cmd;

        return CHAT_COMMAND_OK;
    }

    // command not found in table directly
    command = nullptr;

    // unknown table owner at this point
    if (parentCommand)
        *parentCommand = nullptr;

    if (cmdNamePtr)
        *cmdNamePtr = cmd;

    return CHAT_COMMAND_UNKNOWN;
}

bool IsCommandLogged(std::string& command)
{
    // Bowser said not to log this command
    if (command.find("anticheat hwprint list") != std::string::npos)
        return false;
    return true;
}

/**
 * Execute (sub)command available for chat handler access level with options in command line string
 *
 * @param text  Command line string that will parsed for (sub)command search and command specific data
 *
 * Command output and errors in command execution will send to chat handler.
 */
void ChatHandler::ExecuteCommand(const char* text)
{
    std::string fullcmd = text;                             // original `text` can't be used. It content destroyed in command code processing.

    char const* commandArgs = text;
    std::string commandName;
    while (*commandArgs && *commandArgs != ' ')
        commandName += *commandArgs++;

    while (*commandArgs == ' ')
        ++commandArgs;

    bool handledByScript = ScriptRegistry<AllCommandScript>::ForEachWithReturn([&](AllCommandScript* script)
    {
        return !script->CanExecuteCommand(this, commandName.c_str(), commandArgs);
    });

    if (handledByScript)
        return;

    ChatCommand* command = nullptr;
    ChatCommand* parentCommand = nullptr;

    ChatCommandSearchResult res = FindCommand(getCommandTable(), text, command, &parentCommand);

    switch (res)
    {
        case CHAT_COMMAND_OK:
        {
            if (!isAvailable(*command))
            {
                SendSysMessage(LANG_COMMAND_UNAVAILABLE);
                return;
            }

            std::string realCommandFull = command->FullName;

            if (text[0])
            {
                realCommandFull += " ";
                realCommandFull += text;
            }
            if (m_session && command->Flags & COMMAND_FLAGS_ONLY_ON_SELF)
            {
                ObjectGuid selGuid = m_session->GetPlayer()->GetSelectionGuid();
                if (!selGuid.IsEmpty() && selGuid != m_session->GetPlayer()->GetObjectGuid())
                {
                    PSendSysMessage("|cFF8DE2FFYou can use the command [%s] only on yourself|r", command->FullName.c_str());
                    return;
                }
            }
            SetSentErrorMessage(false);

            // Always log GM commands, regardless of success
            if (command->SecurityLevel > SEC_PLAYER && command->SecurityLevel < SEC_SIGMACHAD && IsCommandLogged(realCommandFull))
            {
                // chat case
                if (m_session && m_session->GetPlayer())
                {
                    Player* p = m_session->GetPlayer();
                    ObjectGuid sel_guid = p->GetSelectionGuid();
                    sLog.outCommand(GetAccountId(), "Command: %s [Player: %s (Group Leader \"%s\", Account: %u) X: %f Y: %f Z: %f O: %f Map: %u Selected: %s]",
                        realCommandFull.c_str(), p->GetName(), p->GetGroup() ? p->GetGroup()->GetLeaderGuid().GetString().c_str() : "NULL", GetAccountId(), p->GetPositionX(), p->GetPositionY(), p->GetPositionZ(), p->GetOrientation(), p->GetMapId(),
                        sel_guid.GetString().c_str());

#ifdef USING_DISCORD_BOT

                    try {
                        std::string message = string_format("Command: {} [Player: {} (Group Leader \"{}\", Account: {}) X: {} Y: {} Z: {} O: {} Map: {} Selected: {}]",
                            realCommandFull.c_str(), p->GetName(), p->GetGroup() ? p->GetGroup()->GetLeaderGuid().GetString().c_str() : "NULL", GetAccountId(), p->GetPositionX(), p->GetPositionY(), p->GetPositionZ(), p->GetOrientation(), p->GetMapId(),
                            sel_guid.GetString().c_str());


                        if (m_session->GetSecurity() == SEC_MODERATOR)
                            sWorld.SendDiscordMessage(1101486865477021726, message);
                        else
                            sWorld.SendDiscordMessage(1075085609737142352, message);


                    }
                    catch ([[maybe_unused]] const std::exception& e) {}
#endif
                }
                else                                        // 0 account -> console
                {
                    sLog.outCommand(GetAccountId(), "Command: %s [Account: %u from %s]",
                        realCommandFull.c_str(), GetAccountId(), GetAccountId() ? "RA-connection" : "Console");
                }
            }

            bool const handled = command->ModuleHandler
                ? command->ModuleHandler(this, (char*)text)
                : (this->*(command->Handler))((char*)text);   // text content destroyed at call
            if (handled)
            {
                if (m_session && command->Flags & COMMAND_FLAGS_CRITICAL)
                {
                    if (Unit* target = GetSelectedUnit())
                        sLog.out(LOG_GM_CRITICAL, "%s: %s. Selected %s. Map %u", m_session->GetUsername().c_str(), realCommandFull.c_str(), target->GetObjectGuid().GetString().c_str(), target->GetMapId());
                    else
                        sLog.out(LOG_GM_CRITICAL, "%s: %s.", m_session->GetUsername().c_str(), realCommandFull.c_str());
                }
            }
            // some commands have custom error messages. Don't send the default one in these cases.
            else if (!HasSentErrorMessage())
            {
                if (!command->Help.empty())
                    SendSysMessage(command->Help.c_str());
                else
                    SendSysMessage(LANG_CMD_SYNTAX);

                if (ChatCommand* showCommand = (strlen(command->Name) == 0 && parentCommand ? parentCommand : command))
                    if (ChatCommand* childs = showCommand->ChildCommands)
                        ShowHelpForSubCommands(childs, showCommand->Name);

                SetSentErrorMessage(true);
            }
            break;
        }
        case CHAT_COMMAND_UNKNOWN_SUBCOMMAND:
        {
            SendSysMessage(LANG_NO_SUBCMD);
            ShowHelpForCommand(command->ChildCommands, text);
            SetSentErrorMessage(true);
            break;
        }
        case CHAT_COMMAND_UNKNOWN:
        {
            SendSysMessage(LANG_NO_CMD);
            SetSentErrorMessage(true);
            break;
        }
    }
}

/**
 * Function find appropriate command and update command security level and help text
 *
 * @param commandTable  Table for first level command search
 * @param text          Command line string that will parsed for (sub)command search
 * @param security      New security level for command
 * @param help          New help text  for command
 *
 * @return true if command has been found, and false in other case
 *
 * All problems found while command search and updated output as to DB errors log
 */
bool ChatHandler::SetDataForCommandInTable(ChatCommand *commandTable, const char* text, uint8 security, std::string const& help, uint8 flags)
{
    std::string fullcommand = text;                         // original `text` can't be used. It content destroyed in command code processing.

    ChatCommand* command = nullptr;
    std::string cmdName;

    ChatCommandSearchResult res = FindCommand(commandTable, text, command, nullptr, &cmdName, true, true);

    switch (res)
    {
        case CHAT_COMMAND_OK:
        {
            if (command->SecurityLevel != security)
                DETAIL_LOG("Table `command` overwrite for command '%s' default security (%hhu) by %hhu",
                           fullcommand.c_str(), command->SecurityLevel, security);

            command->SecurityLevel = security;
            command->Help          = help;
            command->Flags         = flags;
            return true;
        }
        case CHAT_COMMAND_UNKNOWN_SUBCOMMAND:
        {
            // command have subcommands, but not '' subcommand and then any data in `command` useless for it.
            if (cmdName.empty())
                sLog.outErrorDb("Table `command` have command '%s' that only used with some subcommand selection, it can't have help or overwritten access level, skip.", cmdName.c_str());
            else
                sLog.outErrorDb("Table `command` have unexpected subcommand '%s' in command '%s', skip.", cmdName.c_str(), fullcommand.c_str());
            return false;
        }
        case CHAT_COMMAND_UNKNOWN:
        {
            sLog.outErrorDb("Table `command` have nonexistent command '%s', skip.", cmdName.c_str());
            return false;
        }
    }

    return false;
}

bool ChatHandler::ParseCommands(const char* text)
{
    MANGOS_ASSERT(text);
    MANGOS_ASSERT(*text);

    /// chat case (.command or !command format)
    if (m_session)
    {
        if (text[0] != '!' && text[0] != '.')
            return false;

        /// ignore single . and ! in line
        if (strlen(text) < 2)
            return false;

        if (m_session->GetSecurity() == SEC_PLAYER && !sWorld.getConfig(CONFIG_BOOL_PLAYER_COMMANDS))
            return false;
    }

    /// ignore messages staring from many dots.
    if ((text[0] == '.' && text[1] == '.') || (text[0] == '!' && text[1] == '!'))
        return false;

    /// skip first . or ! (in console allowed use command with . and ! and without its)
    if (text[0] == '!' || text[0] == '.')
        ++text;

    ExecuteCommand(text);

    return true;
}

bool ChatHandler::ShowHelpForSubCommands(ChatCommand *table, char const* cmd)
{
    std::string list;
    for (uint32 i = 0; table[i].Name != nullptr; ++i)
    {
        // must be available (ignore handler existence for show command with possible available subcommands
        if (!isAvailable(table[i]))
            continue;

        if (m_session)
            list += "\n    ";
        else
            list += "\n\r    ";

        list += table[i].Name;

        if (table[i].ChildCommands)
            list += " ...";
    }

    if (list.empty())
        return false;

    if (table == getCommandTable())
    {
        SendSysMessage(LANG_AVIABLE_CMD);
        SendSysMessage(list.c_str());
    }
    else
    {
        PSendSysMessage(LANG_SUBCMDS_LIST, cmd);
        SendSysMessage(list.c_str());
    }
    return true;
}

bool ChatHandler::ShowHelpForCommand(ChatCommand *table, const char* cmd)
{
    char const* oldCmd = cmd;
    ChatCommand* command = nullptr;
    ChatCommand* parentCommand = nullptr;

    ChatCommand* showCommand = nullptr;
    ChatCommand* childCommands = nullptr;

    ChatCommandSearchResult res = FindCommand(table, cmd, command, &parentCommand);

    switch (res)
    {
        case CHAT_COMMAND_OK:
        {
            // for "" subcommand use parent command if any for subcommands list output
            if (strlen(command->Name) == 0 && parentCommand)
                showCommand = parentCommand;
            else
                showCommand = command;

            childCommands = showCommand->ChildCommands;
            break;
        }
        case CHAT_COMMAND_UNKNOWN_SUBCOMMAND:
            showCommand = command;
            childCommands = showCommand->ChildCommands;
            break;
        case CHAT_COMMAND_UNKNOWN:
            // not show command list at error in first level command find fail
            childCommands = table != getCommandTable() || strlen(oldCmd) == 0 ? table : nullptr;
            command = nullptr;
            break;
    }

    if (command && !command->Help.empty())
        SendSysMessage(command->Help.c_str());

    if (childCommands)
        if (ShowHelpForSubCommands(childCommands, showCommand ? showCommand->Name : ""))
            return true;

    if (command && command->Help.empty())
        SendSysMessage(LANG_NO_HELP_CMD);

    return command || childCommands;
}

bool ChatHandler::isValidChatMessage(const char* message)
{
    /*

    valid examples:
    |cffa335ee|Hitem:812:0:0:0:0:0:0:0:70|h[Glowing Brightwood Staff]|h|r
    |cff808080|Hquest:2278:47|h[The Platinum Discs]|h|r
    |cff4e96f7|Htalent:2232:-1|h[Taste for Blood]|h|r
    |cff71d5ff|Hspell:21563|h[Command]|h|r
    |cffffd000|Henchant:3919|h[Engineering: Rough Dynamite]|h|r

    | will be escaped to ||
    */

    if (strlen(message) > 255)
        return false;

    const char validSequence[6] = "cHhhr";
    const char* validSequenceIterator = validSequence;

    // more simple checks
    if (sWorld.getConfig(CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_SEVERITY) < 3)
    {
        const std::string validCommands = "cHhr|";

        while (*message)
        {
            // find next pipe command
            message = strchr(message, '|');

            if (!message)
                return true;

            ++message;
            char commandChar = *message;
            if (validCommands.find(commandChar) == std::string::npos)
                return false;

            ++message;
            // validate sequence
            if (sWorld.getConfig(CONFIG_UINT32_CHAT_STRICT_LINK_CHECKING_SEVERITY) == 2)
            {
                if (commandChar == *validSequenceIterator)
                {
                    if (validSequenceIterator == validSequence + 4)
                        validSequenceIterator = validSequence;
                    else
                        ++validSequenceIterator;
                }
                else if (commandChar != '|')
                    return false;
            }
        }
        return true;
    }

    std::istringstream reader(message);
    char buffer[256];

    uint32 color = 0;

    ItemPrototype const* linkedItem = nullptr;
    const SpellEntry* linkedSpell = nullptr;

    std::vector<int> properties;

    while (!reader.eof())
    {
        if (validSequence == validSequenceIterator)
        {
            linkedItem = nullptr;

            reader.ignore(255, '|');
        }
        else if (reader.get() != '|')
        {
            DEBUG_LOG("ChatHandler::isValidChatMessage sequence aborted unexpectedly");
            return false;
        }

        // pipe has always to be followed by at least one char
        if (reader.peek() == '\0')
        {
            DEBUG_LOG("ChatHandler::isValidChatMessage pipe followed by \\0");
            return false;
        }

        // no further pipe commands
        if (reader.eof())
            break;

        char commandChar;
        reader >> std::noskipws >> commandChar;

        // | in normal messages is escaped by ||
        if (commandChar != '|')
        {
            if (commandChar == *validSequenceIterator)
            {
                if (validSequenceIterator == validSequence + 4)
                    validSequenceIterator = validSequence;
                else
                    ++validSequenceIterator;
            }
            else
            {
                DEBUG_LOG("ChatHandler::isValidChatMessage invalid sequence, expected %c but got %c", *validSequenceIterator, commandChar);
                return false;
            }
        }
        else if (validSequence != validSequenceIterator)
        {
            // no escaped pipes in sequences
            DEBUG_LOG("ChatHandler::isValidChatMessage got escaped pipe in sequence");
            return false;
        }

        switch (commandChar)
        {
            case 'c':
                color = 0;
                // validate color, expect 8 hex chars
                for (int i = 0; i < 8; i++)
                {
                    char c;
                    reader >> c;
                    if (!c)
                    {
                        DEBUG_LOG("ChatHandler::isValidChatMessage got \\0 while reading color in |c command");
                        return false;
                    }

                    color <<= 4;
                    // check for hex char
                    if (c >= '0' && c <= '9')
                    {
                        color |= c - '0';
                        continue;
                    }
                    if (c >= 'a' && c <= 'f')
                    {
                        color |= 10 + c - 'a';
                        continue;
                    }
                    DEBUG_LOG("ChatHandler::isValidChatMessage got non hex char '%c' while reading color", c);
                    return false;
                }
                break;
            case 'H':
                // read chars up to colon  = link type
                reader.getline(buffer, 256, ':');
                if (reader.eof())                           // : must be
                    return false;

                if (strcmp(buffer, "item") == 0)
                {
                    // read item entry
                    reader.getline(buffer, 256, ':');
                    if (reader.eof())                       // : must be
                        return false;

                    linkedItem = sObjectMgr.GetItemPrototype(atoi(buffer));
                    if (!linkedItem)
                    {
                        DEBUG_LOG("ChatHandler::isValidChatMessage got invalid itemID %u in |item command", atoi(buffer));
                        return false;
                    }

                    if (color != ItemQualityColors[linkedItem->Quality])
                    {
                        DEBUG_LOG("ChatHandler::isValidChatMessage linked item has color %u, but user claims %u", ItemQualityColors[linkedItem->Quality],
                                  color);
                        return false;
                    }

                    // the itementry is followed by several integers which describe an instance of this item
                    int32 propertyId = 0, propCount = 0;
                    bool negativeNumber = false;
                    char c = 0;
                    while (reader.peek() != '|' && !reader.eof() && propCount < 3)
                    {
                        c = reader.get();

                        // Reset at the property switch
                        if (c == ':')
                        {
                            if (negativeNumber)
                                propertyId *= -1;

                            ++propCount;
                            // Only check if it's a non-zero prop
                            if (propertyId > 0)
                                properties.push_back(propertyId);

                            propertyId = 0;
                            negativeNumber = false;

                            continue;
                        }

                        if (c >= '0' && c <= '9')
                        {
                            propertyId *= 10;
                            propertyId += c - '0';
                        }
                        else if (c == '-')
                            negativeNumber = true;
                        else
                            return false;
                    }

                    // ignore other integers
                    c = reader.peek();
                    while (((c >= '0' && c <= '9') || c == ':') && c != '|' && !reader.eof())
                    {
                        reader.ignore(1);
                        c = reader.peek();
                    }
                }
                else if (strcmp(buffer, "enchant") == 0)
                {
                    if (color != CHAT_LINK_COLOR_ENCHANT)
                        return false;

                    uint32 spellid = 0;
                    // read spell entry
                    char c = reader.peek();
                    while (c >= '0' && c <= '9')
                    {
                        reader.ignore(1);
                        spellid *= 10;
                        spellid += c - '0';
                        c = reader.peek();
                    }
                    linkedSpell = sSpellMgr.GetSpellEntry(spellid);
                    if (!linkedSpell)
                        return false;
                }
                else
                {
                    DEBUG_LOG("ChatHandler::isValidChatMessage user sent unsupported link type '%s'", buffer);
                    return false;
                }
                break;
            case 'h':
                // if h is next element in sequence, this one must contain the linked text :)
                if (*validSequenceIterator == 'h')
                {
                    // links start with '['
                    if (reader.get() != '[')
                    {
                        DEBUG_LOG("ChatHandler::isValidChatMessage link caption doesn't start with '['");
                        return false;
                    }
                    reader.getline(buffer, 256, ']');
                    if (reader.eof())                       // ] must be
                        return false;

                    // verify the link name
                    if (linkedSpell)
                    {
                        bool foundName = false;
                        for (uint8 i = 0; i < MAX_DBC_LOCALE; ++i)
                        {
                            if (!linkedSpell->SpellName[i].empty() && strcmp(linkedSpell->SpellName[i].c_str(), buffer) == 0)
                            {
                                foundName = true;
                                break;
                            }
                        }
                        if (!foundName)
                            return false;
                    }
                    else if (linkedItem)
                    {
                        bool hasValidRandomProperty = false;
                        bool hasRandomProperty = linkedItem->RandomProperty > 0;
                        int enchantCount = 0;

                        // Item has no random property. All properties must be enchants, of which
                        // there can only be two. One permanent, one temporary
                        if (!hasRandomProperty && properties.size() > 2)
                            return false;

                        const ItemRandomPropertiesEntry* iProp = nullptr;
                        for (const auto prop : properties)
                        {
                            iProp = nullptr;

                            if (!hasRandomProperty)
                            {
                                if (sSpellItemEnchantmentStore.LookupEntry(prop))
                                    ++enchantCount;
                                else
                                    return false;
                            }
                            else
                            {
                                // This is tricky - some enchants and random properties share IDs!
                                // do we just compare all of the names?
                                if (!hasValidRandomProperty)
                                    iProp = sItemRandomPropertiesStore.LookupEntry(prop);

                                // Check if we have a valid random property with this prop...
                                if (iProp)
                                {
                                    // Compare the name. If the name isn't correct, maybe it's an enchantment instead.
                                    for (int localeIndex = DB_LOCALE_enUS; localeIndex <= DB_LOCALE_ruRU; ++localeIndex)
                                    {
                                        std::string expectedName = linkedItem->Name1;
                                        Item::GetLocalizedNameWithSuffix(expectedName, linkedItem, iProp, localeIndex, GetDbcLocaleFromDbLocale(DBLocaleConstant(localeIndex)));
                                        if (expectedName == buffer)
                                        {
                                            hasValidRandomProperty = true;
                                            break;
                                        }
                                    }
                                }

                                // Still don't have a valid random prop. If it's an enchantment, inc count.
                                // Else this property is entirely invalid.
                                if (!hasValidRandomProperty)
                                {
                                    if (sSpellItemEnchantmentStore.LookupEntry(prop))
                                        ++enchantCount;
                                    else
                                        return false;
                                }
                            }

                            if (enchantCount > 2)
                                return false;
                        }

                        if (hasRandomProperty && !hasValidRandomProperty)
                            return false;
                        else if (!hasRandomProperty)
                        {
                            // Finally compare base name
                            bool validName = false;
                            for (int localeIndex = DB_LOCALE_enUS; localeIndex <= DB_LOCALE_ruRU; ++localeIndex)
                            {
                                std::string expectedName = linkedItem->Name1;
                                Item::GetLocalizedNameWithSuffix(expectedName, linkedItem, nullptr, localeIndex, GetDbcLocaleFromDbLocale(DBLocaleConstant(localeIndex)));
                                if (expectedName == buffer)
                                {
                                    validName = true;
                                    break;
                                }
                            }
                            if (!validName)
                                return false;
                        }
                    }
                    // that place should never be reached - if nothing linked has been set in |H
                    // it will return false before
                    else
                        return false;
                }
                break;
            case 'r':
            case '|':
                // no further payload
                break;
            default:
                DEBUG_LOG("ChatHandler::isValidChatMessage got invalid command |%c", commandChar);
                return false;
        }
    }

    // check if every opened sequence was also closed properly
    if (validSequence != validSequenceIterator)
        DEBUG_LOG("ChatHandler::isValidChatMessage EOF in active sequence");

    return validSequence == validSequenceIterator;
}

void ChatHandler::BuildChatPacket(WorldPacket& data, ChatMsg msgtype, const std::string& message, Language language /*= LANG_UNIVERSAL*/, uint32 chatTag /*= CHAT_TAG_NONE*/,
                                  ObjectGuid const& senderGuid /*= ObjectGuid()*/, char const* senderName /*= nullptr*/,
                                  ObjectGuid const& targetGuid /*= ObjectGuid()*/, char const* targetName /*= nullptr*/,
                                  char const* channelName /*= nullptr*/, uint8 playerRank /*= 0*/)
{
    std::string messageFinal;
    if (chatTag == CHAT_TAG_GM && language != LANG_ADDON)
    {
        messageFinal = std::string("|c1049e6ff") + message + "|r";
    }
    else
        messageFinal = message;

    data.Initialize(SMSG_MESSAGECHAT);
    data << uint8(msgtype);
    data << uint32(language);

    switch (msgtype)
    {
        case CHAT_MSG_MONSTER_WHISPER:
        case CHAT_MSG_RAID_BOSS_WHISPER:
        case CHAT_MSG_RAID_BOSS_EMOTE:
        case CHAT_MSG_MONSTER_EMOTE:
            MANGOS_ASSERT(senderName);
            data << uint32(strlen(senderName) + 1);
            data << senderName;
            data << ObjectGuid(targetGuid);
            break;

        case CHAT_MSG_SAY:
        case CHAT_MSG_PARTY:
        case CHAT_MSG_YELL:
            data << ObjectGuid(senderGuid);
            data << ObjectGuid(senderGuid);
            break;

        case CHAT_MSG_MONSTER_SAY:
        case CHAT_MSG_MONSTER_YELL:
            MANGOS_ASSERT(senderName);
            data << ObjectGuid(senderGuid);
            data << uint32(strlen(senderName) + 1);
            data << senderName;
            data << ObjectGuid(targetGuid);
            break;

        case CHAT_MSG_CHANNEL:
            MANGOS_ASSERT(channelName);
            data << channelName;
            data << uint32(playerRank);
            data << ObjectGuid(senderGuid);
            break;

        default:
            data << ObjectGuid(senderGuid);
            break;
    }

    if (messageFinal.empty())
        return;

    data << uint32(messageFinal.length() + 1);
    data << messageFinal;
    data << uint8(chatTag);
}

Player * ChatHandler::GetSelectedPlayer()
{
    if (!m_session)
        return nullptr;

    ObjectGuid guid  = m_session->GetPlayer()->GetSelectionGuid();

    if (!guid)
        return m_session->GetPlayer();

    return sObjectMgr.GetPlayer(guid);
}

Unit* ChatHandler::GetSelectedUnit()
{
    if (!m_session)
        return nullptr;

    ObjectGuid guid = m_session->GetPlayer()->GetSelectionGuid();

    if (!guid)
        return m_session->GetPlayer();

    // can be selected player at another map
    return ObjectAccessor::GetUnit(*m_session->GetPlayer(), guid);
}

Creature* ChatHandler::GetSelectedCreature()
{
    if (!m_session)
        return nullptr;

    return m_session->GetPlayer()->GetMap()->GetAnyTypeCreature(m_session->GetPlayer()->GetSelectionGuid());
}

Pet* ChatHandler::GetSelectedPet()
{
    if (!m_session)
        return nullptr;

    return m_session->GetPlayer()->GetMap()->GetPet(m_session->GetPlayer()->GetSelectionGuid());
}

/**
 * Function skip all whitespaces in args string
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 *             allowed nullptr string pointer stored in *args
 */
void ChatHandler::SkipWhiteSpaces(char** args)
{
    if (!*args)
        return;

    while (isWhiteSpace(**args))
        ++(*args);
}

/**
 * Function extract to val arg signed integer value or fail
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param val  return extracted value if function success, in fail case original value unmodified
 * @return     true if value extraction successful
 */
bool  ChatHandler::ExtractInt32(char** args, int32& val)
{
    if (!*args || !**args)
        return false;

    char* tail = *args;

    long valRaw = strtol(*args, &tail, 10);

    if (tail != *args && isWhiteSpace(*tail))
        *(tail++) = '\0';
    else if (tail && *tail)                                 // some not whitespace symbol
        return false;                                       // args not modified and can be re-parsed

    if (valRaw < std::numeric_limits<int32>::min() || valRaw > std::numeric_limits<int32>::max())
        return false;

    // value successfully extracted
    val = int32(valRaw);
    *args = tail;
    return true;
}

/**
 * Function extract to val arg optional signed integer value or use default value. Fail if extracted not signed integer.
 *
 * @param args    variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param val     return extracted value if function success, in fail case original value unmodified
 * @param defVal  default value used if no data for extraction in args
 * @return        true if value extraction successful
 */
bool  ChatHandler::ExtractOptInt32(char** args, int32& val, int32 defVal)
{
    if (!*args || !**args)
    {
        val = defVal;
        return true;
    }

    return ExtractInt32(args, val);
}

/**
 * Function extract to val arg unsigned integer value or fail
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param val  return extracted value if function success, in fail case original value unmodified
 * @param base set used base for extracted value format (10 for decimal, 16 for hex, etc), 0 let auto select by system internal function
 * @return     true if value extraction successful
 */
bool  ChatHandler::ExtractUInt32Base(char** args, uint32& val, uint32 base)
{
    if (!*args || !**args)
        return false;

    char* tail = *args;

    unsigned long valRaw = strtoul(*args, &tail, base);

    if (tail != *args && isWhiteSpace(*tail))
        *(tail++) = '\0';
    else if (tail && *tail)                                 // some not whitespace symbol
        return false;                                       // args not modified and can be re-parsed

    if (valRaw > std::numeric_limits<uint32>::max())
        return false;

    // value successfully extracted
    val = uint32(valRaw);
    *args = tail;

    SkipWhiteSpaces(args);
    return true;
}

/**
 * Function extract to val arg optional unsigned integer value or use default value. Fail if extracted not unsigned integer.
 *
 * @param args    variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param val     return extracted value if function success, in fail case original value unmodified
 * @param defVal  default value used if no data for extraction in args
 * @return        true if value extraction successful
 */
bool  ChatHandler::ExtractOptUInt32(char** args, uint32& val, uint32 defVal)
{
    if (!*args || !**args)
    {
        val = defVal;
        return true;
    }

    return ExtractUInt32(args, val);
}

/**
 * Function extract to val arg float value or fail
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param val  return extracted value if function success, in fail case original value unmodified
 * @return     true if value extraction successful
 */
bool  ChatHandler::ExtractFloat(char** args, float& val)
{
    if (!*args || !**args)
        return false;

    char* tail = *args;

    double valRaw = strtod(*args, &tail);

    if (tail != *args && isWhiteSpace(*tail))
        *(tail++) = '\0';
    else if (tail && *tail)                                 // some not whitespace symbol
        return false;                                       // args not modified and can be re-parsed

    // value successfully extracted
    val = float(valRaw);
    *args = tail;

    SkipWhiteSpaces(args);
    return true;
}

/**
 * Function extract to val arg optional float value or use default value. Fail if extracted not float.
 *
 * @param args    variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param val     return extracted value if function success, in fail case original value unmodified
 * @param defVal  default value used if no data for extraction in args
 * @return        true if value extraction successful
 */
bool  ChatHandler::ExtractOptFloat(char** args, float& val, float defVal)
{
    if (!*args || !**args)
    {
        val = defVal;
        return true;
    }

    return ExtractFloat(args, val);
}

/**
 * Function extract name-like string (from non-numeric or special symbol until whitespace)
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param lit  optional explicit literal requirement. function fail if literal is not starting substring of lit.
 *             Note: function in same way fail if no any literal or literal not fit in this case. Need additional check for select specific fail case
 * @return     name/number-like string without whitespaces, or nullptr if args empty or not appropriate content.
 */
char* ChatHandler::ExtractLiteralArg(char** args, char const* lit /*= nullptr*/)
{
    if (!*args || !**args)
        return nullptr;

    char* head = *args;

    // reject quoted string or link (|-started text)
    switch (head[0])
    {
        // reject quoted string
        case '[':
        case '\'':
        case '"':
            return nullptr;
        // reject link (|-started text)
        case '|':
            // client replace all | by || in raw text
            if (head[1] != '|')
                return nullptr;
            ++head;                                         // skip one |
            break;
        default:
            break;
    }

    if (lit)
    {
        int l = strlen(lit);
        int diff = strncmp(head, lit, l);

        if (diff != 0)
            return nullptr;

        if (head[l] && !isWhiteSpace(head[l]))
            return nullptr;

        char* arg = head;

        if (head[l])
        {
            head[l] = '\0';

            head += l + 1;

            *args = head;
        }
        else
            *args = head + l;

        SkipWhiteSpaces(args);
        return arg;
    }

    char* name = strtok(head, " ");

    char* tail = strtok(nullptr, "");

    *args = tail ? tail : (char*)"";                        // *args don't must be nullptr

    SkipWhiteSpaces(args);

    return name;
}

/**
 * Function extract quote-like string (any characters guarded by some special character, in our cases ['")
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param asis control save quote string wrappers
 * @return     quote-like string, or nullptr if args empty or not appropriate content.
 */
char* ChatHandler::ExtractQuotedArg(char** args, bool asis /*= false*/)
{
    if (!*args || !**args)
        return nullptr;

    if (**args != '\'' && **args != '"' && **args != '[')
        return nullptr;

    char guard = (*args)[0];

    if (guard == '[')
        guard = ']';

    char* tail = (*args) + 1;                               // start scan after first quote symbol
    char* head = asis ? *args : tail;                       // start arg

    while (*tail && *tail != guard)
        ++tail;

    if (!*tail || (tail[1] && !isWhiteSpace(tail[1])))      // fail
        return nullptr;

    if (!tail[1])                                           // quote is last char in string
    {
        if (!asis)
            *tail = '\0';
    }
    else                                                    // quote isn't last char
    {
        if (asis)
            ++tail;

        *tail = '\0';
    }

    *args = tail + 1;

    SkipWhiteSpaces(args);

    return head;
}

/**
 * Function extract quote-like string or literal if quote not detected
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param asis control save quote string wrappers
 * @return     quote/literal string, or nullptr if args empty or not appropriate content.
 */
char* ChatHandler::ExtractQuotedOrLiteralArg(char** args, bool asis /*= false*/)
{
    char *arg = ExtractQuotedArg(args, asis);
    if (!arg)
        arg = ExtractLiteralArg(args);
    return arg;
}

/**
 * Function extract on/off literals as boolean values
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param val  return extracted value if function success, in fail case original value unmodified
 * @return     true at success
 */
bool  ChatHandler::ExtractOnOff(char** args, bool& value)
{
    char* arg = ExtractLiteralArg(args);
    if (!arg)
        return false;

    if (strncmp(arg, "on", 3) == 0 || strncmp(arg, "ON", 3) == 0)
        value = true;
    else if (strncmp(arg, "off", 4) == 0 || strncmp(arg, "OFF", 4) == 0)
        value = false;
    else
        return false;

    return true;
}

/**
 * Function extract shift-link-like string (any characters guarded by | and |h|r with some additional internal structure check)
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 *
 * @param linkTypes  optional nullptr-terminated array of link types, shift-link must fit one from link type from array if provided or extraction fail
 *
 * @param found_idx  if not nullptr then at return index in linkTypes that fit shift-link type, if extraction fail then non modified
 *
 * @param keyPair    if not nullptr then pointer to 2-elements array for return start and end pointer for found key
 *                   if extraction fail then non modified
 *
 * @param somethingPair then pointer to 2-elements array for return start and end pointer if found.
 *                   if not nullptr then shift-link must have data field, if extraction fail then non modified
 *
 * @return     shift-link-like string, or nullptr if args empty or not appropriate content.
 */
char* ChatHandler::ExtractLinkArg(char** args, char const* const* linkTypes /*= nullptr*/, int* foundIdx /*= nullptr*/, char** keyPair /*= nullptr*/, char** somethingPair /*= nullptr*/)
{
    if (!*args || !**args)
        return nullptr;

    // skip if not linked started or encoded single | (doubled by client)
    if ((*args)[0] != '|' || (*args)[1] == '|')
        return nullptr;

    // |color|Hlinktype:key:data...|h[name]|h|r

    char* head = *args;

    // [name] Shift-click form |color|linkType:key|h[name]|h|r
    // or
    // [name] Shift-click form |color|linkType:key:something1:...:somethingN|h[name]|h|r
    // or
    // [name] Shift-click form |linkType:key|h[name]|h|r

    // |color|Hlinktype:key:data...|h[name]|h|r

    char* tail = (*args) + 1;                               // skip |

    if (*tail != 'H')                                       // skip color part, some links can not have color part
    {
        while (*tail && *tail != '|')
            ++tail;

        if (!*tail)
            return nullptr;

        // |Hlinktype:key:data...|h[name]|h|r

        ++tail;                                             // skip |
    }

    // Hlinktype:key:data...|h[name]|h|r

    if (*tail != 'H')
        return nullptr;

    int linktype_idx = 0;

    if (linkTypes)                                          // check link type if provided
    {
        // check linktypes (its include H in name)
        for (; linkTypes[linktype_idx]; ++linktype_idx)
        {
            // exactly string with follow : or |
            int l = strlen(linkTypes[linktype_idx]);
            if (strncmp(tail, linkTypes[linktype_idx], l) == 0 &&
                    (tail[l] == ':' || tail[l] == '|'))
                break;
        }

        // is search fail?
        if (!linkTypes[linktype_idx])                       // nullptr terminator in last element
            return nullptr;

        tail += strlen(linkTypes[linktype_idx]);            // skip linktype string

        // :key:data...|h[name]|h|r

        if (*tail != ':')
            return nullptr;
    }
    else
    {
        while (*tail && *tail != ':')                       // skip linktype string
            ++tail;

        if (!*tail)
            return nullptr;
    }

    ++tail;

    // key:data...|h[name]|h|r
    char* keyStart = tail;                                  // remember key start for return
    char* keyEnd   = tail;                                  // key end for truncate, will updated

    while (*tail && *tail != '|' && *tail != ':')
        ++tail;

    if (!*tail)
        return nullptr;

    keyEnd = tail;                                          // remember key end for truncate

    // |h[name]|h|r or :something...|h[name]|h|r

    char* somethingStart = tail + 1;
    char* somethingEnd   = tail + 1;                        // will updated later if need

    if (*tail == ':' && somethingPair)                      // optional data extraction
    {
        // :something...|h[name]|h|r

        ++tail;

        // something|h[name]|h|r or something:something2...|h[name]|h|r

        while (*tail && *tail != '|' && *tail != ':')
            ++tail;

        if (!*tail)
            return nullptr;

        somethingEnd = tail;                                // remember data end for truncate
    }

    // |h[name]|h|r or :something2...|h[name]|h|r

    while (*tail && (*tail != '|' || *(tail + 1) != 'h'))   // skip ... part if exist
        ++tail;

    if (!*tail)
        return nullptr;

    // |h[name]|h|r

    tail += 2;                                              // skip |h

    // [name]|h|r
    if (!*tail || *tail != '[')
        return nullptr;

    while (*tail && (*tail != ']' || *(tail + 1) != '|'))   // skip name part
        ++tail;

    tail += 2;                                              // skip ]|

    // h|r
    if (!*tail || *tail != 'h'  || *(tail + 1) != '|')
        return nullptr;

    tail += 2;                                              // skip h|

    // r
    if (!*tail || *tail != 'r' || (*(tail + 1) && !isWhiteSpace(*(tail + 1))))
        return nullptr;

    ++tail;                                                 // skip r

    // success

    if (*tail)                                              // truncate all link string
        *(tail++) = '\0';

    if (foundIdx)
        *foundIdx = linktype_idx;

    if (keyPair)
    {
        keyPair[0] = keyStart;
        keyPair[1] = keyEnd;
    }

    if (somethingPair)
    {
        somethingPair[0] = somethingStart;
        somethingPair[1] = somethingEnd;
    }

    *args = tail;

    SkipWhiteSpaces(args);

    return head;
}

/**
 * Function extract name/number/quote/shift-link-like string
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 * @param asis control save quote string wrappers
 * @return     extracted arg string, or nullptr if args empty or not appropriate content.
 */
char* ChatHandler::ExtractArg(char** args, bool asis /*= false*/)
{
    if (!*args || !**args)
        return nullptr;

    char* arg = ExtractQuotedOrLiteralArg(args, asis);
    if (!arg)
        arg = ExtractLinkArg(args);

    return arg;
}

/**
 * Function extract name/quote/number/shift-link-like string, and return it if args have more non-whitespace data
 *
 * @param args variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 *             if args have only single arg then args still pointing to this arg (unmodified pointer)
 * @return     extracted string, or nullptr if args empty or not appropriate content or have single arg totally.
 */
char* ChatHandler::ExtractOptNotLastArg(char** args)
{
    char* arg = ExtractArg(args, true);

    // have more data
    if (*args && **args)
        return arg;

    // optional name not found
    *args = arg ? arg : (char*)"";                          // *args don't must be nullptr

    return nullptr;
}

/**
 * Function extract data from shift-link "|color|LINKTYPE:RETURN:SOMETHING1|h[name]|h|r if linkType == LINKTYPE
 * It also extract literal/quote if not shift-link in args
 *
 * @param args       variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 *                   if args have sift link with linkType != LINKTYPE then args still pointing to this arg (unmodified pointer)
 *
 * @param linkType   shift-link must fit by link type to this arg value or extraction fail
 *
 * @param something1 if not nullptr then shift-link must have data field and it returned into this arg
 *                   if extraction fail then non modified
 *
 * @return           extracted key, or nullptr if args empty or not appropriate content or not fit to linkType.
 */
char* ChatHandler::ExtractKeyFromLink(char** text, char const* linkType, char** something1 /*= nullptr*/)
{
    char const* linkTypes[2];
    linkTypes[0] = linkType;
    linkTypes[1] = nullptr;

    int foundIdx;

    return ExtractKeyFromLink(text, linkTypes, &foundIdx, something1);
}

/**
 * Function extract data from shift-link "|color|LINKTYPE:RETURN:SOMETHING1|h[name]|h|r if LINKTYPE in linkTypes array
 * It also extract literal/quote if not shift-link in args
 *
 * @param args       variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 *                   if args have sift link with linkType != LINKTYPE then args still pointing to this arg (unmodified pointer)
 *
 * @param linkTypes  nullptr-terminated array of link types, shift-link must fit one from link type from array or extraction fail
 *
 * @param found_idx  if not nullptr then at return index in linkTypes that fit shift-link type, for non-link case return -1
 *                   if extraction fail then non modified
 *
 * @param something1 if not nullptr then shift-link must have data field and it returned into this arg
 *                   if extraction fail then non modified
 *
 * @return           extracted key, or nullptr if args empty or not appropriate content or not fit to linkType.
 */
char* ChatHandler::ExtractKeyFromLink(char** text, char const* const* linkTypes, int* found_idx, char** something1 /*= nullptr*/)
{
    // skip empty
    if (!*text || !**text)
        return nullptr;

    // return non link case
    char* arg = ExtractQuotedOrLiteralArg(text);
    if (arg)
    {
        if (found_idx)
            *found_idx = -1;                                // special index case

        return arg;
    }

    char* keyPair[2];
    char* somethingPair[2];

    arg = ExtractLinkArg(text, linkTypes, found_idx, keyPair, something1 ? somethingPair : nullptr);
    if (!arg)
        return nullptr;

    *keyPair[1] = '\0';                                     // truncate key string

    if (something1)
    {
        *somethingPair[1] = '\0';                           // truncate data string
        *something1 = somethingPair[0];
    }

    return keyPair[0];
}

/**
 * Function extract uint32 key from shift-link "|color|LINKTYPE:RETURN|h[name]|h|r if linkType == LINKTYPE
 * It also extract direct number if not shift-link in args
 *
 * @param args       variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 *                   if args have sift link with linkType != LINKTYPE then args still pointing to this arg (unmodified pointer)
 *
 * @param linkType   shift-link must fit by link type to this arg value or extraction fail
 *
 * @param value      store result value at success return, not modified at fail
 *
 * @return           true if extraction succesful
 */
bool ChatHandler::ExtractUint32KeyFromLink(char** text, char const* linkType, uint32& value)
{
    char* arg = ExtractKeyFromLink(text, linkType);
    if (!arg)
        return false;

    return ExtractUInt32(&arg, value);
}

GameObject* ChatHandler::GetGameObjectWithGuid(uint32 lowguid, uint32 entry)
{
    if (!m_session)
        return nullptr;

    Player* pl = m_session->GetPlayer();

    return pl->GetMap()->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, entry, lowguid));
}

Player* ChatHandler::GetPlayer()
{
    return m_session->GetPlayer();
}

GameObject* ChatHandler::GetGameObjectWithGuidGlobal(uint32 lowguid, const GameObjectData* data) const
{
    if (!m_session)
        return nullptr;

    const auto player = m_session->GetPlayer();
    const auto playerMap = player->GetMap();

    const auto goMap = sMapMgr.FindMap(data->position.mapId);

    const auto usedMap = goMap ? goMap : playerMap;

    return usedMap->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, data->id, lowguid));
}

enum SpellLinkType
{
    SPELL_LINK_RAW     = -1,                                // non-link case
    SPELL_LINK_SPELL   = 0,
    SPELL_LINK_TALENT  = 1,
    SPELL_LINK_ENCHANT = 2,
};

static char const* const spellKeys[] =
{
    "Hspell",                                               // normal spell
    "Htalent",                                              // talent spell
    "Henchant",                                             // enchanting recipe spell
    nullptr
};

uint32 ChatHandler::ExtractSpellIdFromLink(char** text)
{
    // number or [name] Shift-click form |color|Henchant:recipe_spell_id|h[prof_name: recipe_name]|h|r
    // number or [name] Shift-click form |color|Hspell:spell_id|h[name]|h|r
    // number or [name] Shift-click form |color|Htalent:talent_id,rank|h[name]|h|r
    int type;
    char* param1_str = nullptr;
    char* idS = ExtractKeyFromLink(text, spellKeys, &type, &param1_str);
    if (!idS)
        return 0;

    uint32 id;
    if (!ExtractUInt32(&idS, id))
        return 0;

    switch (type)
    {
        case SPELL_LINK_RAW:
        case SPELL_LINK_SPELL:
        case SPELL_LINK_ENCHANT:
            return id;
        case SPELL_LINK_TALENT:
        {
            // talent
            TalentEntry const* talentEntry = sTalentStore.LookupEntry(id);
            if (!talentEntry)
                return 0;

            int32 rank;
            if (!ExtractInt32(&param1_str, rank))
                return 0;

            if (rank < 0)                                   // unlearned talent have in shift-link field -1 as rank
                rank = 0;

            return rank < MAX_TALENT_RANK ? talentEntry->RankID[rank] : 0;
        }
    }

    // unknown type?
    return 0;
}

GameTele const* ChatHandler::ExtractGameTeleFromLink(char** text)
{
    // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
    char* cId = ExtractKeyFromLink(text, "Htele");
    if (!cId)
        return nullptr;

    // id case (explicit or from shift link)
    uint32 id;
    if (ExtractUInt32(&cId, id))
        return sObjectMgr.GetGameTele(id);
    else
        return sObjectMgr.GetGameTele(cId);
}

enum GuidLinkType
{
    GUID_LINK_RAW        = -1,                              // non-link case
    GUID_LINK_PLAYER     = 0,
    GUID_LINK_CREATURE   = 1,
    GUID_LINK_GAMEOBJECT = 2,
};

static char const* const guidKeys[] =
{
    "Hplayer",
    "Hcreature",
    "Hgameobject",
    nullptr
};

ObjectGuid ChatHandler::ExtractGuidFromLink(char** text)
{
    int type = 0;

    // |color|Hcreature:creature_guid|h[name]|h|r
    // |color|Hgameobject:go_guid|h[name]|h|r
    // |color|Hplayer:name|h[name]|h|r
    char* idS = ExtractKeyFromLink(text, guidKeys, &type);
    if (!idS)
        return ObjectGuid();

    switch (type)
    {
        case GUID_LINK_RAW:
        case GUID_LINK_PLAYER:
        {
            std::string name = idS;
            if (!normalizePlayerName(name))
                return ObjectGuid();

            if (Player* player = sObjectMgr.GetPlayer(name.c_str()))
                return player->GetObjectGuid();

            return sObjectMgr.GetPlayerGuidByName(name);
        }
        case GUID_LINK_CREATURE:
        {
            uint32 lowguid;
            if (!ExtractUInt32(&idS, lowguid))
                return ObjectGuid();

            if (CreatureData const* data = sObjectMgr.GetCreatureData(lowguid))
                return data->GetObjectGuid(lowguid);
            else
                return ObjectGuid();
        }
        case GUID_LINK_GAMEOBJECT:
        {
            uint32 lowguid;
            if (!ExtractUInt32(&idS, lowguid))
                return ObjectGuid();

            if (GameObjectData const* data = sObjectMgr.GetGOData(lowguid))
                return ObjectGuid(HIGHGUID_GAMEOBJECT, data->id, lowguid);
            else
                return ObjectGuid();
        }
    }

    // unknown type?
    return ObjectGuid();
}

enum LocationLinkType
{
    LOCATION_LINK_RAW               = -1,                   // non-link case
    LOCATION_LINK_PLAYER            = 0,
    LOCATION_LINK_TELE              = 1,
    LOCATION_LINK_TAXINODE          = 2,
    LOCATION_LINK_CREATURE          = 3,
    LOCATION_LINK_GAMEOBJECT        = 4,
    LOCATION_LINK_CREATURE_ENTRY    = 5,
    LOCATION_LINK_GAMEOBJECT_ENTRY  = 6,
    LOCATION_LINK_AREATRIGGER       = 7,
    LOCATION_LINK_AREATRIGGER_TARGET = 8,
};

static char const* const locationKeys[] =
{
    "Htele",
    "Htaxinode",
    "Hplayer",
    "Hcreature",
    "Hgameobject",
    "Hcreature_entry",
    "Hgameobject_entry",
    "Hareatrigger",
    "Hareatrigger_target",
    nullptr
};

bool ChatHandler::ExtractLocationFromLink(char** text, uint32& mapid, float& x, float& y, float& z)
{
    int type = 0;

    // |color|Hplayer:name|h[name]|h|r
    // |color|Htele:id|h[name]|h|r
    // |color|Htaxinode:id|h[name]|h|r
    // |color|Hcreature:creature_guid|h[name]|h|r
    // |color|Hgameobject:go_guid|h[name]|h|r
    // |color|Hcreature_entry:creature_id|h[name]|h|r
    // |color|Hgameobject_entry:go_id|h[name]|h|r
    // |color|Hareatrigger:id|h[name]|h|r
    // |color|Hareatrigger_target:id|h[name]|h|r
    char* idS = ExtractKeyFromLink(text, locationKeys, &type);
    if (!idS)
        return false;

    switch (type)
    {
        case LOCATION_LINK_RAW:
        case LOCATION_LINK_PLAYER:
        {
            std::string name = idS;
            if (!normalizePlayerName(name))
                return false;

            if (Player* player = sObjectMgr.GetPlayer(name.c_str()))
            {
                mapid = player->GetMapId();
                x = player->GetPositionX();
                y = player->GetPositionY();
                z = player->GetPositionZ();
                return true;
            }

            if (ObjectGuid guid = sObjectMgr.GetPlayerGuidByName(name))
            {
                // to point where player stay (if loaded)
                float o;
                bool in_flight;
                return Player::LoadPositionFromDB(guid, mapid, x, y, z, o, in_flight);
            }

            return false;
        }
        case LOCATION_LINK_TELE:
        {
            uint32 id;
            if (!ExtractUInt32(&idS, id))
                return false;

            GameTele const* tele = sObjectMgr.GetGameTele(id);
            if (!tele)
                return false;
            mapid = tele->mapId;
            x = tele->x;
            y = tele->y;
            z = tele->z;
            return true;
        }
        case LOCATION_LINK_TAXINODE:
        {
            uint32 id;
            if (!ExtractUInt32(&idS, id))
                return false;

            TaxiNodesEntry const* node = sObjectMgr.GetTaxiNodeEntry(id);
            if (!node)
                return false;
            mapid = node->map_id;
            x = node->x;
            y = node->y;
            z = node->z;
            return true;
        }
        case LOCATION_LINK_CREATURE:
        {
            uint32 lowguid;
            if (!ExtractUInt32(&idS, lowguid))
                return false;

            if (CreatureData const* data = sObjectMgr.GetCreatureData(lowguid))
            {
                mapid = data->position.mapId;
                x = data->position.x;
                y = data->position.y;
                z = data->position.z;
                return true;
            }
            else
                return false;
        }
        case LOCATION_LINK_GAMEOBJECT:
        {
            uint32 lowguid;
            if (!ExtractUInt32(&idS, lowguid))
                return false;

            if (GameObjectData const* data = sObjectMgr.GetGOData(lowguid))
            {
                mapid = data->position.mapId;
                x = data->position.x;
                y = data->position.y;
                z = data->position.z;
                return true;
            }
            else
                return false;
        }
        case LOCATION_LINK_CREATURE_ENTRY:
        {
            uint32 id;
            if (!ExtractUInt32(&idS, id))
                return false;

            if (sObjectMgr.GetCreatureTemplate(id))
            {
                FindCreatureData worker(id, m_session ? m_session->GetPlayer() : nullptr);

                sObjectMgr.DoCreatureData(worker);

                if (CreatureDataPair const* dataPair = worker.GetResult())
                {
                    mapid = dataPair->second.position.mapId;
                    x = dataPair->second.position.x;
                    y = dataPair->second.position.y;
                    z = dataPair->second.position.z;
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
        case LOCATION_LINK_GAMEOBJECT_ENTRY:
        {
            uint32 id;
            if (!ExtractUInt32(&idS, id))
                return false;

            if (sObjectMgr.GetGameObjectInfo(id))
            {
                FindGOData worker(id, m_session ? m_session->GetPlayer() : nullptr);

                sObjectMgr.DoGOData(worker);

                if (GameObjectDataPair const* dataPair = worker.GetResult())
                {
                    mapid = dataPair->second.position.mapId;
                    x = dataPair->second.position.x;
                    y = dataPair->second.position.y;
                    z = dataPair->second.position.z;
                    return true;
                }
                else
                    return false;
            }
            else
                return false;
        }
        case LOCATION_LINK_AREATRIGGER:
        {
            uint32 id;
            if (!ExtractUInt32(&idS, id))
                return false;

            AreaTriggerEntry const* atEntry = sObjectMgr.GetAreaTrigger(id);
            if (!atEntry)
            {
                PSendSysMessage(LANG_COMMAND_GOAREATRNOTFOUND, id);
                SetSentErrorMessage(true);
                return false;
            }

            mapid = atEntry->mapid;
            x = atEntry->x;
            y = atEntry->y;
            z = atEntry->z;
            return true;
        }
        case LOCATION_LINK_AREATRIGGER_TARGET:
        {
            uint32 id;
            if (!ExtractUInt32(&idS, id))
                return false;

            if (!sObjectMgr.GetAreaTrigger(id))
            {
                PSendSysMessage(LANG_COMMAND_GOAREATRNOTFOUND, id);
                SetSentErrorMessage(true);
                return false;
            }

            AreaTriggerTeleport const* at = sObjectMgr.GetAreaTriggerTeleport(id);
            if (!at)
            {
                PSendSysMessage(LANG_AREATRIGER_NOT_HAS_TARGET, id);
                SetSentErrorMessage(true);
                return false;
            }

            mapid = at->destination.mapId;
            x = at->destination.x;
            y = at->destination.y;
            z = at->destination.z;
            return true;
        }
    }

    // unknown type?
    return false;
}

std::string ChatHandler::ExtractPlayerNameFromLink(char** text)
{
    // |color|Hplayer:name|h[name]|h|r
    char* name_str = ExtractKeyFromLink(text, "Hplayer");
    if (!name_str)
        return "";

    std::string name = name_str;
    if (!normalizePlayerName(name))
        return "";

    return name;
}

/**
 * Function extract at least one from request player data (pointer/guid/name) from args name/shift-link or selected player if no args
 *
 * @param args        variable pointer to non parsed args string, updated at function call to new position (with skipped white spaces)
 *
 * @param player      optional arg   One from 3 optional args must be provided at least (or more).
 * @param player_guid optional arg   For function success only one from provided args need get result
 * @param player_name optional arg   But if early arg get value then all later args will have its (if requested)
 *                                   if player_guid requested and not found then name also will not found
 *                                   So at success can be returned 2 cases: (player/guid/name) or (guid/name)
 *
 * @return           true if extraction successful
 */
bool ChatHandler::ExtractPlayerTarget(char** args, Player** player /*= nullptr*/, ObjectGuid* player_guid /*= nullptr*/, std::string* player_name /*= nullptr*/, bool use_extended_response)
{
    std::string name;
    if (*args && **args)
    {
        name = ExtractPlayerNameFromLink(args);
        if (name.empty())
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
            SetSentErrorMessage(true);
            return false;
        }

        Player* pl = sObjectMgr.GetPlayer(name.c_str());

        // if allowed player pointer
        if (player)
            *player = pl;

        // if need guid value from DB (in name case for check player existence)
        ObjectGuid guid = !pl && (player_guid || player_name) ? sObjectMgr.GetPlayerGuidByName(name) : ObjectGuid();

        // if allowed player guid (if no then only online players allowed)
        if (player_guid)
            *player_guid = pl ? pl->GetObjectGuid() : guid;

        if (player_name)
            *player_name = pl || guid ? name : "";
    }
    else
    {
        Player* pl = GetSelectedPlayer();
        // if allowed player pointer
        if (player)
            *player = pl;
        // if allowed player guid (if no then only online players allowed)
        if (player_guid)
            *player_guid = pl ? pl->GetObjectGuid() : ObjectGuid();

        if (player_name)
            *player_name = pl ? pl->GetName() : "";
    }

    // some from req. data must be provided (note: name is empty if player not exist)
    if ((!player || !*player) && (!player_guid || !*player_guid) && (!player_name || player_name->empty()))
    {
        if (use_extended_response && !name.empty())
        {
            std::string message(GetMangosString(LANG_PLAYER_NOT_FOUND));
            message.append(" (" + name + ")");
            SendSysMessage(message.c_str());
        }
        else
        {
            SendSysMessage(LANG_PLAYER_NOT_FOUND);
        }
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

uint32 ChatHandler::ExtractAccountId(char** args, std::string* accountName /*= nullptr*/, Player** targetIfNullArg /*= nullptr*/, bool checkAccountId)
{
    uint32 account_id = 0;

    ///- Get the account name from the command line
    char* account_str = ExtractLiteralArg(args);

    if (!account_str)
    {


        if (!targetIfNullArg)
            return 0;

        /// only target player different from self allowed (if targetPlayer!=nullptr then not console)
        Player* targetPlayer = GetSelectedPlayer();
        if (!targetPlayer)
            return 0;

        account_id = targetPlayer->GetSession()->GetAccountId();

        if (accountName)
            sAccountMgr.GetName(account_id, *accountName);

        if (targetIfNullArg)
            *targetIfNullArg = targetPlayer;

        return account_id;
    }

    std::string account_name;

    if (checkAccountId && ExtractUInt32(&account_str, account_id))
    {
        if (!sAccountMgr.GetName(account_id, account_name))
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_str);
            SetSentErrorMessage(true);
            return 0;
        }
    }
    else
    {
        account_name = account_str;
        if (!AccountMgr::normalizeString(account_name))
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            SetSentErrorMessage(true);
            return 0;
        }

        account_id = sAccountMgr.GetId(account_name);
        if (!account_id)
        {
            PSendSysMessage(LANG_ACCOUNT_NOT_EXIST, account_name.c_str());
            SetSentErrorMessage(true);
            return 0;
        }
    }

    if (accountName)
        *accountName = account_name;

    if (targetIfNullArg)
        *targetIfNullArg = nullptr;

    return account_id;
}

struct RaceMaskName
{
    char const* literal;
    uint32 raceMask;
};

static RaceMaskName const raceMaskNames[] =
{
    // races
    { "human", (1 << (RACE_HUMAN - 1))   },
    { "orc", (1 << (RACE_ORC - 1))     },
    { "dwarf", (1 << (RACE_DWARF - 1))   },
    { "nightelf", (1 << (RACE_NIGHTELF - 1))},
    { "undead", (1 << (RACE_UNDEAD - 1))  },
    { "tauren", (1 << (RACE_TAUREN - 1))  },
    { "gnome", (1 << (RACE_GNOME - 1))   },
    { "troll", (1 << (RACE_TROLL - 1))   },

    // masks
    { "alliance", RACEMASK_ALLIANCE },
    { "horde",    RACEMASK_HORDE },
    { "all", RACEMASK_ALL_PLAYABLE },

    // terminator
    {nullptr, 0 }
};

bool ChatHandler::ExtractRaceMask(char** text, uint32& raceMask, char const** maskName /*=nullptr*/)
{
    if (ExtractUInt32(text, raceMask))
    {
        if (maskName)
            *maskName = "custom mask";
    }
    else
    {
        for (RaceMaskName const* itr = raceMaskNames; itr->literal; ++itr)
        {
            if (ExtractLiteralArg(text, itr->literal))
            {
                raceMask = itr->raceMask;

                if (maskName)
                    *maskName = itr->literal;
                break;
            }
        }

        if (!raceMask)
            return false;
    }

    return true;
}

std::string ChatHandler::GetNameLink(Player* chr) const
{
    return playerLink(chr->GetName());
}

std::string ChatHandler::GetItemLink(ItemPrototype const* pItem) const
{
    if (m_session)
    {
        uint32 color = ItemQualityColors[pItem->Quality];
        std::ostringstream itemStr;
        itemStr << "|c" << std::hex << color << "|Hitem:" << std::to_string(pItem->ItemId) << ":0:0:0:0:0:0:0|h[" << pItem->Name1 << "]|h|r";
        return itemStr.str();
    }

    return pItem->Name1;
}

bool ChatHandler::needReportToTarget(Player* chr, bool forTeleport) const
{
    Player* pl = m_session->GetPlayer();
    if (forTeleport)
    {
        if (chr->GetSession()->GetSecurity() < m_session->GetSecurity())
            return false;
    }
    return pl != chr && pl->IsVisibleGloballyFor(chr);
}

LocaleConstant ChatHandler::GetSessionDbcLocale() const
{
    return m_session->GetSessionDbcLocale();
}

int ChatHandler::GetSessionDbLocaleIndex() const
{
    return m_session->GetSessionDbLocaleIndex();
}

const char *CliHandler::GetMangosString(int32 entry) const
{
    return sObjectMgr.GetMangosStringForDBCLocale(entry);
}

uint32 CliHandler::GetAccountId() const
{
    return m_accountId;
}

AccountTypes CliHandler::GetAccessLevel() const
{
    return m_loginAccessLevel;
}

bool CliHandler::isAvailable(ChatCommand const& cmd) const
{
    // skip non-console commands in console case
    if (!cmd.AllowConsole)
        return false;

    // normal case
    return GetAccessLevel() >= (AccountTypes)cmd.SecurityLevel;
}

void CliHandler::SendSysMessage(const char *str)
{
    m_print(m_callbackArg, str);
    m_print(m_callbackArg, "\r\n");
}

std::string CliHandler::GetNameLink() const
{
    return GetMangosString(LANG_CONSOLE_COMMAND);
}

bool CliHandler::needReportToTarget(Player* /*chr*/, bool forTelport) const
{
    return true;
}

LocaleConstant CliHandler::GetSessionDbcLocale() const
{
    return sWorld.GetDefaultDbcLocale();
}

int CliHandler::GetSessionDbLocaleIndex() const
{
    return sObjectMgr.GetDBCLocaleIndex();
}

// Check/ Output if a NPC or GO (by guid) is part of a pool or game event
template <typename T>
void ChatHandler::ShowNpcOrGoSpawnInformation(uint32 guid)
{
    if (uint16 pool_id = sPoolMgr.IsPartOfAPool<T>(guid))
    {
        uint16 top_pool_id = sPoolMgr.IsPartOfTopPool<Pool>(pool_id);
        if (!top_pool_id || top_pool_id == pool_id)
            PSendSysMessage(LANG_NPC_GO_INFO_POOL, pool_id);
        else
            PSendSysMessage(LANG_NPC_GO_INFO_TOP_POOL, pool_id, top_pool_id);

        if (int16 event_id = sGameEventMgr.GetGameEventId<Pool>(top_pool_id))
        {
            GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();
            GameEventData const& eventData = events[std::abs(event_id)];

            if (event_id > 0)
                PSendSysMessage(LANG_NPC_GO_INFO_POOL_GAME_EVENT_S, top_pool_id, std::abs(event_id), eventData.description.c_str());
            else
                PSendSysMessage(LANG_NPC_GO_INFO_POOL_GAME_EVENT_D, top_pool_id, std::abs(event_id), eventData.description.c_str());
        }
    }
    else if (int16 event_id = sGameEventMgr.GetGameEventId<T>(guid))
    {
        GameEventMgr::GameEventDataMap const& events = sGameEventMgr.GetEventMap();
        GameEventData const& eventData = events[std::abs(event_id)];

        if (event_id > 0)
            PSendSysMessage(LANG_NPC_GO_INFO_GAME_EVENT_S, uint32(event_id), eventData.description.c_str());
        else
            PSendSysMessage(LANG_NPC_GO_INFO_GAME_EVENT_D, uint32(-event_id), eventData.description.c_str());
    }
}

// Prepare ShortString for a NPC or GO (by guid) with pool or game event IDs
template <typename T>
std::string ChatHandler::PrepareStringNpcOrGoSpawnInformation(uint32 guid)
{
    std::string str;
    if (uint16 pool_id = sPoolMgr.IsPartOfAPool<T>(guid))
    {
        uint16 top_pool_id = sPoolMgr.IsPartOfTopPool<T>(guid);
        if (int16 event_id = sGameEventMgr.GetGameEventId<Pool>(top_pool_id))
        {
            char buffer[100];
            const char* format = GetMangosString(LANG_NPC_GO_INFO_POOL_EVENT_STRING);
            sprintf(buffer, format, pool_id, event_id);
            str = buffer;
        }
        else
        {
            char buffer[100];
            const char* format = GetMangosString(LANG_NPC_GO_INFO_POOL_STRING);
            sprintf(buffer, format, pool_id);
            str = buffer;
        }
    }
    else if (int16 event_id = sGameEventMgr.GetGameEventId<T>(guid))
    {
        char buffer[100];
        const char* format = GetMangosString(LANG_NPC_GO_INFO_EVENT_STRING);
        sprintf(buffer, format, event_id);
        str = buffer;
    }

    return str;
}

// Instantiate template for helper function
template void ChatHandler::ShowNpcOrGoSpawnInformation<Creature>(uint32 guid);
template void ChatHandler::ShowNpcOrGoSpawnInformation<GameObject>(uint32 guid);

template std::string ChatHandler::PrepareStringNpcOrGoSpawnInformation<Creature>(uint32 guid);
template std::string ChatHandler::PrepareStringNpcOrGoSpawnInformation<GameObject>(uint32 guid);


LocaleConstant NullChatHandler::GetSessionDbcLocale() const
{
    return sWorld.GetDefaultDbcLocale();
}

int NullChatHandler::GetSessionDbLocaleIndex() const
{
    return sObjectMgr.GetDBCLocaleIndex();
}

const char *NullChatHandler::GetMangosString(int32 entry) const
{
    return sObjectMgr.GetMangosStringForDBCLocale(entry);
}
