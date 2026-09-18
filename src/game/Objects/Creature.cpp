#include "Util/DevDiagnostics.h"
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

#include "Creature.h"
#include "ArchitectureDiagnostics.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "World.h"
#include "ObjectMgr.h"
#include "ScriptMgr.h"
#include "ScriptObjects.h"
#include "ObjectGuid.h"
#include "SpellMgr.h"
#include "QuestDef.h"
#include "GossipDef.h"
#include "Player.h"
#include "GameEventMgr.h"
#include "PoolManager.h"
#include "Opcodes.h"
#include "Log.h"
#include "LootMgr.h"
#include "MapManager.h"
#include "CreatureAI.h"
#include "CreatureAISelector.h"
#include "Formulas.h"
#include "WaypointMovementGenerator.h"
#include "InstanceData.h"
#include "MapPersistentStateMgr.h"
#include "BattleGroundMgr.h"
#include "Spell.h"
#include "Util.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "Language.h"
#include "Mail.h"
#include "CreatureGroups.h"
#include "ZoneScript.h"
#include "MoveSplineInit.h"
#include "MoveSpline.h"
#include "Anticheat/Anticheat.h"
#include "Anticheat/Movement/Movement.hpp"
#include "CreatureLinkingMgr.h"
#include "TemporarySummon.h"
#include "ScriptedEscortAI.h"
#include "GuardMgr.h"
#include "GuidObjectScaling.h"
#include "PerfStats.h"
#include "Autoscaling/AutoScaler.hpp"

// apply implementation of the singletons
#include "Policies/SingletonImp.h"


TrainerSpell const* TrainerSpellData::Find(uint32 spell_id) const
{
    TrainerSpellMap::const_iterator itr = spellList.find(spell_id);
    if (itr != spellList.end())
        return &itr->second;

    return nullptr;
}

bool VendorItemData::RemoveItem(uint32 item_id)
{
    for (VendorItemList::iterator i = m_items.begin(); i != m_items.end(); ++i)
    {
        if ((*i)->item == item_id)
        {
            m_items.erase(i);
            return true;
        }
    }
    return false;
}

size_t VendorItemData::FindItemSlot(uint32 item_id) const
{
    for (size_t i = 0; i < m_items.size(); ++i)
        if (m_items[i]->item == item_id)
            return i;
    return m_items.size();
}

VendorItem const* VendorItemData::FindItem(uint32 item_id) const
{
    for (const auto item : m_items)
        if (item->item == item_id)
            return item;

    return nullptr;
}

bool AssistDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    if (Unit* victim = m_owner.GetMap()->GetUnit(m_victimGuid))
    {
        while (!m_assistantGuids.empty())
        {
            Creature* assistant = m_owner.GetMap()->GetAnyTypeCreature(*m_assistantGuids.rbegin());
            m_assistantGuids.pop_back();

            if (assistant && assistant->CanAssistTo(&m_owner, victim))
            {
                assistant->SetNoCallAssistance(true);
                if (assistant->AI())
                {
                    assistant->AI()->AttackStart(victim);

                    // When nearby mobs aggro from another mob's initial call for assistance
                    // their leash timers become linked and attacking one will keep the rest from evading.
                    if (assistant->GetVictim())
                        assistant->SetLastLeashExtensionTimePtr(static_cast<Creature*>(&m_owner)->GetLastLeashExtensionTimePtr());
                }
            }
        }
    }
    return true;
}

AssistDelayEvent::AssistDelayEvent(ObjectGuid victim, Unit& owner, std::list<Creature*> const& assistants) : BasicEvent(), m_victimGuid(victim), m_owner(owner)
{
    // Pushing guids because in delay can happen some creature gets despawned => invalid pointer
    m_assistantGuids.reserve(assistants.size());
    for (const auto assistant : assistants)
        m_assistantGuids.push_back(assistant->GetObjectGuid());
}

bool ForcedDespawnDelayEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    m_owner.ForcedDespawn(0, m_secsTimeToRespawn);
    return true;
}

bool TargetedEmoteEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    if (!m_owner.IsInCombat() && !m_owner.IsMoving())
    {
        if (Unit* pTarget = m_owner.GetMap()->GetUnit(m_targetGuid))
        {
            m_owner.SetFacingToObject(pTarget);
            m_owner.HandleEmote(m_emoteId);
            return true;
        }
    }

    m_owner.ClearCreatureState(CSTATE_TARGETED_EMOTE);
    return true;
}

bool TargetedEmoteCleanupEvent::Execute(uint64 /*e_time*/, uint32 /*p_time*/)
{
    if (m_owner.HasCreatureState(CSTATE_TARGETED_EMOTE))
    {
        if (!m_owner.IsInCombat() && !m_owner.IsMoving())
            m_owner.SetFacingTo(m_orientation);
        m_owner.HandleEmoteState(0);
        m_owner.ClearCreatureState(CSTATE_TARGETED_EMOTE);
    }

    return true;
}

void CreatureCreatePos::SelectFinalPoint(Creature* cr)
{
    // if object provided then selected point at specific dist/angle from object forward look
    if (m_closeObject)
    {
        if (m_dist == 0.0f)
        {
            m_pos.x = m_closeObject->GetPositionX();
            m_pos.y = m_closeObject->GetPositionY();
            m_pos.z = m_closeObject->GetPositionZ();
        }
        else
            m_closeObject->GetClosePoint(m_pos.x, m_pos.y, m_pos.z, cr->GetObjectBoundingRadius(), m_dist, m_angle);
    }
}

bool CreatureCreatePos::Relocate(Creature* cr) const
{
    cr->Relocate(m_pos.x, m_pos.y, m_pos.z, m_pos.o);

    if (!cr->IsPositionValid())
    {
        sLog.outError("%s not created. Suggested coordinates isn't valid (X: %f Y: %f)", cr->GetGuidStr().c_str(), cr->GetPositionX(), cr->GetPositionY());
        return false;
    }

    //don't expect creatures to change this v much.
    cr->GetZoneAndAreaId(cr->m_areaUpdateId, cr->m_zoneUpdateId);

    return true;
}

Creature::Creature(CreatureSubtype subtype) :
    Unit(), i_AI(nullptr),
    loot(this), lootForPickPocketed(false), lootForBody(false), lootForSkin(false), skinningForOthersTimer(5000), m_TargetNotReachableTimer(0),
    m_pacifiedTimer(0), m_manaRegen(0),
    m_groupLootTimer(0), m_groupLootId(0), m_lootMoney(0), m_lootGroupRecipientId(0), m_corpseDecayTimer(0),
    m_respawnTime(0), m_respawnDelay(120), m_corpseDelay(60),
    m_wanderDistance(5.0f), m_combatStartTime(0), m_combatResetCount(0), m_subtype(subtype),
    m_defaultMovementType(IDLE_MOTION_TYPE), m_equipmentId(0), m_creatureStateFlags(CSTATE_REGEN_HEALTH | CSTATE_REGEN_MANA),
    m_AI_locked(false), m_temporaryFactionFlags(TEMPFACTION_NONE),
    m_meleeDamageSchoolMask(SPELL_SCHOOL_MASK_NORMAL), m_originalEntry(0), m_creatureGroup(nullptr),
    m_combatStartX(0.0f), m_combatStartY(0.0f), m_combatStartZ(0.0f), m_reactState(REACT_PASSIVE),
    m_lastLeashExtensionTime(nullptr), m_playerDamageTaken(0), m_nonPlayerDamageTaken(0), m_creatureInfo(nullptr),
    m_detectionDistance(20.0f), m_callForHelpTimer(0), m_callForHelpDist(5.0f), m_callsForHelp(true), m_leashDistance(0.0f), m_mountId(0), m_isDeadByDefault(false),
    m_reputationId(-1), m_gossipMenuId(0), m_castingTargetGuid(0)
{
    m_regenTimer = 200;
    m_valuesCount = UNIT_END;

    for (uint32 & spell : m_spells)
        spell = 0;

    ++PerfStats::g_totalCreatures;
}

Creature::~Creature()
{
    CleanupsBeforeDelete();

    m_vendorItemCounts.clear();

    delete i_AI;
    i_AI = nullptr;

    --PerfStats::g_totalCreatures;
}

void Creature::AddToWorld()
{
    bool bWasInWorld = IsInWorld();

    ///- Register the creature for guid lookup
    if (!IsInWorld() && GetObjectGuid().GetHigh() == HIGHGUID_UNIT)
        GetMap()->InsertObject<Creature>(GetObjectGuid(), this);

    sCreatureGroupsManager.LoadCreatureGroup(this, m_creatureGroup);
    if (m_creatureGroup)
    {
        if (m_creatureGroup->IsFormation())
            SetActiveObjectState(true);
        if (GetDeathState() == ALIVE || GetDeathState() == JUST_ALIVED)
            m_creatureGroup->OnRespawn(this);
    }
    Unit::AddToWorld();

    if (!i_AI)
        AIM_Initialize();
    if (!bWasInWorld && m_zoneScript)
        m_zoneScript->OnCreatureCreate(this);

    // The backported AllCreatureScript surface never had a caller for
    // OnCreatureAddWorld - modules registering it were silently dead. Fire it
    // where AzerothCore does: creature fully in the world, first entry only.
    // (AllCreatureScript has no per-hook registry; ForEach walks all scripts.)
    if (!bWasInWorld && IsInWorld())
    {
        ScriptRegistry<AllCreatureScript>::ForEach([&](AllCreatureScript* script)
        {
            script->OnCreatureAddWorld(this);
        });
    }
}

void Creature::RemoveFromWorld()
{
    ///- Remove the creature from the accessor
    if (IsInWorld())
    {
        ScriptRegistry<AllCreatureScript>::ForEach([&](AllCreatureScript* script)
        {
            script->OnCreatureRemoveWorld(this);
        });

        if (AI())
            AI()->OnRemoveFromWorld();
        if (GetObjectGuid().GetHigh() == HIGHGUID_UNIT)
            GetMap()->EraseObject<Creature>(GetObjectGuid());
        if (m_zoneScript)
            m_zoneScript->OnCreatureRemove(this);
    }
    Unit::RemoveFromWorld();
}

void Creature::RemoveCorpse()
{
    if ((GetDeathState() != CORPSE && !IsDeadByDefault()) || (GetDeathState() != ALIVE && IsDeadByDefault()))
        return;

    m_corpseDecayTimer = 0;
    SetDeathState(DEAD);
    UpdateObjectVisibility();

    // stop loot rolling before loot clear and for close client dialogs
    StopGroupLoot();

    // send unlooted rares to player who won them by mail
    if (loot.unlootedCount)
    {
        for (auto& itr : loot.items)
        {
            if (!itr.is_looted && !itr.needs_quest && !itr.lootOwner.IsEmpty())
            {
                if (ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(itr.itemid))
                {
                    if (pProto->Quality >= ITEM_QUALITY_RARE)
                    {
                        if (Player* pPlayer = sObjectMgr.GetPlayer(itr.lootOwner))
                        {
                            if (Item* pItem = Item::CreateItem(itr.itemid, itr.count, pPlayer))
                            {
                                itr.is_looted = true;
                                --loot.unlootedCount;

                                pItem->SaveToDB();
                                std::string subject = pProto->Name1;
                                std::string body = "You won a roll for this item but were unable to loot it in time.";
                                MailDraft(subject, body)
                                    .AddItem(pItem)
                                    .SendMailTo(pPlayer, MailSender(MAIL_CREATURE, GetEntry()), MAIL_CHECK_MASK_COPIED, 15 * MINUTE, 1 * DAY);

                                if (!loot.unlootedCount)
                                    break;
                            }
                        }
                    }
                }
            }
        }
    }

    loot.clear();
    uint32 respawnDelay = 0;

    if (AI())
        AI()->CorpseRemoved(respawnDelay);

    if (m_isCreatureLinkingTrigger)
        GetMap()->GetCreatureLinkingHolder()->DoCreatureLinkingEvent(LINKING_EVENT_DESPAWN, this);

    // script can set time (in seconds) explicit, override the original
    if (respawnDelay)
        m_respawnTime = time(nullptr) + respawnDelay;

    float x, y, z, o;
    GetRespawnCoord(x, y, z, &o);
    GetMap()->CreatureRelocation(this, x, y, z, o);
}

/**
 * change the entry of creature until respawn
 */
bool Creature::InitEntry(uint32 Entry, CreatureData const* data /*=nullptr*/, CreatureDataAddon const* addon /*=nullptr*/, GameEventCreatureData const* eventData /*=nullptr*/)
{
    // use game event entry if any instead default suggested
    if (eventData && eventData->entry_id)
        Entry = eventData->entry_id;

    CreatureInfo const *normalInfo = sObjectMgr.GetCreatureTemplate(Entry);
    if (!normalInfo)
    {
        sLog.outErrorDb("Creature::UpdateEntry creature entry %u does not exist.", Entry);
        return false;
    }

    CreatureInfo const *cinfo = normalInfo;

    SetEntry(Entry);                                        // normal entry always
    m_creatureInfo = cinfo;                                 // map mode related always

    SetObjectScale(sGuidObjectScaling.GetScale(GetGUID(), cinfo->scale));
    // Reset native scale before we apply creature info multiplier, otherwise we are
    // stuck at 1 from the previous m_nativeScaleOverride if the unit's entry is
    // being changed
    m_nativeScaleOverride = cinfo->scale;
    m_nativeScale = cinfo->scale;

    // equal to player Race field, but creature does not have race
    SetByteValue(UNIT_FIELD_BYTES_0, 0, 0);

    // known valid are: CLASS_WARRIOR,CLASS_PALADIN,CLASS_ROGUE,CLASS_MAGE
    SetByteValue(UNIT_FIELD_BYTES_0, 1, uint8(cinfo->unit_class));

    SetInitCreaturePowerType();

    uint32 displayId = ChooseDisplayId(GetCreatureInfo(), data, addon, eventData);
    if (!displayId)                                        // Cancel load if no display id
    {
        sLog.outErrorDb("Creature (Entry: %u) has no display id defined in table `creature_template`, can't load.", Entry);
        return false;
    }

    CreatureDisplayInfoAddon const* minfo = sObjectMgr.GetCreatureDisplayInfoRandomGender(displayId);
    if (!minfo)                                             // Cancel load if no display info addon defined
    {
        sLog.outErrorDb("Creature (Entry: %u) has no display id data defined in table `creature_display_info_addon`, using default.", Entry);
        minfo = sObjectMgr.GetCreatureDisplayInfoAddon(13);
        if (!minfo)
            return false;
    }

    displayId = minfo->display_id;                          // it can be different (for another gender)

    SetNativeDisplayId(displayId);
    SetDisplayId(displayId);
    SetByteValue(UNIT_FIELD_BYTES_0, 2, minfo->gender);

    // Load creature equipment
    if (eventData && eventData->equipment_id)
    {
        LoadEquipment(eventData->equipment_id);             // use event equipment if any for active event
    }
    else if (!addon || addon->equipment_id < 0)
    {
        // use default from the template
        LoadEquipment(cinfo->equipment_id);
    }
    else if (addon && addon->equipment_id >= 0)
    {
        // override
        LoadEquipment(addon->equipment_id);
    }

    SetFloatValue(UNIT_MOD_CAST_SPEED, 1.0f);

    // update speed for the new CreatureInfo base speed mods
    UpdateSpeed(MOVE_WALK, false);
    UpdateSpeed(MOVE_RUN,  false);
    SetFly(CanFly());

    m_defaultMovementType = MovementGeneratorType(data ? data->movement_type : cinfo->movement_type);

    // Apply Poison & Disease immunities for Elemental and Mechanical type creatures
    switch (GetCreatureInfo()->type)
    {
        case CREATURE_TYPE_MECHANICAL:
        {
            ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL, true);
            ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_HEAL_MAX_HEALTH, true);
            ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_HEAL, true);
            ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_PERIODIC_LEECH, true);
            // no break
        }
        case CREATURE_TYPE_ELEMENTAL:
        {
            ApplySpellImmune(0, IMMUNITY_DISPEL, DISPEL_DISEASE, true);
            ApplySpellImmune(0, IMMUNITY_DISPEL, DISPEL_POISON, true);
            break;
        }
    }

    if (HasImmunityFlag(CREATURE_IMMUNITY_TAUNT))
    {
        ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_ATTACK_ME, true);
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TAUNT, true);
    }

    if (HasImmunityFlag(CREATURE_IMMUNITY_MOD_STAT))
    {
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_STAT, true);
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, true);
    }

    if (HasImmunityFlag(CREATURE_IMMUNITY_MOD_CAST_SPEED))
    {
        ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK, true);
    }

    if (HasImmunityFlag(CREATURE_IMMUNITY_DISEASE))
    {
        ApplySpellImmune(0, IMMUNITY_DISPEL, DISPEL_DISEASE, true);
    }

    if (HasImmunityFlag(CREATURE_IMMUNITY_POISON))
    {
        ApplySpellImmune(0, IMMUNITY_DISPEL, DISPEL_POISON, true);
    }

    if (HasImmunityFlag(CREATURE_IMMUNITY_CURSE))
    {
        ApplySpellImmune(0, IMMUNITY_DISPEL, DISPEL_CURSE, true);
    }

    return true;
}


uint32 Creature::GetSpawnFlags() const
{
    if (CreatureData const* data = sObjectMgr.GetCreatureData(GetGUIDLow()))
        return data->spawn_flags;
    return 0;
}

