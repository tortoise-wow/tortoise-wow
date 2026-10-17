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

#ifndef MANGOS_CREATUREAI_H
#define MANGOS_CREATUREAI_H

#include "Common.h"
#include "Platform/Define.h"
#include "Policies/Singleton.h"
#include "Dynamic/ObjectRegistry.h"
#include "Dynamic/FactoryHolder.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"

#include "Utilities/EventMap.h"

class WorldObject;
class GameObject;
class Unit;
class Creature;
class Player;
class SpellEntry;
class ChatHandler;
struct Loot;

#define TIME_INTERVAL_LOOK   5000
#define VISIBILITY_RANGE    10000

enum CanCastResult
{
    CAST_OK                     = 0,
    CAST_FAIL_IS_CASTING        = 1,
    CAST_FAIL_OTHER             = 2,
    CAST_FAIL_TOO_FAR           = 3,
    CAST_FAIL_TOO_CLOSE         = 4,
    CAST_FAIL_POWER             = 5,
    CAST_FAIL_STATE             = 6,
    CAST_FAIL_TARGET_AURA       = 7,
    CAST_FAIL_NOT_IN_LOS        = 8
};

struct CreatureAISpellsEntry : CreatureSpellsEntry
{
    uint32 cooldown;
    CreatureAISpellsEntry(const CreatureSpellsEntry& EntryStruct) : CreatureSpellsEntry(EntryStruct), cooldown(urand(EntryStruct.delayInitialMin, EntryStruct.delayInitialMax)) {}
};

// Selection method used by SelectTarget
enum class SelectTargetMethod
{
    Random,      // just pick a random target
    MaxThreat,   // prefer targets higher in the threat list
    MinThreat,   // prefer targets lower in the threat list
    MaxDistance, // prefer targets further from us
    MinDistance  // prefer targets closer to us
};

template<class ArgumentType, class ResultType>
struct unary_function
{
    using argument_type = ArgumentType;
    using result_type = ResultType;
};

class ObjectDistanceOrderPred
{
public:
    ObjectDistanceOrderPred(const WorldObject* pRefObj, bool ascending = true) : m_refObj(pRefObj), m_ascending(ascending) {}
    bool operator()(const WorldObject* pLeft, const WorldObject* pRight) const
    {
        return m_ascending ? m_refObj->GetDistanceOrder(pLeft, pRight) : !m_refObj->GetDistanceOrder(pLeft, pRight);
    }
private:
    const WorldObject* m_refObj;
    const bool m_ascending;
};

struct DefaultTargetSelector : public unary_function<Unit*, bool>
{
    const Unit* me;
    float m_dist;
    bool m_playerOnly;
    int32 m_aura;

    // unit: the reference unit
    // dist: if 0: ignored, if > 0: maximum distance to the reference unit, if < 0: minimum distance to the reference unit
    // playerOnly: self explaining
    // aura: if 0: ignored, if > 0: the target shall have the aura, if < 0, the target shall NOT have the aura
    DefaultTargetSelector(Unit const* unit, float dist, bool playerOnly, int32 aura) : me(unit), m_dist(dist), m_playerOnly(playerOnly), m_aura(aura) {}

    bool operator()(Unit const* target) const
    {
        if (!me)
            return false;

        if (!target)
            return false;

        if (m_playerOnly && (target->GetTypeId() != TYPEID_PLAYER))
            return false;

        if (m_dist > 0.0f && !me->IsWithinCombatDistInMap(target, m_dist))
            return false;

        if (m_dist < 0.0f && me->IsWithinCombatDistInMap(target, -m_dist))
            return false;

        if (m_aura)
        {
            if (m_aura > 0)
            {
                if (!target->HasAura(m_aura))
                    return false;
            }
            else
            {
                if (target->HasAura(-m_aura))
                    return false;
            }
        }

        return true;
    }
};

