#include "scriptPCH.h"

namespace
{
enum DruidSpells
{
    SPELL_DRUID_WOLFSHEAD_HELM_ENERGY = 17770,
    SPELL_DRUID_SWIFTMEND = 18562,
    SPELL_DRUID_FRENZIED_REGENERATION = 22842,
    SPELL_DRUID_FRENZIED_REGENERATION_RANK_2 = 22895,
    SPELL_DRUID_FRENZIED_REGENERATION_RANK_3 = 22896,
    SPELL_DRUID_EFFLORESCENCE = 51397,
    SPELL_DRUID_RESHIFT = 47379,
    SPELL_DRUID_AESSINAS_BLOOM_RANK_1 = 51446,
    SPELL_DRUID_AESSINAS_BLOOM_RANK_2 = 51447,
    SPELL_DRUID_AESSINAS_BLOOM_BUFF_RANK_1 = 46788,
    SPELL_DRUID_AESSINAS_BLOOM_BUFF_RANK_2 = 46789,
    SPELL_DRUID_AESSINAS_BLOOM_MANA = 52567,
    SPELL_DRUID_PRESERVATION_RANK_1 = 51448,
    SPELL_DRUID_PRESERVATION_RANK_2 = 51449,
    SPELL_DRUID_PRESERVATION_RANK_3 = 51450,
    SPELL_DRUID_BALANCE_OF_ALL_THINGS_RANK_1 = 51433,
    SPELL_DRUID_BALANCE_OF_ALL_THINGS_RANK_2 = 51434,
    SPELL_DRUID_BALANCE_OF_ALL_THINGS_RANK_3 = 51435,
    SPELL_DRUID_ECLIPSE = 51444,
    SPELL_DRUID_ECLIPSE_ARCANE = 51445,
    SPELL_DRUID_NATURE_ECLIPSE = 51442,
    SPELL_DRUID_ARCANE_ECLIPSE = 51443,
    SPELL_DRUID_CENARION_BLESSING_HEAL = 52324,
    SPELL_DRUID_RAPID_SOLSTICE = 52336,
    SPELL_DRUID_RAPID_SOLSTICE_PASSIVE = 52337,
    SPELL_DRUID_RESTO_T25_5P_BONUS = 52345,
    SPELL_DRUID_PRIMAL_FEROCITY_STACK = 52369,
    SPELL_DRUID_ASHAMANES_FURY = 52370,
    SPELL_DRUID_PRIMAL_FEROCITY = 52372,
    SPELL_DRUID_CARNAGE_HEAL = 52735,
    SPELL_DRUID_CARNAGE_COMBO_POINT = 52736,
    SPELL_DRUID_EQUILIBRIUM_INSECT_SWARM = 52924,
    SPELL_DRUID_EQUILIBRIUM_MOONFIRE = 52925,
};

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

void RegisterSpellAndAuraScript(char const* name, SpellScript* (*spellGetter)(SpellEntry const*), AuraScript* (*auraGetter)(SpellEntry const*))
{
    Script* script = new Script;
    script->Name = name;
    script->GetSpellScript = spellGetter;
    script->GetAuraScript = auraGetter;
    script->RegisterSelf();
}

uint32 CountDruidBleedsOnTarget(Unit* target, Player* caster)
{
    if (!target || !caster)
        return 0;

    uint32 bleedCount = 0;
    ObjectGuid casterGuid = caster->GetObjectGuid();
    Unit::AuraList const& periodic = target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
    for (Aura const* aura : periodic)
    {
        if (aura->GetCasterGuid() != casterGuid)
            continue;

        SpellEntry const* auraSpell = aura->GetSpellProto();
        if (auraSpell && (auraSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_RAKE_CLAW>() || auraSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_RIP_BITE>()))
            ++bleedCount;
    }

    return bleedCount;
}

template <ClassFlag... Args>
bool TargetHasDruidPeriodicAuraFromCaster(Unit const* target, ObjectGuid casterGuid)
{
    if (!target)
        return false;

    Unit::AuraList const& periodic = target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
    for (Aura const* aura : periodic)
    {
        if (aura->GetCasterGuid() != casterGuid)
            continue;

        SpellEntry const* auraSpell = aura->GetSpellProto();
        if (auraSpell && auraSpell->IsFitToFamily<SPELLFAMILY_DRUID, Args...>())
            return true;
    }

    return false;
}

template <ClassFlag... Args>
Aura* GetDruidPeriodicDamageAuraFromCaster(Unit* target, ObjectGuid casterGuid)
{
    if (!target)
        return nullptr;

    Unit::AuraList const& periodic = target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
    for (Aura* aura : periodic)
    {
        if (aura->GetCasterGuid() != casterGuid)
            continue;

        SpellEntry const* auraSpell = aura->GetSpellProto();
        if (auraSpell && auraSpell->IsFitToFamily<SPELLFAMILY_DRUID, Args...>())
            return aura;
    }

    return nullptr;
}

template <ClassFlag... Args>
bool TargetHasDruidPeriodicHeal(Unit const* target)
{
    if (!target)
        return false;

    Unit::AuraList const& periodic = target->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
    for (Aura const* aura : periodic)
    {
        SpellEntry const* auraSpell = aura->GetSpellProto();
        if (auraSpell && auraSpell->IsFitToFamily<SPELLFAMILY_DRUID, Args...>())
            return true;
    }

    return false;
}

int32 GetOpenWoundsBonus(Player* caster, bool forClaw)
{
    static uint32 const openWoundsRanks[] = {51402, 51403, 51404};
    int32 bonus = 0;
    for (uint32 openWoundsId : openWoundsRanks)
    {
        if (!caster->HasSpell(openWoundsId) && !caster->HasAura(openWoundsId))
            continue;

        if (SpellEntry const* openWoundsInfo = sSpellMgr.GetSpellEntry(openWoundsId))
            bonus = std::max(bonus, openWoundsInfo->EffectBasePoints[forClaw ? EFFECT_INDEX_0 : EFFECT_INDEX_1] + 1);
    }

    return bonus;
}

int32 GetBalanceOfAllThingsStarfireCritBonus(Unit* caster)
{
    if (!caster)
        return 0;

    int32 bonus = 0;
    Unit::AuraList const& overrides = caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    for (Aura const* aura : overrides)
    {
        if (aura->GetModifier()->m_miscvalue != 5060)
            continue;

        switch (aura->GetId())
        {
            case SPELL_DRUID_BALANCE_OF_ALL_THINGS_RANK_1:
            case SPELL_DRUID_BALANCE_OF_ALL_THINGS_RANK_2:
            case SPELL_DRUID_BALANCE_OF_ALL_THINGS_RANK_3:
                bonus = std::max(bonus, aura->GetModifier()->m_amount);
                break;
        }
    }

    return bonus;
}

bool IsDruidFerociousBite(SpellEntry const* spellInfo)
{
    return spellInfo && spellInfo->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_RIP_BITE>() &&
            spellInfo->HasEffect(SPELL_EFFECT_SCHOOL_DAMAGE);
}

void DealDruidExtraPeriodicDamageTick(Unit* caster, Unit* target, Aura* periodicAura)
{
    if (!caster || !target || !periodicAura || !target->IsAlive())
        return;

    SpellEntry const* spellProto = periodicAura->GetSpellProto();
    if (!spellProto)
        return;

    SpellEffectIndex const effIdx = periodicAura->GetEffIndex();
    uint32 damage = periodicAura->GetModifier()->m_amount > 0 ? uint32(periodicAura->GetModifier()->m_amount) : 0;
    if (!damage)
        return;

    if (spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE || spellProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC)
        damage = target->SpellDamageBonusTaken(caster, spellProto, effIdx, damage, DOT, periodicAura->GetStackAmount());
    else
        damage = target->MeleeDamageBonusTaken(caster, damage, spellProto->GetWeaponAttackType(), spellProto, effIdx, DOT, periodicAura->GetStackAmount());

    uint32 const originalDamage = damage;
    uint32 absorb = 0;
    int32 resist = 0;
    target->CalculateDamageAbsorbAndResist(caster, spellProto->GetSpellSchoolMask(), DOT, damage, &absorb, &resist, spellProto);
    caster->DealDamageMods(target, damage, &absorb);

    uint32 const bonus = resist < 0 ? uint32(std::abs(resist)) : 0;
    damage += bonus;
    uint32 const malus = resist > 0 ? absorb + uint32(resist) : absorb;
    damage = damage <= malus ? 0 : damage - malus;

    SpellPeriodicAuraLogInfo logInfo(periodicAura, damage, absorb, resist, 0.0f);
    target->SendPeriodicAuraLog(&logInfo);

    uint32 procVictim = PROC_FLAG_TAKE_HARMFUL_PERIODIC;
    if (damage)
        procVictim |= PROC_FLAG_TAKEN_ANY_DAMAGE;

    caster->ProcDamageAndSpell(target, PROC_FLAG_DEAL_HARMFUL_PERIODIC, procVictim, PROC_EX_NORMAL_HIT, damage, originalDamage, BASE_ATTACK, spellProto);

    CleanDamage cleanDamage(0, BASE_ATTACK, MELEE_HIT_NORMAL, absorb, resist);
    caster->DealDamage(target, damage, &cleanDamage, DOT, spellProto->GetSpellSchoolMask(), spellProto, true);
}

void AddDruidPrimalFerocityStacks(Player* player, uint8 comboPoints)
{
    if (!player || !comboPoints || !player->HasAura(SPELL_DRUID_PRIMAL_FEROCITY))
        return;

    for (uint8 i = 0; i < comboPoints; ++i)
    {
        SpellAuraHolder* holder = player->GetSpellAuraHolder(SPELL_DRUID_PRIMAL_FEROCITY_STACK);
        if (holder)
            holder->ModStackAmount(1);
        else
            holder = player->AddAura(SPELL_DRUID_PRIMAL_FEROCITY_STACK, ADD_AURA_POSITIVE, player);

        if (!holder || holder->GetStackAmount() < holder->GetSpellProto()->StackAmount)
            continue;

        player->RemoveAurasDueToSpell(SPELL_DRUID_PRIMAL_FEROCITY_STACK);
        player->CastSpell(player, SPELL_DRUID_ASHAMANES_FURY, true);
    }
}

void RefreshDruidCarnageBleed(Unit* target, ObjectGuid casterGuid, uint64 classMask, uint32 spellIconId)
{
    if (!target)
        return;

    Unit::AuraList const& periodic = target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
    for (Aura* aura : periodic)
    {
        if (aura->GetCasterGuid() != casterGuid)
            continue;

        SpellEntry const* auraSpell = aura->GetSpellProto();
        if (!auraSpell || !auraSpell->IsFitToFamily(SPELLFAMILY_DRUID, classMask) ||
                auraSpell->SpellIconID != spellIconId)
            continue;

        aura->GetHolder()->RefreshHolder();
        return;
    }
}

SpellEntry const* GetCurrentShapeshiftSpell(Unit* caster)
{
    if (!caster)
        return nullptr;

    ShapeshiftForm const form = caster->GetShapeshiftForm();
    if (form == FORM_NONE)
        return nullptr;

    Unit::AuraList const& shapeshiftAuras = caster->GetAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
    for (Aura const* aura : shapeshiftAuras)
        if (aura->GetModifier()->m_miscvalue == form)
            return sSpellMgr.GetSpellEntry(aura->GetId());

    return nullptr;
}

struct spell_druid_reshift : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        if (!spell->m_casterUnit || spell->m_casterUnit->GetShapeshiftForm() == FORM_NONE)
            return SPELL_FAILED_ONLY_SHAPESHIFT;