bool Creature::UpdateEntry(uint32 Entry, CreatureData const* data /*=nullptr*/, GameEventCreatureData const* eventData /*=nullptr*/, bool preserveHPAndPower /*=true*/)
{
    bool addonReload = false;

    /*
     * This section of code is an attempt to handle the case where creature entry IDs are
     * updated after creature creation. This is typically done to randomise trash spawn
     * types and it works (mostly) fine until the different creature entries have different
     * creature_template aura entries. What we want to do is ensure auras belonging to
     * the previous creature entry are removed and auras belonging to the new creature
     * entry are applied. This complication is that this function is also called
     * from several other spots, including Creature::Create, which causes a
     * few problems if not handled correctly, for some definition of correct.
     *
     * TL;DR: Hack to handle randomised trash spawn auras without requiring
     * script authors to do it explicitly and/or breaking existing code.
     * Would be better to have a spawn system that could properly handle
     * random entries.
     */

    auto creaAddonData = ObjectMgr::GetCreatureAddon(GetGUIDLow());
    if (m_creatureInfo) // prevent aura unloading if this creature is still under creation
    {
        /*
         * Auras listed in creature_addon override anything contained in creature_template_addon,
         * so we don't want to unload GUID-based auras, even if we're changing the template entry
         */
        if (!creaAddonData && Entry != m_creatureInfo->entry)
        {
            addonReload = true;

            /*
             * Looks like we're changing the creature's entry ID, so remove any auras
             * coming from the creature_template table
             */
            if (m_creatureInfo->auras)
            {
                for (uint32 const* cAura = m_creatureInfo->auras; *cAura; ++cAura)
                {
                    if (HasAura(*cAura))
                    {
                        RemoveAurasDueToSpellByCancel(*cAura);
                    }
                }
            }
        }
    }

    if (!InitEntry(Entry, data, creaAddonData, eventData))
        return false;

    if (addonReload)
    {
        LoadCreatureAddon(true);
    }

    if (GetCreatureInfo()->regeneration & REGEN_FLAG_HEALTH)
        AddCreatureState(CSTATE_REGEN_HEALTH);
    else
        ClearCreatureState(CSTATE_REGEN_HEALTH);

    if (GetCreatureInfo()->regeneration & REGEN_FLAG_POWER)
        AddCreatureState(CSTATE_REGEN_MANA);
    else
        ClearCreatureState(CSTATE_REGEN_MANA);

    // creatures always have melee weapon ready if any
    SetSheath(SHEATH_STATE_MELEE);
    SetByteValue(UNIT_FIELD_BYTES_2, 1, UNIT_BYTE2_FLAG_AURAS);

    SelectLevel(GetCreatureInfo(), preserveHPAndPower ? GetHealthPercent() : 100.0f, preserveHPAndPower ? GetPowerPercent(POWER_MANA) : 100.0f);

    SetFactionTemplateId(GetCreatureInfo()->faction);
    SetDefaultGossipMenuId(GetCreatureInfo()->gossip_menu_id);
    SetUInt32Value(UNIT_NPC_FLAGS, GetCreatureInfo()->npc_flags);

    uint32 attackTimer = GetCreatureInfo()->base_attack_time;
    SetAttackTime(BASE_ATTACK,  attackTimer);
    SetAttackTime(OFF_ATTACK,   attackTimer);
    SetAttackTime(RANGED_ATTACK, GetCreatureInfo()->ranged_attack_time);

    uint32 unitFlags = GetCreatureInfo()->unit_flags;
    // we may need to append or remove additional flags
    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT))
        unitFlags |= UNIT_FLAG_IN_COMBAT;
    // Nostalrius: we need this flag to get a proper animation
    // Giant type creatures walk underwater
    if (CanSwim() && GetCreatureInfo()->type != CREATURE_TYPE_GIANT)
        unitFlags |= UNIT_FLAG_USE_SWIM_ANIMATION;
    SetUInt32Value(UNIT_FIELD_FLAGS, unitFlags);

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_APPEAR_DEAD))
        SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

    // preserve all current dynamic flags if exist
    uint32 dynFlags = GetUInt32Value(UNIT_DYNAMIC_FLAGS);
    SetUInt32Value(UNIT_DYNAMIC_FLAGS, dynFlags ? dynFlags : GetCreatureInfo()->dynamic_flags);

    SetCreateResistance(SPELL_SCHOOL_NORMAL, GetCreatureInfo()->armor);
    SetCreateResistance(SPELL_SCHOOL_HOLY, GetCreatureInfo()->holy_res);
    SetCreateResistance(SPELL_SCHOOL_FIRE, GetCreatureInfo()->fire_res);
    SetCreateResistance(SPELL_SCHOOL_NATURE, GetCreatureInfo()->nature_res);
    SetCreateResistance(SPELL_SCHOOL_FROST, GetCreatureInfo()->frost_res);
    SetCreateResistance(SPELL_SCHOOL_SHADOW, GetCreatureInfo()->shadow_res);
    SetCreateResistance(SPELL_SCHOOL_ARCANE, GetCreatureInfo()->arcane_res);

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_FIXED_Z))
        AddUnitMovementFlag(MOVEFLAG_FIXED_Z);

    SetFly(CanFly());
    SetMeleeDamageSchool(SpellSchools(GetCreatureInfo()->dmg_school));

    SetCanModifyStats(true);
    UpdateAllStats();

    m_reputationId = -1;

    // checked and error show at loading templates
    if (FactionTemplateEntry const* pFactionTemplate = sObjectMgr.GetFactionTemplateEntry(GetCreatureInfo()->faction))
        if (const FactionEntry* pFaction = sObjectMgr.GetFactionEntry(pFactionTemplate->faction))
            m_reputationId = pFaction->reputationListID;

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_PVP))
        SetPvP(true);
    else
        SetPvP(false);

    if (CanAssistPlayers())
        EnableMoveInLosEvent();

    for (int i = 0; i < CREATURE_MAX_SPELLS; ++i)
        m_spells[i] = GetCreatureInfo()->spells[i];

    SetCallForHelpDist(GetCreatureInfo()->call_for_help_range);
    SetLeashDistance(GetCreatureInfo()->leash_range);
    SetDetectionDistance(GetCreatureInfo()->detection_range);

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_LARGE_AOI))
    {
        SetVisibilityModifier(VISIBILITY_DISTANCE_LARGE);
        if (sWorld.getConfig(CONFIG_BOOL_VISIBILITY_FORCE_ACTIVE_OBJECTS))
            SetActiveObjectState(true);
    }

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_GIGANTIC_AOI))
    {
        SetVisibilityModifier(VISIBILITY_DISTANCE_GIGANTIC);
        if (sWorld.getConfig(CONFIG_BOOL_VISIBILITY_FORCE_ACTIVE_OBJECTS))
            SetActiveObjectState(true);
    } 

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_INFINITE_AOI))
    {
        SetVisibilityModifier(MAX_VISIBILITY_DISTANCE);
        if (sWorld.getConfig(CONFIG_BOOL_VISIBILITY_FORCE_ACTIVE_OBJECTS))
            SetActiveObjectState(true);
    }

    if (AI() && GetCreatureInfo()->spell_list_id)
        AI()->SetSpellsList(GetCreatureInfo()->spell_list_id);

    // if eventData set then event active and need apply spell_start
    if (eventData)
        ApplyGameEventSpells(eventData, true);

    return true;
}

uint32 Creature::ChooseDisplayId(CreatureInfo const* cinfo, CreatureData const* data /*= nullptr*/, CreatureDataAddon const* addon /*= nullptr*/, GameEventCreatureData const* eventData /*=nullptr*/)
{
    // Use creature event display id explicit, override any other static models
    if (eventData && eventData->display_id)
        return eventData->display_id;

    // Use creature display id explicit, override template (creature.display_id)
    if (addon && addon->display_id)
        return addon->display_id;

    // use defaults from the template
    uint32 display_id = 0;

    // display id selected here may be replaced with other_gender using own function
    uint32 maxDisplayId = 0;
    for (; maxDisplayId < MAX_DISPLAY_IDS_PER_CREATURE && cinfo->display_id[maxDisplayId]; ++maxDisplayId);

    if (maxDisplayId)
        display_id = cinfo->display_id[urand(0, maxDisplayId - 1)];

    // fail safe, we use creature entry 1 and make error
    if (!display_id)
    {
        sLog.outErrorDb("Creature::ChooseDisplayId can not select native display id for creature entry %u, model from creature entry 1 will be used instead.", cinfo->entry);

        if (const CreatureInfo *creatureDefault = sObjectMgr.GetCreatureTemplate(1))
            display_id = creatureDefault->display_id[0];
    }

    return display_id;
}

void Creature::Update(uint32 update_diff, uint32 diff)
{
    MANTECH_DIAG_SCOPE(Creature, 32, nullptr);
    TurtleDiagnostics::CreatureProbe diagnosticCreature(this, GetGUIDLow(), GetEntry(),
        uint32(m_deathState), IsInCombat(), update_diff);
    update_diff *= sWorld.GetTimeRate();
    diff *= sWorld.GetTimeRate();

    ScriptRegistry<AllCreatureScript>::ForEach([&](AllCreatureScript* script)
    {
        script->OnAllCreatureUpdate(this, update_diff);
    });

    TurtleDiagnostics::CreatureProbe::Stage(this, TurtleDiagnostics::CreatureState);
    // AI was locked and switch was delayed to next update.
    if (HasCreatureState(CSTATE_INIT_AI_ON_UPDATE))
    {
        ClearCreatureState(CSTATE_INIT_AI_ON_UPDATE);
        AIM_Initialize();
    }

    if (HasCreatureState(CSTATE_EVADE_ON_UPDATE))
    {
        ClearCreatureState(CSTATE_EVADE_ON_UPDATE);
        if (IsInCombat() && AI())
            AI()->EnterEvadeMode();
    }

    switch (m_deathState)
    {
        case JUST_ALIVED:
            // Don't must be called, see Creature::SetDeathState JUST_ALIVED -> ALIVE promoting.
            sLog.outError("Creature (GUIDLow: %u Entry: %u ) in wrong state: JUST_ALIVED (4)", GetGUIDLow(), GetEntry());
            break;
        case JUST_DIED:
            // Don't must be called, see Creature::SetDeathState JUST_DIED -> CORPSE promoting.
            sLog.outError("Creature (GUIDLow: %u Entry: %u ) in wrong state: JUST_DEAD (1)", GetGUIDLow(), GetEntry());
            break;
        case DEAD:
        {
            if (m_respawnTime <= time(nullptr) && (!m_isSpawningLinked || GetMap()->GetCreatureLinkingHolder()->CanSpawn(this)))
            {
                DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "Respawning...");
                m_respawnTime = 0;
                lootForPickPocketed = false;
                lootForBody         = false;
                lootForSkin         = false;
                skinningForOthersTimer = 5000;

                // Clear possible auras having IsDeathPersistent() attribute
                RemoveAllAuras();

                // pick a new creature id if db spawn has multiple
                CreatureData const* dbSpawnData = GetCreatureData();
                uint32 const newCreatureId = dbSpawnData ? dbSpawnData->ChooseCreatureId() : m_originalEntry;

                if (newCreatureId != m_originalEntry)
                    m_originalEntry = newCreatureId;

                if (newCreatureId != GetEntry())
                {
                    // need to preserve game event state
                    GameEventCreatureData const* eventData = sGameEventMgr.GetCreatureUpdateDataForActiveEvent(GetGUIDLow());
                    UpdateEntry(newCreatureId, dbSpawnData, eventData);
                }

                CreatureInfo const *cinfo = GetCreatureInfo();

                SelectLevel(cinfo, dbSpawnData ? dbSpawnData->health_percent : 100.0f, dbSpawnData ? dbSpawnData->mana_percent : 100.0f);
                SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
                if (IsDeadByDefault())
                {
                    SetDeathState(JUST_DIED);
                    SetHealth(0);
                    i_motionMaster.Clear();
                    ClearUnitState(UNIT_STAT_ALL_DYN_STATES);
                    LoadCreatureAddon(true);
                }
                else
                    SetDeathState(JUST_ALIVED);

                // Call AI respawn virtual function
                if (AI())
                {
                    // If the creature AI needs to be re-initialized after respawn, do it now
                    // Useful for swapping AIs on mobs that change entry on respawn
                    if (HasCreatureState(CSTATE_INIT_AI_ON_RESPAWN))
                        AIM_Initialize();

                    AI()->JustRespawned();
                }
                
                if (m_zoneScript)
                    m_zoneScript->OnCreatureRespawn(this);

                if (m_isCreatureLinkingTrigger)
                    GetMap()->GetCreatureLinkingHolder()->DoCreatureLinkingEvent(LINKING_EVENT_RESPAWN, this);

                // 5 sec delay before aggro.
                if (!IsLikePlayer())
                    SetTempPacified(5000);

                // Scaling: apply to all dungeon/raid instances, linear vs. 40-player baseline
                if (GetMap()->IsDungeon())
                {
                    uint32 playerCount = GetMap()->GetPlayersCountExceptGMs();
                    uint32 maxCount = ((DungeonMap*)GetMap())->GetMaxPlayers();
                    if (playerCount > 0)
                        sAutoScaler->ScaleCreature(this, playerCount, maxCount, GetMap());
                }
                
                GetMap()->Add(this);

                if (uint16 poolid = sPoolMgr.IsPartOfAPool<Creature>(GetGUIDLow()))
                {
                    // Will eventually call AddObjectToRemoveList
                    sPoolMgr.UpdatePool<Creature>(*GetMap()->GetPersistentState(), poolid, GetGUIDLow());
                    return;
                }
            }
            break;
        }
        case CORPSE:
        {
            Unit::Update(update_diff, diff);
            TurtleDiagnostics::CreatureProbe::Stage(this, TurtleDiagnostics::CreatureState);
            if (IsDeadByDefault())
                break;

            // Youfie - <Nostalrius>
            // Cf. fix de Daemon [c1491] & mon autre bricolage de celui-ci [c1527)
            // Les mobs 11357, 8901, 14826 etc. : ont des minuscules temps de repop. Sans rajouter cette condition, tous les
            // mobs spawn via un event/script despawn (loots avec) au bout de genre 25s, sans qu'on puisse le changer dans la DB car pas de GUID fixe.
            if (m_corpseDecayTimer <= update_diff || (m_respawnTime <= time(nullptr) && GetDBTableGUIDLow() && !IsPet()))
            {
                if (IsInWorld())                            // can be despawned by update pool
                {
                    RemoveCorpse();
                    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "Removing corpse... %u ", GetEntry());
                }
            }
            else
            {
                if (skinningForOthersTimer <= update_diff)
                    skinningForOthersTimer = 0;
                else
                    skinningForOthersTimer -= update_diff;
                m_corpseDecayTimer -= update_diff;
                if (m_groupLootId)
                {
                    if (update_diff < m_groupLootTimer)
                        m_groupLootTimer -= update_diff;
                    else
                        StopGroupLoot();
                }
            }
            if (i_AI)
            {
                m_AI_locked = true;
                i_AI->UpdateAI_corpse(diff);
                m_AI_locked = false;
            }

            break;
        }
        case ALIVE:
        {
            if (IsDeadByDefault())
            {
                if (m_corpseDecayTimer <= update_diff)
                {
                    // since pool system can fail to roll unspawned object, this one can remain spawned, so must set respawn nevertheless
                    if (uint16 poolid = sPoolMgr.IsPartOfAPool<Creature>(GetGUIDLow()))
                        sPoolMgr.UpdatePool<Creature>(*GetMap()->GetPersistentState(), poolid, GetGUIDLow());

                    if (IsInWorld())                        // can be despawned by update pool
                    {
                        RemoveCorpse();
                        DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "Removing alive corpse... %u ", GetEntry());
                    }
                    else
                        return;
                }
                else
                    m_corpseDecayTimer -= update_diff;
            }

            if (m_pacifiedTimer)
            {
                if (m_pacifiedTimer <= update_diff)
                    m_pacifiedTimer = 0;
                else
                    m_pacifiedTimer -= update_diff;
            }

            Unit::Update(update_diff, diff);
            TurtleDiagnostics::CreatureProbe::Stage(this, TurtleDiagnostics::CreatureCombat);

            // creature can be dead after Unit::Update call
            // CORPSE/DEAD state will processed at next tick (in other case death timer will be updated unexpectedly)
            if (!IsAlive())
                break;

            float hpPercent = GetHealthPercent();
            ModifyAuraState(AURA_STATE_HEALTHLESS_15_PERCENT, hpPercent < 16.0f);
            ModifyAuraState(AURA_STATE_HEALTHLESS_10_PERCENT, hpPercent < 11.0f);
            ModifyAuraState(AURA_STATE_HEALTHLESS_5_PERCENT, hpPercent < 6.0f);

            bool unreachableTarget = false;
            bool leash = false;
            if (HasCreatureState(CSTATE_COMBAT))
            {
                if (GetMap()->GetLastMapUpdate() % 3000 <= update_diff)
                {
                    // Prevent mobs from evading while under crowd control.
                    if (HasUnitState(UNIT_STAT_NO_FREE_MOVE))
                        UpdateLeashExtensionTime();

                    // Leash prevents mobs from chasing any further than specified range
                    if (!m_leashingDisabled && m_leashDistance && !IsWithinDist3d(m_combatStartX, m_combatStartY, m_combatStartZ, m_leashDistance))
                        leash = true;
                    // Raid bosses do a periodic combat pulse
                    else if (HasCreatureState(CSTATE_COMBAT_WITH_ZONE))
                        SetInCombatWithZone(false);
                }

                if (GetVictim())
                {
                    if (m_callForHelpTimer <= update_diff)
                    {
                        m_callForHelpTimer = 1000;

                        float x, y, z;
                        GetRespawnCoord(x, y, z, nullptr, nullptr);
                        if (CallsForHelp() && GetDistance(x, y, z) > 10.0f)
                            CallForHelp(m_callForHelpDist);
                    }
                    else
                        m_callForHelpTimer -= update_diff;

                    unreachableTarget = !i_motionMaster.empty() &&
                        !HasExtraFlag(CREATURE_FLAG_EXTRA_NO_UNREACHABLE_EVADE) &&
                        i_motionMaster.GetCurrent()->GetMovementGeneratorType() == CHASE_MOTION_TYPE &&
                        !i_motionMaster.GetCurrent()->IsReachable() &&
                        !HasDistanceCasterMovement() && !GetCharmerOrOwnerGuid().IsPlayer() &&
                        (!CanReachWithMeleeAutoAttack(GetVictim()) || !IsWithinLOSInMap(GetVictim())) &&
                        // GetAntiCheat() is null for synthetic bot sessions;
                        // unguarded, the dereference crashes whenever a creature
                        // has a bot as its current victim. Treat null-anticheat
                        // as "not in knockback" (the safer default for bots).
                        !(GetVictim()->IsPlayer() && [&]() {
                            auto* ac = static_cast<Player*>(GetVictim())->GetSession()->GetAntiCheat();
                            return ac && ac->IsInKnockBack();
                        }());
                }
            }

            if (unreachableTarget)
            {
                m_TargetNotReachableTimer += update_diff;
                if (GetMapId() == 30 && CanHaveThreatList() && m_TargetNotReachableTimer > 1000) // Alterac Valley exploit fix
                    GetThreatManager().modifyThreatPercent(GetVictim(), -101);
            }
            else
                m_TargetNotReachableTimer = 0;

            if (AI())
            {
                TurtleDiagnostics::CreatureProbe::Stage(this, TurtleDiagnostics::CreatureScriptAI);
                // do not allow the AI to be changed during update
                m_AI_locked = true;
                try
                {
                    // Reset after 24 secs
                    if (leash || (m_TargetNotReachableTimer > 24000))
                        AI()->EnterEvadeMode();
                    else if (!IsEvadeBecauseTargetNotReachable())
                    {
                        TurtleDiagnostics::Scope diagnosticAI(TurtleDiagnostics::CreatureAI);
                        AI()->UpdateAI(diff);
                    }
                }
                catch (std::runtime_error& e)
                {
                    sLog.outInfo("[Unit/AI] Crash mob %u map %u", GetEntry(), GetMapId());
                    sLog.outInfo("%s", e.what());
                }
                m_AI_locked = false;
            }

            TurtleDiagnostics::CreatureProbe::Stage(this, TurtleDiagnostics::CreatureRegen);
            // creature can be dead after UpdateAI call
            // CORPSE/DEAD state will processed at next tick (in other case death timer will be updated unexpectedly)
            if (!IsAlive())
                break;

            RegenerateAll(update_diff, IsEvadeBecauseTargetNotReachable());
            break;
        }
        case CORPSE_FALLING:
        {
            SetDeathState(CORPSE);
        }
        default:
            break;
    }
}