// Creature spell lists should be updated every 1.2 seconds according to research.
// https://www.reddit.com/r/wowservers/comments/834nt5/felmyst_ai_system_research/
#define CREATURE_CASTING_DELAY 1200

class CreatureAI
{
    public:
        explicit CreatureAI(Creature* creature) : m_creature(creature), m_bUseAiAtControl(false), m_bMeleeAttack(true), m_bCombatMovement(true), m_uiCastingDelay(CREATURE_CASTING_DELAY), m_uLastAlertTime(0)
        {
            SetSpellsList(creature->GetCreatureInfo()->spell_list_id);
        }

        virtual ~CreatureAI();
        virtual void OnRemoveFromWorld() {}
        // An AI made before its creature entered the map (a summon: TemporarySummon::Summon) hears it enter here,
        // from Unit::AddToWorld before its instant AI notify (ScheduleAINotify(0)) can start an aggro on sight.
        virtual void OnAddToWorld() {}

        virtual uint32 GetData(uint32 /*type*/) { return 0; }

        virtual void InformGuid(const ObjectGuid /*guid*/, uint32 /*type*/=0) {}
        virtual void DoAction(const uint32 /*type*/=0) {}
        virtual void DoAction(Unit* /*pUnit*/, uint32 /*type*/) {}

        ///== Information about AI ========================
        virtual void GetAIInformation(ChatHandler& /*reader*/) {}

        ///== Reactions At =================================

        // Called when an unit moves within visibility distance
        virtual void MoveInLineOfSight(Unit*) {}

        // Called for reaction at enter to combat if not in combat yet (enemy can be nullptr)
        virtual void EnterCombat(Unit* /*enemy*/) {}

        // Called for reaction at stopping attack at no attackers or targets
        virtual void EnterEvadeMode();

        // Called when leaving combat
        virtual void OnCombatStop();

        // Called at reaching home after evade
        virtual void JustReachedHome() {}

        // Called at any heal cast/item used (call non implemented)
        virtual void HealedBy(Unit* /*healer*/, uint32& /*amount_healed*/) {}

        // Called at any Damage to any victim (before damage apply)
        virtual void DamageDeal(Unit* /*done_to*/, uint32 & /*damage*/) {}

        // Called at any Damage from any attacker (before damage apply)
        // Note: it for recalculation damage or special reaction at damage
        // for attack reaction use AttackedBy called for not DOT damage in Unit::DealDamage also
        virtual void DamageTaken(Unit* /*done_by*/, uint32 & /*damage*/) {}

        // Called when the creature is killed
        virtual void JustDied(Unit*) {}

        // Called when the creature summon is killed
        virtual void SummonedCreatureJustDied(Creature* /*unit*/) {}

        // Called when a member of the creature's group is killed
        virtual void GroupMemberJustDied(Creature* /*unit*/, bool /*isLeader*/) {}

        // Called when the creature kills a unit
        virtual void KilledUnit(Unit*) {}

        // Called when owner of m_creature (if m_creature is PROTECTOR_PET) kills a unit
        virtual void OwnerKilledUnit(Unit*) {}

        // Called when the creature summon successfully other creature
        virtual void JustSummoned(Creature*) {}

        // Called when the creature summon successfully a gameobject
        virtual void JustSummoned(GameObject*) {}

        // Called when the creature summon despawn
        virtual void SummonedCreatureDespawn(Creature* /*unit*/) {}

        // Called when hit by a spell
        virtual void SpellHit(WorldObject*, const SpellEntry*) {}

        // Called when spell hits creature's target
        virtual void SpellHitTarget(Unit*, const SpellEntry*) {}

        // Called when the creature is target of hostile action: swing, hostile spell landed, fear/etc)
        virtual void AttackedBy(Unit* attacker);

        // Called when creature is spawned or respawned (for reseting variables)
        virtual void JustRespawned();

        // Called at waypoint reached or point movement finished
        virtual void MovementInform(uint32 /*MovementType*/, uint32 /*Data*/) {}