        return SPELL_CAST_OK;
    }

    std::optional<uint32> OnCalculatePowerCost(SpellEntry const* /*spellInfo*/, Unit* caster, Spell* spell, Item* /*castItem*/) const override
    {
        if (SpellEntry const* shapeshiftSpellInfo = GetCurrentShapeshiftSpell(caster))
            return Spell::CalculatePowerCost(shapeshiftSpellInfo, caster, spell);

        return 0;
    }

    bool OnTakePower(Spell* spell) const override
    {
        if (spell->IsTriggered() && spell->m_triggeredBySpellInfo && spell->m_triggeredBySpellInfo->Id == SPELL_DRUID_RESHIFT &&
                spell->m_spellInfo->HasAura(SPELL_AURA_MOD_SHAPESHIFT))
            return false;

        return true;
    }

    bool OnEffectExecute(Spell* spell, SpellEffectIndex /*effIdx*/) const override
    {
        Player* player = spell->m_caster->ToPlayer();
        if (!player)
            return false;

        SpellEntry const* shapeshiftSpellInfo = GetCurrentShapeshiftSpell(player);
        if (!shapeshiftSpellInfo)
            return false;

        player->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT, AURA_REMOVE_BY_CANCEL);
        player->CastSpell(player, shapeshiftSpellInfo, true, nullptr, nullptr, ObjectGuid(), spell->m_spellInfo);
        return false;
    }
};

