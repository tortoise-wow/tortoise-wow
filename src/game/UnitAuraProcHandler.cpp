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
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "Totem.h"
#include "Creature.h"
#include "Formulas.h"
#include "ObjectAccessor.h"
#include "CreatureAI.h"
#include "ScriptMgr.h"
#include "Util.h"

#include <mutex>
#include <unordered_set>

pAuraProcHandler AuraProcHandler[TOTAL_AURAS] =
{
    &Unit::HandleNULLProc,                                  //  0 SPELL_AURA_NONE
    &Unit::HandleNULLProc,                                  //  1 SPELL_AURA_BIND_SIGHT
    &Unit::HandleNULLProc,                                  //  2 SPELL_AURA_MOD_POSSESS
    &Unit::HandleNULLProc,                                  //  3 SPELL_AURA_PERIODIC_DAMAGE
    &Unit::HandleDummyAuraProc,                             //  4 SPELL_AURA_DUMMY
    &Unit::HandleNULLProc,                                  //  5 SPELL_AURA_MOD_CONFUSE
    &Unit::HandleNULLProc,                                  //  6 SPELL_AURA_MOD_CHARM
    &Unit::HandleNULLProc,                                  //  7 SPELL_AURA_MOD_FEAR
    &Unit::HandleNULLProc,                                  //  8 SPELL_AURA_PERIODIC_HEAL
    &Unit::HandleNULLProc,                                  //  9 SPELL_AURA_MOD_ATTACKSPEED
    &Unit::HandleNULLProc,                                  // 10 SPELL_AURA_MOD_THREAT
    &Unit::HandleNULLProc,                                  // 11 SPELL_AURA_MOD_TAUNT
    &Unit::HandleNULLProc,                                  // 12 SPELL_AURA_MOD_STUN
    &Unit::HandleModDamageAuraProc,                         // 13 SPELL_AURA_MOD_DAMAGE_DONE
    &Unit::HandleNULLProc,                                  // 14 SPELL_AURA_MOD_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  // 15 SPELL_AURA_DAMAGE_SHIELD
    &Unit::HandleNULLProc,                                  // 16 SPELL_AURA_MOD_STEALTH
    &Unit::HandleNULLProc,                                  // 17 SPELL_AURA_MOD_STEALTH_DETECT
    &Unit::HandleInvisibilityAuraProc,                      // 18 SPELL_AURA_MOD_INVISIBILITY
    &Unit::HandleNULLProc,                                  // 19 SPELL_AURA_MOD_INVISIBILITY_DETECTION
    &Unit::HandleNULLProc,                                  // 20 SPELL_AURA_OBS_MOD_HEALTH
    &Unit::HandleNULLProc,                                  // 21 SPELL_AURA_OBS_MOD_MANA
    &Unit::HandleModResistanceAuraProc,                     // 22 SPELL_AURA_MOD_RESISTANCE
    &Unit::HandleNULLProc,                                  // 23 SPELL_AURA_PERIODIC_TRIGGER_SPELL
    &Unit::HandleNULLProc,                                  // 24 SPELL_AURA_PERIODIC_ENERGIZE
    &Unit::HandleNULLProc,                                  // 25 SPELL_AURA_MOD_PACIFY
    &Unit::HandleRemoveByDamageChanceProc,                  // 26 SPELL_AURA_MOD_ROOT
    &Unit::HandleNULLProc,                                  // 27 SPELL_AURA_MOD_SILENCE
    &Unit::HandleNULLProc,                                  // 28 SPELL_AURA_REFLECT_SPELLS
    &Unit::HandleNULLProc,                                  // 29 SPELL_AURA_MOD_STAT
    &Unit::HandleNULLProc,                                  // 30 SPELL_AURA_MOD_SKILL
    &Unit::HandleNULLProc,                                  // 31 SPELL_AURA_MOD_INCREASE_SPEED
    &Unit::HandleNULLProc,                                  // 32 SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED
    &Unit::HandleNULLProc,                                  // 33 SPELL_AURA_MOD_DECREASE_SPEED
    &Unit::HandleNULLProc,                                  // 34 SPELL_AURA_MOD_INCREASE_HEALTH
    &Unit::HandleNULLProc,                                  // 35 SPELL_AURA_MOD_INCREASE_ENERGY
    &Unit::HandleNULLProc,                                  // 36 SPELL_AURA_MOD_SHAPESHIFT
    &Unit::HandleNULLProc,                                  // 37 SPELL_AURA_EFFECT_IMMUNITY
    &Unit::HandleNULLProc,                                  // 38 SPELL_AURA_STATE_IMMUNITY
    &Unit::HandleNULLProc,                                  // 39 SPELL_AURA_SCHOOL_IMMUNITY
    &Unit::HandleNULLProc,                                  // 40 SPELL_AURA_DAMAGE_IMMUNITY
    &Unit::HandleNULLProc,                                  // 41 SPELL_AURA_DISPEL_IMMUNITY
    &Unit::HandleProcTriggerSpellAuraProc,                  // 42 SPELL_AURA_PROC_TRIGGER_SPELL
    &Unit::HandleProcTriggerDamageAuraProc,                 // 43 SPELL_AURA_PROC_TRIGGER_DAMAGE
    &Unit::HandleNULLProc,                                  // 44 SPELL_AURA_TRACK_CREATURES
    &Unit::HandleNULLProc,                                  // 45 SPELL_AURA_TRACK_RESOURCES
    &Unit::HandleNULLProc,                                  // 46 SPELL_AURA_MOD_PARRY_SKILL
    &Unit::HandleNULLProc,                                  // 47 SPELL_AURA_MOD_PARRY_PERCENT
    &Unit::HandleNULLProc,                                  // 48 SPELL_AURA_MOD_DODGE_SKILL
    &Unit::HandleNULLProc,                                  // 49 SPELL_AURA_MOD_DODGE_PERCENT
    &Unit::HandleNULLProc,                                  // 50 SPELL_AURA_MOD_BLOCK_SKILL    obsolete?
    &Unit::HandleNULLProc,                                  // 51 SPELL_AURA_MOD_BLOCK_PERCENT
    &Unit::HandleNULLProc,                                  // 52 SPELL_AURA_MOD_CRIT_PERCENT
    &Unit::HandleNULLProc,                                  // 53 SPELL_AURA_PERIODIC_LEECH
    &Unit::HandleNULLProc,                                  // 54 SPELL_AURA_MOD_HIT_CHANCE
    &Unit::HandleNULLProc,                                  // 55 SPELL_AURA_MOD_SPELL_HIT_CHANCE
    &Unit::HandleNULLProc,                                  // 56 SPELL_AURA_TRANSFORM
    &Unit::HandleNULLProc,                                  // 57 SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  // 58 SPELL_AURA_MOD_INCREASE_SWIM_SPEED
    &Unit::HandleNULLProc,                                  // 59 SPELL_AURA_MOD_DAMAGE_DONE_CREATURE
    &Unit::HandleRemoveByDamageChanceProc,                  // 60 SPELL_AURA_MOD_PACIFY_SILENCE
    &Unit::HandleNULLProc,                                  // 61 SPELL_AURA_MOD_SCALE
    &Unit::HandleNULLProc,                                  // 62 SPELL_AURA_PERIODIC_HEALTH_FUNNEL
    &Unit::HandleNULLProc,                                  // 63 SPELL_AURA_PERIODIC_MANA_FUNNEL obsolete?
    &Unit::HandleNULLProc,                                  // 64 SPELL_AURA_PERIODIC_MANA_LEECH
    &Unit::HandleModCastingSpeedNotStackAuraProc,           // 65 SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  // 66 SPELL_AURA_FEIGN_DEATH
    &Unit::HandleNULLProc,                                  // 67 SPELL_AURA_MOD_DISARM
    &Unit::HandleNULLProc,                                  // 68 SPELL_AURA_MOD_STALKED
    &Unit::HandleNULLProc,                                  // 69 SPELL_AURA_SCHOOL_ABSORB
    &Unit::HandleNULLProc,                                  // 70 SPELL_AURA_EXTRA_ATTACKS      Useless, used by only one spell that has only visual effect
    &Unit::HandleNULLProc,                                  // 71 SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    &Unit::HandleModPowerCostSchoolAuraProc,                // 72 SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT
    &Unit::HandleModPowerCostSchoolAuraProc,                // 73 SPELL_AURA_MOD_POWER_COST_SCHOOL
    &Unit::HandleReflectSpellsSchoolAuraProc,               // 74 SPELL_AURA_REFLECT_SPELLS_SCHOOL
    &Unit::HandleNULLProc,                                  // 75 SPELL_AURA_MOD_LANGUAGE
    &Unit::HandleNULLProc,                                  // 76 SPELL_AURA_FAR_SIGHT
    &Unit::HandleMechanicImmuneResistanceAuraProc,          // 77 SPELL_AURA_MECHANIC_IMMUNITY
    &Unit::HandleNULLProc,                                  // 78 SPELL_AURA_MOUNTED
    &Unit::HandleNULLProc,                                  // 79 SPELL_AURA_MOD_DAMAGE_PERCENT_DONE
    &Unit::HandleNULLProc,                                  // 80 SPELL_AURA_MOD_PERCENT_STAT
    &Unit::HandleNULLProc,                                  // 81 SPELL_AURA_SPLIT_DAMAGE_PCT
    &Unit::HandleNULLProc,                                  // 82 SPELL_AURA_WATER_BREATHING
    &Unit::HandleNULLProc,                                  // 83 SPELL_AURA_MOD_BASE_RESISTANCE
    &Unit::HandleNULLProc,                                  // 84 SPELL_AURA_MOD_REGEN
    &Unit::HandleCantTrigger,                               // 85 SPELL_AURA_MOD_POWER_REGEN
    &Unit::HandleNULLProc,                                  // 86 SPELL_AURA_CHANNEL_DEATH_ITEM
    &Unit::HandleNULLProc,                                  // 87 SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN
    &Unit::HandleNULLProc,                                  // 88 SPELL_AURA_MOD_HEALTH_REGEN_PERCENT
    &Unit::HandleNULLProc,                                  // 89 SPELL_AURA_PERIODIC_DAMAGE_PERCENT
    &Unit::HandleNULLProc,                                  // 90 SPELL_AURA_MOD_RESIST_CHANCE  Useless
    &Unit::HandleNULLProc,                                  // 91 SPELL_AURA_MOD_DETECT_RANGE
    &Unit::HandleNULLProc,                                  // 92 SPELL_AURA_PREVENTS_FLEEING
    &Unit::HandleNULLProc,                                  // 93 SPELL_AURA_MOD_UNATTACKABLE
    &Unit::HandleNULLProc,                                  // 94 SPELL_AURA_INTERRUPT_REGEN
    &Unit::HandleNULLProc,                                  // 95 SPELL_AURA_GHOST
    &Unit::HandleNULLProc,                                  // 96 SPELL_AURA_SPELL_MAGNET
    &Unit::HandleNULLProc,                                  // 97 SPELL_AURA_MANA_SHIELD
    &Unit::HandleNULLProc,                                  // 98 SPELL_AURA_MOD_SKILL_TALENT
    &Unit::HandleNULLProc,                                  // 99 SPELL_AURA_MOD_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //100 SPELL_AURA_AURAS_VISIBLE obsolete? all player can see all auras now
    &Unit::HandleNULLProc,                                  //101 SPELL_AURA_MOD_RESISTANCE_PCT
    &Unit::HandleNULLProc,                                  //102 SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS
    &Unit::HandleNULLProc,                                  //103 SPELL_AURA_MOD_TOTAL_THREAT
    &Unit::HandleNULLProc,                                  //104 SPELL_AURA_WATER_WALK
    &Unit::HandleNULLProc,                                  //105 SPELL_AURA_FEATHER_FALL
    &Unit::HandleNULLProc,                                  //106 SPELL_AURA_HOVER
    &Unit::HandleNULLProc,                                  //107 SPELL_AURA_ADD_FLAT_MODIFIER
    &Unit::HandleNULLProc,                                  //108 SPELL_AURA_ADD_PCT_MODIFIER
    &Unit::HandleAddTargetTriggerAuraProc,                  //109 SPELL_AURA_ADD_TARGET_TRIGGER
    &Unit::HandleNULLProc,                                  //110 SPELL_AURA_MOD_POWER_REGEN_PERCENT
    &Unit::HandleNULLProc,                                  //111 SPELL_AURA_ADD_CASTER_HIT_TRIGGER
    &Unit::HandleOverrideClassScriptAuraProc,               //112 SPELL_AURA_OVERRIDE_CLASS_SCRIPTS
    &Unit::HandleNULLProc,                                  //113 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  //114 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT
    &Unit::HandleNULLProc,                                  //115 SPELL_AURA_MOD_HEALING
    &Unit::HandleNULLProc,                                  //116 SPELL_AURA_MOD_REGEN_DURING_COMBAT
    &Unit::HandleMechanicImmuneResistanceAuraProc,          //117 SPELL_AURA_MOD_MECHANIC_RESISTANCE
    &Unit::HandleNULLProc,                                  //118 SPELL_AURA_MOD_HEALING_PCT
    &Unit::HandleNULLProc,                                  //119 SPELL_AURA_SHARE_PET_TRACKING useless
    &Unit::HandleNULLProc,                                  //120 SPELL_AURA_UNTRACKABLE
    &Unit::HandleNULLProc,                                  //121 SPELL_AURA_EMPATHY
    &Unit::HandleNULLProc,                                  //122 SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT
    &Unit::HandleNULLProc,                                  //123 SPELL_AURA_MOD_TARGET_RESISTANCE
    &Unit::HandleNULLProc,                                  //124 SPELL_AURA_MOD_RANGED_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //125 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  //126 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT
    &Unit::HandleNULLProc,                                  //127 SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS
    &Unit::HandleNULLProc,                                  //128 SPELL_AURA_MOD_POSSESS_PET
    &Unit::HandleNULLProc,                                  //129 SPELL_AURA_MOD_SPEED_ALWAYS
    &Unit::HandleNULLProc,                                  //130 SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS
    &Unit::HandleNULLProc,                                  //131 SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS
    &Unit::HandleNULLProc,                                  //132 SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT
    &Unit::HandleNULLProc,                                  //133 SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT
    &Unit::HandleNULLProc,                                  //134 SPELL_AURA_MOD_MANA_REGEN_INTERRUPT
    &Unit::HandleNULLProc,                                  //135 SPELL_AURA_MOD_HEALING_DONE
    &Unit::HandleNULLProc,                                  //136 SPELL_AURA_MOD_HEALING_DONE_PERCENT
    &Unit::HandleNULLProc,                                  //137 SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE
    &Unit::HandleHasteAuraProc,                             //138 SPELL_AURA_MOD_MELEE_HASTE
    &Unit::HandleNULLProc,                                  //139 SPELL_AURA_FORCE_REACTION
    &Unit::HandleNULLProc,                                  //140 SPELL_AURA_MOD_RANGED_HASTE
    &Unit::HandleNULLProc,                                  //141 SPELL_AURA_MOD_RANGED_AMMO_HASTE
    &Unit::HandleNULLProc,                                  //142 SPELL_AURA_MOD_BASE_RESISTANCE_PCT
    &Unit::HandleNULLProc,                                  //143 SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE
    &Unit::HandleNULLProc,                                  //144 SPELL_AURA_SAFE_FALL
    &Unit::HandleNULLProc,                                  //145 SPELL_AURA_CHARISMA obsolete?
    &Unit::HandleNULLProc,                                  //146 SPELL_AURA_PERSUADED obsolete?
    &Unit::HandleNULLProc,                                  //147 SPELL_AURA_MECHANIC_IMMUNITY_MASK
    &Unit::HandleNULLProc,                                  //148 SPELL_AURA_RETAIN_COMBO_POINTS
    &Unit::HandleCantTrigger,                               //149 SPELL_AURA_RESIST_PUSHBACK
    &Unit::HandleNULLProc,                                  //150 SPELL_AURA_MOD_SHIELD_BLOCKVALUE_PCT
    &Unit::HandleNULLProc,                                  //151 SPELL_AURA_TRACK_STEALTHED
    &Unit::HandleNULLProc,                                  //152 SPELL_AURA_MOD_DETECTED_RANGE
    &Unit::HandleNULLProc,                                  //153 SPELL_AURA_SPLIT_DAMAGE_FLAT
    &Unit::HandleNULLProc,                                  //154 SPELL_AURA_MOD_STEALTH_LEVEL
    &Unit::HandleNULLProc,                                  //155 SPELL_AURA_MOD_WATER_BREATHING
    &Unit::HandleNULLProc,                                  //156 SPELL_AURA_MOD_REPUTATION_GAIN
    &Unit::HandleNULLProc,                                  //157 SPELL_AURA_PET_DAMAGE_MULTI (single test like spell 20782, also single for 214 aura)
    &Unit::HandleNULLProc,                                  //158 SPELL_AURA_MOD_SHIELD_BLOCKVALUE
    &Unit::HandleNULLProc,                                  //159 SPELL_AURA_NO_PVP_CREDIT      only for Honorless Target spell
    &Unit::HandleNULLProc,                                  //160 SPELL_AURA_MOD_AOE_AVOIDANCE
    &Unit::HandleNULLProc,                                  //161 SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT
    &Unit::HandleNULLProc,                                  //162 SPELL_AURA_POWER_BURN_MANA
    &Unit::HandleNULLProc,                                  //163 SPELL_AURA_MOD_CRIT_DAMAGE_BONUS
    &Unit::HandleNULLProc,                                  //164 useless, only one test spell
    &Unit::HandleNULLProc,                                  //165 SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS
    &Unit::HandleNULLProc,                                  //166 SPELL_AURA_MOD_ATTACK_POWER_PCT
    &Unit::HandleNULLProc,                                  //167 SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT
    &Unit::HandleNULLProc,                                  //168 SPELL_AURA_MOD_DAMAGE_DONE_VERSUS
    &Unit::HandleNULLProc,                                  //169 SPELL_AURA_MOD_CRIT_PERCENT_VERSUS
    &Unit::HandleNULLProc,                                  //170 SPELL_AURA_DETECT_AMORE       only for Detect Amore spell
    &Unit::HandleNULLProc,                                  //171 SPELL_AURA_MOD_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  //172 SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  //173 SPELL_AURA_ALLOW_CHAMPION_SPELLS  only for Proclaim Champion spell
    &Unit::HandleNULLProc,                                  //174 SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //175 SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //176 SPELL_AURA_SPIRIT_OF_REDEMPTION   only for Spirit of Redemption spell, die at aura end
    &Unit::HandleNULLProc,                                  //177 SPELL_AURA_AOE_CHARM
    &Unit::HandleNULLProc,                                  //178 SPELL_AURA_MOD_DEBUFF_RESISTANCE
    &Unit::HandleNULLProc,                                  //179 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //180 SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS
    &Unit::HandleNULLProc,                                  //181 SPELL_AURA_MOD_FLAT_SPELL_CRIT_DAMAGE_VERSUS unused
    &Unit::HandleNULLProc,                                  //182 SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //183 SPELL_AURA_MOD_CRITICAL_THREAT only used in 28746
    &Unit::HandleNULLProc,                                  //184 SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //185 SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //186 SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //187 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //188 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //189 SPELL_AURA_MOD_RATING
    &Unit::HandleNULLProc,                                  //190 SPELL_AURA_MOD_FACTION_REPUTATION_GAIN
    &Unit::HandleNULLProc,                                  //191 SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED
    &Unit::HandleNULLProc,                                  //192 SPELL_AURA_AURA_SPELL
    &Unit::HandleNULLProc,                                  //193 SPELL_AURA_SPLIT_DAMAGE_GROUP_PCT
    &Unit::HandleNULLProc,                                  //194 SPELL_AURA_MOD_AOE_DAMAGE_PERCENT_TAKEN
    &Unit::HandleNULLProc,                                  //195 SPELL_AURA_MOD_HONOR_GAIN
    &Unit::HandleNULLProc,                                  //196 SPELL_AURA_ENABLE_FLYING
    &Unit::HandleNULLProc,                                  //197 SPELL_AURA_MOD_PERIODIC_DAMAGE_PERCENT_TAKEN
    &Unit::HandleNULLProc,                                  //198 SPELL_AURA_MOD_CRIT_DAMAGE_BONUS_TAKEN
    &Unit::HandleNULLProc,                                  //199 SPELL_AURA_MOD_SPELL_HEALING_OF_ARMOR_PERCENT
    &Unit::HandleNULLProc,                                  //200 SPELL_AURA_TRANSFER_TOTEM_THREAT
    &Unit::HandleNULLProc,                                  //201 SPELL_AURA_TRIGGER_AURA_ON_TOTEMS
    &Unit::HandleNULLProc,                                  //202 SPELL_AURA_MOD_PET_STAT_PERCENT_OF_OWNER
    &Unit::HandleNULLProc,                                  //203 SPELL_AURA_MOD_PET_ARMOR_PERCENT_OF_OWNER
    &Unit::HandleNULLProc,                                  //204 SPELL_AURA_MOD_PET_RESISTANCE_PERCENT_OF_OWNER
    &Unit::HandleNULLProc,                                  //205 SPELL_AURA_MOD_PET_ATTACK_POWER_PERCENT_OF_OWNER
    &Unit::HandleNULLProc,                                  //206 SPELL_AURA_MOD_PET_SPELL_DAMAGE_PERCENT_OF_OWNER
    &Unit::HandleNULLProc,                                  //207 SPELL_AURA_MOD_ATTACK_AND_SPELL_RANGE
    &Unit::HandleNULLProc,                                  //208 SPELL_AURA_MOD_REAGENT_CONSUMPTION_CHANCE
    &Unit::HandleNULLProc,                                  //209 SPELL_AURA_MOD_MECHANIC_DURATION
    &Unit::HandleNULLProc,                                  //210 SPELL_AURA_MOD_SELF_RESURRECTION_RECOVERY
    &Unit::HandleNULLProc,                                  //211 SPELL_AURA_MOD_PET_SPELL_CRIT_PERCENT_OF_OWNER
    &Unit::HandleNULLProc,                                  //212 SPELL_AURA_MOD_IGNORE_TARGET_ARMOR
    &Unit::HandleNULLProc,                                  //213 SPELL_AURA_MOD_SPELL_DAMAGE_OF_INTELLECT_PERCENT
    &Unit::HandleNULLProc,                                  //214 SPELL_AURA_MOD_MANA_GAIN_PERCENT
    &Unit::HandleNULLProc,                                  //215 SPELL_AURA_MOD_PET_SPELL_HIT_PERCENT_OF_OWNER
    &Unit::HandleNULLProc,                                  //216 SPELL_AURA_MOD_PET_MELEE_HIT_PERCENT_OF_OWNER
    &Unit::HandleNULLProc,                                  //217 SPELL_AURA_MOD_ENERGY_REGEN_TIME
    &Unit::HandleNULLProc,                                  //218 SPELL_AURA_PERIODIC_TRIGGER_SPELL2
    &Unit::HandleNULLProc,                                  //219 SPELL_AURA_219
    &Unit::HandleNULLProc,                                  //220 SPELL_AURA_MOD_DAMAGE_TAKEN_FROM_CASTER_PET
    &Unit::HandleNULLProc,                                  //221 SPELL_AURA_MOD_ATTACK_POWER_AREA
    &Unit::HandleNULLProc,                                  //222 SPELL_AURA_MOD_ATTACK_POWER_PERCENT_AREA
    &Unit::HandleNULLProc,                                  //223 SPELL_AURA_MOD_ITEM_PROC_CHANCE
    &Unit::HandleModBlockDamagePercentAuraProc,             //224 SPELL_AURA_MOD_BLOCK_DAMAGE_PERCENT
    &Unit::HandleNULLProc,                                  //225 SPELL_AURA_MOD_GATHERING_ITEM_CHANCE
    &Unit::HandleModRageFromDamageDealtAuraProc,            //226 SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT
    &Unit::HandleNULLProc,                                  //227 SPELL_AURA_MOD_ATTACKING_RAGE_PERCENT
    &Unit::HandleNULLProc,                                  //228 SPELL_AURA_MOD_SKILL_CAST_TIME
    &Unit::HandleNULLProc,                                  //229 SPELL_AURA_MOD_PERIODIC_DAMAGE_PERCENT_DONE
    &Unit::HandleNULLProc,                                  //230 SPELL_AURA_MOD_CHAIN_DAMAGE_PERCENT_TAKEN
};