        // Called if a temporary summoned of m_creature reach a move point
        virtual void SummonedMovementInform(Creature* /*summoned*/, uint32 /*motion_type*/, uint32 /*point_id*/) {}

        // Called at text emote receive from player
        virtual void ReceiveEmote(Player* /*pPlayer*/, uint32 /*text_emote*/) {}

        // Called only for pets
        virtual void OwnerAttackedBy(Unit* /*attacker*/) {}
        virtual void OwnerAttacked(Unit* /*target*/) {}

        ///== Triggered Actions Requested ==================

        // Called when creature attack expected (if creature can and no have current victim)
        // Note: for reaction at hostile action must be called AttackedBy function.
        virtual void AttackStart(Unit*) {}

        // Called at World update tick
        virtual void UpdateAI(const uint32 /*diff*/) {}

        // Like UpdateAI, but only when the creature is a dead corpse
        virtual void UpdateAI_corpse(const uint32 /*uiDiff*/) {}

        // Called by another script
        virtual void OnScriptEventHappened(uint32 /*uiEvent*/, uint32 /*uiData*/, WorldObject* /*pInvoker*/) {};

        ///== State checks =================================

        // called when the corpse of this creature gets removed
        virtual void CorpseRemoved(uint32 & /*respawnDelay*/) {}

        // Is corpse looting allowed ?
        virtual bool CanBeLooted() const { return true; }

        // Called when filling loot table
        virtual bool FillLoot(Loot* loot, Player* looter) const { return false; }

        // Does creature chase its target.
        bool IsCombatMovementEnabled() const { return m_bCombatMovement; }

        // Does the creature melee attack.
        bool IsMeleeAttackEnabled() const { return m_bMeleeAttack; }

        // Triggers an alert when a Unit moves near stealth detection range.
        virtual void OnMoveInStealth(Unit* who);

        // TrinityCore
        void DoCast(Unit* victim, uint32 spellId, bool triggered = false);
        void DoCastAOE(uint32 spellId, bool triggered = false);
        bool UpdateVictim();
        bool UpdateVictimWithGaze();
        void SetGazeOn(Unit* target);


        // Select the best target (in <targetType> order) from the threat list that fulfill the following:
   // - Not among the first <offset> entries in <targetType> order (or SelectTargetMethod::MaxThreat order,
   //   if <targetType> is SelectTargetMethod::Random).
   // - Within at most <dist> yards (if dist > 0.0f)
   // - At least -<dist> yards away (if dist < 0.0f)
   // - Is a player (if playerOnly = true)
   // - Not the current tank (if withTank = false)
   // - Has aura with ID <aura> (if aura > 0)
   // - Does not have aura with ID -<aura> (if aura < 0)
        Unit* SelectTarget(SelectTargetMethod targetType, uint32 position = 0, float dist = 0.0f, bool playerOnly = false, int32 aura = 0);

        // Select the best target (in <targetType> order) satisfying <predicate> from the threat list.
        // If <offset> is nonzero, the first <offset> entries in <targetType> order (or SelectTargetMethod::MaxThreat
        // order, if <targetType> is SelectTargetMethod::Random) are skipped.
        template <class PREDICATE>
        Unit* SelectTarget(SelectTargetMethod targetType, uint32 position, PREDICATE const& predicate)
        {
            auto const& threatlist = m_creature->GetThreatManager().getThreatList();
            if (position >= threatlist.size())
                return nullptr;

            std::list<Unit*> targetList;
            for (auto itr : threatlist)
            {
				if (predicate(itr->getTarget()))
					targetList.push_back(itr->getTarget());
            }

            if (position >= targetList.size())
                return nullptr;

            if (targetType == SelectTargetMethod::MaxDistance || targetType == SelectTargetMethod::MinDistance)
                targetList.sort(ObjectDistanceOrderPred(m_creature));

            switch (targetType)
            {
            case SelectTargetMethod::MaxDistance:
            case SelectTargetMethod::MaxThreat:
            {
                std::list<Unit*>::iterator itr = targetList.begin();
                std::advance(itr, position);
                return *itr;
            }
            case SelectTargetMethod::MinDistance:
            case SelectTargetMethod::MinThreat:
            {
                std::list<Unit*>::reverse_iterator ritr = targetList.rbegin();
                std::advance(ritr, position);
                return *ritr;
            }
            case SelectTargetMethod::Random:
            {
                std::list<Unit*>::iterator itr = targetList.begin();
                std::advance(itr, urand(position, targetList.size() - 1));
                return *itr;
            }
            default:
                break;
            }

            return nullptr;
        }


