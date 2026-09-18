/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "ScriptMgr.h"
#include "Player.h"
#include "Spell.h"
#include "Chat.h"
#include "DynamicObject.h"
#include "SpellAuras.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"
#include "Pet.h"
#include "GameObject.h"
#include "GameObjectAI.h"
#include "GossipDef.h"
#include "Creature.h"
#include "Totem.h"
#include "CreatureAI.h"
#include "BattleGroundMgr.h"
#include "BattleGround.h"
#include "BattleGroundTG.h"
#include "BattleGroundWS.h"
#include "Language.h"
#include "SocialMgr.h"
#include "VMapFactory.h"
#include "Util.h"
#include "TemporarySummon.h"
#include "MoveMapSharedDefines.h"
#include "GameEventMgr.h"
#include "Chat.h"
#include "CompanionManager.hpp"
#include "ScriptObjects.h"
#include "MountManager.hpp"
#include "ToyManager.hpp"

#include "InstanceData.h"
#include "ScriptMgr.h"

#include <cmath>

using namespace Spells;

pEffect SpellEffects[TOTAL_SPELL_EFFECTS] =
{
    &Spell::EffectNULL,                                     //  0
    &Spell::EffectInstaKill,                                //  1 SPELL_EFFECT_INSTAKILL
    &Spell::EffectSchoolDMG,                                //  2 SPELL_EFFECT_SCHOOL_DAMAGE
    &Spell::EffectDummy,                                    //  3 SPELL_EFFECT_DUMMY
    &Spell::EffectUnused,                                   //  4 SPELL_EFFECT_PORTAL_TELEPORT          unused from pre-1.2.1
    &Spell::EffectTeleportUnits,                            //  5 SPELL_EFFECT_TELEPORT_UNITS
    &Spell::EffectApplyAura,                                //  6 SPELL_EFFECT_APPLY_AURA
    &Spell::EffectEnvironmentalDMG,                         //  7 SPELL_EFFECT_ENVIRONMENTAL_DAMAGE
    &Spell::EffectPowerDrain,                               //  8 SPELL_EFFECT_POWER_DRAIN
    &Spell::EffectHealthLeech,                              //  9 SPELL_EFFECT_HEALTH_LEECH
    &Spell::EffectHeal,                                     // 10 SPELL_EFFECT_HEAL
    &Spell::EffectBind,                                     // 11 SPELL_EFFECT_BIND
    &Spell::EffectUnused,                                   // 12 SPELL_EFFECT_PORTAL                   unused from pre-1.2.1, exist 2 spell, but not exist any data about its real usage
    &Spell::EffectUnused,                                   // 13 SPELL_EFFECT_RITUAL_BASE              unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 14 SPELL_EFFECT_RITUAL_SPECIALIZE        unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 15 SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL   unused from pre-1.2.1
    &Spell::EffectQuestComplete,                            // 16 SPELL_EFFECT_QUEST_COMPLETE
    &Spell::EffectWeaponDmg,                                // 17 SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL
    &Spell::EffectResurrect,                                // 18 SPELL_EFFECT_RESURRECT
    &Spell::EffectAddExtraAttacks,                          // 19 SPELL_EFFECT_ADD_EXTRA_ATTACKS
    &Spell::EffectEmpty,                                    // 20 SPELL_EFFECT_DODGE                    one spell: Dodge
    &Spell::EffectEmpty,                                    // 21 SPELL_EFFECT_EVADE                    one spell: Evade (DND)
    &Spell::EffectParry,                                    // 22 SPELL_EFFECT_PARRY
    &Spell::EffectBlock,                                    // 23 SPELL_EFFECT_BLOCK                    one spell: Block
    &Spell::EffectCreateItem,                               // 24 SPELL_EFFECT_CREATE_ITEM
    &Spell::EffectEmpty,                                    // 25 SPELL_EFFECT_WEAPON                   spell per weapon type, in ItemSubclassmask store mask that can be used for usability check at equip, but current way using skill also work.
    &Spell::EffectEmpty,                                    // 26 SPELL_EFFECT_DEFENSE                  one spell: Defense
    &Spell::EffectPersistentAA,                             // 27 SPELL_EFFECT_PERSISTENT_AREA_AURA
    &Spell::EffectSummon,                                   // 28 SPELL_EFFECT_SUMMON
    &Spell::EffectLeapForward,                              // 29 SPELL_EFFECT_LEAP
    &Spell::EffectEnergize,                                 // 30 SPELL_EFFECT_ENERGIZE
    &Spell::EffectWeaponDmg,                                // 31 SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
    &Spell::EffectTriggerMissileSpell,                      // 32 SPELL_EFFECT_TRIGGER_MISSILE
    &Spell::EffectOpenLock,                                 // 33 SPELL_EFFECT_OPEN_LOCK
    &Spell::EffectSummonChangeItem,                         // 34 SPELL_EFFECT_SUMMON_CHANGE_ITEM
    &Spell::EffectApplyAreaAura,                            // 35 SPELL_EFFECT_APPLY_AREA_AURA_PARTY
    &Spell::EffectLearnSpell,                               // 36 SPELL_EFFECT_LEARN_SPELL
    &Spell::EffectEmpty,                                    // 37 SPELL_EFFECT_SPELL_DEFENSE            one spell: SPELLDEFENSE (DND)
    &Spell::EffectDispel,                                   // 38 SPELL_EFFECT_DISPEL
    &Spell::EffectEmpty,                                    // 39 SPELL_EFFECT_LANGUAGE                 misc store lang id
    &Spell::EffectDualWield,                                // 40 SPELL_EFFECT_DUAL_WIELD
    &Spell::EffectSummonWild,                               // 41 SPELL_EFFECT_SUMMON_WILD
    &Spell::EffectSummonGuardian,                           // 42 SPELL_EFFECT_SUMMON_GUARDIAN
    &Spell::EffectTeleUnitsFaceCaster,                      // 43 SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER
    &Spell::EffectLearnSkill,                               // 44 SPELL_EFFECT_SKILL_STEP
    &Spell::EffectAddHonor,                                 // 45 SPELL_EFFECT_ADD_HONOR                honor/pvp related
    &Spell::EffectSpawn,                                    // 46 SPELL_EFFECT_SPAWN                    spawn/login animation, expected by spawn unit cast, also base points store some dynflags
    &Spell::EffectTradeSkill,                               // 47 SPELL_EFFECT_TRADE_SKILL
    &Spell::EffectUnused,                                   // 48 SPELL_EFFECT_STEALTH                  one spell: Base Stealth
    &Spell::EffectUnused,                                   // 49 SPELL_EFFECT_DETECT                   one spell: Detect
    &Spell::EffectTransmitted,                              // 50 SPELL_EFFECT_TRANS_DOOR
    &Spell::EffectUnused,                                   // 51 SPELL_EFFECT_FORCE_CRITICAL_HIT       unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 52 SPELL_EFFECT_GUARANTEE_HIT            unused from pre-1.2.1
    &Spell::EffectEnchantItemPerm,                          // 53 SPELL_EFFECT_ENCHANT_ITEM
    &Spell::EffectEnchantItemTmp,                           // 54 SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
    &Spell::EffectTameCreature,                             // 55 SPELL_EFFECT_TAMECREATURE
    &Spell::EffectSummonPet,                                // 56 SPELL_EFFECT_SUMMON_PET
    &Spell::EffectLearnPetSpell,                            // 57 SPELL_EFFECT_LEARN_PET_SPELL
    &Spell::EffectWeaponDmg,                                // 58 SPELL_EFFECT_WEAPON_DAMAGE
    &Spell::EffectOpenLock,                                 // 59 SPELL_EFFECT_OPEN_LOCK_ITEM
    &Spell::EffectProficiency,                              // 60 SPELL_EFFECT_PROFICIENCY
    &Spell::EffectSendEvent,                                // 61 SPELL_EFFECT_SEND_EVENT
    &Spell::EffectPowerBurn,                                // 62 SPELL_EFFECT_POWER_BURN
    &Spell::EffectThreat,                                   // 63 SPELL_EFFECT_THREAT
    &Spell::EffectTriggerSpell,                             // 64 SPELL_EFFECT_TRIGGER_SPELL
    &Spell::EffectUnused,                                   // 65 SPELL_EFFECT_HEALTH_FUNNEL            unused
    &Spell::EffectUnused,                                   // 66 SPELL_EFFECT_POWER_FUNNEL             unused from pre-1.2.1
    &Spell::EffectHealMaxHealth,                            // 67 SPELL_EFFECT_HEAL_MAX_HEALTH
    &Spell::EffectInterruptCast,                            // 68 SPELL_EFFECT_INTERRUPT_CAST
    &Spell::EffectDistract,                                 // 69 SPELL_EFFECT_DISTRACT
    &Spell::EffectPull,                                     // 70 SPELL_EFFECT_PULL                     one spell: Distract Move
    &Spell::EffectPickPocket,                               // 71 SPELL_EFFECT_PICKPOCKET
    &Spell::EffectAddFarsight,                              // 72 SPELL_EFFECT_ADD_FARSIGHT
    &Spell::EffectSummonPossessed,                          // 73 SPELL_EFFECT_SUMMON_POSSESSED
    &Spell::EffectSummonTotem,                              // 74 SPELL_EFFECT_SUMMON_TOTEM
    &Spell::EffectHealMechanical,                           // 75 SPELL_EFFECT_HEAL_MECHANICAL          one spell: Mechanical Patch Kit
    &Spell::EffectSummonObjectWild,                         // 76 SPELL_EFFECT_SUMMON_OBJECT_WILD
    &Spell::EffectScriptEffect,                             // 77 SPELL_EFFECT_SCRIPT_EFFECT
    &Spell::EffectUnused,                                   // 78 SPELL_EFFECT_ATTACK
    &Spell::EffectSanctuary,                                // 79 SPELL_EFFECT_SANCTUARY
    &Spell::EffectAddComboPoints,                           // 80 SPELL_EFFECT_ADD_COMBO_POINTS
    &Spell::EffectUnused,                                   // 81 SPELL_EFFECT_CREATE_HOUSE             one spell: Create House (TEST)
    &Spell::EffectNULL,                                     // 82 SPELL_EFFECT_BIND_SIGHT
    &Spell::EffectDuel,                                     // 83 SPELL_EFFECT_DUEL
    &Spell::EffectStuck,                                    // 84 SPELL_EFFECT_STUCK
    &Spell::EffectSummonPlayer,                             // 85 SPELL_EFFECT_SUMMON_PLAYER
    &Spell::EffectActivateObject,                           // 86 SPELL_EFFECT_ACTIVATE_OBJECT
    &Spell::EffectSummonTotem,                              // 87 SPELL_EFFECT_SUMMON_TOTEM_SLOT1
    &Spell::EffectSummonTotem,                              // 88 SPELL_EFFECT_SUMMON_TOTEM_SLOT2
    &Spell::EffectSummonTotem,                              // 89 SPELL_EFFECT_SUMMON_TOTEM_SLOT3
    &Spell::EffectSummonTotem,                              // 90 SPELL_EFFECT_SUMMON_TOTEM_SLOT4
    &Spell::EffectUnused,                                   // 91 SPELL_EFFECT_THREAT_ALL               one spell: zzOLDBrainwash
    &Spell::EffectEnchantHeldItem,                          // 92 SPELL_EFFECT_ENCHANT_HELD_ITEM
    &Spell::EffectUnused,                                   // 93 SPELL_EFFECT_SUMMON_PHANTASM
    &Spell::EffectSelfResurrect,                            // 94 SPELL_EFFECT_SELF_RESURRECT
    &Spell::EffectSkinning,                                 // 95 SPELL_EFFECT_SKINNING
    &Spell::EffectCharge,                                   // 96 SPELL_EFFECT_CHARGE
    &Spell::EffectSummonCritter,                            // 97 SPELL_EFFECT_SUMMON_CRITTER
    &Spell::EffectKnockBack,                                // 98 SPELL_EFFECT_KNOCK_BACK
    &Spell::EffectDisEnchant,                               // 99 SPELL_EFFECT_DISENCHANT
    &Spell::EffectInebriate,                                //100 SPELL_EFFECT_INEBRIATE
    &Spell::EffectFeedPet,                                  //101 SPELL_EFFECT_FEED_PET
    &Spell::EffectDismissPet,                               //102 SPELL_EFFECT_DISMISS_PET
    &Spell::EffectReputation,                               //103 SPELL_EFFECT_REPUTATION
    &Spell::EffectSummonObject,                             //104 SPELL_EFFECT_SUMMON_OBJECT_SLOT1
    &Spell::EffectSummonObject,                             //105 SPELL_EFFECT_SUMMON_OBJECT_SLOT2
    &Spell::EffectSummonObject,                             //106 SPELL_EFFECT_SUMMON_OBJECT_SLOT3
    &Spell::EffectSummonObject,                             //107 SPELL_EFFECT_SUMMON_OBJECT_SLOT4
    &Spell::EffectDispelMechanic,                           //108 SPELL_EFFECT_DISPEL_MECHANIC
    &Spell::EffectSummonDeadPet,                            //109 SPELL_EFFECT_SUMMON_DEAD_PET
    &Spell::EffectDestroyAllTotems,                         //110 SPELL_EFFECT_DESTROY_ALL_TOTEMS
    &Spell::EffectDurabilityDamage,                         //111 SPELL_EFFECT_DURABILITY_DAMAGE
    &Spell::EffectSummonDemon,                              //112 SPELL_EFFECT_SUMMON_DEMON
    &Spell::EffectResurrectNew,                             //113 SPELL_EFFECT_RESURRECT_NEW
    &Spell::EffectTaunt,                                    //114 SPELL_EFFECT_ATTACK_ME
    &Spell::EffectDurabilityDamagePCT,                      //115 SPELL_EFFECT_DURABILITY_DAMAGE_PCT
    &Spell::EffectSkinPlayerCorpse,                         //116 SPELL_EFFECT_SKIN_PLAYER_CORPSE       one spell: Remove Insignia, bg usage, required special corpse flags...
    &Spell::EffectSpiritHeal,                               //117 SPELL_EFFECT_SPIRIT_HEAL              one spell: Spirit Heal
    &Spell::EffectSkill,                                    //118 SPELL_EFFECT_SKILL                    professions and more
    &Spell::EffectApplyAreaAura,                            //119 SPELL_EFFECT_APPLY_AREA_AURA_PET
    &Spell::EffectUnused,                                   //120 SPELL_EFFECT_TELEPORT_GRAVEYARD       one spell: Graveyard Teleport Test
    &Spell::EffectWeaponDmg,                                //121 SPELL_EFFECT_NORMALIZED_WEAPON_DMG
    &Spell::EffectReputation,                               //122 SPELL_EFFECT_REPUTATION_QUEST_REWARD  only used by Silithyst Cap Reward
    &Spell::EffectSendTaxi,                                 //123 SPELL_EFFECT_SEND_TAXI                taxi/flight related (misc value is taxi path id)
    &Spell::EffectPlayerPull,                               //124 SPELL_EFFECT_PLAYER_PULL              opposite of knockback effect (pulls player twoard caster)
    &Spell::EffectModifyThreatPercent,                      //125 SPELL_EFFECT_MODIFY_THREAT_PERCENT
    &Spell::EffectUnused,                                   //126 SPELL_EFFECT_STEAL_BENEFICIAL_BUFF    future spell steal effect? now only used one test spell
    &Spell::EffectUnused,                                   //127 SPELL_EFFECT_PROSPECTING              future Prospecting spell, not have spells
    &Spell::EffectApplyAreaAura,                            //128 SPELL_EFFECT_APPLY_AREA_AURA_FRIEND
    &Spell::EffectApplyAreaAura,                            //129 SPELL_EFFECT_APPLY_AREA_AURA_ENEMY
    &Spell::EffectDespawnObject,                            //130 SPELL_EFFECT_DESPAWN_OBJECT
    &Spell::EffectNostalrius,                               //131 SPELL_EFFECT_NOSTALRIUS
    &Spell::EffectApplyAreaAura,                            //132 SPELL_EFFECT_APPLY_AREA_AURA_RAID
    &Spell::EffectApplyAreaAura,                            //133 SPELL_EFFECT_APPLY_AREA_AURA_OWNER
    &Spell::EffectApplyAreaAura,                            //134 SPELL_EFFECT_APPLY_AURA_PET
};

void Spell::EffectEmpty(SpellEffectIndex /*eff_idx*/)
{
    // NOT NEED ANY IMPLEMENTATION CODE, EFFECT POSISBLE USED AS MARKER OR CLIENT INFORM
}

void Spell::EffectNULL(SpellEffectIndex /*eff_idx*/)
{
    DEBUG_LOG("WORLD: Spell Effect DUMMY");
}

void Spell::EffectUnused(SpellEffectIndex /*eff_idx*/)
{
    // NOT USED BY ANY SPELL OR USELESS OR IMPLEMENTED IN DIFFERENT WAY IN MANGOS
}

void Spell::EffectResurrectNew(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->IsAlive())
        return;

    if (!unitTarget->IsInWorld())
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        // Pet case
        Pet* pet = unitTarget->ToPet();
        if (!pet)
            return;
        Unit* owner = pet->GetOwner();
        if (!owner)
            return;
        uint32 health = damage;

        pet->NearTeleportTo(m_caster->GetPosition(), 0);
        pet->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
        pet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
        pet->SetDeathState(ALIVE);
        pet->ClearUnitState(UNIT_STAT_ALL_DYN_STATES);
        pet->SetHealth(pet->GetMaxHealth() > health ? health : pet->GetMaxHealth());

        pet->AIM_Initialize();

        // m_caster->PetSpellInitialize(); -- action bar not removed at death and not required send at revive
        pet->SavePetToDB(PET_SAVE_AS_CURRENT);

        // Remove Demonic Sacrifice auras (Blizzlike - cf patchnote 1.12)
        Unit::AuraList const& auraClassScripts = owner->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (Unit::AuraList::const_iterator itr = auraClassScripts.begin(); itr != auraClassScripts.end();)
        {
            if ((*itr)->GetModifier()->m_miscvalue == 2228)
            {
                owner->RemoveAurasDueToSpell((*itr)->GetId());
                itr = auraClassScripts.begin();
            }
            else
                ++itr;
        }
        return;
    }

    Player* pTarget = ((Player*)unitTarget);

    if (pTarget->IsRessurectRequested())      // already have one active request
        return;

    uint32 health = damage;
    uint32 mana = m_spellInfo->EffectMiscValue[eff_idx];
    pTarget->SetResurrectRequestData(m_caster->GetObjectGuid(), m_caster->GetMapId(), m_caster->GetInstanceId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), m_caster->GetOrientation(), health, mana);
    SendResurrectRequest(pTarget, m_casterUnit && m_casterUnit->IsSpiritHealer());

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectInstaKill(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || !unitTarget->IsAlive())
        return;

    if (m_caster == unitTarget) // Prevent interrupt message
        finish();

    WorldPacket data(SMSG_SPELLINSTAKILLLOG, (8 + 4));
    data << unitTarget->GetObjectGuid(); // Victim GUID
    data << uint32(m_spellInfo->Id);
    m_caster->SendMessageToSet(&data, true);

    m_caster->DealDamage(unitTarget, unitTarget->GetMaxHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, m_spellInfo, false, this);
}

void Spell::EffectEnvironmentalDMG(SpellEffectIndex eff_idx)
{
    if (!unitTarget || !unitTarget->IsAlive())
        return;

    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->EnvironmentalDamage(DAMAGE_FIRE, damage);
    else
    {
        uint32 absorb = 0;
        int32 resist = 0;
        unitTarget->CalculateDamageAbsorbAndResist(m_caster, m_spellInfo->GetSpellSchoolMask(), SPELL_DIRECT_DAMAGE, damage, &absorb, &resist, m_spellInfo);
        m_caster->SendSpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, damage, m_spellInfo->GetSpellSchoolMask(), absorb, resist, false, 0, false);

    }
}

void Spell::EffectSchoolDMG(SpellEffectIndex effect_idx)
{
    if (unitTarget && unitTarget->IsAlive())
    {
        switch (m_spellInfo->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                break;
            }

            default:
                break;
        }

        if (m_spellScript)
            m_spellScript->OnEffectDamageCalculate(this, effect_idx, damage);

        if (damage >= 0)
            m_damage += damage;
    }
}