struct spell_druid_swiftmend : public SpellScript
{
    SpellCastResult OnCheckCast(Spell* spell, bool /*strict*/) const override
    {
        Unit* target = spell->m_targets.getUnitTarget();
        return target && target->GetAura(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_DRUID, UI64LIT(0x50)) ? SPELL_CAST_OK : SPELL_FAILED_TARGET_AURASTATE;
    }

    bool OnEffectHealCalculate(Spell* spell, SpellEffectIndex /*effIdx*/, int32& heal) const override
    {
        Unit* target = spell->GetUnitTarget();
        if (!target)
            return false;

        Unit::AuraList const& periodicHeals = target->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
        Aura* targetAura = nullptr;
        for (Aura* aura : periodicHeals)
        {
            if (aura->GetSpellProto()->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_REJUVENATION, CF_DRUID_REGROWTH>())
                if (!targetAura || aura->GetAuraDuration() < targetAura->GetAuraDuration())
                    targetAura = aura;
        }

        if (!targetAura)
        {
            sLog.outError("Target (GUID: %u TypeId: %u) has aurastate AURA_STATE_SWIFTMEND but no matching aura.", target->GetGUIDLow(), target->GetTypeId());
            return false;
        }

        int32 tickheal = targetAura->GetModifier()->m_amount;
        int32 tickcount = 0;
        uint32 const period = targetAura->GetModifier()->periodictime;
        if (!period)
            return false;

        if (targetAura->GetSpellProto()->IsFitToFamilyMask<CF_DRUID_REGROWTH>())
            tickcount = 18000 / period;
        if (targetAura->GetSpellProto()->IsFitToFamilyMask<CF_DRUID_REJUVENATION>())
            tickcount = 12000 / period;

        target->RemoveAurasDueToSpell(targetAura->GetId());
        heal += tickheal * tickcount;
        return true;
    }
};

struct spell_druid_aessinas_bloom : public AuraScript
{
    std::optional<SpellProcEventTriggerCheck> OnCheckProc(Unit const* /*owner*/, Unit* victim, SpellAuraHolder* holder, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procExtra*/, WeaponAttackType /*attType*/, bool /*isVictim*/) override
    {
        switch (holder->GetId())
        {
            case SPELL_DRUID_AESSINAS_BLOOM_RANK_1:
            case SPELL_DRUID_AESSINAS_BLOOM_RANK_2:
                if (!procSpell || !procSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_HEALING_TOUCH>())
                    return SPELL_PROC_TRIGGER_FAILED;

                if (!TargetHasDruidPeriodicHeal<CF_DRUID_REJUVENATION, CF_DRUID_REGROWTH>(victim))
                    return SPELL_PROC_TRIGGER_FAILED;

                return std::nullopt;
            case SPELL_DRUID_AESSINAS_BLOOM_BUFF_RANK_1:
            case SPELL_DRUID_AESSINAS_BLOOM_BUFF_RANK_2:
                if (!procSpell || !procSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_HEALING_TOUCH>())
                    return SPELL_PROC_TRIGGER_FAILED;

                return std::nullopt;
            default:
                return std::nullopt;
        }
    }

    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!owner || !aura || !procSpell)
            return SPELL_AURA_PROC_FAILED;

        switch (aura->GetId())
        {
            case SPELL_DRUID_AESSINAS_BLOOM_BUFF_RANK_1:
            case SPELL_DRUID_AESSINAS_BLOOM_BUFF_RANK_2:
                break;
            default:
                return std::nullopt;
        }

        return SPELL_AURA_PROC_CANT_TRIGGER;
    }
};