// Fonctions Nostalrius
inline bool SpellCanTrigger(const SpellEntry* spellProto, const SpellEntry* procSpell, uint8 eff_idx = 0)
{
    return (procSpell && procSpell->SpellFamilyName == spellProto->SpellFamilyName && procSpell->SpellFamilyFlags & spellProto->EffectItemType[eff_idx]);
}

SpellProcEventTriggerCheck Unit::IsTriggeredAtSpellProcEvent(Unit *pVictim, SpellAuraHolder* holder, SpellEntry const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, bool isVictim, SpellProcEventEntry const*& spellProcEvent, bool isSpellTriggeredByAuraOrItem) const
{
    SpellEntry const* spellProto = holder->GetSpellProto();

    // Flurry can't proc on additional windfury attacks
    if (spellProto->SpellIconID == 108 && spellProto->SpellVisual == 2759 && m_extraAttacks)
        return SPELL_PROC_TRIGGER_FAILED;

    // Allow Blind to proc Irritating agent and nothing else
    if (procSpell && procSpell->Id == 2094 &&
        spellProto->Id != 52535 && spellProto->Id != 52536 && spellProto->Id != 52537)
        return SPELL_PROC_TRIGGER_FAILED;

    if (holder->GetAuraScript())
        if (std::optional<SpellProcEventTriggerCheck> result = holder->GetAuraScript()->OnCheckProc(this, pVictim, holder, procSpell, procFlag, procExtra, attType, isVictim))
            return *result;

    /// [TODO]
    /// Delete all these spells, and manage it via the DB (spell_proc_event)
    if (procSpell && !(procExtra & PROC_EX_CAST_END))
    {
        // Wrath of Cenarius
        if (spellProto->Id == 25906)
        {
            // Make all damaging paladin spells able to proc Wrath of Cenarius.
            if (procSpell->SpellFamilyName == SPELLFAMILY_PALADIN)
                if (procSpell->IsDirectDamageSpell() || procSpell->HasDamagingAura())
                    return roll_chance_u(spellProto->procChance) ? SPELL_PROC_TRIGGER_OK : SPELL_PROC_TRIGGER_ROLL_FAILED;
        }
        // SPELL_AURA_ADD_TARGET_PROC
        // Chance of proc calculated after.
        if (spellProto->EffectApplyAuraName[0] == SPELL_AURA_ADD_TARGET_TRIGGER)
        {
            // All spells that proc on the victim
            // Then the others
            if (isVictim)
                return SPELL_PROC_TRIGGER_FAILED;

            switch (spellProto->Id)
            {
                // Frosty Zap
            case 24392:
                if (SpellCanTrigger(spellProto, procSpell))
                    return SPELL_PROC_TRIGGER_OK;
                break;
            }

            return SPELL_PROC_TRIGGER_FAILED;
        }
    }

    // Get proc Event Entry
    spellProcEvent = sSpellMgr.GetSpellProcEvent(spellProto->Id);

    // Custom hard-coded cases which depend on the proc event for firing...
    if (procSpell)
    {
        // Fear Ward always procs on any Fear (except ones cast by ourselves...)
        if (spellProto->Id == 6346 && isVictim)
        {
            if (procSpell->Mechanic == MECHANIC_FEAR)
                return SPELL_PROC_TRIGGER_OK;
            else
                return SPELL_PROC_TRIGGER_FAILED;
        }
    }

    // Get EventProcFlag
    uint32 EventProcFlag;
    if (spellProcEvent && spellProcEvent->procFlags) // if exist get custom spellProcEvent->procFlags
        EventProcFlag = spellProcEvent->procFlags;
    else
        EventProcFlag = spellProto->procFlags;       // else get from spell proto
    // Continue if no trigger exist
    if (!EventProcFlag)
        return SPELL_PROC_TRIGGER_FAILED;

    // Check spellProcEvent data requirements
    if (!SpellMgr::IsSpellProcEventCanTriggeredBy(spellProcEvent, EventProcFlag, procSpell, procFlag, procExtra))
        return SPELL_PROC_TRIGGER_FAILED;

    // In most cases req get honor or XP from kill
    if ((EventProcFlag & procFlag & PROC_FLAG_KILL) && IsPlayer())
    {
        if (!pVictim)
            return SPELL_PROC_TRIGGER_FAILED;

        bool allow = ((Player*)this)->IsHonorOrXPTarget(pVictim);
        if (!allow)
            return SPELL_PROC_TRIGGER_FAILED;
    }
    // Aura added by spell can`t trigger from self (prevent drop charges/do triggers)
    // But except periodic triggers (can triggered from self)
    if (procSpell && procSpell->Id == spellProto->Id && !(EventProcFlag & PROC_FLAG_TAKE_HARMFUL_PERIODIC))
        return SPELL_PROC_TRIGGER_FAILED;

    // Check if current equipment allows aura to proc
    if (!isVictim && IsPlayer())
    {
        if (spellProto->EquippedItemClass == ITEM_CLASS_WEAPON && !spellProto->HasAttribute(SPELL_ATTR_EX3_NO_PROC_EQUIP_REQUIREMENT))
        {
            Item *item = nullptr;
            if (attType == BASE_ATTACK)
                item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            else if (attType == OFF_ATTACK)
                item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            else
                item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);

            if (!CanUseEquippedWeapon(attType))
                return SPELL_PROC_TRIGGER_FAILED;

            if (!item || item->IsBroken() || item->GetProto()->Class != ITEM_CLASS_WEAPON || !((1 << item->GetProto()->SubClass) & spellProto->EquippedItemSubClassMask))
                return SPELL_PROC_TRIGGER_FAILED;
        }
        else if (spellProto->EquippedItemClass == ITEM_CLASS_ARMOR)
        {
            // Check if player is wearing shield
            Item *item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
            if (!item || item->IsBroken() || item->GetProto()->Class != ITEM_CLASS_ARMOR || !((1 << item->GetProto()->SubClass) & spellProto->EquippedItemSubClassMask))
                return SPELL_PROC_TRIGGER_FAILED;
        }
    }

    if (isSpellTriggeredByAuraOrItem && procSpell &&
        !procSpell->HasAttribute(SPELL_ATTR_EX3_NOT_A_PROC) &&
        !spellProto->HasAttribute(SPELL_ATTR_EX3_CAN_PROC_FROM_PROCS))
        return SPELL_PROC_TRIGGER_FAILED;

    if (spellProto->HasAttribute(SPELL_ATTR_EX3_ONLY_PROC_OUTDOORS) &&
        !GetTerrain()->IsOutdoors(GetPositionX(), GetPositionY(), GetPositionZ()))
        return SPELL_PROC_TRIGGER_FAILED;

    if (spellProto->HasAttribute(SPELL_ATTR_EX3_ONLY_PROC_ON_CASTER) &&
        holder->GetTarget()->GetObjectGuid() != holder->GetCasterGuid())
        return SPELL_PROC_TRIGGER_FAILED;

    // Get chance from spell
    float chance = (float)spellProto->procChance;
    // If in spellProcEvent exist custom chance, chance = spellProcEvent->customChance;
    if (spellProcEvent && spellProcEvent->customChance)
        chance = spellProcEvent->customChance;

    // If PPM exist calculate chance from PPM
    if (!isVictim && spellProcEvent && spellProcEvent->ppmRate != 0)
    {
        uint32 WeaponSpeed = GetAttackTime(attType);
        chance = GetPPMProcChance(WeaponSpeed, spellProcEvent->ppmRate);
    }

    // Apply chance modifier aura
    if (Player* modOwner = GetSpellModOwner())
    {
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CHANCE_OF_SUCCESS, chance);
        if (modOwner->HasOption(PLAYER_CHEAT_ALWAYS_PROC))
            return SPELL_PROC_TRIGGER_OK;
    }

    if (roll_chance_f(chance))
        return SPELL_PROC_TRIGGER_OK;

    return SPELL_PROC_TRIGGER_ROLL_FAILED;
}