void Spell::EffectDummy(SpellEffectIndex eff_idx)
{
    if (!unitTarget && !gameObjTarget && !itemTarget && !corpseTarget)
        return;

    // selection by spell family
    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
                case 59011: // Thorn Gorge flag: validate again when the cast completes.
                {
                    if (eff_idx == EFFECT_INDEX_0 && unitTarget && unitTarget->IsPlayer())
                    {
                        Player* player = static_cast<Player*>(unitTarget);
                        if (BattleGround* bg = player->GetBattleGround())
                            if (bg->GetTypeID() == BATTLEGROUND_TG)
                                static_cast<BattleGroundTG*>(bg)->CompleteFlagPickup(player, player->GetMap()->GetGameObject(m_originalCasterGUID));
                    }
                    return;
                }
                case 18955: // Ranshalla's Torch Trap
                case 18993: // Ranshalla's Altar Trap
                {
                    if (unitTarget)
                        unitTarget->RemoveAurasDueToSpellByCancel(18953);
                    return;
                }
                case 18954: // Ranshalla Despawn
                {
                    if (Creature* pRanshalla = ToCreature(unitTarget))
                        pRanshalla->ForcedDespawn();

                    return;
                }
                case 32061: // EPL PvP A Game of Towers: (TXT) ToWoW - Tower Kill Credit (DND)
                {
                    if (Player* pPlayer = ToPlayer(m_casterUnit))
                        if (Creature* pCreature = ToCreature(unitTarget))
                            pPlayer->KilledMonsterCredit(pCreature->GetEntry(), 0);

                    return;
                }
                case 23383: // Alliance Flag Click
                case 23384: // Horde Flag Click
                {
                    if (!m_casterGo)
                        return;

                    Player* pPlayer = ToPlayer(unitTarget);
                    if (!pPlayer || !pPlayer->CanUseBattleGroundObject())
                        return;

                    BattleGround* bg = pPlayer->GetBattleGround();
                    if (!bg)
                        return;

                    switch (m_casterGo->GetEntry())
                    {
                        case 179785:    // Silverwing Flag
                        case 179786:    // Warsong Flag
                            if (bg->GetTypeID() == BATTLEGROUND_WS)
                                bg->EventPlayerClickedOnFlag(pPlayer, m_casterGo);
                            break;
                        default:
                            return;
                    }

                    pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                    pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOD_INVISIBILITY);
                    m_casterGo->Delete();
                    return;
                }
                case 6700: // Dimensional Portal (Used by Arugal)
                {
                    if (unitTarget->GetTypeId() == TYPEID_UNIT)
                    {
                        if (InstanceData* m_pInstance = unitTarget->GetInstanceData())
                            m_pInstance->SetData(5, 3);

                        unitTarget->SetVisibility(VISIBILITY_OFF);
                    }
                    return;
                }
                case 11885: // Capture Treant
                case 11886: // Capture Wildkin
                case 11887: // Capture Hippogryph
                case 11888: // Capture Faerie Dragon
                case 11889: // Capture Mountain Giant
                case 12699: // Summon Screecher Spirit
                {
                    if (unitTarget->IsDead() && unitTarget->GetTypeId() == TYPEID_UNIT)
                        ((Creature*)unitTarget)->ForcedDespawn(1000);
                    return;
                }
                case 19395: // Gordunni Trap
                {
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
                    {
                        // If GameObject casting was implemented, or activating a trap actually despawned it, this wouldn't be needed.
                        if (GameObject* pObject = unitTarget->FindNearestGameObject(144050, INTERACTION_DISTANCE))
                        {
                            if (pObject->HasStaticDBSpawnData())
                                unitTarget->CastSpell(unitTarget, urand(0, 1) ? 19394 : 11756, true);
                            else
                                pObject->AddObjectToRemoveList();
                        }
                    }

                    return;
                }
                case 19614: // Despawn Caster
                {
                    if (Creature* pCreature = ToCreature(unitTarget))
                        pCreature->DespawnOrUnsummon();

                    return;
                }
                case 6655: // Force Target - Bow
                {
                    if (unitTarget)
                        unitTarget->HandleEmote(EMOTE_ONESHOT_BOW);
                    return;
                }
                case 25716: // Force Self - Bow
                {
                    if (m_casterUnit)
                        m_casterUnit->HandleEmote(EMOTE_ONESHOT_BOW);

                    return;
                }
                case 27798: // Nature's Bounty
                {
                    switch(unitTarget->GetPowerType())
                    {
                        case POWER_RAGE :
                            unitTarget->CastSpell(unitTarget, 27783, true);
                        return;
                        case POWER_ENERGY :
                            unitTarget->CastSpell(unitTarget, 27784, true);
                        return;
                        case POWER_MANA :
                            unitTarget->CastSpell(unitTarget, 27782, true);
                        return;
                    }
                    return;
                }
                case 8344: // Universal Remote
                {
                    if (!m_originalCaster)
                        return;

                    // Random spell
                    uint32 randomSpell = urand(0, 2);
                    switch (randomSpell)
                    {
                        case 0:
                            m_originalCaster->CastSpell(unitTarget, 8345, true);
                            break;
                        case 1:
                            m_originalCaster->CastSpell(unitTarget, 8346, true);
                            break;
                        case 2:
                            unitTarget->CastSpell(unitTarget, 8599, true);
                            break;
                        default:
                            m_originalCaster->CastSpell(unitTarget, 8346, true);
                            break;
                    }
                    return;
                }
                case 13006: // Shrink ray
                {
                    if (!m_originalCaster)
                        return;
                    /* Source: http://www.wowwiki.com/Gnomish_Shrink_Ray
                        Not implemented yet:
                        Make every attacking enemy bigger
                        Make every attacking enemy smaller
                        Give shield-wielding enemies "Improved Blocking" for 30 seconds (this shows no visible effect other than target is hit by the ray)
                        13003 - Shrink Ray (single target)
                        13010 - Shrink (TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE)
                        13004 - Grow  (TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE)
                     */
                    uint32 r = urand(0, 99);
                    // Normal behavior
                    if (r > 15)
                        m_originalCaster->CastSpell(unitTarget, 13003, true);
                    else if (r > 13) // Make the user bigger
                    {
                        m_originalCaster->AddAura(13004);
                        m_originalCaster->SendSpellGo(m_originalCaster, 13004);
                    }
                    else if (r > 11) // Make the user smaller
                    {
                        m_originalCaster->AddAura(13010);
                        m_originalCaster->SendSpellGo(m_originalCaster, 13010);
                    }
                    else if (r > 9) // Make the target bigger
                    {
                        unitTarget->AddAura(13004);
                        m_originalCaster->SendSpellGo(unitTarget, 13004);
                    }
                    else if (r > 4) // Make the user's entire party smaller
                        m_originalCaster->CastSpell(unitTarget, 13010, true);
                    else // Make the user's entire party bigger
                        m_originalCaster->CastSpell(unitTarget, 13004, true);
                    break;
                }
                // Gnomish Mind Control Cap
                case 13180:
                {
                    if (!m_originalCaster)
                        return;

                    // [WowWiki] Possible malfunctions
                    // - Fails with no effect, but cooldown is trigged.
                    // - User becomes mind controlled by target. However, if the Gnomish Mind Control Cap is used while in a shapeshift form (such as cat form), this effect does not trigger.
                    switch (urand(0, 5))
                    {
                        case 0:
                            break;
                        case 1:
                            if (m_originalCaster->GetShapeshiftForm() == FORM_NONE)
                                unitTarget->CastSpell(m_originalCaster, 13181, true);
                            break;
                        default:
                            m_originalCaster->CastSpell(unitTarget, 13181, true);
                            break;
                    }
                    return;
                }
                // BRD : Invocation de Theldren
                case 27517:
                {
                    if (!m_originalCaster)
                        return;

                    if (InstanceData* pInst = m_originalCaster->GetInstanceData())
                        pInst->CustomSpellCasted(27517, m_originalCaster, unitTarget);
                    return;
                }
                case 14537:                                 // Six Demon Bag
                {
                    if (!unitTarget)
                        return;

                    Unit* newTarget = unitTarget;
                    uint32 spell_id = 0;
                    uint32 roll = urand(0, 99);
                    if (roll < 25)                          // Fireball (25% chance)
                        spell_id = 11921;
                    else if (roll < 50)                     // Frostbolt (25% chance)
                        spell_id = 13322;
                    else if (roll < 70)                     // Chain Lighting (20% chance)
                        spell_id = 21179;
                    else if (roll < 77)                     // Polymorph (10% chance, 7% to target)
                        spell_id = 13323;
                    else if (roll < 80)                     // Polymorph (10% chance, 3% to self, backfire)
                    {
                        spell_id = 13323;
                        newTarget = m_casterUnit;
                    }
                    else if (roll < 95)                     // Enveloping Winds (15% chance)
                        spell_id = 25189;
                    else                                    // Summon Felhund minion (5% chance)
                    {
                        spell_id = 14642;
                        newTarget = m_casterUnit;
                    }

                    if (newTarget)
                        m_caster->CastSpell(newTarget, spell_id, true, m_CastItem);

                    return;
                }
                case 17009:                                 // Voodoo
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch (urand(0, 6))
                    {
                        case 0:
                            spell_id = 16707;
                            break;    // Hex
                        case 1:
                            spell_id = 16708;
                            break;    // Hex
                        case 2:
                            spell_id = 16709;
                            break;    // Hex
                        case 3:
                            spell_id = 16711;
                            break;    // Grow
                        case 4:
                            spell_id = 16712;
                            break;    // Special Brew
                        case 5:
                            spell_id = 16713;
                            break;    // Ghostly
                        case 6:
                            spell_id = 16716;
                            break;    // Launch
                    }

                    m_caster->CastSpell(unitTarget, spell_id, true, nullptr, nullptr, m_originalCasterGUID, m_spellInfo);
                    return;
                }
                case 3360:                                  // Curse of the Eye
                {
                    if (!unitTarget)
                        return;

                    uint32 spell_id = (unitTarget->GetGender() == GENDER_MALE) ? 10651 : 10653;

                    m_caster->CastSpell(unitTarget, spell_id, true);
                    return;
                }
                case 7671:                                  // Pyrewood Transformation (Human <-> Worgen)
                {
                    if (!unitTarget)
                        return;

                    switch (unitTarget->GetEntry())
                    {
                    case 1891: // Pyrewood Watcher
                        unitTarget->ToCreature()->UpdateEntry(1892);
                        break;
                    case 1892: // Moonrage Watcher
                        unitTarget->ToCreature()->UpdateEntry(1891);
                        break;
                    case 1893: // Moonrage Sentry
                        unitTarget->ToCreature()->UpdateEntry(1894);
                        break;
                    case 1894: // Pyrewood Sentry
                        unitTarget->ToCreature()->UpdateEntry(1893);
                        break;
                    case 1895: // Pyrewood Elder
                        unitTarget->ToCreature()->UpdateEntry(1896);
                        break;
                    case 1896: // Moonrage Elder
                        unitTarget->ToCreature()->UpdateEntry(1895);
                        break;
                    case 3528: // Pyrewood Armorer
                        unitTarget->ToCreature()->UpdateEntry(3529);
                        break;
                    case 3529: // Moonrage Armorer
                        unitTarget->ToCreature()->UpdateEntry(3528);
                        break;
                    case 3530: // Pyrewood Tailor
                        unitTarget->ToCreature()->UpdateEntry(3531);
                        break;
                    case 3531: // Moonrage Tailor
                        unitTarget->ToCreature()->UpdateEntry(3530);
                        break;
                    case 3532: // Pyrewood Leatherworker
                        unitTarget->ToCreature()->UpdateEntry(3533);
                        break;
                    case 3533: // Moonrage Leatherworker
                        unitTarget->ToCreature()->UpdateEntry(3532);
                        break;
                    default:
                        return;
                    };
                    return;
                }
                case 8593:                                  // Symbol of life (restore creature to life)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    ((Creature*)unitTarget)->SetDeathState(JUST_ALIVED);
                    return;
                }
                case 9204:                                  // Hate to Zero
                {
                    if (!unitTarget || !m_casterUnit)
                        return;

                    m_casterUnit->GetThreatManager().modifyThreatPercent(unitTarget, -100);
                    return;
                }
                case 9976:                                  // Polly Eats the E.C.A.C.
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Summon Polly Jr.
                    unitTarget->CastSpell(unitTarget, 9998, true);

                    ((Creature*)unitTarget)->ForcedDespawn(100);
                    return;
                }
                case 13535:                                 // Tame Beast
                {
                    if (!m_originalCaster || m_originalCaster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Creature* channelTarget = m_originalCaster->GetMap()->GetCreature(m_originalCaster->GetChannelObjectGuid());

                    if (!channelTarget)
                        return;

                    m_originalCaster->CastSpell(channelTarget, 13481, true, nullptr, nullptr, m_originalCasterGUID, m_spellInfo);
                    return;
                }
                case 13567:                                 // Dummy Trigger
                {
                    // can be used for different aura triggering, so select by aura
                    if (!m_triggeredByAuraSpell || !unitTarget)
                        return;

                    switch (m_triggeredByAuraSpell->Id)
                    {
                        case 26467:                         // Persistent Shield
                        {
                            int32 damagePoints = int32(damage);
                            m_caster->CastCustomSpell(unitTarget, 26470, &damagePoints, nullptr, nullptr, true);
                            break;
                        }
                        default:
                            sLog.outError("EffectDummy: Non-handled case for spell 13567 for triggered aura %u", m_triggeredByAuraSpell->Id);
                            break;
                    }
                    return;
                }
                case 15998:                                 // Capture Worg Pup
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;

                    creatureTarget->ForcedDespawn();
                    return;
                }
                case 17251:                                 // Spirit Healer Res
                {
                    if (!unitTarget)
                        return;

                    Unit* caster = GetAffectiveCaster();

                    if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
                        data << unitTarget->GetObjectGuid();
                        ((Player*)caster)->GetSession()->SendPacket(&data);
                    }
                    return;
                }
                case 17950:
                {
                    if (!unitTarget)
                        return;

                    // Shadow Portal
                    const uint32 spell_list[6] = {17863, 17939, 17943, 17944, 17946, 17948};

                    m_caster->CastSpell(unitTarget, spell_list[urand(0, 5)], true);
                    return;
                }
                case 19411:                                 // Lava Bomb
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 20494, true);
                    return;
                }
                case 20474:                                 // Lava Bomb
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 20495, true);
                    return;
                }
                case 19593:                                // Egg Explosion (Buru)
                {
                    if (!unitTarget)
                        return;

                    if (!m_originalCaster)
                        return;

                    int32 damage;
                    if (unitTarget->IsPlayer()) // damage from 100 - 500 based on proximity - max range 25
                        damage = 100 + ((25 - std::min(m_originalCaster->GetCombatDistance(unitTarget), 25.f)) / 25.f) * 400;
                    else if (unitTarget->GetEntry() == 15370) // buru
                    {
                        damage = unitTarget->GetHealth() * 15 / 100; // 15% hp for buru

                        if (unitTarget->GetVictim())
                            unitTarget->GetThreatManager().modifyThreatPercent(unitTarget->GetVictim(), -100);
                    }
                    else
                        return;

                    m_originalCaster->CastCustomSpell(unitTarget, 5255, &damage, nullptr, nullptr, true);
                    return;
                }
                case 20572:                                 // Blood Fury
                {
                    Player* pPlayer = m_caster->ToPlayer();
                    if (!pPlayer)
                        return;

                    pPlayer->CastSpell(pPlayer, 23230, true);

                    damage = damage * (pPlayer->GetInt32Value(UNIT_FIELD_ATTACK_POWER)) / 100;
                    if (damage > 0)
                    {
                        int32 damagePoints = int32(damage);
                        pPlayer->CastCustomSpell(pPlayer, 23234, &damagePoints, nullptr, nullptr, true, nullptr);
                    }
                    return;
                }
                case 21147:                                 // Arcane Vacuum
                {
                    if (unitTarget && m_casterUnit)
                    {
                        // Spell used by Azuregos to teleport all the players to him
                        // This also resets the target threat
                        if (m_casterUnit->GetThreatManager().getThreat(unitTarget))
                            m_casterUnit->GetThreatManager().modifyThreatPercent(unitTarget, -100);

                        // cast summon player
                        m_casterUnit->CastSpell(unitTarget, 21150, true);
                    }

                    return;
                }
                case 23019:                                 // Crystal Prison Dummy DND
                {
                    if (!unitTarget || !unitTarget->IsAlive() || unitTarget->GetTypeId() != TYPEID_UNIT || ((Creature*)unitTarget)->IsPet())
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;
                    if (creatureTarget->IsPet())
                        return;

                    GameObject* pGameObj = new GameObject;

                    Map *map = creatureTarget->GetMap();

                    // create before death for get proper coordinates
                    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), 179644, map,
                                          creatureTarget->GetPositionX(), creatureTarget->GetPositionY(), creatureTarget->GetPositionZ(),
                                          creatureTarget->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
                    {
                        delete pGameObj;
                        return;
                    }

                    pGameObj->SetRespawnTime(creatureTarget->GetRespawnTime() - time(nullptr));
                    pGameObj->SetOwnerGuid(m_caster->GetObjectGuid());
                    //Pose un soucis(Maxinus)
                    // pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->GetLevel());
                    pGameObj->SetSpellId(m_spellInfo->Id);
                    pGameObj->SetWorldMask(m_caster->GetWorldMask());

                    creatureTarget->ForcedDespawn();

                    DEBUG_LOG("AddObject at SpellEfects.cpp EffectDummy");
                    map->Add(pGameObj);

                    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM, 8);
                    data << ObjectGuid(pGameObj->GetObjectGuid());
                    m_caster->SendMessageToSet(&data, true);

                    return;
                }
                case 4050:                                    // Explosive Sheep
                {
                    Unit* pOwner = m_casterUnit ? m_casterUnit->GetCharmerOrOwner() : nullptr;
                    if (pOwner && pOwner->GetTypeId() == TYPEID_PLAYER)
                    {
                        uint32 spellid = m_casterUnit->GetUInt32Value(UNIT_CREATED_BY_SPELL);
                        SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellid);
                        if (spellInfo)
                            pOwner->CooldownEvent(spellInfo);
                    }
                    return;
                }
                case 23138:                                 // Gate of Shazzrah
                {
                    if (!unitTarget)
                        return;

                    // Effect probably include a threat change, but it is unclear if fully
                    // reset or just forced upon target for teleport (SMSG_HIGHEST_THREAT_UPDATE)

                    // Gate of Shazzrah
                    m_caster->CastSpell(unitTarget, 23139, true);
                    return;
                }
                case 23645:                                 // Hourglass Sand
                    if (m_casterUnit)
                        m_casterUnit->RemoveAurasDueToSpell(23170); // Brood Affliction: Bronze
                    return;
                case 23725:                                 // Gift of Life (warrior bwl trinket)
                    if (m_casterUnit)
                    {
                        int32 lifegivingGemHealthMod = int32(m_casterUnit->GetMaxHealth() * 0.15);
                        m_casterUnit->CastCustomSpell(m_casterUnit, 23782, &lifegivingGemHealthMod, nullptr, nullptr, true, nullptr);
                    }
                    return;
                case 24781:                                 // Dream Fog
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT || !unitTarget)
                        return;
                    // TODO Note: Should actually not only AttackStart, but fixate on the target
                    ((Creature*)m_caster)->AI()->AttackStart(unitTarget);
                    return;
                }
                case 24930:                                 // Hallow's End Treat
                {
                    if (!m_casterUnit)
                        return;

                    uint32 spell_id = 0;

                    switch (urand(1, 4))
                    {
                        case 1:
                            spell_id = 24924;
                            break;    // Larger and Orange
                        case 2:
                            spell_id = 24925;
                            break;    // Skeleton
                        case 3:
                            spell_id = 24926;
                            break;    // Pirate
                        case 4:
                            spell_id = 24927;
                            break;    // Ghost
                    }

                    m_casterUnit->CastSpell(m_casterUnit, spell_id, true);
                    return;
                }
                case 25938:                                 // Viscidus explode
                {
                    if (!m_casterUnit)
                        return;

                    if (m_casterUnit->GetHealthPercent() < 5.0f) // Only way to kill this boss
                    {
                        m_casterUnit->CastSpell(m_casterUnit, 26003, true);
                        return;
                    }

                    // Stop combat
                    m_casterUnit->CombatStop();

                    // Fake death
                    //m_casterUnit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_FEING_DEATH);
                    m_casterUnit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
                    m_casterUnit->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);
                    m_casterUnit->AddUnitState(UNIT_STAT_FEIGN_DEATH);

                    // Summon globs
                    m_casterUnit->CastSpell(m_casterUnit, 25885, true);

                    return;
                }
                case 26003:                                 // Viscidus suicide
                {
                    if (!m_casterUnit)
                        return;

                    if (!m_casterUnit->IsAlive())
                        return;

                    m_casterUnit->CastSpell(m_casterUnit, 26002, true);
                    return;
                }
                case 26074:                                 // Holiday Cheer
                    // implemented at client side
                    return;
                case 26399: // Despawn Tentacles (C'thun)
                {
                    if (!m_caster)
                        return;

                    if (unitTarget)
                        m_caster->DealDamage(unitTarget, unitTarget->GetMaxHealth(), nullptr, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, nullptr, false);

                    return;
                }
                case 26626:                                 // Mana Burn Area
                {
                    if (unitTarget->GetPowerType() != POWER_MANA)
                        return;

                    m_caster->CastSpell(unitTarget, 25779, true);
                    return;
                }
                case 28006:                                 // Arcane Cloaking
                {
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
                        // Naxxramas Entry Flag Effect DND
                        m_caster->CastSpell(unitTarget, 29296, true);

                    return;
                }
                case 28098:                                 // Stalagg Tesla Effect
                case 28110:                                 // Feugen Tesla Effect
                {
                    if (!m_casterUnit)
                        return;

                    if (unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (m_casterUnit->GetVictim() && !m_casterUnit->IsWithinDistInMap(unitTarget, 60.0f))
                    {
                        // Cast Shock on nearby targets
                        if (Unit* pTarget = ((Creature*)m_casterUnit)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            unitTarget->CastSpell(pTarget, 28099, false);
                    }
                    else
                    {
                        // "Evade"
                        unitTarget->RemoveAurasDueToSpell(m_spellInfo->Id == 28098 ? 28097 : 28109);
                        unitTarget->DeleteThreatList();
                        unitTarget->CombatStop(true);
                        // Recast chain (Stalagg Chain or Feugen Chain
                        unitTarget->CastSpell(m_casterUnit, m_spellInfo->Id == 28098 ? 28096 : 28111, false);
                    }
                    return;
                }
                case 8897:                                 // Destroy Rocket Boots
                {
                    m_caster->CastSpell(unitTarget, 8893, true);
                    return;
                }
                case 23185:                                 // Aura of Frost
                case 25044:                                 // Aura of Nature
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->HasAura(25040))         // Mark of Nature
                        unitTarget->CastSpell(unitTarget, 25043, true);

                    if (unitTarget->HasAura(23182))         // Mark of Frost
                        unitTarget->CastSpell(unitTarget, 23186, true);

                    unitTarget->RemoveAurasAtMechanicImmunity(1 << (MECHANIC_BANDAGE - 1), 0);
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                    if (!unitTarget->IsStandingUp())
                        unitTarget->SetStandState(UNIT_STAND_STATE_STAND);

                    return;
                }
                case 28414:                                 // Call of the Ashbringer
                {
                    if (!m_caster || !m_caster->IsPlayer())
                        return;
                    static constexpr uint32 AshbringerSounds[12] = { 8906,8907,8908,8920,8921,8922,8923,8924,8925,8926,8927,8928};
                    m_caster->PlayDirectSound(AshbringerSounds[urand(0, 11)], m_caster->ToPlayer());
                    return;
                }
                case 28441:                                 // AB Effect 000
                {

                    return;
                }
                case 28697:                                 // Forgiveness (SM Ashbringer event)
                {
                    if (unitTarget && m_casterUnit)
                    {
                        m_casterUnit->Kill(unitTarget, nullptr);
                    }
                    return;
                }
                case 21343:                                 // Snowball Knockdown
                {
                    if (unitTarget && m_caster && unitTarget->IsPlayer() && m_caster->IsPlayer())
                    {
                        if (!unitTarget->HasAura(21354) &&                                      // Has no Snowball Resistant aura
                            unitTarget->ToPlayer()->IsInSameRaidWith(m_caster->ToPlayer()))     // Is grouped with target
                        {
                            unitTarget->CastSpell(unitTarget, 21167, true);
                        }
                    }
                    return;
                }
                case 26899: // Friendship Bracelet
                {
                    if (unitTarget && m_casterUnit && unitTarget->HasAura(26898))
                    {
                        unitTarget->RemoveAurasDueToSpell(26898);        // Remove Heartbroken
                        unitTarget->CastSpell(unitTarget, 26921, true);  // Create Bracelet
                        m_casterUnit->CastSpell(m_casterUnit, 26664, true);      // Cast The Power of Friendship
                    }
                    return;
                }
                case 27662: // Silver Shafted Arrow
                {
                    if (unitTarget && m_caster && unitTarget->IsPlayer())
                    {
                        if (!unitTarget->GetMiniPet())
                            unitTarget->CastSpell(unitTarget, 27570, true);
                    }
                    return;
                }
                case 14813: // Dark Iron Drunk Mug
                {
                    if (unitTarget->HasAura(14823) || unitTarget->GetEntry() == 14871)
                        return;

                    if (m_originalCasterGUID && m_originalCasterGUID.IsGameObject())
                    {
                        if (GameObject* pMug = unitTarget->GetMap()->GetGameObject(m_originalCasterGUID))
                        {
                            float fX, fY, fZ;
                            pMug->GetContactPoint(unitTarget, fX, fY, fZ);
                            unitTarget->GetMotionMaster()->MovePoint(1, fX, fY, fZ, MOVE_WALK_MODE);
                            m_caster->SummonGameObject(165738, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 30000);
                        }
                    }
                    return;
                }
                case 23845: // Attract Jubjub
                {
                    if (GameObject* pMug = m_caster->FindNearestGameObject(165578, 3.0f))
                    {
                        float fX, fY, fZ;
                        pMug->GetContactPoint(unitTarget, fX, fY, fZ);
                        unitTarget->GetMotionMaster()->MovePoint(1, fX, fY, fZ, MOVE_WALK_MODE);
                    }
                    return;
                }
                case 29518: // Sillithus Flag Click (DND)
                {
                    // Also mark player with pvp on
                    unitTarget->CastSpell(unitTarget, 29519, true);
                    unitTarget->SetPvPContested(true);

                    return;
                }
                case 26879: // Love is in the Air - Remove Amorous
                {
                    unitTarget->RemoveAurasDueToSpell(26869);
                    return;
                }
                case 17190: // Ras Frostwhisper Visual Dummy
                {
                    if (unitTarget)
                    {
                        unitTarget->CastSpell(unitTarget, 17186, true);    // Human form
                        unitTarget->SetHealth(unitTarget->GetMaxHealth()); // Back to full health
                    }
                    return;
                }
                case 16032: // Merging Oozes
                {
                    if (unitTarget && m_caster && unitTarget->IsCreature() && m_caster->IsCreature())
                    {
                        // Summon Gargantuan Ooze
                        m_caster->SummonCreature(9621, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 420000);
                        ((Creature*)m_caster)->DespawnOrUnsummon();
                        ((Creature*)unitTarget)->DespawnOrUnsummon();
                    }
                    return;
                }
            }
            // All IconID Check in there
            switch (m_spellInfo->SpellIconID)
            {
                // Berserking (troll racial traits)
                case 1661:
                {
                    if (!m_casterUnit)
                        return;

                    uint32 healthPerc = uint32((float(m_casterUnit->GetHealth()) / m_casterUnit->GetMaxHealth()) * 100);
                    int32 melee_mod = 10;
                    if (healthPerc <= 40)
                        melee_mod = 30;
                    if (healthPerc < 100 && healthPerc > 40)
                        melee_mod = 10 + (100 - healthPerc) / 3;

                    // FIXME: custom spell required this aura state by some unknown reason, we not need remove it anyway
                    m_casterUnit->ModifyAuraState(AURA_STATE_BERSERKING, true);
                    m_casterUnit->CastCustomSpell(m_casterUnit, 26635, &melee_mod, &melee_mod, &melee_mod, true, nullptr);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
            break;
        case SPELLFAMILY_DRUID:
            break;
        case SPELLFAMILY_SHAMAN:
        {
            break;
        }
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr.GetPetAura(m_spellInfo->Id))
    {
        if (m_casterUnit)
            m_casterUnit->AddPetAura(petSpell);

        return;
    }

    // Script based implementation. Must be used only for not good for implementation in core spell effects
    // So called only for not processed cases
    if (gameObjTarget)
        sScriptMgr.OnEffectDummy(m_caster, m_spellInfo->Id, eff_idx, gameObjTarget);
    else if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
        sScriptMgr.OnEffectDummy(m_caster, m_spellInfo->Id, eff_idx, (Creature*)unitTarget);
}

