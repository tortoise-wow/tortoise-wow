/*
 * Turtle WoW compatibility methods for Eluna's VMaNGOS adapter.
 *
 * Upstream implements ElunaSpellInfo for every supported core, but only the
 * Trinity-family adapters register it with Lua. Turtle stores its canonical
 * spell definitions in SpellEntry objects loaded from spell_template, so the
 * stable vanilla-compatible portion of the API can be exposed without copying
 * or retaining mutable database data.
 */

#ifndef TURTLE_ELUNA_CUSTOM_METHODS_H
#define TURTLE_ELUNA_CUSTOM_METHODS_H

#include "ElunaIncludes.h"
#include "ElunaSpellWrapper.h"
#include "ElunaTemplate.h"

namespace LuaCustom
{
namespace SpellInfoCompat
{
    inline SpellEntry const* GetEntry(ElunaSpellInfo* info)
    {
        return info->GetSpellInfo();
    }

    inline uint32 CheckIndex(Eluna* E, uint32 count, char const* kind)
    {
        uint32 index = E->CHECKVAL<uint32>(2);
        if (index >= count)
        {
            std::string message(kind);
            message += " index out of range";
            luaL_argerror(E->L, 2, message.c_str());
        }
        return index;
    }

#define TURTLE_ELUNA_SPELL_GETTER(name, value)                 \
    inline int name(Eluna* E, ElunaSpellInfo* info)            \
    {                                                           \
        SpellEntry const* spell = GetEntry(info);               \
        E->Push(value);                                         \
        return 1;                                               \
    }