void Creature::StartGroupLoot(Group* group, uint32 timer)
{
    m_groupLootId = group->GetId();
    m_groupLootTimer = timer;
}

void Creature::StopGroupLoot()
{
    if (!m_groupLootId)
        return;

    if (Group* group = sObjectMgr.GetGroupById(m_groupLootId))
        group->EndRoll(&loot);

    m_groupLootTimer = 0;
    m_groupLootId = 0;
}

void Creature::RegenerateAll(uint32 update_diff, bool skipCombatCheck)
{
    m_regenTimer -= update_diff;

    if (m_regenTimer > 0)
        return;

    if (!IsInCombat() || IsPolymorphed() || skipCombatCheck)
        RegenerateHealth();

    RegenerateMana();

    m_regenTimer = REGEN_TIME_FULL;
}

void Creature::RegenerateMana()
{
    if (!IsRegeneratingMana())
        return;

    uint32 curValue = GetPower(POWER_MANA);
    uint32 maxValue = GetMaxPower(POWER_MANA);

    if (curValue >= maxValue)
        return;

    uint32 addvalue = 0;

    // Combat and any controlled creature
    if (IsInCombat() || GetCharmerOrOwnerGuid())
    {
        if (!IsUnderLastManaUseEffect())
            addvalue = m_manaRegen;
    }
    else
        addvalue = maxValue / 3;

    ModifyPower(POWER_MANA, addvalue);
}

void Creature::RegenerateHealth()
{
    if (!IsRegeneratingHealth())
        return;

    uint32 curValue = GetHealth();
    uint32 maxValue = GetMaxHealth();

    if (curValue >= maxValue)
        return;

    uint32 addvalue = 0;

    // Not only pet, but any controlled creature
    if (GetCharmerOrOwnerGuid())
    {
        if (IsPolymorphed())
        {
            addvalue = maxValue / 10;
        }
        else
        {
            float HealthIncreaseRate = sWorld.getConfig(CONFIG_FLOAT_RATE_HEALTH);

            addvalue = uint32(GetRegenHPPerSpirit() * HealthIncreaseRate);
            addvalue *= 4; // The timer ticks every 4 seconds.
        }
    }
    else
    {
        addvalue = maxValue / 3;
    }

    if (addvalue < 0)
        addvalue = 0;

    ModifyHealth(addvalue);
}

bool Creature::DoFlee()
{
    /*
    Some observations from tests on classic regarding what happens if creature
    cant flee at the moment its health reaches the amount it should flee at.

    In these cases both emote and move is delayed:
    - Casting a spell.
    - Mind controlled.

    In these cases emote is shown instantly:
    - Stunned
    - Feared.
    */

    if (!GetVictim() || HasAuraType(SPELL_AURA_PREVENTS_FLEEING) ||
        HasUnitState(UNIT_STAT_FEIGN_DEATH | UNIT_STAT_POSSESSED | UNIT_STAT_DISTRACTED | UNIT_STAT_CONFUSED))
        return false;

    float hpPercent = GetHealthPercent();
    ModifyAuraState(AURA_STATE_HEALTHLESS_15_PERCENT, hpPercent < 16.0f);
    ModifyAuraState(AURA_STATE_HEALTHLESS_10_PERCENT, hpPercent < 11.0f);
    ModifyAuraState(AURA_STATE_HEALTHLESS_5_PERCENT, hpPercent < 6.0f);

    SetNoSearchAssistance(true);

    SetFleeing(true, GetVictim()->GetObjectGuid(), 0, sWorld.getConfig(CONFIG_UINT32_CREATURE_FAMILY_FLEE_DELAY));
    MonsterTextEmote(CREATURE_FLEE_TEXT, GetVictim());
    UpdateSpeed(MOVE_RUN, false);
    InterruptSpellsWithInterruptFlags(SPELL_INTERRUPT_FLAG_MOVEMENT);
    return true;
}

bool Creature::DoFleeToGetAssistance()
{
    if (!GetVictim() || HasAuraType(SPELL_AURA_PREVENTS_FLEEING) ||
        HasUnitState(UNIT_STAT_FEIGN_DEATH | UNIT_STAT_POSSESSED | UNIT_STAT_DISTRACTED | UNIT_STAT_CONFUSED))
        return false;

    float radius = sWorld.getConfig(CONFIG_FLOAT_CREATURE_FAMILY_FLEE_ASSISTANCE_RADIUS);

    if (radius > 0)
    {
        Creature* pCreature = nullptr;

        MaNGOS::NearestAssistCreatureInCreatureRangeCheck u_check(this, GetVictim(), radius);
        MaNGOS::CreatureLastSearcher<MaNGOS::NearestAssistCreatureInCreatureRangeCheck> searcher(pCreature, u_check);
        Cell::VisitGridObjects(this, searcher, radius);

        SetNoSearchAssistance(true);

        if (!pCreature)
            SetFleeing(true, GetVictim()->GetObjectGuid(), 0, sWorld.getConfig(CONFIG_UINT32_CREATURE_FAMILY_FLEE_DELAY));
        else
        {
            GetMotionMaster()->MoveSeekAssistance(pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ());
            SetTargetGuid(ObjectGuid());
        }
        MonsterTextEmote(CREATURE_FLEE_TEXT, GetVictim());
        UpdateSpeed(MOVE_RUN, false);
        InterruptSpellsWithInterruptFlags(SPELL_INTERRUPT_FLAG_MOVEMENT);
        return true;
    }

    return false;
}


float Creature::GetFleeingSpeed() const
{
    //TODO: There are different speeds for the different mobs, isn't there?
    return GetSpeed(MOVE_RUN);
}

void Creature::MoveAwayFromTarget(Unit* pTarget, float distance)
{
    if (HasUnitState(UNIT_STAT_NOT_MOVE | UNIT_STAT_CONFUSED | UNIT_STAT_LOST_CONTROL))
        return;

    GetMotionMaster()->MoveDistance(pTarget, distance);
    InterruptSpellsWithInterruptFlags(SPELL_INTERRUPT_FLAG_MOVEMENT);
}

bool Creature::AIM_Initialize()
{
    // make sure nothing can change the AI during AI update
    if (m_AI_locked)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "AIM_Initialize: failed to init, locked.");
        AddCreatureState(CSTATE_INIT_AI_ON_UPDATE);
        return false;
    }

    // Clear flag. Escort AI will set it if this creature is escortable
    SetEscortable(false);

    i_motionMaster.Initialize();

    CreatureAI * oldAI = i_AI;
    i_AI = FactorySelector::selectAI(this);

    delete oldAI;
    return true;
}

bool Creature::Create(uint32 guidlow, CreatureCreatePos& cPos, CreatureInfo const* cinfo, uint32 firstCreatureId, const CreatureData *data /*= nullptr*/, GameEventCreatureData const* eventData /*= nullptr*/)
{
    SetMap(cPos.GetMap());

    if (!CreateFromProto(guidlow, cinfo, firstCreatureId, data, eventData))
        return false;

    cPos.SelectFinalPoint(this);

    if (!cPos.Relocate(this))
        return false;

    switch (GetCreatureInfo()->rank)
    {
        case CREATURE_ELITE_RARE:
            m_corpseDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_RARE);
            break;
        case CREATURE_ELITE_ELITE:
            m_corpseDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_ELITE);
            break;
        case CREATURE_ELITE_RAREELITE:
            m_corpseDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_RAREELITE);
            break;
        case CREATURE_ELITE_WORLDBOSS:
            m_corpseDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_WORLDBOSS);
            break;
        default:
            m_corpseDelay = sWorld.getConfig(CONFIG_UINT32_CORPSE_DECAY_NORMAL);
            break;
    }

    // Add to CreatureLinkingHolder if needed
    if (sCreatureLinkingMgr.GetLinkedTriggerInformation(this))
        cPos.GetMap()->GetCreatureLinkingHolder()->AddSlaveToHolder(this);
    if (sCreatureLinkingMgr.IsLinkedEventTrigger(this))
    {
        m_isCreatureLinkingTrigger = true;
        cPos.GetMap()->GetCreatureLinkingHolder()->AddMasterToHolder(this);
    }

    LoadCreatureAddon();
    InitializeReactState();
    SetWalk(!HasExtraFlag(CREATURE_FLAG_EXTRA_ALWAYS_RUN), true);
    return true;
}

bool Creature::IsTrainerOf(Player* pPlayer, bool msg) const
{
    if (!IsTrainer())
        return false;

    TrainerSpellData const* cSpells = GetTrainerSpells();
    TrainerSpellData const* tSpells = GetTrainerTemplateSpells();

    // for not pet trainer expected not empty trainer list always
    if ((!cSpells || cSpells->spellList.empty()) && (!tSpells || tSpells->spellList.empty()))
    {
        sLog.outErrorDb("Creature %u (Entry: %u) have UNIT_NPC_FLAG_TRAINER but have empty trainer spell list.",
                        GetGUIDLow(), GetEntry());
        return false;
    }

    switch (GetCreatureInfo()->trainer_type)
    {
        case TRAINER_TYPE_CLASS:
            if (pPlayer->GetClass() != GetCreatureInfo()->trainer_class)
            {
                if (msg)
                {
                    pPlayer->PlayerTalkClass->ClearMenus();
                    switch (GetCreatureInfo()->trainer_class)
                    {
                        case CLASS_DRUID:
                            pPlayer->PlayerTalkClass->SendGossipMenu(4913, GetObjectGuid());
                            break;
                        case CLASS_HUNTER:
                            pPlayer->PlayerTalkClass->SendGossipMenu(10090, GetObjectGuid());
                            break;
                        case CLASS_MAGE:
                            pPlayer->PlayerTalkClass->SendGossipMenu(328, GetObjectGuid());
                            break;
                        case CLASS_PALADIN:
                            pPlayer->PlayerTalkClass->SendGossipMenu(1635, GetObjectGuid());
                            break;
                        case CLASS_PRIEST:
                            pPlayer->PlayerTalkClass->SendGossipMenu(4436, GetObjectGuid());
                            break;
                        case CLASS_ROGUE:
                            pPlayer->PlayerTalkClass->SendGossipMenu(4797, GetObjectGuid());
                            break;
                        case CLASS_SHAMAN:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5003, GetObjectGuid());
                            break;
                        case CLASS_WARLOCK:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5836, GetObjectGuid());
                            break;
                        case CLASS_WARRIOR:
                            pPlayer->PlayerTalkClass->SendGossipMenu(4985, GetObjectGuid());
                            break;
                    }
                }
                return false;
            }
            break;
        case TRAINER_TYPE_PETS:
            if (pPlayer->GetClass() != CLASS_HUNTER)
            {
                if (msg)
                {
                    pPlayer->PlayerTalkClass->ClearMenus();
                    pPlayer->PlayerTalkClass->SendGossipMenu(3620, GetObjectGuid());
                }
                return false;
            }
            break;
        case TRAINER_TYPE_MOUNTS:
            if (GetCreatureInfo()->trainer_race && pPlayer->GetRace() != GetCreatureInfo()->trainer_race)
            {
                // Allowed to train if exalted
                if (FactionTemplateEntry const* faction_template = GetFactionTemplateEntry())
                {
                    if (pPlayer->GetReputationRank(faction_template->faction) == REP_EXALTED)
                        return true;
                }

                if (msg)
                {
                    pPlayer->PlayerTalkClass->ClearMenus();
                    switch (GetCreatureInfo()->trainer_class)
                    {
                        case RACE_DWARF:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5865, GetObjectGuid());
                            break;
                        case RACE_GNOME:
                            pPlayer->PlayerTalkClass->SendGossipMenu(4881, GetObjectGuid());
                            break;
                        case RACE_HUMAN:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5861, GetObjectGuid());
                            break;
                        case RACE_NIGHTELF:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5862, GetObjectGuid());
                            break;
                        case RACE_ORC:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5863, GetObjectGuid());
                            break;
                        case RACE_TAUREN:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5864, GetObjectGuid());
                            break;
                        case RACE_TROLL:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5816, GetObjectGuid());
                            break;
                        case RACE_UNDEAD:
                            pPlayer->PlayerTalkClass->SendGossipMenu(624, GetObjectGuid());
                            break;
                        case RACE_HIGH_ELF:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5861, GetObjectGuid());
                            break;
                        case RACE_GOBLIN:
                            pPlayer->PlayerTalkClass->SendGossipMenu(5861, GetObjectGuid());
                            break;
                    }
                }
                return false;
            }
            break;
        case TRAINER_TYPE_TRADESKILLS:
            if (GetCreatureInfo()->trainer_spell && !pPlayer->HasSpell(GetCreatureInfo()->trainer_spell))
            {
                if (msg)
                {
                    pPlayer->PlayerTalkClass->ClearMenus();
                    pPlayer->PlayerTalkClass->SendGossipMenu(11031, GetObjectGuid());
                }
                return false;
            }
            break;
        default:
            return false;                                   // checked and error output at creature_template loading
    }
    return true;
}

bool Creature::CanInteractWithBattleMaster(Player* pPlayer, bool msg) const
{
    if (!IsBattleMaster())
        return false;

    BattleGroundTypeId bgTypeId = sBattleGroundMgr.GetBattleMasterBG(GetEntry());
    if (bgTypeId == BATTLEGROUND_TYPE_NONE)
        return false;

    if (!msg)
        return pPlayer->GetBGAccessByLevel(bgTypeId);

    if (!pPlayer->GetBGAccessByLevel(bgTypeId))
    {
        pPlayer->PlayerTalkClass->ClearMenus();
        switch (bgTypeId)
        {
            case BATTLEGROUND_AV:
                pPlayer->PlayerTalkClass->SendGossipMenu(7616, GetObjectGuid());
                break;
            case BATTLEGROUND_WS:
                pPlayer->PlayerTalkClass->SendGossipMenu(7599, GetObjectGuid());
                break;
            case BATTLEGROUND_AB:
                pPlayer->PlayerTalkClass->SendGossipMenu(7642, GetObjectGuid());
                break;
            default:
                break;
        }
        return false;
    }
    return true;
}

bool Creature::CanTrainAndResetTalentsOf(Player* pPlayer) const
{
    return pPlayer->GetLevel() >= 10
           && GetCreatureInfo()->trainer_type == TRAINER_TYPE_CLASS
           && pPlayer->GetClass() == GetCreatureInfo()->trainer_class;
}

/**
 * Return original player who tap creature, it can be different from player/group allowed to loot so not use it for loot code
 */
Player* Creature::GetOriginalLootRecipient() const
{
    return m_lootRecipientGuid ? ObjectAccessor::FindPlayer(m_lootRecipientGuid) : nullptr;
}

/**
 * Return group if player tap creature as group member, independent is player after leave group or stil be group member
 */
Group* Creature::GetGroupLootRecipient() const
{
    // original recipient group if set and not disbanded
    return m_lootGroupRecipientId ? sObjectMgr.GetGroupById(m_lootGroupRecipientId) : nullptr;
}

/**
 * Return player who can loot tapped creature (member of group or single player)
 *
 * In case when original player tap creature as group member then group tap prefered.
 * This is for example important if player after tap leave group.
 * If group not exist or disbanded or player tap creature not as group member return player
 */
Player* Creature::GetLootRecipient() const
{
    // original recipient group if set and not disbanded
    Group* group = GetGroupLootRecipient();

    // original recipient player if online
    Player* player = GetOriginalLootRecipient();

    // if group not set or disbanded return original recipient player if any
    if (!group)
        return player;

    // group case

    // return player if it still be in original recipient group
    if (player && player->GetGroup() == group)
        return player;

    // find any in group
    for (GroupReference *itr = group->GetFirstMember(); itr != nullptr; itr = itr->next())
        if (Player *p = itr->getSource())
            return p;

    return nullptr;
}

/**
 * Set player and group (if player group member) who tap creature
 */
void Creature::SetLootRecipient(Unit *unit)
{
    // set the player whose group should receive the right
    // to loot the creature after it dies
    // should be set to nullptr after the loot disappears

    if (!unit)
    {
        m_lootRecipientGuid.Clear();
        m_lootGroupRecipientId = 0;
        RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_LOOTABLE);
        return;
    }

    Player* player = unit->GetCharmerOrOwnerPlayerOrPlayerItself();
    if (!player)                                            // normal creature, no player involved
        return;

    // set player for non group case or if group will disbanded
    if (unit->IsPet() && player->GetPetGuid() == unit->GetObjectGuid())
        m_lootRecipientGuid = unit->GetObjectGuid();
    else
        m_lootRecipientGuid = player->GetObjectGuid();

    // set group for group existing case including if player will leave group at loot time
    if (Group* group = player->GetGroup())
        m_lootGroupRecipientId = group->GetId();

    SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TAPPED);
}