void Spell::EffectTriggerSpell(SpellEffectIndex eff_idx)
{
    // only unit case known
    if (!unitTarget)
    {
        if (gameObjTarget || itemTarget)
            sLog.outError("Spell::EffectTriggerSpell (Spell: %u): Unsupported non-unit case!", m_spellInfo->Id);
        return;
    }

    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[eff_idx];

    // normal case
    SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(triggered_spell_id);
    if (!spellInfo)
    {
        sLog.outError("EffectTriggerSpell of spell %u: triggering unknown spell id %i", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    // select formal caster for triggered spell
    WorldObject* caster = m_caster;

    // some triggered spells require specific equipment
    if (spellInfo->EquippedItemClass >= 0 && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        // main hand weapon required
        if (spellInfo->HasAttribute(SPELL_ATTR_EX3_REQUIRES_MAIN_HAND_WEAPON))
        {
            Item* item = ((Player*)m_caster)->GetWeaponForAttack(BASE_ATTACK, true, false);

            // skip spell if no weapon in slot or broken
            if (!item)
                return;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(spellInfo))
                return;
        }

        // offhand hand weapon required
        if (spellInfo->AttributesEx3 & SPELL_ATTR_EX3_REQUIRES_OFFHAND_WEAPON)
        {
            Item* item = ((Player*)m_caster)->GetWeaponForAttack(OFF_ATTACK, true, false);

            // skip spell if no weapon in slot or broken
            if (!item)
                return;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(spellInfo))
                return;
        }
    }
    else
    {
        // Note: not exist spells with weapon req. and IsSpellHaveCasterSourceTargets == true
        // so this just for speedup places in else
        caster = spellInfo->IsSpellWithCasterSourceTargetsOnly() ? unitTarget : m_caster;
    }

    caster->CastSpell(unitTarget, spellInfo, true, nullptr, nullptr, m_originalCasterGUID);
}

void Spell::EffectTriggerMissileSpell(SpellEffectIndex effect_idx)
{
    uint32 triggered_spell_id = m_spellInfo->EffectTriggerSpell[effect_idx];

    // normal case
    SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(triggered_spell_id);

    if (!spellInfo)
    {
        sLog.outError("EffectTriggerMissileSpell of spell %u (eff: %u): triggering unknown spell id %u",
                      m_spellInfo->Id, effect_idx, triggered_spell_id);
        return;
    }

    if (m_CastItem)
        DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", spellInfo->Id);

    m_caster->CastSpell(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, spellInfo, true, m_CastItem, nullptr, m_originalCasterGUID);
}

void Spell::EffectTeleportUnits(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->IsTaxiFlying())
        return;

    switch (m_spellInfo->EffectImplicitTargetB[eff_idx])
    {
        case TARGET_LOCATION_CASTER_HOME_BIND:
        {
            // Only players can teleport to innkeeper
            if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            bool hearthCooldown = m_spellInfo->Id != 556;   // Astral Recall doesn't set HS on cooldown
            ((Player*)unitTarget)->TeleportToHomebind(unitTarget == m_caster ? TELE_TO_SPELL : 0, hearthCooldown);
            return;
        }
        case TARGET_ENUM_UNITS_SCRIPT_AOE_AT_SRC_LOC:                     // in all cases first TARGET_LOCATION_DATABASE
        case TARGET_LOCATION_DATABASE:
        {
            SpellTargetPosition const* st = sSpellMgr.GetSpellTargetPosition(m_spellInfo->Id);
            if (!st)
            {
                sLog.outError("Spell::EffectTeleportUnits - unknown Teleport coordinates for spell ID %u", m_spellInfo->Id);
                return;
            }

            if (st->mapId == unitTarget->GetMapId())
                unitTarget->NearTeleportTo(*st, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget == m_caster ? TELE_TO_SPELL : 0));
            else if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                ((Player*)unitTarget)->TeleportTo(*st, unitTarget == m_caster ? TELE_TO_SPELL : 0);
            break;
        }
        case TARGET_LOCATION_CASTER_DEST:
        {
            if (!m_casterUnit)
                return;

            // m_destN filled, but sometimes for wrong dest and does not have TARGET_FLAG_DEST_LOCATION

            float x = unitTarget->GetPositionX();
            float y = unitTarget->GetPositionY();
            float z = unitTarget->GetPositionZ();
            float orientation = m_caster->GetOrientation();

            m_casterUnit->NearTeleportTo(x, y, z, orientation, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget == m_caster ? TELE_TO_SPELL : 0));
            return;
        }
        default:
        {
            // If not exist data for dest location - return
            if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
            {
                sLog.outError("Spell::EffectTeleportUnits - unknown EffectImplicitTargetB[%u] = %u for spell ID %u", eff_idx, m_spellInfo->EffectImplicitTargetB[eff_idx], m_spellInfo->Id);
                return;
            }
            // Init dest coordinates
            float x = m_targets.m_destX;
            float y = m_targets.m_destY;
            float z = m_targets.m_destZ;
            float orientation = unitTarget->GetOrientation();
            // Teleport
            unitTarget->NearTeleportTo(x, y, z, orientation, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget == m_caster ? TELE_TO_SPELL : 0));
            return;
        }
    }

}

void Spell::EffectApplyAura(SpellEffectIndex eff_idx)
{
    if (!unitTarget || !m_spellAuraHolder)
        return;

    if (!m_spellInfo->EffectApplyAuraName[eff_idx])
        return;

    // ghost spell check, allow apply any auras at player loading in ghost mode (will be cleanup after load)
    if ((!unitTarget->IsAlive() && !(m_spellInfo->CanTargetDeadTarget() || m_spellInfo->IsDeathPersistentSpell())) &&
            (unitTarget->GetTypeId() != TYPEID_PLAYER || !((Player*)unitTarget)->GetSession()->PlayerLoading()))
        return;

    if (unitTarget->HasMorePowerfulSpellActive(m_spellInfo))
        return;

    Unit* caster = GetAffectiveCaster();
    if (!caster)
    {
        // FIXME: currently we can't have auras applied explicitly by gameobjects
        // so for auras from wild gameobjects (no owner) target used
        if (m_originalCasterGUID.IsGameObject())
            caster = unitTarget;
        else
            return;
    }

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell: Aura is: %u [Spell%u:DiminishingGroup%u]", m_spellInfo->EffectApplyAuraName[eff_idx], m_spellInfo->Id, m_diminishGroup);

    switch (m_spellInfo->Id)
    {
    // Turtle WoW. Plainsrunning:
    case PLAINSRUNNING_SPELL:
        if (unitTarget->GetRace() != RACE_TAUREN) 
        {
            unitTarget->ToPlayer()->RemoveSpell(PLAINSRUNNING_SPELL, false, false);
            return;
        }
        if (((Player*)unitTarget)->InBattleGround())
        {
            unitTarget->RemoveAurasDueToSpell(PLAINSRUNNING_SPELL);
            ChatHandler(unitTarget->ToPlayer()).SendSysMessage("|cffff8040Plainsrunning is disabled on Battlegrounds.|r");
            return;
        }
        if (!unitTarget->ToPlayer()->HasSkill(762))
        {
            if (eff_idx == EFFECT_INDEX_0)
                ChatHandler(unitTarget->ToPlayer()).SendSysMessage("|cffff8040You are afraid of high speeds, better learn to ride first.|r");
            return;
        }
        if (eff_idx == EFFECT_INDEX_1)
        {
            if (unitTarget->ToPlayer()->GetSkillValue(SKILL_RIDING) == 150)
                m_currentBasePoints[EFFECT_INDEX_1] = 40;
            else
                m_currentBasePoints[EFFECT_INDEX_1] = 20;
        }
        break;
    case PLAINSRUNNING_FIRST_TICK:
        if (!unitTarget->ToPlayer()->IsMoving())
        {
            unitTarget->RemoveAura(12567, eff_idx);
            return;
        }
        if (eff_idx == EFFECT_INDEX_1)
        {
            if (unitTarget->ToPlayer()->GetSkillValue(SKILL_RIDING) == 150)
                m_currentBasePoints[EFFECT_INDEX_1] = 60;
            else
                m_currentBasePoints[EFFECT_INDEX_1] = 40;
        }
        break;
    case PLAINSRUNNING_SECOND_TICK:
        if (!unitTarget->ToPlayer()->IsMoving())
        {
            unitTarget->RemoveAura(12568, eff_idx);
            return;
        }
        if (eff_idx == EFFECT_INDEX_0)
        {
            if (unitTarget->ToPlayer()->GetSkillValue(SKILL_RIDING) == 150)
                m_currentBasePoints[EFFECT_INDEX_0] = 100;
            else
                m_currentBasePoints[EFFECT_INDEX_0] = 60;
        }
        break;
    }

    Aura* aur = CreateAura(m_spellInfo, eff_idx, &m_currentBasePoints[eff_idx], m_spellAuraHolder, unitTarget, caster, m_CastItem);
    m_spellAuraHolder->AddAura(aur, eff_idx);
}

void Spell::EffectPowerDrain(SpellEffectIndex eff_idx)
{
    if (m_spellInfo->EffectMiscValue[eff_idx] < 0 || m_spellInfo->EffectMiscValue[eff_idx] >= MAX_POWERS)
        return;

    Powers drain_power = Powers(m_spellInfo->EffectMiscValue[eff_idx]);

    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;
    if (unitTarget->GetPowerType() != drain_power)
        return;
    if (damage < 0)
        return;

    int32 curPower = unitTarget->GetPower(drain_power);

    //add spell damage bonus
    damage = m_caster->SpellDamageBonusDone(unitTarget, m_spellInfo, eff_idx, uint32(damage), SPELL_DIRECT_DAMAGE, 1, this);
    damage = unitTarget->SpellDamageBonusTaken(m_caster, m_spellInfo, eff_idx, uint32(damage), SPELL_DIRECT_DAMAGE, 1, this);

    int32 new_damage;
    if (curPower < damage)
        new_damage = curPower;
    else
        new_damage = damage;

    unitTarget->ModifyPower(drain_power, -new_damage);

    ExecuteLogInfo info(unitTarget->GetObjectGuid());
    info.powerDrain.power = drain_power;
    info.powerDrain.amount = new_damage;
    info.powerDrain.multiplier = 0.0f;

    // Don`t restore from self drain
    if (drain_power == POWER_MANA && m_caster != unitTarget)
    {
        float manaMultiplier = m_spellInfo->EffectMultipleValue[eff_idx];
        if (manaMultiplier == 0)
            manaMultiplier = 1;

        if (m_casterUnit)
        {
            if (Player *modOwner = m_casterUnit->GetSpellModOwner())
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, manaMultiplier);
        }

        int32 gain = int32(new_damage * manaMultiplier);

        if (m_casterUnit)
            m_casterUnit->ModifyPower(POWER_MANA, gain);

        info.powerDrain.multiplier = manaMultiplier;
    }
}

void Spell::EffectSendEvent(SpellEffectIndex effIdx)
{
    /*
    we do not handle a flag dropping or clicking on flag in battleground by sendevent system
    */
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell ScriptStart %u for spellid %u in EffectSendEvent ", m_spellInfo->EffectMiscValue[effIdx], m_spellInfo->Id);

    // In some cases, the spell does not require a focus but still uses a game object
    // eg. using an Altar or similar GO.
    // Therefore, pass the GO as the target if this is the case.
    GameObject* gObject = focusObject ? focusObject : m_targets.getGOTarget();

    if (!sScriptMgr.OnProcessEvent(m_spellInfo->EffectMiscValue[effIdx], m_caster, gObject, true))
        m_caster->GetMap()->ScriptsStart(sEventScripts, m_spellInfo->EffectMiscValue[effIdx], m_caster->GetObjectGuid(), gObject ? gObject->GetObjectGuid() : ObjectGuid());
}

void Spell::EffectPowerBurn(SpellEffectIndex eff_idx)
{
    if (m_spellInfo->EffectMiscValue[eff_idx] < 0 || m_spellInfo->EffectMiscValue[eff_idx] >= MAX_POWERS)
        return;

    Powers powertype = Powers(m_spellInfo->EffectMiscValue[eff_idx]);

    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;
    if (unitTarget->GetPowerType() != powertype)
        return;
    if (damage < 0)
        return;

    int32 curPower = int32(unitTarget->GetPower(powertype));

    int32 new_damage = (curPower < damage) ? curPower : damage;

    unitTarget->ModifyPower(powertype, -new_damage);
    float multiplier = m_spellInfo->EffectMultipleValue[eff_idx];

    if (m_casterUnit)
    {
        if (Player *modOwner = m_casterUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);
    }

    new_damage = int32(new_damage * multiplier);
    m_damage += new_damage;
}

void Spell::EffectHeal(SpellEffectIndex eff_idx)
{
    if (unitTarget && unitTarget->IsAlive() && damage >= 0)
    {
        // Try to get original caster
        WorldObject *caster = GetAffectiveCasterObject();
        if (!caster)
            return;

        int32 addhealth = damage;

        if (m_spellScript && !m_spellScript->OnEffectHealCalculate(this, eff_idx, addhealth))
            return;


        // Alchemists Stone
        if (m_spellInfo->Category == SPELLCATEGORY_ITEM_COMBAT_CONSUMABLE_POTION && unitTarget->HasAura(17619))
            addhealth *= 1.33f;

        addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, eff_idx, addhealth, HEAL, 1, this);
        addhealth = unitTarget->SpellHealingBonusTaken(caster, m_spellInfo, eff_idx, addhealth, HEAL, 1, this);

        m_healing += addhealth;
    }
}

void Spell::EffectHealMechanical(SpellEffectIndex eff_idx)
{
    // Mechanic creature type should be correctly checked by targetCreatureType field
    if (unitTarget && unitTarget->IsAlive() && damage >= 0)
    {
        // Try to get original caster
        WorldObject *caster = GetAffectiveCasterObject();
        if (!caster)
            return;

        uint32 addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, eff_idx, damage, HEAL);
        addhealth = unitTarget->SpellHealingBonusTaken(caster, m_spellInfo, eff_idx, addhealth, HEAL);

        caster->DealHeal(unitTarget, addhealth, m_spellInfo);
    }
}

void Spell::EffectHealthLeech(SpellEffectIndex effIndex)
{
    if (!unitTarget || !unitTarget->IsAlive() || damage < 0)
        return;

    int32 initialDamage = damage;
    damage = m_caster->SpellDamageBonusDone(unitTarget, m_spellInfo, effIndex, uint32(damage), SPELL_DIRECT_DAMAGE);
    damage = unitTarget->SpellDamageBonusTaken(m_caster, m_spellInfo, effIndex, uint32(damage), SPELL_DIRECT_DAMAGE);

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "HealthLeech :%i", damage);

    float healMultiplier = m_spellInfo->EffectMultipleValue[effIndex];

    if (m_casterUnit)
    {
        if (Player *modOwner = m_casterUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, healMultiplier);
    }

    // get max possible damage, don't count overkill for heal
    if (damage > static_cast<int32>(unitTarget->GetHealth()))
        damage = unitTarget->GetHealth();

    if (m_casterUnit && m_casterUnit->IsAlive())
        m_casterUnit->DealHeal(m_casterUnit, uint32(damage * healMultiplier), m_spellInfo);

    // Non delayed spells bonus damage is added later
    if (!m_delayed)
        damage = initialDamage;

    m_damage += damage;
}

void Spell::DoCreateItem(SpellEffectIndex eff_idx, uint32 itemtype)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    uint32 newitemid = itemtype;
    ItemPrototype const *pProto = sObjectMgr.GetItemPrototype(newitemid);
    if (!pProto)
    {
        player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, nullptr, nullptr);
        return;
    }

    // bg reward have some special in code work
    uint32 bgType = 0;
    switch (m_spellInfo->Id)
    {
        case SPELL_AV_MARK_WINNER:
        case SPELL_AV_MARK_LOSER:
            bgType = BATTLEGROUND_AV;
            break;
        case SPELL_WS_ALLY_WINNER:
        case SPELL_WS_HORDE_WINNER:
        case SPELL_WS_OLD_LOSER:
        case SPELL_WS_MARK_WINNER:
        case SPELL_WS_MARK_LOSER:
            bgType = BATTLEGROUND_WS;
            break;
        case SPELL_AB_OLD_WINNER:
        case SPELL_AB_MARK_WINNER:
        case SPELL_AB_MARK_LOSER:
            bgType = BATTLEGROUND_AB;
            break;
        case SPELL_BR_MARK_WINNER:
        case SPELL_BR_MARK_LOSER:
            bgType = BATTLEGROUND_BR;
                break;
        default:
            break;
    }

    uint32 num_to_add = damage;

    if (num_to_add < 1)
        num_to_add = 1;
    if (num_to_add > pProto->Stackable)
        num_to_add = pProto->Stackable;

    // init items_count to 1, since 1 item will be created regardless of specialization
    int items_count = 1;

    // really will be created more items
    num_to_add *= items_count;

    // can the player store the new item?
    ItemPosCountVec dest;
    uint32 no_space = 0;
    InventoryResult msg = player->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, newitemid, num_to_add, &no_space);
    if (msg != EQUIP_ERR_OK)
    {
        // convert to possible store amount
        if (msg == EQUIP_ERR_INVENTORY_FULL || msg == EQUIP_ERR_CANT_CARRY_MORE_OF_THIS)
            num_to_add -= no_space;
        else
        {
            // if not created by another reason from full inventory or unique items amount limitation
            player->SendEquipError(msg, nullptr, nullptr, newitemid);
            return;
        }
    }

    if (num_to_add)
    {
        // create the new item and store it
        Item* pItem = player->StoreNewItem(dest, newitemid, true, Item::GenerateItemRandomPropertyId(newitemid));

        // was it successful? return error if not
        if (!pItem)
        {
            player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, nullptr, nullptr);
            return;
        }

        // set the "Crafted by ..." property of the item
        if (pItem->GetProto()->HasSignature() ||
            (player->HasChallenge(CHALLENGE_CRAFTMASTER) && player->GetLevel() < PLAYER_MAX_LEVEL &&
             pItem->GetProto()->InventoryType != INVTYPE_NON_EQUIP))
            pItem->SetGuidValue(ITEM_FIELD_CREATOR, player->GetObjectGuid());

        // send info to the client
        if (pItem)
            player->SendNewItem(pItem, num_to_add, true, bgType == 0);

        // we succeeded in creating at least one item, so a levelup is possible
        if (bgType == 0)
            player->UpdateCraftSkill(m_spellInfo->Id);
    }

    // for battleground marks send by mail if not add all expected
    if (no_space > 0 && bgType)
    {
        if (BattleGround* bg = sBattleGroundMgr.GetBattleGroundTemplate(BattleGroundTypeId(bgType)))
            bg->SendRewardMarkByMail(player, newitemid, no_space);
    }
}

void Spell::EffectCreateItem(SpellEffectIndex eff_idx)
{
    DoCreateItem(eff_idx, m_spellInfo->EffectItemType[eff_idx]);

    ExecuteLogInfo info;
    info.createItem.itemEntry = m_spellInfo->EffectItemType[eff_idx];
    AddExecuteLogInfo(eff_idx, info);
}

void Spell::EffectPersistentAA(SpellEffectIndex eff_idx)
{
    WorldObject* pCaster = GetAffectiveCasterObject();

    if (GameObject* pGo = ToGameObject(pCaster))
        if (Unit* pOwner = pGo->GetOwner())
            pCaster = pOwner;

    if (!pCaster)
        pCaster = m_caster;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));

    if (Unit* pUnit = pCaster->ToUnit())
    {
        if (Player* modOwner = pUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius);
    }

    DynamicObject* dynObj = new DynamicObject;
    if (!dynObj->Create(pCaster->GetMap()->GenerateLocalLowGuid(HIGHGUID_DYNAMICOBJECT), pCaster, m_spellInfo->Id,
                        eff_idx, m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, m_duration, radius, DYNAMIC_OBJECT_AREA_SPELL))
    {
        delete dynObj;
        return;
    }

    pCaster->AddDynObject(dynObj);
    pCaster->GetMap()->Add(dynObj);
}

void Spell::EffectEnergize(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    if (m_spellInfo->EffectMiscValue[eff_idx] < 0 || m_spellInfo->EffectMiscValue[eff_idx] >= MAX_POWERS)
        return;

    Powers power = Powers(m_spellInfo->EffectMiscValue[eff_idx]);

    if (damage < 0)
        return;

    if (unitTarget->GetMaxPower(power) == 0)
        return;

    // Alchemists Stone
    if (m_spellInfo->Category == SPELLCATEGORY_ITEM_COMBAT_CONSUMABLE_POTION && unitTarget->HasAura(17619))
        damage *= 1.33f;

    if (Player* modOwner = m_casterUnit->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_POWER_GAIN, damage, this);

    m_caster->EnergizeBySpell(unitTarget, m_spellInfo->Id, damage, power);
}

void Spell::SendLoot(ObjectGuid guid, LootType loottype, LockType lockType)
{
    if (!m_casterUnit)
        return;

    if (gameObjTarget)
    {
        if (m_spellScript && m_spellScript->OnSendLoot(this, gameObjTarget, loottype, lockType))
            return;

        switch (gameObjTarget->GetGoType())
        {
            case GAMEOBJECT_TYPE_DOOR:
            case GAMEOBJECT_TYPE_BUTTON:
            case GAMEOBJECT_TYPE_QUESTGIVER:
            case GAMEOBJECT_TYPE_SPELL_FOCUS:
            case GAMEOBJECT_TYPE_GOOBER:
                gameObjTarget->Use(m_casterUnit);
                return;

            case GAMEOBJECT_TYPE_CHEST:
                gameObjTarget->Use(m_casterUnit);
                // Don't return, let loots been taken
                break;

            case GAMEOBJECT_TYPE_TRAP:
                if (lockType == LOCKTYPE_DISARM_TRAP)
                {
                    gameObjTarget->SetLootState(GO_JUST_DEACTIVATED);
                    return;
                }
                else if (gameObjTarget->GetEntry() == 178559) // Larva Spewer
                {
                    // Alternative state = destroyed
                    gameObjTarget->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                    // Save state
                    if (gameObjTarget->GetInstanceData())
                        gameObjTarget->GetInstanceData()->SetData(0 /*TYPE_LARVA_SPEWER*/, 3 /*DONE*/);
                    return;
                }
                sLog.outError("Spell::SendLoot unhandled locktype %u for GameObject trap (entry %u) for spell %u.", lockType, gameObjTarget->GetEntry(), m_spellInfo->Id);
                return;
            default:
                sLog.outError("Spell::SendLoot unhandled GameObject type %u (entry %u) for spell %u.", gameObjTarget->GetGoType(), gameObjTarget->GetEntry(), m_spellInfo->Id);
                return;
        }
    }

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // Send loot
    ((Player*)m_caster)->SendLoot(guid, loottype);
}

