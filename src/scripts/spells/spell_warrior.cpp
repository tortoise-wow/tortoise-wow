#include "scriptPCH.h"
#include "Item.h"
#include "Player.h"
#include "SpellClassMask.h"

namespace
{
enum WarriorSpells
{
    SPELL_WARRIOR_DEEP_WOUND                   = 12721,
    SPELL_WARRIOR_DEEP_WOUNDS_R1               = 12162,
    SPELL_WARRIOR_DEEP_WOUNDS_R2               = 12850,
    SPELL_WARRIOR_DEEP_WOUNDS_R3               = 12868,
    SPELL_WARRIOR_SHIELD_SPECIALIZATION_R1     = 12298,
    SPELL_WARRIOR_SHIELD_SPECIALIZATION_R2     = 12724,
    SPELL_WARRIOR_SHIELD_SPECIALIZATION_R3     = 12725,
    SPELL_WARRIOR_SHIELD_SPECIALIZATION_R4     = 12726,
    SPELL_WARRIOR_SHIELD_SPECIALIZATION_R5     = 12727,
    SPELL_WARRIOR_SHIELD_SPECIALIZATION_TRIGGER = 23602,
    SPELL_WARRIOR_SLAM_R1                      = 1464,
    SPELL_WARRIOR_GAG_ORDER_R1                 = 12311,
    SPELL_WARRIOR_GAG_ORDER_R2                 = 12958,
    SPELL_WARRIOR_LAST_STAND_TRIGGER           = 12976,
    SPELL_WARRIOR_DEATH_WISH                   = 12328,
    SPELL_WARRIOR_RECKLESSNESS                 = 1719,
    SPELL_WARRIOR_SWEEPING_STRIKES_TRIGGER     = 12723,
    SPELL_WARRIOR_ENRAGE_R1                    = 12880,
    SPELL_WARRIOR_ENRAGE_R2                    = 14201,
    SPELL_WARRIOR_ENRAGE_R3                    = 14202,
    SPELL_WARRIOR_ENRAGE_R4                    = 14203,
    SPELL_WARRIOR_ENRAGE_R5                    = 14204,
    SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK = 26654,
    SPELL_WARRIOR_RETALIATION_TRIGGER          = 22858,
    SPELL_WARRIOR_WHIRLWIND                    = 1680,
    SPELL_WARRIOR_EXECUTE_TRIGGER              = 20647,
    SPELL_WARRIOR_WARRIORS_WRATH_TRIGGER       = 21887,
    SPELL_WARRIOR_REPRISAL_R1                  = 51593,
    SPELL_WARRIOR_REPRISAL_R2                  = 51594,
    SPELL_WARRIOR_REPRISAL_REFUND              = 51595,
    SPELL_WARRIOR_BLOOD_DRINKER_HEAL           = 51619,
    SPELL_WARRIOR_MASTER_STRIKE_MACE           = 54016,
    SPELL_WARRIOR_MASTER_STRIKE_SWORD          = 54017,
    SPELL_WARRIOR_MASTER_STRIKE_AXE            = 54018,
    SPELL_WARRIOR_MASTER_STRIKE_POLEARM        = 54019,
    SPELL_WARRIOR_MASTER_STRIKE_FIST_WEAPON    = 54020,
    SPELL_WARRIOR_MASTER_STRIKE_STAFF          = 54021,
    SPELL_WARRIOR_MASTER_STRIKE_DAGGER         = 54022,
};

constexpr int32 RAGE_POWER_PER_RAGE = 10;

template <class T>
SpellScript* GetSpellScript(SpellEntry const*)
{
    return new T();
}

template <class T>
AuraScript* GetAuraScript(SpellEntry const*)
{
    return new T();
}

void RegisterSpellScript(char const* name, SpellScript* (*getter)(SpellEntry const*))
{
    Script* script = new Script;
    script->Name = name;
    script->GetSpellScript = getter;
    script->RegisterSelf();
}

void RegisterAuraScript(char const* name, AuraScript* (*getter)(SpellEntry const*))
{
    Script* script = new Script;
    script->Name = name;
    script->GetAuraScript = getter;
    script->RegisterSelf();
}

struct spell_warrior_bloodthirst : public SpellScript
{
    void OnEffectDamageCalculate(Spell* spell, SpellEffectIndex effIdx, float& damage) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit)
            return;