// return true if this creature is tapped by the player or by a member of his group.
bool Creature::IsTappedBy(Player const* player) const
{
    ASSERT(player);

    if (player->GetObjectGuid() == m_lootRecipientGuid)
        return true;

    if (m_lootRecipientGuid && player->GetPetGuid() == m_lootRecipientGuid)
        return true;

    Group const* playerGroup = player->GetGroup();
    if (playerGroup && playerGroup->isBGGroup())
        return true;

    if (!playerGroup || playerGroup != GetGroupLootRecipient()) // if we dont have a group we arent the recipient
        return false;                                           // if creature doesnt have group bound it means it was solo killed by someone else

    return true;
}

// checked for trading of soulbound items within raid
bool Creature::WasPlayerPresentAtDeath(Player const* player) const
{
    return m_playersPresentAtDeath.find(player->GetObjectGuid()) != m_playersPresentAtDeath.end();
}

void Creature::SaveToDB()
{
    // this should only be used when the creature has already been loaded
    // preferably after adding to map, because mapid may not be valid otherwise
    CreatureData const *data = sObjectMgr.GetCreatureData(GetGUIDLow());
    if (!data)
    {
        sLog.outError("Creature::SaveToDB failed, cannot get creature data!");
        return;
    }

    SaveToDB(GetMapId());
}

void Creature::SaveToDB(uint32 mapid)
{
    // update in loaded data
    CreatureData& data = sObjectMgr.NewOrExistCreatureData(GetGUIDLow());

    // data->guid = guid must not be updated at save
    data.creature_id[0] = GetEntry();
    data.position.mapId = mapid;
    data.position.x = GetPositionX();
    data.position.y = GetPositionY();
    data.position.z = GetPositionZ();
    data.position.o = GetOrientation();
    data.spawntimesecsmin = m_respawnDelay;
    data.spawntimesecsmax = m_respawnDelay;
    data.wander_distance = GetDefaultMovementType() == IDLE_MOTION_TYPE ? 0.0f : m_wanderDistance;
    data.movement_type = m_wanderDistance == 0.0f && GetDefaultMovementType() == RANDOM_MOTION_TYPE ? IDLE_MOTION_TYPE : GetDefaultMovementType();
    if (m_isActiveObject)
        data.spawn_flags |= SPAWN_FLAG_ACTIVE;

    // updated in DB
    std::ostringstream ss;
    ss << "REPLACE INTO `creature` VALUES ("
       << GetGUIDLow() << ","
       << data.creature_id[0] << ","
       << data.creature_id[1] << ","
       << data.creature_id[2] << ","
       << data.creature_id[3] << ","
       << mapid << ","
       << GetPositionX() << ","
       << GetPositionY() << ","
       << GetPositionZ() << ","
       << GetOrientation() << ","
       << data.spawntimesecsmin << ","                     // respawn time minimum
       << data.spawntimesecsmax << ","                     // respawn time maximum
       << data.wander_distance << ","                      // wander distance
       << data.health_percent << ","                       // health_percent
       << data.mana_percent << ","                         // mana_percent
       << GetDefaultMovementType() << ","                  // default movement generator type
       << data.spawn_flags << ","
       << m_visibilityModifier << ")";
    sWorld.ExecuteUpdate("%s", ss.str().c_str());
}

void Creature::CheckLootDistance(float& distance) const 
{
    //TODO: Move this to DB.
    switch (GetEntry())
    {
    case 1853: // Darkmaster Gandling
        distance = 120.f;
        break;
    }
}

void Creature::SelectLevel(const CreatureInfo *cinfo, float percentHealth, float percentMana)
{
    uint32 rank = IsPet() ? 0 : cinfo->rank;

    // level
    uint32 minlevel = std::min(cinfo->level_max, cinfo->level_min);
    uint32 maxlevel = std::max(cinfo->level_max, cinfo->level_min);
    uint32 level = minlevel == maxlevel ? minlevel : urand(minlevel, maxlevel);
    SetLevel(level);

    float rellevel = maxlevel == minlevel ? 0 : (float(level - minlevel)) / (maxlevel - minlevel);

    // health
    float healthmod = _GetHealthMod(rank);

    uint32 minhealth = std::min(cinfo->health_max, cinfo->health_min);
    uint32 maxhealth = std::max(cinfo->health_max, cinfo->health_min);
    uint32 health = uint32(healthmod * (minhealth + uint32(rellevel * (maxhealth - minhealth))));

    if (sWorld.IsAprilFools())
    {
        if (cinfo->type == CREATURE_TYPE_CRITTER)
            health = 100'000'000;
    }

    SetCreateHealth(health);
    SetMaxHealth(health);

    if (percentHealth == 100.0f)
        SetHealth(health);
    else
        SetHealthPercent(percentHealth);

    // mana
    uint32 minmana = std::min(cinfo->mana_max, cinfo->mana_min);
    uint32 maxmana = std::max(cinfo->mana_max, cinfo->mana_min);
    uint32 mana = minmana + uint32(rellevel * (maxmana - minmana));

    SetCreateMana(mana);
    SetMaxPower(POWER_MANA, mana);

    if (percentMana == 100.0f)
        SetPower(POWER_MANA, mana);
    else
        SetPowerPercent(POWER_MANA, percentMana);

    SetModifierValue(UNIT_MOD_HEALTH, BASE_VALUE, float(health));
    SetModifierValue(UNIT_MOD_MANA, BASE_VALUE, float(mana));

    // damage
    float damagemod = _GetDamageMod(rank);

    SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, cinfo->dmg_min * damagemod);
    SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, cinfo->dmg_max * damagemod);

    SetBaseWeaponDamage(OFF_ATTACK, MINDAMAGE, cinfo->dmg_min * damagemod);
    SetBaseWeaponDamage(OFF_ATTACK, MAXDAMAGE, cinfo->dmg_max * damagemod);

    SetBaseWeaponDamage(RANGED_ATTACK, MINDAMAGE, cinfo->ranged_dmg_min * damagemod);
    SetBaseWeaponDamage(RANGED_ATTACK, MAXDAMAGE, cinfo->ranged_dmg_max * damagemod);
}

float Creature::_GetHealthMod(int32 rank)
{
    switch (rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_NORMAL_HP);
        case CREATURE_ELITE_ELITE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_HP);
        case CREATURE_ELITE_RAREELITE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_HP);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_HP);
        case CREATURE_ELITE_RARE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_HP);
        default:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_HP);
    }
}

float Creature::_GetDamageMod(int32 rank)
{
    switch (rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_NORMAL_DAMAGE);
        case CREATURE_ELITE_ELITE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_DAMAGE);
        case CREATURE_ELITE_RAREELITE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_DAMAGE);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_DAMAGE);
        case CREATURE_ELITE_RARE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_DAMAGE);
        default:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_DAMAGE);
    }
}

float Creature::_GetSpellDamageMod(int32 rank)
{
    switch (rank)                                           // define rates for each elite rank
    {
        case CREATURE_ELITE_NORMAL:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_NORMAL_SPELLDAMAGE);
        case CREATURE_ELITE_ELITE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE);
        case CREATURE_ELITE_RAREELITE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_RAREELITE_SPELLDAMAGE);
        case CREATURE_ELITE_WORLDBOSS:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_WORLDBOSS_SPELLDAMAGE);
        case CREATURE_ELITE_RARE:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_RARE_SPELLDAMAGE);
        default:
            return sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_ELITE_ELITE_SPELLDAMAGE);
    }
}

bool Creature::CreateFromProto(uint32 guidlow, CreatureInfo const* cinfo, uint32 firstCreatureId, const CreatureData *data /*=nullptr*/, GameEventCreatureData const* eventData /*=nullptr*/)
{
    SetZoneScript();
    m_originalEntry = cinfo->entry;

    Object::_Create(guidlow, firstCreatureId, cinfo->GetHighGuid());

    return UpdateEntry(cinfo->entry, data, eventData, false);
}

bool Creature::LoadFromDB(uint32 guidlow, Map *map, bool force)
{
    CreatureData const* data = sObjectMgr.GetCreatureData(guidlow);

    if (!data)
    {
        sLog.outErrorDb("Creature (GUID: %u) not found in table `creature`, can't load. ", guidlow);
        return false;
    }

    if (!force && (data->spawn_flags & SPAWN_FLAG_DISABLED))
        return false;

    uint32 const creatureId = data->ChooseCreatureId();
    CreatureInfo const *cinfo = sObjectMgr.GetCreatureTemplate(creatureId);
    if (!cinfo)
    {
        sLog.outErrorDb("Creature (Entry: %u) not found in table `creature_template`, can't load. ", creatureId);
        return false;
    }

    GameEventCreatureData const* eventData = sGameEventMgr.GetCreatureUpdateDataForActiveEvent(guidlow);

    // Creature can be loaded already in map if grid has been unloaded while creature walk to another grid
    if (map->GetCreature(cinfo->GetObjectGuid(guidlow)))
        return false;

    CreatureCreatePos pos(map, data->position.x, data->position.y, data->position.z, data->position.o);
    SetHomePosition(data->position.x, data->position.y, data->position.z, data->position.o);

    if (!Create(guidlow, pos, cinfo, data->creature_id[0], data, eventData))
        return false;

    m_wanderDistance = data->wander_distance;

    m_respawnDelay = data->GetRandomRespawnTime();
    m_deathState = data->spawn_flags & SPAWN_FLAG_DEAD ? DEAD : ALIVE;

    if (data->spawn_flags & SPAWN_FLAG_ACTIVE)
        m_isActiveObject = true;

    if (data->visibility_mod)
        m_visibilityModifier = data->visibility_mod;

    m_respawnTime  = map->GetPersistentState()->GetCreatureRespawnTime(GetGUIDLow());

    if (m_respawnTime > time(nullptr))                         // not ready to respawn
    {
        m_deathState = DEAD;
        if (CanFly())
        {
            float tz = GetMap()->GetHeight(data->position.x, data->position.y, data->position.z, false);
            if (data->position.z - tz > 0.1)
                Relocate(data->position.x, data->position.y, tz);
        }
    }
    else if (m_respawnTime)                                 // respawn time set but expired
    {
        m_respawnTime = 0;

        GetMap()->GetPersistentState()->SaveCreatureRespawnTime(GetGUIDLow(), 0);
    }

    uint32 curhealth = cinfo->health_max * data->health_percent / 100.0f;
    if (curhealth)
    {
        curhealth = uint32(curhealth * _GetHealthMod(GetCreatureInfo()->rank));
        if (curhealth < 1)
            curhealth = 1;
    }

    uint32 curmana = cinfo->mana_max * data->mana_percent / 100.0f;

    if (sCreatureLinkingMgr.IsSpawnedByLinkedMob(this))
    {
        m_isSpawningLinked = true;
        if (m_deathState == ALIVE && !GetMap()->GetCreatureLinkingHolder()->CanSpawn(this))
        {
            m_deathState = DEAD;

            // Just set to dead, so need to relocate like above
            if (CanFly())
            {
                float tz = GetTerrain()->GetHeightStatic(data->position.x, data->position.y, data->position.z, false);
                if (data->position.z - tz > 0.1)
                    Relocate(data->position.x, data->position.y, tz);
            }
        }
    }

    SetHealth(m_deathState == ALIVE ? curhealth : 0);
    SetPower(POWER_MANA, curmana);

    // checked at creature_template loading
    m_defaultMovementType = MovementGeneratorType(data->movement_type);
    m_isDeadByDefault = (data->spawn_flags & SPAWN_FLAG_DEAD) != 0;

    // Creature Linking, Initial load is handled like respawn
    if (m_isCreatureLinkingTrigger && IsAlive())
        GetMap()->GetCreatureLinkingHolder()->DoCreatureLinkingEvent(LINKING_EVENT_RESPAWN, this);

    if (data->spawn_flags & SPAWN_FLAG_NOT_VISIBLE)
        SetVisibility(VISIBILITY_OFF);

    // We need to assign new AI on respawn if spawn has multiple creature ids
    if (data->GetCreatureIdCount() > 1)
        AddCreatureState(CSTATE_INIT_AI_ON_RESPAWN);

    return true;
}

void Creature::LoadEquipment(uint32 equip_entry, bool force)
{
    if (equip_entry == 0)
    {
        if (force)
        {
            for (uint8 i = 0; i < MAX_VIRTUAL_ITEM_SLOT; ++i)
                SetVirtualItem(VirtualItemSlot(i), 0);
            m_equipmentId = 0;
        }
        return;
    }

    if (EquipmentInfo const *einfo = sObjectMgr.GetEquipmentInfo(equip_entry))
    {
        m_equipmentId = equip_entry;
        for (uint8 i = 0; i < MAX_VIRTUAL_ITEM_SLOT; ++i)
            SetVirtualItem(VirtualItemSlot(i), einfo->equipentry[i]);
    }
}

bool Creature::HasQuest(uint32 quest_id) const
{
    QuestRelationsMapBounds bounds = sObjectMgr.GetCreatureQuestRelationsMapBounds(GetEntry());
    for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}

bool Creature::HasInvolvedQuest(uint32 quest_id) const
{
    QuestRelationsMapBounds bounds = sObjectMgr.GetCreatureQuestInvolvedRelationsMapBounds(GetEntry());
    for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
    {
        if (itr->second == quest_id)
            return true;
    }
    return false;
}


struct CreatureRespawnDeleteWorker
{
    explicit CreatureRespawnDeleteWorker(uint32 guid) : i_guid(guid) {}

    void operator()(MapPersistentState* state) const
    {
        state->SaveCreatureRespawnTime(i_guid, 0);
    }

    uint32 i_guid;
};

void Creature::DeleteFromDB()
{
    CreatureData const* data = sObjectMgr.GetCreatureData(GetGUIDLow());
    if (!data)
    {
        DEBUG_LOG("Trying to delete not saved creature!");
        return;
    }

    DeleteFromDB(GetGUIDLow(), data);
}

void Creature::DeleteFromDB(uint32 lowguid, CreatureData const* data)
{
    auto instanceId = sMapMgr.GetContinentInstanceId(data->position.mapId, data->position.x, data->position.y);
    CreatureRespawnDeleteWorker worker(lowguid);
    sMapPersistentStateMgr.DoForAllStatesWithMapId(data->position.mapId, instanceId, worker);

    sObjectMgr.DeleteCreatureData(lowguid);

    WorldDatabase.BeginTransaction();
    sWorld.ExecuteUpdate("DELETE FROM creature WHERE guid=%u", lowguid);
    sWorld.ExecuteUpdate("DELETE FROM creature_addon WHERE guid=%u", lowguid);
    sWorld.ExecuteUpdate("DELETE FROM creature_movement WHERE id=%u", lowguid);
    sWorld.ExecuteUpdate("DELETE FROM game_event_creature WHERE guid=%u", lowguid);
    sWorld.ExecuteUpdate("DELETE FROM game_event_creature_data WHERE guid=%u", lowguid);
    sWorld.ExecuteUpdate("DELETE FROM creature_battleground WHERE guid=%u", lowguid);
    WorldDatabase.CommitTransaction();
}

float Creature::GetAttackDistance(Unit const* pTarget) const
{
    float const aggroRate = sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO);
    if (aggroRate == 0)
        return 0.0f;

    // proximity aggro does not target disabled pets (owner is mounted)
    if (pTarget->IsPet() && !static_cast<Pet const*>(pTarget)->IsEnabled())
        return 0.0f;

    // pets and charmed mobs use owner level
    Player* pPlayer = pTarget->GetCharmerOrOwnerPlayer();
    uint32 const targetlevel = pPlayer ? pPlayer->GetLevelForTarget(this) : pTarget->GetLevelForTarget(this);
    uint32 const creaturelevel = GetLevelForTarget(pPlayer ? pPlayer : pTarget);

    int32 leveldif = int32(targetlevel) - int32(creaturelevel);

    // "The maximum Aggro Radius has a cap of 25 levels under. Example: A level 30 char has the same Aggro Radius of a level 5 char on a level 60 mob."
    if (leveldif < - MAX_LEVEL_DIFF_FOR_AGGRO_RANGE)
        leveldif = - MAX_LEVEL_DIFF_FOR_AGGRO_RANGE;

    // "The aggro radius of a mob having the same level as the player is roughly 18 yards"
    float const detectionRange = GetDetectionRange();
    float finalDistance = detectionRange;
    if (finalDistance < 1)
        return 0.0f;

    // "Aggro Radius varies with level difference at a rate of roughly 1 yard/level"
    // radius grow if playlevel < creaturelevel
    finalDistance -= (float)leveldif;

    // detect range auras
    // SPELL_AURA_MOD_DETECT_RANGE: For exemple [2908 - Soothe Animal]. Affects only if level < 70 (rank 3).
    AuraList const& nModDetectRange = GetAurasByType(SPELL_AURA_MOD_DETECT_RANGE);
    for (const auto i : nModDetectRange)
        if (i->GetSpellProto()->MaxTargetLevel >= GetLevel())
            finalDistance += i->GetModifier()->m_amount;

    // detected range auras
    finalDistance += pTarget->GetTotalAuraModifier(SPELL_AURA_MOD_DETECTED_RANGE);

    // "Minimum Aggro Radius for a mob seems to be combat range (5 yards)"
    float const minDistance = std::min(detectionRange, 5.0f);
    if (finalDistance < minDistance)
        finalDistance = minDistance;

    return (finalDistance * aggroRate);
}