void Spell::EffectOpenLock(SpellEffectIndex eff_idx)
{
    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
    {
        DEBUG_LOG("WORLD: Open Lock - No Player Caster!");
        return;
    }

    Player* player = (Player*)m_caster;

    uint32 lockId = 0;
    ObjectGuid guid;

    // Get lockId
    if (gameObjTarget)
    {
        GameObjectInfo const* goInfo = gameObjTarget->GetGOInfo();

        if (goInfo->CannotBeUsedUnderImmunity() && m_caster->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IMMUNE))
            return;

        // Arathi Basin banner opening !
        if ((goInfo->type == GAMEOBJECT_TYPE_BUTTON && goInfo->button.noDamageImmune) || (goInfo->type == GAMEOBJECT_TYPE_GOOBER && goInfo->goober.losOK))
        {
            //CanUseBattleGroundObject() already called in CheckCast()
            // in battleground check
            if (BattleGround *bg = player->GetBattleGround())
            {
                // check if it's correct bg
                if (bg->GetTypeID() == BATTLEGROUND_AB || bg->GetTypeID() == BATTLEGROUND_AV || bg->GetTypeID() == BATTLEGROUND_SV)
                    bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        else if (goInfo->type == GAMEOBJECT_TYPE_FLAGSTAND)
        {
            //CanUseBattleGroundObject() already called in CheckCast()
            // in battleground check
            if (BattleGround *bg = player->GetBattleGround())
                return;
        }
        lockId = goInfo->GetLockId();
        guid = gameObjTarget->GetObjectGuid();
    }
    else if (itemTarget)
    {
        lockId = itemTarget->GetProto()->LockID;
        guid = itemTarget->GetObjectGuid();
    }
    else
    {
        DEBUG_LOG("WORLD: Open Lock - No GameObject/Item Target!");
        return;
    }

    SkillType skillId = SKILL_NONE;
    int32 reqSkillValue = 0;
    int32 skillValue;

    SpellCastResult res = CanOpenLock(eff_idx, lockId, skillId, reqSkillValue, skillValue);
    if (res != SPELL_CAST_OK)
    {
        SendCastResult(res);
        return;
    }

    // mark item as unlocked
    if (itemTarget)
    {
        itemTarget->SetFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_UNLOCKED);
        itemTarget->SetState(ITEM_CHANGED);
    }

    SendLoot(guid, LOOT_SKINNING, LockType(m_spellInfo->EffectMiscValue[eff_idx]));

    if (gameObjTarget && m_casterUnit)
    {
        if (player)
            sScriptMgr.OnGameObjectOpen(player, gameObjTarget);
        if (gameObjTarget->AI())
            gameObjTarget->AI()->OnUse(m_casterUnit);
    }

    // not allow use skill grow at item base open
    if (!m_CastItem && skillId != SKILL_NONE)
    {
        // update skill if really known
        if (uint32 pureSkillValue = player->GetSkillValuePure(skillId))
        {
            if (gameObjTarget)
            {
                // Allow one skill-up until respawned
                if (!gameObjTarget->IsInSkillupList(player) &&
                        player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue))
                    gameObjTarget->AddToSkillupList(player);
            }
            else if (itemTarget)
            {
                // Do one skill-up
                player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue);
            }
        }
    }

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(guid));
}

void Spell::EffectSummonChangeItem(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)m_caster;

    // applied only to using item
    if (!m_CastItem)
        return;

    // ... only to item in own inventory/bank/equip_slot
    if (m_CastItem->GetOwnerGuid() != player->GetObjectGuid())
        return;

    uint32 newitemid = m_spellInfo->EffectItemType[eff_idx];
    if (!newitemid)
        return;

    uint16 pos = m_CastItem->GetPos();

    Item *pNewItem = Item::CreateItem(newitemid, 1, player);
    if (!pNewItem)
        return;

    for (uint8 j = PERM_ENCHANTMENT_SLOT; j <= TEMP_ENCHANTMENT_SLOT; ++j)
    {
        if (m_CastItem->GetEnchantmentId(EnchantmentSlot(j)))
            pNewItem->SetEnchantment(EnchantmentSlot(j), m_CastItem->GetEnchantmentId(EnchantmentSlot(j)), m_CastItem->GetEnchantmentDuration(EnchantmentSlot(j)), m_CastItem->GetEnchantmentCharges(EnchantmentSlot(j)));
    }

    if (m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) < m_CastItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY))
    {
        double loosePercent = 1 - m_CastItem->GetUInt32Value(ITEM_FIELD_DURABILITY) / double(m_CastItem->GetUInt32Value(ITEM_FIELD_MAXDURABILITY));
        player->DurabilityLoss(pNewItem, loosePercent);
    }

    if (player->IsInventoryPos(pos))
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanStoreItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            ClearCastItem();

            player->StoreItem(dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsBankPos(pos))
    {
        ItemPosCountVec dest;
        uint8 msg = player->CanBankItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), dest, pNewItem, true);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            ClearCastItem();

            player->BankItem(dest, pNewItem, true);
            return;
        }
    }
    else if (player->IsEquipmentPos(pos))
    {
        uint16 dest;
        uint8 msg = player->CanEquipItem(m_CastItem->GetSlot(), dest, pNewItem, true, false);
        if (msg == EQUIP_ERR_OK)
        {
            player->DestroyItem(m_CastItem->GetBagSlot(), m_CastItem->GetSlot(), true);

            // prevent crash at access and unexpected charges counting with item update queue corrupt
            ClearCastItem();

            player->EquipItem(dest, pNewItem, true);
            player->AutoUnequipOffhandIfNeed();
            return;
        }
    }

    // fail
    delete pNewItem;
}

void Spell::EffectProficiency(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player *p_target = (Player*)unitTarget;

    uint32 subClassMask = m_spellInfo->EquippedItemSubClassMask;
    if (m_spellInfo->EquippedItemClass == ITEM_CLASS_WEAPON && !(p_target->GetWeaponProficiency() & subClassMask))
    {
        p_target->AddWeaponProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_WEAPON, p_target->GetWeaponProficiency());
    }
    if (m_spellInfo->EquippedItemClass == ITEM_CLASS_ARMOR && !(p_target->GetArmorProficiency() & subClassMask))
    {
        p_target->AddArmorProficiency(subClassMask);
        p_target->SendProficiency(ITEM_CLASS_ARMOR, p_target->GetArmorProficiency());
    }
}

void Spell::EffectApplyAreaAura(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    AreaAura* Aur = new AreaAura(m_spellInfo, eff_idx, &m_currentBasePoints[eff_idx], m_spellAuraHolder, unitTarget, m_casterUnit ? m_casterUnit : unitTarget, m_CastItem);
    m_spellAuraHolder->AddAura(Aur, eff_idx);
}

void Spell::EffectSummon(SpellEffectIndex eff_idx)
{
    if (!m_casterUnit)
        return;

    if (!m_casterUnit->GetPetGuid().IsEmpty())
        return;

    if (!unitTarget)
        return;

    uint32 pet_entry = m_spellInfo->EffectMiscValue[eff_idx];
    if (!pet_entry)
        return;

    CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(pet_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummon: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    uint32 level = m_casterUnit->GetLevel();
    Pet* spawnCreature = new Pet(SUMMON_PET);

    if (m_casterUnit->GetTypeId() == TYPEID_PLAYER && spawnCreature->LoadPetFromDB((Player*)m_casterUnit, pet_entry))
    {
        // Summon in dest location
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
            spawnCreature->Relocate(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_casterUnit->GetOrientation());

        // set timer for unsummon
        if (m_duration > 0)
            spawnCreature->SetDuration(m_duration);

        return;
    }

    // Summon in dest location
    CreatureCreatePos pos(m_casterUnit->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_caster->GetOrientation());

    if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        pos = CreatureCreatePos(m_casterUnit, -m_casterUnit->GetOrientation());

    Map *map = m_casterUnit->GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!spawnCreature->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        sLog.outErrorDb("Spell::EffectSummon: can't create creature with entry %u for spell %u", cInfo->entry, m_spellInfo->Id);
        delete spawnCreature;
        return;
    }

    spawnCreature->SetSummonPoint(pos);

    // set timer for unsummon
    if (m_duration > 0)
        spawnCreature->SetDuration(m_duration);

    spawnCreature->SetOwnerGuid(m_casterUnit->GetObjectGuid());
    spawnCreature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    spawnCreature->SetInitCreaturePowerType();
    spawnCreature->SetFactionTemplateId(m_casterUnit->GetFactionTemplateId());
    spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
    spawnCreature->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
    spawnCreature->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
    spawnCreature->SetCreatorGuid(m_casterUnit->GetObjectGuid());
    spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    spawnCreature->InitStatsForLevel(level, m_casterUnit);

    spawnCreature->GetCharmInfo()->SetPetNumber(pet_number, false);

    spawnCreature->InitializeDefaultName();
    spawnCreature->AIM_Initialize();
    spawnCreature->InitPetCreateSpells();
    spawnCreature->SetHealth(spawnCreature->GetMaxHealth());
    spawnCreature->SetPower(POWER_MANA, spawnCreature->GetMaxPower(POWER_MANA));

    map->Add((Creature*)spawnCreature);

    m_casterUnit->SetPet(spawnCreature);

    if (m_casterUnit->GetTypeId() == TYPEID_PLAYER)
    {
        spawnCreature->SetReactState(REACT_DEFENSIVE);
        spawnCreature->SavePetToDB(PET_SAVE_AS_CURRENT);
        ((Player*)m_casterUnit)->PetSpellInitialize();
    }
    else
        spawnCreature->SetReactState(REACT_AGGRESSIVE);

    if (m_casterUnit->GetTypeId() == TYPEID_UNIT && ((Creature*)m_casterUnit)->AI())
        ((Creature*)m_casterUnit)->AI()->JustSummoned((Creature*)spawnCreature);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(spawnCreature->GetObjectGuid()));

    if (m_spellScript)
        m_spellScript->OnSummon(this, spawnCreature);
}

void Spell::EffectLearnSpell(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            EffectLearnPetSpell(eff_idx);

        return;
    }

    Player *player = (Player*)unitTarget;

    uint32 spellToLearn = m_spellInfo->EffectTriggerSpell[eff_idx];
    player->LearnSpell(spellToLearn, false);

    DEBUG_LOG("Spell: Player %u has learned spell %u from NpcGUID=%u", player->GetGUIDLow(), spellToLearn, m_caster->GetGUIDLow());
}

void Spell::EffectDispel(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    // Fill possible dispel list
    int32 priority_dispel = -1;
    std::list <std::pair<SpellAuraHolder* , uint32> > dispel_list;

    bool checkFaction = true;
    // Pierre de sort dissipe sorts negatifs et positifs.
    if (m_spellInfo->IsFitToFamily<SPELLFAMILY_WARLOCK, CF_WARLOCK_SPELLSTONE>())
        checkFaction = false;
    bool friendly = checkFaction && !isReflected ? unitTarget->IsFriendlyTo(m_caster) : false;
    // Create dispel mask by dispel type
    int32 dispel_type = m_spellInfo->EffectMiscValue[eff_idx];
    uint32 dispelMask  = GetDispellMask(dispel_type < 0 ? DISPEL_ALL : DispelType(dispel_type));
    Unit::SpellAuraHolderMap const& auras = unitTarget->GetSpellAuraHolderMap();
    for (const auto& aura : auras)
    {
        SpellAuraHolder* holder = aura.second;
        if ((1 << holder->GetSpellProto()->Dispel) & dispelMask)
        {
            if (holder->GetSpellProto()->Dispel == DISPEL_MAGIC ||
                holder->GetSpellProto()->Dispel == DISPEL_POISON)
            {
                if (checkFaction)
                {
                    bool positive = holder->IsPositive();
                    // do not remove positive auras if friendly target
                    // do not remove negative auras if non-friendly target
                    // when removing charm auras ignore hostile reaction from the charm
                    if (!friendly && holder->GetSpellProto()->IsCharmSpell())
                    {
                        if (CharmInfo *charm = unitTarget->GetCharmInfo())
                            if (FactionTemplateEntry const* ft = charm->GetOriginalFactionTemplate())
                                if (ft->IsFriendlyTo(*m_caster->GetFactionTemplateEntry()))
                                    priority_dispel = dispel_list.size();
                    }
                    else if (positive == friendly)
                        continue;
                }
            }
            dispel_list.push_back(std::pair<SpellAuraHolder* , uint32>(holder, holder->GetStackAmount()));
        }
    }
    // Ok if exist some buffs for dispel try dispel it
    if (!dispel_list.empty())
    {
        std::vector<std::pair<SpellAuraHolder* , uint32> > success_list; // (spell_id,casterGuid)
        std::vector < uint32 > fail_list; // spell_id

        // some spells have effect value = 0 and all from its by meaning expect 1
        if (!damage)
            damage = 1;

        // Dispel N = damage buffs (or while exist buffs for dispel)
        for (int32 count = 0; count < damage && !dispel_list.empty(); ++count)
        {
            // Random select buff for dispel
            std::list<std::pair<SpellAuraHolder*, uint32> >::iterator dispel_itr = dispel_list.begin();
            if (priority_dispel >= 0)
            {
                std::advance(dispel_itr, priority_dispel);
                priority_dispel = -1;
            }
            else
            {
                std::advance(dispel_itr, urand(0, dispel_list.size() - 1));
            }

            SpellAuraHolder *holder = dispel_itr->first;

            dispel_itr->second -= 1;

            // remove entry from dispel_list if nothing left in stack
            if (dispel_itr->second == 0)
                dispel_list.erase(dispel_itr);

            SpellEntry const* spellInfo = holder->GetSpellProto();
            // Base dispel chance
            // TODO: possible chance depend from spell level??
            int32 miss_chance = 0;
            // Apply dispel mod from aura caster
            if (Unit *caster = holder->GetCaster())
            {
                if (Player* modOwner = caster->GetSpellModOwner())
                    modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_RESIST_DISPEL_CHANCE, miss_chance, this);
            }
            // Try dispel
            if (roll_chance_i(miss_chance))
                fail_list.push_back(spellInfo->Id);
            else
            {
                bool foundDispelled = false;
                for (auto& success_iter : success_list)
                {
                    if (success_iter.first->GetId() == holder->GetId() && success_iter.first->GetCasterGuid() == holder->GetCasterGuid())
                    {
                        success_iter.second += 1;
                        foundDispelled = true;
                        break;
                    }
                }
                if (!foundDispelled)
                    success_list.push_back(std::pair<SpellAuraHolder* , uint32>(holder, 1));
            }
        }
        // Send success log and really remove auras
        if (!success_list.empty())
        {
            int32 count = success_list.size();
            WorldPacket data(SMSG_SPELLDISPELLOG, 8 + 8 + 4 + count * 4);
            data << unitTarget->GetPackGUID();              // Victim GUID
            data << m_caster->GetPackGUID();                // Caster GUID
            data << uint32(count);
            for (const auto& j : success_list)
            {
                SpellAuraHolder* dispelledHolder = j.first;
                uint32 removedAura = dispelledHolder->GetId();
                ObjectGuid removedAuraCasterGuid = dispelledHolder->GetCasterGuid();
                uint32 dispelCount = j.second;
                data << removedAura;   // Spell Id

                if (dispelledHolder->GetAuraScript())
                    dispelledHolder->GetAuraScript()->OnDispel(dispelledHolder, unitTarget, this, dispelCount);

                unitTarget->RemoveAuraHolderDueToSpellByDispel(removedAura, dispelCount, removedAuraCasterGuid);

                if (m_spellScript)
                    m_spellScript->OnSuccessfulSpecificDispel(this, eff_idx, removedAura, removedAuraCasterGuid, dispelCount);
            }

            m_caster->SendMessageToSet(&data, true);
        }
        // Send fail log to client
        if (!fail_list.empty())
        {
            // Failed to dispel
            WorldPacket data(SMSG_DISPEL_FAILED, 8 + 8 + 4 * fail_list.size());
            data << m_caster->GetObjectGuid();              // Caster GUID
            data << unitTarget->GetObjectGuid();            // Victim GUID
            for (const auto& j : fail_list)
                data << uint32(j);                         // Spell Id
            m_caster->SendMessageToSet(&data, true);
        }
    }

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectDualWield(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanDualWield(true);
}

void Spell::EffectPull(SpellEffectIndex /*eff_idx*/)
{
    // TODO: create a proper pull towards distract spell center for distract
    DEBUG_LOG("WORLD: Spell Effect DUMMY");
}