        float attackPower = spell->m_casterUnit->GetTotalAttackPowerValue(BASE_ATTACK);
        if (Unit* target = spell->GetUnitTarget())
            attackPower += spell->m_casterUnit->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS, target->GetCreatureTypeMask());

        damage += attackPower * 0.35f;
    }
};

struct spell_warrior_shield_slam : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit)
            return true;

        int32 dispelChance = 50;

        if (Player* player = spell->m_casterUnit->ToPlayer())
        {
            int32 extraChance = 0;
            if (player->HasAura(SPELL_WARRIOR_GAG_ORDER_R2))
                extraChance = 100;
            else if (player->HasAura(SPELL_WARRIOR_GAG_ORDER_R1))
                extraChance = 50;

            dispelChance += (100 - dispelChance) * extraChance / 100;
        }

        return roll_chance_i(dispelChance);
    }

    void OnEffectDamageCalculate(Spell* spell, SpellEffectIndex /*effIdx*/, float& damage) const override
    {
        if (spell->m_casterUnit)
            damage += int32(spell->m_casterUnit->GetShieldBlockValue());
    }
};

struct spell_warrior_shield_specialization : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit)
            return true;

        Player* player = spell->m_casterUnit->ToPlayer();
        if (!player)
            return true;

        static constexpr uint32 spellRanks[] =
        {
            SPELL_WARRIOR_SHIELD_SPECIALIZATION_R1,
            SPELL_WARRIOR_SHIELD_SPECIALIZATION_R2,
            SPELL_WARRIOR_SHIELD_SPECIALIZATION_R3,
            SPELL_WARRIOR_SHIELD_SPECIALIZATION_R4,
            SPELL_WARRIOR_SHIELD_SPECIALIZATION_R5,
        };

        SpellEntry const* triggerSpell = sSpellMgr.GetSpellEntry(SPELL_WARRIOR_SHIELD_SPECIALIZATION_TRIGGER);
        if (!triggerSpell)
            return true;

        for (int32 rank = int32(sizeof(spellRanks) / sizeof(spellRanks[0])) - 1; rank >= 0; --rank)
        {
            if (player->HasAura(spellRanks[rank]) || player->HasSpell(spellRanks[rank]))
            {
                SpellEntry const* talentSpell = sSpellMgr.GetSpellEntry(spellRanks[rank]);
                if (talentSpell)
                    spell->damage = (talentSpell->EffectBasePoints[EFFECT_INDEX_1] + triggerSpell->EffectDieSides[effIdx]) * RAGE_POWER_PER_RAGE;

                break;
            }
        }

        return true;
    }
};

struct spell_warrior_flurry : public AuraScript
{
    SpellModifier* m_slamCastTimeMod = nullptr;

    void OnAfterApply(Aura* aura, bool apply) override
    {
        if (aura->GetEffIndex() != EFFECT_INDEX_1 || aura->GetModifier()->m_auraname != SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK)
            return;

        Player* player = aura->GetTarget()->ToPlayer();
        if (!player)
            return;

        int32 amount = aura->GetModifier()->m_amount;

        aura->GetTarget()->ApplyCastTimePercentMod(amount, !apply);

        if (apply)
        {
            if (m_slamCastTimeMod)
                return;

            SpellEntry const* slam = sSpellMgr.GetSpellEntry(SPELL_WARRIOR_SLAM_R1);
            if (!slam)
                return;

            int32 const slamCastTime = slam->GetCastTime(nullptr);
            if (slamCastTime <= 0)
                return;

            int32 const hastedSlamCastTime = slamCastTime * 100 / (100 + amount);
            int32 const slamCastTimeReduction = slamCastTime - hastedSlamCastTime;
            if (slamCastTimeReduction <= 0)
                return;

            m_slamCastTimeMod = new SpellModifier(
                SPELLMOD_CASTING_TIME,
                SPELLMOD_FLAT,
                -slamCastTimeReduction,
                aura->GetId(),
                UI64LIT(1) << CF_WARRIOR_SLAM);

            player->AddSpellMod(m_slamCastTimeMod, true);
        }
        else if (m_slamCastTimeMod)
        {
            player->AddSpellMod(m_slamCastTimeMod, false);
            m_slamCastTimeMod = nullptr;
        }
    }