void Creature::SetDeathState(DeathState s)
{
    if ((s == JUST_DIED && !IsDeadByDefault()) || (s == JUST_ALIVED && IsDeadByDefault()))
    {
        auto data = sObjectMgr.GetCreatureData(GetGUIDLow());

        uint32 respawnDelay = m_respawnDelay;
        ApplyDynamicRespawnDelay(respawnDelay, data);

        // the max/default time for corpse decay (before creature is looted/AllLootRemovedFromCorpse() is called)
        m_corpseDecayTimer = HasExtraFlag(CREATURE_FLAG_EXTRA_DESPAWN_INSTANTLY) ? 1 : m_corpseDelay * IN_MILLISECONDS;

        if (data)
        {
            if (data->spawn_flags & SPAWN_FLAG_RANDOM_RESPAWN_TIME)
                respawnDelay *= float(urand(90, 110)) / 100.0f;
            if (data->spawn_flags & SPAWN_FLAG_DYNAMIC_RESPAWN_TIME && sWorld.GetActiveSessionCount() > sWorld.m_dynamicRespawnRatio)
                respawnDelay *= sWorld.m_dynamicRespawnRatio;
        }
        m_respawnTime = time(nullptr) + respawnDelay;        // respawn delay (spawntimesecs)

        // always save boss respawn time at death to prevent crash cheating
        if (sWorld.getConfig(CONFIG_BOOL_SAVE_RESPAWN_TIME_IMMEDIATELY) || IsWorldBoss())
            SaveRespawnTime();
    }

    Unit::SetDeathState(s);

    if (s == JUST_DIED)
    {
        // Turtle: Store players in map during raid creature death,
        // to allow trading of soulbound items among eligible players.
        if (IsInWorld() && FindMap() && FindMap()->IsRaid())
        {
            Map::PlayerList const& players = FindMap()->GetPlayers();
            for (auto const& itr : players)
                m_playersPresentAtDeath.insert(itr.getSource()->GetObjectGuid());
        }

        SetTargetGuid(ObjectGuid());                        // remove target selection in any cases (can be set at aura remove in Unit::SetDeathState)
        SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);

        if (!IsPet() && GetCreatureInfo()->skinning_loot_id)
            if (LootTemplates_Skinning.HaveLootFor(GetCreatureInfo()->skinning_loot_id))
                if (HasLootRecipient())
                    SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

        if (HasSearchedAssistance())
        {
            SetNoSearchAssistance(false);
            UpdateSpeed(MOVE_RUN, false);
        }

        // Return, since we promote to CORPSE_FALLING. CORPSE_FALLING is promoted to CORPSE at next update.
        if (!HasCreatureState(CSTATE_DESPAWNING) && CanFly() && FallGround())
            return;

        Unit::SetDeathState(CORPSE);
    }

    if (s == JUST_ALIVED)
    {
        m_playersPresentAtDeath.clear();

        ClearUnitState(UNIT_STAT_ALL_DYN_STATES);

        CreatureInfo const *cinfo = GetCreatureInfo();

        if (!GetHealth())
            SetHealth(GetMaxHealth());

        SetLootRecipient(nullptr);

        if (GetTemporaryFactionFlags() & TEMPFACTION_RESTORE_RESPAWN)
            ClearTemporaryFaction();

        Unit::SetDeathState(ALIVE);

        SetMeleeDamageSchool(SpellSchools(cinfo->dmg_school));

        // Dynamic flags may be adjusted by spells. Clear them
        // first and let spell from *addon apply where needed.
        SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
        LoadCreatureAddon(true);

        // Flags after LoadCreatureAddon. Any spell in *addon
        // will not be able to adjust these.
        SetUInt32Value(UNIT_NPC_FLAGS, cinfo->npc_flags);
        RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

        SetWalk(!HasExtraFlag(CREATURE_FLAG_EXTRA_ALWAYS_RUN), true);
        i_motionMaster.Initialize();
    }
}

bool Creature::FallGround()
{
    // Only if state is JUST_DIED. CORPSE_FALLING is set below and promoted to CORPSE later
    if (GetDeathState() != JUST_DIED)
        return false;

    // use larger distance for vmap height search than in most other cases
    float tz = GetMap()->GetHeight(GetPositionX(), GetPositionY(), GetPositionZ(), true, MAX_FALL_DISTANCE);

    if (tz <= INVALID_HEIGHT)
    {
        DEBUG_LOG("FallGround: creature %u at map %u (x: %f, y: %f, z: %f), not able to retrive a proper GetHeight (z: %f).",
                  GetEntry(), GetMap()->GetId(), GetPositionX(), GetPositionX(), GetPositionZ(), tz);
        return false;
    }

    float Z = fabs(GetPositionZ() - tz);
    // Abort too if the ground is very near
    if (Z < 0.1f || Z >= fabs(INVALID_HEIGHT))
        return false;

    Unit::SetDeathState(CORPSE_FALLING);

    // For creatures that are moving towards target and dies, the visual effect is not nice.
    // It is possibly caused by a xyz mismatch in DestinationHolder's GetLocationNow and the location
    // of the mob in client. For mob that are already reached target or dies while not moving
    // the visual appear to be fairly close to the expected.

    Movement::MoveSplineInit init(*this, "FallGround");
    init.MoveTo(GetPositionX(), GetPositionY(), tz);
    init.SetFall();
    init.Launch();
    return true;
}

void Creature::CastSpawnSpell()
{
    if (GetCreatureInfo()->spawn_spell_id)
    {
        SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
        SpellCastResult result = CastSpell(this, GetCreatureInfo()->spawn_spell_id, false);
        if (result != SPELL_CAST_OK)
        {
            RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
            sLog.outError("%s failed to cast spawn spell %u due to reason %u.", GetGuidStr().c_str(), GetCreatureInfo()->spawn_spell_id, result);
        }
    }
}

void Creature::Respawn()
{
    RemoveCorpse();

    // forced recreate creature object at clients
    UnitVisibility currentVis = GetVisibility();
    SetVisibility(VISIBILITY_RESPAWN);
    SetUnitMovementFlags(MOVEFLAG_NONE);
    CheckPendingVisibilityAndViewUpdate();                  // will call UpdateObjectVisibility
    SetVisibility(currentVis);                              // restore visibility state
    CheckPendingVisibilityAndViewUpdate();                  // will call UpdateObjectVisibility

    if (IsDespawned())
    {
        if (HasStaticDBSpawnData())
            GetMap()->GetPersistentState()->SaveCreatureRespawnTime(GetGUIDLow(), 0);
        m_respawnTime = time(nullptr);                         // respawn at next tick
    }
}

void Creature::DespawnOrUnsummon(uint32 msTimeToDespawn /*= 0*/, uint32 secsTimeToRespawn /*= 0*/)
{
    if (IsTemporarySummon())
        static_cast<TemporarySummon*>(this)->UnSummon(msTimeToDespawn);
    else if (IsPet())
        static_cast<Pet*>(this)->DelayedUnsummon(msTimeToDespawn, PET_SAVE_AS_DELETED);
    else
        ForcedDespawn(msTimeToDespawn, secsTimeToRespawn);
}

void Creature::ForcedDespawn(uint32 msTimeToDespawn /*= 0*/, uint32 secsTimeToRespawn /*= 0*/)
{
    AddCreatureState(CSTATE_DESPAWNING);

    if (msTimeToDespawn)
    {
        ForcedDespawnDelayEvent* pEvent = new ForcedDespawnDelayEvent(*this, secsTimeToRespawn);

        m_Events.AddEvent(pEvent, m_Events.CalculateTime(msTimeToDespawn));
        return;
    }

    uint32 oldRespawnDelay;
    if (secsTimeToRespawn)
    {
        oldRespawnDelay = m_respawnDelay;
        m_respawnDelay = secsTimeToRespawn;
    }

    if (IsAlive())
        SetDeathState(JUST_DIED);

    if (secsTimeToRespawn)
        m_respawnDelay = oldRespawnDelay;

    RemoveCorpse();
    SetHealth(0);                                           // just for nice GM-mode view
    ClearCreatureState(CSTATE_DESPAWNING);
}

bool Creature::IsImmuneToSpell(SpellEntry const *spellInfo, bool castOnSelf) const
{
    if (!spellInfo)
        return false;

    if (!castOnSelf)
    {
        if (spellInfo->Mechanic && GetCreatureInfo()->mechanic_immune_mask & (1 << (spellInfo->Mechanic - 1)))
            return true;

        if ((GetCreatureInfo()->school_immune_mask & (1 << spellInfo->School)) && !spellInfo->IsPositiveSpell())
            return true;
    }

    // HACK!
    if (IsWorldBoss())
    {
        if (spellInfo->IsFitToFamily<SPELLFAMILY_HUNTER, CF_HUNTER_SCORPID_STING>())
            return true;

        switch (spellInfo->Id)
        {
            case 67:              // Vindication
            case 26017:
            case 26018:
                return true;
        }
    }

    return Unit::IsImmuneToSpell(spellInfo, castOnSelf);
}

bool Creature::IsImmuneToDamage(SpellSchoolMask meleeSchoolMask, SpellEntry const* spellInfo) const
{
    if (GetCreatureInfo()->school_immune_mask & meleeSchoolMask)
        return true;

    return Unit::IsImmuneToDamage(meleeSchoolMask, spellInfo);
}

bool Creature::IsImmuneToSpellEffect(SpellEntry const *spellInfo, SpellEffectIndex index, bool castOnSelf) const
{
    if (!castOnSelf && spellInfo->EffectMechanic[index] && GetCreatureInfo()->mechanic_immune_mask & (1 << (spellInfo->EffectMechanic[index] - 1)))
        return true;

    return Unit::IsImmuneToSpellEffect(spellInfo, index, castOnSelf);
}

bool Creature::IsVisibleInGridForPlayer(Player const* pl) const
{
    // gamemaster in GM mode see all, including ghosts
    if (pl->IsGameMaster())
        return true;

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_INVISIBLE))
        return false;

    // Visilibity based on the quest status:
    uint32 phaseQuestId = GetPhaseQuestId();
    if (phaseQuestId)
    {
        auto status = pl->GetQuestStatusData(phaseQuestId);
        if (!status || !status->m_rewarded)
            return false;
    }

    // If players is alive or haven't released spirit yet
    if (pl->IsAlive() || pl->GetDeathTimer() > 0)
        return (IsAlive() || m_corpseDecayTimer > 0 || (IsDeadByDefault() && m_deathState == CORPSE));

    // Dead player see live creatures near own corpse
    if (IsAlive())
    {
        Corpse *corpse = pl->GetCorpse();
        if (corpse)
        {
            // 20 - aggro distance for same level, 25 - max additional distance if player level less that creature level
            if (corpse->IsWithinDistInMap(this, (20 + 25)*sWorld.getConfig(CONFIG_FLOAT_RATE_CREATURE_AGGRO)))
                return true;
        }
    }

    // Dead player can see ghosts:
    if (GetCreatureInfo()->type_flags & CREATURE_TYPEFLAGS_GHOST_VISIBLE)
        return true;

    // and not see any other
    return false;
}

void Creature::SendAIReaction(AiReaction reactionType)
{
    WorldPacket data(SMSG_AI_REACTION, 12);

    data << GetObjectGuid();
    data << uint32(reactionType);

    ((WorldObject*)this)->SendObjectMessageToSet(&data, true);

    DEBUG_FILTER_LOG(LOG_FILTER_AI_AND_MOVEGENSS, "WORLD: Sent SMSG_AI_REACTION, type %u.", reactionType);
}

void Creature::CallAssistance()
{
    if (!HasCreatureState(CSTATE_ALREADY_CALL_ASSIST) && GetVictim() && !IsPet() && !IsCharmed())
    {
        SetNoCallAssistance(true);

        float radius = sWorld.getConfig(CONFIG_FLOAT_CREATURE_FAMILY_ASSISTANCE_RADIUS);
        if (radius > 0)
        {
            std::list<Creature*> assistList;

            MaNGOS::AnyAssistCreatureInRangeCheck u_check(this, GetVictim(), radius);
            MaNGOS::CreatureListSearcher<MaNGOS::AnyAssistCreatureInRangeCheck> searcher(assistList, u_check);
            Cell::VisitGridObjects(this, searcher, radius);

            if (!GetVictim()) // May be invalidated by AI at grid activation ...
                return;

            if (!assistList.empty())
            {
                AssistDelayEvent *e = new AssistDelayEvent(GetVictim()->GetObjectGuid(), *this, assistList);
                m_Events.AddEvent(e, m_Events.CalculateTime(sWorld.getConfig(CONFIG_UINT32_CREATURE_FAMILY_ASSISTANCE_DELAY)));
            }
        }
    }
}

void Creature::CallForHelp(float fRadius)
{
    if (fRadius <= 0.0f || !GetVictim() || IsPet() || IsCharmed())
        return;

    MaNGOS::CallOfHelpCreatureInRangeDo u_do(this, GetVictim(), fRadius);
    MaNGOS::CreatureWorker<MaNGOS::CallOfHelpCreatureInRangeDo> worker(this, u_do);
    Cell::VisitGridObjects(this, worker, fRadius);
}

bool Creature::CanAssistTo(const Unit* u, const Unit* enemy, bool checkfaction /*= true*/) const
{
    if (!IsAlive())
        return false;

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_NO_ASSIST))
        return false;

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_NO_AGGRO))
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_IMMUNE_TO_NPC))
        return false;

    // skip fighting creature
    if (IsInCombat())
        return false;

    // only free creature
    if (GetCharmerOrOwnerGuid())
        return false;

    // Invisible
    if (m_invisibilityMask)
        return false;

    if (IsInEvadeMode())
        return false;

    // only from same creature faction
    if (checkfaction)
    {
        if (GetFactionTemplateId() != u->GetFactionTemplateId())
            return false;
    }
    else
    {
        if (!IsFriendlyTo(u))
            return false;
    }

    // skip non hostile to caster enemy creatures
    if (!IsHostileTo(enemy))
        return false;

    return true;
}

bool Creature::CanInitiateAttack()
{
    if (HasUnitState(UNIT_STAT_STUNNED | UNIT_STAT_PENDING_STUNNED | UNIT_STAT_FEIGN_DEATH))
        return false;

    if (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING | UNIT_FLAG_NOT_SELECTABLE))
        return false;

    if (IsPassiveToHostile())
        return false;

    if (IsTempPacified())
        return false;

    if (HasExtraFlag(CREATURE_FLAG_EXTRA_NO_AGGRO))
        return false;

    return true;
}

class DynamicRespawnRatesPlayerChecker
{
public:
    DynamicRespawnRatesPlayerChecker(Creature* crea) : _count(0), _hasNearbyEscort(false)
    {
        _myLevel = crea->GetLevel();
        _maxLevelDiff = sWorld.getConfig(CONFIG_UINT32_DYN_RESPAWN_PLAYERS_LEVELDIFF);
    }

    void operator()(Player* player)
    {
        if (_hasNearbyEscort || player->GetEscortingGuid())
        {
            _hasNearbyEscort = true;
            return;
        }

        if (uint32(abs(int32(player->GetLevel()) - (int32)_myLevel)) > _maxLevelDiff)
            return;

        ++_count;
    }

    uint32 GetCount() const { return _count; }
    bool HasNearbyEscort() const { return _hasNearbyEscort; }
private:
    uint32 _count;
    uint32 _myLevel;
    uint32 _maxLevelDiff;
    bool _hasNearbyEscort;
};

class DynamicRespawnRatesCreatureChecker
{
public:
    DynamicRespawnRatesCreatureChecker(uint32 entry)
        : m_entry(entry), m_aliveCount(0), m_deadCount(0)
    {}
    void operator()(Creature* u)
    {
        if (u->GetEntry() != m_entry)
            return;

        if (u->IsAlive())
            m_aliveCount++;
        else
            m_deadCount++;
    }

    uint32 GetAliveCount() const { return m_aliveCount; }
    uint32 GetDeadCount() const { return m_deadCount; }
private:
    uint32 m_entry;
    uint32 m_aliveCount;
    uint32 m_deadCount;
};

void Creature::ApplyDynamicRespawnDelay(uint32& delay, CreatureData const* data)
{
    if (!IsInWorld())
        return;

    // Only affects continents
    if (GetMapId() > 1)
        return;

    // Only affects normal spawns
    if (GetSubtype() != CREATURE_SUBTYPE_GENERIC)
        return;

    if (!data || (data->spawn_flags & SPAWN_FLAG_NO_DYNAMIC_RESPAWN))
        return;

    // Only affects rares and above with the forced flag
    if (GetCreatureInfo()->rank >= CREATURE_ELITE_ELITE)
    {
        if (!(data->spawn_flags & SPAWN_FLAG_FORCE_DYNAMIC_ELITE))
            return;
    }

    if (GetLevel() > sWorld.getConfig(CONFIG_UINT32_DYN_RESPAWN_AFFECT_LEVEL_BELOW))
        return;

    float checkRange = sWorld.getConfig(CONFIG_FLOAT_DYN_RESPAWN_CHECK_RANGE);
    if (checkRange <= 0)
        return;

    if (delay > sWorld.getConfig(CONFIG_UINT32_DYN_RESPAWN_AFFECT_RESPAWN_TIME_BELOW))
        return;

    if (delay < sWorld.getConfig(CONFIG_UINT32_DYN_RESPAWN_MIN_RESPAWN_TIME))
        return;

    DynamicRespawnRatesPlayerChecker check(this);
    MaNGOS::PlayerWorker<DynamicRespawnRatesPlayerChecker> searcher(check);
    Cell::VisitWorldObjects(this, searcher, checkRange);

    // No dynamic respawns around an in progress escort
    if (check.HasNearbyEscort())
        return;

    uint32 playerCount = check.GetCount();
    if (playerCount < sWorld.getConfig(CONFIG_UINT32_DYN_RESPAWN_PLAYERS_THRESHOLD))
        return;

    float maxReductionRate = sWorld.getConfig(CONFIG_FLOAT_DYN_RESPAWN_MAX_REDUCTION_RATE);

    DynamicRespawnRatesCreatureChecker check2(GetEntry());
    MaNGOS::CreatureWorker<DynamicRespawnRatesCreatureChecker> worker(this, check2);
    Cell::VisitGridObjects(this, worker, checkRange);

    float reductionRate;
    if ((check2.GetAliveCount() < check2.GetDeadCount()) &&
        (check2.GetAliveCount() < playerCount))
    {
        reductionRate = maxReductionRate;
    }
    else
    {
        reductionRate = playerCount * sWorld.getConfig(CONFIG_FLOAT_DYN_RESPAWN_PERCENT_PER_PLAYER) / 100.0f;
        if (reductionRate > maxReductionRate)
            reductionRate = maxReductionRate;
    }

    // Invalid configuration
    if (reductionRate < 0)
        return;

    uint32 originalDelay = delay;
    uint32 reduction = static_cast<uint32>(reductionRate * originalDelay);
    if (reduction >= delay)
        delay = 0;
    else
        delay -= reduction;

    uint32 minimum = sWorld.getConfig(CONFIG_UINT32_DYN_RESPAWN_MIN_RESPAWN_TIME);
    uint32 indoorMinimum = sWorld.getConfig(CONFIG_UINT32_DYN_RESPAWN_MIN_RESPAWN_TIME_INDOORS);
    if (GetCreatureInfo()->rank >= CREATURE_ELITE_ELITE)
    {
        uint32 eliteMin = sWorld.getConfig(CONFIG_UINT32_DYN_RESPAWN_MIN_RESPAWN_TIME_ELITE);
        if (minimum < eliteMin)
            minimum = eliteMin;
    }
    else if (indoorMinimum > 0 && !GetTerrain()->IsOutdoors(GetPositionX(), GetPositionY(), GetPositionZ()))
    {
        minimum = indoorMinimum;
    }

    // Cap the lower-end reduction at the chosen minimum
    if (delay < minimum)
        delay = minimum;

    // Prevent bad configs extending the respawn time beyond default
    if (delay > originalDelay)
        delay = originalDelay;
}