struct spell_druid_healing_touch : public SpellScript
{
    bool OnEffectHealCalculate(Spell* spell, SpellEffectIndex /*effIdx*/, int32& heal) const override
    {
        if (!spell || !spell->m_casterUnit || !spell->m_casterUnit->HasAura(SPELL_DRUID_RESTO_T25_5P_BONUS))
            return true;

        Unit* target = spell->GetUnitTarget();
        if (!target || !spell->m_spellInfo->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_HEALING_TOUCH>())
            return true;

        int32 bonusPct = 0;
        if (TargetHasDruidPeriodicHeal<CF_DRUID_REJUVENATION>(target))
            bonusPct += 10;

        if (TargetHasDruidPeriodicHeal<CF_DRUID_REGROWTH>(target))
            bonusPct += 10;

        if (bonusPct > 0)
            heal = heal * (100 + bonusPct) / 100;

        return true;
    }

    void OnFinish(Spell* spell, bool ok) const override
    {
        if (!ok || !spell || !spell->m_casterUnit || !spell->GetPowerCost())
            return;

        // m_appliedMods can hold mods whose aura went away during the cast:
        // Player::AddSpellMod(mod, false) deletes them without touching the
        // spell's list, so dereferencing its entries crashed here (SIGSEGV in
        // Spell::finish). Look the bloom mods up in the caster's live list and
        // only compare pointers against m_appliedMods.
        Player* player = spell->m_casterUnit->ToPlayer();
        if (!player)
            return;

        int32 refundPct = 0;
        for (uint32 bloomSpellId : { SPELL_DRUID_AESSINAS_BLOOM_BUFF_RANK_1, SPELL_DRUID_AESSINAS_BLOOM_BUFF_RANK_2 })
        {
            for (uint32 op = 0; op < MAX_SPELLMOD; ++op)
            {
                SpellModifier* mod = player->GetSpellMod(SpellModOp(op), bloomSpellId);
                if (!mod || !mod->ownerAura ||
                    std::find(spell->m_appliedMods.begin(), spell->m_appliedMods.end(), mod) == spell->m_appliedMods.end())
                    continue;

                if (SpellEntry const* bloomInfo = mod->ownerAura->GetSpellProto())
                    refundPct = std::max(refundPct, bloomInfo->CalculateSimpleValue(EFFECT_INDEX_0));
            }
        }

        if (refundPct <= 0)
            return;

        int32 const refund = spell->GetPowerCost() * uint32(refundPct) / 100;
        if (refund > 0)
            spell->m_casterUnit->CastCustomSpell(spell->m_casterUnit, SPELL_DRUID_AESSINAS_BLOOM_MANA, &refund, nullptr, nullptr, true, nullptr);
    }
};

struct spell_druid_berserk : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return true;

        Player* player = spell->m_caster->ToPlayer();
        if (!player || !spell->m_casterUnit)
            return false;

        if (player->GetShapeshiftForm() == FORM_CAT)
            player->CastSpell(player, 45710, true);

        if (player->GetShapeshiftForm() == FORM_BEAR || player->GetShapeshiftForm() == FORM_DIREBEAR)
        {
            int32 healthModSpellBasePoints0 = int32(spell->m_casterUnit->GetMaxHealth() * 0.2f);
            spell->m_casterUnit->CastCustomSpell(spell->m_casterUnit, 45709, &healthModSpellBasePoints0, nullptr, nullptr, true, nullptr);
        }

        return false;
    }
};

struct spell_druid_berserk_form_swap : public AuraScript
{
    void OnAfterShapeshift(Aura* aura, ShapeshiftForm /*oldForm*/, ShapeshiftForm newForm) override
    {
        Unit* target = aura->GetTarget();
        if (!target)
            return;

        switch (aura->GetId())
        {
            case 45709:
            {
                if (newForm != FORM_CAT)
                    return;

                int32 const duration = aura->GetAuraDuration();
                target->m_Events.AddLambdaEventAtOffset([target, duration]()
                {
                    if (SpellAuraHolder* newBerserk = target->AddAura(45710, ADD_AURA_POSITIVE))
                    {
                        newBerserk->SetAuraDuration(duration);
                        newBerserk->UpdateAuraDuration();
                    }
                }, 1);
                target->RemoveAurasDueToSpellByCancel(45709);
                break;
            }
            case 45710:
            {
                if (newForm != FORM_BEAR && newForm != FORM_DIREBEAR)
                    return;

                int32 const duration = aura->GetAuraDuration();
                target->m_Events.AddLambdaEventAtOffset([target, duration]()
                {
                    int32 healthModSpellBasePoints0 = int32(target->GetMaxHealth() * 0.2f);
                    if (SpellAuraHolder* newBerserk = target->AddAura(45709, ADD_AURA_POSITIVE, nullptr, &healthModSpellBasePoints0))
                    {
                        newBerserk->SetAuraDuration(duration);
                        newBerserk->UpdateAuraDuration();
                    }
                }, 1);
                target->RemoveAurasDueToSpellByCancel(45710);
                break;
            }
            default:
                break;
        }
    }
};