void Spell::EffectDistract(SpellEffectIndex eff_idx)
{
    // Check for possible target
    if (!unitTarget || unitTarget->IsInCombat())
        return;

    // target must be OK to do this
    if (unitTarget->HasUnitState(UNIT_STAT_CAN_NOT_REACT))
        return;

    unitTarget->SetFacingTo(unitTarget->GetAngle(m_targets.m_destX, m_targets.m_destY));
    unitTarget->ClearUnitState(UNIT_STAT_MOVING);

    if (unitTarget->GetTypeId() == TYPEID_UNIT)
        unitTarget->GetMotionMaster()->MoveDistract(damage * IN_MILLISECONDS);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectPickPocket(SpellEffectIndex /*eff_idx*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // victim must be creature and attackable
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || !unitTarget->IsAlive() || m_caster->IsFriendlyTo(unitTarget))
        return;

    ((Player*)m_caster)->SendLoot(unitTarget->GetObjectGuid(), LOOT_PICKPOCKETING);
}

void Spell::EffectAddFarsight(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 duration = m_spellInfo->GetDuration();
    DynamicObject* dynObj = new DynamicObject;

    // set radius to 0: spell not expected to work as persistent aura
    if (!dynObj->Create(m_caster->GetMap()->GenerateLocalLowGuid(HIGHGUID_DYNAMICOBJECT), m_caster,
                        m_spellInfo->Id, eff_idx, m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, duration, 0, DYNAMIC_OBJECT_FARSIGHT_FOCUS))
    {
        delete dynObj;
        return;
    }

    m_caster->AddDynObject(dynObj);
    m_caster->GetMap()->Add(dynObj);

    ((Player*)m_caster)->GetCamera().SetView(dynObj);
}

void Spell::EffectSummonWild(SpellEffectIndex eff_idx)
{
    uint32 creature_entry = m_spellInfo->EffectMiscValue[eff_idx];
    if (!creature_entry)
        return;

    uint32 level = m_caster->GetLevel();

    // level of creature summoned using engineering item based at engineering skill level
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_CastItem)
    {
        ItemPrototype const *proto = m_CastItem->GetProto();
        if (proto && proto->RequiredSkill == SKILL_ENGINEERING)
        {
            uint16 skill202 = ((Player*)m_caster)->GetSkillValue(SKILL_ENGINEERING);
            if (skill202)
                level = skill202 / 5;
        }
    }

    // select center of summon position
    float center_x = m_targets.m_destX;
    float center_y = m_targets.m_destY;
    float center_z = m_targets.m_destZ;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));
    int32 duration = m_spellInfo->GetDuration();
    TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DEATH_AND_DEAD_DESPAWN;

    int32 amount = damage > 0 ? damage : 1;

    for (int32 count = 0; count < amount; ++count)
    {
        float px, py, pz;
        // If dest location if present
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            // Summon 1 unit in dest location
            if (count == 0)
            {
                px = m_targets.m_destX;
                py = m_targets.m_destY;
                pz = m_targets.m_destZ;
            }
            // Summon in random point all other units if location present
            else
                m_caster->GetRandomPoint(center_x, center_y, center_z, radius, px, py, pz);
        }
        // Summon if dest location not present near caster
        else
        {
            if (radius > 0.0f)
            {
                // not using bounding radius of caster here
                m_caster->GetClosePoint(px, py, pz, 0.0f, radius);
            }
            else
            {
                // EffectRadiusIndex 0 or 36
                px = m_caster->GetPositionX();
                py = m_caster->GetPositionY();
                pz = m_caster->GetPositionZ();
            }
        }

        if (Creature *summon = m_caster->SummonCreature(creature_entry, px, py, pz, m_caster->GetOrientation(), summonType, duration))
        {
            summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
            // UNIT_FIELD_CREATEDBY are not set for these kind of spells.
            // Does exceptions exist? If so, what are they?
            // summon->SetCreatorGuid(m_caster->GetObjectGuid());

            if (count == 0)
                AddExecuteLogInfo(eff_idx, ExecuteLogInfo(summon->GetObjectGuid()));

            if (m_spellScript)
                m_spellScript->OnSummon(this, summon);
        }
    }
}

// Used for SPELL_EFFECT_SUMMON_GUARDIAN and SPELL_EFFECT_SUMMON_POSSESSED
void Spell::EffectSummonGuardian(SpellEffectIndex eff_idx)
{
    if (!m_casterUnit)
        return;

    uint32 pet_entry = m_spellInfo->EffectMiscValue[eff_idx];
    if (!pet_entry)
        return;

    CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(pet_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummonGuardian: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    // second direct cast unsummon guardian(s) (guardians without like functionality have cooldown > spawn time)
    if (!m_IsTriggeredSpell && m_casterUnit->GetTypeId() == TYPEID_PLAYER)
    {
        bool found = false;
        // including protector
        while (Pet* old_summon = m_casterUnit->FindGuardianWithEntry(pet_entry))
        {
            old_summon->Unsummon(PET_SAVE_AS_DELETED, m_casterUnit);
            found = true;
        }

        if (found && !(m_spellInfo->DurationIndex && m_spellInfo->Category))
            return;
    }

    // Hard cap for NPC summoned guardians
    if (m_casterUnit->GetTypeId() != TYPEID_PLAYER && m_casterUnit->GetGuardianCountWithEntry(pet_entry) > 15)
        return;

    // Guardian pets use their creature template level by default
    uint32 level = urand(cInfo->level_min, cInfo->level_max);
    if (m_casterUnit->GetTypeId() != TYPEID_PLAYER)
    {
        // If EffectMultipleValue <= 0, guardian pets use their caster level modified by EffectMultipleValue for their own level
        if (m_spellInfo->EffectMultipleValue[eff_idx] <= 0)
        {
            uint32 resultLevel = std::max(m_casterUnit->GetLevel() + m_spellInfo->EffectMultipleValue[eff_idx], 0.0f);

            // Result level should be a valid level for creatures
            if (resultLevel > 0 && resultLevel <= CREATURE_MAX_LEVEL)
                level = resultLevel;
        }
    }
    // level of pet summoned using engineering item based at engineering skill level
    else if (m_CastItem)
    {
        ItemPrototype const *proto = m_CastItem->GetProto();
        if (proto && proto->RequiredSkill == SKILL_ENGINEERING)
        {
            uint16 engiLevel = ((Player*)m_casterUnit)->GetSkillValue(SKILL_ENGINEERING);
            if (engiLevel)
            {
                level = engiLevel / 5;
            }
        }
    }

    // select center of summon position
    float center_x = m_targets.m_destX;
    float center_y = m_targets.m_destY;
    float center_z = m_targets.m_destZ;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));

    int32 amount = damage > 0 ? damage : 1;

    for (int32 count = 0; count < amount; ++count)
    {
        Pet* spawnCreature = new Pet(GUARDIAN_PET);

        // If dest location if present
        // Summon 1 unit in dest location
        CreatureCreatePos pos(m_casterUnit->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_casterUnit->GetOrientation());

        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            // Summon in random point all other units if location present
            if (count > 0)
            {
                float x, y, z;
                m_casterUnit->GetRandomPoint(center_x, center_y, center_z, radius, x, y, z);
                pos = CreatureCreatePos(m_casterUnit->GetMap(), x, y, z, m_casterUnit->GetOrientation());
            }
        }
        // Summon if dest location not present near caster
        else
            pos = CreatureCreatePos(m_casterUnit, m_casterUnit->GetOrientation());

        Map *map = m_casterUnit->GetMap();
        uint32 pet_number = sObjectMgr.GeneratePetNumber();
        if (!spawnCreature->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
        {
            sLog.outError("Spell::DoSummonGuardian: can't create creature entry %u for spell %u.", pet_entry, m_spellInfo->Id);
            delete spawnCreature;
            return;
        }

        spawnCreature->SetSummonPoint(pos);

        if (m_duration > 0)
            spawnCreature->SetDuration(m_duration);

        spawnCreature->SetOwnerGuid(m_casterUnit->GetObjectGuid());
        spawnCreature->SetInitCreaturePowerType();
        spawnCreature->SetUInt32Value(UNIT_NPC_FLAGS, spawnCreature->GetCreatureInfo()->npc_flags);
        spawnCreature->SetFactionTemplateId(m_casterUnit->GetFactionTemplateId());
        spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
        spawnCreature->SetCreatorGuid(m_casterUnit->GetObjectGuid());
        spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

        uint32 totalGuardians = m_casterUnit->GetGuardiansCount();
        float followAngle = PET_FOLLOW_ANGLE + (M_PI_F / 6) * totalGuardians;
        while (followAngle > M_PI_F * 2)
            followAngle -= M_PI_F * 2;
        spawnCreature->SetFollowAngle(followAngle);

        spawnCreature->InitStatsForLevel(level, m_casterUnit);
        spawnCreature->GetCharmInfo()->SetPetNumber(pet_number, false);
        spawnCreature->LoadCreatureAddon();
        spawnCreature->InitializeDefaultName();
        spawnCreature->AIM_Initialize();

        m_casterUnit->AddGuardian(spawnCreature);

        if (m_spellScript)
            m_spellScript->OnSummonBeforeAdd(this, spawnCreature, count);

        map->Add((Creature*)spawnCreature);

        // Notify Summoner
        if (m_casterUnit->GetTypeId() == TYPEID_UNIT && ((Creature*)m_casterUnit)->AI())
            ((Creature*)m_casterUnit)->AI()->JustSummoned(spawnCreature);

        if (count == 0)
            AddExecuteLogInfo(eff_idx, ExecuteLogInfo(spawnCreature->GetObjectGuid()));

        if (m_spellScript)
            m_spellScript->OnSummon(this, spawnCreature);
    }
}

void Spell::EffectSummonPossessed(SpellEffectIndex eff_idx)
{
    Player* pCaster = m_caster->ToPlayer();
    if (!pCaster)
        return;

    uint32 creatureEntry = m_spellInfo->EffectMiscValue[eff_idx];

    Creature* pMinion = pCaster->SummonPossessedMinion(creatureEntry, m_spellInfo->Id, m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, m_caster->GetOrientation(), m_spellInfo->GetDuration());
    if (!pMinion)
    {
        sLog.outError("Spell::EffectSummonPossessed: creature entry %u for spell %u could not be summoned.", creatureEntry, m_spellInfo->Id);
        return;
    }

    // Notify Summoner
    if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->AI())
        m_originalCaster->AI()->JustSummoned(pMinion);

    if (m_spellScript)
        m_spellScript->OnSummon(this, pMinion);
}

void Spell::EffectTeleUnitsFaceCaster(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    if (unitTarget->IsTaxiFlying())
        return;

    float fx, fy, fz;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.getDestination(fx, fy, fz);
    else
    {
        float dis = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx]));
        m_caster->GetClosePoint(fx, fy, fz, unitTarget->GetObjectBoundingRadius(), dis);
    }

    unitTarget->NearTeleportTo(fx, fy, fz, -m_caster->GetOrientation(), TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget == m_caster ? TELE_TO_SPELL : 0));
}

void Spell::EffectLearnSkill(SpellEffectIndex eff_idx)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (damage < 0)
        return;

    Player* target = static_cast<Player*>(unitTarget);

    uint16 skillid = uint16(m_spellInfo->EffectMiscValue[eff_idx]);
    uint16 step = uint16(damage);
    uint16 current = std::max(uint16(1), target->GetSkillValuePure(skillid));
    uint16 max = (step * 75);
    target->SetSkill(skillid, current, max, step);

    if (WorldObject const* caster = GetCastingObject())
        DEBUG_LOG("Spell: %s has learned skill %u (to maxlevel %u) from %s", target->GetGuidStr().c_str(), skillid, max, caster->GetGuidStr().c_str());
}

void Spell::EffectAddHonor(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // honor-spells don't scale with level and won't be casted by an item
    // also we must use damage (spelldescription says +25 honor but damage is only 24)
    uint32 const honor = uint32(std::max(1.0f, std::ceil(float(damage) * 0.1f)));
    ((Player*)unitTarget)->GetHonorMgr().Add(float(honor), QUEST);
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "SpellEffect::AddHonor (spell_id %u) rewards %u honor points for player: %u", m_spellInfo->Id, honor, ((Player*)unitTarget)->GetGUIDLow());
}

void Spell::EffectSpawn(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || (unitTarget->GetTypeId() != TYPEID_UNIT))
        return;

    if (unitTarget->GetVisibility() != VISIBILITY_ON)
        unitTarget->SetVisibility(VISIBILITY_ON);
    unitTarget->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SPAWNING);
}

void Spell::EffectTradeSkill(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    // uint32 skillid =  m_spellInfo->EffectMiscValue[i];
    // uint16 skillmax = ((Player*)unitTarget)->(skillid);
    // ((Player*)unitTarget)->SetSkill(skillid,skillval?skillval:1,skillmax+75);
}

void Spell::EffectEnchantItemPerm(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!itemTarget)
        return;

    Player* p_caster = (Player*)m_caster;

    // not grow at item use at item case
    p_caster->UpdateCraftSkill(m_spellInfo->Id);

    uint32 enchant_id = m_spellInfo->EffectMiscValue[eff_idx];
    if (!enchant_id)
        return;

    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
        return;

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    if (itemTarget->CanBeTradedEvenIfSoulBound())
        return;

    if (item_owner->HasChallenge(CHALLENGE_VAGRANT_MODE) && item_owner->GetLevel() < PLAYER_MAX_LEVEL && itemTarget->IsEquipped())
    {
        p_caster->GetSession()->SendNotification("You cannot enchant items that are currently equipped while participating in a Vagrant's Endeavor challenge.");
        return;
    }

    if (!sWorld.getConfig(CONFIG_BOOL_GM_ALLOW_TRADES) && p_caster->GetSession()->GetSecurity() > SEC_PLAYER)
        return;

    if (item_owner != p_caster && p_caster->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_BOOL_GM_LOG_TRADE))
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(), "GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
                        p_caster->GetName(), p_caster->GetSession()->GetAccountId(),
                        itemTarget->GetProto()->Name1.c_str(), itemTarget->GetEntry(),
                        item_owner->GetName(), item_owner->GetSession()->GetAccountId());
    }

    // remove old enchanting before applying new if equipped
    item_owner->ApplyEnchantment(itemTarget, PERM_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, PERM_ENCHANTMENT_SLOT, true);

    // do not allow trading raid items after enchanting them
    if (itemTarget->CanBeTradedEvenIfSoulBound())
        itemTarget->ResetSoulBoundTradeData();
}

void Spell::EffectEnchantItemTmp(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;

    if (!itemTarget)
        return;

    uint32 enchant_id = m_spellInfo->EffectMiscValue[eff_idx];
    uint32 charges    = sSpellMgr.GetSpellEnchantCharges(m_spellInfo->Id);

    if (!enchant_id)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have 0 as enchanting id", m_spellInfo->Id, eff_idx);
        return;
    }

    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have nonexistent enchanting id %u ", m_spellInfo->Id, eff_idx, enchant_id);
        return;
    }

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    if (itemTarget->CanBeTradedEvenIfSoulBound())
        return;

    if (!sWorld.getConfig(CONFIG_BOOL_GM_ALLOW_TRADES) && p_caster->GetSession()->GetSecurity() > SEC_PLAYER)
        return;

    if (item_owner != p_caster && p_caster->GetSession()->GetSecurity() > SEC_PLAYER && sWorld.getConfig(CONFIG_BOOL_GM_LOG_TRADE))
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(), "GM %s (Account: %u) enchanting(temp): %s (Entry: %d) for player: %s (Account: %u)",
                        p_caster->GetName(), p_caster->GetSession()->GetAccountId(),
                        itemTarget->GetProto()->Name1.c_str(), itemTarget->GetEntry(),
                        item_owner->GetName(), item_owner->GetSession()->GetAccountId());
    }

    // remove old enchant before applying new
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant_id, damage * 1000, charges);

    // add new enchanting if equipped
    item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, true);

    // do not allow trading raid items after enchanting them
    if (itemTarget->CanBeTradedEvenIfSoulBound())
        itemTarget->ResetSoulBoundTradeData();
}

void Spell::EffectTameCreature(SpellEffectIndex /*eff_idx*/)
{
    // Caster must be player, checked in Spell::CheckCast
    // Spell can be triggered, we need to check original caster prior to caster
    Player* plr = (Player*)GetAffectiveCaster();

    Creature* creatureTarget = (Creature*)unitTarget;

    // cast finish successfully
    //SendChannelUpdate(0);
    finish();

    Pet* pet = new Pet(HUNTER_PET);

    // Nostalrius: defensive as default behaviour
    pet->SetReactState(REACT_DEFENSIVE);

    if (!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        return;
    }

    pet->SetOwnerGuid(plr->GetObjectGuid());
    pet->SetCreatorGuid(plr->GetObjectGuid());
    pet->SetFactionTemplateId(plr->GetFactionTemplateId());
    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    if (plr->IsPvP())
        pet->SetPvP(true);

    if (!pet->InitStatsForLevel(creatureTarget->GetLevel()))
    {
        sLog.outError("Pet::InitStatsForLevel() failed for creature (Entry: %u)!", creatureTarget->GetEntry());
        delete pet;
        return;
    }

    pet->GetCharmInfo()->SetPetNumber(pet->GetObjectGuid().GetEntry(), true);
    pet->InitializeDefaultName();
    pet->AIM_Initialize();
    pet->InitPetCreateSpells();
    pet->SetHealth(pet->GetMaxHealth());

    // "kill" original creature
    creatureTarget->ForcedDespawn();

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->GetLevel() - 1);

    // Apply default loyalty at summon
    LoyaltyLevel defaultLoyalty = LoyaltyLevel(sWorld.getConfig(CONFIG_UINT32_PET_DEFAULT_LOYALTY));
    while (pet->GetLoyaltyLevel() != defaultLoyalty)
        pet->ModifyLoyalty(pet->GetStartLoyaltyPoints(defaultLoyalty));

    // add to world
    pet->GetMap()->Add((Creature*)pet);

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, creatureTarget->GetLevel());

    // caster have pet now
    plr->SetPet(pet);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    plr->PetSpellInitialize();
}

void Spell::EffectSummonPet(SpellEffectIndex eff_idx)
{
    if (!m_casterUnit)
        return;

    uint32 petLevel = m_casterUnit->IsPlayer() ? m_casterUnit->GetLevel() : std::max(int32(m_casterUnit->GetLevel()) + int32(m_spellInfo->EffectMultipleValue[eff_idx]), 1);

    ObjectGuid petGuid = m_casterUnit->EffectSummonPet(m_spellInfo->Id, m_spellInfo->EffectMiscValue[eff_idx], petLevel);
    if (petGuid)
        AddExecuteLogInfo(eff_idx, ExecuteLogInfo(petGuid));
}

ObjectGuid Unit::EffectSummonPet(uint32 spellId, uint32 petEntry, uint32 petLevel)
{
    if (!UnsummonOldPetBeforeNewSummon(petEntry))
        return ObjectGuid();

    CreatureInfo const* cInfo = petEntry ? sObjectMgr.GetCreatureTemplate(petEntry) : nullptr;

    // == 0 in case call current pet, check only real summon case
    if (petEntry && !cInfo)
    {
        sLog.outErrorDb("EffectSummonPet: creature entry %u not found for spell %u.", petEntry, spellId);
        return ObjectGuid();
    }

    Pet* NewSummon = new Pet;

    // petEntry==0 for hunter "call pet" (current pet summoned if any)
    if (GetTypeId() == TYPEID_PLAYER && NewSummon->LoadPetFromDB((Player*)this, petEntry))
    {
        if (NewSummon->getPetType() == SUMMON_PET)
        {
            // Remove Demonic Sacrifice auras (known pet)
            Unit::AuraList const& auraClassScripts = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
            for (Unit::AuraList::const_iterator itr = auraClassScripts.begin(); itr != auraClassScripts.end();)
            {
                if ((*itr)->GetModifier()->m_miscvalue == 2228)
                {
                    RemoveAurasDueToSpell((*itr)->GetId());
                    itr = auraClassScripts.begin();
                }
                else
                    ++itr;
            }
        }
        return NewSummon->GetObjectGuid();
    }

    // not error in case fail hunter call pet
    if (!petEntry)
    {
        delete NewSummon;
        return ObjectGuid();
    }

    CreatureCreatePos pos(this, GetOrientation(), PET_FOLLOW_DIST, MINI_PET_SUMMON_ANGLE);

    Map *map = GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!NewSummon->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        delete NewSummon;
        return ObjectGuid();
    }

    NewSummon->SetSummonPoint(pos);

    NewSummon->setPetType(SUMMON_PET);

    uint32 faction = GetFactionTemplateId();
    if (GetTypeId() == TYPEID_UNIT)
        NewSummon->SetReactState(REACT_AGGRESSIVE);
    else
        NewSummon->SetReactState(REACT_DEFENSIVE);

    NewSummon->SetOwnerGuid(GetObjectGuid());
    NewSummon->SetCreatorGuid(GetObjectGuid());
    NewSummon->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    NewSummon->SetFactionTemplateId(faction);
    NewSummon->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(nullptr)));
    NewSummon->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
    NewSummon->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
    NewSummon->SetUInt32Value(UNIT_CREATED_BY_SPELL, spellId);

    // Only players have permanent pets. Fix elite pets for NPCs.
    NewSummon->GetCharmInfo()->SetPetNumber(pet_number, GetTypeId() == TYPEID_PLAYER);
    NewSummon->GetCharmInfo()->SetCommandState(COMMAND_FOLLOW);

    // this enables pet details window (Shift+P)
    if (IsPvP())
        NewSummon->SetPvP(true);

    NewSummon->InitStatsForLevel(petLevel, this);
    NewSummon->InitPetCreateSpells();

    if (NewSummon->getPetType() == SUMMON_PET)
    {
        // Remove Demonic Sacrifice auras (new pet)
        Unit::AuraList const& auraClassScripts = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (Unit::AuraList::const_iterator itr = auraClassScripts.begin(); itr != auraClassScripts.end();)
        {
            if ((*itr)->GetModifier()->m_miscvalue == 2228)
            {
                RemoveAurasDueToSpell((*itr)->GetId());
                itr = auraClassScripts.begin();
            }
            else
                ++itr;
        }

        // generate new name for summon pet
        std::string new_name = sObjectMgr.GeneratePetName(petEntry);
        if (!new_name.empty())
            NewSummon->SetName(new_name);
        else
            NewSummon->InitializeDefaultName();
    }
    else if (NewSummon->getPetType() == HUNTER_PET)
    {
        NewSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PET_RENAME);
        NewSummon->InitializeDefaultName();
    }

    NewSummon->AIM_Initialize();
    NewSummon->SetHealth(NewSummon->GetMaxHealth());
    NewSummon->SetPower(POWER_MANA, NewSummon->GetMaxPower(POWER_MANA));

    map->Add((Creature*)NewSummon);

    SetPet(NewSummon);
    DEBUG_LOG("New Pet has guid %u", NewSummon->GetGUIDLow());

    if (Player* caster = ToPlayer())
    {
        NewSummon->SavePetToDB(PET_SAVE_AS_CURRENT);
        caster->PetSpellInitialize();
        caster->m_petEntry = NewSummon->GetEntry();
        caster->m_petSpell = NewSummon->GetUInt32Value(UNIT_CREATED_BY_SPELL);
    }

    return NewSummon->GetObjectGuid();
}

void Spell::EffectLearnPetSpell(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)m_caster;

    Pet *pet = _player->GetPet();
    if (!pet)
        return;
    if (!pet->IsAlive())
        return;

    SpellEntry const *learn_spellproto = sSpellMgr.GetSpellEntry(m_spellInfo->EffectTriggerSpell[eff_idx]);
    if (!learn_spellproto)
        return;

    if (!pet->CanLearnPetSpell(learn_spellproto->Id))
        return;

    pet->SetTP(pet->m_TrainingPoints - pet->GetTPForSpell(learn_spellproto->Id));
    pet->LearnSpell(learn_spellproto->Id);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    _player->PetSpellInitialize();
}

void Spell::EffectTaunt(SpellEffectIndex eff_idx)
{
    if (!unitTarget || !m_casterUnit)
        return;

    // this effect use before aura Taunt apply for prevent taunt already attacking target
    // for spell as marked "non effective at already attacking target"
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (unitTarget->GetVictim() == m_caster)
        {
            SendCastResult(SPELL_FAILED_DONT_REPORT);
            return;
        }
    }

    // Also use this effect to set the taunter's threat to the taunted creature's highest value
    if (unitTarget->CanHaveThreatList() && unitTarget->GetThreatManager().getCurrentVictim())
    {
        //Add the difference of the Current highest value threat and the taunter's threat
        unitTarget->GetThreatManager().addThreat(m_casterUnit, unitTarget->GetThreatManager().getCurrentVictim()->getThreat() - unitTarget->GetThreatManager().getThreat(m_casterUnit, false));


        // Patch 1.11 notes
        // https://web.archive.org/web/20061109034626/http://evilempireguild.org:80/guides/kenco2.php
        // Recently(1.11.x), the behaviour of Taunt has been buffed slightly.It now does three things :

        // Taunt debuff.The mob is forced to attack you for 3 seconds.Later taunts by other players override this.
        // You are given threat equal to the mob's previous aggro target, permanently. Importantly, you won't necessarily get as much threat
        // as the highest person on the mob's list, only as much as whoever is currently tanking it.
        // You gain complete aggro on the mob at the instant you taunt.Usually you would need 10 % more threat to gain aggro(see section 3),
        // but a taunt now gives you instant aggro on the mob.
        // Of course if other people are generating significant threat on the mob, they could exceed your threat by more than 10 % before the taunt debuff wears off,
        // and will gain aggro as soon as it does.There is no limit to the amount of threat you can gain from Taunt.
        unitTarget->GetThreatManager().setCurrentVictimIfCan(m_casterUnit);
    }

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectWeaponDmg(SpellEffectIndex eff_idx)
{
    if (!m_casterUnit)
        return;

    if (!unitTarget)
        return;

    if (!unitTarget->IsAlive())
        return;

    // multiple weapon dmg effect workaround
    // execute only the last weapon damage
    // and handle all effects at once
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        switch (m_spellInfo->Effect[j])
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                if (j < int(eff_idx))                             // we must calculate only at last weapon effect
                    return;
                break;
        }
    }

    // some spell specific modifiers
    bool customBonusDamagePercentMod = false;
    float bonusDamagePercentMod  = 1.0f;                    // applied to fixed effect damage bonus if set customBonusDamagePercentMod
    float weaponDamagePercentMod = 1.0f;                    // applied to weapon damage (and to fixed effect damage bonus if customBonusDamagePercentMod not set
    bool normalized = false;

    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_ROGUE:
        {
            // Ambush
            if (m_spellInfo->IsFitToFamilyMask<CF_ROGUE_AMBUSH>())
            {
                customBonusDamagePercentMod = true;
                bonusDamagePercentMod = 2.5f;               // 250%
            }
            break;
        }
    }

    int32 bonus = 0;
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        switch (m_spellInfo->Effect[j])
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                bonus += CalculateDamage(SpellEffectIndex(j), unitTarget);
                break;
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
                bonus += CalculateDamage(SpellEffectIndex(j), unitTarget);
                normalized = true;
                break;
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                weaponDamagePercentMod *= float(CalculateDamage(SpellEffectIndex(j), unitTarget)) / 100.0f;

                // applied only to prev.effects fixed damage
                if (customBonusDamagePercentMod)
                    bonus = int32(bonus * bonusDamagePercentMod);
                else
                    bonus = int32(bonus * weaponDamagePercentMod);
                break;
            default:
                break;                                      // not weapon damage effect, just skip
        }
    }

    // apply to non-weapon bonus weapon total pct effect, weapon total flat effect included in weapon damage
    if (bonus)
    {
        UnitMods unitMod;
        switch (m_attackType)
        {
            default:
            case BASE_ATTACK:
                unitMod = UNIT_MOD_DAMAGE_MAINHAND;
                break;
            case OFF_ATTACK:
                unitMod = UNIT_MOD_DAMAGE_OFFHAND;
                break;
            case RANGED_ATTACK:
                unitMod = UNIT_MOD_DAMAGE_RANGED;
                break;
        }

        float weapon_total_pct = m_casterUnit->GetModifierValue(unitMod, TOTAL_PCT);
        bonus = int32(bonus * weapon_total_pct);
    }

    // + weapon damage with applied weapon% dmg to base weapon damage in call
    for (uint8 i = 0; i < m_casterUnit->GetWeaponDamageCount(m_attackType); i++)
    {
        if (unitTarget->IsImmuneToDamage(GetSchoolMask(m_casterUnit->GetWeaponDamageSchool(m_attackType, i))))
            continue;

        bonus += int32(m_casterUnit->CalculateDamage(m_attackType, normalized, i) * weaponDamagePercentMod);
    }

    // Seal of Command
    if (m_spellInfo->School == SPELL_SCHOOL_HOLY)
    {
        // Add spell gear bonus and spell modifiers
        bonus = m_casterUnit->SpellDamageBonusDone(unitTarget, m_spellInfo, eff_idx, bonus, SPELL_DIRECT_DAMAGE);
        bonus = unitTarget->SpellDamageBonusTaken(m_casterUnit, m_spellInfo, eff_idx, bonus, SPELL_DIRECT_DAMAGE);
    }

    if (m_spellScript)
    {
        float scriptBonus = bonus;
        m_spellScript->OnEffectDamageCalculate(this, eff_idx, scriptBonus);
        bonus = int32(scriptBonus);
    }

    // prevent negative damage
    m_damage += uint32(bonus > 0 ? bonus : 0); 
}