void Creature::SaveRespawnTime()
{
    if (IsPet() || !HasStaticDBSpawnData())
        return;

    if (m_respawnTime > time(nullptr))                         // dead (no corpse)
        GetMap()->GetPersistentState()->SaveCreatureRespawnTime(GetGUIDLow(), m_respawnTime);
    else if (m_corpseDecayTimer > 0)                        // dead (corpse)
        GetMap()->GetPersistentState()->SaveCreatureRespawnTime(GetGUIDLow(), time(nullptr) + m_respawnDelay + m_corpseDecayTimer / IN_MILLISECONDS);
}

bool Creature::IsOutOfThreatArea(Unit* pVictim) const
{
    if (m_leashingDisabled) return false;
    if (HasExtraFlag(CREATURE_FLAG_EXTRA_NO_LEASH_EVADE))
        return false;

    // In dungeons, there is no threat area limit - only for active creatures (technical limitation, non actives are not updated without players around them)
    if (GetMap()->IsDungeon())
        return false;

    if (!pVictim)
        return true;

    if (pVictim->IsInMap(this))
    {
        // Use attack distance in distance check if threat radius is lower. This prevents creature bounce in and out of combat every update tick.
        float threatAreaDistance = std::max(GetAttackDistance(pVictim) * 1.5f, sWorld.getConfig(CONFIG_FLOAT_THREAT_RADIUS));
        bool inThreatArea = IsWithinDist3d(m_combatStartX, m_combatStartY, m_combatStartZ, threatAreaDistance) || pVictim->IsWithinDist3d(m_combatStartX, m_combatStartY, m_combatStartZ, threatAreaDistance);
        if (!inThreatArea && (GetLastLeashExtensionTime() + 12 < time(nullptr)))
            return true;
    }

    return false;
}

std::shared_ptr<time_t> const& Creature::GetLastLeashExtensionTimePtr() const
{
    if (m_lastLeashExtensionTime == nullptr)
        m_lastLeashExtensionTime = std::make_shared<time_t>(time(nullptr));
    return m_lastLeashExtensionTime;
}

void Creature::SetLastLeashExtensionTimePtr(std::shared_ptr<time_t> const& timer)
{
    m_lastLeashExtensionTime = timer;
}

void Creature::ClearLastLeashExtensionTimePtr()
{
    m_lastLeashExtensionTime.reset();
}

time_t Creature::GetLastLeashExtensionTime() const
{
    return *GetLastLeashExtensionTimePtr();
}

void Creature::UpdateLeashExtensionTime()
{
    (*GetLastLeashExtensionTimePtr()) = time(nullptr);
}

CreatureDataAddon const* Creature::GetCreatureAddon() const
{
    return ObjectMgr::GetCreatureAddon(GetGUIDLow());
}

CreatureData const* Creature::GetCreatureData() const
{
    return sObjectMgr.GetCreatureData(GetDBTableGUIDLow());
}

void Creature::LoadDefaultAuras(uint32 const* auras, bool reload)
{
    for (uint32 const* pSpellId = auras; *pSpellId; ++pSpellId)
    {
        SpellEntry const* pSpellEntry = sSpellMgr.GetSpellEntry(*pSpellId);
        if (!pSpellEntry)
        {
            sLog.outErrorDb("Creature (GUIDLow: %u Entry: %u ) has wrong spell %u defined in `auras` field.", GetGUIDLow(), GetEntry(), *pSpellId);
            continue;
        }

        if (HasAura(*pSpellId))
            continue;

        CastSpell(this, pSpellEntry, true);
    }
}

//creature_addon table
void Creature::LoadCreatureAddon(bool reload)
{
    if (CreatureDataAddon const* cainfo = GetCreatureAddon())
    {
        if (!reload && cainfo->mount_display_id >= 0)
            m_mountId = cainfo->mount_display_id;

        if (m_mountId != 0)
            Mount(m_mountId);

        SetStandState(cainfo->stand_state);
        SetSheath(SheathState(cainfo->sheath_state));
        SetUInt32Value(UNIT_NPC_EMOTESTATE, cainfo->emote_state);

        if (cainfo->auras)
            LoadDefaultAuras(cainfo->auras, reload);
        else if (m_creatureInfo->auras)
            LoadDefaultAuras(m_creatureInfo->auras, reload);
    }
    else
    {
        if (!reload)
            m_mountId = m_creatureInfo->mount_display_id;

        if (m_mountId != 0)
            Mount(m_mountId);

        SetStandState(UNIT_STAND_STATE_STAND);
        SetSheath(SHEATH_STATE_MELEE);
        SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);

        if (m_creatureInfo->auras)
            LoadDefaultAuras(m_creatureInfo->auras, reload);
    }
}

/// Send a message to LocalDefense channel for players opposition team in the zone
void Creature::SendZoneUnderAttackMessage(Player* attacker)
{
    uint32 areaId = GetAreaId();
    time_t now = time(nullptr);
    static std::unordered_map<uint32, time_t> areaAttackedCooldowns;
    if (areaAttackedCooldowns[areaId] + 10 < now)
    {
        areaAttackedCooldowns[areaId] = now;
        Team enemyTeam = attacker->GetTeam();

        WorldPacket data(SMSG_ZONE_UNDER_ATTACK, 4);
        data << uint32(areaId);
        GetMap()->SendToPlayers(&data, (enemyTeam == ALLIANCE ? HORDE : ALLIANCE));
    }
}

void Creature::SetInCombatWithZone(bool initialPulse)
{
    if (!CanHaveThreatList())
    {
        sLog.outError("Creature entry %u call SetInCombatWithZone but creature cannot have threat list.", GetEntry());
        return;
    }

    Map* pMap = GetMap();

    if (!pMap->IsDungeon())
    {
        sLog.outError("Creature entry %u call SetInCombatWithZone for map (id: %u) that isn't an instance.", GetEntry(), pMap->GetId());
        return;
    }

    Map::PlayerList const &PlList = pMap->GetPlayers();

    if (PlList.isEmpty())
        return;

    if (!HasCreatureState(CSTATE_COMBAT_WITH_ZONE))
    {
        UpdateCombatWithZoneState(true);

        // Attack closest player first.
        // Prevent case where boss runs to somebody who just entered raid.
        if (initialPulse && !GetVictim() && AI())
        {
            if (Player* pPlayer = FindNearestHostilePlayer(MAX_VISIBILITY_DISTANCE))
                AI()->AttackStart(pPlayer);
        }
    }

    for (const auto& i : PlList)
    {
        if (Player* pPlayer = i.getSource())
        {
            if (!initialPulse && pPlayer->IsInCombat())
                continue;

            if (IsValidAttackTarget(pPlayer))
            {
                EnterCombatWithTarget(pPlayer);
                Pet* pet = pPlayer->GetPet();
                if (pet && IsValidAttackTarget(pet))
                    EnterCombatWithTarget(pet);
            }
        }
    }
}


bool Creature::CallsForHelp() const
{
    return m_callsForHelp;
}

void Creature::SetCallsForHelp(bool callsForHelp)
{
    m_callsForHelp = callsForHelp;
}

bool Creature::MeetsSelectAttackingRequirement(Unit* pTarget, SpellEntry const* pSpellInfo, uint32 selectFlags) const
{
    if (selectFlags)
    {
        if ((selectFlags & SELECT_FLAG_PLAYER) && pTarget->GetTypeId() != TYPEID_PLAYER)
            return false;

        if ((selectFlags & SELECT_FLAG_NO_TOTEM) && pTarget->ToCreature() && pTarget->ToCreature()->IsTotem())
            return false;

        if ((selectFlags & SELECT_FLAG_NO_PET) && pTarget->ToCreature() && pTarget->ToCreature()->IsPet())
            return false;

        if ((selectFlags & SELECT_FLAG_POWER_MANA) && pTarget->GetPowerType() != POWER_MANA)
            return false;

        if ((selectFlags & SELECT_FLAG_POWER_NOT_MANA) && pTarget->GetPowerType() == POWER_MANA)
            return false;

        if ((selectFlags & SELECT_FLAG_POWER_RAGE) && pTarget->GetPowerType() != POWER_RAGE)
            return false;

        if ((selectFlags & SELECT_FLAG_POWER_ENERGY) && pTarget->GetPowerType() != POWER_ENERGY)
            return false;

        if ((selectFlags & SELECT_FLAG_IN_MELEE_RANGE) && !CanReachWithMeleeAutoAttack(pTarget))
            return false;

        if ((selectFlags & SELECT_FLAG_NOT_IN_MELEE_RANGE) && CanReachWithMeleeAutoAttack(pTarget))
            return false;

        if ((selectFlags & SELECT_FLAG_IN_LOS) && !IsWithinLOSInMap(pTarget))
            return false;

        if ((selectFlags & SELECT_FLAG_PLAYER_NOT_GM) && (pTarget->GetTypeId() != TYPEID_PLAYER || pTarget->ToPlayer()->IsGameMaster()))
            return false;

        if ((selectFlags & SELECT_FLAG_PET) && !pTarget->ToPet())
            return false;

        if ((selectFlags & SELECT_FLAG_NOT_PLAYER) && pTarget->GetTypeId() == TYPEID_PLAYER)
            return false;
    }

    // It makes more sense to check IsValidAttackTarget, but will break Kel'Thuzad script when IsTargetableBy is synced with vmangos.
    // He is immune to players in first phase, but picks random targets from threat list for summons to attack.
    if (pTarget->IsDead() ||
        pTarget->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING | UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NON_ATTACKABLE_2 | UNIT_FLAG_TAXI_FLIGHT | UNIT_FLAG_NOT_SELECTABLE) ||
        IsFriendlyTo(pTarget))
        return false;

    if (pSpellInfo)
        return pSpellInfo->IsTargetInRange(this, pTarget);

    return true;
}

void Creature::LogDeath(Unit* pKiller) const
{
    // by default we log bosses only
    if (!IsWorldBoss() && !HasExtraFlag(CREATURE_FLAG_EXTRA_INSTANCE_BIND))
        return;

    if (pKiller)
    {
        if (Player* pPlayer = pKiller->GetCharmerOrOwnerPlayerOrPlayerItself())
        {
            if (Group* pGroup = pPlayer->GetGroup())
            {
                float groupMembersCount = 0;
                float groupItemLevel = 0;
                std::string groupMemberNames;
                for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
                {
                    if (Player* pMember = itr->getSource())
                    {
                        groupMembersCount++;
                        groupItemLevel += pMember->GetAverageItemLevel();

                        // Not self.
                        if (pMember == pPlayer)
                            continue;

                        if (!groupMemberNames.empty())
                            groupMemberNames += ", ";

                        groupMemberNames += pMember->GetName() + std::string(" (Guid: ") + std::to_string(pMember->GetGUIDLow()) + ")";
                    }
                }

                sLog.outRaid("[Map %u] [Instance %u] %s killed by Player %s (Guid: %u) in group (ilvl %g) with: %s", GetMapId(), GetMap()->GetInstanceId(), GetGuidStr().c_str(), pPlayer->GetName(), pPlayer->GetGUIDLow(), (groupItemLevel / groupMembersCount), groupMemberNames.c_str());
            }
            else
                sLog.outRaid("[Map %u] [Instance %u] %s killed by Player %s (Guid: %u) not in group.", GetMapId(), GetMap()->GetInstanceId(), GetGuidStr().c_str(), pPlayer->GetName(), pPlayer->GetGUIDLow());
        }
        else
            sLog.outRaid("[Map %u] [Instance %u] %s killed by %s.", GetMapId(), GetMap()->GetInstanceId(), GetGuidStr().c_str(), pKiller->GetGuidStr().c_str());
    }
    else
        sLog.outRaid("[Map %u] [Instance %u] %s died on its own.", GetMapId(), GetMap()->GetInstanceId(), GetGuidStr().c_str());
}

Unit* Creature::SelectAttackingTarget(AttackingTarget target, uint32 position, uint32 spellId, uint32 selectFlags) const
{
    return SelectAttackingTarget(target, position, sSpellMgr.GetSpellEntry(spellId), selectFlags);
}

Unit* Creature::SelectAttackingTarget(AttackingTarget target, uint32 position, SpellEntry const* pSpellInfo /*= nullptr*/, uint32 selectFlags/*= SELECT_FLAG_NO_TOTEM*/) const
{
    if (!CanHaveThreatList())
        return nullptr;

    // ThreatList m_threatlist;
    ThreatList const& threatlist = GetThreatManager().getThreatList();
    ThreatList::const_iterator itr = threatlist.begin();
    ThreatList::const_reverse_iterator ritr = threatlist.rbegin();

    if (position >= threatlist.size() || threatlist.empty())
        return nullptr;

    switch (target)
    {
        case ATTACKING_TARGET_RANDOM:
        {
            std::vector<Unit*> suitableUnits;
            suitableUnits.reserve(threatlist.size() - position);
            advance(itr, position);
            for (; itr != threatlist.end(); ++itr)
                if (Unit* pTarget = GetMap()->GetUnit((*itr)->getUnitGuid()))
                    if (MeetsSelectAttackingRequirement(pTarget, pSpellInfo, selectFlags))
                        suitableUnits.push_back(pTarget);

            if (!suitableUnits.empty())
                return suitableUnits[urand(0, suitableUnits.size() - 1)];

            break;
        }
        case ATTACKING_TARGET_TOPAGGRO:
        {
            advance(itr, position);
            for (; itr != threatlist.end(); ++itr)
                if (Unit* pTarget = GetMap()->GetUnit((*itr)->getUnitGuid()))
                    if (MeetsSelectAttackingRequirement(pTarget, pSpellInfo, selectFlags))
                        return pTarget;

            break;
        }
        case ATTACKING_TARGET_BOTTOMAGGRO:
        {
            advance(ritr, position);
            for (; ritr != threatlist.rend(); ++ritr)
                if (Unit* pTarget = GetMap()->GetUnit((*itr)->getUnitGuid()))
                    if (MeetsSelectAttackingRequirement(pTarget, pSpellInfo, selectFlags))
                        return pTarget;

            break;
        }
        case ATTACKING_TARGET_NEAREST:
        {
            float distance = -1;
            float combatDistance = 0;
            Unit* pTarget = nullptr;
            Unit* suitableTarget = nullptr;

            advance(itr, position);
            for (; itr != threatlist.end(); ++itr)
            {
                pTarget = GetMap()->GetUnit((*itr)->getUnitGuid());
                if (pTarget && MeetsSelectAttackingRequirement(pTarget, pSpellInfo, selectFlags))
                {
                    combatDistance = GetDistance3dToCenter(pTarget);
                    if (!suitableTarget || combatDistance < distance)
                    {
                        distance = combatDistance;
                        suitableTarget = pTarget;
                    }
                }
            }

            return suitableTarget;
        }
        case ATTACKING_TARGET_FARTHEST:
        {
            float distance = -1;
            float combatDistance = 0;
            Unit* pTarget = nullptr;
            Unit* suitableTarget = nullptr;

            advance(itr, position);
            for (; itr != threatlist.end(); ++itr)
            {
                pTarget = GetMap()->GetUnit((*itr)->getUnitGuid());
                if (pTarget && MeetsSelectAttackingRequirement(pTarget, pSpellInfo, selectFlags))
                {
                    combatDistance = GetCombatDistance(pTarget);
                    if (combatDistance > distance)
                    {
                        distance = combatDistance;
                        suitableTarget = pTarget;
                    }
                }
            }

            return suitableTarget;
        }
    }

    return nullptr;
}

bool Creature::IsInEvadeMode() const
{
    if (IsEvadeBecauseTargetNotReachable())
        return true;

    if (GetMotionMaster()->GetCurrentMovementGeneratorType() == HOME_MOTION_TYPE)
        return true;

    if (IsPet())
        if (Creature const* pOwner = GetOwnerCreature())
            if (pOwner->IsInEvadeMode())
                return true;

    if (!IsInCombat() && GetMotionMaster()->GetCurrentMovementGeneratorType() == PATROL_MOTION_TYPE)
    {
        if (CreatureGroup* pGroup = GetCreatureGroup())
            if (pGroup->IsFormation() && pGroup->GetLeaderGuid() != GetObjectGuid())
                if (Creature* pLeader = GetMap()->GetCreature(pGroup->GetLeaderGuid()))
                    if (pLeader->IsInEvadeMode())
                        return true;
    }

    return false;
}

bool Creature::HasSpell(uint32 spellID) const
{
    uint8 i;
    for (i = 0; i < CREATURE_MAX_SPELLS; ++i)
        if (spellID == m_spells[i])
            break;
    return i < CREATURE_MAX_SPELLS;                         // break before end of iteration of known spells
}

time_t Creature::GetRespawnTimeEx() const
{
    time_t now = time(nullptr);
    if (m_respawnTime > now)                                // dead (no corpse)
        return m_respawnTime;
    else if (m_corpseDecayTimer > 0)                        // dead (corpse)
        return now + m_respawnDelay + m_corpseDecayTimer / IN_MILLISECONDS;
    else
        return now;
}