struct spell_druid_tree_of_life_aura : public AuraScript
{
    int32 OnAuraValueCalculate(Aura* aura, Unit* caster, Unit* /*target*/, SpellEntry const* /*spellProto*/, SpellEffectIndex /*effIdx*/, Item* /*castItem*/, int32 value) override
    {
        if (!caster || !aura->GetHolder()->IsAddedBySpell())
            return value;

        return caster->GetStat(STAT_SPIRIT) * (float(value) / 100.0f);
    }
};

struct spell_druid_preservation : public AuraScript
{
    void OnPeriodicHealingBonus(Aura* periodicAura, Aura* modifierAura, Unit* caster, Unit* target, uint32& amount) override
    {
        if (!periodicAura || !modifierAura || !caster || !target)
            return;

        switch (modifierAura->GetId())
        {
            case SPELL_DRUID_PRESERVATION_RANK_1:
            case SPELL_DRUID_PRESERVATION_RANK_2:
            case SPELL_DRUID_PRESERVATION_RANK_3:
                break;
            default:
                return;
        }

        SpellEntry const* periodicSpell = periodicAura->GetSpellProto();
        if (!periodicSpell || !periodicSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_REGROWTH>())
            return;

        ObjectGuid const casterGuid = caster->GetObjectGuid();
        Unit::AuraList const& periodicHeals = target->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
        for (Aura const* aura : periodicHeals)
        {
            if (aura->GetCasterGuid() != casterGuid)
                continue;

            SpellEntry const* auraSpell = aura->GetSpellProto();
            if (!auraSpell || !auraSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_REJUVENATION>())
                continue;

            amount += amount * modifierAura->GetModifier()->m_amount / 100;
            return;
        }
    }
};

struct spell_druid_enrage : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_1)
            return true;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return false;

        int32 reductionMod = target->HasAura(9634) ? -16 : -27;
        target->CastCustomSpell(target, 25503, nullptr, &reductionMod, nullptr, true);
        return false;
    }
};

struct spell_druid_shapeshift_root_snare_removal : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return true;

        Unit* target = spell->GetUnitTarget() ? spell->GetUnitTarget() : spell->m_casterUnit;
        if (!target)
            return false;

        target->RemoveNegativeSpellsCausingAura(SPELL_AURA_MOD_ROOT);
        Unit::AuraList const& slowingAuras = target->GetAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
        for (auto iter = slowingAuras.begin(); iter != slowingAuras.end();)
        {
            SpellEntry const* auraSpellInfo = (*iter)->GetSpellProto();
            if ((*iter)->IsPositive() || auraSpellInfo->HasAttribute(SPELL_ATTR_NEGATIVE))
            {
                ++iter;
                continue;
            }

            uint32 auraMechanicMask = auraSpellInfo->GetAllSpellMechanicMask();
            if ((auraMechanicMask & MECHANIC_NOT_REMOVED_BY_SHAPESHIFT) ||
                    (auraSpellInfo->SpellIconID == 15 && auraSpellInfo->Dispel == 0 &&
                    (auraMechanicMask & (1 << (MECHANIC_SNARE - 1))) == 0))
            {
                ++iter;
                continue;
            }

            target->RemoveAurasDueToSpellByCancel(auraSpellInfo->Id);
            iter = slowingAuras.begin();
        }

        return false;
    }
};

struct spell_druid_open_wounds : public SpellScript
{
    void OnEffectDamageCalculate(Spell* spell, SpellEffectIndex /*effIdx*/, float& damage) const override
    {
        if (spell->m_spellInfo->SpellVisual != 3882)
            return;

        Player* player = spell->m_caster->ToPlayer();
        if (!player)
            return;

        if (uint32 bleeds = CountDruidBleedsOnTarget(spell->GetUnitTarget(), player))
        {
            int32 bonusPerBleed = GetOpenWoundsBonus(player, true);
            if (bonusPerBleed > 0)
                damage = int32(float(damage) * (100.0f + bonusPerBleed * bleeds) / 100.0f);
        }
    }
};

struct spell_druid_ferocious_bite : public SpellScript
{
    void OnComboPointsSpent(Spell* spell, uint8 comboPoints) const override
    {
        Player* player = spell && spell->m_casterUnit ? spell->m_casterUnit->ToPlayer() : nullptr;
        AddDruidPrimalFerocityStacks(player, comboPoints);
    }

    void OnEffectDamageCalculate(Spell* spell, SpellEffectIndex effIdx, float& damage) const override
    {
        if (spell->m_spellInfo->SpellVisual != 6587)
            return;

        Player* player = spell->m_caster->ToPlayer();
        if (!player)
            return;

        if (uint32 combo = player->GetComboPoints())
            damage += int32(player->GetTotalAttackPowerValue(BASE_ATTACK) * combo * 0.03f);

        damage += int32(player->GetPower(POWER_ENERGY) * spell->m_spellInfo->DmgMultiplier[effIdx]);
        player->SetPower(POWER_ENERGY, 0);

        if (spell->m_spellInfo->Id == 22567 || spell->m_spellInfo->Id == 22568 || spell->m_spellInfo->Id == 22827 ||
                spell->m_spellInfo->Id == 22828 || spell->m_spellInfo->Id == 22829)
        {
            static uint32 const feralAggressionRanks[] = {16858, 16859, 16860, 16861, 16862};
            for (uint32 feralAggressionId : feralAggressionRanks)
            {
                if (!player->HasSpell(feralAggressionId))
                    continue;

                if (SpellEntry const* feralAggressionInfo = sSpellMgr.GetSpellEntry(feralAggressionId))
                {
                    int32 damageBonusPct = feralAggressionInfo->EffectBasePoints[EFFECT_INDEX_1] + 1;
                    damage = int32(float(damage) * (100.0f + damageBonusPct) / 100.0f);
                }
                break;
            }
        }

    }
};