void Spell::EffectThreat(SpellEffectIndex eff_idx)
{
    if (!unitTarget || !unitTarget->IsAlive() || !m_casterUnit || !m_casterUnit->IsAlive())
        return;

    if (!unitTarget->CanHaveThreatList())
        return;

    unitTarget->AddThreat(m_casterUnit, float(damage), false, m_spellInfo->GetSpellSchoolMask(), m_spellInfo);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectHealMaxHealth(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;
    uint32 heal = m_casterUnit ? m_casterUnit->GetMaxHealth() : unitTarget->GetMaxHealth();

    // Healing percent modifiers
    float  DoneTotalMod = 1.0f;
    float  TakenTotalMod = 1.0f;

    // Healing done percent
    if (m_casterUnit)
    {
        std::list <Aura*> const& mHealingDonePct = m_casterUnit->GetAurasByType(SPELL_AURA_MOD_HEALING_DONE_PERCENT);
        for (const auto i : mHealingDonePct)
            DoneTotalMod *= (100.0f + i->GetModifier()->m_amount) / 100.0f;
    }

    heal *= DoneTotalMod;

    // Healing taken percent
    float minval = float(unitTarget->GetMaxNegativeAuraModifier(SPELL_AURA_MOD_HEALING_PCT));
    if (minval)
        TakenTotalMod *= (100.0f + minval) / 100.0f;

    float maxval = float(unitTarget->GetMaxPositiveAuraModifier(SPELL_AURA_MOD_HEALING_PCT));
    if (maxval)
        TakenTotalMod *= (100.0f + maxval) / 100.0f;

    heal *= TakenTotalMod;

    m_healing += heal;
}

void Spell::EffectInterruptCast(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;
    if (!unitTarget->IsAlive())
        return;

    // TODO: not all spells that used this effect apply cooldown at school spells
    // also exist case: apply cooldown to interrupted cast only and to all spells
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
    {
        if (Spell* spell = unitTarget->GetCurrentSpell(CurrentSpellTypes(i)))
        {
            // Nostalrius: fix CS of instant spells (with CC Delay)
            if (i != CURRENT_CHANNELED_SPELL && !spell->GetCastTime())
                continue;

            SpellEntry const* curSpellInfo = spell->m_spellInfo;
            // check if we can interrupt spell
            if ((spell->getState() == SPELL_STATE_CASTING
                || (spell->getState() == SPELL_STATE_PREPARING && spell->GetCastTime() > 0.0f))
                && curSpellInfo->PreventionType == SPELL_PREVENTION_TYPE_SILENCE
                && ((i == CURRENT_GENERIC_SPELL && curSpellInfo->InterruptFlags & SPELL_INTERRUPT_FLAG_DAMAGE)
                || (i == CURRENT_CHANNELED_SPELL && curSpellInfo->ChannelInterruptFlags & CHANNEL_FLAG_INTERRUPT)))
            {
                unitTarget->ProhibitSpellSchool(curSpellInfo->GetSpellSchoolMask(), m_spellInfo->GetDuration());
                unitTarget->InterruptSpell(CurrentSpellTypes(i), false);

                ExecuteLogInfo info(unitTarget->GetObjectGuid());
                info.interruptCast.spellId = curSpellInfo->Id;
                AddExecuteLogInfo(eff_idx, info);
            }
        }
    }
}

void Spell::EffectSummonObjectWild(SpellEffectIndex eff_idx)
{
    // TODO: Objects summoned here should probably be _removed from the map_ once their
    // duration has expired, rather than simply made invisible
    uint32 gameobject_id = m_spellInfo->EffectMiscValue[eff_idx];

    GameObject* pGameObj = new GameObject;

    WorldObject* target = focusObject;
    if (!target)
        target = m_caster;

    float x, y, z, o;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
        o = target->GetOrientation();
    }
    else
    {
        m_caster->GetPosition(x, y, z);
        o = m_caster->GetOrientation();
    }

    Map *map = target->GetMap();

    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
                          x, y, z, o, 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    int32 duration = m_spellInfo->GetDuration();

    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    // Wild object not have owner and check clickable by players
    map->Add(pGameObj);

    if (pGameObj->GetGoType() == GAMEOBJECT_TYPE_FLAGDROP && m_caster->IsPlayer())
    {
        Player *pl = (Player*)m_caster;
        BattleGround* bg = ((Player *)m_caster)->GetBattleGround();

        switch (pGameObj->GetMapId())
        {
            case 489:                                       //WS
            {
                if (bg && bg->GetTypeID() == BATTLEGROUND_WS && bg->GetStatus() == STATUS_IN_PROGRESS)
                {
                    Team team = ALLIANCE;
                    if (pl->GetTeam() == team)
                        team = HORDE;

                    ((BattleGroundWS*)bg)->SetDroppedFlagGuid(pGameObj->GetObjectGuid(), team);
                }
                break;
            }
        }
    }

    pGameObj->SetWorldMask(m_caster->GetWorldMask());
    pGameObj->SummonLinkedTrapIfAny();

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(pGameObj);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(pGameObj->GetObjectGuid()));

    if (m_spellScript)
        m_spellScript->OnSummon(this, pGameObj);
}

void Spell::EffectScriptEffect(SpellEffectIndex eff_idx)
{
    // TODO: we must implement hunter pet summon at login there (spell 6962)
    switch (m_spellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
                case 8856:                                  // Bending Shinbone
                {
                    if (!itemTarget && m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch (urand(1, 5))
                    {
                        case 1:
                            spell_id = 8854;
                            break;
                        default:
                            spell_id = 8855;
                            break;
                    }

                    m_casterUnit->CastSpell(m_casterUnit, spell_id, true, nullptr);
                    return;
                }
                case 10101:                                 // Knock Away
                {
                    if (!unitTarget || !m_casterUnit)
                        return;

                    m_casterUnit->GetThreatManager().modifyThreatPercent(unitTarget, -100);

                    return;
                }
                case 17512:                                 // Piccolo of the Flaming Fire
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->HandleEmoteCommand(EMOTE_STATE_DANCE);

                    return;
                }
                case 22539:                                 // Shadow Flame (All script effects, not just end ones to
                case 22972:                                 // prevent player from dodging the last triggered spell)
                case 22975:
                case 22976:
                case 22977:
                case 22978:
                case 22979:
                case 22980:
                case 22981:
                case 22982:
                case 22983:
                case 22984:
                case 22985:
                {
                    if (!unitTarget || !unitTarget->IsAlive())
                        return;

                    // Onyxia Scale Cloak
                    if (unitTarget->HasAura(22683))
                        return;

                    // Shadow Flame
                    m_caster->CastSpell(unitTarget, 22682, true);
                    return;
                }
                case 24194:                                 // Uther's Tribute
                case 24195:                                 // Grom's Tribute
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint8 race = m_casterUnit->GetRace();
                    uint32 spellId = 0;

                    switch (m_spellInfo->Id)
                    {
                        case 24194:
                            switch (race)
                            {
                                case RACE_HUMAN:
                                case RACE_HIGH_ELF:
                                    spellId = 24105;
                                    break;
                                case RACE_DWARF:
                                    spellId = 24107;
                                    break;
                                case RACE_NIGHTELF:
                                    spellId = 24108;
                                    break;
                                case RACE_GNOME:
                                    spellId = 24106;
                                    break;
                            }
                            break;
                        case 24195:
                            switch (race)
                            {
                                case RACE_ORC:
                                case RACE_GOBLIN:
                                    spellId = 24104;
                                    break;
                                case RACE_UNDEAD:
                                    spellId = 24103;
                                    break;
                                case RACE_TAUREN:
                                    spellId = 24102;
                                    break;
                                case RACE_TROLL:
                                    spellId = 24101;
                                    break;
                            }
                            break;
                    }

                    if (spellId)
                        m_casterUnit->CastSpell(m_casterUnit, spellId, true);

                    return;
                }
                case 24320:                                 // Poisonous Blood
                {
                    unitTarget->CastSpell(unitTarget, 24321, true, nullptr, nullptr, m_caster->GetObjectGuid());
                    return;
                }
                case 24324: // Nostalrius : Ivina (Hakkar)
                {
                    enum
                    {
                        SPELL_BLOODSIPHON_STUN       = 24324,
                        SPELL_BLOODSIPHON_DAMAGE     = 24323,
                        SPELL_BLOODSIPHON_HEAL       = 24322,
                        SPELL_POISONOUS_BLOOD        = 24321
                    };

                    if (!unitTarget || !unitTarget->IsAlive() || !m_casterUnit)
                        return;

                    if (unitTarget->HasAura(SPELL_POISONOUS_BLOOD))
                        unitTarget->CastSpell(m_casterUnit, SPELL_BLOODSIPHON_DAMAGE, true);
                    else
                        unitTarget->CastSpell(m_casterUnit, SPELL_BLOODSIPHON_HEAL, true);
                    return;
                }
                case 24693:                                 // Hakkar Power Down - cast by priests on death
                {
                    if (!m_caster)
                        return;

                    if (Map* pMap = m_caster->GetMap())
                        if (InstanceData* pInstance = pMap->GetInstanceData())
                            pInstance->SetData(0, 0);

                    return;
                }
                case 24936:                                 // RC Tank Control
                {
                    if (Creature* pTonk = m_caster->FindNearestCreature(15328, 5, true))
                    {
                        pTonk->SetWalk(false);
                        pTonk->m_spells[0] = 24933;
                        pTonk->m_spells[1] = 25003;
                        pTonk->m_spells[2] = 27746;
                        switch (urand(0, 3))
                        {
                            case 0:
                                pTonk->m_spells[3] = 25026;
                                break;
                            case 1:
                                pTonk->m_spells[3] = 25027;
                                break;
                            case 2:
                                pTonk->m_spells[3] = 27759;
                                break;
                            case 3:
                                pTonk->m_spells[3] = 25024;
                                break;
                        }
                        if (m_originalCasterGUID.IsGameObject())
                        {
                            if (Player* player = pTonk->FindNearestPlayer(5))
                            {
                                if (player->HasAuraType(SPELL_AURA_MOD_ROOT) && !player->HasAura(24937))
                                    player->CastSpell(pTonk, 24937, true);
                            }
                        }
                        else
                        m_caster->CastSpell(pTonk, 24937, true);
                    }
                    return;
                }
                case 25671:                                 // Drain Mana
                case 25755:
                {
                    m_caster->CastSpell(unitTarget, 26639, true);
                    return;
                }
                case 25676: // Moam                         // Drain Mana
                case 26559: // Obsidian Nullifier
                {
                    m_caster->CastSpell(unitTarget, 25671, true);
                    return;
                }
                case 25754: // Obsidian Destroyer           // Drain Mana
                case 26457: // Obsidian Eradicator
                {
                    m_caster->CastSpell(unitTarget, 25755, true);
                    return;
                }
                case 26004:                                 // Mistletoe
                {
                    if (!unitTarget)
                        return;

                    unitTarget->HandleEmote(EMOTE_ONESHOT_CHEER);
                    return;
                }
                case 26137:                                 // Rotate Trigger
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, urand(0, 1) ? 26009 : 26136, true);
                    return;
                }
                case 26218:                                 // Mistletoe
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spells[2] = {26206, 26207};

                    m_caster->CastSpell(unitTarget, spells[urand(0, 1)], true);
                    return;
                }
                case 26275:                                 // PX-238 Winter Wondervolt TRAP
                {
                    if (!unitTarget || !m_caster->IsWithinDist(unitTarget, INTERACTION_DISTANCE))
                        return;

                    uint32 spells[4] = {26272, 26157, 26273, 26274};

                    // check presence
                    for (uint32 spell : spells)
                        if (unitTarget->HasAura(spell))
                            unitTarget->RemoveAurasDueToSpellByCancel(spell);

                    unitTarget->CastSpell(unitTarget, spells[urand(0, 3)], true);
                    return;
                }
                case 26656:                                 // Summon Black Qiraji Battle Tank
                {
                    if (!unitTarget)
                        return;

                    // Prevent stacking of mounts
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    const MapEntry* mapEntry = sMapStorage.LookupEntry<MapEntry>(unitTarget->GetMapId());
                    if (!mapEntry)
                        return;

                    // Two separate mount spells depending on if mounting is allowed or not
                    if (!mapEntry->IsMountAllowed())
                        unitTarget->CastSpell(unitTarget, 25863, true, m_CastItem);
                    else
                        unitTarget->CastSpell(unitTarget, 26655, true, m_CastItem);

                    return;
                }
                case 26923:                                 // Valentine (Guards)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (unitTarget->HasAura(26898)) // Heartbroken
                        return;

                    if (!((Player*)unitTarget)->HasItemCount(21815, 1, false)) // Love Token
                        return;

                    // Guard spellIds map [Pledge of Friendship , Pledge of Adoration]
                    std::map<uint32, std::vector<uint32>> loveAirSpellsMapForFaction = {
                            {11, {27242, 27510}},   // Stormwind
                            {85, {27247, 27507}},   // Orgrimmar
                            {57, {27244, 27506}},   // Ironforge
                            {68, {27246, 27515}},   // Undercity Guardian
                            {71, {27246, 27515}},   // Undercity Seeker
                            {79, {27245, 27504}},   // Darnassus
                            {105, {27248, 27513}}   // Thunderbluff
                    };

                    auto itr = loveAirSpellsMapForFaction.find(m_caster->GetFactionTemplateId());
                    if (itr == loveAirSpellsMapForFaction.end())
                        return;

                    if (itr->second.size() < 2)
                        return;

                    uint32 adorationOrFriendship = itr->second[0];
                    uint32 adoredOrBroken = 26680;      // Adored as default.

                    static ObjectGuidSet talkedToPlayers1;

                    // Sets 1 in 6 chance to cast Heartbroken.
                    if (!urand(0, 5) && !unitTarget->HasAura(26680) &&
                        talkedToPlayers1.find(unitTarget->GetObjectGuid()) != talkedToPlayers1.end())
                    {
                        adoredOrBroken = 26898;     // Heartbroken.
                    }
                    else if (!unitTarget->HasAura(26680))
                    {
                        adorationOrFriendship = itr->second[1];    // Pledge of Adoration for related faction.
                    }
                    else
                    {
                        adorationOrFriendship = itr->second[0];    // Pledge of Friendship for related faction.
                    }

                    talkedToPlayers1.insert(unitTarget->GetObjectGuid());

                    if (!unitTarget->HasAura(adoredOrBroken))
                        unitTarget->CastSpell(unitTarget, adoredOrBroken, false);           // Cast Adored or Broken.

                    if (adoredOrBroken == 26898)
                        return;

                    unitTarget->CastSpell(unitTarget, adorationOrFriendship, true);     // Get a Pledge.
                    unitTarget->CastSpell(unitTarget, 26879, true);                     // Remove Amorous.

                    return;
                }
                case 26663:                     // Valentine (Citizens)
                case 27541:
                case 27547:
                case 27548:
                case 27549:
                case 27550:
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Civilian spellIds map [Gift of Friendship , Gift of Adoration]
                    std::map<uint32, std::vector<uint32>> loveAirSpellsMapForFaction = {
                            {12, {27525, 27509}},   // Stormwind
                            {29, {27523, 27505}},   // Orgrimmar orcs
                            {55, {27520, 27503}},   // Ironforge dwarves
                            {68, {27529, 27512}},   // Undercity
                            {80, {27519, 26901}},   // Darnassus
                            {104, {27524, 27511}},  // Thunderbluff
                            {126, {27523, 27505}},  // Orgrimmar trolls
                            {875, {27520, 27503}}   // Ironforge gnomes
                    };

                    auto itr = loveAirSpellsMapForFaction.find(m_caster->GetFactionTemplateId());
                    if (itr == loveAirSpellsMapForFaction.end())
                        return;

                    if (itr->second.size() < 2)
                        return;

                    uint32 adorationOrFriendship = itr->second[0];
                    uint32 adoredOrBroken = 26680;      // Adored as default.

                    static ObjectGuidSet talkedToPlayers2;

                    // Sets 1 in 6 chance to cast Heartbroken.
                    if (!urand(0, 5) && !unitTarget->HasAura(26680) &&
                        talkedToPlayers2.find(unitTarget->GetObjectGuid()) != talkedToPlayers2.end())
                    {
                        adoredOrBroken = 26898;     // Heartbroken.
                    }
                    else if (!unitTarget->HasAura(26680))
                    {
                        adorationOrFriendship = itr->second[1];    // Gift of Adoration for related faction
                    }
                    else
                    {
                        adorationOrFriendship = itr->second[0];    // Gift of Friendship for related faction
                    }

                    talkedToPlayers2.insert(unitTarget->GetObjectGuid());

                    if (!unitTarget->HasAura(adoredOrBroken))
                        unitTarget->CastSpell(unitTarget, adoredOrBroken, false);           // Cast Adored or Broken.

                    if (adoredOrBroken == 26898)
                        return;

                    unitTarget->CastSpell(unitTarget, adorationOrFriendship, true);     // Get a Pledge.
                    unitTarget->CastSpell(unitTarget, 26879, true);                     // Remove Amorous.

                    return;
                }
                case 27654:                         // Love is in the Air Test
                case 26870:                         // Amorous Timer, Standard Test
                {
                    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                    {
                        unitTarget->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP); // Add gossip flag for NPC missing it
                        unitTarget->CastSpell(unitTarget, m_spellInfo->Id == 26870 ? 26869 : 27741, true); // Apply aura instantly
                    }
                    return;
                }
                case 27687:                                 // Summon Bone Minions
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 27690, true);
                    unitTarget->CastSpell(unitTarget, 27691, true);
                    unitTarget->CastSpell(unitTarget, 27692, true);
                    unitTarget->CastSpell(unitTarget, 27693, true);
                    return;
                }
                case 27695:                                 // Summon Bone Mages
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 27696, true);
                    unitTarget->CastSpell(unitTarget, 27697, true);
                    unitTarget->CastSpell(unitTarget, 27698, true);
                    unitTarget->CastSpell(unitTarget, 27699, true);
                    return;
                }
                case 28374:                                 // Decimate (Naxxramas: Gluth)
                {
                //implemented in SpellEffects::EffectSchoolDMG  instead, under case 28375
                /*
                    if (!unitTarget)
                        return;
                    // Should only affect players and zombies
                    if (unitTarget->IsPlayer() || unitTarget->GetEntry() == 16360)
                    {
                        int32 damage = unitTarget->GetHealth() - unitTarget->GetMaxHealth() * 0.05f;
                        if (damage > 0)
                            m_caster->CastCustomSpell(unitTarget, 28375, &damage, nullptr, nullptr, true);
                    }
                */
                    return;
                }
                case 28408:                                 // Chains of Kel'Thuzad (Naxxramas: Kel'thuzad)
                {
                    if (!m_casterUnit)
                        return;

                    // Select maintank + 4 random targets
                    std::vector<Unit*> viableTargets;
                    const ThreatList& tl = m_casterUnit->GetThreatManager().getThreatList();
                    for (const auto it : tl)
                    {
                        if (it->getUnitGuid().IsPlayer())
                        {
                            if (Unit* pUnit = m_casterUnit->GetMap()->GetUnit(it->getUnitGuid()))
                            {
                                if (pUnit->IsAlive())
                                    viableTargets.push_back(pUnit);
                            }
                        }
                    }

                    int num_targets = std::min(int(viableTargets.size()), 5)-1; // leaving 1 target not MCed to avoid reset due to all MCed

                    // always MC maintank
                    if (Unit* maintank = m_casterUnit->GetVictim())
                    {
                        auto it = std::find(viableTargets.begin(), viableTargets.end(), maintank);
                        if (it != viableTargets.end())
                            viableTargets.erase(it);
                        num_targets -= 1;

                        maintank->RemoveAurasDueToSpell(27819); // remove Detonate Mana aura
                        maintank->CastSpell(maintank, 28409, true); // modifies scale
                        m_casterUnit->CastSpell(maintank, 28410, true); // applies dmg and healing mod, as well as the charm itself
                    }

                    for (int i = 0; i < num_targets; i++)
                    {
                        int rand = irand(0, viableTargets.size() - 1);
                        Unit* target = viableTargets[rand];
                        viableTargets.erase(viableTargets.begin() + rand);

                        target->RemoveAurasDueToSpell(27819); // remove Detonate Mana aura
                        target->CastSpell(target, 28409, true); // modifies scale
                        m_casterUnit->CastSpell(target, 28410, true); // applies dmg and healing mod, as well as the charm itself
                    }
                    return;
                }
                case 28560:                                 // Summon Blizzard
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 28561, true, nullptr, nullptr, m_caster->GetObjectGuid());
                    return;
                }
                case 30918:                                 // Improved Sprint
                {
                    if (!unitTarget)
                        return;

                    // Removes snares and roots.
                    unitTarget->RemoveAurasAtMechanicImmunity(IMMUNE_TO_ROOT_AND_SNARE_MASK, 30918, true);
                    break;
                }
                case 29182:                                 // Teleport to Twin Emperors
                {
                    if (unitTarget && unitTarget->IsPlayer())
                        unitTarget->CastSpell(unitTarget, 29181, true);
                    return;
                }
                case 29188:                                 // Teleport to Final Chamber
                {
                    if (unitTarget && unitTarget->IsPlayer())
                        unitTarget->CastSpell(unitTarget, 29190, true);
                    return;
                }
                case 30132:                                 // Despawn Ice Block
                {
                    if (unitTarget && unitTarget->IsPlayer())
                    {
                        unitTarget->RemoveAurasDueToSpell(31800); // Icebolt immunity spell
                        unitTarget->RemoveAurasDueToSpell(28522); // Icebolt stun/damage spell
                    }
                    return;
                }
                case 28352:                                    // Atiesh - Breath of Sargeras
                {
                    if (unitTarget && m_caster)
                        m_caster->CastSpell(unitTarget, 28342, true);
                    return;
                }
                case 26532:                                 // Winter Veil summons
                case 26541:
                case 26469:
                case 26528:
                {
                    if (Player* player = m_caster->ToPlayer())
                    {
                        /* Set which npc entry should we check later to see if we should despawn the pet
                           or just summon a new one */
                        uint32 entryToCheck;
                        switch (m_spellInfo->Id) {
                            case 26532: // Green Helper
                                entryToCheck = 15698;
                                break;
                            case 26541: // Red Helper
                                entryToCheck = 15705;
                                break;
                            case 26469: // Tiny Snowman
                                entryToCheck = 15710;
                                break;
                            case 26528: // Winter Reindeer
                                entryToCheck = 15706;
                                break;
                        }
                        // Remove minipet without consuming a snowball (only if it's the same pet)
                        if (player->GetMiniPet() && player->GetMiniPet()->GetEntry() == entryToCheck)
                        {
                            player->RemoveMiniPet();
                            return;
                        }
                    }
                    break;
                }
                case 26678:                                 // Bag of Candies
                {
                    uint32 candySpells[8] = { 26668, 26670, 26671, 26672, 26673, 26674, 26675, 26676 };
                    if (m_casterUnit)
                        m_casterUnit->CastSpell(m_casterUnit, candySpells[urand(0, 7)], true);
                    return;
                }
                case 27657:                                 // Valentine End Check
                {
                    if (m_casterUnit && !sGameEventMgr.IsActiveEvent(8))
                    {
                        m_casterUnit->RemoveAurasDueToSpell(26870); // Amorous Timer, Standard Test
                        m_casterUnit->RemoveAurasDueToSpell(27742); // Amorous Timer, Standard
                        m_casterUnit->RemoveAurasDueToSpell(26869); // Amorous
                        m_casterUnit->RemoveAurasDueToSpell(27654); // Love is in the Air Test
                        m_casterUnit->RemoveAurasDueToSpell(27741); // Love is in the Air
                    }
                    return;
                }
                case 27686:                                 // Teleport to Razelikh (GROUP)
                {
                    if (Player* pPlayerTarget = ToPlayer(unitTarget))
                    {
                        if (Group* pGroup = pPlayerTarget->GetGroup())
                        {
                            for (GroupReference* itr = pGroup->GetFirstMember(); itr != nullptr; itr = itr->next())
                            {
                                Player* pGroupMember = itr->getSource();
                                if (pGroupMember && pGroupMember->IsWithinDistInMap(m_caster, 20.0f))
                                    pGroupMember->CastSpell(pGroupMember, 13142, true); // Teleport to Razelikh
                            }
                        }
                    }
                    return;
                }
                case 28732: // Naxxramas Worshipper - Widow's Embrace
                {
                    if (m_casterUnit && m_casterUnit->IsAlive())
                        m_casterUnit->Kill(m_casterUnit, nullptr);
                    return;
                }
                case 29710: // Test Ribbon Pole Channel Trigger
                {
                    if (Player* pPlayerTarget = ToPlayer(unitTarget))
                    {
                        pPlayerTarget->CastSpell(pPlayerTarget, PickRandomValue(29705, 29726, 29727), false);
                    }
                    return;
                }
                case 30882: // EPL PvP A Game of Towers: Tower Capture Test (DND)
                {
                    std::list<Player*> players;
                    m_casterUnit->GetAlivePlayerListInRange(m_casterUnit, players, VISIBILITY_DISTANCE_NORMAL);
                    for (const auto& pTarget : players)
                    {
                        if (!pTarget->IsFriendlyTo(m_casterUnit))
                            continue;

                        if (!pTarget->IsOutdoorPvPActive())
                            continue;

                        pTarget->CastSpell(pTarget, 31929, true);
                    }
                    return;
                }
                case 31929: // EPL PvP A Game of Towers: Tower Capture (DND)
                {
                    if (Player* pTarget = ToPlayer(unitTarget))
                        pTarget->CastSpell(pTarget, 32061, true);

                    return;
                }
            }
            break;
        }
    }

    // normal DB scripted effect
    if (!unitTarget)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell ScriptStart spellid %u in EffectScriptEffect ", m_spellInfo->Id);
    m_caster->GetMap()->ScriptsStart(sSpellScripts, m_spellInfo->Id, m_caster->GetObjectGuid(), unitTarget->GetObjectGuid());
}