    std::optional<SpellAuraProcResult> OnProc(Unit* /*owner*/, Unit* /*victim*/, uint32 /*amount*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (aura->GetEffIndex() == EFFECT_INDEX_1 && aura->GetModifier()->m_auraname == SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK)
            return SPELL_AURA_PROC_CANT_TRIGGER;

        return std::nullopt;
    }
};

struct spell_warrior_execute_trigger : public SpellScript
{
    void OnEffectDamageCalculate(Spell* spell, SpellEffectIndex /*effIdx*/, float& /*damage*/) const override
    {
        if (spell->m_casterUnit)
            spell->m_casterUnit->SetPower(POWER_RAGE, 0);
    }
};

struct spell_warrior_execute : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return true;

        Unit* target = spell->GetUnitTarget();
        if (!target || !spell->m_casterUnit)
            return false;

        int32 extraRageDamage = int32(spell->m_casterUnit->GetPower(POWER_RAGE) * spell->m_spellInfo->DmgMultiplier[effIdx]);
        if (Player* player = spell->m_casterUnit->ToPlayer())
            player->ApplySpellMod(spell->m_spellInfo->Id, SPELLMOD_EFFECT_PAST_FIRST, extraRageDamage, spell);

        int32 basePoints0 = spell->damage + extraRageDamage;
        spell->m_casterUnit->CastCustomSpell(target, SPELL_WARRIOR_EXECUTE_TRIGGER, &basePoints0, nullptr, nullptr, true, nullptr);
        return false;
    }
};

struct spell_warrior_warriors_wrath : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return true;

        if (Unit* target = spell->GetUnitTarget())
            spell->m_caster->CastSpell(target, SPELL_WARRIOR_WARRIORS_WRATH_TRIGGER, true);

        return false;
    }
};

struct spell_warrior_deep_wounds : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target || !spell->m_casterUnit)
            return false;

        float damage;
        if (spell->m_casterUnit->HaveOffhandWeapon() && spell->m_casterUnit->GetAttackTimer(BASE_ATTACK) > spell->m_casterUnit->GetAttackTimer(OFF_ATTACK))
            damage = (spell->m_casterUnit->GetFloatValue(UNIT_FIELD_MINOFFHANDDAMAGE) + spell->m_casterUnit->GetFloatValue(UNIT_FIELD_MAXOFFHANDDAMAGE)) / 2;
        else
            damage = (spell->m_casterUnit->GetFloatValue(UNIT_FIELD_MINDAMAGE) + spell->m_casterUnit->GetFloatValue(UNIT_FIELD_MAXDAMAGE)) / 2;

        switch (spell->m_spellInfo->Id)
        {
            case SPELL_WARRIOR_DEEP_WOUNDS_R1: damage *= 0.2f; break;
            case SPELL_WARRIOR_DEEP_WOUNDS_R2: damage *= 0.4f; break;
            case SPELL_WARRIOR_DEEP_WOUNDS_R3: damage *= 0.6f; break;
        }

        int32 deepWoundsDotBasePoints0 = int32(damage / 4);
        spell->m_casterUnit->CastCustomSpell(target, SPELL_WARRIOR_DEEP_WOUND, &deepWoundsDotBasePoints0, nullptr, nullptr, true, nullptr);
        return false;
    }
};

struct spell_warrior_last_stand : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        if (spell->m_casterUnit)
        {
            int32 healthModSpellBasePoints0 = int32(spell->m_casterUnit->GetMaxHealth() * 0.3f);
            spell->m_casterUnit->CastCustomSpell(spell->m_casterUnit, SPELL_WARRIOR_LAST_STAND_TRIGGER, &healthModSpellBasePoints0, nullptr, nullptr, true, nullptr);
        }

        return false;
    }
};

struct spell_warrior_bloodrage : public SpellScript
{
    void OnEffectDamageCalculate(Spell* spell, SpellEffectIndex effIdx, float& damage) const override
    {
        if (effIdx != EFFECT_INDEX_2 || !spell->m_casterUnit)
            return;

        damage = spell->m_casterUnit->GetCreateHealth() * 0.2f;
    }

    void OnBeforeProc(Spell* spell, Unit* target, SpellMissInfo missInfo, uint32& /*procAttacker*/, uint32& /*procVictim*/, uint32& procEx, bool& /*triggerWeaponProcs*/) const override
    {
        if (missInfo != SPELL_MISS_NONE || !spell->m_casterUnit || target != spell->m_casterUnit)
            return;

        procEx &= ~PROC_EX_NORMAL_HIT;
        procEx |= PROC_EX_CRITICAL_HIT;
    }

    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
            if (Unit* target = spell->GetUnitTarget())
                target->SetInCombatState();