struct spell_druid_thorns_explosion : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* victim, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!victim)
            return SPELL_AURA_PROC_FAILED;

        if (aura->GetId() == 46431 && !owner->HasAuraType(SPELL_AURA_DAMAGE_SHIELD))
            return SPELL_AURA_PROC_CANT_TRIGGER;

        owner->CastSpell(victim, aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()], true, nullptr, aura);
        return SPELL_AURA_PROC_OK;
    }
};

struct spell_druid_idol_of_equilibrium : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell || !spell->m_casterUnit)
            return false;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return false;

        Aura* periodicAura = nullptr;
        switch (spell->m_spellInfo->Id)
        {
            case SPELL_DRUID_EQUILIBRIUM_INSECT_SWARM:
                periodicAura = GetDruidPeriodicDamageAuraFromCaster<CF_DRUID_INSECT_SWARM>(target, spell->m_casterUnit->GetObjectGuid());
                break;
            case SPELL_DRUID_EQUILIBRIUM_MOONFIRE:
                periodicAura = GetDruidPeriodicDamageAuraFromCaster<CF_DRUID_MOONFIRE>(target, spell->m_casterUnit->GetObjectGuid());
                break;
            default:
                return false;
        }

        DealDruidExtraPeriodicDamageTick(spell->m_casterUnit, target, periodicAura);
        return false;
    }
};

struct spell_druid_balance_of_all_things : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* victim, uint32 damage, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!owner || !procSpell)
            return SPELL_AURA_PROC_FAILED;

        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return SPELL_AURA_PROC_CANT_TRIGGER;

        if (!damage || !procSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_WRATH>())
            return SPELL_AURA_PROC_OK;

        if (!TargetHasDruidPeriodicAuraFromCaster<CF_DRUID_INSECT_SWARM>(victim, owner->GetObjectGuid()))
            return SPELL_AURA_PROC_OK;

        int32 const refund = Spell::CalculatePowerCost(procSpell, owner) * uint32(aura->GetModifier()->m_amount) / 100;
        if (refund > 0)
            owner->CastCustomSpell(owner, 51671, &refund, nullptr, nullptr, true, nullptr, aura);

        return SPELL_AURA_PROC_OK;
    }
};

struct spell_druid_carnage : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* victim, uint32 damage, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!owner || !procSpell || !damage)
            return SPELL_AURA_PROC_FAILED;

        switch (aura->GetEffIndex())
        {
            case EFFECT_INDEX_0:
            {
                if (!procSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_MAUL_SWIPE, CF_DRUID_SAVAGE_BITE>())
                    return SPELL_AURA_PROC_CANT_TRIGGER;

                int32 const heal = int32(float(damage) * float(aura->GetModifier()->m_amount) / 100.0f);
                if (heal <= 0)
                    return SPELL_AURA_PROC_FAILED;

                owner->CastCustomSpell(owner, SPELL_DRUID_CARNAGE_HEAL, &heal, nullptr, nullptr, true, nullptr, aura);
                return SPELL_AURA_PROC_OK;
            }
            case EFFECT_INDEX_1:
            {
                Player* player = owner->ToPlayer();
                if (!player || !victim || !IsDruidFerociousBite(procSpell))
                    return SPELL_AURA_PROC_CANT_TRIGGER;

                uint8 const comboPoints = player->GetComboPoints();
                if (!comboPoints)
                    return SPELL_AURA_PROC_FAILED;

                float const chance = float(aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0) * 2 * comboPoints);
                if (!roll_chance_f(chance))
                    return SPELL_AURA_PROC_FAILED;

                RefreshDruidCarnageBleed(victim, owner->GetObjectGuid(), UI64LIT(1) << CF_DRUID_RAKE_CLAW, 494);
                RefreshDruidCarnageBleed(victim, owner->GetObjectGuid(), UI64LIT(1) << CF_DRUID_RIP_BITE, 108);

                if (Spell* spell = owner->GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    spell->AddTriggeredSpell(SPELL_DRUID_CARNAGE_COMBO_POINT);
                else
                    owner->CastSpell(victim, SPELL_DRUID_CARNAGE_COMBO_POINT, true, nullptr, aura);

                return SPELL_AURA_PROC_OK;
            }
            default:
                break;
        }

        return SPELL_AURA_PROC_CANT_TRIGGER;
    }
};