void Spell::EffectSanctuary(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    unitTarget->InterruptSpellsCastedOnMe(true);
    unitTarget->InterruptAttacksOnMe(0.0f, false);
    unitTarget->m_lastSanctuaryTime = WorldTimer::getMSTime();

    // Flask of Petrification does not cause mobs to stop attacking.
    // Aspect of Arlokk should not cause Hakkar to evade.
    unitTarget->DoResetThreat();

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectAddComboPoints(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget)
        return;

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (damage <= 0)
        return;

    ((Player*)m_caster)->AddComboPoints(unitTarget, damage);
    ((Player*)m_caster)->SetUInt64Value(PLAYER_FIELD_COMBO_TARGET, unitTarget->GetGUID());
}

void Spell::EffectDuel(SpellEffectIndex eff_idx)
{
    if (!m_casterUnit || !unitTarget || !m_casterUnit->IsPlayer() || !unitTarget->IsPlayer())
        return;

    Player *caster = (Player*)m_casterUnit;
    Player *target = (Player*)unitTarget;

    // if the caster is already in a m_duel or has issued a challenge
    if (caster->m_duel && caster->m_duel->opponent != target->GetObjectGuid())
    {
        if (caster->m_duel->startTime)
            caster->DuelComplete(DUEL_WON);
        else
            caster->DuelComplete(DUEL_INTERRUPTED);

       delete caster->m_duel;
       delete target->m_duel;
       caster->m_duel = target->m_duel = nullptr;
    }

    // if the caster attempts to m_duel somebody they're already in a m_duel with
    if (caster->m_duel && caster->m_duel->opponent == target->GetObjectGuid() && caster->m_duel->startTime)
    {
        SendCastResult(SPELL_FAILED_TARGET_ENEMY);
        return;
    }

    // if the target already has a pending m_duel/is dueling, reject the request
    if (target->m_duel)
    {
        SendCastResult(SPELL_FAILED_TARGET_DUELING);
        return;
    }

    // caster or target already have requested m_duel
    if (caster->m_duel || !target->GetSocial() || target->GetSocial()->HasIgnore(caster->GetObjectGuid()) || target->FindMap() != caster->FindMap())
        return;

    // Players can only fight a m_duel with each other outside (=not inside dungeons and not in capital cities)
    const auto *casterAreaEntry = AreaEntry::GetById(caster->GetAreaId());
    if (casterAreaEntry && !(casterAreaEntry->Flags & AREA_FLAG_DUEL))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    const auto *targetAreaEntry = AreaEntry::GetById(target->GetAreaId());
    if (targetAreaEntry && !(targetAreaEntry->Flags & AREA_FLAG_DUEL))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    // Disable dueling in arena. The areas we use aren't official maps and lack the AREA_FLAG_DUEL on them.
    if (caster->InArena() || target->InArena())
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    //CREATE DUEL FLAG OBJECT
    GameObject* pGameObj = new GameObject;

    uint32 gameobject_id = m_spellInfo->EffectMiscValue[eff_idx];

    Map *map = m_casterUnit->GetMap();
    float x = (m_casterUnit->GetPositionX() + unitTarget->GetPositionX()) * 0.5f;
    float y = (m_casterUnit->GetPositionY() + unitTarget->GetPositionY()) * 0.5f;
    float z = m_casterUnit->GetPositionZ();

    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map, x, y, z,
                          m_casterUnit->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_FACTION, m_casterUnit->GetFactionTemplateId());
    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_casterUnit->GetLevel() + 1);
    int32 duration = m_spellInfo->GetDuration();
    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    m_casterUnit->AddGameObject(pGameObj);
    map->Add(pGameObj);
    //END

    // Send request
    WorldPacket data(SMSG_DUEL_REQUESTED, 8 + 8);
    data << pGameObj->GetObjectGuid();
    data << caster->GetObjectGuid();
    caster->GetSession()->SendPacket(&data);
    target->GetSession()->SendPacket(&data);

    // create m_duel-info
    DuelInfo *m_duel   = new DuelInfo;
    m_duel->initiator  = caster;
    m_duel->opponent   = target->GetObjectGuid();
    m_duel->startTime  = 0;
    m_duel->startTimer = 0;

    DuelInfo *duel2   = new DuelInfo;
    duel2->initiator  = caster;
    duel2->opponent   = caster->GetObjectGuid();
    duel2->startTime  = 0;
    duel2->startTimer = 0;

    if (Transport* t = caster->GetTransport())
    {
        m_duel->transportGuid  = t->GetGUIDLow();
        duel2->transportGuid = t->GetGUIDLow();
    }
    caster->m_duel     = m_duel;
    target->m_duel      = duel2;

    caster->SetGuidValue(PLAYER_DUEL_ARBITER, pGameObj->GetObjectGuid());
    target->SetGuidValue(PLAYER_DUEL_ARBITER, pGameObj->GetObjectGuid());

    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_DUEL_REQUEST, [&](PlayerScript* script)
    {
        script->OnDuelRequest(target, caster);
    });
}

void Spell::EffectStuck(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!sWorld.getConfig(CONFIG_BOOL_CAST_UNSTUCK))
        return;

    Player* pTarget = (Player*)unitTarget;

    DEBUG_LOG("Spell Effect: Stuck");
    sLog.outInfo("Player %s (guid %u) used auto-unstuck feature at map %u (%f, %f, %f).", pTarget->GetName(), pTarget->GetGUIDLow(), m_caster->GetMapId(), m_caster->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());

    if (pTarget->IsTaxiFlying())
        return;

    // TP to last overmap position
    if (fabs(pTarget->m_lastSafePosition.x) > 0.1f && fabs(pTarget->m_lastSafePosition.y) > 0.1f)
        pTarget->TeleportTo(pTarget->GetMapId(), pTarget->m_lastSafePosition.x, pTarget->m_lastSafePosition.y, pTarget->m_lastSafePosition.z - 2.0f + 0.7f, pTarget->m_lastSafePosition.o);
}

void Spell::EffectSummonPlayer(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // https://github.com/vmangos/core/pull/1945/commits/32f853d8874fc82590a0dfa394cc03d1c9ed61b2
    //// Evil Twin (ignore player summon, but hide this for summoner)
    //if (unitTarget->HasAura(23445))
    //    return;

    float x, y, z;
    WorldObject* landingObject = m_caster;
    // summon to the ritual go location if any
    if (GameObject* pGo = m_targets.getGOTarget())
        if (pGo->GetGoType() == GAMEOBJECT_TYPE_SUMMONING_RITUAL)
            landingObject = pGo;

    landingObject->GetClosePoint(x, y, z, unitTarget->GetObjectBoundingRadius());
    ((Player*)unitTarget)->SendSummonRequest(m_caster->GetObjectGuid(), m_caster->GetMapId(), m_caster->GetZoneId(), x, y, z);
}

void Spell::EffectActivateObject(SpellEffectIndex eff_idx)
{
    if (!gameObjTarget)
        return;

    GameObjectActions action = (GameObjectActions)m_spellInfo->EffectMiscValue[eff_idx];

    // Can be handled by script.
    if (gameObjTarget->AI() && gameObjTarget->AI()->OnActivateBySpell(m_caster, m_spellInfo->Id, (uint32)action))
        return;

    switch (action)
    {
        case GameObjectActions::None:
            sLog.outError("Spell::EffectActivateObject: Incorrect GameObjectActions::None action in spell %u", m_spellInfo->Id);
            break;
        case GameObjectActions::AnimateCustom0:
            gameObjTarget->SendGameObjectCustomAnim(0);
            break;
        case GameObjectActions::AnimateCustom1:
            gameObjTarget->SendGameObjectCustomAnim(1);
            break;
        case GameObjectActions::AnimateCustom2:
            gameObjTarget->SendGameObjectCustomAnim(2);
            break;
        case GameObjectActions::AnimateCustom3:
            gameObjTarget->SendGameObjectCustomAnim(3);
            break;
        case GameObjectActions::Disturb:
            if (m_casterUnit)
                gameObjTarget->Use(m_casterUnit);
            break;
        case GameObjectActions::Unlock:
            gameObjTarget->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            break;
        case GameObjectActions::Lock:
            gameObjTarget->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            break;
        case GameObjectActions::Open:
            if (m_casterUnit)
                gameObjTarget->Use(m_casterUnit);
            /* Seems like Open can also trigger traps, cause linked trap to trigger, etc... go figure...
            gameObjTarget->UseDoorOrButton(0, false, m_caster);
            */
            break;
        case GameObjectActions::OpenAndUnlock:
            gameObjTarget->UseDoorOrButton(0, false);
            gameObjTarget->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            break;
        case GameObjectActions::Close:
            gameObjTarget->ResetDoorOrButton();
            break;
        case GameObjectActions::ToggleOpen:
            // No use cases, implementation unknown
            break;
        case GameObjectActions::Destroy:
            gameObjTarget->UseDoorOrButton(0, true);
            break;
        case GameObjectActions::Rebuild:
            gameObjTarget->ResetDoorOrButton();
            break;
        case GameObjectActions::Creation:
            // No use cases, implementation unknown
            break;
        case GameObjectActions::Despawn:
            gameObjTarget->Despawn();
            gameObjTarget->SetLootState(GO_JUST_DEACTIVATED);
            break;
        case GameObjectActions::MakeInert:
            gameObjTarget->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GameObjectActions::MakeActive:
            gameObjTarget->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GameObjectActions::CloseAndLock:
            gameObjTarget->ResetDoorOrButton();
            gameObjTarget->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            break;
        case GameObjectActions::UseArtKit0:
            // Source for values unknown
            /*
            switch (m_spellInfo->Id)
            {
                case 46904: // Light Bonfire (Art Kit)
                    gameObjTarget->SetGoArtKit(121);
                    break;
            }
            */
            break;
        case GameObjectActions::UseArtKit1:
            // Source for values unknown
            /*
            switch (m_spellInfo->Id)
            {
                case 36639: // Test Cauldron Bubble
                    gameObjTarget->SetGoArtKit(81);
                    break;
                case 46903: // Stamp Out Bonfire (Art Kit)
                    gameObjTarget->SetGoArtKit(122);
                    break;
            }
            */
            break;
        case GameObjectActions::UseArtKit2:
            // No use cases, implementation unknown
            break;
        case GameObjectActions::UseArtKit3:
            // No use cases, implementation unknown
            break;
        case GameObjectActions::SetTapList:
            // No use cases, implementation unknown
            break;
        default:
            sLog.outError("Spell::EffectActivateObject: Unhandled GameObjectActions action %u in spell %u", (uint32)action, m_spellInfo->Id);
            break;
    }
}

void Spell::EffectSummonTotem(SpellEffectIndex eff_idx)
{
    if (!m_casterUnit)
        return;

    int slot = 0;
    switch (m_spellInfo->Effect[eff_idx])
    {
        case SPELL_EFFECT_SUMMON_TOTEM:
            slot = TOTEM_SLOT_NONE;
            break;
        case SPELL_EFFECT_SUMMON_TOTEM_SLOT1:
            slot = TOTEM_SLOT_FIRE;
            break;
        case SPELL_EFFECT_SUMMON_TOTEM_SLOT2:
            slot = TOTEM_SLOT_EARTH;
            break;
        case SPELL_EFFECT_SUMMON_TOTEM_SLOT3:
            slot = TOTEM_SLOT_WATER;
            break;
        case SPELL_EFFECT_SUMMON_TOTEM_SLOT4:
            slot = TOTEM_SLOT_AIR;
            break;
        default:
            return;
    }

    // unsummon old totem
    if (slot < MAX_TOTEM_SLOT)
        if (Totem *OldTotem = m_casterUnit->GetTotem(TotemSlot(slot)))
            OldTotem->UnSummon();

    // FIXME: Setup near to finish point because GetObjectBoundingRadius set in Create but some Create calls can be dependent from proper position
    // if totem have creature_template_addon.auras with persistent point for example or script call
    float angle = slot < MAX_TOTEM_SLOT ? M_PI_F / MAX_TOTEM_SLOT - (slot * 2 * M_PI_F / MAX_TOTEM_SLOT) : 0;

    CreatureCreatePos pos(m_casterUnit, m_casterUnit->GetOrientation(), 2.0f, angle);

    CreatureInfo const *cinfo = sObjectMgr.GetCreatureTemplate(m_spellInfo->EffectMiscValue[eff_idx]);
    if (!cinfo)
    {
        sLog.outErrorDb("Creature entry %u does not exist but used in spell %u totem summon.", m_spellInfo->EffectMiscValue[eff_idx], m_spellInfo->Id);
        return;
    }

    Totem* pTotem = new Totem;

    if (!pTotem->Create(m_casterUnit->GetMap()->GenerateLocalLowGuid(HIGHGUID_UNIT), pos, cinfo, m_casterUnit))
    {
        delete pTotem;
        return;
    }

    pTotem->SetSummonPoint(pos);

    if (slot < MAX_TOTEM_SLOT)
        m_casterUnit->_AddTotem(TotemSlot(slot), pTotem);

    //pTotem->SetName("");                                  // generated by client
    pTotem->SetOwner(m_casterUnit);
    pTotem->SetTypeBySummonSpell(m_spellInfo);              // must be after Create call where m_spells initialized

    pTotem->SetDuration(m_duration);

    if (damage)                                             // if not spell info, DB values used
    {
        pTotem->SetMaxHealth(damage);
        pTotem->SetHealth(damage);
    }

    pTotem->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    if (m_casterUnit->GetTypeId() == TYPEID_PLAYER)
        pTotem->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

    if (m_casterUnit->IsPvP())
        pTotem->SetPvP(true);

    pTotem->Summon(m_casterUnit);

    Unit::AuraList const& triggerAuraOnTotemAuras = m_casterUnit->GetAurasByType(SPELL_AURA_TRIGGER_AURA_ON_TOTEMS);
    for (const auto aura : triggerAuraOnTotemAuras)
        if (uint32 spellId = aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()])
            pTotem->AddAura(spellId, ADD_AURA_PASSIVE, m_casterUnit);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(pTotem->GetObjectGuid()));

    if (m_spellScript)
        m_spellScript->OnSummon(this, pTotem);
}

void Spell::EffectEnchantHeldItem(SpellEffectIndex eff_idx)
{
    // this is only item spell effect applied to main-hand weapon of target player (players in area)
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* item_owner = (Player*)unitTarget;
    Item* item = item_owner->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

    if (!item)
        return;

    // must be equipped
    if (!item ->IsEquipped())
        return;

    if (item->CanBeTradedEvenIfSoulBound())
        return;

    // Nostalrius (INTERFACTION) : Totem furie-des-vents ecrase les benes de puissance et des rois Paladin.
    /*if (m_spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN)
    {
        Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_MOD_ATTACK_POWER);
        for (const auto aura : auras)
        {
            if (aura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_PALADIN &&
                    // Bene de puissance inf et sup
                    (aura->GetSpellProto()->SpellIconID == 298 || aura->GetSpellProto()->SpellIconID == 1802))
            {
                unitTarget->RemoveAurasDueToSpell(aura->GetSpellProto()->Id);
                break; // Normalement on ne peut avoir qu'une seule bene de ce type.
            }
        }
        Unit::AuraList const& auras2 = unitTarget->GetAurasByType(SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE);
        for (const auto aura : auras2)
        {
            if (aura->GetSpellProto()->SpellFamilyName == SPELLFAMILY_PALADIN &&
                    // Bene des rois inf et sup
                    (aura->GetSpellProto()->Id == 20217 || aura->GetSpellProto()->Id == 25898))
            {
                unitTarget->RemoveAurasDueToSpell(aura->GetSpellProto()->Id);
                break; // Normalement on ne peut avoir qu'une seule bene de ce type.
            }
        }
    }
    */

    if (m_triggeredByAuraSpell && m_caster->IsCreature())
    {
        switch (m_triggeredByAuraSpell->Id)
        {
            // Flametongue Totem Passive
            case 8229: // Rank 1
            case 8251: // Rank 2
            case 10524: // Rank 3
            case 16388: // Rank 4
            // Windfury Totem Passive
            case 8515: // Rank 1
            case 10609: // Rank 2
            case 10612: // Rank 3
            {
                if (Unit* pOwner = m_casterUnit->GetOwner())
                {
                    if (pOwner != unitTarget)
                    {
                        auto AddTemporaryAura = [](Unit* pTarget, uint32 spellId)
                        {
                            if (SpellAuraHolder* holder = pTarget->GetSpellAuraHolder(spellId))
                            {
                                if (!holder->IsPermanent())
                                    holder->SetAuraDuration(10000);
                            }
                            else if (SpellAuraHolder* pAura = pTarget->AddAura(spellId))
                            {
                                pAura->SetPassive(false);
                                pAura->SetPermanent(false);
                                pAura->SetAuraMaxDuration(10000);
                                pAura->SetAuraDuration(10000);
                            }
                        };

                        if (pOwner->HasAura(29192))
                            AddTemporaryAura(unitTarget, 29192);
                        else if (pOwner->HasAura(29193))
                            AddTemporaryAura(unitTarget, 29193);
                    }
                }
                break;
            }
        }
    }

    if (m_spellInfo->EffectMiscValue[eff_idx])
    {
        uint32 enchant_id = m_spellInfo->EffectMiscValue[eff_idx];

        // Base points first
        int32 duration = m_currentBasePoints[eff_idx] * IN_MILLISECONDS;

        // Try duration index next
        if (!duration)
            duration = m_spellInfo->GetDuration();

        // 10 seconds for enchants which don't have listed duration
        if (!duration)
            duration = 10 * IN_MILLISECONDS;

        SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if (!pEnchant)
            return;

        uint32 charges = sSpellMgr.GetSpellEnchantCharges(m_spellInfo->Id);

        // Always go to temp enchantment slot
        EnchantmentSlot slot = TEMP_ENCHANTMENT_SLOT;

        // Enchantment will not be applied if a different one already exists
        if (item->GetEnchantmentId(slot) && item->GetEnchantmentId(slot) != enchant_id)
            return;

        // Apply the temporary enchantment
        item->SetEnchantment(slot, enchant_id, duration, charges);
        item_owner->ApplyEnchantment(item, slot, true);

        // do not allow trading raid items after enchanting them
        if (item->CanBeTradedEvenIfSoulBound())
            item->ResetSoulBoundTradeData();
    }
}