    TURTLE_ELUNA_SPELL_GETTER(GetId, spell->Id)
    TURTLE_ELUNA_SPELL_GETTER(GetDispel, spell->Dispel)
    TURTLE_ELUNA_SPELL_GETTER(GetMechanic, spell->Mechanic)
    TURTLE_ELUNA_SPELL_GETTER(GetAttributes, spell->Attributes)
    TURTLE_ELUNA_SPELL_GETTER(GetAttributesEx, spell->AttributesEx)
    TURTLE_ELUNA_SPELL_GETTER(GetAttributesEx2, spell->AttributesEx2)
    TURTLE_ELUNA_SPELL_GETTER(GetAttributesEx3, spell->AttributesEx3)
    TURTLE_ELUNA_SPELL_GETTER(GetAttributesEx4, spell->AttributesEx4)
    TURTLE_ELUNA_SPELL_GETTER(GetStances, spell->Stances)
    TURTLE_ELUNA_SPELL_GETTER(GetStancesNot, spell->StancesNot)
    TURTLE_ELUNA_SPELL_GETTER(GetTargets, spell->Targets)
    TURTLE_ELUNA_SPELL_GETTER(GetTargetCreatureType, spell->TargetCreatureType)
    TURTLE_ELUNA_SPELL_GETTER(GetRequiresSpellFocus, spell->RequiresSpellFocus)
    TURTLE_ELUNA_SPELL_GETTER(GetCasterAuraState, spell->CasterAuraState)
    TURTLE_ELUNA_SPELL_GETTER(GetTargetAuraState, spell->TargetAuraState)
    TURTLE_ELUNA_SPELL_GETTER(GetCategory, spell->Category)
    TURTLE_ELUNA_SPELL_GETTER(GetRecoveryTime, spell->RecoveryTime)
    TURTLE_ELUNA_SPELL_GETTER(GetCategoryRecoveryTime, spell->CategoryRecoveryTime)
    TURTLE_ELUNA_SPELL_GETTER(GetStartRecoveryCategory, spell->StartRecoveryCategory)
    TURTLE_ELUNA_SPELL_GETTER(GetStartRecoveryTime, spell->StartRecoveryTime)
    TURTLE_ELUNA_SPELL_GETTER(GetInterruptFlags, spell->InterruptFlags)
    TURTLE_ELUNA_SPELL_GETTER(GetAuraInterruptFlags, spell->AuraInterruptFlags)
    TURTLE_ELUNA_SPELL_GETTER(GetChannelInterruptFlags, spell->ChannelInterruptFlags)
    TURTLE_ELUNA_SPELL_GETTER(GetProcFlags, spell->procFlags)
    TURTLE_ELUNA_SPELL_GETTER(GetProcChance, spell->procChance)
    TURTLE_ELUNA_SPELL_GETTER(GetProcCharges, spell->procCharges)
    TURTLE_ELUNA_SPELL_GETTER(GetMaxLevel, spell->maxLevel)
    TURTLE_ELUNA_SPELL_GETTER(GetBaseLevel, spell->baseLevel)
    TURTLE_ELUNA_SPELL_GETTER(GetSpellLevel, spell->spellLevel)
    TURTLE_ELUNA_SPELL_GETTER(GetPowerType, spell->powerType)
    TURTLE_ELUNA_SPELL_GETTER(GetManaCost, spell->manaCost)
    TURTLE_ELUNA_SPELL_GETTER(GetManaCostPerlevel, spell->manaCostPerlevel)
    TURTLE_ELUNA_SPELL_GETTER(GetManaPerSecond, spell->manaPerSecond)
    TURTLE_ELUNA_SPELL_GETTER(GetManaPerSecondPerLevel, spell->manaPerSecondPerLevel)
    TURTLE_ELUNA_SPELL_GETTER(GetManaCostPercentage, spell->ManaCostPercentage)
    TURTLE_ELUNA_SPELL_GETTER(GetSpeed, spell->speed)
    TURTLE_ELUNA_SPELL_GETTER(GetStackAmount, spell->StackAmount)
    TURTLE_ELUNA_SPELL_GETTER(GetEquippedItemClass, spell->EquippedItemClass)
    TURTLE_ELUNA_SPELL_GETTER(GetEquippedItemSubClassMask, spell->EquippedItemSubClassMask)
    TURTLE_ELUNA_SPELL_GETTER(GetEquippedItemInventoryTypeMask, spell->EquippedItemInventoryTypeMask)
    TURTLE_ELUNA_SPELL_GETTER(GetSpellVisual, spell->SpellVisual)
    TURTLE_ELUNA_SPELL_GETTER(GetSpellIconID, spell->SpellIconID)
    TURTLE_ELUNA_SPELL_GETTER(GetActiveIconID, spell->activeIconID)
    TURTLE_ELUNA_SPELL_GETTER(GetPriority, spell->spellPriority)
    TURTLE_ELUNA_SPELL_GETTER(GetMaxTargetLevel, spell->MaxTargetLevel)
    TURTLE_ELUNA_SPELL_GETTER(GetMaxAffectedTargets, spell->MaxAffectedTargets)
    TURTLE_ELUNA_SPELL_GETTER(GetSpellFamilyName, spell->SpellFamilyName)
    TURTLE_ELUNA_SPELL_GETTER(GetDmgClass, spell->DmgClass)
    TURTLE_ELUNA_SPELL_GETTER(GetPreventionType, spell->PreventionType)
    TURTLE_ELUNA_SPELL_GETTER(GetSchoolMask, static_cast<uint32>(spell->GetSpellSchoolMask()))
    TURTLE_ELUNA_SPELL_GETTER(GetDuration, spell->GetDuration())
    TURTLE_ELUNA_SPELL_GETTER(GetMaxDuration, spell->GetMaxDuration())
    TURTLE_ELUNA_SPELL_GETTER(GetMaxRange, Spells::GetSpellMaxRange(sSpellRangeStore.LookupEntry(spell->rangeIndex)))
    TURTLE_ELUNA_SPELL_GETTER(GetMinRange, Spells::GetSpellMinRange(sSpellRangeStore.LookupEntry(spell->rangeIndex)))
    TURTLE_ELUNA_SPELL_GETTER(GetMaxTicks, static_cast<uint32>(spell->GetAuraMaxTicks()))
    TURTLE_ELUNA_SPELL_GETTER(GetRank, static_cast<uint32>(sSpellMgr.GetSpellRank(spell->Id)))
    TURTLE_ELUNA_SPELL_GETTER(GetAllEffectsMechanicMask, spell->GetAllSpellMechanicMask())
    TURTLE_ELUNA_SPELL_GETTER(GetAttackType, static_cast<uint32>(spell->GetWeaponAttackType()))
    TURTLE_ELUNA_SPELL_GETTER(IsPassive, spell->IsPassiveSpell())
    TURTLE_ELUNA_SPELL_GETTER(CanBeUsedInCombat, !spell->IsNonCombatSpell())
    TURTLE_ELUNA_SPELL_GETTER(IsPositive, spell->IsPositiveSpell())
    TURTLE_ELUNA_SPELL_GETTER(IsChanneled, spell->IsChanneledSpell())
    TURTLE_ELUNA_SPELL_GETTER(NeedsComboPoints, spell->NeedsComboPoints())
    TURTLE_ELUNA_SPELL_GETTER(IsAutoRepeatRangedSpell, spell->IsAutoRepeatRangedSpell())
    TURTLE_ELUNA_SPELL_GETTER(IsRanked, sSpellMgr.GetSpellRank(spell->Id) != 0)
    TURTLE_ELUNA_SPELL_GETTER(IsAffectingArea, spell->IsAreaOfEffectSpell())
    TURTLE_ELUNA_SPELL_GETTER(IsTargetingArea, spell->IsAreaOfEffectSpell())
    TURTLE_ELUNA_SPELL_GETTER(HasAreaAuraEffect, spell->HasAreaAuraEffect())

#undef TURTLE_ELUNA_SPELL_GETTER

#define TURTLE_ELUNA_SPELL_INDEXED_GETTER(name, count, kind, value) \
    inline int name(Eluna* E, ElunaSpellInfo* info)                 \
    {                                                               \
        SpellEntry const* spell = GetEntry(info);                   \
        uint32 index = CheckIndex(E, count, kind);                   \
        E->Push(value);                                             \
        return 1;                                                   \
    }

    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetTotem, MAX_SPELL_TOTEMS, "totem", spell->Totem[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetReagent, MAX_SPELL_REAGENTS, "reagent", spell->Reagent[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetReagentCount, MAX_SPELL_REAGENTS, "reagent", spell->ReagentCount[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectType, MAX_EFFECT_INDEX, "effect", spell->Effect[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectApplyAuraName, MAX_EFFECT_INDEX, "effect", spell->EffectApplyAuraName[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectAmplitude, MAX_EFFECT_INDEX, "effect", spell->EffectAmplitude[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectDieSides, MAX_EFFECT_INDEX, "effect", spell->EffectDieSides[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectRealPointsPerLevel, MAX_EFFECT_INDEX, "effect", spell->EffectRealPointsPerLevel[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectBasePoints, MAX_EFFECT_INDEX, "effect", spell->EffectBasePoints[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectPointsPerComboPoint, MAX_EFFECT_INDEX, "effect", spell->EffectPointsPerComboPoint[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectValueMultiplier, MAX_EFFECT_INDEX, "effect", spell->EffectMultipleValue[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectDamageMultiplier, MAX_EFFECT_INDEX, "effect", spell->DmgMultiplier[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectBonusMultiplier, MAX_EFFECT_INDEX, "effect", spell->EffectBonusCoefficient[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectMiscValue, MAX_EFFECT_INDEX, "effect", spell->EffectMiscValue[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectChainTarget, MAX_EFFECT_INDEX, "effect", spell->EffectChainTarget[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectItemType, MAX_EFFECT_INDEX, "effect", spell->EffectItemType[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectTriggerSpell, MAX_EFFECT_INDEX, "effect", spell->EffectTriggerSpell[index])
    TURTLE_ELUNA_SPELL_INDEXED_GETTER(GetEffectMechanic, MAX_EFFECT_INDEX, "effect", static_cast<uint32>(spell->GetEffectMechanic(static_cast<SpellEffectIndex>(index))))

#undef TURTLE_ELUNA_SPELL_INDEXED_GETTER

    inline int GetSpellFamilyFlags(Eluna* E, ElunaSpellInfo* info)
    {
        uint32 index = CheckIndex(E, 3, "spell family flag");
        uint64 flags = GetEntry(info)->SpellFamilyFlags;
        E->Push(index < 2 ? static_cast<uint32>(flags >> (index * 32)) : uint32(0));
        return 1;
    }

    inline int GetSpellMechanicMaskByEffectMask(Eluna* E, ElunaSpellInfo* info)
    {
        uint32 effectMask = E->CHECKVAL<uint32>(2);
        E->Push(GetEntry(info)->GetSpellMechanicMask(effectMask));
        return 1;
    }

    inline int HasEffect(Eluna* E, ElunaSpellInfo* info)
    {
        uint32 effect = E->CHECKVAL<uint32>(2);
        E->Push(GetEntry(info)->HasEffect(static_cast<SpellEffects>(effect)));
        return 1;
    }

    inline int HasAura(Eluna* E, ElunaSpellInfo* info)
    {
        uint32 aura = E->CHECKVAL<uint32>(2);
        E->Push(GetEntry(info)->HasAura(static_cast<AuraType>(aura)));
        return 1;
    }

    inline int IsPositiveEffect(Eluna* E, ElunaSpellInfo* info)
    {
        uint32 index = CheckIndex(E, MAX_EFFECT_INDEX, "effect");
        E->Push(GetEntry(info)->IsPositiveEffect(static_cast<SpellEffectIndex>(index)));
        return 1;
    }

    inline int IsRankOf(Eluna* E, ElunaSpellInfo* info)
    {
        ElunaSpellInfo* other = E->CHECKOBJ<ElunaSpellInfo>(2);
        uint32 first = sSpellMgr.GetFirstSpellInChain(GetEntry(info)->Id);
        uint32 otherFirst = sSpellMgr.GetFirstSpellInChain(GetEntry(other)->Id);
        E->Push(first == otherFirst);
        return 1;
    }

    inline int IsDifferentRankOf(Eluna* E, ElunaSpellInfo* info)
    {
        ElunaSpellInfo* other = E->CHECKOBJ<ElunaSpellInfo>(2);
        SpellEntry const* spell = GetEntry(info);
        SpellEntry const* otherSpell = GetEntry(other);
        E->Push(spell->Id != otherSpell->Id &&
            sSpellMgr.GetFirstSpellInChain(spell->Id) == sSpellMgr.GetFirstSpellInChain(otherSpell->Id));
        return 1;
    }

    inline int IsHighRankOf(Eluna* E, ElunaSpellInfo* info)
    {
        ElunaSpellInfo* other = E->CHECKOBJ<ElunaSpellInfo>(2);
        E->Push(sSpellMgr.IsHighRankOfSpell(GetEntry(info)->Id, GetEntry(other)->Id));
        return 1;
    }

    ElunaRegister<ElunaSpellInfo> Methods[] =
    {
        { "GetId", &GetId },
        { "GetDispel", &GetDispel },
        { "GetMechanic", &GetMechanic },
        { "GetAttributes", &GetAttributes },
        { "GetAttributesEx", &GetAttributesEx },
        { "GetAttributesEx2", &GetAttributesEx2 },
        { "GetAttributesEx3", &GetAttributesEx3 },
        { "GetAttributesEx4", &GetAttributesEx4 },
        { "GetStances", &GetStances },
        { "GetStancesNot", &GetStancesNot },
        { "GetTargets", &GetTargets },
        { "GetTargetCreatureType", &GetTargetCreatureType },
        { "GetRequiresSpellFocus", &GetRequiresSpellFocus },
        { "GetCasterAuraState", &GetCasterAuraState },
        { "GetTargetAuraState", &GetTargetAuraState },
        { "GetCategory", &GetCategory },
        { "GetRecoveryTime", &GetRecoveryTime },
        { "GetCategoryRecoveryTime", &GetCategoryRecoveryTime },
        { "GetStartRecoveryCategory", &GetStartRecoveryCategory },
        { "GetStartRecoveryTime", &GetStartRecoveryTime },
        { "GetInterruptFlags", &GetInterruptFlags },
        { "GetAuraInterruptFlags", &GetAuraInterruptFlags },
        { "GetChannelInterruptFlags", &GetChannelInterruptFlags },
        { "GetProcFlags", &GetProcFlags },
        { "GetProcChance", &GetProcChance },
        { "GetProcCharges", &GetProcCharges },
        { "GetMaxLevel", &GetMaxLevel },
        { "GetBaseLevel", &GetBaseLevel },
        { "GetSpellLevel", &GetSpellLevel },
        { "GetPowerType", &GetPowerType },
        { "GetManaCost", &GetManaCost },
        { "GetManaCostPerlevel", &GetManaCostPerlevel },
        { "GetManaPerSecond", &GetManaPerSecond },
        { "GetManaPerSecondPerLevel", &GetManaPerSecondPerLevel },
        { "GetManaCostPercentage", &GetManaCostPercentage },
        { "GetSpeed", &GetSpeed },
        { "GetStackAmount", &GetStackAmount },
        { "GetTotem", &GetTotem },
        { "GetReagent", &GetReagent },
        { "GetReagentCount", &GetReagentCount },
        { "GetEquippedItemClass", &GetEquippedItemClass },
        { "GetEquippedItemSubClassMask", &GetEquippedItemSubClassMask },
        { "GetEquippedItemInventoryTypeMask", &GetEquippedItemInventoryTypeMask },
        { "GetSpellVisual", &GetSpellVisual },
        { "GetSpellIconID", &GetSpellIconID },
        { "GetActiveIconID", &GetActiveIconID },
        { "GetPriority", &GetPriority },
        { "GetMaxTargetLevel", &GetMaxTargetLevel },
        { "GetMaxAffectedTargets", &GetMaxAffectedTargets },
        { "GetSpellFamilyName", &GetSpellFamilyName },
        { "GetSpellFamilyFlags", &GetSpellFamilyFlags },
        { "GetDmgClass", &GetDmgClass },
        { "GetPreventionType", &GetPreventionType },
        { "GetSchoolMask", &GetSchoolMask },
        { "GetDuration", &GetDuration },
        { "GetMaxDuration", &GetMaxDuration },
        { "GetMaxRange", &GetMaxRange },
        { "GetMinRange", &GetMinRange },
        { "GetMaxTicks", &GetMaxTicks },
        { "GetRank", &GetRank },
        { "GetAllEffectsMechanicMask", &GetAllEffectsMechanicMask },
        { "GetAttackType", &GetAttackType },
        { "GetSpellMechanicMaskByEffectMask", &GetSpellMechanicMaskByEffectMask },
        { "IsPassive", &IsPassive },
        { "CanBeUsedInCombat", &CanBeUsedInCombat },
        { "IsPositive", &IsPositive },
        { "IsChanneled", &IsChanneled },
        { "NeedsComboPoints", &NeedsComboPoints },
        { "IsAutoRepeatRangedSpell", &IsAutoRepeatRangedSpell },
        { "IsRanked", &IsRanked },
        { "IsAffectingArea", &IsAffectingArea },
        { "IsTargetingArea", &IsTargetingArea },
        { "HasAreaAuraEffect", &HasAreaAuraEffect },
        { "HasEffect", &HasEffect },
        { "HasAura", &HasAura },
        { "IsPositiveEffect", &IsPositiveEffect },
        { "IsRankOf", &IsRankOf },
        { "IsDifferentRankOf", &IsDifferentRankOf },
        { "IsHighRankOf", &IsHighRankOf },
        { "GetEffectType", &GetEffectType },
        { "GetEffectApplyAuraName", &GetEffectApplyAuraName },
        { "GetEffectAmplitude", &GetEffectAmplitude },
        { "GetEffectDieSides", &GetEffectDieSides },
        { "GetEffectRealPointsPerLevel", &GetEffectRealPointsPerLevel },
        { "GetEffectBasePoints", &GetEffectBasePoints },
        { "GetEffectPointsPerComboPoint", &GetEffectPointsPerComboPoint },
        { "GetEffectValueMultiplier", &GetEffectValueMultiplier },
        { "GetEffectDamageMultiplier", &GetEffectDamageMultiplier },
        { "GetEffectBonusMultiplier", &GetEffectBonusMultiplier },
        { "GetEffectMiscValue", &GetEffectMiscValue },
        { "GetEffectChainTarget", &GetEffectChainTarget },
        { "GetEffectItemType", &GetEffectItemType },
        { "GetEffectTriggerSpell", &GetEffectTriggerSpell },
        { "GetEffectMechanic", &GetEffectMechanic }
    };
}

    inline int AuraGetSpellInfo(Eluna* E, Aura* aura)
    {
        ElunaSpellInfo info(aura->GetId());
        E->Push(&info);
        return 1;
    }

    inline int SpellGetSpellInfo(Eluna* E, Spell* spell)
    {
        ElunaSpellInfo info(spell->GetSpellInfo()->Id);
        E->Push(&info);
        return 1;
    }

    inline int GlobalGetSpellInfo(Eluna* E)
    {
        uint32 spellId = E->CHECKVAL<uint32>(1);
        if (!sSpellMgr.GetSpellEntry(spellId))
            return luaL_argerror(E->L, 1, "invalid spell id");

        ElunaSpellInfo info(spellId);
        E->Push(&info);
        return 1;
    }

    inline int GlobalGetCoreName(Eluna* E)
    {
        // ELUNA_VMANGOS selects the closest compatible Eluna API surface; it
        // does not change this fork's MaNGOS identity.
        E->Push("MaNGOS");
        return 1;
    }

    inline int GlobalGetCoreVersion(Eluna* E)
    {
        E->Push(REVISION_HASH);
        return 1;
    }

    inline bool HasGlobal(Eluna* E, char const* name)
    {
        lua_getglobal(E->L, name);
        bool exists = !lua_isnoneornil(E->L, -1);
        lua_pop(E->L, 1);
        return exists;
    }

    inline bool HasMethod(Eluna* E, char const* typeName, char const* methodName)
    {
        lua_getglobal(E->L, typeName);
        if (!lua_istable(E->L, -1))
        {
            lua_pop(E->L, 1);
            return false;
        }

        lua_getfield(E->L, -1, methodName);
        bool exists = !lua_isnoneornil(E->L, -1);
        lua_pop(E->L, 2);
        return exists;
    }

    ElunaRegister<Aura> AuraMethods[] =
    {
        { "GetSpellInfo", &AuraGetSpellInfo }
    };

    ElunaRegister<Spell> SpellMethods[] =
    {
        { "GetSpellInfo", &SpellGetSpellInfo }
    };

    ElunaRegister<> GlobalSpellInfoMethods[] =
    {
        { "GetSpellInfo", &GlobalGetSpellInfo }
    };

    ElunaRegister<> GlobalIdentityMethods[] =
    {
        { "GetCoreName", &GlobalGetCoreName },
        { "GetCoreVersion", &GlobalGetCoreVersion }
    };

    inline void RegisterCustomMethods(Eluna* E)
    {
        // Prefer native support automatically if a future Eluna revision adds
        // these bindings to its VMaNGOS backend.
        if (!HasGlobal(E, "ElunaSpellInfo"))
        {
            ElunaTemplate<ElunaSpellInfo>::Register(E, "ElunaSpellInfo");
            ElunaTemplate<ElunaSpellInfo>::SetMethods(E, SpellInfoCompat::Methods);
        }

        if (!HasMethod(E, "Aura", "GetSpellInfo"))
            ElunaTemplate<Aura>::SetMethods(E, AuraMethods);
        if (!HasMethod(E, "Spell", "GetSpellInfo"))
            ElunaTemplate<Spell>::SetMethods(E, SpellMethods);
        if (!HasGlobal(E, "GetSpellInfo"))
            ElunaTemplate<>::SetMethods(E, GlobalSpellInfoMethods);

        // These intentionally replace the compatibility backend's identity.
        ElunaTemplate<>::SetMethods(E, GlobalIdentityMethods);
    }
}

#endif