        return true;
    }
};

struct spell_warrior_unbridled_wrath : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit)
            return true;

        Player* player = spell->m_casterUnit->ToPlayer();
        if (player && player->IsTwoHandUsed())
            spell->damage *= 2;

        return true;
    }
};

struct spell_warrior_master_strike : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        // Weapon-specific effects are dispatched after the damage effect below.
        // The native template's empty secondary trigger is its script placeholder.
        // Preserve any real trigger supplied by a subsequent content update.
        return effIdx != EFFECT_INDEX_1 ||
            spell->m_spellInfo->Effect[effIdx] != SPELL_EFFECT_TRIGGER_SPELL ||
            spell->m_spellInfo->EffectTriggerSpell[effIdx] != 0;
    }

    void OnEffectExecuted(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit)
            return;

        Player* player = spell->m_casterUnit->ToPlayer();
        Unit* target = spell->GetUnitTarget();
        if (!player || !target)
            return;

        Item* mainHand = player->GetWeaponForAttack(BASE_ATTACK, true, true);
        if (!mainHand)
            return;

        static constexpr uint32 helperSpells[] =
        {
            SPELL_WARRIOR_MASTER_STRIKE_MACE,
            SPELL_WARRIOR_MASTER_STRIKE_SWORD,
            SPELL_WARRIOR_MASTER_STRIKE_AXE,
            SPELL_WARRIOR_MASTER_STRIKE_POLEARM,
            SPELL_WARRIOR_MASTER_STRIKE_FIST_WEAPON,
            SPELL_WARRIOR_MASTER_STRIKE_STAFF,
            SPELL_WARRIOR_MASTER_STRIKE_DAGGER,
        };

        for (uint32 helperSpellId : helperSpells)
        {
            SpellEntry const* helperSpell = sSpellMgr.GetSpellEntry(helperSpellId);
            if (!helperSpell || !mainHand->IsFitToSpellRequirements(helperSpell))
                continue;

            player->CastSpell(helperSpell->IsPositiveSpell() ? player : target, helperSpellId, true);
            return;
        }
    }
};

struct spell_warrior_master_strike_polearm : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx == EFFECT_INDEX_0)
        {
            Unit* target = spell->GetUnitTarget();
            m_targetWasMounted = target && target->IsMounted();
        }

        return m_targetWasMounted;
    }

    mutable bool m_targetWasMounted = false;
};

struct spell_warrior_revenge : public SpellScript
{
    void OnEffectExecuted(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit)
            return;

        Player* player = spell->m_casterUnit->ToPlayer();
        if (!player)
            return;

        uint32 refundChance = 0;
        if (player->HasAura(SPELL_WARRIOR_REPRISAL_R2))
            refundChance = 100;
        else if (player->HasAura(SPELL_WARRIOR_REPRISAL_R1))
            refundChance = 50;

        if (refundChance && roll_chance_i(refundChance) && spell->GetPowerCost())
        {
            int32 refundBasePoints = int32(spell->GetPowerCost());
            player->CastCustomSpell(player, SPELL_WARRIOR_REPRISAL_REFUND, &refundBasePoints, nullptr, nullptr, true);
        }
    }
};

struct spell_warrior_blood_drinker : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!owner->HasAura(SPELL_WARRIOR_DEATH_WISH) && !owner->HasAura(SPELL_WARRIOR_RECKLESSNESS) &&
            !owner->HasAura(SPELL_WARRIOR_ENRAGE_R1) && !owner->HasAura(SPELL_WARRIOR_ENRAGE_R2) &&
            !owner->HasAura(SPELL_WARRIOR_ENRAGE_R3) && !owner->HasAura(SPELL_WARRIOR_ENRAGE_R4) &&
            !owner->HasAura(SPELL_WARRIOR_ENRAGE_R5))
            return SPELL_AURA_PROC_FAILED;

        int32 const heal = std::max<int32>(1, int32(owner->GetMaxHealth()) * (aura->GetModifier()->m_amount + 1) / 10000);
        owner->CastCustomSpell(owner, SPELL_WARRIOR_BLOOD_DRINKER_HEAL, &heal, nullptr, nullptr, true, nullptr, aura);
        return SPELL_AURA_PROC_OK;
    }
};