struct spell_druid_eclipse : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 damage, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown) override
    {
        if (!owner || !procSpell)
            return SPELL_AURA_PROC_FAILED;

        if (aura->GetEffIndex() != EFFECT_INDEX_0)
            return SPELL_AURA_PROC_CANT_TRIGGER;

        uint32 const triggerSpellId = aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()];
        uint32 excludedSpellId = 0;
        switch (triggerSpellId)
        {
            case SPELL_DRUID_ARCANE_ECLIPSE:
                if (!procSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_WRATH>())
                    return SPELL_AURA_PROC_CANT_TRIGGER;
                excludedSpellId = SPELL_DRUID_NATURE_ECLIPSE;
                break;
            case SPELL_DRUID_NATURE_ECLIPSE:
                if (!procSpell->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_STARFIRE>())
                    return SPELL_AURA_PROC_CANT_TRIGGER;
                excludedSpellId = SPELL_DRUID_ARCANE_ECLIPSE;
                break;
            default:
                return SPELL_AURA_PROC_FAILED;
        }

        if (!damage || owner->HasAura(triggerSpellId) || owner->HasAura(excludedSpellId) || owner->HasSpellCooldown(triggerSpellId))
            return SPELL_AURA_PROC_FAILED;

        SpellEntry const* triggerSpellInfo = sSpellMgr.GetSpellEntry(triggerSpellId);
        if (!triggerSpellInfo)
            return SPELL_AURA_PROC_FAILED;

        int32 bonus = triggerSpellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
        if (Player* player = owner->ToPlayer())
            bonus += int32(player->GetSpellCritPercent(GetFirstSchoolInMask(procSpell->GetSpellSchoolMask())) * float(aura->GetModifier()->m_amount) / 100.0f);

        owner->CastCustomSpell(owner, triggerSpellId, &bonus, nullptr, nullptr, true, nullptr, aura);
        if (owner->HasAura(SPELL_DRUID_RAPID_SOLSTICE_PASSIVE))
            owner->CastSpell(owner, SPELL_DRUID_RAPID_SOLSTICE, true, nullptr, aura);

        owner->AddSpellCooldown(triggerSpellId, 0, time(nullptr) + (cooldown ? cooldown : 30));
        return SPELL_AURA_PROC_OK;
    }
};

struct spell_druid_starfire : public SpellScript
{
    void OnSpellCritChanceCalculate(Spell* spell, Unit const* victim, float& critChance) const override
    {
        if (!spell->m_spellInfo->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_STARFIRE>())
            return;

        Unit* caster = spell->m_casterUnit;
        if (!caster || !TargetHasDruidPeriodicAuraFromCaster<CF_DRUID_MOONFIRE>(victim, caster->GetObjectGuid()))
            return;

        critChance += GetBalanceOfAllThingsStarfireCritBonus(caster);
    }
};

struct spell_druid_frenzied_regeneration : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, Unit* /*caster*/, Unit* target, WorldObject* /*targetObject*/, SpellEntry const*& spellInfo) override
    {
        if (!target)
        {
            spellInfo = nullptr;
            return;
        }

        int32 staminaPercentPerRage = aura->GetModifier()->m_amount;
        int32 rage = target->GetPower(POWER_RAGE);
        if (rage > 100)
            rage = 100;

        target->ModifyPower(POWER_RAGE, -rage);
        int32 heal = int32((rage / 10.0f) * target->GetStat(STAT_STAMINA) * staminaPercentPerRage / 100.0f);
        target->CastCustomSpell(target, 22845, &heal, nullptr, nullptr, true, nullptr, aura);
        spellInfo = nullptr;
    }
};

struct spell_druid_efflorescence : public AuraScript
{
    void OnAfterApply(Aura* aura, bool apply) override
    {
        if (apply || aura->GetHolder()->GetRemoveMode() != AURA_REMOVE_BY_STACK)
            return;

        if (aura->GetModifier()->m_auraname != SPELL_AURA_PERIODIC_HEAL)
            return;

        SpellEntry const* spellInfo = aura->GetSpellProto();
        if (!spellInfo || !spellInfo->IsFitToFamily<SPELLFAMILY_DRUID, CF_DRUID_REJUVENATION, CF_DRUID_REGROWTH>())
            return;

        Unit* caster = aura->GetCaster();
        Unit* target = aura->GetTarget();
        if (!caster || !target || !target->IsAlive())
            return;

        Aura* efflorescence = caster->GetAura(SPELL_DRUID_EFFLORESCENCE, EFFECT_INDEX_0);
        if (!efflorescence)
            return;

        int32 const remainingDuration = aura->GetAuraDuration();
        uint32 const period = aura->GetModifier()->periodictime;
        int32 const tickHeal = aura->GetModifier()->m_amount;
        if (remainingDuration <= 0 || !period || tickHeal <= 0)
            return;

        int32 const remainingValue = int32(float(tickHeal) * float(remainingDuration) / float(period));
        int32 const heal = remainingValue * efflorescence->GetModifier()->m_amount / 100;
        if (heal > 0)
            caster->DealHeal(target, uint32(heal), efflorescence->GetSpellProto());
    }
};

struct spell_druid_rip : public SpellScript, public AuraScript
{
    void OnComboPointsSpent(Spell* spell, uint8 comboPoints) const override
    {
        Player* player = spell && spell->m_casterUnit ? spell->m_casterUnit->ToPlayer() : nullptr;
        AddDruidPrimalFerocityStacks(player, comboPoints);
    }

    void OnPeriodicCalculateAmount(Aura* aura, float& amount) override
    {
        Unit* caster = aura->GetCaster();
        Player* player = caster ? caster->ToPlayer() : nullptr;
        if (!player)
            return;

        int32 const bonusPct = GetOpenWoundsBonus(player, false);
        if (bonusPct > 0)
            amount = amount * (100.0f + bonusPct) / 100.0f;
    }

    int32 OnDurationCalculate(WorldObject const* caster, Unit const* /*target*/, int32 /*duration*/) override
    {
        Player const* player = caster ? caster->ToPlayer() : nullptr;
        if (!player)
            return 0;

        uint8 comboPoints = player->GetComboPoints();
        if (comboPoints > 5)
            comboPoints = 5;

        return comboPoints ? int32(8000 + comboPoints * 2000) : 0;
    }
};