void Creature::GetRespawnCoord(float &x, float &y, float &z, float* ori, float* dist) const
{
    // Nostalrius : pouvoir changer point de spawn d'un mob -> Creature::SetHomePosition
    if (m_homePosition.x > 0.1f || m_homePosition.x < -0.1f)
    {
        x = m_homePosition.x;
        y = m_homePosition.y;
        z = m_homePosition.z;
        if (ori)
            *ori = m_homePosition.o;
        if (dist)
            *dist = GetWanderDistance();
    }
    else if (CreatureData const* data = sObjectMgr.GetCreatureData(GetGUIDLow()))
    {
        x = data->position.x;
        y = data->position.y;
        z = data->position.z;
        if (ori)
            *ori = data->position.o;
        if (dist)
            *dist = GetWanderDistance();
    }
    else
    {
        float orient;

        GetSummonPoint(x, y, z, orient);

        if (ori)
            *ori = orient;
        if (dist)
            *dist = GetWanderDistance();
    }

    //lets check if our creatures have valid spawn coordinates
    MANGOS_ASSERT(MaNGOS::IsValidMapCoord(x, y, z) || PrintCoordinatesError(x, y, z, "respawn"));
}

void Creature::AllLootRemovedFromCorpse()
{
    skinningForOthersTimer = 5000;
    if (lootForBody && !HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
    {
        uint32 corpseLootedDelay;

        if (!lootForSkin)                                   // corpse was not skinned -> apply corpseLootedDelay
        {
            // use a static spawntimesecs/3 modifier (guessed/made up value) unless config are more than 0.0
            // spawntimesecs=3min:  corpse decay after 1min
            // spawntimesecs=4hour: corpse decay after 1hour 20min
            if (sWorld.getConfig(CONFIG_FLOAT_RATE_CORPSE_DECAY_LOOTED) > 0.0f)
                corpseLootedDelay = (uint32)((m_corpseDelay * IN_MILLISECONDS) * sWorld.getConfig(CONFIG_FLOAT_RATE_CORPSE_DECAY_LOOTED));
            else
                corpseLootedDelay = (m_respawnDelay * IN_MILLISECONDS) / 3;
        }
        else                                                // corpse was skinned, corpse will despawn next update
            corpseLootedDelay = 0;

        // if m_respawnTime is not expired already
        if (m_respawnTime >= time(nullptr))
        {
            // if spawntimesecs is larger than default corpse delay always use corpseLootedDelay
            if (m_respawnDelay > m_corpseDelay)
                m_corpseDecayTimer = corpseLootedDelay;
            else
            {
                // if m_respawnDelay is relatively short and corpseDecayTimer is larger than corpseLootedDelay
                if (m_corpseDecayTimer > corpseLootedDelay)
                    m_corpseDecayTimer = corpseLootedDelay;
            }
        }
        else
        {
            m_corpseDecayTimer = 0;

            // TODO: reaching here, means mob will respawn at next tick.
            // This might be a place to set some aggro delay so creature has
            // ~5 seconds before it can react to hostile surroundings.

            // It's worth noting that it will not be fully correct either way.
            // At this point another "instance" of the creature are presumably expected to
            // be spawned already, while this corpse will not appear in respawned form.
        }
    }
}

std::string Creature::GetAIName() const
{
    return sObjectMgr.GetCreatureTemplate(GetEntry())->ai_name;
}

std::string Creature::GetScriptName() const
{
    return sScriptMgr.GetScriptName(GetScriptId());
}

uint32 Creature::GetScriptId() const
{
    return sObjectMgr.GetCreatureTemplate(GetEntry())->script_id;
}

VendorItemData const* Creature::GetVendorItems() const
{
    return sObjectMgr.GetNpcVendorItemList(GetEntry());
}

VendorItemData const* Creature::GetVendorTemplateItems() const
{
    uint32 vendorId = GetCreatureInfo()->vendor_id;
    return vendorId ? sObjectMgr.GetNpcVendorTemplateItemList(vendorId) : nullptr;
}

uint32 Creature::GetVendorItemCurrentCount(VendorItem const* vItem)
{
    if (!vItem->maxcount)
        return vItem->maxcount;

    VendorItemCounts::iterator itr = m_vendorItemCounts.begin();
    for (; itr != m_vendorItemCounts.end(); ++itr)
        if (itr->itemId == vItem->item)
            break;

    if (itr == m_vendorItemCounts.end())
        return vItem->maxcount;

    VendorItemCount* vCount = &*itr;

    time_t ptime = time(nullptr);

    if (vCount->lastIncrementTime + vCount->restockDelay <= ptime)
    {
        ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(vItem->item);

        uint32 diff = uint32((ptime - vCount->lastIncrementTime) / vCount->restockDelay);
        if ((vCount->count + diff * pProto->BuyCount) >= vItem->maxcount)
        {
            m_vendorItemCounts.erase(itr);
            return vItem->maxcount;
        }

        vCount->count += diff * pProto->BuyCount;
        vCount->lastIncrementTime = ptime;
    }

    return vCount->count;
}

uint32 Creature::UpdateVendorItemCurrentCount(VendorItem const* vItem, uint32 used_count)
{
    if (!vItem->maxcount)
        return 0;

    VendorItemCounts::iterator itr = m_vendorItemCounts.begin();
    for (; itr != m_vendorItemCounts.end(); ++itr)
        if (itr->itemId == vItem->item)
            break;

    uint32 restockDelay = vItem->incrtime;
    if (vItem->itemflags & VENDOR_ITEM_FLAG_RANDOM_RESTOCK)
        restockDelay *= float(urand(80, 120)) / 100.0f;
    if (vItem->itemflags & VENDOR_ITEM_FLAG_DYNAMIC_RESTOCK && sWorld.GetActiveSessionCount() > BLIZZLIKE_REALM_POPULATION)
        restockDelay *= float(BLIZZLIKE_REALM_POPULATION) / float(sWorld.GetActiveSessionCount());

    if (itr == m_vendorItemCounts.end())
    {
        uint32 new_count = vItem->maxcount > used_count ? vItem->maxcount - used_count : 0;
        m_vendorItemCounts.push_back(VendorItemCount(vItem->item, new_count, restockDelay));
        return new_count;
    }

    VendorItemCount* vCount = &*itr;

    time_t ptime = time(nullptr);

    if (vCount->lastIncrementTime + vCount->restockDelay <= ptime)
    {
        ItemPrototype const* pProto = sObjectMgr.GetItemPrototype(vItem->item);

        uint32 diff = uint32((ptime - vCount->lastIncrementTime) / vCount->restockDelay);
        if ((vCount->count + diff * pProto->BuyCount) < vItem->maxcount)
            vCount->count += diff * pProto->BuyCount;
        else
            vCount->count = vItem->maxcount;
    }

    vCount->count = vCount->count > used_count ? vCount->count - used_count : 0;
    vCount->lastIncrementTime = ptime;
    vCount->restockDelay = restockDelay;
    return vCount->count;
}

TrainerSpellData const* Creature::GetTrainerTemplateSpells() const
{
    uint32 trainerId = GetCreatureInfo()->trainer_id;
    return trainerId ? sObjectMgr.GetNpcTrainerTemplateSpells(trainerId) : nullptr;
}

TrainerSpellData const* Creature::GetTrainerSpells() const
{
    return sObjectMgr.GetNpcTrainerSpells(GetEntry());
}

// overwrite WorldObject function for proper name localization
const char* Creature::GetNameForLocaleIdx(int32 loc_idx) const
{
    if (loc_idx >= 0)
    {
        CreatureLocale const *cl = sObjectMgr.GetCreatureLocale(GetEntry());
        if (cl)
        {
            if (cl->Name.size() > (size_t)loc_idx && !cl->Name[loc_idx].empty())
                return cl->Name[loc_idx].c_str();
        }
    }

    return Creature::GetName();
}

void Creature::SetFactionTemporary(uint32 factionId, uint32 tempFactionFlags)
{
    m_temporaryFactionFlags = tempFactionFlags;
    SetFactionTemplateId(factionId);
}

void Creature::ClearTemporaryFaction()
{
    // No restore if creature is charmed/possessed.
    // For later we may consider extend to restore to charmer faction where charmer is creature.
    // This can also be done by update any pet/charmed of creature at any faction change to charmer.
    if (IsCharmed())
        return;

    m_temporaryFactionFlags = TEMPFACTION_NONE;
    SetFactionTemplateId(GetCreatureInfo()->faction);
}

void Creature::SendAreaSpiritHealerQueryOpcode(Player *pl)
{
    uint32 next_resurrect = 0;
    if (Spell* pcurSpell = GetCurrentSpell(CURRENT_CHANNELED_SPELL))
        next_resurrect = pcurSpell->GetCastedTime();
    WorldPacket data(SMSG_AREA_SPIRIT_HEALER_TIME, 8 + 4);
    data << ObjectGuid(GetObjectGuid());
    data << uint32(next_resurrect);
    pl->SendDirectMessage(&data);
}

void Creature::DisappearAndDie()
{
    if (!IsInWorld())
    {
        sLog.outInfo("[CRASH][%s]DisappearAndDie: Creature is not InWorld.", GetName());
        return;
    }

    if (IsCreature() && ToCreature()->IsPet())
        ((Pet*)this)->Unsummon(PET_SAVE_AS_DELETED);
    DestroyForNearbyPlayers();
    if (IsAlive())
        SetDeathState(JUST_DIED);
    RemoveCorpse();
}

void Creature::GetHomePosition(float &x, float &y, float &z, float &o)
{
    if (m_homePosition.x != 0.0f)
    {
        x = m_homePosition.x;
        y = m_homePosition.y;
        z = m_homePosition.z;
        o = m_homePosition.o;
        return;
    }
    GetRespawnCoord(x, y, z, &o);
}
void Creature::SetHomePosition(float x, float y, float z, float o)
{
    m_homePosition.x = x;
    m_homePosition.y = y;
    m_homePosition.z = z;
    m_homePosition.o = o;
}

void Creature::ResetHomePosition()
{
    if (CreatureData const *data = sObjectMgr.GetCreatureData(GetGUIDLow()))
        SetHomePosition(data->position.x, data->position.y, data->position.z, data->position.o);
    else if (IsTemporarySummon())
        GetSummonPoint(m_homePosition.x, m_homePosition.y, m_homePosition.z, m_homePosition.o);
}

void Creature::RemoveAurasAtReset()
{
    if (HasExtraFlag(CREATURE_FLAG_EXTRA_KEEP_POSITIVE_AURAS_ON_EVADE))
    {
        RemoveAllNegativeAuras(AURA_REMOVE_BY_DEFAULT);
        return;
    }

    for (SpellAuraHolderMap::iterator iter = m_spellAuraHolders.begin(); iter != m_spellAuraHolders.end();)
    {
        if (!(iter->second->GetCasterGuid().IsPlayer() && !iter->second->IsPermanent() && iter->second->IsPositive()) && iter->second->GetSpellProto()->IsAuraRemovedOnEvade())
        {
            RemoveSpellAuraHolder(iter->second, AURA_REMOVE_BY_DEFAULT);
            iter = m_spellAuraHolders.begin();
        }
        else
            ++iter;
    }
}

void Creature::OnLeaveCombat()
{
    UpdateCombatState(false);
    UpdateCombatWithZoneState(false);

    if (m_creatureGroup)
        m_creatureGroup->OnLeaveCombat(this);

    if (i_AI)
        i_AI->EnterEvadeMode();

    if (m_zoneScript)
        m_zoneScript->OnCreatureEvade(this);
}

void Creature::OnEnterCombat(Unit* pWho, bool notInCombat)
{
    if (!pWho)
        return;

    if (i_AI && !HasUnitState(UNIT_STAT_CONFUSED | UNIT_STAT_FLEEING))
        i_AI->AttackedBy(pWho);

    if (m_creatureGroup)
        m_creatureGroup->OnMemberAttackStart(this, pWho);

    if (notInCombat)
    {
        ResetCombatTime();
        UpdateCombatState(true);

        SetStandState(UNIT_STAND_STATE_STAND);
        m_pacifiedTimer = 0;

        if (m_zoneScript)
            m_zoneScript->OnCreatureEnterCombat(this);

        if (IsMounted())
            Unmount();

        if (i_AI)
            i_AI->EnterCombat(pWho);

        // Mark as At War with faction in client so player can attack back.
        if (GetReputationId() >= 0)
        {
            if (Player* pPlayer = pWho->ToPlayer())
            {
                if (pPlayer->GetReputationMgr().SetAtWar(GetReputationId(), true))
                {
                    pPlayer->SetTemporaryAtWarWithFaction(GetFactionId());
                    pPlayer->SendFactionAtWar(GetReputationId(), true);
                }
            }
        }

        if (pWho->IsPlayer() && CanSummonGuards())
            sGuardMgr.SummonGuard(this, static_cast<Player*>(pWho));

        if (Creature* pOwner = GetOwnerCreature())
            SetLastLeashExtensionTimePtr(pOwner->GetLastLeashExtensionTimePtr());
    }
}

void Creature::ResetStats()
{
    if (m_creatureInfo)
    {
        SetBaseWeaponDamage(BASE_ATTACK, MINDAMAGE, m_creatureInfo->dmg_min);
        SetBaseWeaponDamage(BASE_ATTACK, MAXDAMAGE, m_creatureInfo->dmg_max);
        UpdateDamagePhysical(BASE_ATTACK);
    }
    RemoveAllAuras();
}

Unit* Creature::GetNearestVictimInRange(float min, float max, bool includeCreatures)
{
    if (!CanHaveThreatList())
        return nullptr;

    float bestRange = max;
    Unit* pUnit = nullptr;

    ThreatList const& tList = GetThreatManager().getThreatList();
    for (const auto i : tList)
    {
        Unit* pTarget = GetMap()->GetUnit(i->getUnitGuid());
        if (!pTarget || (!pTarget->IsPlayer() && !includeCreatures))
            continue;

        float currRange = GetDistance(pTarget);
        if (currRange <= bestRange && currRange >= min)
        {
            bestRange = currRange;
            pUnit = pTarget;
        }
    }
    return pUnit;
}

Unit* Creature::GetFarthestVictimInRange(float min, float max, bool includeCreatures)
{
    if (!CanHaveThreatList())
        return nullptr;

    float bestRange = min;
    Unit* pUnit = nullptr;

    ThreatList const& tList = GetThreatManager().getThreatList();
    for (const auto i : tList)
    {
        Unit* pTarget = GetMap()->GetUnit(i->getUnitGuid());
        if (!pTarget || (!pTarget->IsPlayer() && !includeCreatures))
            continue;

        float currRange = GetDistance(pTarget);
        if (currRange > bestRange && currRange < max)
        {
            bestRange = currRange;
            pUnit = pTarget;
        }
    }
    return pUnit;
}

Unit* Creature::GetVictimInRange(float min, float max, bool includeCreatures)
{
    if (!CanHaveThreatList())
        return nullptr;

    ThreatList const& tList = GetThreatManager().getThreatList();
    for (const auto i : tList)
    {
        Unit* pTarget = GetMap()->GetUnit(i->getUnitGuid());

        if (pTarget && IsInRange(pTarget, min, max))
        {
            if (!pTarget->IsPlayer() && !includeCreatures)
                continue;
            return pTarget;
        }
    }
    return nullptr;
}

Unit* Creature::GetHostileCasterInRange(float min, float max)
{
    if (!CanHaveThreatList())
        return nullptr;

    ThreatList const& tList = GetThreatManager().getThreatList();
    for (const auto i : tList)
    {
        Unit* pTarget = GetMap()->GetUnit(i->getUnitGuid());

        if (pTarget && pTarget->IsCaster() && IsInRange(pTarget, min, max))
            return pTarget;
    }
    return nullptr;
}

Unit* Creature::GetHostileCaster()
{
    if (!CanHaveThreatList())
        return nullptr;

    ThreatList const& tList = GetThreatManager().getThreatList();
    for (const auto i : tList)
    {
        Unit* pTarget = GetMap()->GetUnit(i->getUnitGuid());

        if (pTarget && pTarget->IsCaster())
            return pTarget;
    }
    return nullptr;
}

void Creature::ProcessThreatList(ThreatListProcesser* f)
{
    if (!CanHaveThreatList())
        return;

    ThreatList const& tList = GetThreatManager().getThreatList();
    for (const auto i : tList)
    {
        Unit* target = GetMap()->GetUnit(i->getUnitGuid());

        if (target)
            if (f->Process(target))
                return;
    }
}

bool Creature::CastSpellOnFarthestVictim(uint32 spellId, float min, float max, bool triggered)
{
    if (Unit* pTarget = GetFarthestVictimInRange(min, max))
    {
        CastSpell(pTarget, spellId, triggered);
        return true;
    }
    return false;
}

bool Creature::CastSpellOnNearestVictim(uint32 spellId, float min, float max, bool triggered)
{
    if (Unit* pTarget = GetNearestVictimInRange(min, max))
    {
        CastSpell(pTarget, spellId, triggered);
        return true;
    }
    return false;
}

bool Creature::CastSpellOnHostileCasterInRange(uint32 spellId, float min, float max, bool triggered)
{
    if (Unit* pTarget = GetHostileCasterInRange(min, max))
    {
        CastSpell(pTarget, spellId, triggered);
        return true;
    }
    return false;
}

void Creature::AddThreatsOf(Creature const* pOther)
{
    ThreatList const& tList = pOther->GetThreatManager().getThreatList();
    for (const auto i : tList)
    {
        Unit* pTarget = GetMap()->GetUnit(i->getUnitGuid());

        if (pTarget && pTarget->IsAlive() && !IsFriendlyTo(pTarget))
        {
            pTarget->SetInCombatWith(this);
            AddThreat(pTarget);
        }
    }
}

// select nearest hostile unit within the given attack distance (i.e. distance is ignored if > than ATTACK_DISTANCE), regardless of threat list.
Unit* Creature::SelectNearestTargetInAttackDistance(float dist) const
{
    CellPair p(MaNGOS::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit *target = nullptr;

    if (dist > ATTACK_DISTANCE)
        sLog.outError("Creature (GUID: %u Entry: %u) SelectNearestTargetInAttackDistance called with dist > ATTACK_DISTANCE. Extra distance ignored.", GetGUIDLow(), GetEntry());

    MaNGOS::NearestHostileUnitInAttackDistanceCheck u_check(this, dist);
    MaNGOS::UnitLastSearcher<MaNGOS::NearestHostileUnitInAttackDistanceCheck> searcher(target, u_check);

    TypeContainerVisitor<MaNGOS::UnitLastSearcher<MaNGOS::NearestHostileUnitInAttackDistanceCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
    TypeContainerVisitor<MaNGOS::UnitLastSearcher<MaNGOS::NearestHostileUnitInAttackDistanceCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

    cell.Visit(p, world_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE);
    cell.Visit(p, grid_unit_searcher, *GetMap(), *this, ATTACK_DISTANCE);

    return target;
}

Unit* Creature::SelectNearestHostileUnitInAggroRange(bool useLOS, bool ignoreCivilians) const
{
    // Selects nearest hostile target within creature's aggro range. Used primarily by
    //  pets set to aggressive. Will not return neutral or friendly targets.
    CellPair p(MaNGOS::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(p);
    cell.SetNoCreate();

    Unit *target = nullptr;

    MaNGOS::NearestHostileUnitInAggroRangeCheck u_check(ToCreature(), useLOS, ignoreCivilians);
    MaNGOS::UnitLastSearcher<MaNGOS::NearestHostileUnitInAggroRangeCheck> searcher(target, u_check);

    TypeContainerVisitor<MaNGOS::UnitLastSearcher<MaNGOS::NearestHostileUnitInAggroRangeCheck>, WorldTypeMapContainer > world_unit_searcher(searcher);
    TypeContainerVisitor<MaNGOS::UnitLastSearcher<MaNGOS::NearestHostileUnitInAggroRangeCheck>, GridTypeMapContainer >  grid_unit_searcher(searcher);

    cell.Visit(p, world_unit_searcher, *GetMap(), *this, GetDetectionRange() + MAX_LEVEL_DIFF_FOR_AGGRO_RANGE);
    cell.Visit(p, grid_unit_searcher, *GetMap(), *this, GetDetectionRange() + MAX_LEVEL_DIFF_FOR_AGGRO_RANGE);

    return target;
}

Creature* Creature::FindNearestFriendlyGuard(float range) const
{
    Creature* pGuard = nullptr;

    MaNGOS::NearestFriendlyGuardInRangeCheck u_check(this, range);
    MaNGOS::CreatureLastSearcher<MaNGOS::NearestFriendlyGuardInRangeCheck> searcher(pGuard, u_check);

    Cell::VisitGridObjects(this, searcher, range);

    return pGuard;
}

void Creature::CallNearestGuard(Unit* pEnemy) const
{
    if (Creature* pGuard = FindNearestFriendlyGuard(50.0f))
        if (pGuard->AI() && pGuard->IsValidAttackTarget(pEnemy))
            pGuard->AI()->AttackStart(pEnemy);
}

SpellCastResult Creature::TryToCast(Unit* pTarget, uint32 uiSpell, uint32 uiCastFlags, uint8 uiChance)
{
    if (IsNonMeleeSpellCasted(false) && !(uiCastFlags & (CF_TRIGGERED | CF_INTERRUPT_PREVIOUS)))
        return SPELL_FAILED_SPELL_IN_PROGRESS;

    const SpellEntry* pSpellInfo = sSpellMgr.GetSpellEntry(uiSpell);

    if (!pSpellInfo)
    {
        sLog.outError("TryToCast: attempt to cast unknown spell %u by creature with entry: %u", uiSpell, GetEntry());
        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    return TryToCast(pTarget, pSpellInfo, uiCastFlags, uiChance);
}

SpellCastResult Creature::TryToCast(Unit* pTarget, const SpellEntry* pSpellInfo, uint32 uiCastFlags, uint8 uiChance)
{
    if (!pTarget)
        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

    // This spell should only be cast when target does not have the aura it applies.
    if ((uiCastFlags & CF_AURA_NOT_PRESENT) && pTarget->HasAura(pSpellInfo->Id))
        return SPELL_FAILED_AURA_BOUNCED;

    if ((uiCastFlags & CF_IGNORE_HARDCORE_TARGETS) && pSpellInfo->IsCharmSpell())
        if (Player* playerTarget = pTarget->ToPlayer())
            if (playerTarget->IsHardcore())
                return SPELL_FAILED_BAD_TARGETS;

    if (GetMotionMaster()->GetCurrentMovementGeneratorType() == TIMED_FLEEING_MOTION_TYPE)
        return SPELL_FAILED_FLEEING;

    // This spell is only used to interrupt enemy spell cast.
    if ((uiCastFlags & CF_TARGET_CASTING) && !pTarget->IsNonMeleeSpellCasted())
        return SPELL_FAILED_UNKNOWN;

    // This spell is only used when target is in melee range.
    if ((uiCastFlags & CF_ONLY_IN_MELEE) && !CanReachWithMeleeAutoAttack(pTarget))
        return SPELL_FAILED_OUT_OF_RANGE;

    // This spell should not be used if target is in melee range.
    if ((uiCastFlags & CF_NOT_IN_MELEE) && CanReachWithMeleeAutoAttack(pTarget))
        return SPELL_FAILED_TOO_CLOSE;

    // This spell should only be cast when we cannot get into melee range.
    if ((uiCastFlags & CF_TARGET_UNREACHABLE) && (CanReachWithMeleeAutoAttack(pTarget) || (GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE) || !(HasUnitState(UNIT_STAT_ROOT) || !GetMotionMaster()->GetCurrent()->IsReachable())))
        return SPELL_FAILED_MOVING;

    // Custom checks
    if (!(uiCastFlags & CF_FORCE_CAST))
    {
        // Motion Master is not updated when this state is active.
        if (!HasUnitState(UNIT_STAT_CAN_NOT_MOVE))
        {
            // Can't cast while fleeing.
            switch (GetMotionMaster()->GetCurrentMovementGeneratorType())
            {
                case TIMED_FLEEING_MOTION_TYPE:
                case DISTANCING_MOTION_TYPE:
                    return SPELL_FAILED_FLEEING;
            }
        }

        // If the spell requires to be behind the target.
        if (pSpellInfo->Custom & SPELL_CUSTOM_BEHIND_TARGET && pTarget->HasInArc(this))
            return SPELL_FAILED_UNIT_NOT_BEHIND;

        // If the spell requires the target having a specific power type.
        if (!pSpellInfo->IsAreaOfEffectSpell() && !pSpellInfo->IsTargetPowerTypeValid(pTarget->GetPowerType()))
            return SPELL_FAILED_UNKNOWN;

        // No point in casting if target is immune.
        if ((pTarget != this) && !pSpellInfo->IsPositiveSpell() && pTarget->IsImmuneToDamage(pSpellInfo->GetSpellSchoolMask(), pSpellInfo))
            return SPELL_FAILED_IMMUNE;

        // Mind control abilities can't be used with just 1 attacker or mob will reset.
        if ((GetThreatManager().getThreatList().size() == 1) && pSpellInfo->IsCharmSpell())
            return SPELL_FAILED_UNKNOWN;

        // Do not use dismounting spells when target is not mounted (there are 4 such spells).
        if (!pTarget->IsMounted() && pSpellInfo->IsDismountSpell())
            return SPELL_FAILED_ONLY_MOUNTED;
    }

    // Interrupt any previous spell
    if ((uiCastFlags & CF_INTERRUPT_PREVIOUS) && IsNonMeleeSpellCasted(false))
        InterruptNonMeleeSpells(false);

    Spell *spell = new Spell(this, pSpellInfo, uiCastFlags & CF_TRIGGERED);

    SpellCastTargets targets;

    // Don't set unit target on destination target based spells, otherwise the spell will cancel
    // as soon as the target dies or leaves the area of the effect
    if (pSpellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
        targets.setDestination(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());
    else
        targets.setUnitTarget(pTarget);

    if (pSpellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION)
        if (WorldObject* caster = spell->GetCastingObject())
            targets.setSource(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());

    spell->SetCastItem(nullptr);
    return spell->prepare(std::move(targets), nullptr, uiChance);
}

// use this function to avoid having hostile creatures attack
// friendlies and other mobs they shouldn't attack
bool Creature::_IsTargetAcceptable(Unit const *target) const
{
    ASSERT(target);

    // if the target cannot be attacked, the target is not acceptable
    if (IsFriendlyTo(target)
        || !target->IsTargetable(true, IsCharmerOrOwnerPlayerOrPlayerItself())
        || target->HasUnitState(UNIT_STAT_FEIGN_DEATH))
        return false;

    Unit *myVictim = GetAttackerForHelper();
    Unit *targetVictim = target->GetAttackerForHelper();

    // if I'm already fighting target, or I'm hostile towards the target, the target is acceptable
    if (myVictim == target || targetVictim == this || IsHostileTo(target))
        return true;

    // if the target's victim is friendly, and the target is neutral, the target is acceptable
    if (targetVictim && IsFriendlyTo(targetVictim))
        return true;

    // if the target's victim is not friendly, or the target is friendly, the target is not acceptable
    return false;
}

// this should not be called by petAI or
bool Creature::canCreatureAttack(Unit const *pVictim, bool force) const
{
    if (!pVictim->IsInMap(this))
        return false;

    if (!CanAttack(pVictim, force))
        return false;

    if (GetMap()->IsDungeon())
        return true;

    //Use AttackDistance in distance check if threat radius is lower. This prevents creature bounce in and out of combat every update tick.
    float dist = std::max(GetAttackDistance(pVictim), 150.0f);

    if (Unit *unit = GetCharmerOrOwner())
    {
        if (!pVictim->IsWithinDist(unit, dist))
            return false;
    }
    else if (!pVictim->IsWithinDist3d(m_homePosition, dist))
        return false;

    return pVictim->IsInAccessablePlaceFor(this);
}

time_t Creature::GetCombatTime() const
{
    time_t diff = time(nullptr) - m_combatStartTime;

    return diff;
}

void Creature::ResetCombatTime(bool combat)
{
    m_combatStartTime = time(nullptr);

    if (combat)
        ++m_combatResetCount;
    else
        m_combatResetCount = 0;
}

void Creature::EnterCombatWithTarget(Unit* pVictim)
{
    if (!GetVictim() && AI())
        AI()->AttackStart(pVictim);
    else if (GetVictim() != pVictim)
    {
        AddThreat(pVictim);
        pVictim->SetInCombatWith(this);
    }
}

bool Creature::canStartAttack(Unit const* who, bool force) const
{
    if (IsCivilian())
        return false;

    if (!CanFly() && (GetDistanceZ(who) > CREATURE_Z_ATTACK_RANGE))
        //|| who->IsControlledByPlayer() && who->IsFlying()))
        // we cannot check flying for other creatures, too much map/vmap calculation
        // TODO: should switch to range attack
        return false;

    if (!force)
    {
        if (!_IsTargetAcceptable(who))
            return false;

        if (who->IsInCombat())
            if (Unit *victim = who->GetAttackerForHelper())
                if (IsWithinDistInMap(victim, 10.0f))
                    force = true;

        if (!force && (IsNeutralToAll() || !IsWithinDistInMap(who, GetAttackDistance(who), true, SizeFactor::None)))
            return false;
    }

    if (!canCreatureAttack(who, force))
        return false;

    return IsWithinLOSInMap(who);
}

void Creature::ApplyGameEventSpells(GameEventCreatureData const* eventData, bool activated)
{
    uint32 cast_spell = activated ? eventData->spell_id_start : eventData->spell_id_end;
    uint32 remove_spell = activated ? eventData->spell_id_end : eventData->spell_id_start;

    if (remove_spell)
        if (SpellEntry const* spellEntry = sSpellMgr.GetSpellEntry(remove_spell))
            if (spellEntry->IsSpellAppliesAura())
                RemoveAurasDueToSpell(remove_spell);

    if (cast_spell)
        CastSpell(this, cast_spell, true);
}

void Creature::FillGuidsListFromThreatList(std::vector<ObjectGuid>& guids, uint32 maxamount /*= 0*/)
{
    if (!CanHaveThreatList())
        return;

    ThreatList const& threats = GetThreatManager().getThreatList();

    maxamount = maxamount > 0 ? std::min(maxamount, uint32(threats.size())) : threats.size();

    guids.reserve(guids.size() + maxamount);

    for (ThreatList::const_iterator itr = threats.begin(); maxamount && itr != threats.end(); ++itr, --maxamount)
        guids.push_back((*itr)->getUnitGuid());
}

struct AddCreatureToRemoveListInMapsWorker
{
    AddCreatureToRemoveListInMapsWorker(ObjectGuid guid) : i_guid(guid) {}

    void operator()(Map* map)
    {
        if (Creature* pCreature = map->GetCreature(i_guid))
            pCreature->AddObjectToRemoveList();
    }

    ObjectGuid i_guid;
};

void Creature::AddToRemoveListInMaps(uint32 db_guid, CreatureData const* data)
{
    AddCreatureToRemoveListInMapsWorker worker(data->GetObjectGuid(db_guid));
    sMapMgr.DoForAllMapsWithMapId(data->position.mapId, worker);
}

struct SpawnCreatureInMapsWorker
{
    SpawnCreatureInMapsWorker(uint32 guid, CreatureData const* data)
        : i_guid(guid), i_data(data) {}

    void operator()(Map* map)
    {
        // We use spawn coords to spawn
        if (map->IsLoaded(i_data->position.x, i_data->position.y))
        {
            Creature* pCreature = new Creature;
            //DEBUG_LOG("Spawning creature %u",*itr);
            if (!pCreature->LoadFromDB(i_guid, map))
                delete pCreature;
            else
                map->Add(pCreature);
        }
    }

    uint32 i_guid;
    CreatureData const* i_data;
};

void Creature::SpawnInMaps(uint32 db_guid, CreatureData const* data)
{
    SpawnCreatureInMapsWorker worker(db_guid, data);
    sMapMgr.DoForAllMapsWithMapId(data->position.mapId, worker);
}

bool Creature::HasStaticDBSpawnData() const
{
    return sObjectMgr.GetCreatureData(GetGUIDLow()) != nullptr;
}

uint32 Creature::GetDBTableGUIDLow() const
{
    if (HasStaticDBSpawnData())
        return GetGUIDLow();
    return 0;
}

void Creature::SetVirtualItem(VirtualItemSlot slot, uint32 item_id)
{
    if (item_id == 0)
    {
        SetUInt32Value(UNIT_VIRTUAL_ITEM_DISPLAY + slot, 0);
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 0, 0);
        SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 1, 0);
        return;
    }

    ItemPrototype const* proto = sObjectMgr.GetItemPrototype(item_id);
    if (!proto)
    {
        sLog.outError("Not listed in 'item_template' item (ID:%u) used as virtual item for %s", item_id, GetGuidStr().c_str());
        return;
    }

    SetUInt32Value(UNIT_VIRTUAL_ITEM_DISPLAY + slot, proto->DisplayInfoID);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2), VIRTUAL_ITEM_INFO_0_OFFSET_CLASS,    proto->Class);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2), VIRTUAL_ITEM_INFO_0_OFFSET_SUBCLASS, proto->SubClass);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2), VIRTUAL_ITEM_INFO_0_OFFSET_MATERIAL, proto->Material);
    SetByteValue(UNIT_VIRTUAL_ITEM_INFO + (slot * 2), VIRTUAL_ITEM_INFO_0_OFFSET_INVENTORYTYPE, proto->InventoryType);
    SetUInt32Value(UNIT_VIRTUAL_ITEM_INFO + (slot * 2) + 1, proto->Sheath);
}