        ///== Helper functions =============================

        // Will auto attack if the swing timer is ready.
        bool DoMeleeAttackIfReady();

        // Attempts to cast a spell and returns the result.
        CanCastResult DoCastSpellIfCan(Unit* pTarget, uint32 uiSpell, uint32 uiCastFlags = 0, ObjectGuid uiOriginalCasterGUID = ObjectGuid());

        // Helper functions for cast spell
        virtual CanCastResult CanCastSpell(Unit* pTarget, const SpellEntry* pSpell, bool isTriggered, bool bCanIgnoreLOS = false);

        // Clears any group/raid icons this creature may have
        void ClearTargetIcon();

        // Assigns a creature_spells list to the AI.
        void SetSpellsList(uint32 entry);
        void SetSpellsList(const CreatureSpellsList* pSpellsList);

        // Goes through the creature's spells list to update timers and cast spells.
        void UpdateSpellsList(const uint32 uiDiff);
        void DoSpellsListCasts(const uint32 uiDiff);

        // Enables or disables melee attacks.
        void SetMeleeAttack(bool enabled);

        // Enables or disabled combat movement.
        void SetCombatMovement(bool enabled);
        
        // Pointer to controlled by AI creature
        Creature* const m_creature;
        bool SwitchAiAtControl() const { return !m_bUseAiAtControl; }
        void SetUseAiAtControl(bool v) { m_bUseAiAtControl = v; }

        bool IsCombatMovement() { return m_bCombatMovement; }
    protected:
        bool CanTriggerAlert(Unit const* who);
        void TriggerAlertDirect(Unit const* who);
        ///== Fields =======================================
        bool   m_bUseAiAtControl;
        bool   m_bMeleeAttack;                                  // If we allow melee auto attack
        bool   m_bCombatMovement;                               // If we allow targeted movement gen (chasing target)
        uint32 m_uiCastingDelay;                                // Milliseconds elapsed since last spell list update
        uint32 m_uLastAlertTime;
        turtle_vector<CreatureAISpellsEntry, Category_AI> m_CreatureSpells;    // Contains the currently used creature_spells template
};

struct SelectableAI : FactoryHolder<CreatureAI>, Permissible<Creature>
{
    explicit SelectableAI(const char* id) : FactoryHolder<CreatureAI>(id) {}
};

template<class REAL_AI>
struct CreatureAIFactory : SelectableAI
{
    explicit CreatureAIFactory(const char* name) : SelectableAI(name) {}

    CreatureAI* Create(void*) const override;

    int Permit(const Creature* c) const override { return REAL_AI::Permissible(c); }
};

enum Permitions
{
    PERMIT_BASE_NO                 = -1,
    PERMIT_BASE_IDLE               = 1,
    PERMIT_BASE_REACTIVE           = 100,
    PERMIT_BASE_PROACTIVE          = 200,
    PERMIT_BASE_FACTION_SPECIFIC   = 400,
    PERMIT_BASE_SPECIAL            = 800
};

using CreatureAICreator = FactoryHolder<CreatureAI>;
using CreatureAIRegistry = FactoryHolder<CreatureAI>::FactoryHolderRegistry;
using CreatureAIRepository = FactoryHolder<CreatureAI>::FactoryHolderRepository;
#endif