struct spell_druid_moonclaw : public AuraScript
{
    void OnAfterApply(Aura* aura, bool apply) override
    {
        Unit* target = aura->GetTarget();
        UpdateAura(target, 24932, apply);
        UpdateAura(target, 24907, apply);
    }

private:
    static void UpdateAura(Unit* target, uint32 spellId, bool apply)
    {
        Aura* aura = target->GetAura(spellId, EFFECT_INDEX_0);
        if (!aura || aura->GetCasterGuid() != target->GetObjectGuid())
            return;

        aura->ApplyModifier(false);
        if (!apply && aura->GetModifier()->m_amount > 3)
            aura->GetModifier()->m_amount -= 1;
        aura->ApplyModifier(true);
    }
};

struct spell_druid_glyph_of_the_moon : public AuraScript
{
    void OnAfterShapeshift(Aura* aura, ShapeshiftForm oldForm, ShapeshiftForm newForm) override
    {
        Player* target = aura->GetTarget()->ToPlayer();
        if (!target)
            return;

        if (newForm == FORM_MOONKIN && !target->HasAura(22650))
            target->AddAura(22650);
        else if (oldForm == FORM_MOONKIN && newForm != FORM_MOONKIN)
            target->RemoveAurasDueToSpell(22650);
    }
};

struct spell_druid_primal_fury : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (owner->GetShapeshiftForm() == FORM_CAT)
        {
            if (aura->GetEffIndex() == EFFECT_INDEX_0)
                return SPELL_AURA_PROC_FAILED;

            if (!procSpell || !procSpell->HasEffect(SPELL_EFFECT_ADD_COMBO_POINTS))
                return SPELL_AURA_PROC_FAILED;
        }
        else if (aura->GetEffIndex() == EFFECT_INDEX_1)
            return SPELL_AURA_PROC_FAILED;

        return std::nullopt;
    }
};

struct spell_druid_cenarion_blessing : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!owner)
            return SPELL_AURA_PROC_FAILED;

        Unit* target = owner->FindLowestHpFriendlyUnit(30.0f);
        if (!target)
            return SPELL_AURA_PROC_FAILED;

        owner->CastSpell(target, SPELL_DRUID_CENARION_BLESSING_HEAL, true, nullptr, aura);
        return SPELL_AURA_PROC_OK;
    }
};
}

void AddSC_druid_spell_scripts()
{
    RegisterSpellScript("spell_druid_reshift", &GetSpellScript<spell_druid_reshift>);
    RegisterSpellScript("spell_druid_swiftmend", &GetSpellScript<spell_druid_swiftmend>);
    RegisterSpellScript("spell_druid_healing_touch", &GetSpellScript<spell_druid_healing_touch>);
    RegisterSpellScript("spell_druid_berserk", &GetSpellScript<spell_druid_berserk>);
    RegisterSpellScript("spell_druid_enrage", &GetSpellScript<spell_druid_enrage>);
    RegisterSpellScript("spell_druid_shapeshift_root_snare_removal", &GetSpellScript<spell_druid_shapeshift_root_snare_removal>);
    RegisterSpellScript("spell_druid_open_wounds", &GetSpellScript<spell_druid_open_wounds>);
    RegisterSpellScript("spell_druid_ferocious_bite", &GetSpellScript<spell_druid_ferocious_bite>);
    RegisterSpellScript("spell_druid_starfire", &GetSpellScript<spell_druid_starfire>);
    RegisterSpellScript("spell_druid_idol_of_equilibrium", &GetSpellScript<spell_druid_idol_of_equilibrium>);
    RegisterAuraScript("spell_druid_aessinas_bloom", &GetAuraScript<spell_druid_aessinas_bloom>);
    RegisterAuraScript("spell_druid_thorns_explosion", &GetAuraScript<spell_druid_thorns_explosion>);
    RegisterAuraScript("spell_druid_balance_of_all_things", &GetAuraScript<spell_druid_balance_of_all_things>);
    RegisterAuraScript("spell_druid_carnage", &GetAuraScript<spell_druid_carnage>);
    RegisterAuraScript("spell_druid_eclipse", &GetAuraScript<spell_druid_eclipse>);
    RegisterAuraScript("spell_druid_frenzied_regeneration", &GetAuraScript<spell_druid_frenzied_regeneration>);
    RegisterAuraScript("spell_druid_efflorescence", &GetAuraScript<spell_druid_efflorescence>);
    RegisterSpellAndAuraScript("spell_druid_rip", &GetSpellScript<spell_druid_rip>, &GetAuraScript<spell_druid_rip>);
    RegisterAuraScript("spell_druid_moonclaw", &GetAuraScript<spell_druid_moonclaw>);
    RegisterAuraScript("spell_druid_berserk_form_swap", &GetAuraScript<spell_druid_berserk_form_swap>);
    RegisterAuraScript("spell_druid_tree_of_life_aura", &GetAuraScript<spell_druid_tree_of_life_aura>);
    RegisterAuraScript("spell_druid_preservation", &GetAuraScript<spell_druid_preservation>);
    RegisterAuraScript("spell_druid_glyph_of_the_moon", &GetAuraScript<spell_druid_glyph_of_the_moon>);
    RegisterAuraScript("spell_druid_primal_fury", &GetAuraScript<spell_druid_primal_fury>);
    RegisterAuraScript("spell_druid_cenarion_blessing", &GetAuraScript<spell_druid_cenarion_blessing>);
}