void Creature::JoinCreatureGroup(Creature* leader, float dist, float angle, uint32 options)
{
    if (CreatureGroup* myGroup = GetCreatureGroup())
    {
        sLog.outError("%s attempts to join group, but is already in one.", GetGuidStr().c_str());
        return;
    }

    CreatureGroup* group = leader->GetCreatureGroup();
    if (!group)
    {
        group = new CreatureGroup(leader->GetObjectGuid());
        leader->SetCreatureGroup(group);
    }
    group->AddMember(GetObjectGuid(), dist, angle, options);
    SetCreatureGroup(group);
    if (group->IsFormation())
        GetMotionMaster()->Initialize();
}

void Creature::LeaveCreatureGroup()
{
    if (CreatureGroup* pGroup = GetCreatureGroup())
    {
        if (pGroup->GetOriginalLeaderGuid() == GetObjectGuid())
        {
            pGroup->DisbandGroup(this);
            delete pGroup;
        }
        else
        {
            if (pGroup->GetLeaderGuid() == GetObjectGuid())
                pGroup->RemoveTemporaryLeader(this);
            else
                pGroup->RemoveMember(GetObjectGuid());
        }

        SetCreatureGroup(nullptr);
    }
}

bool Creature::HasWeapon() const
{
    uint8 itemClass = GetByteValue(UNIT_VIRTUAL_ITEM_INFO + (0 * 2) + 0, VIRTUAL_ITEM_INFO_0_OFFSET_CLASS);
    return itemClass == ITEM_CLASS_WEAPON;
}

std::string Creature::GetDebuffs()
{
	SpellAuraHolderMap const& auraHolders = GetSpellAuraHolderMap();

	std::string rd;

	if (!auraHolders.empty())
	{
		for (SpellAuraHolderMap::const_iterator itr = auraHolders.begin(); itr != auraHolders.end(); ++itr)
		{
			SpellAuraHolder *holder = itr->second;

			SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(holder->GetId());

			SpellIconEntry const *spellIconEntry = sSpellIconStore.LookupEntry(spellInfo->SpellIconID);

			//sLog.outInfo("%i) id = %u, %s ", i, spellInfo->GetIcon(spellInfo->SpellIconID),  holder->GetId(), spellInfo->SpellName[sWorld.GetDefaultDbcLocale()].c_str());

			rd = rd +
				"" + spellInfo->GetIcon(spellInfo->SpellIconID) +
				"|" + std::to_string(holder->GetStackAmount()) +
				"|" + std::to_string(spellInfo->Dispel) +
				"|" + spellInfo->SpellName[sWorld.GetDefaultDbcLocale()].c_str() +
				"|" + std::to_string(holder->GetAuraDuration()) +
				"|" + spellInfo->ToolTip[sWorld.GetDefaultDbcLocale()].c_str() +
				"|" + std::to_string(holder->GetId()) +
				"#";

		}
	}

	if (rd == "")
		rd = "TW_BUFF:0";

	rd.pop_back();

	//sLog.outInfo("send");
	//sLog.outInfo("%s", rd.c_str());
	//sLog.outInfo(" ----------------------- ");

	return "TW_BUFF:" + rd;

}