struct spell_warrior_defensive_tactics : public AuraScript
{
    void OnThreatCalculate(Aura* aura, SpellEntry const* /*threatSpell*/, SpellSchoolMask schoolMask, float& threat) override
    {
        if (schoolMask == SPELL_SCHOOL_MASK_NONE)
            return;

        Player* player = aura->GetTarget()->ToPlayer();
        if (!player)
            return;

        ShapeshiftForm const form = player->GetShapeshiftForm();
        if (form != FORM_BATTLESTANCE && form != FORM_BERSERKERSTANCE)
            return;

        Item const* offhand = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
        if (!offhand || offhand->IsBroken() || offhand->GetProto()->InventoryType != INVTYPE_SHIELD)
            return;

        threat *= (100.0f + (aura->GetModifier()->m_amount + 1) * 30 / 100) / 100.0f;
    }
};

struct spell_warrior_intimidating_shout : public SpellScript
{
    bool OnCheckTarget(Spell const* spell, Unit* target, SpellEffectIndex /*eff*/) const override
    {
        return target != spell->m_targets.getUnitTarget();
    }
};

struct spell_warrior_sweeping_strikes : public AuraScript
{
    void OnHolderInit(SpellAuraHolder* holder, WorldObject* /*caster*/) override
    {
        holder->SetRemovedOnShapeLost(false);
    }

    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* victim, uint32 damage, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!victim || !victim->IsAlive())
            return SPELL_AURA_PROC_FAILED;

        if (procSpell && (procSpell->Id == SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK || procSpell->Id == SPELL_WARRIOR_SWEEPING_STRIKES_TRIGGER))
            return SPELL_AURA_PROC_FAILED;

        if (procSpell && !procSpell->IsDirectDamageSpell())
            return SPELL_AURA_PROC_FAILED;

        if (!damage)
            return SPELL_AURA_PROC_FAILED;

        float radius = ATTACK_DISTANCE;
        if (procSpell && procSpell->Id == SPELL_WARRIOR_WHIRLWIND)
            radius = 8.0f;

        Unit* target = owner->SelectRandomUnfriendlyTarget(victim, radius, false, true, true);
        if (!target)
            return SPELL_AURA_PROC_OK;

        int32 basepoints = 0;
        uint32 triggerSpellId = SPELL_WARRIOR_SWEEPING_STRIKES_TRIGGER;
        if (procSpell && procSpell->Id == SPELL_WARRIOR_EXECUTE_TRIGGER)
        {
            if (victim->GetHealthPercent() <= 20.0f && target->GetHealthPercent() <= 20.0f)
            {
                int32 const initialDamage = damage * 100 / owner->CalcArmorReducedDamage(victim, 100);
                basepoints = initialDamage * owner->CalcArmorReducedDamage(target, 100) / 100;
            }
            else if (victim->GetHealthPercent() <= 20.0f)
                triggerSpellId = SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK;
            else
            {
                int32 const initialDamage = damage * 100 / owner->CalcArmorReducedDamage(victim, 100);
                basepoints = initialDamage * owner->CalcArmorReducedDamage(target, 100) / 100;
            }
        }
        else
        {
            int32 const initialDamage = damage * 100 / owner->CalcArmorReducedDamage(victim, 100);
            basepoints = initialDamage * owner->CalcArmorReducedDamage(target, 100) / 100;
        }

        if (basepoints)
            owner->CastCustomSpell(target, triggerSpellId, &basepoints, nullptr, nullptr, true, nullptr, aura);
        else
            owner->CastSpell(target, triggerSpellId, true, nullptr, aura);

        return SPELL_AURA_PROC_OK;
    }
};

struct spell_warrior_retaliation : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* victim, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!victim || !owner->HasInArc(victim) || owner->HasUnitState(UNIT_STAT_CAN_NOT_REACT))
            return SPELL_AURA_PROC_FAILED;

        owner->CastSpell(victim, SPELL_WARRIOR_RETALIATION_TRIGGER, true, nullptr, aura);
        return SPELL_AURA_PROC_OK;
    }
};