void Spell::EffectDisEnchant(SpellEffectIndex /*eff_idx*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!itemTarget || !itemTarget->GetProto()->DisenchantID)
        return;

    Player* pCaster = static_cast<Player*>(m_caster);

    itemTarget->SetBinding(true);
    pCaster->UpdateCraftSkill(m_spellInfo->Id);
    pCaster->SendLoot(itemTarget->GetObjectGuid(), LOOT_DISENCHANTING);

    // item will be removed at disenchanting end
}

void Spell::EffectInebriate(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)unitTarget;
    uint16 currentDrunk = player->GetDrunkValue();
    uint16 drunkMod = damage * 256;
    if (currentDrunk + drunkMod > 0xFFFF)
        currentDrunk = 0xFFFF;
    else
        currentDrunk += drunkMod;
    player->SetDrunkValue(currentDrunk, m_CastItem ? m_CastItem->GetEntry() : 0);

    // Turtle WoW Winter Veil quests:  
    if (player->ToPlayer()->GetQuestStatus(50328) == QUEST_STATUS_INCOMPLETE) // The Winter Veil Gourmet
    {
            int32 dummy_player{ 70010 }; // DUMMY_PLAYER_YOURSELF 
            CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(dummy_player);

            if (cInfo != nullptr)
                player->ToPlayer()->KilledMonster(cInfo, ObjectGuid());
    }
}

void Spell::EffectFeedPet(SpellEffectIndex eff_idx)
{
    Player* pPlayer = m_caster->ToPlayer();
    if (!pPlayer)
        return;

    Item* foodItem = itemTarget;
    if (!foodItem)
        return;

    Pet *pet = pPlayer->GetPet();
    if (!pet)
        return;

    if (!pet->IsAlive())
        return;

    if (!m_spellInfo->IsTargetInRange(pPlayer, pet))
    {
        SendCastResult(SPELL_FAILED_OUT_OF_RANGE);
        return;
    }

    if (!pet->IsWithinLOSInMap(pPlayer))
    {
        SendCastResult(SPELL_FAILED_LINE_OF_SIGHT);
        return;
    }

    int32 benefit = pet->GetCurrentFoodBenefitLevel(foodItem->GetProto()->ItemLevel);
    if (benefit <= 0)
        return;

    ExecuteLogInfo info;
    info.feedPet.itemEntry = foodItem->GetProto()->ItemId;

    uint32 count = 1;
    pPlayer->DestroyItemCount(foodItem, count, true);
    pPlayer->CastCustomSpell(pPlayer, m_spellInfo->EffectTriggerSpell[eff_idx], &benefit, nullptr, nullptr, true);

    AddExecuteLogInfo(eff_idx, info);
}

void Spell::EffectDismissPet(SpellEffectIndex eff_idx)
{
    Player* pPlayer = m_caster->ToPlayer();
    if (!pPlayer)
        return;

    Pet* pet = pPlayer->GetPet();

    // not let dismiss dead pet
    if (!pet || !pet->IsAlive())
        return;

    pet->Unsummon(PET_SAVE_NOT_IN_SLOT, pPlayer);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(pet->GetObjectGuid()));
}

void Spell::EffectSummonObject(SpellEffectIndex eff_idx)
{
    if (!m_casterUnit)
        return;

    uint32 go_id = m_spellInfo->EffectMiscValue[eff_idx];

    uint8 slot = 0;
    switch (m_spellInfo->Effect[eff_idx])
    {
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT1:
            slot = 0;
            break;
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT2:
            slot = 1;
            break;
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT3:
            slot = 2;
            break;
        case SPELL_EFFECT_SUMMON_OBJECT_SLOT4:
            slot = 3;
            break;
        default:
            return;
    }

    if (ObjectGuid guid = m_casterUnit->m_ObjectSlotGuid[slot])
    {
        if (GameObject* obj = m_casterUnit ? m_casterUnit->GetMap()->GetGameObject(guid) : nullptr)
            obj->SetLootState(GO_JUST_DEACTIVATED);

        m_casterUnit->m_ObjectSlotGuid[slot].Clear();
    }

    GameObject* pGameObj = new GameObject;

    float x, y, z;
    // If dest location if present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
    }
    // Summon in random point all other units if location present
    else
        m_casterUnit->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    Map *map = m_casterUnit->GetMap();
    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), go_id, map,
                          x, y, z, m_casterUnit->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_casterUnit->GetLevel());
    int32 duration = m_spellInfo->GetDuration();
    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);
    m_casterUnit->AddGameObject(pGameObj);

    map->Add(pGameObj);
    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM, 8);
    data << ObjectGuid(pGameObj->GetObjectGuid());
    m_casterUnit->SendMessageToSet(&data, true);

    m_casterUnit->m_ObjectSlotGuid[slot] = pGameObj->GetObjectGuid();

    pGameObj->SummonLinkedTrapIfAny();

    if (m_casterUnit->GetTypeId() == TYPEID_UNIT && ((Creature*)m_casterUnit)->AI())
        ((Creature*)m_casterUnit)->AI()->JustSummoned(pGameObj);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(pGameObj->GetObjectGuid()));

    if (m_spellScript)
        m_spellScript->OnSummon(this, pGameObj);
}

void Spell::EffectResurrect(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (unitTarget->IsAlive())
        return;
    if (!unitTarget->IsInWorld())
        return;

    Player* pTarget = ((Player*)unitTarget);

    if (pTarget->IsRessurectRequested())      // already have one active request
        return;

    uint32 health = pTarget->GetMaxHealth() * damage / 100;
    uint32 mana   = pTarget->GetMaxPower(POWER_MANA) * damage / 100;

    pTarget->SetResurrectRequestData(m_caster->GetObjectGuid(), m_caster->GetMapId(), m_caster->GetInstanceId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), m_caster->GetOrientation(), health, mana);
    SendResurrectRequest(pTarget, m_casterUnit && m_casterUnit->IsSpiritHealer());

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectAddExtraAttacks(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    if (!unitTarget->IsAlive()|| unitTarget->IsExtraAttacksLocked())
        return;

    uint32 const attacks = std::min(5u, uint32(damage) + unitTarget->GetExtraAttacks());
    unitTarget->AddExtraAttackOnUpdate();
    unitTarget->SetExtraAttaks(attacks);

    ExecuteLogInfo info(unitTarget->GetObjectGuid());
    info.extraAttacks.count = uint32(damage);
    AddExecuteLogInfo(eff_idx, info);
}

void Spell::EffectParry(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanParry(true);
}

void Spell::EffectBlock(SpellEffectIndex /*eff_idx*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanBlock(true);
}

void Spell::EffectLeapForward(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    if (unitTarget->IsTaxiFlying())
        return;

    float x, y, z;
    m_targets.getDestination(x, y, z);

    unitTarget->NearTeleportTo(x, y, z, unitTarget->GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT | TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget == m_caster ? TELE_TO_SPELL : 0));

}

void Spell::EffectReputation(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)unitTarget;

    int32  rep_change = m_currentBasePoints[eff_idx];
    uint32 faction_id = m_spellInfo->EffectMiscValue[eff_idx];

    FactionEntry const* factionEntry = sObjectMgr.GetFactionEntry(faction_id);

    if (!factionEntry)
        return;

    rep_change = _player->CalculateReputationGain(REPUTATION_SOURCE_SPELL, rep_change, faction_id);

    _player->GetReputationMgr().ModifyReputation(factionEntry, rep_change);
}

void Spell::EffectQuestComplete(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 quest_id = m_spellInfo->EffectMiscValue[eff_idx];

    ((Player*)unitTarget)->AreaExploredOrEventHappens(quest_id);
}

void Spell::EffectSelfResurrect(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->IsAlive())
        return;
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!unitTarget->IsInWorld())
        return;

    // Disallow resurrection in arena.
    Player* player = unitTarget->ToPlayer();
    if (player && player->InArena())
    {
        player->RepopAtGraveyard(); // not working?
        return;
    }

    uint32 health = 0;
    uint32 mana = 0;

    // flat case
    if (damage < 0)
    {
        health = uint32(-damage);
        mana = m_spellInfo->EffectMiscValue[eff_idx];
    }
    // percent case
    else
    {
        health = uint32(damage / 100.0f * unitTarget->GetMaxHealth());
        if (unitTarget->GetMaxPower(POWER_MANA) > 0)
            mana = uint32(damage / 100.0f * unitTarget->GetMaxPower(POWER_MANA));
    }

    int32 const recoveryMod = unitTarget->GetTotalAuraModifier(SPELL_AURA_MOD_SELF_RESURRECTION_RECOVERY);
    if (recoveryMod > 0)
    {
        health += health * uint32(recoveryMod) / 100;
        mana += mana * uint32(recoveryMod) / 100;
    }

    if (Aura const* manaBonus = unitTarget->GetAura(51893, EFFECT_INDEX_0))
        if (manaBonus->GetModifier()->m_amount > 0)
            mana += mana * uint32(manaBonus->GetModifier()->m_amount) / 100;

    health = std::min<uint32>(health, unitTarget->GetMaxHealth());
    mana = std::min<uint32>(mana, unitTarget->GetMaxPower(POWER_MANA));

    Player *plr = ((Player*)unitTarget);
    plr->ResurrectPlayer(0.0f);

    plr->SetHealth(health);
    plr->SetPower(POWER_MANA, mana);
    plr->SetPower(POWER_RAGE, 0);
    plr->SetPower(POWER_ENERGY, plr->GetMaxPower(POWER_ENERGY));

    plr->SpawnCorpseBones();
}

void Spell::EffectSkinning(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget->IsInWorld() || unitTarget->GetTypeId() != TYPEID_UNIT)
        return;
    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER || !m_caster->IsInWorld())
        return;

    Creature* creature = (Creature*) unitTarget;
    int32 targetLevel = creature->GetLevel();

    ((Player*)m_caster)->SendLoot(creature->GetObjectGuid(), LOOT_SKINNING);
    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

    int32 reqValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel - 10) * 10 : targetLevel * 5;

    int32 skillValue = ((Player*)m_caster)->GetSkillValuePure(SKILL_SKINNING);

    // Double chances for elites
    ((Player*)m_caster)->UpdateGatherSkill(SKILL_SKINNING, skillValue, reqValue, creature->IsElite() ? 2 : 1);
}

void Spell::EffectCharge(SpellEffectIndex /*eff_idx*/)
{
    if (!unitTarget)
        return;

    // cf Spell::OnSpellLaunch
}

#define FIELD_REPAIR_BOT_75B 50041
#define POLEY_DUMMY_SUMMON_SPELL 28505
#define ACTUAL_POLEY_ITEM 22781

void Spell::EffectSummonCritter(SpellEffectIndex eff_idx)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* player = (Player*) m_caster;

    uint32 pet_entry = m_spellInfo->EffectMiscValue[eff_idx];
    if (!pet_entry)
        return;

    CreatureInfo const* cInfo = sObjectMgr.GetCreatureTemplate(pet_entry);

    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummonCritter: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    Pet* old_critter = player->GetMiniPet();

    // for same pet just despawn
    if (old_critter && old_critter->GetEntry() == cInfo->entry)
    {
        player->RemoveMiniPet();
        return;
    }

    // despawn old pet before summon new
    if (old_critter)
        player->RemoveMiniPet();

    CreatureCreatePos pos(m_caster, m_caster->GetOrientation(), PET_FOLLOW_DIST, 270.0f * (M_PI_F / 180.0f));
    if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        pos = CreatureCreatePos(m_caster, m_caster->GetOrientation());

    // summon new pet
    Pet* critter = new Pet(MINI_PET);

    Map *map = m_caster->GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!critter->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        sLog.outError("Spell::EffectSummonCritter, spellid %u: no such creature entry %u", m_spellInfo->Id, pet_entry);
        delete critter;
        return;
    }

    critter->SetSummonPoint(pos);
    critter->SetOwnerGuid(m_caster->GetObjectGuid());
    critter->SetCreatorGuid(m_caster->GetObjectGuid());
    critter->SetFactionTemplateId(m_caster->GetFactionTemplateId());
    critter->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
    critter->InitializeDefaultName();
    critter->AIM_Initialize();
    critter->InitPetCreateSpells();                         // e.g. disgusting oozeling has a create spell as critter...
    critter->SelectLevel(critter->GetCreatureInfo());       // some summoned creaters have different from 1 DB data for level/hp
    critter->SetUInt32Value(UNIT_NPC_FLAGS, critter->GetCreatureInfo()->npc_flags);
    // some mini-pets have quests

    // set timer for unsummon
    int32 duration = m_spellInfo->GetDuration();
    if (duration > 0)
        critter->SetDuration(duration);

    player->_SetMiniPet(critter);

    map->Add((Creature*)critter);
    critter->SetFacingToObject(player);

    // Notify Summoner
    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(critter);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(critter->GetObjectGuid()));

    if (m_spellScript)
        m_spellScript->OnSummon(this, critter);
}

void Spell::EffectKnockBack(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->IsTaxiFlying())
        return;

    // remove Dream Fog Sleep aura to let target be launched
    // ugly and barely working solution untill proper pending states handling implemented
    unitTarget->RemoveAurasDueToSpell(24778);

    unitTarget->KnockBackFrom(m_caster, float(m_spellInfo->EffectMiscValue[eff_idx]) / 10, float(damage) / 10);
}

void Spell::EffectSendTaxi(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->ActivateTaxiPathTo(m_spellInfo->EffectMiscValue[eff_idx], m_spellInfo->Id, true);
}


void Spell::EffectPlayerPull(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    float dist = unitTarget->GetDistance2d(m_caster);
    if (damage && dist > damage)
        dist = float(damage);
    unitTarget->KnockBackFrom(m_caster, -dist, float(m_spellInfo->EffectMiscValue[eff_idx]) / 10);
}

void Spell::EffectDispelMechanic(SpellEffectIndex eff_idx)
{
    if (!unitTarget)
        return;

    uint32 mechanic = m_spellInfo->EffectMiscValue[eff_idx];

    Unit::SpellAuraHolderMap& Auras = unitTarget->GetSpellAuraHolderMap();
    for (Unit::SpellAuraHolderMap::iterator iter = Auras.begin(), next; iter != Auras.end(); iter = next)
    {
        next = iter;
        ++next;
        SpellEntry const *spell = iter->second->GetSpellProto();
        if (iter->second->HasMechanic(mechanic))
        {
            unitTarget->RemoveAurasDueToSpell(spell->Id);
            if (Auras.empty())
                break;
            else
                next = Auras.begin();
        }
    }

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectSummonDeadPet(SpellEffectIndex /*eff_idx*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player *_player = (Player*)m_caster;
    Pet *pet = _player->GetPet();
    if (!pet)
        return;
    if (pet->IsAlive())
        return;
    if (damage < 0)
        return;

    // Chakor : Teleport the pet to the player's location
    pet->NearTeleportTo(_player->GetPosition(), false);
    pet->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
    pet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    pet->SetDeathState(ALIVE);
    pet->ClearUnitState(UNIT_STAT_ALL_DYN_STATES);
    pet->SetHealth(uint32(pet->GetMaxHealth() * (float(damage) / 100)));

    pet->AIM_Initialize();

    // _player->PetSpellInitialize(); -- action bar not removed at death and not required send at revive
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
}

void Spell::EffectDestroyAllTotems(SpellEffectIndex /*eff_idx*/)
{
    if (!m_casterUnit)
        return;

    for (int slot = 0; slot < MAX_TOTEM_SLOT; ++slot)
    {
        if (Totem* totem = m_casterUnit->GetTotem(TotemSlot(slot)))
        {
            if (m_spellScript)
                m_spellScript->OnDestroyTotem(this, totem);

            totem->UnSummon();
        }
    }



}

void Spell::EffectDurabilityDamage(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->EffectMiscValue[eff_idx];

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if (slot < 0)
    {
        ((Player*)unitTarget)->DurabilityPointsLossAll(damage, (slot < -1));

        ExecuteLogInfo info(unitTarget->GetObjectGuid());
        info.durabilityDamage.itemEntry = -1;
        info.durabilityDamage.unk = -1;
        AddExecuteLogInfo(eff_idx, info);

        return;
    }

    // invalid slot value
    if (slot >= INVENTORY_SLOT_BAG_END)
        return;

    if (Item* item = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
    {
        ((Player*)unitTarget)->DurabilityPointsLoss(item, damage);

        ExecuteLogInfo info(unitTarget->GetObjectGuid());
        info.durabilityDamage.itemEntry = item->GetProto()->ItemId;
        info.durabilityDamage.unk = -1;
        AddExecuteLogInfo(eff_idx, info);
    }
}

void Spell::EffectDurabilityDamagePCT(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = m_spellInfo->EffectMiscValue[eff_idx];

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if (slot < 0)
    {
        ((Player*)unitTarget)->DurabilityLossAll(double(damage) / 100.0f, (slot < -1));
        return;
    }

    // invalid slot value
    if (slot >= INVENTORY_SLOT_BAG_END)
        return;

    if (damage <= 0)
        return;

    if (Item* item = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        ((Player*)unitTarget)->DurabilityLoss(item, double(damage) / 100.0f);
}

void Spell::EffectModifyThreatPercent(SpellEffectIndex eff_idx)
{
    if (!unitTarget || !m_casterUnit)
        return;

    unitTarget->GetThreatManager().modifyThreatPercent(m_casterUnit, damage);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(unitTarget->GetObjectGuid()));
}

void Spell::EffectTransmitted(SpellEffectIndex eff_idx)
{
    if (!m_casterUnit)
        return;

    const auto ui_NameId{ m_spellInfo->EffectMiscValue[eff_idx] };

    GameObjectInfo const* goinfo{ sObjectMgr.GetGameObjectInfo(ui_NameId) };

    if (!goinfo)
    {
        sLog.outErrorDb("Gameobject (Entry: %u) not exist and not created at spell (ID: %u) cast", ui_NameId, m_spellInfo->Id);
        return;
    }

    float fx{}, fy{}, fz{};
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        fx = m_targets.m_destX;
        fy = m_targets.m_destY;
        fz = m_targets.m_destZ;
    }
    else if (m_spellInfo->EffectRadiusIndex[eff_idx] && m_spellInfo->speed == 0)
    {
        float x{}, y{}, z{};
        m_casterUnit->GetPosition(x, y, z);

        const float dis{ GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellInfo->EffectRadiusIndex[eff_idx])) };
        fx = x + dis * cos(m_casterUnit->GetOrientation());
        fy = y + dis * sin(m_casterUnit->GetOrientation());
        fz = z;

        m_casterUnit->GetMap()->GetLosHitPosition(x, y, (z + .5f), fx, fy, fz, -1.5f);
    }
    else
    {
        const float min_dis{ GetSpellMinRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex)) };
        const float max_dis{ GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex)) };
        const float dis{ rand_norm_f() * (max_dis - min_dis) + min_dis };

        const float max_angle{ (max_dis - min_dis) / (max_dis + m_caster->GetObjectBoundingRadius()) };
        const float angle_offset{ max_angle * (rand_norm_f() - .5f) };

        float x{}, y{}, z{};
        m_casterUnit->GetPosition(x, y, z);

        fx = x + dis * cos((m_casterUnit->GetOrientation() + angle_offset));
        fy = y + dis * sin((m_casterUnit->GetOrientation() + angle_offset));
        fz = z;

        m_casterUnit->GetMap()->GetLosHitPosition(x, y, (z + 2.f), fx, fy, fz, -1.5f);
    }

    Map* cMap{ m_casterUnit->GetMap() };

    if (goinfo->type == GAMEOBJECT_TYPE_FISHINGNODE)
    {
        GridMapLiquidData liqData;

        if (!m_caster->GetTerrain()->IsSwimmable(fx, fy, m_caster->GetPositionZ() + 1.f, 1.5, &liqData))
        {
            m_caster->GetTerrain()->IsSwimmable(fx, fy, liqData.level, 1.5, &liqData);
        }

        float x{}, y{}, z{};
        m_casterUnit->GetPosition(x, y, z);

        if ((abs(liqData.depth_level) < 1) || !(m_caster->GetMap()->isInLineOfSight(x, y, z + 2.f, fx, fy, liqData.level))) // Hack to prevent fishing bobber from failing to land on fishing hole
        {
            // This is not proper, we really need to ignore not materialized objects
            SendCastResult(SPELL_FAILED_NOT_FISHABLE);
            SendChannelUpdate(0);
            finish();
            return;
        }

        // Replace by water level in this case
        fz = liqData.level;
    }

    GameObject* pGameObj{ new GameObject };

    if (!pGameObj->Create(cMap->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), ui_NameId, cMap,
        fx, fy, fz, m_casterUnit->GetOrientation(), 0.0f, 0.0f, 0.0f, 0.0f, GO_ANIMPROGRESS_DEFAULT, GO_STATE_READY))
    {
        delete pGameObj;
        return;
    }

    int32 duration{ m_spellInfo->GetDuration() };

    switch (goinfo->type)
    {
        case GAMEOBJECT_TYPE_FISHINGNODE:
        {
            m_casterUnit->SetChannelObjectGuid(pGameObj->GetObjectGuid());
            m_casterUnit->AddGameObject(pGameObj); // Will removed at spell cancel

            // End time of range when possible catch fish (FISHING_BOBBER_READY_TIME..GetDuration(m_spellInfo))
            // Start time == fish-FISHING_BOBBER_READY_TIME (0..GetDuration(m_spellInfo)-FISHING_BOBBER_READY_TIME)
            int32 lastSec{};
            switch (urand(0, 3))
            {
                case 0:
                {
                    lastSec = 3;
                    break;
                }
                case 1:
                {
                    lastSec = 7;
                    break;
                }
                case 2:
                {
                    lastSec = 13;
                    break;
                }
                case 3:
                {
                    lastSec = 17;
                    break;
                }
            }

            duration = duration - lastSec * IN_MILLISECONDS + FISHING_BOBBER_READY_TIME * IN_MILLISECONDS;
            break;
        }
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
        {
            if (m_casterUnit->GetTypeId() == TYPEID_PLAYER)
            {
                // Set the summoning target
                if (m_casterUnit->GetTypeId() == TYPEID_PLAYER && m_casterUnit->ToPlayer()->GetSelectionGuid())
                    pGameObj->SetSummonTarget(m_casterUnit->ToPlayer()->GetSelectionGuid());

                // Will be removed at Spell::Cancel or GameObject::RemoveUniqueUse if activated
                m_casterUnit->AddGameObject(pGameObj);

                // The caster becomes a go user too
                pGameObj->AddUniqueUse(m_casterUnit->ToPlayer());
                m_targets.setGOTarget(pGameObj);
                SetChannelingVisual(true);
            }
            break;
        }
        case GAMEOBJECT_TYPE_FISHINGHOLE:
        case GAMEOBJECT_TYPE_CHEST:
        default:
            break;
    }

    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetOwnerGuid(m_casterUnit->GetObjectGuid());
    
    if (m_casterUnit->GetTypeId() == TYPEID_PLAYER)
    {
        if (Group* group{ static_cast<Player*>(m_casterUnit)->GetGroup() })
        {
            pGameObj->SetOwnerGroupId(group->GetId());
        }
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_casterUnit->GetLevel());
    pGameObj->SetSpellId(m_spellInfo->Id);

    DEBUG_LOG("AddObject at SpellEfects.cpp EffectTransmitted");

    cMap->Add(pGameObj);

    pGameObj->SummonLinkedTrapIfAny();

    if (m_casterUnit->GetTypeId() == TYPEID_UNIT && static_cast<Creature*>(m_casterUnit)->AI())
    {
        static_cast<Creature*>(m_casterUnit)->AI()->JustSummoned(pGameObj);
    }

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(pGameObj->GetObjectGuid()));

    if (m_spellScript)
        m_spellScript->OnSummon(this, pGameObj);
}

void Spell::EffectSkill(SpellEffectIndex /*eff_idx*/)
{
    DEBUG_LOG("WORLD: SkillEFFECT");
}

void Spell::EffectSummonDemon(SpellEffectIndex eff_idx)
{
    float px = m_targets.m_destX;
    float py = m_targets.m_destY;
    float pz = m_targets.m_destZ;

    // summon to the ritual object location if any
    if (GameObject* pGo = m_targets.getGOTarget())
        if (pGo->GetGoType() == GAMEOBJECT_TYPE_SUMMONING_RITUAL)
            pGo->GetPosition(px, py, pz);

    uint32 const summonDuration = m_duration > 0 ? m_duration : 3600000;
    Creature* pSummon = m_caster->SummonCreature(m_spellInfo->EffectMiscValue[eff_idx], px, py, pz, m_caster->GetOrientation(), TEMPSUMMON_TIMED_COMBAT_OR_DEAD_DESPAWN, summonDuration);
    if (!pSummon)
        return;

    // might not always work correctly, maybe the creature that dies from CoD casts the effect on itself and is therefore the caster?
    pSummon->SetLevel(m_caster->GetLevel());

    // TODO: Add damage/mana/hp according to level

    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->Id == 1122)
        sLog.out(LOG_HARDCORE_MODE, "DANGER: Player %s (level: %u) cast summon infernal on %f %f %f %u", m_caster->GetName(), m_caster->GetLevel(), px, py, pz, m_caster->GetMapId());

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(pSummon->GetObjectGuid()));

    if (m_spellScript)
        m_spellScript->OnSummon(this, pSummon);
}

void Spell::EffectSpiritHeal(SpellEffectIndex /*eff_idx*/)
{
    // TODO player can't see the heal-animation - he should respawn some ticks later
    if (!unitTarget || unitTarget->IsAlive())
        return;
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!unitTarget->IsInWorld())
        return;
    if (!unitTarget->HasAura(2584))
        return;

    auto player = unitTarget->ToPlayer();

    if (!player)
        return;

    // no resurrection on a GY other than homie if BG is not in progress
    if (((player->GetBattleGround() && player->GetBattleGround()->GetStatus() != STATUS_IN_PROGRESS) || !player->InGurubashiArena(true)) && !player->IsGameMaster())
        player->RepopAtGraveyard();

    player->RemoveAurasDueToSpell(2584);
    player->ResurrectPlayer(1.0f);
    player->SpawnCorpseBones();
    player->AutoReSummonPet();
}

// remove insignia spell effect
void Spell::EffectSkinPlayerCorpse(SpellEffectIndex eff_idx)
{
    DEBUG_LOG("Effect: SkinPlayerCorpse");
    Player* playerCaster = m_caster->ToPlayer();
    if (!playerCaster)
        return;

    Unit *target = unitTarget;
    if (!target && corpseTarget)
        target = ObjectAccessor::FindPlayer(corpseTarget->GetOwnerGuid());
    if (!target)
    {
        ASSERT(corpseTarget);
        sObjectAccessor.ConvertCorpseForPlayer(corpseTarget->GetOwnerGuid(), playerCaster);
        playerCaster->SendLoot(corpseTarget->GetObjectGuid(), LOOT_INSIGNIA);
        DEBUG_LOG("Effect SkinPlayerCorpse: corpse owner was not found");
        return;
    }

    if (target->GetTypeId() != TYPEID_PLAYER || target->IsAlive())
        return;

    ((Player*)target)->RemovedInsignia(playerCaster, corpseTarget);

    AddExecuteLogInfo(eff_idx, ExecuteLogInfo(target->GetObjectGuid()));
}
void Spell::EffectBind(SpellEffectIndex eff_idx)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    uint32 area_id;
    WorldLocation loc;
    player->GetPosition(loc);
    area_id = player->GetAreaId();

    player->SetHomebindToLocation(loc, area_id);

    // binding
    WorldPacket data(SMSG_BINDPOINTUPDATE, (4 + 4 + 4 + 4 + 4));
    data << float(loc.x);
    data << float(loc.y);
    data << float(loc.z);
    data << uint32(loc.mapId);
    data << uint32(area_id);
    player->SendDirectMessage(&data);

    DEBUG_LOG("New Home Position X is %f", loc.x);
    DEBUG_LOG("New Home Position Y is %f", loc.y);
    DEBUG_LOG("New Home Position Z is %f", loc.z);
    DEBUG_LOG("New Home MapId is %u", loc.mapId);
    DEBUG_LOG("New Home AreaId is %u", area_id);

    // zone update
    data.Initialize(SMSG_PLAYERBOUND, 8 + 4);
    data << m_caster->GetObjectGuid();
    data << uint32(area_id);
    player->SendDirectMessage(&data);
}

void Spell::EffectDespawnObject(SpellEffectIndex eff_idx)
{
    DEBUG_LOG("SPELL_EFFECT_DESPAWN_OBJECT");
    if (!gameObjTarget)
        return;
    gameObjTarget->AddObjectToRemoveList();
}
void Spell::EffectNostalrius(SpellEffectIndex eff_idx)
{
    DEBUG_LOG("SPELL_EFFECT_NOSTALRIUS");
}
