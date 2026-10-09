/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) vMaNGOS contributors <https://github.com/vmangos/core>
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

#include "ScriptMgr.h"
#include "Policies/SingletonImp.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "ScriptObjects.h"
#include "WaypointManager.h"
#include "World.h"
#include "GossipDef.h"
#include "SpellAuras.h"
#include "SpellEntry.h"
#include "Conditions.h"
#include "GameEventMgr.h"
#include "CreatureGroups.h"
#ifdef ENABLE_ELUNA
#include "LuaEngine.h"
#endif
#include "InstanceData.h"

#include <algorithm>

typedef std::vector<Script*> ScriptVector;
int num_sc_scripts;
ScriptVector m_NPC_scripts;

ScriptMapMap sQuestEndScripts;
ScriptMapMap sQuestStartScripts;
ScriptMapMap sSpellScripts;
ScriptMapMap sCreatureSpellScripts;
ScriptMapMap sGameObjectScripts;
ScriptMapMap sEventScripts;
ScriptMapMap sGenericScripts;
ScriptMapMap sGossipScripts;
ScriptMapMap sCreatureMovementScripts;
ScriptMapMap sCreatureAIScripts;

ScriptMgr sScriptMgr;

namespace
{
    template<class TScript>
    void AddAfterDatabaseLoadScripts()
    {
        ScriptRegistry<TScript>::AddAfterDatabaseLoadScripts();
    }

    void AddScriptObjectRegistriesAfterDatabaseLoad()
    {
        AddAfterDatabaseLoadScripts<CreatureScript>();
        AddAfterDatabaseLoadScripts<GameObjectScript>();
        AddAfterDatabaseLoadScripts<ItemScript>();
        AddAfterDatabaseLoadScripts<SpellScriptLoader>();
        AddAfterDatabaseLoadScripts<AreaTriggerScript>();
        AddAfterDatabaseLoadScripts<InstanceMapScript>();
        AddAfterDatabaseLoadScripts<TransportScript>();
        AddAfterDatabaseLoadScripts<WeatherScript>();
        AddAfterDatabaseLoadScripts<ConditionScript>();
        AddAfterDatabaseLoadScripts<GameEventScript>();
        AddAfterDatabaseLoadScripts<BattlegroundScript>();
        AddAfterDatabaseLoadScripts<OutdoorPvPScript>();
    }
}

ScriptMgr::ScriptMgr()
    : m_scheduledScripts(0),
      m_scriptLoaderCallback(nullptr),
      m_modulesLoaderCallback(nullptr)
{
}

ScriptMgr::~ScriptMgr()
{
    m_NPC_scripts.clear();

    num_sc_scripts = 0;
}

void DisableScriptAction(ScriptInfo& script)
{
    script.command = SCRIPT_COMMAND_DISABLED;
    script.target_type = TARGET_T_PROVIDED_TARGET;
    script.raw.data[4] = 0;
    script.condition = 0;
}