struct spell_adrift_strikes : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* victim, uint32 damage, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!victim || !victim->IsAlive())
            return SPELL_AURA_PROC_FAILED;

        if (procSpell && (procSpell->Id == SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK || procSpell->Id == SPELL_WARRIOR_SWEEPING_STRIKES_TRIGGER))
            return SPELL_AURA_PROC_FAILED;

        if (procSpell && !procSpell->IsDirectDamageSpell())
            return SPELL_AURA_PROC_FAILED;

        if (!damage)
            return SPELL_AURA_PROC_FAILED;

        float radius = ATTACK_DISTANCE;
        if (procSpell && procSpell->Id == SPELL_WARRIOR_WHIRLWIND)
            radius = 8.0f;

        Unit* target = owner->SelectRandomUnfriendlyTarget(victim, radius, false, true, true);
        if (!target)
            return SPELL_AURA_PROC_OK;

        int32 basepoints = 0;
        uint32 triggerSpellId = SPELL_WARRIOR_SWEEPING_STRIKES_TRIGGER;
        if (procSpell && procSpell->Id == SPELL_WARRIOR_EXECUTE_TRIGGER)
        {
            if (victim->GetHealthPercent() <= 20.0f && target->GetHealthPercent() <= 20.0f)
            {
                int32 const initialDamage = damage * 100 / owner->CalcArmorReducedDamage(victim, 100);
                basepoints = initialDamage * owner->CalcArmorReducedDamage(target, 100) / 100;
            }
            else if (victim->GetHealthPercent() <= 20.0f)
                triggerSpellId = SPELL_WARRIOR_SWEEPING_STRIKES_EXTRA_ATTACK;
            else
            {
                int32 const initialDamage = damage * 100 / owner->CalcArmorReducedDamage(victim, 100);
                basepoints = initialDamage * owner->CalcArmorReducedDamage(target, 100) / 100;
            }
        }
        else
        {
            int32 const initialDamage = damage * 100 / owner->CalcArmorReducedDamage(victim, 100);
            basepoints = initialDamage * owner->CalcArmorReducedDamage(target, 100) / 100;
        }

        if (basepoints)
            owner->CastCustomSpell(target, triggerSpellId, &basepoints, nullptr, nullptr, true, nullptr, aura);
        else
            owner->CastSpell(target, triggerSpellId, true, nullptr, aura);

        return SPELL_AURA_PROC_OK;
    }
};
}

void AddSC_warrior_spell_scripts()
{
    RegisterSpellScript("spell_warrior_bloodthirst", &GetSpellScript<spell_warrior_bloodthirst>);
    RegisterSpellScript("spell_warrior_shield_slam", &GetSpellScript<spell_warrior_shield_slam>);
    RegisterSpellScript("spell_warrior_shield_specialization", &GetSpellScript<spell_warrior_shield_specialization>);
    RegisterSpellScript("spell_warrior_execute_trigger", &GetSpellScript<spell_warrior_execute_trigger>);
    RegisterSpellScript("spell_warrior_execute", &GetSpellScript<spell_warrior_execute>);
    RegisterSpellScript("spell_warrior_warriors_wrath", &GetSpellScript<spell_warrior_warriors_wrath>);
    RegisterSpellScript("spell_warrior_deep_wounds", &GetSpellScript<spell_warrior_deep_wounds>);
    RegisterSpellScript("spell_warrior_last_stand", &GetSpellScript<spell_warrior_last_stand>);
    RegisterSpellScript("spell_warrior_bloodrage", &GetSpellScript<spell_warrior_bloodrage>);
    RegisterSpellScript("spell_warrior_unbridled_wrath", &GetSpellScript<spell_warrior_unbridled_wrath>);
    RegisterSpellScript("spell_warrior_master_strike", &GetSpellScript<spell_warrior_master_strike>);
    RegisterSpellScript("spell_warrior_master_strike_polearm", &GetSpellScript<spell_warrior_master_strike_polearm>);
    RegisterSpellScript("spell_warrior_revenge", &GetSpellScript<spell_warrior_revenge>);
    RegisterSpellScript("spell_warrior_intimidating_shout", &GetSpellScript<spell_warrior_intimidating_shout>);
    RegisterAuraScript("spell_warrior_flurry", &GetAuraScript<spell_warrior_flurry>);
    RegisterAuraScript("spell_warrior_blood_drinker", &GetAuraScript<spell_warrior_blood_drinker>);
    RegisterAuraScript("spell_warrior_defensive_tactics", &GetAuraScript<spell_warrior_defensive_tactics>);
    RegisterAuraScript("spell_warrior_sweeping_strikes", &GetAuraScript<spell_warrior_sweeping_strikes>);
    RegisterAuraScript("spell_warrior_retaliation", &GetAuraScript<spell_warrior_retaliation>);
    RegisterAuraScript("spell_adrift_strikes", &GetAuraScript<spell_adrift_strikes>);
}