SpellAuraProcResult Unit::HandleNULLProc(Unit* /*pVictim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    if (cooldown)
    {
        // We need a way to store cooldown for procs which dont trigger another spell.
        // Since spell ids are limited to uint16 in db, lets use unique fake id above that.
        // Eye of the Dead needs this to not proc on each hit, but we can't add and check
        // cooldown for its spell id, because the spell has an on use cooldown already.
        uint32 const spellId = triggeredByAura->GetId() + UINT16_MAX;

        if (HasSpellCooldown(spellId))
            return SPELL_AURA_PROC_FAILED;

        AddSpellCooldown(spellId, 0, time(nullptr) + cooldown);
    }

    // no proc handler for this aura type
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModBlockDamagePercentAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* /*triggeredByAura*/, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 procEx, uint32 /*cooldown*/)
{
    return (procEx & PROC_EX_BLOCK) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_CANT_TRIGGER;
}

SpellAuraProcResult Unit::HandleHasteAuraProc(Unit *pVictim, uint32 damage, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 procEx, uint32 cooldown)
{
    // Flurry: last charge crit will reapply the buff, don't remove any charges
    if (triggeredByAura->GetSpellProto()->SpellIconID == 108 && 
        triggeredByAura->GetSpellProto()->SpellVisual == 2759 &&
        triggeredByAura->GetHolder()->GetAuraCharges() <= 1 &&
        (procEx & PROC_EX_CRITICAL_HIT))
        return SPELL_AURA_PROC_FAILED;

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleDummyAuraProc(Unit *pVictim, uint32 damage, int32 originalAmount, Aura* triggeredByAura, SpellEntry const * procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellEntry const *dummySpell = triggeredByAura->GetSpellProto();
    int32  triggerAmount = triggeredByAura->GetModifier()->m_amount;

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    uint32 triggered_spell_id = 0;
    Unit* target = pVictim;
    int32  basepoints[MAX_EFFECT_INDEX] = {0, 0, 0};


    switch (dummySpell->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (dummySpell->Id)
            {
                // Illusion Passive
                case 7131:
                {
                    if (Creature* pCreature = ToCreature())
                        pCreature->DespawnOrUnsummon(1);
                    return SPELL_AURA_PROC_OK;
                }
                // Eye for an Eye
                case 9799:
                case 25988:
                {
                    // prevent damage back from weapon special attacks
                    if (!procSpell || procSpell->DmgClass != SPELL_DAMAGE_CLASS_MAGIC)
                        return SPELL_AURA_PROC_FAILED;

                    // return damage % to attacker but < 50% own total health
                    basepoints[0] = triggerAmount * int32(originalAmount) / 100;
                    if (basepoints[0] > (int32)GetMaxHealth() / 2)
                        basepoints[0] = (int32)GetMaxHealth() / 2;

                    triggered_spell_id = 25997;
                    break;
                }
                // Twisted Reflection (boss spell)
                case 21063:
                    triggered_spell_id = 21064;
                    break;
                // Unstable Power
                case 24658:
                {
                    // Need to remove one 24659 aura
                    // Holy Nova both heals and damages, so check needed to avoid consuming 2 charges
                    if (!procSpell->IsFitToFamilyMask<CF_PRIEST_HOLY_NOVA1>() && procSpell->UsesSpellPower())
                        RemoveAuraHolderFromStack(24659);
                    return SPELL_AURA_PROC_OK;
                }
                // Restless Strength
                case 24661:
                {
                    // Need remove one 24662 aura
                    RemoveAuraHolderFromStack(24662);
                    return SPELL_AURA_PROC_OK;
                }
                // Viscidus Frost Weakness
                // Disabled for now, handled on Viscidus script
                /*
                case 25926:
                {
                    if (!procSpell)
                        return SPELL_AURA_PROC_FAILED;

                    if (GetSchoolMask(procSpell->School) == SPELL_SCHOOL_MASK_FROST)
                    {
                        ++triggeredByAura->GetModifier()->m_amount;
                        triggerAmount = triggeredByAura->GetModifier()->m_amount;

                        if (triggerAmount == 100)
                            triggered_spell_id = 26034; // Slowed
                        else if (triggerAmount == 150)
                            triggered_spell_id = 26036; // Slowed more
                        else if (triggerAmount == 200)
                            triggered_spell_id = 25937; // Freeze
                    }

                    break;
                }
                */
                // Viscidus Freeze
                case 25937:
                {
                    if (procSpell)
                    {
                        // Wand spell entry marked as physical, we need to add an exception here
                        if (GetSchoolMask(procSpell->School) != SPELL_SCHOOL_MASK_NORMAL || procSpell->Id == 5019)
                            return SPELL_AURA_PROC_FAILED;
                    }

                    ++triggeredByAura->GetModifier()->m_amount;
                    triggerAmount = triggeredByAura->GetModifier()->m_amount;

                    if (triggerAmount == 50)
                        MonsterTextEmote(-1531044, nullptr, true); // Cracks
                    else if (triggerAmount == 100)
                        MonsterTextEmote(-1531045, nullptr, true); // Shatter
                    else if (triggerAmount == 150)
                    {
                        RemoveAurasDueToSpell(25937);
                        triggered_spell_id = 25938; // Explode
                    }

                    break;
                }
                // Adaptive Warding (Frostfire Regalia set)
                case 28764:
                {
                    if (!procSpell)
                        return SPELL_AURA_PROC_FAILED;

                    // find Mage Armor
                    bool found = false;
                    AuraList const& mRegenInterupt = GetAurasByType(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT);
                    for (const auto iter : mRegenInterupt)
                    {
                        if (SpellEntry const* iterSpellProto = iter->GetSpellProto())
                        {
                            if (iterSpellProto->IsFitToFamily<SPELLFAMILY_MAGE, CF_MAGE_MAGE_ARMOR>())
                            {
                                found = true;
                                break;
                            }
                        }
                    }
                    if (!found)
                        return SPELL_AURA_PROC_FAILED;

                    switch (GetFirstSchoolInMask(procSpell->GetSpellSchoolMask()))
                    {
                        case SPELL_SCHOOL_NORMAL:
                        case SPELL_SCHOOL_HOLY:
                            return SPELL_AURA_PROC_FAILED;                   // ignored
                        case SPELL_SCHOOL_FIRE:
                            triggered_spell_id = 28765;
                            break;
                        case SPELL_SCHOOL_NATURE:
                            triggered_spell_id = 28768;
                            break;
                        case SPELL_SCHOOL_FROST:
                            triggered_spell_id = 28766;
                            break;
                        case SPELL_SCHOOL_SHADOW:
                            triggered_spell_id = 28769;
                            break;
                        case SPELL_SCHOOL_ARCANE:
                            triggered_spell_id = 28770;
                            break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }

                    target = this;
                    break;
                }
                // Obsidian Armor (Justice Bearer`s Pauldrons shoulder)
                case 27539:
                {
                    if (!procSpell)
                        return SPELL_AURA_PROC_FAILED;

                    switch (GetFirstSchoolInMask(procSpell->GetSpellSchoolMask()))
                    {
                        case SPELL_SCHOOL_NORMAL:
                            return SPELL_AURA_PROC_FAILED;                   // ignore
                        case SPELL_SCHOOL_HOLY:
                            triggered_spell_id = 27536;
                            break;
                        case SPELL_SCHOOL_FIRE:
                            triggered_spell_id = 27533;
                            break;
                        case SPELL_SCHOOL_NATURE:
                            triggered_spell_id = 27538;
                            break;
                        case SPELL_SCHOOL_FROST:
                            triggered_spell_id = 27534;
                            break;
                        case SPELL_SCHOOL_SHADOW:
                            triggered_spell_id = 27535;
                            break;
                        case SPELL_SCHOOL_ARCANE:
                            triggered_spell_id = 27540;
                            break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }

                    target = this;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
            break;
        case SPELLFAMILY_WARRIOR:
            break;
        case SPELLFAMILY_WARLOCK:
            break;
        case SPELLFAMILY_PRIEST:
            break;
        case SPELLFAMILY_DRUID:
            break;
        case SPELLFAMILY_ROGUE:
            break;
        case SPELLFAMILY_HUNTER:
            break;
        case SPELLFAMILY_PALADIN:
            break;
        case SPELLFAMILY_SHAMAN:
            break;
        default:
            break;
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    SpellEntry const* triggerEntry = sSpellMgr.GetSpellEntry(triggered_spell_id);

    if (!triggerEntry)
    {
        sLog.outError("Unit::HandleDummyAuraProc: Spell %u have not existed triggered spell %u", dummySpell->Id, triggered_spell_id);
        return SPELL_AURA_PROC_FAILED;
    }

    // default case
    if (!target || (target != this && !target->IsAlive()))
        return SPELL_AURA_PROC_FAILED;

    if (cooldown && HasSpellCooldown(triggered_spell_id))
        return SPELL_AURA_PROC_FAILED;

    if (basepoints[EFFECT_INDEX_0] || basepoints[EFFECT_INDEX_1] || basepoints[EFFECT_INDEX_2])
        CastCustomSpell(target, triggered_spell_id,
                        basepoints[EFFECT_INDEX_0] ? &basepoints[EFFECT_INDEX_0] : nullptr,
                        basepoints[EFFECT_INDEX_1] ? &basepoints[EFFECT_INDEX_1] : nullptr,
                        basepoints[EFFECT_INDEX_2] ? &basepoints[EFFECT_INDEX_2] : nullptr,
                        true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown)
        AddSpellCooldown(triggered_spell_id, 0, time(nullptr) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleProcTriggerSpellAuraProc(Unit* pVictim, uint32 damage, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlags, uint32 procEx, uint32 cooldown)
{
    // Get triggered aura spell info
    SpellEntry const* auraSpellInfo = triggeredByAura->GetSpellProto();

    // Allow overwritting caster
    Unit* pCaster = this;

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    // Set trigger spell id, target, custom basepoints
    uint32 trigger_spell_id = auraSpellInfo->EffectTriggerSpell[triggeredByAura->GetEffIndex()];
    Unit*  target = nullptr;
    int32  basepoints[MAX_EFFECT_INDEX] = {0, 0, 0};

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    // Try handle unknown trigger spells
    // Custom requirements (not listed in procEx) Warning! damage dealing after this
    // Custom triggered spells
    switch (auraSpellInfo->SpellFamilyName)
    {
        case SPELLFAMILY_GENERIC:
            switch (auraSpellInfo->Id)
            {
                //case 5301:  break;                        // Defensive State (DND)
                //case 7137:  break;                        // Shadow Charge (Rank 1)
                //case 7377:  break;                        // Take Immune Periodic Damage <Not Working>
                //case 13358: break;                        // Defensive State (DND)
                //case 16092: break;                        // Defensive State (DND)
                //case 18943: break;                        // Double Attack
                //case 19194: break;                        // Double Attack
                //case 19817: break;                        // Double Attack
                //case 19818: break;                        // Double Attack
                //case 22835: break;                        // Drunken Rage
                //    trigger_spell_id = 14822; break;
                case 23780:                                 // Aegis of Preservation (Aegis of Preservation trinket)
                    trigger_spell_id = 23781;
                    break;
                //case 24949: break;                        // Defensive State 2 (DND)
                case 27522:                                 // Mana Drain Trigger
                {
                    // On successful melee or ranged attack gain $29471s1 mana and if possible drain $27526s1 mana from the target.
                    if (IsAlive())
                        CastSpell(this, 29471, true, castItem, triggeredByAura);
                    if (pVictim && pVictim->IsAlive())
                        CastSpell(pVictim, 27526, true, castItem, triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                }
                case 31255:                                 // Deadly Swiftness (Rank 1)
                {
                    // whenever you deal damage to a target who is below 20% health.
                    //if (pVictim->GetHealth() > pVictim->GetMaxHealth() / 5)
                        //return SPELL_AURA_PROC_FAILED;

                    target = this;
                    trigger_spell_id = 22588;
                    break;
                }
                case 28200:                                 // Talisman of Ascendance
                {
                    if (procSpell && (procSpell->IsAreaOfEffectSpell() || procSpell->Effect[0] == SPELL_EFFECT_SCRIPT_EFFECT))
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
            }
            break;
        case SPELLFAMILY_MAGE:
            break;
        case SPELLFAMILY_WARRIOR:
            break;
        case SPELLFAMILY_ROGUE:
            break;
        case SPELLFAMILY_WARLOCK:
            break;
        case SPELLFAMILY_PRIEST:
            break;
        case SPELLFAMILY_DRUID:
            break;
        case SPELLFAMILY_HUNTER:
            break;
        case SPELLFAMILY_PALADIN:
            break;
        case SPELLFAMILY_SHAMAN:
            break;
        default:
            break;
    }

    // All ok. Check current trigger spell
    SpellEntry const* triggerEntry = sSpellMgr.GetSpellEntry(trigger_spell_id);
    if (!triggerEntry)
    {
        // A malformed spell row may be evaluated every combat tick. Keep the
        // diagnostic actionable without producing hundreds of identical log
        // lines and making an already slow map update even more expensive.
        static std::mutex reportedProcMutex;
        static std::unordered_set<uint64> reportedProcPairs;
        uint64 const pairKey = (uint64(auraSpellInfo->Id) << 32) | trigger_spell_id;
        bool firstReport = false;
        {
            std::lock_guard<std::mutex> lock(reportedProcMutex);
            firstReport = reportedProcPairs.insert(pairKey).second;
        }
        if (firstReport)
            sLog.outError("Unit::HandleProcTriggerSpell: aura spell %u references missing trigger spell %u (effect %u); suppressing duplicate reports",
                          auraSpellInfo->Id, trigger_spell_id, uint32(triggeredByAura->GetEffIndex()));
        return SPELL_AURA_PROC_FAILED;
    }

    // Custom basepoints/target for exist spell
    // dummy basepoints or other customs
    switch (trigger_spell_id)
    {
        // Cast positive spell on enemy target
        case 7099:  // Curse of Mending
        case 20233: // Improved Lay on Hands (cast on target)
        {
            target = pVictim;
            break;
        }
        // Finishing moves that add combo points
        case 14189: // Seal Fate (Netherblade set)
        case 14157: // Ruthlessness
        {
            // Need add combopoint AFTER finishing move (or they get dropped in finish phase)
            if (Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL))
            {
                spell->AddTriggeredSpell(trigger_spell_id);
                return SPELL_AURA_PROC_OK;
            }
            return SPELL_AURA_PROC_FAILED;
        }
    }

    if (cooldown && HasSpellCooldown(trigger_spell_id))
        return SPELL_AURA_PROC_FAILED;

    // try detect target manually if not set
    if (target == nullptr)
        target = !(procFlags & PROC_FLAG_DEAL_HELPFUL_SPELL) && Spells::IsPositiveSpell(trigger_spell_id) ? this : pVictim;

    // default case
    if (!target || (target != this && !target->IsAlive()))
        return SPELL_AURA_PROC_FAILED;

    if (basepoints[EFFECT_INDEX_0] || basepoints[EFFECT_INDEX_1] || basepoints[EFFECT_INDEX_2])
        pCaster->CastCustomSpell(target, trigger_spell_id,
                        basepoints[EFFECT_INDEX_0] ? &basepoints[EFFECT_INDEX_0] : nullptr,
                        basepoints[EFFECT_INDEX_1] ? &basepoints[EFFECT_INDEX_1] : nullptr,
                        basepoints[EFFECT_INDEX_2] ? &basepoints[EFFECT_INDEX_2] : nullptr,
                        true, castItem, triggeredByAura);
    else
        pCaster->CastSpell(target, trigger_spell_id, true, castItem, triggeredByAura, pCaster->GetObjectGuid(), nullptr, procSpell);

    if (cooldown)
        AddSpellCooldown(trigger_spell_id, 0, time(nullptr) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleProcTriggerDamageAuraProc(Unit *pVictim, uint32 damage, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlags, uint32 procEx, uint32 cooldown)
{
    SpellEntry const *spellInfo = triggeredByAura->GetSpellProto();
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "ProcDamageAndSpell: doing %u damage from spell id %u (triggered by auratype %u of spell %u)",
                     triggeredByAura->GetModifier()->m_amount, spellInfo->Id, triggeredByAura->GetModifier()->m_auraname, triggeredByAura->GetId());
    
    // Trigger damage can be resisted...
    if (SpellMissInfo missInfo = SpellHitResult(pVictim, spellInfo, triggeredByAura->GetEffIndex(), false))
    {
        SendSpellDamageResist(pVictim, spellInfo->Id);
        return SPELL_AURA_PROC_OK;
    }

    SpellNonMeleeDamage damageInfo(this, pVictim, spellInfo->Id, SpellSchools(spellInfo->School));
    damageInfo.damage = CalculateSpellDamage(pVictim, spellInfo, triggeredByAura->GetEffIndex());
    damageInfo.damage = SpellDamageBonusDone(pVictim, spellInfo, triggeredByAura->GetEffIndex(), damageInfo.damage, SPELL_DIRECT_DAMAGE);
    damageInfo.damage = pVictim->SpellDamageBonusTaken(this, spellInfo, triggeredByAura->GetEffIndex(), damageInfo.damage, SPELL_DIRECT_DAMAGE);
    damageInfo.target->CalculateAbsorbResistBlock(this, &damageInfo, spellInfo);
    DealDamageMods(damageInfo.target, damageInfo.damage, &damageInfo.absorb);
    SendSpellNonMeleeDamageLog(&damageInfo);
    DealSpellDamage(&damageInfo, true);
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleOverrideClassScriptAuraProc(Unit *pVictim, uint32 /*damage*/, int32 /*originalAmount*/, Aura *triggeredByAura, SpellEntry const *procSpell, uint32 /*procFlag*/, uint32 /*procEx*/ , uint32 cooldown)
{
    int32 scriptId = triggeredByAura->GetModifier()->m_miscvalue;

    if (!pVictim || !pVictim->IsAlive())
        return SPELL_AURA_PROC_FAILED;

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId() == TYPEID_PLAYER
                     ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : nullptr;

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    uint32 triggered_spell_id = 0;

    switch (scriptId)
    {
        case 4309: // Crepuscule
        {
            triggered_spell_id = 17941;
            break;
        }
    }

    // not processed
    if (!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    // standard non-dummy case
    SpellEntry const* triggerEntry = sSpellMgr.GetSpellEntry(triggered_spell_id);

    if (!triggerEntry || triggered_spell_id == 18350)
    {
        sLog.outError("Unit::HandleOverrideClassScriptAuraProc: Spell %u triggering for class script id %u", triggered_spell_id, scriptId);
        return SPELL_AURA_PROC_FAILED;
    }

    if (cooldown && HasSpellCooldown(triggered_spell_id))
        return SPELL_AURA_PROC_FAILED;

    CastSpell(pVictim, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown)
        AddSpellCooldown(triggered_spell_id, 0, time(nullptr) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModCastingSpeedNotStackAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* /*triggeredByAura*/, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // Skip melee hits or instant cast spells
    return !(procSpell == nullptr || procSpell->GetCastTime(this) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleReflectSpellsSchoolAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // Skip Melee hits and spells ws wrong school
    return !(procSpell == nullptr || (triggeredByAura->GetModifier()->m_miscvalue & GetSchoolMask(procSpell->School)) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleModPowerCostSchoolAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // Skip melee hits and spells ws wrong school or zero cost
    return !(procSpell == nullptr ||
             (procSpell->manaCost == 0 && procSpell->ManaCostPercentage == 0) ||           // Cost check
             (triggeredByAura->GetModifier()->m_miscvalue & GetSchoolMask(procSpell->School)) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;  // School check
}

SpellAuraProcResult Unit::HandleMechanicImmuneResistanceAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // Compare mechanic
    return !(procSpell == nullptr || procSpell->Mechanic != triggeredByAura->GetModifier()->m_miscvalue)
           ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

// Nostalrius
SpellAuraProcResult Unit::HandleAddTargetTriggerAuraProc(Unit *pVictim, uint32 /*damage*/, int32 /*originalAmount*/, Aura *triggeredByAura, SpellEntry const *procSpell, uint32 /*procFlag*/, uint32 /*procEx*/ , uint32 cooldown)
{
    const SpellEntry* aurEntry = triggeredByAura->GetSpellProto();
    uint32 trigger_spell_id = aurEntry->EffectTriggerSpell[0];
    float chance = 0.0f;

    if (aurEntry->EffectBasePoints[0] != -1)
        chance = aurEntry->EffectBasePoints[0];
    else
        sLog.outError("Spell %u has chance = -1 but not handled in core ...", aurEntry->Id);

    // Si il y a plusieurs ticks ...
    // Blizzard - 8 ticks (fix procs abuses de morsure de givre)
    if (aurEntry->IsFitToFamily<SPELLFAMILY_MAGE, CF_MAGE_BLIZZARD>())
        chance /= 8;

    // Chance = -1 -> A calculer manuellement
    if (!roll_chance_f(chance))
        return SPELL_AURA_PROC_FAILED;

    if (trigger_spell_id)
    {
        bool bTarget = true;
        switch (aurEntry->Id)
        {
            case 17768: // Casque tete de loup
            case 24392: // Gelee soudaine
            case 14179: // Frappes implacables
                bTarget = false;
                break;
        }

        if (bTarget)
        {
            // Cone de glace
            if (pVictim->GetGUID() == GetGUID())
                return SPELL_AURA_PROC_FAILED;
            pVictim->CastSpell(pVictim, trigger_spell_id, true, nullptr, triggeredByAura);
        }
        else
            CastSpell(this, trigger_spell_id, true, nullptr, triggeredByAura);
    }
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModResistanceAuraProc(Unit* /*pVictim*/, uint32 damage, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    SpellEntry const *spellInfo = triggeredByAura->GetSpellProto();

    // Inner Fire
    if (spellInfo->IsFitToFamily<SPELLFAMILY_PRIEST, CF_PRIEST_INNER_FIRE>())
    {
        // only at real damage
        if (!damage)
            return SPELL_AURA_PROC_FAILED;
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModDamageAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // the aura school mask must match the spell school
    return (procSpell == nullptr || (GetSchoolMask(procSpell->School) & triggeredByAura->GetModifier()->m_miscvalue))
        ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleModRageFromDamageDealtAuraProc(Unit* pVictim, uint32 damage, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 procFlag, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    if (!damage || !pVictim || pVictim == this || GetPowerType() != POWER_RAGE || !IsPlayer())
        return SPELL_AURA_PROC_FAILED;

    if (!(procFlag & (PROC_FLAG_TAKE_MELEE_SWING | PROC_FLAG_TAKE_MELEE_ABILITY | PROC_FLAG_TAKE_RANGED_ATTACK |
                      PROC_FLAG_TAKE_RANGED_ABILITY | PROC_FLAG_TAKE_HARMFUL_ABILITY | PROC_FLAG_TAKE_HARMFUL_SPELL |
                      PROC_FLAG_TAKE_HARMFUL_PERIODIC | PROC_FLAG_TAKEN_ANY_DAMAGE)))
        return SPELL_AURA_PROC_FAILED;

    int32 const percent = triggeredByAura->GetModifier()->m_amount;
    if (percent <= 0)
        return SPELL_AURA_PROC_FAILED;

    static_cast<Player*>(this)->RewardRage(damage * uint32(percent) / 100, false);
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleRemoveByDamageChanceProc(Unit* pVictim, uint32 damage, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    // The chance to dispel an aura depends on the damage taken with respect to the casters level.
    uint32 max_dmg = GetLevel() > 8 ? 25 * GetLevel() - 150 : 50;
    float chance = float(damage) / max_dmg * 100.0f;
    if (roll_chance_f(chance))
    {
        triggeredByAura->SetInUse(true);
        RemoveAurasByCasterSpell(triggeredByAura->GetId(), triggeredByAura->GetCasterGuid());
        triggeredByAura->SetInUse(false);
        return SPELL_AURA_PROC_OK;
    }

    return SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleInvisibilityAuraProc(Unit* pVictim, uint32 damage, int32 /*originalAmount*/, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    if (triggeredByAura->GetSpellProto()->HasAttribute(SPELL_ATTR_PASSIVE) || !triggeredByAura->GetSpellProto()->IsPositiveSpell())
        return SPELL_AURA_PROC_FAILED;

    RemoveAurasDueToSpell(triggeredByAura->GetId());
    return SPELL_AURA_PROC_OK;
}