void ScriptMgr::LoadScripts(ScriptMapMap& scripts, const char* tablename)
{
    if (IsScriptScheduled())                                // function don't must be called in time scripts use.
        return;

    scripts.clear();                                        // need for reload support

    //                                                  0    1       2         3         4          5          6         7           8             9          10        11        12        13        14    15 16 17 18       19
    QueryResult *result = WorldDatabase.PQuery("SELECT id, delay, command, datalong, datalong2, datalong3, datalong4, target_param1, target_param2, target_type, data_flags, dataint, dataint2, dataint3, dataint4, x, y, z, o, condition_id FROM %s ORDER BY `id`, `delay`, `priority`", tablename);

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();
        ScriptInfo tmp;
        tmp.id           = fields[0].GetUInt32();
        tmp.delay        = fields[1].GetUInt32();
        tmp.command      = fields[2].GetUInt32();
        tmp.raw.data[0]  = fields[3].GetUInt32();
        tmp.raw.data[1]  = fields[4].GetUInt32();
        tmp.raw.data[2]  = fields[5].GetUInt32();
        tmp.raw.data[3]  = fields[6].GetUInt32();

        tmp.target_param1     = fields[7].GetUInt32();
        tmp.target_param2 = fields[8].GetUInt32();
        tmp.target_type   = fields[9].GetUInt8();

        tmp.raw.data[4]  = fields[10].GetUInt32();
        tmp.raw.data[5]  = fields[11].GetInt32();
        tmp.raw.data[6]  = fields[12].GetInt32();
        tmp.raw.data[7]  = fields[13].GetInt32();
        tmp.raw.data[8]  = fields[14].GetInt32();
        tmp.x            = fields[15].GetFloat();
        tmp.y            = fields[16].GetFloat();
        tmp.z            = fields[17].GetFloat();
        tmp.o            = fields[18].GetFloat();
        tmp.condition    = fields[19].GetUInt32();

        if (tmp.condition && !sConditionStorage.LookupEntry<ConditionEntry>(tmp.condition))
        {
            sLog.outErrorDb("Table `%s` has condition = %u for script id %u, but this condition entry does not exist.", tablename, tmp.condition, tmp.id);
            continue;
        }

		if (tmp.command >= SCRIPT_COMMAND_MAX)
		{
			sLog.outErrorDb("Table `%s` has command = %u for script id %u, but this command is not exist", tablename, tmp.command, tmp.id);
			continue;
		}

        if (!CheckScriptTargets(tmp.target_type, tmp.target_param1, tmp.target_param2, tablename, tmp.id))
            DisableScriptAction(tmp);

        if (tmp.raw.data[4] != (tmp.raw.data[4] & ALL_DB_SCRIPT_FLAGS))
        {
            sLog.outErrorDb("Table `%s` has unknown script flags (data_flags = %u) for script id %u", tablename, tmp.raw.data[4], tmp.id);
            continue;
        }

        if (!tmp.target_type && (tmp.raw.data[4] & SF_GENERAL_SWAP_INITIAL_TARGETS) && (tmp.raw.data[4] & SF_GENERAL_SWAP_FINAL_TARGETS))
        {
            sLog.outErrorDb("Table `%s` has nonsensical flag combination (data_flags = %u) without a buddy for script id %u", tablename, tmp.raw.data[4], tmp.id);
            continue;
        }

        if (tmp.target_type && (tmp.raw.data[4] & SF_GENERAL_TARGET_SELF) && !(tmp.raw.data[4] & (SF_GENERAL_SWAP_INITIAL_TARGETS | SF_GENERAL_SWAP_FINAL_TARGETS)))
        {
            sLog.outErrorDb("Table `%s` has nonsensical flag and target combination (data_flags = %u) (target_type = %u) for script id %u", tablename, tmp.raw.data[4], tmp.target_type, tmp.id);
            continue;
        }

        // generic command args check
        switch (tmp.command)
        {
            case SCRIPT_COMMAND_TALK:
            {
                if (tmp.talk.chatType > CHAT_TYPE_ZONE_YELL)
                {
                    sLog.outErrorDb("Table `%s` has invalid CHAT_TYPE_ (datalong = %u) in SCRIPT_COMMAND_TALK for script id %u", tablename, tmp.talk.chatType, tmp.id);
                    continue;
                }

                if (tmp.talk.textId[0] == 0)
                {
                    sLog.outErrorDb("Table `%s` has invalid talk text id (dataint = %i) in SCRIPT_COMMAND_TALK for script id %u", tablename, tmp.talk.textId[0], tmp.id);
                    continue;
                }

                // Positive ids reference broadcast_text; legacy ScriptDev2 and
                // Turtle content deliberately use negative script_texts ids.
                // Both forms are handled by DoScriptText().
                break;
            }
            case SCRIPT_COMMAND_EMOTE:
            {
                if (!sEmotesStore.LookupEntry(tmp.emote.emoteId[0]))
                {
                    sLog.outErrorDb("Table `%s` has invalid emote id (datalong = %u) in SCRIPT_COMMAND_EMOTE for script id %u", tablename, tmp.emote.emoteId[0], tmp.id);
                    continue;
                }
                for (uint32 i = 1; i < MAX_EMOTE_ID; i++)
                {
                    if (tmp.emote.emoteId[i] && !sEmotesStore.LookupEntry(tmp.emote.emoteId[i]))
                    {
                        sLog.outErrorDb("Table `%s` has invalid emote id (datalong%u = %u) in SCRIPT_COMMAND_EMOTE for script id %u", tablename, i, tmp.emote.emoteId[i], tmp.id);
                        tmp.emote.emoteId[i] = 0;
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_FIELD_SET:
            {
                tmp.setField.fieldId = GetIndexOfUpdateFieldForCurrentBuild(tmp.setField.fieldId);
                break;
            }
            case SCRIPT_COMMAND_MOVE_TO:
            {
                if (tmp.moveTo.coordinatesType >= MOVETO_COORDINATES_MAX)
                {
                    sLog.outErrorDb("Table `%s` has invalid coordinates type (datalong = %u) in SCRIPT_COMMAND_MOVE_TO for script id %u", tablename, tmp.moveTo.coordinatesType, tmp.id);
                    continue;
                }

                // combined flags of MoveOptions enum
                if (tmp.moveTo.movementOptions > 511)
                {
                    sLog.outErrorDb("Table `%s` has invalid movement options (datalong3 = %u) in SCRIPT_COMMAND_MOVE_TO for script id %u", tablename, tmp.moveTo.movementOptions, tmp.id);
                    continue;
                }

                break;
            }
            case SCRIPT_COMMAND_MODIFY_FLAGS:
            {
                tmp.modFlags.fieldId = GetIndexOfUpdateFieldForCurrentBuild(tmp.modFlags.fieldId);
                break;
            }
            case SCRIPT_COMMAND_INTERRUPT_CASTS:
            {
                if (tmp.interruptCasts.spellId && !sSpellMgr.GetSpellEntry(tmp.interruptCasts.spellId))
                {
                    if (!sSpellMgr.IsExistingSpellId(tmp.interruptCasts.spellId))
                    {
                        sLog.outErrorDb("Table `%s` using nonexistent spell (id: %u) in SCRIPT_COMMAND_INTERRUPT_CASTS for script id %u", tablename, tmp.interruptCasts.spellId, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }

                break;
            }
            case SCRIPT_COMMAND_TELEPORT_TO:
            {
                if (!sMapStorage.LookupEntry<MapEntry>(tmp.teleportTo.mapId))
                {
                    sLog.outErrorDb("Table `%s` has invalid map (Id: %u) in SCRIPT_COMMAND_TELEPORT_TO for script id %u", tablename, tmp.teleportTo.mapId, tmp.id);
                    continue;
                }

                if (!MaNGOS::IsValidMapCoord(tmp.x, tmp.y, tmp.z, tmp.o))
                {
                    sLog.outErrorDb("Table `%s` has invalid coordinates (X: %f Y: %f) in SCRIPT_COMMAND_TELEPORT_TO for script id %u", tablename, tmp.x, tmp.y, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_QUEST_EXPLORED:
            {
                Quest const* quest = sObjectMgr.GetQuestTemplate(tmp.questExplored.questId);
                if (!quest)
                {
                    if (!sObjectMgr.IsExistingQuestId(tmp.questExplored.questId))
                    {
                        sLog.outErrorDb("Table `%s` has invalid quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u", tablename, tmp.questExplored.questId, tmp.id);
                        continue;
                    }
                    else
                    {
                        DisableScriptAction(tmp);
                        break;
                    }
                }

                if (!quest->HasSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT))
                {
                    sLog.outErrorDb("Table `%s` has quest (ID: %u) in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, but quest not have flag QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT in quest flags. Script command or quest flags wrong. Quest modified to require objective.", tablename, tmp.questExplored.questId, tmp.id);

                    // this will prevent quest completing without objective
                    const_cast<Quest*>(quest)->SetSpecialFlag(QUEST_SPECIAL_FLAG_EXPLORATION_OR_EVENT);

                    // continue; - quest objective requirement set and command can be allowed
                }

                if (float(tmp.questExplored.distance) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u",
                                    tablename, tmp.questExplored.distance, tmp.id);
                    continue;
                }

                if (tmp.questExplored.distance && float(tmp.questExplored.distance) > DEFAULT_VISIBILITY_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too large distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, max distance is %f or 0 for disable distance check",
                                    tablename, tmp.questExplored.distance, tmp.id, DEFAULT_VISIBILITY_DISTANCE);
                    continue;
                }

                if (tmp.questExplored.distance && float(tmp.questExplored.distance) < INTERACTION_DISTANCE)
                {
                    sLog.outErrorDb("Table `%s` has too small distance (%u) for exploring objective complete in `datalong2` in SCRIPT_COMMAND_QUEST_EXPLORED in `datalong` for script id %u, min distance is %f or 0 for disable distance check",
                                    tablename, tmp.questExplored.distance, tmp.id, INTERACTION_DISTANCE);
                    continue;
                }

                break;
            }
            case SCRIPT_COMMAND_KILL_CREDIT:
            {
                if (!sObjectMgr.GetCreatureTemplate(tmp.killCredit.creatureEntry))
                {
                    if (!sObjectMgr.IsExistingCreatureId(tmp.killCredit.creatureEntry))
                    {
                        sLog.outErrorDb("Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_KILL_CREDIT for script id %u", tablename, tmp.killCredit.creatureEntry, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                break;
            }
            case SCRIPT_COMMAND_RESPAWN_GAMEOBJECT:
            {
                if (tmp.GetGOGuid()) // cant check when using buddy\source\target instead
                {
                    GameObjectData const* data = sObjectMgr.GetGOData(tmp.GetGOGuid());
                    if (!data)
                    {
                        if (!sObjectMgr.IsExistingGameObjectGuid(tmp.GetGOGuid()))
                        {
                            sLog.outErrorDb("Table `%s` has invalid gameobject (GUID: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u", tablename, tmp.GetGOGuid(), tmp.id);
                            continue;
                        }
                        else
                        {
                            DisableScriptAction(tmp);
                            break;
                        }
                    }

                    GameObjectInfo const* info = sObjectMgr.GetGameObjectInfo(data->id);
                    if (!info)
                    {
                        sLog.outErrorDb("Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u", tablename, tmp.GetGOGuid(), data->id, tmp.id);
                        continue;
                    }

                    if (info->type == GAMEOBJECT_TYPE_FISHINGNODE ||
                        info->type == GAMEOBJECT_TYPE_FISHINGHOLE ||
                        info->type == GAMEOBJECT_TYPE_DOOR)
                    {
                        sLog.outErrorDb("Table `%s` have gameobject type (%u) unsupported by command SCRIPT_COMMAND_RESPAWN_GAMEOBJECT for script id %u", tablename, info->id, tmp.id);
                        continue;
                    }
                }
                
                break;
            }
            case SCRIPT_COMMAND_TEMP_SUMMON_CREATURE:
            {
                if (!MaNGOS::IsValidMapCoord(tmp.x, tmp.y, tmp.z, tmp.o))
                {
                    sLog.outErrorDb("Table `%s` has invalid coordinates (X: %f Y: %f) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u", tablename, tmp.x, tmp.y, tmp.id);
                    continue;
                }

                if (!sObjectMgr.GetCreatureTemplate(tmp.summonCreature.creatureEntry))
                {
                    if (!sObjectMgr.IsExistingCreatureId(tmp.summonCreature.creatureEntry))
                    {
                        sLog.outErrorDb("Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_TEMP_SUMMON_CREATURE for script id %u", tablename, tmp.summonCreature.creatureEntry, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                break;
            }
            case SCRIPT_COMMAND_OPEN_DOOR:
            case SCRIPT_COMMAND_CLOSE_DOOR:
            {
                if (tmp.GetGOGuid()) // cant check when using buddy\source\target instead
                {
                    GameObjectData const* data = sObjectMgr.GetGOData(tmp.GetGOGuid());
                    if (!data)
                    {
                        if (!sObjectMgr.IsExistingGameObjectGuid(tmp.GetGOGuid()))
                        {
                            sLog.outErrorDb("Table `%s` has invalid gameobject (GUID: %u) in %s for script id %u", tablename, tmp.GetGOGuid(), (tmp.command == SCRIPT_COMMAND_OPEN_DOOR ? "SCRIPT_COMMAND_OPEN_DOOR" : "SCRIPT_COMMAND_CLOSE_DOOR"), tmp.id);
                            continue;
                        }
                        else
                        {
                            DisableScriptAction(tmp);
                            break;
                        }
                    }

                    GameObjectInfo const* info = sObjectMgr.GetGameObjectInfo(data->id);
                    if (!info)
                    {
                        sLog.outErrorDb("Table `%s` has gameobject with invalid entry (GUID: %u Entry: %u) in %s for script id %u", tablename, tmp.GetGOGuid(), data->id, (tmp.command == SCRIPT_COMMAND_OPEN_DOOR ? "SCRIPT_COMMAND_OPEN_DOOR" : "SCRIPT_COMMAND_CLOSE_DOOR"), tmp.id);
                        continue;
                    }

                    if (info->type != GAMEOBJECT_TYPE_DOOR)
                    {
                        sLog.outErrorDb("Table `%s` has gameobject type (%u) non supported by command %s for script id %u", tablename, info->id, (tmp.command == SCRIPT_COMMAND_OPEN_DOOR ? "SCRIPT_COMMAND_OPEN_DOOR" : "SCRIPT_COMMAND_CLOSE_DOOR"), tmp.id);
                        continue;
                    }
                }

                break;
            }
            case SCRIPT_COMMAND_REMOVE_AURA:
            {
                if (tmp.removeAura.spellId && !sSpellMgr.GetSpellEntry(tmp.removeAura.spellId))
                {
                    if (!sSpellMgr.IsExistingSpellId(tmp.removeAura.spellId))
                    {
                        sLog.outErrorDb("Table `%s` using nonexistent spell (id: %u) in SCRIPT_COMMAND_REMOVE_AURA for script id %u",
                            tablename, tmp.removeAura.spellId, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                break;
            }
            case SCRIPT_COMMAND_CAST_SPELL:
            {
                if (!sSpellMgr.GetSpellEntry(tmp.castSpell.spellId))
                {
                    if (!sSpellMgr.IsExistingSpellId(tmp.castSpell.spellId))
                    {
                        sLog.outErrorDb("Table `%s` using nonexistent spell (id: %u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                            tablename, tmp.castSpell.spellId, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                if (tmp.castSpell.flags & ~ALL_CAST_FLAGS)
                {
                    sLog.outErrorDb("Table `%s` using unknown flags in datalong2 (%u) in SCRIPT_COMMAND_CAST_SPELL for script id %u",
                                    tablename, tmp.castSpell.flags, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_REMOVE_ITEM:
            case SCRIPT_COMMAND_CREATE_ITEM:
            {
                if (!sObjectMgr.GetItemPrototype(tmp.createItem.itemId))
                {
                    if (!sObjectMgr.IsExistingItemId(tmp.createItem.itemId))
                    {
                        sLog.outErrorDb("Table `%s` has nonexistent item (entry: %u) in SCRIPT_COMMAND_*_ITEM for script id %u",
                            tablename, tmp.createItem.itemId, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                if (!tmp.createItem.amount)
                {
                    sLog.outErrorDb("Table `%s` SCRIPT_COMMAND_*_ITEM but amount is %u for script id %u",
                                    tablename, tmp.createItem.amount, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_TAKE_MONEY:
            {
                if (!tmp.takeMoney.amount)
                {
                    sLog.outErrorDb("Table `%s` SCRIPT_COMMAND_TAKE_MONEY but amount is %u for script id %u",
                                    tablename, tmp.takeMoney.amount, tmp.id);
                    continue;
                }
                if (tmp.takeMoney.amount > 0x7FFFFFFF)
                {
                    sLog.outErrorDb("Table `%s` SCRIPT_COMMAND_TAKE_MONEY amount is too large (%u) for script id %u",
                                    tablename, tmp.takeMoney.amount, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_DESPAWN_CREATURE:
            {
                break;
            }
            case SCRIPT_COMMAND_SET_EQUIPMENT:
            {
                bool cancel = false;
                bool disable = false;

                for (int i = 0; i < 3; i++)
                {
                    if (tmp.setEquipment.slot[i] > 0)
                    {
                        if (!sObjectMgr.GetItemPrototype(tmp.setEquipment.slot[i]))
                        {
                            if (!sObjectMgr.IsExistingItemId(tmp.createItem.itemId))
                            {
                                sLog.outErrorDb("Table `%s` has nonexistent item (dataint%i: %u) in SCRIPT_COMMAND_SET_EQUIPMENT for script id %u",
                                    tablename, i, tmp.setEquipment.slot[i], tmp.id);
                                cancel = true;
                            }
                            else
                                disable = true;
                        }
                    }
                }
                
                if (cancel)
                    continue;

                if (disable)
                    DisableScriptAction(tmp);

                break;
            }
            case SCRIPT_COMMAND_MOVEMENT:
            {
                switch (tmp.movement.movementType)
                {
                    case IDLE_MOTION_TYPE:
                    case RANDOM_MOTION_TYPE:
                    case WAYPOINT_MOTION_TYPE:
                    case CONFUSED_MOTION_TYPE:
                    case CHASE_MOTION_TYPE:
                    case HOME_MOTION_TYPE:
                    case FLEEING_MOTION_TYPE:
                    case DISTRACT_MOTION_TYPE:
                    case FOLLOW_MOTION_TYPE:
                    case CHARGE_MOTION_TYPE:
                    case DISTANCING_MOTION_TYPE:
                        break;
                    default:
                    {
                        sLog.outErrorDb("Table `%s` SCRIPT_COMMAND_MOVEMENT has invalid MovementType %u for script id %u",
                            tablename, tmp.movement.movementType, tmp.id);
                        continue;
                    }
                }

                if (tmp.movement.boolParam > 1)
                {
                    sLog.outErrorDb("Table `%s` SCRIPT_COMMAND_MOVEMENT has wrong value in datalong2=%u (must be bool 0/1) for script id %u",
                                    tablename, tmp.movement.boolParam, tmp.id);
                    continue;
                }

                break;
            }
            case SCRIPT_COMMAND_SET_ACTIVEOBJECT:
            {
                break;
            }
            case SCRIPT_COMMAND_SET_FACTION:
            {
                if (tmp.faction.factionId && !sObjectMgr.GetFactionTemplateEntry(tmp.faction.factionId))
                {
                    sLog.outErrorDb("Table `%s` has datalong2 = %u in SCRIPT_COMMAND_SET_FACTION for script id %u, but this faction does not exist.", tablename, tmp.faction.factionId, tmp.id);
                    continue;
                }

                break;
            }
            case SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL:
            {
                if (tmp.morph.isDisplayId)
                {
                    if (tmp.morph.creatureOrModelEntry && !sCreatureDisplayInfoStore.LookupEntry(tmp.morph.creatureOrModelEntry))
                    {
                        sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL for script id %u, but this model does not exist.", tablename, tmp.morph.creatureOrModelEntry, tmp.id);
                        continue;
                    }
                }
                else
                {
                    if (tmp.morph.creatureOrModelEntry && !sObjectMgr.GetCreatureTemplate(tmp.morph.creatureOrModelEntry))
                    {
                        if (!sObjectMgr.IsExistingCreatureId(tmp.morph.creatureOrModelEntry))
                        {
                            sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_MORPH_TO_ENTRY_OR_MODEL for script id %u, but this creature_template does not exist.", tablename, tmp.morph.creatureOrModelEntry, tmp.id);
                            continue;
                        }
                        else
                            DisableScriptAction(tmp);
                    }
                }

                break;
            }
            case SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL:
            {
                if (tmp.mount.isDisplayId)
                {
                    if (tmp.mount.creatureOrModelEntry && !sCreatureDisplayInfoStore.LookupEntry(tmp.mount.creatureOrModelEntry))
                    {
                        sLog.outErrorDb("Table `%s` has datalong2 = %u in SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL for script id %u, but this model does not exist.", tablename, tmp.mount.creatureOrModelEntry, tmp.id);
                        continue;
                    }
                }
                else
                {
                    if (tmp.mount.creatureOrModelEntry && !sObjectMgr.GetCreatureTemplate(tmp.mount.creatureOrModelEntry))
                    {
                        if (!sObjectMgr.IsExistingCreatureId(tmp.mount.creatureOrModelEntry))
                        {
                            sLog.outErrorDb("Table `%s` has datalong2 = %u in SCRIPT_COMMAND_MOUNT_TO_ENTRY_OR_MODEL for script id %u, but this creature_template does not exist.", tablename, tmp.mount.creatureOrModelEntry, tmp.id);
                            continue;
                        }
                        else
                            DisableScriptAction(tmp);
                    }
                }

                break;
            }
            case SCRIPT_COMMAND_SET_RUN:
            {
                break;
            }
            case SCRIPT_COMMAND_ATTACK_START:
            {
                break;
            }
            case SCRIPT_COMMAND_UPDATE_ENTRY:
            {
                if (!sObjectMgr.GetCreatureTemplate(tmp.updateEntry.creatureEntry))
                {
                    if (!sObjectMgr.IsExistingCreatureId(tmp.updateEntry.creatureEntry))
                    {
                        sLog.outErrorDb("Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_UPDATE_ENTRY for script id %u", tablename, tmp.updateEntry.creatureEntry, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }

                break;
            }
            case SCRIPT_COMMAND_STAND_STATE:
            {
                if (tmp.standState.standState >= MAX_UNIT_STAND_STATE)
                {
                    sLog.outErrorDb("Table `%s` has invalid stand state (datalong = %u) in SCRIPT_COMMAND_STAND_STATE for script id %u", tablename, tmp.standState.standState, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_MODIFY_THREAT:
            {
                if (tmp.modThreat.target > SO_MODIFYTHREAT_ALL_ATTACKERS)
                {
                    sLog.outErrorDb("Table `%s` has invalid target type (datalong = %u) in SCRIPT_COMMAND_MODIFY_THREAT for script id %u", tablename, tmp.modThreat.target, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_SEND_TAXI_PATH:
            {
                if (!sTaxiPathStore.LookupEntry(tmp.sendTaxiPath.taxiPathId))
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_SEND_TAXI_PATH for script id %u, but this taxi path does not exist.", tablename, tmp.sendTaxiPath.taxiPathId, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_TERMINATE_SCRIPT:
            {
                if (tmp.terminateScript.creatureEntry && !sObjectMgr.GetCreatureTemplate(tmp.terminateScript.creatureEntry))
                {
                    if (!sObjectMgr.IsExistingCreatureId(tmp.terminateScript.creatureEntry))
                    {
                        sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_TERMINATE_SCRIPT for script id %u, but this npc entry does not exist.", tablename, tmp.terminateScript.creatureEntry, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                if (tmp.terminateScript.creatureEntry && !tmp.terminateScript.searchRadius)
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u in  SCRIPT_COMMAND_TERMINATE_SCRIPT for script id %u, but search radius is too small (datalong2 = %u).", tablename, tmp.terminateScript.creatureEntry, tmp.id, tmp.terminateScript.searchRadius);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_TERMINATE_CONDITION:
            {
                if (!sConditionStorage.LookupEntry<ConditionEntry>(tmp.terminateCond.conditionId))
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_TERMINATE_CONDITION for script id %u, but this condition_id does not exist.", tablename, tmp.terminateCond.conditionId, tmp.id);
                    continue;
                }
                if (tmp.terminateCond.failQuest && !sObjectMgr.GetQuestTemplate(tmp.terminateCond.failQuest))
                {
                    sLog.outErrorDb("Table `%s` has datalong2 = %u in SCRIPT_COMMAND_TERMINATE_CONDITION for script id %u, but this questId does not exist.", tablename, tmp.terminateCond.failQuest, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_ENTER_EVADE_MODE:
            {
                break;
            }
            case SCRIPT_COMMAND_SET_HOME_POSITION:
            {
                break;
            }
            case SCRIPT_COMMAND_TURN_TO:
            {
                if (tmp.turnTo.facingLogic > 1)
                {
                    sLog.outErrorDb("Table `%s` using unknown option in datalong (%u) in SCRIPT_COMMAND_TURN_TO for script id %u", tablename, tmp.turnTo.facingLogic, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_MEETINGSTONE:
            {
                if (!sAreaStorage.LookupEntry<AreaEntry>(tmp.meetingstone.areaId))
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u in  SCRIPT_COMMAND_MEETINGSTONE for script id %u, but there is no area with this Id.", tablename, tmp.meetingstone.areaId, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_SET_INST_DATA:
            {
                if (tmp.setData.type >= SO_INSTDATA_MAX)
                {
                    sLog.outErrorDb("Table `%s` has invalid option datalong3 = %u in SCRIPT_COMMAND_SET_INST_DATA for script id %u.", tablename, tmp.setData.type, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_SET_INST_DATA64:
            {
                if (tmp.setData64.type >= SO_INSTDATA64_MAX)
                {
                    sLog.outErrorDb("Table `%s` has invalid option datalong3 = %u in SCRIPT_COMMAND_SET_INST_DATA64 for script id %u.", tablename, tmp.setData64.type, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_START_SCRIPT:
            case SCRIPT_COMMAND_START_SCRIPT_ON_GROUP:
            {
                if (100 < (tmp.startScript.chance[0] + tmp.startScript.chance[1] + tmp.startScript.chance[2] + tmp.startScript.chance[3]))
                {
                    sLog.outErrorDb("Table `%s` has a total chance exceeding 100%% in SCRIPT_COMMAND_START_SCRIPT for script id %u.", tablename, tmp.id);
                    continue;
                }
                bool abort = false;
                for (uint8 i = 0; i < 4; i++)
                {
                    if (tmp.startScript.chance[i] < 0)
                    {
                        abort = true;
                        sLog.outErrorDb("Table `%s` has dataint%u with negative chance in SCRIPT_COMMAND_START_SCRIPT for script id %u.", tablename, i, tmp.id);
                        break;
                    }
                    else if (!tmp.startScript.scriptId[i] && (tmp.startScript.chance[i] > 0))
                    {
                        abort = true;
                        sLog.outErrorDb("Table `%s` has dataint%u=%i but no provided script id in SCRIPT_COMMAND_START_SCRIPT for script id %u.", tablename, i, tmp.startScript.chance[i], tmp.id);
                        break;
                    }
                    else if (tmp.startScript.scriptId[i] && !tmp.startScript.chance[i])
                    {
                        abort = true;
                        sLog.outErrorDb("Table `%s` has datalong%u=%u with 0%% chance in SCRIPT_COMMAND_START_SCRIPT for script id %u.", tablename, i, tmp.startScript.scriptId[i], tmp.id);
                        break;
                    }
                }

                if (abort)
                    continue;

                break;
            }
            case SCRIPT_COMMAND_SET_PHASE:
            {
                if (tmp.setPhase.phase >= 32) // Max EventAI phase.
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u above maximum allowed phase in SCRIPT_COMMAND_SET_PHASE for script id %u.", tablename, tmp.setPhase.phase, tmp.id);
                    continue;
                }
                if (tmp.setPhase.mode >= SO_SETPHASE_MAX)
                {
                    sLog.outErrorDb("Table `%s` has invalid option datalong2 = %u in SCRIPT_COMMAND_SET_PHASE for script id %u.", tablename, tmp.setPhase.mode, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_SET_PHASE_RANDOM:
            {
                bool error = false;
                for (uint8 i = 0; i < 4; i++)
                {
                    if (tmp.setPhaseRandom.phase[i] >= 32) // Max EventAI phase.
                    {
                        sLog.outErrorDb("Table `%s` has datalong%u = %u above maximum allowed phase in SCRIPT_COMMAND_SET_PHASE_RANDOM for script id %u.", tablename, i, tmp.setPhaseRandom.phase[i], tmp.id);
                        error = true;
                        break;
                    }
                }

                if (error)
                    continue;

                break;
            }
            case SCRIPT_COMMAND_SET_PHASE_RANGE:
            {
                if (tmp.setPhaseRange.phaseMax >= 32) // Max EventAI phase.
                {
                    sLog.outErrorDb("Table `%s` has datalong2 = %u above maximum allowed phase in SCRIPT_COMMAND_SET_PHASE_RANGE for script id %u.", tablename, tmp.setPhaseRange.phaseMax, tmp.id);
                    continue;
                }
                if (tmp.setPhaseRange.phaseMin >= tmp.setPhaseRange.phaseMax)
                {
                    sLog.outErrorDb("Table `%s` has phase_min (datalong=%u) >= phase_max (datalong2=%u) in SCRIPT_COMMAND_SET_PHASE_RANGE for script id %u.", tablename, tmp.setPhaseRange.phaseMin, tmp.setPhaseRange.phaseMax, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_DEAL_DAMAGE:
            {
                if (tmp.dealDamage.damage == 0)
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_DEAL_DAMAGE for script id %u.", tablename, tmp.dealDamage.damage, tmp.id);
                    continue;
                }
                if (tmp.dealDamage.isPercent)
                {
                    if (tmp.dealDamage.damage > 100)
                    {
                        sLog.outErrorDb("Table `%s` has datalong = %u above 100% with datalong2 = %u in SCRIPT_COMMAND_DEAL_DAMAGE for script id %u.", tablename, tmp.dealDamage.damage, tmp.dealDamage.isPercent, tmp.id);
                        continue;
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_SET_SHEATH:
            {
                if (tmp.setSheath.sheathState >= MAX_SHEATH_STATE)
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u above maximum sheath state in SCRIPT_COMMAND_SET_SHEATH for script id %u.", tablename, tmp.setSheath.sheathState, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_INVINCIBILITY:
            {
                if (tmp.invincibility.isPercent)
                {
                    if (tmp.invincibility.health > 100)
                    {
                        sLog.outErrorDb("Table `%s` has datalong = %u above 100% with datalong2 = %u in SCRIPT_COMMAND_INVINCIBILITY for script id %u.", tablename, tmp.invincibility.health, tmp.invincibility.isPercent, tmp.id);
                        continue;
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_GAME_EVENT:
            {
                if (!sGameEventMgr.IsValidEvent(tmp.gameEvent.eventId))
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u for a non-existing game event in SCRIPT_COMMAND_GAME_EVENT for script id %u.", tablename, tmp.gameEvent.eventId, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_CREATURE_SPELLS:
            {
                if (100 < (tmp.creatureSpells.chance[0] + tmp.creatureSpells.chance[1] + tmp.creatureSpells.chance[2] + tmp.creatureSpells.chance[3]))
                {
                    sLog.outErrorDb("Table `%s` has a total chance exceeding 100%% in SCRIPT_COMMAND_CREATURE_SPELLS for script id %u.", tablename, tmp.id);
                    continue;
                }
                bool abort = false;
                for (uint8 i = 0; i < 4; i++)
                {
                    if (tmp.creatureSpells.chance[i] < 0)
                    {
                        abort = true;
                        sLog.outErrorDb("Table `%s` has dataint%u with negative chance in SCRIPT_COMMAND_CREATURE_SPELLS for script id %u.", tablename, i, tmp.id);
                        break;
                    }
                    else if (tmp.creatureSpells.spellListId[i])
                    {
                        if (!sObjectMgr.GetCreatureSpellsList(tmp.creatureSpells.spellListId[i]))
                        {
                            if (!sObjectMgr.IsExistingCreatureSpellsId(tmp.creatureSpells.spellListId[i]))
                            {
                                abort = true;
                                sLog.outErrorDb("Table `%s` has datalong%u=%u for a non-existent creature spells template in SCRIPT_COMMAND_CREATURE_SPELLS for script id %u.", tablename, i, tmp.creatureSpells.spellListId[i], tmp.id);
                            }
                            else
                                DisableScriptAction(tmp);
                            break;
                        }
                        if (!tmp.creatureSpells.chance[i])
                        {
                            abort = true;
                            sLog.outErrorDb("Table `%s` has datalong%u=%u with 0%% chance in SCRIPT_COMMAND_CREATURE_SPELLS for script id %u.", tablename, i, tmp.creatureSpells.spellListId[i], tmp.id);
                            break;
                        }
                    }
                }

                if (abort)
                    continue;

                break;
            }
            case SCRIPT_COMMAND_REMOVE_GUARDIANS:
            {
                if (tmp.removeGuardian.creatureId)
                {
                    if (!sObjectMgr.GetCreatureTemplate(tmp.removeGuardian.creatureId))
                    {
                        if (!sObjectMgr.IsExistingCreatureId(tmp.removeGuardian.creatureId))
                        {
                            sLog.outErrorDb("Table `%s` has invalid creature (Entry: %u) in SCRIPT_COMMAND_REMOVE_GUARDIANS for script id %u", tablename, tmp.removeGuardian.creatureId, tmp.id);
                            continue;
                        }
                        else
                            DisableScriptAction(tmp);
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_ADD_SPELL_COOLDOWN:
            {
                if (!sSpellMgr.GetSpellEntry(tmp.addCooldown.spellId))
                {
                    if (!sSpellMgr.IsExistingSpellId(tmp.addCooldown.spellId))
                    {
                        sLog.outErrorDb("Table `%s` using nonexistent spell (id: %u) in SCRIPT_COMMAND_ADD_SPELL_COOLDOWN for script id %u", tablename, tmp.addCooldown.spellId, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                if (tmp.addCooldown.cooldown == 0)
                {
                    sLog.outErrorDb("Table `%s` has datalong2 = %u in SCRIPT_COMMAND_ADD_SPELL_COOLDOWN for script id %u.", tablename, tmp.addCooldown.cooldown, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_REMOVE_SPELL_COOLDOWN:
            {
                if (tmp.removeCooldown.spellId)
                {
                    if (!sSpellMgr.GetSpellEntry(tmp.removeCooldown.spellId))
                    {
                        if (!sSpellMgr.IsExistingSpellId(tmp.removeCooldown.spellId))
                        {
                            sLog.outErrorDb("Table `%s` using nonexistent spell (id: %u) in SCRIPT_COMMAND_REMOVE_SPELL_COOLDOWN for script id %u", tablename, tmp.removeCooldown.spellId, tmp.id);
                            continue;
                        }
                        else
                            DisableScriptAction(tmp);
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_SET_REACT_STATE:
            {
                if (tmp.setReactState.state > REACT_AGGRESSIVE)
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_SET_REACT_STATE for script id %u.", tablename, tmp.setReactState.state, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_START_WAYPOINTS:
            {
                if (tmp.startWaypoints.wpSource > PATH_FROM_SPECIAL)
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_START_WAYPOINTS for script id %u.", tablename, tmp.startWaypoints.wpSource, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_SET_DEFAULT_MOVEMENT:
            {
                if (tmp.setDefaultMovement.movementType >= MAX_DB_MOTION_TYPE)
                {
                    sLog.outErrorDb("Table `%s` has datalong = %u in SCRIPT_COMMAND_SET_DEFAULT_MOVEMENT for script id %u.", tablename, tmp.setDefaultMovement.movementType, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_START_SCRIPT_FOR_ALL:
            {
                if (!tmp.startScriptForAll.searchRadius)
                {
                    sLog.outErrorDb("Table `%s` has datalong4 = %u in SCRIPT_COMMAND_START_SCRIPT_FOR_ALL for script id %u.", tablename, tmp.startScriptForAll.searchRadius, tmp.id);
                    continue;
                }
                switch (tmp.startScriptForAll.objectType)
                {
                    case SO_STARTFORALL_GAMEOBJECTS:
                    {
                        if (tmp.startScriptForAll.objectEntry)
                        {
                            if (!sObjectMgr.GetGameObjectInfo(tmp.startScriptForAll.objectEntry))
                            {
                                if (!sObjectMgr.IsExistingGameObjectId(tmp.startScriptForAll.objectEntry))
                                {
                                    sLog.outErrorDb("Table `%s` has gameobject with invalid entry (datalong3: %u) in SCRIPT_COMMAND_START_SCRIPT_FOR_ALL for script id %u", tablename, tmp.startScriptForAll.objectEntry, tmp.id);
                                    continue;
                                }
                                else
                                    DisableScriptAction(tmp);
                            }
                        }
                        break;
                    }
                    case SO_STARTFORALL_CREATURES:
                    {
                        if (tmp.startScriptForAll.objectEntry)
                        {
                            if (!sObjectMgr.GetCreatureTemplate(tmp.startScriptForAll.objectEntry))
                            {
                                if (!sObjectMgr.IsExistingCreatureId(tmp.startScriptForAll.objectEntry))
                                {
                                    sLog.outErrorDb("Table `%s` has invalid creature (datalong3: %u) in SCRIPT_COMMAND_START_SCRIPT_FOR_ALL for script id %u", tablename, tmp.startScriptForAll.objectEntry, tmp.id);
                                    continue;
                                }
                                else
                                    DisableScriptAction(tmp);
                            }
                        }
                        break;
                    }
                    case SO_STARTFORALL_UNITS:
                    case SO_STARTFORALL_PLAYERS:
                    {
                        break;
                    } 
                    default:
                    {
                        sLog.outErrorDb("Table `%s` has datalong2 = %u in SCRIPT_COMMAND_START_SCRIPT_FOR_ALL for script id %u.", tablename, tmp.startScriptForAll.objectType, tmp.id);
                        continue;
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_FAIL_QUEST:
            {
                if (!sObjectMgr.GetQuestTemplate(tmp.failQuest.questId))
                {
                    if (!sObjectMgr.IsExistingQuestId(tmp.failQuest.questId))
                    {
                        sLog.outErrorDb("Table `%s` has invalid quest (ID: %u) in SCRIPT_COMMAND_FAIL_QUEST in `datalong` for script id %u", tablename, tmp.failQuest.questId, tmp.id);
                        continue;
                    }
                    else
                    {
                        DisableScriptAction(tmp);
                        break;
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_ADD_AURA:
            {
                if (auto pSpellEntry = sSpellMgr.GetSpellEntry(tmp.addAura.spellId))
                {
                    if (!pSpellEntry->IsSpellAppliesAura((1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)) && !pSpellEntry->HasEffect(SPELL_EFFECT_PERSISTENT_AREA_AURA))
                    {
                        sLog.outErrorDb("Table `%s` has a spell that does not apply any auras (id: %u) in SCRIPT_COMMAND_ADD_AURA for script id %u",
                            tablename, tmp.addAura.spellId, tmp.id);
                        continue;
                    }
                }
                else
                {
                    if (!sSpellMgr.IsExistingSpellId(tmp.addAura.spellId))
                    {
                        sLog.outErrorDb("Table `%s` using nonexistent spell (id: %u) in SCRIPT_COMMAND_ADD_AURA for script id %u",
                            tablename, tmp.addAura.spellId, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                break;
            }
            case SCRIPT_COMMAND_SUMMON_OBJECT:
            {
                if (!sObjectMgr.GetGameObjectInfo(tmp.summonObject.gameobject_entry))
                {
                    if (!sObjectMgr.IsExistingGameObjectId(tmp.summonObject.gameobject_entry))
                    {
                        sLog.outErrorDb("Table `%s` has datalong = %u for script id %u, but this gameobject_template does not exist.", tablename, tmp.summonObject.gameobject_entry, tmp.id);
                        continue;
                    }
                    else
                        DisableScriptAction(tmp);
                }
                break;
            }
            case SCRIPT_COMMAND_JOIN_CREATURE_GROUP:
            {
                if (tmp.joinCreatureGroup.options & ~ALL_CREATURE_GROUP_OPTIONS)
                {
                    sLog.outErrorDb("Table `%s` using unknown option in datalong (%u) in SCRIPT_COMMAND_JOIN_CREATURE_GROUP for script id %u", tablename, tmp.joinCreatureGroup.options, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_SET_GO_STATE:
            {
                if (tmp.setGoState.state > GO_STATE_ACTIVE_ALTERNATIVE)
                {
                    sLog.outErrorDb("Table `%s` using invalid go state in datalong (%u) in SCRIPT_COMMAND_SET_GO_STATE for script id %u",
                        tablename, tmp.setGoState.state, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_DESPAWN_GAMEOBJECT:
            {
                if (tmp.GetGOGuid()) // cant check when using buddy\source\target instead
                {
                    GameObjectData const* data = sObjectMgr.GetGOData(tmp.GetGOGuid());
                    if (!data)
                    {
                        if (!sObjectMgr.IsExistingGameObjectGuid(tmp.GetGOGuid()))
                        {
                            sLog.outErrorDb("Table `%s` has invalid gameobject (GUID: %u) in SCRIPT_COMMAND_DESPAWN_GAMEOBJECT for script id %u", tablename, tmp.GetGOGuid(), tmp.id);
                            continue;
                        }
                        else
                        {
                            DisableScriptAction(tmp);
                            break;
                        }
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_LOAD_GAMEOBJECT_SPAWN:
            {
                GameObjectData const* data = sObjectMgr.GetGOData(tmp.GetGOGuid());
                if (!data)
                {
                    if (!sObjectMgr.IsExistingGameObjectGuid(tmp.GetGOGuid()))
                    {
                        sLog.outErrorDb("Table `%s` has invalid gameobject (GUID: %u) in SCRIPT_COMMAND_LOAD_GAMEOBJECT_SPAWN for script id %u", tablename, tmp.GetGOGuid(), tmp.id);
                        continue;
                    }
                    else
                    {
                        DisableScriptAction(tmp);
                        break;
                    }
                }
                break;
            }
            case SCRIPT_COMMAND_SET_GOSSIP_MENU:
            {
                if (tmp.setGossipMenu.gossipMenuId && !sObjectMgr.IsExistingGossipMenuId(tmp.setGossipMenu.gossipMenuId))
                {
                    sLog.outErrorDb("Table `%s` using nonexistent gossip menu (id: %u) in SCRIPT_COMMAND_SET_GOSSIP_MENU for script id %u",
                        tablename, tmp.setGossipMenu.gossipMenuId, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_PLAY_CUSTOM_ANIM:
            {
                if (tmp.playCustomAnim.animId > 3)
                {
                    sLog.outErrorDb("Table `%s` using invalid anim id in datalong (%u) in SCRIPT_COMMAND_PLAY_CUSTOM_ANIM for script id %u",
                        tablename, tmp.setGoState.state, tmp.id);
                    continue;
                }
                break;
            }
            case SCRIPT_COMMAND_LOAD_CREATURE_SPAWN:
            {
                if (!sObjectMgr.GetCreatureData(tmp.loadCreature.dbGuid))
                {
                    if (!sObjectMgr.IsExistingCreatureGuid(tmp.loadCreature.dbGuid))
                    {
                        sLog.outErrorDb("Table `%s` using invalid creature guid in datalong (%u) in SCRIPT_COMMAND_LOAD_CREATURE_SPAWN for script id %u",
                            tablename, tmp.loadCreature.dbGuid, tmp.id);
                        continue;
                    }
                    else
                    {
                        DisableScriptAction(tmp);
                        break;
                    }
                }

                break;
            }
            case SCRIPT_COMMAND_START_SCRIPT_ON_ZONE:
            {
                if (!tmp.startScriptOnZone.scriptId)
                {
                    sLog.outErrorDb("Table `%s` has no provided script id in SCRIPT_COMMAND_START_SCRIPT_ON_ZONE for script id %u.", tablename, tmp.id);
                    continue;
                }
                if (!tmp.startScriptOnZone.zoneId)
                {
                    sLog.outErrorDb("Table `%s` has no provided zone id in SCRIPT_COMMAND_START_SCRIPT_ON_ZONE for script id %u.", tablename, tmp.id);
                    continue;
                }
                break;
            }
        }

        if (scripts.find(tmp.id) == scripts.end())
        {
            ScriptMap emptyMap;
            scripts[tmp.id] = emptyMap;
        }
        scripts[tmp.id].insert(ScriptMap::value_type(tmp.delay, tmp));
    }
    while (result->NextRow());

    delete result;
}

bool ScriptMgr::CheckScriptTargets(uint32 targetType, uint32 targetParam1, uint32 targetParam2, char const* tableName, uint32 tableEntry)
{
    switch (targetType)
    {
        case TARGET_T_PROVIDED_TARGET:
        case TARGET_T_HOSTILE:
        case TARGET_T_OWNER_OR_SELF:
        case TARGET_T_OWNER:
        {
            if (targetParam1)
                sLog.outErrorDb("Table `%s` has target_param1 = %u with target_type = %u for id %u, but this target type has no parameters.", tableName, targetParam1, targetType, tableEntry);
            if (targetParam2)
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but this target type has no parameters.", tableName, targetParam2, targetType, tableEntry);
            break;
        }
        case TARGET_T_HOSTILE_SECOND_AGGRO:
        case TARGET_T_HOSTILE_LAST_AGGRO:
        case TARGET_T_HOSTILE_RANDOM:
        case TARGET_T_HOSTILE_RANDOM_NOT_TOP:
        {
            if (targetParam1 & ~MAX_SELECT_FLAG_MASK)
            {
                sLog.outErrorDb("Table `%s` has unknown select flag target_param1 = %u with target_type = %u for id %u.", tableName, targetParam1, targetType, tableEntry);
                return false;
            }
            if (targetParam2)
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but this parameter is unused.", tableName, targetParam2, targetType, tableEntry);
            break;
        }
        case TARGET_T_NEAREST_CREATURE_WITH_ENTRY:
        case TARGET_T_RANDOM_CREATURE_WITH_ENTRY:
        {
            if (!sObjectMgr.GetCreatureTemplate(targetParam1))
            {
                if (!sObjectMgr.IsExistingCreatureId(targetParam1))
                    sLog.outErrorDb("Table `%s` has target_param1 = %u for id %u, but this creature_template does not exist.", tableName, targetParam1, tableEntry);
                return false;
            }
            if (!targetParam2)
            {
                sLog.outErrorDb("Table `%s` has target_param2 = %u for id %u, but search radius is expected.", tableName, targetParam2, tableEntry);
                return false;
            }
            break;
        }
        case TARGET_T_CREATURE_WITH_GUID:
        {
            if (!sObjectMgr.GetCreatureData(targetParam1))
            {
                if (!sObjectMgr.IsExistingCreatureGuid(targetParam1))
                    sLog.outErrorDb("Table `%s` has target_param1 = %u for id %u, but this creature guid does not exist.", tableName, targetParam1, tableEntry);
                return false;
            }
            if (targetParam2)
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but this parameter is unused.", tableName, targetParam2, targetType, tableEntry);
            break;
        }
        case TARGET_T_NEAREST_GAMEOBJECT_WITH_ENTRY:
        case TARGET_T_RANDOM_GAMEOBJECT_WITH_ENTRY:
        {
            if (!sObjectMgr.GetGameObjectInfo(targetParam1))
            {
                if (!sObjectMgr.IsExistingGameObjectId(targetParam1))
                    sLog.outErrorDb("Table `%s` has target_param1 = %u for id %u, but this gameobject_template does not exist.", tableName, targetParam1, tableEntry);
                return false;
            }
            if (!targetParam2)
            {
                sLog.outErrorDb("Table `%s` has target_param2 = %u for id %u, but search radius is expected.", tableName, targetParam2, tableEntry);
                return false;
            }
            break;
        }
        case TARGET_T_GAMEOBJECT_WITH_GUID:
        {
            if (!sObjectMgr.GetGOData(targetParam1))
            {
                if (!sObjectMgr.IsExistingGameObjectGuid(targetParam1))
                    sLog.outErrorDb("Table `%s` has target_param1 = %u for id %u, but this gameobject guid does not exist.", tableName, targetParam1, tableEntry);
                return false;

            }
            if (targetParam2)
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but this parameter is unused.", tableName, targetParam2, targetType, tableEntry);
            break;
        }
        case TARGET_T_GAMEOBJECT_FROM_INSTANCE_DATA:
        case TARGET_T_FRIENDLY_CC:
        case TARGET_T_NEAREST_PLAYER:
        case TARGET_T_NEAREST_HOSTILE_PLAYER:
        case TARGET_T_NEAREST_FRIENDLY_PLAYER:
        {
            if (targetParam2)
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but this parameter is unused.", tableName, targetParam2, targetType, tableEntry);
            break;
        }
        case TARGET_T_FRIENDLY:
        {
            if (targetParam2 > 1)
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but a bool value is expected.", tableName, targetParam2, targetType, tableEntry);
            break;
        }
        case TARGET_T_FRIENDLY_INJURED:
        case TARGET_T_FRIENDLY_INJURED_EXCEPT:
        {
            if (targetParam2 > 100)
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but a percent value is expected.", tableName, targetParam2, targetType, tableEntry);
            break;
        }
        case TARGET_T_FRIENDLY_MISSING_BUFF:
        case TARGET_T_FRIENDLY_MISSING_BUFF_EXCEPT:
        {
            if (targetParam2 && !sSpellMgr.GetSpellEntry(targetParam2))
            {
                if (!sSpellMgr.IsExistingSpellId(targetParam2))
                    sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but spell does not exist.", tableName, targetParam2, targetType, tableEntry);

                return false;
            }
            break;
        }
        case TARGET_T_MAP_EVENT_SOURCE:
        case TARGET_T_MAP_EVENT_TARGET:
        {
            if (!targetParam1)
            {
                sLog.outErrorDb("Table `%s` has target_param1 = %u with target_type = %u for id %u, but a scripted map event id is expected.", tableName, targetParam1, targetType, tableEntry);
                return false;
            }
            if (targetParam2)
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but this parameter is unused.", tableName, targetParam2, targetType, tableEntry);
            break;
        }
        case TARGET_T_MAP_EVENT_EXTRA_TARGET:
        {
            if (!targetParam1)
            {
                sLog.outErrorDb("Table `%s` has target_param1 = %u with target_type = %u for id %u, but a scripted map event id is expected.", tableName, targetParam1, targetType, tableEntry);
                return false;
            }
            if (!targetParam2)
            {
                sLog.outErrorDb("Table `%s` has target_param2 = %u with target_type = %u for id %u, but a creature or gameobject entry is expected.", tableName, targetParam2, targetType, tableEntry);
                return false;
            }
            break;
        }
        default:
        {
            if (targetType >= TARGET_T_END)
                sLog.outError("Table `%s` has an unknown target_type = %u used for id %u.", tableName, targetType, tableEntry);
            break;
        }
    }

    return true;
}

void ScriptMgr::LoadGameObjectScripts()
{
    LoadScripts(sGameObjectScripts, "gameobject_scripts");

    // check ids
    for (const auto& itr : sGameObjectScripts)
    {
        if (!sObjectMgr.GetGOData(itr.first))
            if (!sObjectMgr.IsExistingGameObjectGuid(itr.first))
                sLog.outErrorDb("Table `gameobject_scripts` has not existing gameobject (GUID: %u) as script id", itr.first);
    }
}

void ScriptMgr::LoadQuestEndScripts()
{
    LoadScripts(sQuestEndScripts, "quest_end_scripts");

    // check ids
    for (const auto& itr : sQuestEndScripts)
    {
        if (!sObjectMgr.GetQuestTemplate(itr.first) && !sObjectMgr.IsExistingQuestId(itr.first))
            sLog.outErrorDb("Table `quest_end_scripts` has not existing quest (Id: %u) as script id", itr.first);
    }
}

void ScriptMgr::LoadQuestStartScripts()
{
    LoadScripts(sQuestStartScripts, "quest_start_scripts");

    // check ids
    for (const auto& itr : sQuestStartScripts)
    {
        if (!sObjectMgr.GetQuestTemplate(itr.first) && !sObjectMgr.IsExistingQuestId(itr.first))
            sLog.outErrorDb("Table `quest_start_scripts` has not existing quest (Id: %u) as script id", itr.first);
    }
}

void ScriptMgr::LoadSpellScripts()
{
    LoadScripts(sSpellScripts, "spell_scripts");

    // check ids
    for (const auto& itr : sSpellScripts)
    {
        SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(itr.first);

        if (!spellInfo)
        {
            if (!sSpellMgr.IsExistingSpellId(itr.first))
                sLog.outErrorDb("Table `spell_scripts` has not existing spell (Id: %u) as script id", itr.first);
            continue;
        }

        //check for correct spellEffect
        bool found = false;
        for (uint32 i : spellInfo->Effect)
        {
            // skip empty effects
            if (!i)
                continue;

            if (i == SPELL_EFFECT_SCRIPT_EFFECT)
            {
                found =  true;
                break;
            }
        }

        if (!found)
            sLog.outErrorDb("Table `spell_scripts` has unsupported spell (Id: %u) without SPELL_EFFECT_SCRIPT_EFFECT (%u) spell effect", itr.first, SPELL_EFFECT_SCRIPT_EFFECT);
    }
}

void ScriptMgr::LoadGenericScripts()
{
    LoadScripts(sGenericScripts, "generic_scripts");

    std::set<uint32> genericIds;                            // Store possible event ids
    CollectPossibleGenericIds(genericIds);

    // Then check if all scripts are in above list of possible script entries.
    for (const auto& itr : sGenericScripts)
    {
        auto const itr2 = genericIds.find(itr.first);
        if (itr2 == genericIds.end())
            sLog.outErrorDb("Table `generic_scripts` has script (Id: %u) not referenced from anywhere", itr.first);
    }

    // Check if any generic script ids are missing.
    for (uint32 id : genericIds)
    {
        auto const itr = sGenericScripts.find(id);
        if (itr == sGenericScripts.end())
            sLog.outErrorDb("Table `generic_scripts` does not contain script (Id: %u) referenced from another script", id);
    }
}

void ScriptMgr::LoadEventScripts()
{
    LoadScripts(sEventScripts, "event_scripts");

    std::set<uint32> eventIds;                              // Store possible event ids

    CollectPossibleEventIds(eventIds);

    // Then check if all scripts are in above list of possible script entries
    for (const auto& itr : sEventScripts)
    {
        std::set<uint32>::const_iterator itr2 = eventIds.find(itr.first);
        if (itr2 == eventIds.end())
            sLog.outErrorDb("Table `event_scripts` has script (Id: %u) not referring to any gameobject_template type 10 data2 field, type 3 data6 field, type 13 data 2 field, type 29 or any spell effect %u",
                itr.first, SPELL_EFFECT_SEND_EVENT);
    }
}

void ScriptMgr::LoadCreatureSpellScripts()
{
    LoadScripts(sCreatureSpellScripts, "creature_spells_scripts");

    // checks are done in LoadCreatureSpells
}

void ScriptMgr::LoadGossipScripts()
{
    LoadScripts(sGossipScripts, "gossip_scripts");

    // checks are done in LoadGossipMenuItems
}

void ScriptMgr::LoadCreatureMovementScripts()
{
    LoadScripts(sCreatureMovementScripts, "creature_movement_scripts");

    // checks are done in WaypointManager::Load
}

void ScriptMgr::LoadCreatureEventAIScripts()
{
    LoadScripts(sCreatureAIScripts, "creature_ai_scripts");

    
    QueryResult* result;
    Field* fields;

    // Check for scripts with delay, which is not supported for this table.
    for (uint8 i = 1; i <= 3; i++)
    {
        result = WorldDatabase.Query("SELECT DISTINCT id FROM creature_ai_scripts WHERE delay != 0");

        if (result)
        {
            do
            {
                fields = result->Fetch();
                uint32 scriptId = fields[0].GetUInt32();
                sLog.outErrorDb("Table `creature_ai_scripts` has script (Id: %u) with delay!=0 but this is not supported for creature AI events.", scriptId);
            } while (result->NextRow());
            delete result;
        }
    }

    // Get all script Ids referenced in creature_ai_events table.
    std::set<uint32> actionIds;
    for (uint8 i = 1; i <= 3; i++)
    {
        result = WorldDatabase.PQuery("SELECT action%u_script FROM creature_ai_events", i);

        if (result)
        {
            do
            {
                fields = result->Fetch();
                uint32 scriptId = fields[0].GetUInt32();
                if (scriptId)
                    actionIds.insert(scriptId);
            } while (result->NextRow());
            delete result;
        }
    }

    // Then check if all scripts are in above list of used script Ids.
    for (const auto& itr : sCreatureAIScripts)
    {
        std::set<uint32>::const_iterator itr2 = actionIds.find(itr.first);
        if (itr2 == actionIds.end())
            sLog.outErrorDb("Table `creature_ai_scripts` has script (Id: %u) not used by any creature AI events.", itr.first);
    }
}

void ScriptMgr::CheckAllScriptTexts()
{
    CheckScriptTexts(sQuestEndScripts);
    CheckScriptTexts(sQuestStartScripts);
    CheckScriptTexts(sSpellScripts);
    CheckScriptTexts(sCreatureSpellScripts);
    CheckScriptTexts(sGameObjectScripts);
    CheckScriptTexts(sEventScripts);
    CheckScriptTexts(sGenericScripts);
    CheckScriptTexts(sGossipScripts);
    CheckScriptTexts(sCreatureMovementScripts);
    CheckScriptTexts(sCreatureAIScripts);
}

void ScriptMgr::CheckScriptTexts(ScriptMapMap const& scripts)
{
    for (const auto& script : scripts)
    {
        for (ScriptMap::const_iterator itrM = script.second.begin(); itrM != script.second.end(); ++itrM)
        {
            if (itrM->second.command == SCRIPT_COMMAND_TALK)
            {
                for (int i : itrM->second.talk.textId)
                {
                    if (i > 0 && !sObjectMgr.GetBroadcastTextLocale(i))
                        sLog.outErrorDb("Table `broadcast_text` is missing text id %i, used in database script id %u.", i, script.first);
                    else if (i < 0 && !GetTextData(i))
                        sLog.outErrorDb("Table `script_texts` is missing text id %i, used in database script id %u.", i, script.first);
                }
            }
        }
    }
}

void ScriptMgr::LoadAreaTriggerScripts()
{
    m_AreaTriggerScripts.clear();                           // need for reload case
    QueryResult *result = WorldDatabase.Query("SELECT entry, script_name FROM scripted_areatrigger");

    if (!result)
    {
        return;
    }

    do
    {
        Field *fields = result->Fetch();

        uint32 triggerId       = fields[0].GetUInt32();
        const char *scriptName = fields[1].GetString();

        if (!sObjectMgr.GetAreaTrigger(triggerId))
        {
            if (!sObjectMgr.IsExistingAreaTriggerId(triggerId))
                sLog.outErrorDb("Table `scripted_areatrigger` has area trigger (ID: %u) not listed in `AreaTrigger.dbc`.", triggerId);
            continue;
        }

        m_AreaTriggerScripts[triggerId] = GetScriptId(scriptName);
    }
    while (result->NextRow());

    delete result;
}

void ScriptMgr::LoadEventIdScripts()
{
    m_EventIdScripts.clear();                           // need for reload case
    QueryResult *result = WorldDatabase.Query("SELECT id, script_name FROM scripted_event_id");

    if (!result)
    {
        return;
    }

    std::set<uint32> eventIds;                              // Store possible event ids
    CollectPossibleEventIds(eventIds);

    do
    {
        Field *fields = result->Fetch();

        uint32 eventId          = fields[0].GetUInt32();
        const char *scriptName  = fields[1].GetString();

        std::set<uint32>::const_iterator itr = eventIds.find(eventId);
        if (itr == eventIds.end())
            sLog.outErrorDb("Table `scripted_event_id` has id %u not referring to any gameobject_template type 10 data2 field, type 3 data6 field, type 13 data 2 field, type 29 or any spell effect %u or path taxi node data",
                            eventId, SPELL_EFFECT_SEND_EVENT);

        m_EventIdScripts[eventId] = GetScriptId(scriptName);
    }
    while (result->NextRow());

    delete result;
}

void ScriptMgr::LoadScriptNames()
{
    m_scriptNames.emplace_back("");

    char const* tableNames[] =
    {
        "creature_template",
        "gameobject_template",
        "item_template",
        "spell_template",
        "scripted_areatrigger",
        "scripted_event_id",
        "map_template",
    };

    for (char const* tableName : tableNames)
    {
        QueryResult* result = WorldDatabase.PQuery("SELECT DISTINCT(script_name) FROM %s WHERE script_name <> ''", tableName);
        if (!result)
            continue;

        do
        {
            m_scriptNames.emplace_back((*result)[0].GetString());
        }
        while (result->NextRow());

        delete result;
    }

    // Scripts that ship in this binary but whose creature_template.script_name
    // rows live in world-DB updates a realm may not have applied. Without the
    // name here RegisterSelf() reports "not assigned in database" and the
    // script is dropped; with it the script registers and LoadCreatureTemplates
    // binds it to the entry when the DB row carries no script (see there).
    // Zul Farrak Farraki Arena, update 20260626153218 (2026-09-05).
    static char const* const kFallbackScriptNames[] =
    {
        "npc_champion_razjal_the_quick",
        "npc_kathzen_the_brutal",
        "npc_juthza_the_cunning",
    };
    for (char const* fallback : kFallbackScriptNames)
        m_scriptNames.emplace_back(fallback);

    std::sort(m_scriptNames.begin(), m_scriptNames.end());
    m_scriptNames.erase(std::unique(m_scriptNames.begin(), m_scriptNames.end()), m_scriptNames.end());
}

uint32 ScriptMgr::GetScriptId(const char *name) const
{
    // use binary search to find the script name in the sorted vector
    // assume "" is the first element
    if (!name)
        return 0;

    ScriptNameMap::const_iterator itr =
        std::lower_bound(m_scriptNames.begin(), m_scriptNames.end(), name);

    if (itr == m_scriptNames.end() || *itr != name)
        return 0;

    return uint32(itr - m_scriptNames.begin());
}

uint32 ScriptMgr::GetAreaTriggerScriptId(uint32 triggerId) const
{
    AreaTriggerScriptMap::const_iterator itr = m_AreaTriggerScripts.find(triggerId);
    if (itr != m_AreaTriggerScripts.end())
        return itr->second;

    return 0;
}

uint32 ScriptMgr::GetEventIdScriptId(uint32 eventId) const
{
    EventIdScriptMap::const_iterator itr = m_EventIdScripts.find(eventId);
    if (itr != m_EventIdScripts.end())
        return itr->second;

    return 0;
}

CreatureAI* ScriptMgr::GetCreatureAI(Creature* pCreature)
{
    Script* pTempScript = m_NPC_scripts[pCreature->GetScriptId()];

    if (!pTempScript || !pTempScript->GetAI)
    {
        if (CreatureScript* script = ScriptRegistry<CreatureScript>::GetScriptById(pCreature->GetScriptId()))
            if (CreatureAI* ai = script->GetAI(pCreature))
                return ai;

        CreatureAI* ai = nullptr;
        ScriptRegistry<AllCreatureScript>::ForEachWithReturn([&](AllCreatureScript* script)
        {
            ai = script->GetCreatureAI(pCreature);
            return ai != nullptr;
        });

#ifdef ENABLE_ELUNA
        if (!ai)
            if (Eluna* e = pCreature->GetEluna())
                ai = e->GetAI(pCreature);
#endif

        return ai;
    }

    return pTempScript->GetAI(pCreature);
}

GameObjectAI* ScriptMgr::GetGameObjectAI(GameObject* pGobj)
{
    Script* pTempScript = m_NPC_scripts[pGobj->GetGOInfo()->ScriptId];

    if (!pTempScript || !pTempScript->GOGetAI)
    {
        if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGobj->GetGOInfo()->ScriptId))
            if (GameObjectAI* ai = script->GetAI(pGobj))
                return ai;

        GameObjectAI* ai = nullptr;
        ScriptRegistry<AllGameObjectScript>::ForEachWithReturn([&](AllGameObjectScript* script)
        {
            ai = script->GetGameObjectAI(pGobj);
            return ai != nullptr;
        });

        return ai;
    }

    return pTempScript->GOGetAI(pGobj);
}

InstanceData* ScriptMgr::CreateInstanceData(Map* pMap)
{
    Script* pTempScript = m_NPC_scripts[pMap->GetScriptId()];

    if (!pTempScript || !pTempScript->GetInstanceData)
    {
        if (InstanceMapScript* script = ScriptRegistry<InstanceMapScript>::GetScriptById(pMap->GetScriptId()))
            return script->GetInstanceData(pMap);

        return nullptr;
    }

    return pTempScript->GetInstanceData(pMap);
}

SpellScript* ScriptMgr::GetSpellScript(SpellEntry const* pSpell)
{
    if (!pSpell->ScriptId || pSpell->ScriptId >= m_NPC_scripts.size())
        return nullptr;

    Script* pTempScript = m_NPC_scripts[pSpell->ScriptId];

    if (!pTempScript || !pTempScript->GetSpellScript)
    {
        if (SpellScriptLoader* script = ScriptRegistry<SpellScriptLoader>::GetScriptById(pSpell->ScriptId))
            return script->GetSpellScript();

        return nullptr;
    }

    return pTempScript->GetSpellScript(pSpell);
}

AuraScript* ScriptMgr::GetAuraScript(SpellEntry const* pSpell)
{
    if (!pSpell->ScriptId || pSpell->ScriptId >= m_NPC_scripts.size())
        return nullptr;

    Script* pTempScript = m_NPC_scripts[pSpell->ScriptId];

    if (!pTempScript || !pTempScript->GetAuraScript)
    {
        if (SpellScriptLoader* script = ScriptRegistry<SpellScriptLoader>::GetScriptById(pSpell->ScriptId))
            return script->GetAuraScript();

        return nullptr;
    }

    return pTempScript->GetAuraScript(pSpell);
}

bool ScriptMgr::IsBotManaged(Player* who)
{
    bool managed = false;
    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_IS_MANAGED_BOT,
        [&](PlayerScript* script) { if (script->IsManagedBot(who)) managed = true; });
    return managed;
}

uint8 ScriptMgr::GetBotRoles(Player* who)
{
    uint8 roles = 0;
    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_GET_BOT_ROLES,
        [&](PlayerScript* script) { roles = uint8(roles | script->GetBotRoles(who)); });
    return roles;
}

bool ScriptMgr::OnAddonMessage(Player* from, std::string const& msg)
{
    bool consumed = false;
    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_ADDON_MESSAGE,
        [&](PlayerScript* script) { if (!consumed && script->OnAddonMessage(from, msg)) consumed = true; });
    return consumed;
}

bool ScriptMgr::OnGossipHello(Player* pPlayer, Creature* pCreature)
{
    Script* pTempScript = m_NPC_scripts[pCreature->GetScriptId()];

    if (pTempScript && pTempScript->pGossipHello)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (pTempScript->pGossipHello(pPlayer, pCreature))
            return true;
    }

    if (CreatureScript* script = ScriptRegistry<CreatureScript>::GetScriptById(pCreature->GetScriptId()))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (script->OnGossipHello(pPlayer, pCreature))
            return true;
    }

    if (ScriptRegistry<AllCreatureScript>::ForEachWithReturn([&](AllCreatureScript* script)
    {
        return script->CanCreatureGossipHello(pPlayer, pCreature);
    }))
        return true;

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        return e->OnGossipHello(pPlayer, pCreature);
#endif

    return false;
}

bool ScriptMgr::OnGossipHello(Player* pPlayer, GameObject* pGameObject)
{
    Script* pTempScript = m_NPC_scripts[pGameObject->GetGOInfo()->ScriptId];

    if (pTempScript && pTempScript->pGOGossipHello)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (pTempScript->pGOGossipHello(pPlayer, pGameObject))
            return true;
    }

    if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGameObject->GetGOInfo()->ScriptId))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (script->OnGossipHello(pPlayer, pGameObject))
            return true;
    }

    if (ScriptRegistry<AllGameObjectScript>::ForEachWithReturn([&](AllGameObjectScript* script)
    {
        return script->CanGameObjectGossipHello(pPlayer, pGameObject);
    }))
        return true;

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        return e->OnGossipHello(pPlayer, pGameObject);
#endif

    return false;
}

bool ScriptMgr::OnGossipSelect(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action, const char* code)
{
    sLog.outDebug("Gossip selection%s, sender: %d, action: %d", code ? " with code" : "", sender, action);

    Script* pTempScript = m_NPC_scripts[pCreature->GetScriptId()];

    if (code)
    {
        if (pTempScript && pTempScript->pGossipSelectWithCode)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            if (pTempScript->pGossipSelectWithCode(pPlayer, pCreature, sender, action, code))
                return true;
        }

        if (CreatureScript* script = ScriptRegistry<CreatureScript>::GetScriptById(pCreature->GetScriptId()))
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            return script->OnGossipSelectCode(pPlayer, pCreature, sender, action, code);
        }
    }
    else
    {
        if (pTempScript && pTempScript->pGossipSelect)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            if (pTempScript->pGossipSelect(pPlayer, pCreature, sender, action))
                return true;
        }

        if (CreatureScript* script = ScriptRegistry<CreatureScript>::GetScriptById(pCreature->GetScriptId()))
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            return script->OnGossipSelect(pPlayer, pCreature, sender, action);
        }
    }

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        return code ? e->OnGossipSelectCode(pPlayer, pCreature, sender, action, code)
                    : e->OnGossipSelect(pPlayer, pCreature, sender, action);
#endif

    return false;
}

bool ScriptMgr::OnGossipSelect(Player* pPlayer, GameObject* pGameObject, uint32 sender, uint32 action, const char* code)
{
    sLog.outDebug("Gossip selection%s, sender: %d, action: %d", code ? " with code" : "", sender, action);

    Script* pTempScript = m_NPC_scripts[pGameObject->GetGOInfo()->ScriptId];

    if (code)
    {
        if (pTempScript && pTempScript->pGOGossipSelectWithCode)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            if (pTempScript->pGOGossipSelectWithCode(pPlayer, pGameObject, sender, action, code))
                return true;
        }

        if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGameObject->GetGOInfo()->ScriptId))
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            return script->OnGossipSelectCode(pPlayer, pGameObject, sender, action, code);
        }
    }
    else
    {
        if (pTempScript && pTempScript->pGOGossipSelect)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            if (pTempScript->pGOGossipSelect(pPlayer, pGameObject, sender, action))
                return true;
        }

        if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGameObject->GetGOInfo()->ScriptId))
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            return script->OnGossipSelect(pPlayer, pGameObject, sender, action);
        }
    }

    if (ScriptRegistry<AllGameObjectScript>::ForEachWithReturn([&](AllGameObjectScript* script)
    {
        return script->CanGameObjectGossipSelect(pPlayer, pGameObject, sender, action, code);
    }))
        return true;

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        return code ? e->OnGossipSelectCode(pPlayer, pGameObject, sender, action, code)
                    : e->OnGossipSelect(pPlayer, pGameObject, sender, action);
#endif

    return false;
}

bool ScriptMgr::OnQuestAccept(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    //quest scripts have higher priority
    if (OnQuestAcceptByScript(pPlayer, pQuest)) return true;

    Script* pTempScript = m_NPC_scripts[pCreature->GetScriptId()];

    if (pTempScript && pTempScript->pQuestAcceptNPC)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (pTempScript->pQuestAcceptNPC(pPlayer, pCreature, pQuest))
            return true;
    }

    if (CreatureScript* script = ScriptRegistry<CreatureScript>::GetScriptById(pCreature->GetScriptId()))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return script->OnQuestAccept(pPlayer, pCreature, pQuest);
    }

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        return e->OnQuestAccept(pPlayer, pCreature, pQuest);
#endif

    return false;
}

bool ScriptMgr::OnQuestAccept(Player* pPlayer, GameObject* pGameObject, Quest const* pQuest)
{
    //quest scripts have higher priority
    if (OnQuestAcceptByScript(pPlayer, pQuest)) return true;

    Script* pTempScript = m_NPC_scripts[pGameObject->GetGOInfo()->ScriptId];

    if (pTempScript && pTempScript->pGOQuestAccept)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (pTempScript->pGOQuestAccept(pPlayer, pGameObject, pQuest))
            return true;
    }

    if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGameObject->GetGOInfo()->ScriptId))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return script->OnQuestAccept(pPlayer, pGameObject, pQuest);
    }

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        return e->OnQuestAccept(pPlayer, pGameObject, pQuest);
#endif

    return false;
}

bool ScriptMgr::OnQuestAccept(Player* pPlayer, Item* pItem, Quest const* pQuest)
{
    //quest scripts have higher priority
    if (OnQuestAcceptByScript(pPlayer, pQuest)) return true;

    Script* pTempScript = m_NPC_scripts[pItem->GetProto()->ScriptId];

    if (pTempScript && pTempScript->pItemHello)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (pTempScript->pItemHello(pPlayer, pItem, pQuest))
            return true;
    }

    if (ItemScript* script = ScriptRegistry<ItemScript>::GetScriptById(pItem->GetProto()->ScriptId))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return script->OnQuestAccept(pPlayer, pItem, pQuest);
    }

    return ScriptRegistry<AllItemScript>::ForEachWithReturn([&](AllItemScript* script)
    {
        return script->CanItemQuestAccept(pPlayer, pItem, pQuest);
    });
}

bool ScriptMgr::OnQuestAcceptByScript(Player* pPlayer, Quest const* pQuest)
{
    auto questScriptIter = m_questScripts.find(pQuest->GetQuestId());
    if (questScriptIter != m_questScripts.end())
    {
        Script* pQuestScript = questScriptIter->second;

        if (pQuestScript->GetQuestInstance == nullptr) return false;
        RegisterQuestInstance(pQuestScript, pPlayer);

        return true;
    }
    return false;
}

bool ScriptMgr::OnQuestRewardedByScript(Player* pPlayer, Quest const* pQuest)
{
    uint32 questID = pQuest->GetQuestId();

    std::pair<QuestInstanceMultiMap::iterator, QuestInstanceMultiMap::iterator> QuestInstancesByPlayer = m_questInstancies.equal_range(pPlayer->GetObjectGuid());
    
    for (QuestInstanceMultiMap::iterator pQuestIter = QuestInstancesByPlayer.first; pQuestIter != QuestInstancesByPlayer.second; pQuestIter++)
    {
        auto& Elem = *pQuestIter;
        if (Elem.second->GetQuestId() == questID)
        {
            Elem.second->OnQuestFinished();
            m_questInstancies.erase(pQuestIter);
            return true;
        }
    }

    return false;
}

void ScriptMgr::RegisterQuestInstance(Script* pQuestScript, Player* pPlayer)
{
    QuestInstance* pNewInstance = pQuestScript->GetQuestInstance(pPlayer->GetObjectGuid());
    m_questInstancies.emplace(std::pair < ObjectGuid, std::shared_ptr < QuestInstance > >(pPlayer->GetObjectGuid(), std::shared_ptr<QuestInstance>(pNewInstance)));
    pNewInstance->OnQuestStarted();
}

bool ScriptMgr::OnQuestCanceled(Player* pPlayer, uint32 questID)
{
    std::pair<QuestInstanceMultiMap::iterator, QuestInstanceMultiMap::iterator> QuestInstancesByPlayer = m_questInstancies.equal_range(pPlayer->GetObjectGuid());

    for (QuestInstanceMultiMap::iterator pQuestIter = QuestInstancesByPlayer.first; pQuestIter != QuestInstancesByPlayer.second; pQuestIter++)
    {
        auto& Elem = *pQuestIter;
        if (Elem.second->GetQuestId() == questID)
        {
            Elem.second->OnQuestCanceled();
            m_questInstancies.erase(pQuestIter);
            return true;
        }
    }

    return false;
}

bool ScriptMgr::OnQuestRewarded(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (OnQuestRewardedByScript(pPlayer, pQuest)) return true;

    Script* pTempScript = m_NPC_scripts[pCreature->GetScriptId()];

    if (pTempScript && pTempScript->pQuestRewardedNPC)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (pTempScript->pQuestRewardedNPC(pPlayer, pCreature, pQuest))
            return true;
    }

    if (CreatureScript* script = ScriptRegistry<CreatureScript>::GetScriptById(pCreature->GetScriptId()))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return script->OnQuestReward(pPlayer, pCreature, pQuest, 0);
    }

    return false;
}

bool ScriptMgr::OnQuestRewarded(Player* pPlayer, GameObject* pGameObject, Quest const* pQuest)
{
    if (OnQuestRewardedByScript(pPlayer, pQuest)) return true;

    Script* pTempScript = m_NPC_scripts[pGameObject->GetGOInfo()->ScriptId];

    if (pTempScript && pTempScript->pQuestRewardedGO)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (pTempScript->pQuestRewardedGO(pPlayer, pGameObject, pQuest))
            return true;
    }

    if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGameObject->GetGOInfo()->ScriptId))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return script->OnQuestReward(pPlayer, pGameObject, pQuest, 0);
    }

    return false;
}

uint32 ScriptMgr::GetDialogStatus(Player* pPlayer, Creature* pCreature)
{
    Script* pTempScript = m_NPC_scripts[pCreature->GetScriptId()];

    if (pTempScript && pTempScript->pNPCDialogStatus)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return pTempScript->pNPCDialogStatus(pPlayer, pCreature);
    }

    if (CreatureScript* script = ScriptRegistry<CreatureScript>::GetScriptById(pCreature->GetScriptId()))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return script->GetDialogStatus(pPlayer, pCreature);
    }

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        e->GetDialogStatus(pPlayer, pCreature);
#endif

    return DIALOG_STATUS_UNDEFINED;
}

uint32 ScriptMgr::GetDialogStatus(Player* pPlayer, GameObject* pGameObject)
{
    Script* pTempScript = m_NPC_scripts[pGameObject->GetGOInfo()->ScriptId];

    if (pTempScript && pTempScript->pGODialogStatus)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return pTempScript->pGODialogStatus(pPlayer, pGameObject);
    }

    if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGameObject->GetGOInfo()->ScriptId))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return script->GetDialogStatus(pPlayer, pGameObject);
    }

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        e->GetDialogStatus(pPlayer, pGameObject);
#endif

    return DIALOG_STATUS_UNDEFINED;
}

bool ScriptMgr::OnGameObjectOpen(Player* pPlayer, GameObject* pGameObject)
{
    Script* pTempScript = m_NPC_scripts[pGameObject->GetGOInfo()->ScriptId];

    if (pTempScript && pTempScript->GOOpen && pTempScript->GOOpen(pPlayer, pGameObject))
        return true;

    if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGameObject->GetGOInfo()->ScriptId))
        return script->OnGossipHello(pPlayer, pGameObject);

    return false;
}

bool ScriptMgr::OnGameObjectUse(Player* pPlayer, GameObject* pGameObject)
{
    Script* pTempScript = m_NPC_scripts[pGameObject->GetGOInfo()->ScriptId];

    if (pTempScript && pTempScript->pGOHello)
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        if (pTempScript->pGOHello(pPlayer, pGameObject))
            return true;
    }

    if (GameObjectScript* script = ScriptRegistry<GameObjectScript>::GetScriptById(pGameObject->GetGOInfo()->ScriptId))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        return script->OnGossipHello(pPlayer, pGameObject);
    }

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        return e->OnGameObjectUse(pPlayer, pGameObject);
#endif

    return false;
}

bool ScriptMgr::OnItemUse(Player* pPlayer, Item* pItem, SpellCastTargets& targets)
{
    Script* pTempScript = m_NPC_scripts[pItem->GetProto()->ScriptId];

    if (ScriptRegistry<AllItemScript>::ForEachWithReturn([&](AllItemScript* script)
    {
        return script->CanItemUse(pPlayer, pItem, targets);
    }))
        return true;

	if (pTempScript && pTempScript->pItemUse && pTempScript->pItemUse(pPlayer, pItem, targets))
		return true;

    if (ItemScript* script = ScriptRegistry<ItemScript>::GetScriptById(pItem->GetProto()->ScriptId))
        return script->OnUse(pPlayer, pItem, targets);

	return false;
}

bool ScriptMgr::OnItemUseSpell(Player* pPlayer, Item* pItem, SpellCastTargets const& targets)
{
    Script* pTempScript = m_NPC_scripts[pItem->GetProto()->ScriptId];

    if (pTempScript && pTempScript->pItemUseSpell && pTempScript->pItemUseSpell(pPlayer, pItem, targets))
        return true;

    if (ItemScript* script = ScriptRegistry<ItemScript>::GetScriptById(pItem->GetProto()->ScriptId))
        return script->OnUseSpell(pPlayer, pItem, targets);

    return false;
}

bool ScriptMgr::OnAreaTrigger(Player* pPlayer, AreaTriggerEntry const* atEntry)
{
    Script* pTempScript = m_NPC_scripts[GetAreaTriggerScriptId(atEntry->id)];

    if (pTempScript && pTempScript->pAreaTrigger && pTempScript->pAreaTrigger(pPlayer, atEntry))
        return true;

    if (AreaTriggerScript* script = ScriptRegistry<AreaTriggerScript>::GetScriptById(GetAreaTriggerScriptId(atEntry->id)))
        return script->OnTrigger(pPlayer, atEntry);

#ifdef ENABLE_ELUNA
    if (Eluna* e = pPlayer->GetEluna())
        return e->OnAreaTrigger(pPlayer, atEntry);
#endif

    return false;
}

bool ScriptMgr::OnProcessEvent(uint32 eventId, Object* pSource, Object* pTarget, bool isStart)
{
    Script* pTempScript = m_NPC_scripts[GetEventIdScriptId(eventId)];

    if (pTempScript && pTempScript->pProcessEventId)
        return pTempScript->pProcessEventId(eventId, pSource, pTarget, isStart);

    // isStart may be false, when event is from taxi node events (arrival=false, departure=true)
    if (GameEventScript* script = ScriptRegistry<GameEventScript>::GetScriptById(GetEventIdScriptId(eventId)))
    {
        if (isStart)
            script->OnStart(eventId);
        else
            script->OnStop(eventId);

        // Only a database-bound script HANDLES the event. A non-bound one is an
        // observer - the Eluna bridge sits at registry id 0, which is exactly
        // what GetEventIdScriptId() returns for every event WITHOUT a
        // scripted_event_id row - and reporting "handled" here kept the DB
        // event_scripts from ever running: Zul Farrak gong 141832, event 2488,
        // Gahzrilla never summoned (2026-09-05).
        if (script->IsDatabaseBound())
            return true;
    }

    return false;
}

bool ScriptMgr::OnEffectDummy(WorldObject* pCaster, uint32 spellId, SpellEffectIndex effIndex, Creature* pTarget)
{
    Script* pTempScript = m_NPC_scripts[pTarget->GetScriptId()];

#ifdef ENABLE_ELUNA
    if (Eluna* e = pCaster->GetEluna())
        e->OnDummyEffect(pCaster, spellId, effIndex, pTarget);
#endif

    if (pTempScript && pTempScript->pEffectDummyCreature && pTempScript->pEffectDummyCreature(pCaster, spellId, effIndex, pTarget))
        return true;

    return false;
}

bool ScriptMgr::OnEffectDummy(WorldObject* pCaster, uint32 spellId, SpellEffectIndex effIndex, GameObject* pTarget)
{
    Script* pTempScript = m_NPC_scripts[pTarget->GetGOInfo()->ScriptId];

#ifdef ENABLE_ELUNA
    if (Eluna* e = pCaster->GetEluna())
        e->OnDummyEffect(pCaster, spellId, effIndex, pTarget);
#endif

    if (pTempScript && pTempScript->pEffectDummyGameObj && pTempScript->pEffectDummyGameObj(pCaster, spellId, effIndex, pTarget))
        return true;

    return false;
}

bool ScriptMgr::OnAuraDummy(Aura const* pAura, bool apply)
{
    Script* pTempScript = m_NPC_scripts[((Creature*)pAura->GetTarget())->GetScriptId()];

    if (pTempScript && pTempScript->pEffectAuraDummy && pTempScript->pEffectAuraDummy(pAura, apply))
        return true;

    return false;
}

uint32 GetAreaTriggerScriptId(uint32 triggerId)
{
    return sScriptMgr.GetAreaTriggerScriptId(triggerId);
}

uint32 GetEventIdScriptId(uint32 eventId)
{
    return sScriptMgr.GetEventIdScriptId(eventId);
}

uint32 GetScriptId(const char *name)
{
    return sScriptMgr.GetScriptId(name);
}

char const* GetScriptName(uint32 id)
{
    return sScriptMgr.GetScriptName(id);
}

uint32 GetScriptIdsCount()
{
    return sScriptMgr.GetScriptIdsCount();
}

void ScriptMgr::Initialize()
{
    // Load database (must be called after SD2Config.SetSource).
    LoadDatabase();
    
    // Resize script ids to needed ammount of assigned ScriptNames (from core)
    m_NPC_scripts.resize(GetScriptIdsCount(), nullptr);

    if (!m_scriptLoaderCallback)
    {
        sLog.outError("Script loader callback was not registered.");
        return;
    }

    if (!m_modulesLoaderCallback)
    {
        sLog.outError("Modules loader callback was not registered.");
        return;
    }

    m_scriptLoaderCallback();
    m_modulesLoaderCallback();
    AddScriptObjectRegistriesAfterDatabaseLoad();

    // Check existance scripts for all registered by core script names
    for (uint32 i = 1; i < GetScriptIdsCount(); ++i)
    {
        if (!m_NPC_scripts[i])
            sLog.outError("Script not found: %s.", GetScriptName(i));
    }

}

void ScriptMgr::LoadDatabase()
{
    LoadScriptTexts();
    LoadScriptWaypoints();
    LoadEscortData();
}

void ScriptMgr::LoadScriptTexts()
{
    sObjectMgr.LoadMangosStrings(WorldDatabase, "script_texts", TEXT_SOURCE_TEXT_START, TEXT_SOURCE_TEXT_END, true);

    QueryResult* result = WorldDatabase.PQuery("SELECT entry, sound, type, language, emote FROM script_texts WHERE entry BETWEEN %i AND %i", TEXT_SOURCE_TEXT_END, TEXT_SOURCE_TEXT_START);

    if (result)
    {
        do
        {
            Field* pFields = result->Fetch();
            StringTextData pTemp;

            int32 iId           = pFields[0].GetInt32();
            pTemp.SoundId     = pFields[1].GetUInt32();
            pTemp.Type        = pFields[2].GetUInt32();
            pTemp.Language    = pFields[3].GetUInt32();
            pTemp.Emote       = pFields[4].GetUInt32();

            if (iId >= 0)
            {
                sLog.outErrorDb("Entry %i in table `script_texts` is not a negative value.", iId);
                continue;
            }

            if (pTemp.SoundId)
            {
                if (!sObjectMgr.GetSoundEntry(pTemp.SoundId))
                    sLog.outErrorDb("Entry %i in table `script_texts` has soundId %u but sound does not exist.", iId, pTemp.SoundId);
            }

            if (!GetLanguageDescByID(pTemp.Language))
                sLog.outErrorDb("Entry %i in table `script_texts` using Language %u but Language does not exist.", iId, pTemp.Language);

            if (pTemp.Type > CHAT_TYPE_ZONE_YELL)
                sLog.outErrorDb("Entry %i in table `script_texts` has Type %u but this Chat Type does not exist.", iId, pTemp.Type);

            m_mTextDataMap[iId] = pTemp;
        } while (result->NextRow());

        delete result;
    }
}

void ScriptMgr::LoadScriptWaypoints()
{
    // Drop Existing Waypoint list
    m_mPointMoveMap.clear();

    uint64 uiCreatureCount = 0;

    // Load Waypoints
    QueryResult* result = WorldDatabase.PQuery("SELECT COUNT(entry) FROM script_waypoint GROUP BY entry");
    if (result)
    {
        uiCreatureCount = result->GetRowCount();
        delete result;
    }

    result = WorldDatabase.PQuery("SELECT entry, pointid, location_x, location_y, location_z, waittime FROM script_waypoint ORDER BY pointid");

    if (result)
    {
        do
        {
            Field* pFields = result->Fetch();
            ScriptPointMove pTemp;

            pTemp.uiCreatureEntry   = pFields[0].GetUInt32();
            uint32 uiEntry          = pTemp.uiCreatureEntry;
            pTemp.uiPointId         = pFields[1].GetUInt32();
            pTemp.fX                = pFields[2].GetFloat();
            pTemp.fY                = pFields[3].GetFloat();
            pTemp.fZ                = pFields[4].GetFloat();
            pTemp.uiWaitTime        = pFields[5].GetUInt32();

            CreatureInfo const* pCInfo = sObjectMgr.GetCreatureTemplate(pTemp.uiCreatureEntry);

            if (!pCInfo)
            {
                if (!sObjectMgr.IsExistingCreatureId(pTemp.uiCreatureEntry))
                    sLog.outErrorDb("DB table script_waypoint has waypoint for nonexistant creature entry %u", pTemp.uiCreatureEntry);
                continue;
            }

            if (!pCInfo->script_id)
                sLog.outErrorDb("DB table script_waypoint has waypoint for creature entry %u, but creature does not have script_name defined and then useless.", pTemp.uiCreatureEntry);

            m_mPointMoveMap[uiEntry].push_back(pTemp);
        } while (result->NextRow());

        delete result;
    }
}

void ScriptMgr::LoadEscortData()
{
    // Drop Existing Escort list
    m_mEscortDataMap.clear();

    uint64 EscortDataCount = 0;

    QueryResult* pResult = WorldDatabase.PQuery("SELECT creature_id, quest, escort_faction FROM script_escort_data");

    if (pResult)
    {
        do
        {
            Field* pFields = pResult->Fetch();
            CreatureEscortData pTemp;

            pTemp.uiCreatureEntry    = pFields[0].GetUInt32();
            pTemp.uiQuestEntry       = pFields[1].GetUInt32();
            pTemp.uiEscortFaction    = pFields[2].GetUInt32();

            CreatureInfo const* pCInfo = sObjectMgr.GetCreatureTemplate(pTemp.uiCreatureEntry);

            if (!pCInfo)
            {
                sLog.outErrorDb("DB table script_escort_data has data for non-existant creature entry %u", pTemp.uiCreatureEntry);
                continue;
            }

            if (!pCInfo->script_id)
                sLog.outErrorDb("DB table script_escort_data has data for creature entry %u, but creature does not have script_name defined and then useless.", pTemp.uiCreatureEntry);

            // Calcul de uiLastWaypointEntry, et mise en "cache"
            std::vector<ScriptPointMove> const points = GetPointMoveList(pTemp.uiCreatureEntry);
            if(points.empty())
            {
                sLog.outErrorDb("Le PNJ %u de script_escort_data n'a pas de donnees de Waypoints !", pTemp.uiCreatureEntry);
                continue;
            }
            pTemp.uiLastWaypointEntry = 0;
            std::vector<ScriptPointMove>::const_iterator it;
            for(it = points.begin(); it != points.end(); ++it)
            {
                if(it->uiPointId > pTemp.uiLastWaypointEntry)
                    pTemp.uiLastWaypointEntry = it->uiPointId;
            }
            m_mEscortDataMap[pTemp.uiCreatureEntry] = pTemp;
            ++EscortDataCount;
        } while (pResult->NextRow());

        delete pResult;
    }
}

std::shared_ptr<QuestInstance> ScriptMgr::GetSharedCopy(ObjectGuid PlayerGuid, QuestInstance* pOrig)
{
    if (pOrig == nullptr)
        return std::shared_ptr<QuestInstance>();

    std::pair<QuestInstanceMultiMap::iterator, QuestInstanceMultiMap::iterator> QuestInstancesByPlayer = m_questInstancies.equal_range(PlayerGuid);

    for (QuestInstanceMultiMap::iterator pQuestIter = QuestInstancesByPlayer.first; pQuestIter != QuestInstancesByPlayer.second; pQuestIter++)
    {
        auto& Elem = *pQuestIter;
        if (Elem.second.operator->() == pOrig)
        {
            return Elem.second;
        }
    }

    return std::shared_ptr<QuestInstance>();
}

void ScriptMgr::CollectPossibleGenericIds(std::set<uint32>& genericIds)
{
    char const* script_tables[10] =
    {
        "creature_ai_scripts",
        "creature_movement_scripts",
        "creature_spells_scripts",
        "event_scripts",
        "generic_scripts",
        "gossip_scripts",
        "gameobject_scripts",
        "spell_scripts",
        "quest_end_scripts",
        "quest_start_scripts"
    };
    Field* fields;
    for (const auto& script_table : script_tables)
    {
        // From SCRIPT_COMMAND_START_SCRIPT and SCRIPT_COMMAND_START_SCRIPT_ON_GROUP.
        std::unique_ptr<QueryResult> result(WorldDatabase.PQuery("SELECT `datalong`, `datalong2`, `datalong3`, `datalong4` FROM `%s` WHERE `command` IN (39, 90)", script_table));

        if (result)
        {
            do
            {
                fields = result->Fetch();
                uint32 script1 = fields[0].GetUInt32();
                if (script1)
                    genericIds.insert(script1);
                uint32 script2 = fields[1].GetUInt32();
                if (script2)
                    genericIds.insert(script2);
                uint32 script3 = fields[2].GetUInt32();
                if (script3)
                    genericIds.insert(script3);
                uint32 script4 = fields[3].GetUInt32();
                if (script4)
                    genericIds.insert(script4);
            } while (result->NextRow());
        }

        // From SCRIPT_COMMAND_TEMP_SUMMON_CREATURE.
        result.reset(WorldDatabase.PQuery("SELECT `dataint2` FROM `%s` WHERE `command`=10 && `dataint2`!=0", script_table));

        if (result)
        {
            do
            {
                fields = result->Fetch();
                uint32 script1 = fields[0].GetUInt32();
                if (script1)
                    genericIds.insert(script1);
            } while (result->NextRow());
        }

        // From SCRIPT_COMMAND_START_SCRIPT_FOR_ALL and SCRIPT_COMMAND_START_SCRIPT_ON_ZONE.
        result.reset(WorldDatabase.PQuery("SELECT `datalong` FROM `%s` WHERE `command` IN (68, 92) && `datalong`!=0", script_table));

        if (result)
        {
            do
            {
                fields = result->Fetch();
                uint32 script1 = fields[0].GetUInt32();
                if (script1)
                    genericIds.insert(script1);
            } while (result->NextRow());
        }

        // From SCRIPT_COMMAND_START_MAP_EVENT, SCRIPT_COMMAND_ADD_MAP_EVENT_TARGET and SCRIPT_COMMAND_EDIT_MAP_EVENT.
        result.reset(WorldDatabase.PQuery("SELECT `dataint2`, `dataint4` FROM `%s` WHERE `command` IN (61, 63, 69)", script_table));

        if (result)
        {
            do
            {
                fields = result->Fetch();
                int32 script1 = fields[0].GetInt32();
                if (script1 > 0)
                    genericIds.insert(script1);
                int32 script2 = fields[1].GetInt32();
                if (script2 > 0)
                    genericIds.insert(script2);
            } while (result->NextRow());
        }
    }
}

void ScriptMgr::CollectPossibleEventIds(std::set<uint32>& eventIds)
{

    // Load all possible script entries from gameobjects.
    std::unique_ptr<QueryResult> result(WorldDatabase.Query("SELECT `data2` FROM `gameobject_template` WHERE `type`=10 && `data2` > 0"));
    Field* fields;
    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 eventId = fields[0].GetUInt32();
            if (eventId)
                eventIds.insert(eventId);
        } while (result->NextRow());
    }
    result.reset(WorldDatabase.Query("SELECT `data6` FROM `gameobject_template` WHERE `type`=3 && `data6` > 0"));
    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 eventId = fields[0].GetUInt32();
            if (eventId)
                eventIds.insert(eventId);
        } while (result->NextRow());
    }
    result.reset(WorldDatabase.Query("SELECT `data2` FROM `gameobject_template` WHERE `type`=13 && `data2` > 0"));
    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 eventId = fields[0].GetUInt32();
            if (eventId)
                eventIds.insert(eventId);
        } while (result->NextRow());
    }
    result.reset(WorldDatabase.Query("SELECT `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11` FROM `gameobject_template` WHERE `type`=29"));
    if (result)
    {
        do
        {
            fields = result->Fetch();
            uint32 data4 = fields[0].GetUInt32();
            if (data4)
                eventIds.insert(data4);
            uint32 data5 = fields[1].GetUInt32();
            if (data5)
                eventIds.insert(data5);
            uint32 data6 = fields[2].GetUInt32();
            if (data6)
                eventIds.insert(data6);
            uint32 data7 = fields[3].GetUInt32();
            if (data7)
                eventIds.insert(data7);
            uint32 data8 = fields[4].GetUInt32();
            if (data8)
                eventIds.insert(data8);
            uint32 data9 = fields[5].GetUInt32();
            if (data9)
                eventIds.insert(data9);
            uint32 data10 = fields[6].GetUInt32();
            if (data10)
                eventIds.insert(data10);
            uint32 data11 = fields[7].GetUInt32();
            if (data11)
                eventIds.insert(data11);
        } while (result->NextRow());
    }

    // Load all possible script entries from spells.
    for (uint32 i = 1; i < sSpellMgr.GetMaxSpellId(); ++i)
    {
        const SpellEntry* spell = sSpellMgr.GetSpellEntry(i);
        if (spell)
        {
            for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
            {
                if (spell->Effect[j] == SPELL_EFFECT_SEND_EVENT)
                {
                    if (spell->EffectMiscValue[j])
                        eventIds.insert(spell->EffectMiscValue[j]);
                }
            }
        }
    }
}

void DoScriptText(int32 iTextEntry, WorldObject* pSource, Unit* pTarget, int32 chatTypeOverride, float rangeOverride)
{
    if (!pSource)
    {
        sLog.outError("DoScriptText entry %i, invalid Source pointer.", iTextEntry);
        return;
    }

    uint8 Type;
    uint32 Emote;
    uint32 Language;
    uint32 SoundId;

    if (iTextEntry >= 0)
    {
        if (const BroadcastText* bct = sObjectMgr.GetBroadcastTextLocale(iTextEntry))
        {
            Type = bct->chatType;
            Emote = bct->emoteId1;
            Language = bct->languageId;
            SoundId = bct->soundId;
        }
        else
        {
            sLog.outError("DoScriptText with source entry %u (TypeId=%u, guid=%u) attempts to process broadcast text id %i, but text id does not exist.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), iTextEntry);
            return;
        }
    }
    else
    {
        if (const StringTextData* pData = sScriptMgr.GetTextData(iTextEntry))
        {
            Type = pData->Type;
            Emote = pData->Emote;
            Language = pData->Language;
            SoundId = pData->SoundId;
        }
        else
        {
            sLog.outError("DoScriptText with source entry %u (TypeId=%u, guid=%u) could not find text entry %i.", pSource->GetEntry(), pSource->GetTypeId(), pSource->GetGUIDLow(), iTextEntry);
            return;
        }
    }

    if (chatTypeOverride >= 0)
        Type = chatTypeOverride;

    DEBUG_LOG("DoScriptText: text entry=%i, Sound=%u, Type=%u, Language=%u, Emote=%u",
        iTextEntry, SoundId, Type, Language, Emote);

    if (SoundId)
    {
        if (sObjectMgr.GetSoundEntry(SoundId))
        {
            if(Type == CHAT_TYPE_ZONE_YELL)
            {
                if(Map* pZone = pSource->GetMap())
                    pZone->PlayDirectSoundToMap(SoundId, pZone->IsContinent() ? pSource->GetZoneId() : 0);
            }
            else
                pSource->PlayDirectSound(SoundId);
        }
        else
            sLog.outError("DoScriptText entry %i tried to process invalid sound id %u.", iTextEntry, SoundId);
    }

    if (Emote)
    {
        if (pSource->GetTypeId() == TYPEID_UNIT || pSource->GetTypeId() == TYPEID_PLAYER)
            ((Unit*)pSource)->HandleEmoteCommand(Emote);
        else
            sLog.outError("DoScriptText entry %i tried to process emote for invalid TypeId (%u).", iTextEntry, pSource->GetTypeId());
    }

    switch (Type)
    {
        case CHAT_TYPE_SAY:
            pSource->MonsterSay(iTextEntry, Language, pTarget);
            break;
        case CHAT_TYPE_YELL:
            pSource->MonsterYell(iTextEntry, Language, pTarget);
            break;
        case CHAT_TYPE_TEXT_EMOTE:
            pSource->MonsterTextEmote(iTextEntry, pTarget, false, rangeOverride);
            break;
        case CHAT_TYPE_BOSS_EMOTE:
            pSource->MonsterTextEmote(iTextEntry, pTarget, true, rangeOverride);
            break;
        case CHAT_TYPE_WHISPER:
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                pSource->MonsterWhisper(iTextEntry, pTarget);
            else
                sLog.outError("DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", iTextEntry);

            break;
        case CHAT_TYPE_BOSS_WHISPER:
        {
            if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                pSource->MonsterWhisper(iTextEntry, pTarget, true);
            else
                sLog.outError("DoScriptText entry %i cannot whisper without target unit (TYPEID_PLAYER).", iTextEntry);

            break;
        }
        case CHAT_TYPE_ZONE_YELL:
            pSource->MonsterYellToZone(iTextEntry, Language, pTarget);
            break;
    }
}


/**
 * Function that either simulates or does script text for a map
 *
 * @param iTextEntry Entry of the text, stored in SD2-database, only type CHAT_TYPE_ZONE_YELL supported
 * @param uiCreatureEntry Id of the creature of whom saying will be simulated
 * @param pMap Given Map on which the map-wide text is displayed
 * @param pCreatureSource Can be nullptr. If pointer to Creature is given, then the creature does the map-wide text
 * @param pTarget Can be nullptr. Possible target for the text
 */
void DoOrSimulateScriptTextForMap(int32 iTextEntry, uint32 uiCreatureEntry, Map* pMap, Creature* pCreatureSource /*=nullptr*/, Unit* pTarget /*=nullptr*/)
{
    if (!pMap)
    {
        sLog.outError("DoOrSimulateScriptTextForMap entry %i, invalid Map pointer.", iTextEntry);
        return;
    }

    CreatureInfo const* pInfo = sObjectMgr.GetCreatureTemplate(uiCreatureEntry);
    if (!pInfo)
    {
        sLog.outError("DoOrSimulateScriptTextForMap has invalid source entry %u for map %u.", uiCreatureEntry, pMap->GetId());
        return;
    }

    uint8 Type;
    uint32 Emote;
    uint32 LanguageId;
    uint32 SoundId;

    if (iTextEntry >= 0)
    {
        if (const BroadcastText* bct = sObjectMgr.GetBroadcastTextLocale(iTextEntry))
        {
            Type = bct->chatType;
            Emote = bct->emoteId1;
            LanguageId = bct->languageId;
            SoundId = bct->soundId;
        }
        else
        {
            sLog.outError("DoOrSimulateScriptTextForMap with source entry %u for map %u could not find broadcast text id %i.", uiCreatureEntry, pMap->GetId(), iTextEntry);
            return;
        }
    }
    else
    {
        if (MangosStringLocale const* pData = sObjectMgr.GetMangosStringLocale(iTextEntry))
        {
            Type = pData->Type;
            Emote = pData->Emote;
            LanguageId = pData->LanguageId;
            SoundId = pData->SoundId;
        }
        else
        {
            sLog.outError("DoOrSimulateScriptTextForMap with source entry %u for map %u could not find script text entry %i.", uiCreatureEntry, pMap->GetId(), iTextEntry);
            return;
        }
    }

    sLog.outDebug("SD2: DoOrSimulateScriptTextForMap: text entry=%i, Sound=%u, Type=%u, Language=%u, Emote=%u",
          iTextEntry, SoundId, Type, LanguageId, Emote);

    if (Type != CHAT_TYPE_ZONE_YELL)
    {
        sLog.outError("DoSimulateScriptTextForMap entry %i has not supported chat type %u.", iTextEntry, Type);
        return;
    }

    if (SoundId)
        pMap->PlayDirectSoundToMap(SoundId);

    if (pCreatureSource)                                // If provided pointer for sayer, use direct version
        pMap->MonsterYellToMap(pCreatureSource->GetObjectGuid(), iTextEntry, Language(LanguageId), pTarget);
    else                                                // Simulate yell
        pMap->MonsterYellToMap(pInfo, iTextEntry, Language(LanguageId), pTarget);
}

void Script::RegisterSelf(bool bReportError)
{
    if (QuestID != 0)
    {
        if (GetQuestInstance != nullptr)
        {
            sScriptMgr.m_questScripts[QuestID] = this;
            ++num_sc_scripts;
        }
        else
        {
            sLog.outError("Script for quest %s can't be registered: GetQuestInstance function is nullptr", Name.c_str());
        }
    }
    else if (uint32 id = sScriptMgr.GetScriptId(Name.c_str()))
    {
        m_NPC_scripts[id] = this;
        ++num_sc_scripts;
    }
    else
    {
        // Don't report unused generic scripts
        if (bReportError)
            sLog.outError("Script registering but script_name %s is not assigned in database. Script will not be used.", Name.c_str());

        delete this;
    }
}

// Returns a target based on the type specified.
WorldObject* GetTargetByType(WorldObject* pSource, WorldObject* pTarget, Map* pMap, uint8 targetType, uint32 param1, uint32 param2, SpellEntry const* pSpellEntry)
{
    switch (targetType)
    {
        case TARGET_T_PROVIDED_TARGET:
            return pTarget;
        case TARGET_T_HOSTILE:
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->GetVictim();
            break;
        case TARGET_T_HOSTILE_SECOND_AGGRO:
            if (Creature* pCreatureSource = ToCreature(pSource))
                return pCreatureSource->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 1, pSpellEntry, param1 ? param1 : SELECT_FLAG_NO_TOTEM);
            break;
        case TARGET_T_HOSTILE_LAST_AGGRO:
            if (Creature* pCreatureSource = ToCreature(pSource))
                return pCreatureSource->SelectAttackingTarget(ATTACKING_TARGET_BOTTOMAGGRO, 0, pSpellEntry, param1 ? param1 : SELECT_FLAG_NO_TOTEM);
            break;
        case TARGET_T_HOSTILE_RANDOM:
            if (Creature* pCreatureSource = ToCreature(pSource))
                return pCreatureSource->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, pSpellEntry, param1 ? param1 : SELECT_FLAG_NO_TOTEM);
            break;
        case TARGET_T_HOSTILE_RANDOM_NOT_TOP:
            if (Creature* pCreatureSource = ToCreature(pSource))
                return pCreatureSource->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, pSpellEntry, param1 ? param1 : SELECT_FLAG_NO_TOTEM);
            break;
        case TARGET_T_OWNER_OR_SELF:
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->GetCharmerOrOwnerOrSelf();
            break;
        case TARGET_T_OWNER:
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->GetOwner();
            break;
        case TARGET_T_NEAREST_CREATURE_WITH_ENTRY:
        {
            WorldObject* pSearcher;
            if (!((pSearcher = pSource) || (pSearcher = pTarget)))
                return nullptr;
            return pSearcher->FindNearestCreature(param1, param2, true);
        }
        case TARGET_T_RANDOM_CREATURE_WITH_ENTRY:
        {
            WorldObject* pSearcher;
            if (!((pSearcher = pSource) || (pSearcher = pTarget)))
                return nullptr;
            return pSearcher->FindRandomCreature(param1, param2, true, pSearcher->ToCreature());
        }
        case TARGET_T_CREATURE_WITH_GUID:
        {
            if (!pMap)
                return nullptr;
            if (CreatureData const* pCreatureData = sObjectMgr.GetCreatureData(param1))
                return pMap->GetCreature(ObjectGuid(HIGHGUID_UNIT, pCreatureData->creature_id[0], param1));
            break;
        }
        case TARGET_T_CREATURE_FROM_INSTANCE_DATA:
        {
            if (!pMap)
                return nullptr;
            if (InstanceData* pInstanceData = pMap->GetInstanceData())
                return pInstanceData->GetCreature(pInstanceData->GetData64(param1));
            break;
        }
        case TARGET_T_NEAREST_GAMEOBJECT_WITH_ENTRY:
        {
            WorldObject* pSearcher;
            if (!((pSearcher = pSource) || (pSearcher = pTarget)))
                return nullptr;
            return pSearcher->FindNearestGameObject(param1, param2);
        }
        case TARGET_T_RANDOM_GAMEOBJECT_WITH_ENTRY:
        {
            WorldObject* pSearcher;
            if (!((pSearcher = pSource) || (pSearcher = pTarget)))
                return nullptr;
            return pSearcher->FindRandomGameObject(param1, param2);
        }
        case TARGET_T_GAMEOBJECT_WITH_GUID:
        {
            if (!pMap)
                return nullptr;
            if (GameObjectData const* pGameObjectData = sObjectMgr.GetGOData(param1))
                return pMap->GetGameObject(ObjectGuid(HIGHGUID_GAMEOBJECT, pGameObjectData->id, param1));
            break;
        }
        case TARGET_T_GAMEOBJECT_FROM_INSTANCE_DATA:
        {
            if (!pMap)
                return nullptr;
            if (InstanceData* pInstanceData = pMap->GetInstanceData())
                return pInstanceData->GetGameObject(pInstanceData->GetData64(param1));
            break;
        }
        case TARGET_T_FRIENDLY:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->SelectRandomFriendlyTarget(param2 ? ToUnit(pTarget) : nullptr, param1 ? param1 : 30.0f, true);
            break;
        case TARGET_T_FRIENDLY_INJURED:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->FindLowestHpFriendlyUnit(param1 ? param1 : 30.0f, param2 ? param2 : 50, true);
            break;
        case TARGET_T_FRIENDLY_INJURED_EXCEPT:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->FindLowestHpFriendlyUnit(param1 ? param1 : 30.0f, param2 ? param2 : 50, true, ToUnit(pTarget));
            break;
        case TARGET_T_FRIENDLY_MISSING_BUFF:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->FindFriendlyUnitMissingBuff(param1 ? param1 : 30.0f, param2);
            break;
        case TARGET_T_FRIENDLY_MISSING_BUFF_EXCEPT:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->FindFriendlyUnitMissingBuff(param1 ? param1 : 30.0f, param2, ToUnit(pTarget));
            break;
        case TARGET_T_FRIENDLY_CC:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->FindFriendlyUnitCC(param1 ? param1 : 30.0f);
            break;
        case TARGET_T_MAP_EVENT_SOURCE:
            if (Map* pMap = pSource ? pSource->GetMap() : (pTarget ? pTarget->GetMap() : nullptr))
                if (ScriptedEvent const* pEvent = pMap->GetScriptedMapEvent(param1))
                    return pEvent->GetSourceObject();
            break;
        case TARGET_T_MAP_EVENT_TARGET:
            if (Map* pMap = pSource ? pSource->GetMap() : (pTarget ? pTarget->GetMap() : nullptr))
                if (ScriptedEvent const* pEvent = pMap->GetScriptedMapEvent(param1))
                    return pEvent->GetTargetObject();
            break;
        case TARGET_T_MAP_EVENT_EXTRA_TARGET:
            if (Map* pMap = pSource ? pSource->GetMap() : (pTarget ? pTarget->GetMap() : nullptr))
                if (ScriptedEvent const* pEvent = pMap->GetScriptedMapEvent(param1))
                    for (const auto& target : pEvent->m_vTargets)
                        if (WorldObject* pObject = pMap->GetWorldObject(target.target))
                            if (pObject && (pObject->GetEntry() == param2))
                                return pObject;
            break;
        case TARGET_T_NEAREST_PLAYER:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (pSource)
                return pSource->FindNearestPlayer(param1);
            break;
        case TARGET_T_NEAREST_HOSTILE_PLAYER:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->FindNearestHostilePlayer(param1);
            break;
        case TARGET_T_NEAREST_FRIENDLY_PLAYER:
            if (!param1 && pSpellEntry)
                param1 = sSpellRangeStore.LookupEntry(pSpellEntry->rangeIndex)->maxRange;
            if (Unit* pUnitSource = ToUnit(pSource))
                return pUnitSource->FindNearestFriendlyPlayer(param1);
            break;
    }

    return nullptr;
}

QuestInstance::QuestInstance(ObjectGuid InPlayerGuid, uint32 InQuestID)
    : PlayerGuid(InPlayerGuid), QuestID(InQuestID)
{
    QuestTemplate = sObjectMgr.GetQuestTemplate(QuestID);

    if (!QuestTemplate)
    {
        sLog.outError("FATAL: Can't find QuestTemplate with id %llu while creating script instance", QuestID);
        MANGOS_ASSERT(QuestTemplate);
    }
}

Player* QuestInstance::GetPlayer() const
{
    return ObjectMgr::GetPlayer(PlayerGuid);
}

bool QuestInstance::GoToStage(uint32 newStage)
{
    if (newStage < QuestStage)
    {
        DETAIL_LOG("[QuestInstance]: '%s' new stage '%d' but old '%d' which is lower", GetQuestTemplate()->GetTitle().c_str(), newStage, QuestStage);
    }
    SetQuestStage(newStage);
    return true;
}


// --- module queries -------------------------------------------------------
// Thin wrappers over the ScriptRegistry loops so call sites stay readable.
// All of these sit on cold paths (group forming, looking-for-team, death and
// release), so the loop costs nothing worth measuring.

bool Script_IsAIControlled(Player const* player)
{
    if (!player)
        return false;

    return ScriptRegistry<PlayerScript>::ForEachEnabledHookWithReturn(PLAYERHOOK_IS_AI_CONTROLLED, [&](PlayerScript* script)
    {
        return script->IsAIControlled(player);
    });
}

bool Script_HasAIFollowers(Player const* player)
{
    if (!player)
        return false;

    return ScriptRegistry<PlayerScript>::ForEachEnabledHookWithReturn(PLAYERHOOK_HAS_AI_FOLLOWERS, [&](PlayerScript* script)
    {
        return script->HasAIFollowers(player);
    });
}

uint8 Script_GetAllowedRoles(Player const* player)
{
    if (!player)
        return 0;

    uint8 roles = 0;
    ScriptRegistry<PlayerScript>::ForEachEnabledHookWithReturn(PLAYERHOOK_GET_ALLOWED_ROLES, [&](PlayerScript* script)
    {
        return script->GetAllowedRoles(player, roles);
    });

    return roles;
}

void Script_SetForcedRole(Player* player, uint8 role)
{
    if (!player)
        return;

    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_SET_FORCED_ROLE, [&](PlayerScript* script)
    {
        script->SetForcedRole(player, role);
    });
}

bool Script_IsMachineDriven(Player const* player)
{
    if (!player)
        return false;

    return ScriptRegistry<PlayerScript>::ForEachEnabledHookWithReturn(PLAYERHOOK_IS_MACHINE_DRIVEN, [&](PlayerScript* script)
    {
        return script->IsMachineDriven(player);
    });
}

bool Script_IsUpdateCritical(Player const* player)
{
    if (!player)
        return false;

    return ScriptRegistry<PlayerScript>::ForEachEnabledHookWithReturn(PLAYERHOOK_IS_UPDATE_CRITICAL, [&](PlayerScript* script)
    {
        return script->IsUpdateCritical(player);
    });
}

bool Script_IsAIUpdateDue(Player* player, uint32 diff)
{
    return ScriptRegistry<PlayerScript>::ForEachEnabledHookWithReturn(PLAYERHOOK_IS_AI_UPDATE_DUE,
        [&](PlayerScript* script) { return script->IsAIUpdateDue(player, diff); });
}

void Script_UpdateAI(Player* player, uint32 diff, bool minimal)
{
    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_AI_UPDATE,
        [&](PlayerScript* script) { script->OnAIUpdate(player, diff, minimal); });
}
