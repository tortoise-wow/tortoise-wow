#include "scriptPCH.h"
#include "DynamicObject.h"
#include "Pet.h"
#include "Totem.h"

namespace
{
enum ShamanSpells
{
    SPELL_SHAMAN_IMPROVED_MOLTEN_BLAST_R1 = 46107,
    SPELL_SHAMAN_IMPROVED_MOLTEN_BLAST_R2 = 46108,
    SPELL_SHAMAN_MOLTEN_CORE              = 46110,
    SPELL_SHAMAN_REKINDLED_FLAME          = 46109,
    SPELL_SHAMAN_ELEMENTAL_WEAPONS_R1     = 16266,
    SPELL_SHAMAN_ELEMENTAL_WEAPONS_R2     = 29079,
    SPELL_SHAMAN_ELEMENTAL_WEAPONS_R3     = 29080,
    SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R1  = 58248,
    SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R2  = 58249,
    SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R3  = 58250,
    SPELL_SHAMAN_EARTHEN_BULWARK_R1       = 58128,
    SPELL_SHAMAN_EARTHEN_BULWARK_R2       = 58129,
    SPELL_SHAMAN_EARTHEN_BULWARK_R3       = 58130,
    SPELL_SHAMAN_EARTH_SHIELD_R1          = 45525,
    SPELL_SHAMAN_EARTH_SHIELD_R2          = 51525,
    SPELL_SHAMAN_EARTH_SHIELD_R3          = 51526,
    SPELL_SHAMAN_CALL_OF_EARTH_R1         = 58238,
    SPELL_SHAMAN_CALL_OF_EARTH_R2         = 58239,
    SPELL_SHAMAN_CLEARCASTING             = 16246,
    SPELL_SHAMAN_IMPROVED_CLEARCASTING    = 46761,
    SPELL_SHAMAN_IMPROVED_CLEARCASTING_SET = 46762,
    SPELL_SHAMAN_FLAMETONGUE_DAMAGE_BONUS = 51838,
    SPELL_SHAMAN_ELEMENTAL_SHELL          = 51847,
    SPELL_SHAMAN_ELEMENTAL_SHELL_PASSIVE  = 51848,
    SPELL_SHAMAN_TIDAL_WAVE_PASSIVE       = 41826, // Totem of Tides (58146) equip aura
    SPELL_SHAMAN_TIDAL_WAVE               = 41825, // 25-32 frost, enemies within 10 yd
    SPELL_SHAMAN_LIGHTNING_STRIKE_NATURE_R1 = 51386,
    SPELL_SHAMAN_LIGHTNING_STRIKE_NATURE_R2 = 52419,
    SPELL_SHAMAN_LIGHTNING_STRIKE_NATURE_R3 = 52421,
    SPELL_SHAMAN_STORMSTRIKE_AURA        = 52412,
    SPELL_SHAMAN_TOTEMIC_SLAM            = 45500,
    SPELL_SHAMAN_HEX                     = 45504,
    SPELL_SHAMAN_ANCIENT_RITES_TOTEMIC_SLAM = 58241,
    SPELL_SHAMAN_ANCIENT_RITES_FERAL_SPIRIT = 58242,
    SPELL_SHAMAN_ANCIENT_RITES_HEX       = 58243,
    SPELL_SHAMAN_ANCIENT_RITES_KNOCKDOWN = 58244,
    SPELL_SHAMAN_ANCIENT_RITES_FROGS     = 58245,
    SPELL_SHAMAN_ANCIENT_RITES_LUNGE     = 58246,
    SPELL_SHAMAN_ANCIENT_RITES_LUNGE_DAMAGE = 58247,
    SPELL_SHAMAN_THUNDERCALL             = 52872,
    SPELL_SHAMAN_HEALING_WAY             = 29203,
    SPELL_SHAMAN_IMPROVED_FIRE_TOTEMS_R1 = 16086,
    SPELL_SHAMAN_IMPROVED_FIRE_TOTEMS_R2 = 16544,
    SPELL_SHAMAN_IMPROVED_SEARING_TOTEM_R1 = 56558,
    SPELL_SHAMAN_IMPROVED_SEARING_TOTEM_R2 = 56559,
    SPELL_SHAMAN_ENHANCING_TOTEMS_R1     = 16259,
    SPELL_SHAMAN_ENHANCING_TOTEMS_R2     = 16295,
    SPELL_SHAMAN_IMPROVED_WATER_SHIELD_R1 = 51369,
    SPELL_SHAMAN_IMPROVED_WATER_SHIELD_R2 = 51370,
    SPELL_SHAMAN_IMPROVED_WATER_SHIELD_R3 = 51371,
    SPELL_SHAMAN_UNDERTOW_R1             = 51372,
    SPELL_SHAMAN_UNDERTOW_R2             = 51373,
    SPELL_SHAMAN_TIDAL_SURGE_R1          = 51491,
    SPELL_SHAMAN_TIDAL_SURGE_R2          = 51492,
    SPELL_SHAMAN_CALMING_WINDS_R1        = 51383,
    SPELL_SHAMAN_CALMING_WINDS_R2        = 51384,
    SPELL_SHAMAN_CALMING_WINDS_R3        = 51385,
    SPELL_SHAMAN_EARTHQUAKE_R1           = 48306,
    SPELL_SHAMAN_EARTHQUAKE_R2           = 48307,
    SPELL_SHAMAN_EARTHQUAKE_R3           = 48308,
    SPELL_SHAMAN_EARTHQUAKE_SPLASH       = 52878,
    SPELL_SHAMAN_EARTHQUAKE_AFTERSHOCK   = 51489,
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

SpellAuraHolder* GetCasterFlameShock(Unit* caster, Unit* target)
{
    if (!caster || !target)
        return nullptr;

    uint32 const flameShockRanks[] = {8050, 8052, 8053, 10447, 10448, 29228};
    for (uint32 spellId : flameShockRanks)
        if (SpellAuraHolder* holder = target->GetSpellAuraHolder(spellId, caster->GetObjectGuid()))
            return holder;

    return nullptr;
}

bool IsElementalWeaponsSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_ELEMENTAL_WEAPONS_R1:
        case SPELL_SHAMAN_ELEMENTAL_WEAPONS_R2:
        case SPELL_SHAMAN_ELEMENTAL_WEAPONS_R3:
            return true;
        default:
            return false;
    }
}

bool IsRockbiterEnchantment(uint32 enchantmentId)
{
    switch (enchantmentId)
    {
        case 1:
        case 6:
        case 29:
        case 503:
        case 683:
        case 1663:
        case 1664:
            return true;
        default:
            return false;
    }
}

bool IsEarthenBulwarkSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_EARTHEN_BULWARK_R1:
        case SPELL_SHAMAN_EARTHEN_BULWARK_R2:
        case SPELL_SHAMAN_EARTHEN_BULWARK_R3:
            return true;
        default:
            return false;
    }
}

bool IsCallOfEarthSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_CALL_OF_EARTH_R1:
        case SPELL_SHAMAN_CALL_OF_EARTH_R2:
            return true;
        default:
            return false;
    }
}

bool IsLightningStrikeNatureDamageSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_LIGHTNING_STRIKE_NATURE_R1:
        case SPELL_SHAMAN_LIGHTNING_STRIKE_NATURE_R2:
        case SPELL_SHAMAN_LIGHTNING_STRIKE_NATURE_R3:
            return true;
        default:
            return false;
    }
}

bool IsHealingWayAffectedSpell(SpellEntry const* spellInfo)
{
    return spellInfo && spellInfo->IsFitToFamily<SPELLFAMILY_SHAMAN,
        CF_SHAMAN_HEALING_WAVE,
        CF_SHAMAN_LESSER_HEALING_WAVE,
        CF_SHAMAN_CHAIN_HEAL>();
}

bool IsLightningShieldSpell(SpellEntry const* spellInfo)
{
    return spellInfo && spellInfo->IsFitToFamily<SPELLFAMILY_SHAMAN, CF_SHAMAN_LIGHTNING_SHIELD>();
}

bool IsWaterShieldSpell(SpellEntry const* spellInfo)
{
    return spellInfo && spellInfo->IsFitToFamily<SPELLFAMILY_SHAMAN, CF_SHAMAN_WATER_SHIELD_EFFECT>();
}

bool IsImprovedFireTotemsSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_IMPROVED_FIRE_TOTEMS_R1:
        case SPELL_SHAMAN_IMPROVED_FIRE_TOTEMS_R2:
            return true;
        default:
            return false;
    }
}

bool IsUndertowSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_UNDERTOW_R1:
        case SPELL_SHAMAN_UNDERTOW_R2:
            return true;
        default:
            return false;
    }
}

bool IsTidalSurgeSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_TIDAL_SURGE_R1:
        case SPELL_SHAMAN_TIDAL_SURGE_R2:
            return true;
        default:
            return false;
    }
}

bool IsCalmingWindsSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_CALMING_WINDS_R1:
        case SPELL_SHAMAN_CALMING_WINDS_R2:
        case SPELL_SHAMAN_CALMING_WINDS_R3:
            return true;
        default:
            return false;
    }
}

bool IsEarthquakeSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_EARTHQUAKE_R1:
        case SPELL_SHAMAN_EARTHQUAKE_R2:
        case SPELL_SHAMAN_EARTHQUAKE_R3:
            return true;
        default:
            return false;
    }
}

bool IsEarthShieldSpell(SpellEntry const* spellInfo)
{
    return spellInfo && spellInfo->IsFitToFamily<SPELLFAMILY_SHAMAN, CF_SHAMAN_EARTH_SHIELD>();
}

bool HasEarthShield(Unit const* owner)
{
    if (!owner)
        return false;

    Unit::SpellAuraHolderMap const& auras = owner->GetSpellAuraHolderMap();
    for (auto const& itr : auras)
        if (IsEarthShieldSpell(itr.second->GetSpellProto()))
            return true;

    return false;
}

Aura* GetStormstrikeAura(Unit* owner)
{
    return owner ? owner->GetAura(SPELL_SHAMAN_STORMSTRIKE_AURA, EFFECT_INDEX_0) : nullptr;
}

uint32 GetElementalWeaponsFrostbrandSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_ELEMENTAL_WEAPONS_R1: return SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R1;
        case SPELL_SHAMAN_ELEMENTAL_WEAPONS_R2: return SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R2;
        case SPELL_SHAMAN_ELEMENTAL_WEAPONS_R3: return SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R3;
        default: return 0;
    }
}

void RemoveElementalWeaponsFrostbrandAuras(Unit* target)
{
    if (!target)
        return;

    target->RemoveAurasDueToSpell(SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R1);
    target->RemoveAurasDueToSpell(SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R2);
    target->RemoveAurasDueToSpell(SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R3);
}

bool HasRockbiterEnchantmentForSwing(Unit const* owner, uint32 procFlag)
{
    Player const* player = owner ? owner->ToPlayer() : nullptr;
    if (!player)
        return false;

    Item const* weapon = player->GetItemByPos(INVENTORY_SLOT_BAG_0, (procFlag & PROC_FLAG_OFF_HAND_WEAPON_SWING) ? EQUIPMENT_SLOT_OFFHAND : EQUIPMENT_SLOT_MAINHAND);
    return weapon && IsRockbiterEnchantment(weapon->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
}

bool HasRockbiterEnchantment(Unit const* owner)
{
    Player const* player = owner ? owner->ToPlayer() : nullptr;
    if (!player)
        return false;

    Item const* mainHand = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
    if (mainHand && IsRockbiterEnchantment(mainHand->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT)))
        return true;

    Item const* offHand = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    return offHand && IsRockbiterEnchantment(offHand->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT));
}

bool HasEarthenBulwark(Unit const* owner)
{
    return owner &&
           (owner->HasAura(SPELL_SHAMAN_EARTHEN_BULWARK_R1) ||
            owner->HasAura(SPELL_SHAMAN_EARTHEN_BULWARK_R2) ||
            owner->HasAura(SPELL_SHAMAN_EARTHEN_BULWARK_R3));
}

bool HasShieldEquipped(Unit const* owner)
{
    Player const* player = owner ? owner->ToPlayer() : nullptr;
    if (!player)
        return false;

    Item const* offhand = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
    return offhand && offhand->GetProto()->Class == ITEM_CLASS_ARMOR && offhand->GetProto()->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD;
}

bool IsDamageDealtProc(uint32 procFlag, uint32 procEx)
{
    if (procFlag & PROC_FLAG_DEAL_HARMFUL_PERIODIC)
        return !(procEx & PROC_EX_PERIODIC_POSITIVE);

    uint32 const damageFlags = PROC_FLAG_DEAL_MELEE_SWING |
                               PROC_FLAG_DEAL_MELEE_ABILITY |
                               PROC_FLAG_DEAL_RANGED_ATTACK |
                               PROC_FLAG_DEAL_RANGED_ABILITY |
                               PROC_FLAG_DEAL_HARMFUL_ABILITY |
                               PROC_FLAG_DEAL_HARMFUL_SPELL;

    return (procFlag & damageFlags) &&
           (procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT));
}

uint32 CalculatePct(uint32 value, uint32 pct)
{
    return uint32(uint64(value) * pct / 100);
}

uint32 GetEarthenBulwarkBuildPct(Aura const* aura)
{
    if (!aura)
        return 0;

    return std::max<int32>(aura->GetModifier()->m_amount, 0);
}

uint32 GetEarthenBulwarkBuildAmount(Unit const* owner, Aura const* aura, uint32 damage)
{
    uint32 buildAmount = CalculatePct(damage, GetEarthenBulwarkBuildPct(aura));
    if (HasShieldEquipped(owner))
        buildAmount *= 3;

    return buildAmount;
}

uint32 GetEarthenBulwarkCap(Unit* owner, Aura const* aura)
{
    return owner ? CalculatePct(owner->GetMaxHealth(), GetEarthenBulwarkBuildPct(aura)) : 0;
}

int32 GetEarthenBulwarkAbsorbPct(Aura const* aura)
{
    if (!aura || !IsEarthenBulwarkSpell(aura->GetId()))
        return 0;

    return std::max<int32>(aura->GetSpellProto()->CalculateSimpleValue(aura->GetEffIndex()), 0);
}

void CastElementalWeaponsTrigger(Unit* owner, Aura* aura, uint32 amount = 0)
{
    if (!owner || !aura)
        return;

    uint32 const triggerSpellId = aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()];
    if (!triggerSpellId)
        return;

    if (amount)
    {
        uint32 const cap = GetEarthenBulwarkCap(owner, aura);
        if (!cap)
            return;

        if (Aura* existingAura = owner->GetAura(triggerSpellId, EFFECT_INDEX_0))
        {
            int32 newAmount = std::min<int32>(existingAura->GetModifier()->m_amount + amount, cap);
            existingAura->GetModifier()->m_amount = newAmount;
            existingAura->GetHolder()->RefreshHolder();
            return;
        }

        int32 basePoints = std::min<uint32>(amount, cap);
        owner->CastCustomSpell(owner, triggerSpellId, &basePoints, nullptr, nullptr, true, nullptr, aura);
        return;
    }

    owner->CastSpell(owner, triggerSpellId, true, nullptr, aura);
}

bool HasElementalWeaponsFrostbrand(Unit* caster)
{
    return caster &&
           (caster->HasAura(SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R1) ||
            caster->HasAura(SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R2) ||
            caster->HasAura(SPELL_SHAMAN_ELEMENTAL_WEAPONS_FB_R3) ||
            caster->HasAura(SPELL_SHAMAN_ELEMENTAL_WEAPONS_R1) ||
            caster->HasAura(SPELL_SHAMAN_ELEMENTAL_WEAPONS_R2) ||
            caster->HasAura(SPELL_SHAMAN_ELEMENTAL_WEAPONS_R3));
}

bool TargetHasCasterFrostShock(Unit const* target, Unit const* caster)
{
    if (!target || !caster)
        return false;

    uint32 const frostShockRanks[] = {8056, 8058, 10472, 10473};
    for (uint32 spellId : frostShockRanks)
        if (target->GetSpellAuraHolder(spellId, caster->GetObjectGuid()))
            return true;

    return false;
}

int32 GetImprovedMoltenBlastPct(Unit* caster)
{
    if (!caster)
        return 0;

    if (caster->HasAura(SPELL_SHAMAN_IMPROVED_MOLTEN_BLAST_R2, EFFECT_INDEX_0))
        if (SpellEntry const* talent = sSpellMgr.GetSpellEntry(SPELL_SHAMAN_IMPROVED_MOLTEN_BLAST_R2))
            return talent->CalculateSimpleValue(EFFECT_INDEX_0);

    if (caster->HasAura(SPELL_SHAMAN_IMPROVED_MOLTEN_BLAST_R1, EFFECT_INDEX_0))
        if (SpellEntry const* talent = sSpellMgr.GetSpellEntry(SPELL_SHAMAN_IMPROVED_MOLTEN_BLAST_R1))
            return talent->CalculateSimpleValue(EFFECT_INDEX_0);

    return 0;
}

int32 CalculateImprovedMoltenBlastDamage(Unit* caster, SpellAuraHolder* flameShock)
{
    int32 pct = GetImprovedMoltenBlastPct(caster);
    if (pct <= 0 || !flameShock)
        return 0;

    if (caster->HasAura(SPELL_SHAMAN_MOLTEN_CORE, EFFECT_INDEX_1))
        if (SpellEntry const* moltenCore = sSpellMgr.GetSpellEntry(SPELL_SHAMAN_MOLTEN_CORE))
            pct += moltenCore->CalculateSimpleValue(EFFECT_INDEX_1);

    Aura* periodicAura = flameShock->GetAuraByEffectIndex(EFFECT_INDEX_1);
    if (!periodicAura)
        return 0;

    int32 const remainingDuration = flameShock->GetAuraDuration();
    uint32 const period = periodicAura->GetModifier()->periodictime;
    int32 const tickDamage = periodicAura->GetModifier()->m_amount;
    if (remainingDuration <= 0 || !period || tickDamage <= 0)
        return 0;

    int32 const remainingDamage = int32(float(tickDamage) * float(remainingDuration) / float(period));
    return remainingDamage * pct / 100;
}

void FocusSearingTotem(Unit* caster, Unit* target)
{
    if (!caster || !target)
        return;

    Totem* totem = caster->GetTotem(TOTEM_SLOT_FIRE);
    if (!totem || !totem->IsAlive())
        return;

    SpellEntry const* totemSpell = sSpellMgr.GetSpellEntry(totem->GetUInt32Value(UNIT_CREATED_BY_SPELL));
    if (!totemSpell || !totemSpell->IsFitToFamily<SPELLFAMILY_SHAMAN, CF_SHAMAN_SEARING_TOTEM>())
        return;

    if (totem->AI())
        totem->AI()->AttackStart(target);
}

uint32 GetLightningShieldDamageSpell(uint32 spellId)
{
    switch (spellId)
    {
        case 324: return 26364;
        case 325: return 26365;
        case 905: return 26366;
        case 945: return 26367;
        case 8134: return 26369;
        case 10431: return 26370;
        case 10432: return 26363;
        default: return 0;
    }
}

Aura* GetActiveElementalShield(Unit* owner)
{
    if (!owner)
        return nullptr;

    uint32 const shieldRanks[] =
    {
        324, 325, 905, 945, 8134, 10431, 10432,
        45527, 51533, 51534, 51535, 51536,
        45525, 51525, 51526,
    };

    for (uint32 spellId : shieldRanks)
        if (Aura* aura = owner->GetAura(spellId, EFFECT_INDEX_0))
            return aura;

    return nullptr;
}

SpellAuraHolder* GetActiveEarthShieldHolder(Unit* owner)
{
    if (!owner)
        return nullptr;

    uint32 const earthShieldRanks[] =
    {
        SPELL_SHAMAN_EARTH_SHIELD_R1,
        SPELL_SHAMAN_EARTH_SHIELD_R2,
        SPELL_SHAMAN_EARTH_SHIELD_R3,
    };

    for (uint32 spellId : earthShieldRanks)
        if (SpellAuraHolder* holder = owner->GetSpellAuraHolder(spellId))
            return holder;

    return nullptr;
}

SpellAuraHolder* GetActiveWaterShieldHolder(Unit* owner)
{
    if (!owner)
        return nullptr;

    uint32 const waterShieldRanks[] = {45527, 51533, 51534, 51535, 51536};
    for (uint32 spellId : waterShieldRanks)
        if (SpellAuraHolder* holder = owner->GetSpellAuraHolder(spellId))
            return holder;

    return nullptr;
}

uint32 GetWaterShieldMaxCharges(Unit* owner, SpellAuraHolder* holder)
{
    if (!owner || !holder)
        return 0;

    uint32 charges = holder->GetSpellProto()->procCharges;
    if (Player* modOwner = owner->GetSpellModOwner())
        modOwner->ApplySpellMod(holder->GetId(), SPELLMOD_CHARGES, charges);

    return charges;
}

uint32 GetWaterShieldCharges(Unit const* owner)
{
    Unit* mutableOwner = const_cast<Unit*>(owner);
    if (SpellAuraHolder* holder = GetActiveWaterShieldHolder(mutableOwner))
        return holder->GetAuraCharges();

    return 0;
}

uint32 GetEarthShieldMaxCharges(Unit* owner, SpellAuraHolder* holder)
{
    if (!owner || !holder)
        return 0;

    uint32 charges = holder->GetSpellProto()->procCharges;
    if (Player* modOwner = owner->GetSpellModOwner())
        modOwner->ApplySpellMod(holder->GetId(), SPELLMOD_CHARGES, charges);

    return charges;
}

bool IsOffensiveNatureDirectSpell(Unit const* owner, Unit* victim, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, bool isVictim)
{
    if (!owner || !victim || !procSpell || isVictim)
        return false;

    if (!owner->IsValidAttackTarget(victim))
        return false;

    if (!(procSpell->GetSpellSchoolMask() & SPELL_SCHOOL_MASK_NATURE))
        return false;

    if (procSpell->IsPositiveSpell())
        return false;

    if (procEx & (PROC_EX_CAST_END | PROC_EX_PERIODIC_POSITIVE))
        return false;

    uint32 const directSpellFlags = PROC_FLAG_DEAL_HARMFUL_ABILITY | PROC_FLAG_DEAL_HARMFUL_SPELL;
    if (!(procFlag & directSpellFlags) || (procFlag & PROC_FLAG_DEAL_HARMFUL_PERIODIC))
        return false;

    return procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT);
}

bool IsOffensiveDirectSpell(Unit const* owner, Unit* victim, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, bool isVictim)
{
    if (!owner || !victim || !procSpell || isVictim)
        return false;

    if (!owner->IsValidAttackTarget(victim))
        return false;

    if (procSpell->SpellFamilyName != SPELLFAMILY_SHAMAN)
        return false;

    if (!procSpell->HasEffect(SPELL_EFFECT_SCHOOL_DAMAGE))
        return false;

    if (procSpell->IsPositiveSpell())
        return false;

    if (procEx & (PROC_EX_CAST_END | PROC_EX_PERIODIC_POSITIVE))
        return false;

    uint32 const directSpellFlags = PROC_FLAG_DEAL_HARMFUL_ABILITY | PROC_FLAG_DEAL_HARMFUL_SPELL;
    if (!(procFlag & directSpellFlags) || (procFlag & PROC_FLAG_DEAL_HARMFUL_PERIODIC))
        return false;

    return procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT);
}

int32 GetLightningStrikeShieldAmount(Unit* owner, Aura const* shieldAura)
{
    if (!owner || !shieldAura)
        return 0;

    uint32 triggerSpellId = GetLightningShieldDamageSpell(shieldAura->GetId());
    if (!triggerSpellId)
        triggerSpellId = shieldAura->GetSpellProto()->EffectTriggerSpell[EFFECT_INDEX_0];

    SpellEntry const* triggerSpell = sSpellMgr.GetSpellEntry(triggerSpellId);
    if (!triggerSpell)
        return 0;

    int32 amount = triggerSpell->CalculateSimpleValue(EFFECT_INDEX_0);
    int32 const attackPower = int32(owner->GetTotalAttackPowerValue(BASE_ATTACK));

    SpellEntry const* shieldSpell = shieldAura->GetSpellProto();
    if (IsLightningShieldSpell(shieldSpell))
        amount += attackPower / 4;
    else if (IsWaterShieldSpell(shieldSpell))
        amount += attackPower / 18;
    else if (IsEarthShieldSpell(shieldSpell))
        amount += attackPower / 15;

    return amount;
}

void TriggerElementalShell(Unit* owner)
{
    if (!owner || !owner->HasAura(SPELL_SHAMAN_ELEMENTAL_SHELL_PASSIVE))
        return;

    owner->CastSpell(owner, SPELL_SHAMAN_ELEMENTAL_SHELL, true);
}

void TriggerTidalWave(Unit* owner)
{
    if (!owner || !owner->HasAura(SPELL_SHAMAN_TIDAL_WAVE_PASSIVE))
        return;

    // triggered casts skip the cooldown check in Spell::CheckCast, so honour 41825's 2 s recoveryTime here
    if (owner->HasSpellCooldown(SPELL_SHAMAN_TIDAL_WAVE))
        return;

    owner->CastSpell(owner, SPELL_SHAMAN_TIDAL_WAVE, true);
}

bool TriggerEmpoweredElementalShield(Spell* spell, bool consumeCharge)
{
    if (!spell || !spell->m_casterUnit)
        return true;

    Aura* shieldAura = GetActiveElementalShield(spell->m_casterUnit);
    if (!shieldAura)
        return false;

    uint32 triggerSpellId = GetLightningShieldDamageSpell(shieldAura->GetId());
    if (!triggerSpellId)
        triggerSpellId = shieldAura->GetSpellProto()->EffectTriggerSpell[EFFECT_INDEX_0];
    if (!triggerSpellId)
        return false;

    int32 const amount = GetLightningStrikeShieldAmount(spell->m_casterUnit, shieldAura);
    if (amount <= 0)
        return false;

    SpellEntry const* shieldSpell = shieldAura->GetSpellProto();
    if (IsLightningShieldSpell(shieldSpell))
    {
        Unit* target = spell->GetUnitTarget();
        if (target && spell->m_casterUnit->IsValidAttackTarget(target))
            spell->m_casterUnit->CastCustomSpell(target, triggerSpellId, &amount, nullptr, nullptr, true);
    }
    else if (IsWaterShieldSpell(shieldSpell))
        spell->m_casterUnit->EnergizeBySpell(spell->m_casterUnit, triggerSpellId, amount, POWER_MANA);
    else if (IsEarthShieldSpell(shieldSpell))
        spell->m_casterUnit->CastCustomSpell(spell->m_casterUnit, triggerSpellId, &amount, nullptr, nullptr, true);

    TriggerElementalShell(spell->m_casterUnit);

    if (consumeCharge)
        if (SpellAuraHolder* holder = shieldAura->GetHolder())
            if (holder->DropAuraCharge())
                spell->m_casterUnit->RemoveSpellAuraHolder(holder);

    return false;
}

uint32 GetImprovedSearingTotemSpell(Unit const* owner)
{
    if (!owner)
        return 0;

    if (owner->HasAura(SPELL_SHAMAN_IMPROVED_FIRE_TOTEMS_R2))
        return SPELL_SHAMAN_IMPROVED_SEARING_TOTEM_R2;

    if (owner->HasAura(SPELL_SHAMAN_IMPROVED_FIRE_TOTEMS_R1))
        return SPELL_SHAMAN_IMPROVED_SEARING_TOTEM_R1;

    return 0;
}

void RemoveImprovedSearingTotemAuras(Unit* owner)
{
    if (!owner)
        return;

    owner->RemoveAurasDueToSpell(SPELL_SHAMAN_IMPROVED_SEARING_TOTEM_R1);
    owner->RemoveAurasDueToSpell(SPELL_SHAMAN_IMPROVED_SEARING_TOTEM_R2);
}

int32 GetEnhancingTotemsStoneskinPct(Unit const* owner)
{
    if (!owner)
        return 0;

    if (SpellEntry const* talent = owner->HasAura(SPELL_SHAMAN_ENHANCING_TOTEMS_R2) ? sSpellMgr.GetSpellEntry(SPELL_SHAMAN_ENHANCING_TOTEMS_R2) : nullptr)
        return talent->CalculateSimpleValue(EFFECT_INDEX_1);

    if (SpellEntry const* talent = owner->HasAura(SPELL_SHAMAN_ENHANCING_TOTEMS_R1) ? sSpellMgr.GetSpellEntry(SPELL_SHAMAN_ENHANCING_TOTEMS_R1) : nullptr)
        return talent->CalculateSimpleValue(EFFECT_INDEX_1);

    return 0;
}

bool IsImprovedWaterShieldSpell(uint32 spellId)
{
    switch (spellId)
    {
        case SPELL_SHAMAN_IMPROVED_WATER_SHIELD_R1:
        case SPELL_SHAMAN_IMPROVED_WATER_SHIELD_R2:
        case SPELL_SHAMAN_IMPROVED_WATER_SHIELD_R3:
            return true;
        default:
            return false;
    }
}

bool IsCalmingWindsSpellModTarget(SpellEntry const* spellInfo)
{
    return spellInfo && spellInfo->SpellFamilyName == SPELLFAMILY_SHAMAN && (spellInfo->SpellFamilyFlags & UI64LIT(50331648));
}

bool IsHealingWaveSpell(SpellEntry const* spellInfo)
{
    return spellInfo && spellInfo->IsFitToFamily<SPELLFAMILY_SHAMAN,
        CF_SHAMAN_HEALING_WAVE,
        CF_SHAMAN_LESSER_HEALING_WAVE>();
}

uint32 CalculateBasePowerCost(SpellEntry const* spellInfo, Unit* caster)
{
    if (!spellInfo || !caster)
        return 0;

    int32 powerCost = spellInfo->manaCost + spellInfo->manaCostPerlevel * (int32(caster->GetSpellRank(spellInfo)) / 5 - spellInfo->baseLevel);
    if (spellInfo->ManaCostPercentage)
    {
        switch (spellInfo->powerType)
        {
            case POWER_HEALTH:
                powerCost += spellInfo->ManaCostPercentage * caster->GetCreateHealth() / 100;
                break;
            case POWER_MANA:
                powerCost += spellInfo->ManaCostPercentage * caster->GetCreateMana() / 100;
                break;
            case POWER_RAGE:
            case POWER_FOCUS:
            case POWER_ENERGY:
            case POWER_HAPPINESS:
                powerCost += spellInfo->ManaCostPercentage * caster->GetMaxPower(Powers(spellInfo->powerType)) / 100;
                break;
            default:
                return 0;
        }
    }

    return std::max<int32>(powerCost, 0);
}

struct spell_shaman_thunderhead : public SpellScript
{
    void OnSetTargetMap(Spell* spell, SpellEffectIndex /*effIdx*/, uint32& targetMode, float& /*radius*/, uint32& /*unMaxTargets*/, bool& /*selectClosestTargets*/) const override
    {
        if (!spell->m_casterUnit || !spell->m_casterUnit->HasAura(45508) || spell->m_casterUnit->GetTargetGuid().IsEmpty())
            return;

        Unit* friendlyTarget = spell->m_casterUnit->GetMap()->GetUnit(spell->m_casterUnit->GetTargetGuid());
        if (!friendlyTarget || !friendlyTarget->IsFriendlyTo(spell->m_casterUnit) ||
                !friendlyTarget->IsCharmerOrOwnerPlayerOrPlayerItself() ||
                spell->m_spellInfo->MinTargetLevel > friendlyTarget->GetLevel() ||
                (!spell->m_casterUnit->IsPvP() && friendlyTarget->IsPvP()))
            return;

        targetMode = TARGET_UNIT_FRIEND;
        spell->m_targets.setUnitTarget(friendlyTarget);
    }
};

struct spell_shaman_lightning_shield : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* victim, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown) override
    {
        if (!victim)
            return SPELL_AURA_PROC_FAILED;

        uint32 triggerSpellId = GetLightningShieldDamageSpell(aura->GetId());
        if (!triggerSpellId)
            return SPELL_AURA_PROC_FAILED;

        if (cooldown && owner->HasSpellCooldown(triggerSpellId))
            return SPELL_AURA_PROC_FAILED;

        Unit* caster = owner;
        if (Unit* auraCaster = aura->GetCaster())
            if (owner->IsWithinDistInMap(auraCaster, VISIBILITY_DISTANCE_NORMAL))
                caster = auraCaster;

        caster->CastSpell(victim, triggerSpellId, true, nullptr, aura, caster->GetObjectGuid(), nullptr, procSpell);
        if (cooldown)
            owner->AddSpellCooldown(triggerSpellId, 0, time(nullptr) + cooldown);

        TriggerElementalShell(owner);
        return SPELL_AURA_PROC_OK;
    }
};

struct spell_shaman_water_shield : public AuraScript
{
    void OnAfterApply(Aura* aura, bool /*apply*/) override
    {
        if (aura && aura->GetEffIndex() == EFFECT_INDEX_1)
            aura->GetTarget()->UpdateManaRegen();
    }

    void OnAuraChargesChanged(SpellAuraHolder* holder) override
    {
        if (holder && holder->GetTarget())
            holder->GetTarget()->UpdateManaRegen();
    }

    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!aura || aura->GetEffIndex() != EFFECT_INDEX_0 || !IsWaterShieldSpell(aura->GetSpellProto()))
            return std::nullopt;

        TriggerTidalWave(owner);
        TriggerElementalShell(owner);
        return std::nullopt;
    }
};

struct spell_shaman_earth_shield : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!aura || aura->GetEffIndex() != EFFECT_INDEX_0 || !IsEarthShieldSpell(aura->GetSpellProto()))
            return std::nullopt;

        TriggerElementalShell(owner);
        return std::nullopt;
    }
};

struct spell_shaman_improved_fire_totems : public AuraScript
{
    void OnAfterApply(Aura* aura, bool apply) override
    {
        if (!aura || aura->GetEffIndex() != EFFECT_INDEX_0 || !IsImprovedFireTotemsSpell(aura->GetId()))
            return;

        Unit* target = aura->GetTarget();
        RemoveImprovedSearingTotemAuras(target);
        if (!apply)
            return;

        if (uint32 spellId = GetImprovedSearingTotemSpell(target))
            target->CastSpell(target, spellId, true, nullptr, aura);
    }
};

struct spell_shaman_stoneskin : public AuraScript
{
    int32 OnAuraValueCalculate(Aura* aura, Unit* caster, Unit* /*target*/, SpellEntry const* /*spellProto*/, SpellEffectIndex effIdx, Item* /*castItem*/, int32 value) override
    {
        if (!aura || (effIdx != EFFECT_INDEX_0 && effIdx != EFFECT_INDEX_1))
            return value;

        Unit* modOwner = caster ? caster->GetSpellModOwner() : nullptr;
        int32 const pct = GetEnhancingTotemsStoneskinPct(modOwner);
        if (pct <= 0)
            return value;

        return value * (100 + pct) / 100;
    }
};

struct spell_shaman_improved_water_shield : public AuraScript
{
    void OnManaRegenCalculate(Aura* aura, float& baseRegen, float& mp5Regen) override
    {
        if (!aura || aura->GetEffIndex() != EFFECT_INDEX_1 || !IsImprovedWaterShieldSpell(aura->GetId()))
            return;

        uint32 const charges = GetWaterShieldCharges(aura->GetTarget());
        if (!charges)
            return;

        if (SpellEntry const* talent = aura->GetSpellProto())
            mp5Regen += float(talent->CalculateSimpleValue(EFFECT_INDEX_0)) / 5.0f;

        int32 const pctTenths = std::max<int32>(aura->GetModifier()->m_amount, 0);
        if (pctTenths)
        {
            float const multiplier = (1000.0f + pctTenths * charges) / 1000.0f;
            baseRegen *= multiplier;
            mp5Regen *= multiplier;
        }
    }
};

struct spell_shaman_calming_winds : public AuraScript
{
    void OnThreatCalculate(Aura* aura, SpellEntry const* threatSpell, SpellSchoolMask schoolMask, float& threat) override
    {
        if (!aura || !IsCalmingWindsSpell(aura->GetId()) || !HasRockbiterEnchantment(aura->GetTarget()))
            return;

        if (aura->GetEffIndex() == EFFECT_INDEX_0)
        {
            if (!(schoolMask & SPELL_SCHOOL_MASK_NORMAL))
                return;
        }
        else if (aura->GetEffIndex() == EFFECT_INDEX_1)
        {
            if (!IsCalmingWindsSpellModTarget(threatSpell))
                return;
        }
        else
            return;

        int32 const pct = std::abs(aura->GetModifier()->m_amount);
        if (pct > 0 && pct < 100)
            threat = threat * 100.0f / float(100 - pct);
    }
};

struct spell_shaman_lightning_strike_shield : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit)
            return true;

        return TriggerEmpoweredElementalShield(spell, true);
    }
};

struct spell_shaman_stormhowl_trigger_elemental_shield : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell->m_casterUnit)
            return true;

        return TriggerEmpoweredElementalShield(spell, false);
    }
};

struct spell_shaman_lightning_strike_nature_damage : public SpellScript
{
    void OnEffectDamageCalculate(Spell* spell, SpellEffectIndex effIdx, float& damage) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell || !spell->m_casterUnit || !IsLightningStrikeNatureDamageSpell(spell->m_spellInfo->Id))
            return;

        Aura* stormstrike = GetStormstrikeAura(spell->m_casterUnit);
        if (!stormstrike)
            return;

        damage *= float(stormstrike->GetModifier()->m_amount + 100) / 100.0f;
    }

    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        if (missInfo != SPELL_MISS_NONE || !spell || !spell->m_casterUnit || !IsLightningStrikeNatureDamageSpell(spell->m_spellInfo->Id))
            return;

        if (SpellAuraHolder* holder = spell->m_casterUnit->GetSpellAuraHolder(SPELL_SHAMAN_STORMSTRIKE_AURA))
            if (holder->DropAuraCharge())
                spell->m_casterUnit->RemoveSpellAuraHolder(holder);
    }
};

struct spell_shaman_stormstrike : public AuraScript
{
    std::optional<SpellProcEventTriggerCheck> OnCheckProc(Unit const* /*owner*/, Unit* /*victim*/, SpellAuraHolder* holder, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procExtra*/, WeaponAttackType /*attType*/, bool /*isVictim*/) override
    {
        if (!holder || holder->GetId() != SPELL_SHAMAN_STORMSTRIKE_AURA)
            return std::nullopt;

        if (procSpell && IsLightningStrikeNatureDamageSpell(procSpell->Id))
            return SPELL_PROC_TRIGGER_FAILED;

        return std::nullopt;
    }
};

struct spell_shaman_thundercall : public AuraScript
{
    void OnPeriodicDamageCalculateAmount(Aura* aura, float& amount) override
    {
        if (!aura || aura->GetId() != SPELL_SHAMAN_THUNDERCALL || !aura->IsPersistent())
            return;

        DynamicObject* dynObj = static_cast<PersistentAreaAura*>(aura)->GetDynObject();
        uint32 const affectedCount = dynObj ? dynObj->GetAffectedCount() : m_lastAffectedCount;
        if (dynObj && affectedCount)
            m_lastAffectedCount = affectedCount;

        if (affectedCount > 1)
            amount /= affectedCount;
    }

private:
    uint32 m_lastAffectedCount = 0;
};

struct spell_shaman_healing_way : public AuraScript
{
    void OnSpellHealingBonusTaken(Aura* aura, WorldObject* /*caster*/, SpellEntry const* spellInfo, SpellEffectIndex /*effIdx*/, int32 /*healAmount*/, DamageEffectType /*damageType*/, uint32 /*stack*/, Spell* /*spell*/, float& takenTotalMod) override
    {
        if (!aura || aura->GetId() != SPELL_SHAMAN_HEALING_WAY || !IsHealingWayAffectedSpell(spellInfo))
            return;

        takenTotalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
    }
};

struct spell_shaman_totemic_slam : public SpellScript
{
    void OnEffectDamageCalculate(Spell* spell, SpellEffectIndex effIdx, float& damage) const override
    {
        if (effIdx != EFFECT_INDEX_0 || !spell || !spell->m_casterUnit || spell->m_spellInfo->Id != SPELL_SHAMAN_TOTEMIC_SLAM)
            return;

        float attackPowerMultiplier = 0.10f;
        if (spell->m_casterUnit->HasAura(SPELL_SHAMAN_ANCIENT_RITES_TOTEMIC_SLAM))
            attackPowerMultiplier += 0.05f;

        damage = spell->m_casterUnit->GetTotalAttackPowerValue(BASE_ATTACK) * attackPowerMultiplier;
    }

    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        if (missInfo != SPELL_MISS_NONE || !spell || !spell->m_casterUnit || spell->m_spellInfo->Id != SPELL_SHAMAN_TOTEMIC_SLAM)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsAlive() || !spell->m_casterUnit->HasAura(SPELL_SHAMAN_ANCIENT_RITES_TOTEMIC_SLAM))
            return;

        spell->m_casterUnit->CastSpell(target, SPELL_SHAMAN_ANCIENT_RITES_KNOCKDOWN, true);
    }
};

struct spell_shaman_hex : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        if (missInfo != SPELL_MISS_NONE || !spell || !spell->m_casterUnit || spell->m_spellInfo->Id != SPELL_SHAMAN_HEX)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsAlive() || !spell->m_casterUnit->HasAura(SPELL_SHAMAN_ANCIENT_RITES_HEX))
            return;

        spell->m_casterUnit->CastSpell(target, SPELL_SHAMAN_ANCIENT_RITES_FROGS, true);
    }
};

struct spell_shaman_call_of_earth : public AuraScript
{
    std::optional<SpellProcEventTriggerCheck> OnCheckProc(Unit const* owner, Unit* victim, SpellAuraHolder* holder, SpellEntry const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType /*attType*/, bool isVictim) override
    {
        if (!holder || !IsCallOfEarthSpell(holder->GetId()))
            return std::nullopt;
        // Ambiguity between spell description and patch notes on whether this procs on all damaging spells or just nature ones
        // Added a secondary helper for this, IsOffensiveDirectSpell, that can be swapped in
        if (!IsOffensiveNatureDirectSpell(owner, victim, procSpell, procFlag, procExtra, isVictim))
            return SPELL_PROC_TRIGGER_FAILED;

        return HasEarthShield(owner)
            ? SPELL_PROC_TRIGGER_OK
            : SPELL_PROC_TRIGGER_FAILED;
    }

    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!aura || !IsCallOfEarthSpell(aura->GetId()))
            return std::nullopt;

        if (aura->GetEffIndex() != EFFECT_INDEX_2)
            return SPELL_AURA_PROC_CANT_TRIGGER;

        SpellAuraHolder* earthShield = GetActiveEarthShieldHolder(owner);
        if (!earthShield)
            return SPELL_AURA_PROC_FAILED;

        uint32 const maxCharges = GetEarthShieldMaxCharges(owner, earthShield);
        if (!maxCharges)
            return SPELL_AURA_PROC_FAILED;

        uint32 const replenish = std::max<int32>(aura->GetModifier()->m_amount, 0);
        if (!replenish)
            return SPELL_AURA_PROC_FAILED;

        uint32 const charges = std::min<uint32>(earthShield->GetAuraCharges() + replenish, maxCharges);
        if (charges != earthShield->GetAuraCharges())
            earthShield->SetAuraCharges(charges);

        earthShield->RefreshHolder();
        return SPELL_AURA_PROC_OK;
    }
};

struct spell_shaman_undertow : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*amount*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!aura || !IsUndertowSpell(aura->GetId()))
            return std::nullopt;

        SpellAuraHolder* waterShield = GetActiveWaterShieldHolder(owner);
        if (!waterShield)
            return SPELL_AURA_PROC_FAILED;

        uint32 const maxCharges = GetWaterShieldMaxCharges(owner, waterShield);
        if (!maxCharges)
            return SPELL_AURA_PROC_FAILED;

        if (waterShield->GetAuraCharges() >= maxCharges)
        {
            if (waterShield->DropAuraCharge())
                owner->RemoveSpellAuraHolder(waterShield);
            return SPELL_AURA_PROC_OK;
        }

        waterShield->SetAuraCharges(waterShield->GetAuraCharges() + 1);
        waterShield->RefreshHolder();
        return SPELL_AURA_PROC_OK;
    }
};

struct spell_shaman_tidal_surge : public AuraScript
{
    std::optional<SpellProcEventTriggerCheck> OnCheckProc(Unit const* owner, Unit* victim, SpellAuraHolder* holder, SpellEntry const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType /*attType*/, bool isVictim) override
    {
        if (!holder || !IsTidalSurgeSpell(holder->GetId()))
            return std::nullopt;

        if (!owner || !victim || isVictim || !procSpell || !victim->GetSpellAuraHolder(SPELL_SHAMAN_HEALING_WAY))
            return SPELL_PROC_TRIGGER_FAILED;

        if (!(procFlag & PROC_FLAG_DEAL_HELPFUL_SPELL) || !(procExtra & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)))
            return SPELL_PROC_TRIGGER_FAILED;

        return std::nullopt;
    }

    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*amount*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!aura || !IsTidalSurgeSpell(aura->GetId()) || !procSpell)
            return std::nullopt;

        int32 const pct = std::max<int32>(aura->GetModifier()->m_amount, 0);
        uint32 const mana = CalculateBasePowerCost(procSpell, owner) * pct / 100;
        if (mana)
            owner->EnergizeBySpell(owner, aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()], mana, POWER_MANA);

        return SPELL_AURA_PROC_OK;
    }
};

struct spell_shaman_elemental_focus : public AuraScript
{
    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 /*damage*/, int32 /*originalAmount*/, Aura* aura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/) override
    {
        if (!owner || !aura || aura->GetEffIndex() != EFFECT_INDEX_0)
            return std::nullopt;

        uint32 triggerSpellId = aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()];
        if (owner->HasAura(triggerSpellId))
            owner->RemoveAurasDueToSpellByCancel(triggerSpellId);

        if (owner->HasAura(SPELL_SHAMAN_IMPROVED_CLEARCASTING))
            owner->RemoveAurasDueToSpellByCancel(SPELL_SHAMAN_IMPROVED_CLEARCASTING);

        if (owner->HasAura(SPELL_SHAMAN_IMPROVED_CLEARCASTING_SET))
        {
            owner->CastSpell(owner, SPELL_SHAMAN_IMPROVED_CLEARCASTING, true, nullptr, aura);
            return SPELL_AURA_PROC_OK;
        }

        return std::nullopt;
    }
};

struct spell_shaman_elemental_weapons : public AuraScript
{
    void OnAfterApply(Aura* aura, bool apply) override
    {
        if (!aura || aura->GetEffIndex() != EFFECT_INDEX_0)
            return;

        uint32 const frostbrandSpellId = GetElementalWeaponsFrostbrandSpell(aura->GetId());
        if (!frostbrandSpellId)
            return;

        Unit* target = aura->GetTarget();
        if (!target)
            return;

        if (!apply)
        {
            target->RemoveAurasDueToSpell(frostbrandSpellId);
            return;
        }

        RemoveElementalWeaponsFrostbrandAuras(target);
        target->CastSpell(target, frostbrandSpellId, true, nullptr, aura);
    }

    std::optional<SpellProcEventTriggerCheck> OnCheckProc(Unit const* owner, Unit* /*victim*/, SpellAuraHolder* holder, SpellEntry const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType /*attType*/, bool isVictim) override
    {
        if (!holder || !IsElementalWeaponsSpell(holder->GetId()))
            return std::nullopt;

        if (isVictim)
            return SPELL_PROC_TRIGGER_FAILED;

        if (procSpell)
            return std::nullopt;

        if (!IsDamageDealtProc(procFlag, procExtra))
            return SPELL_PROC_TRIGGER_FAILED;

        if (HasEarthenBulwark(owner))
            return HasRockbiterEnchantment(owner) ? SPELL_PROC_TRIGGER_OK : SPELL_PROC_TRIGGER_FAILED;

        return (procFlag & PROC_FLAG_DEAL_MELEE_SWING) && HasRockbiterEnchantmentForSwing(owner, procFlag)
            ? SPELL_PROC_TRIGGER_OK
            : SPELL_PROC_TRIGGER_FAILED;
    }

    std::optional<SpellAuraProcResult> OnProc(Unit* owner, Unit* /*victim*/, uint32 damage, int32 /*originalAmount*/, Aura* aura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 /*cooldown*/) override
    {
        if (!aura || !IsElementalWeaponsSpell(aura->GetId()))
            return std::nullopt;

        switch (aura->GetEffIndex())
        {
            case EFFECT_INDEX_0:
            case EFFECT_INDEX_1:
                return SPELL_AURA_PROC_CANT_TRIGGER;
            case EFFECT_INDEX_2:
                break;
            default:
                return std::nullopt;
        }

        if (!damage || !IsDamageDealtProc(procFlag, procEx))
            return SPELL_AURA_PROC_FAILED;

        bool const hasEarthenBulwark = HasEarthenBulwark(owner);
        if (hasEarthenBulwark && !HasRockbiterEnchantment(owner))
            return SPELL_AURA_PROC_FAILED;

        if (!hasEarthenBulwark &&
            (procSpell || !(procFlag & PROC_FLAG_DEAL_MELEE_SWING) || !HasRockbiterEnchantmentForSwing(owner, procFlag)))
            return SPELL_AURA_PROC_FAILED;

        CastElementalWeaponsTrigger(owner, aura, GetEarthenBulwarkBuildAmount(owner, aura, damage));
        return SPELL_AURA_PROC_OK;
    }
};

struct spell_shaman_earthen_bulwark : public AuraScript
{
    void OnAbsorb(Aura* aura, int32& currentAbsorb, int32& remainingDamage, bool& dropCharge, DamageEffectType /*damageType*/) override
    {
        int32 const absorbPct = GetEarthenBulwarkAbsorbPct(aura);
        if (absorbPct <= 0)
            return;

        dropCharge = false;
        currentAbsorb = std::min(currentAbsorb, remainingDamage * absorbPct / 100);
    }
};

struct spell_shaman_frostbrand_attack : public SpellScript
{
    void OnSpellCritChanceCalculate(Spell* spell, Unit const* victim, float& critChance) const override
    {
        Unit* caster = spell->m_casterUnit;
        if (!caster || !HasElementalWeaponsFrostbrand(caster) || !TargetHasCasterFrostShock(victim, caster))
            return;

        critChance = 100.0f;
    }
};

struct spell_shaman_lightning_speed : public AuraScript
{
    std::optional<SpellProcEventTriggerCheck> OnCheckProc(Unit const* /*owner*/, Unit* /*victim*/, SpellAuraHolder* /*holder*/, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procExtra*/, WeaponAttackType /*attType*/, bool /*isVictim*/) override
    {
        if (!procSpell)
            return SPELL_PROC_TRIGGER_FAILED;

        if (procSpell->IsFitToFamily<SPELLFAMILY_SHAMAN, CF_SHAMAN_LIGHTNING_BOLT>())
            return roll_chance_u(10) ? SPELL_PROC_TRIGGER_OK : SPELL_PROC_TRIGGER_ROLL_FAILED;

        if (procSpell->SpellIconID == 2210)
            return roll_chance_u(50) ? SPELL_PROC_TRIGGER_OK : SPELL_PROC_TRIGGER_ROLL_FAILED;

        return SPELL_PROC_TRIGGER_FAILED;
    }
};

struct spell_shaman_rockbiter_proc : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return true;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->CanHaveThreatList() || !spell->m_casterUnit)
            return false;

        if (target->GetThreatManager().getThreat(spell->m_casterUnit))
            target->GetThreatManager().addThreat(spell->m_casterUnit, spell->damage * spell->m_casterUnit->GetAttackTime(BASE_ATTACK) / 1000);

        return false;
    }
};

struct spell_shaman_flametongue_proc : public SpellScript
{
    bool OnEffectExecute(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (effIdx != EFFECT_INDEX_0)
            return true;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return false;

        Item* weapon = spell->m_CastItem;
        if (!weapon)
        {
            Player* player = spell->m_casterUnit ? spell->m_casterUnit->ToPlayer() : nullptr;
            weapon = player ? player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND) : nullptr;
            if (!weapon)
            {
                sLog.outError("spell_shaman_flametongue_proc: spell %u requires cast item or main-hand weapon", spell->m_spellInfo->Id);
                return false;
            }
        }

        int32 spellDamage = spell->m_caster->SpellBaseDamageBonusDone(spell->m_spellInfo->GetSpellSchoolMask());
        float weaponSpeed = (1.0f / IN_MILLISECONDS) * weapon->GetProto()->Delay;
        int32 totalDamage = int32((spell->damage + 3.85f * spellDamage) * 0.01f * weaponSpeed);
        if (spell->m_spellInfo->IsFitToFamily<SPELLFAMILY_SHAMAN, CF_SHAMAN_FLAMETONGUE_WEAPON>() &&
            spell->m_casterUnit && GetCasterFlameShock(spell->m_casterUnit, target))
        {
            if (Aura const* setBonus = spell->m_casterUnit->GetAura(SPELL_SHAMAN_FLAMETONGUE_DAMAGE_BONUS, EFFECT_INDEX_0))
                totalDamage += totalDamage * setBonus->GetBasePoints() / 100;
        }

        spell->m_caster->CastCustomSpell(target, 10444, &totalDamage, nullptr, nullptr, true, weapon);
        return false;
    }
};

struct spell_shaman_molten_blast : public SpellScript
{
    void OnHit(Spell* spell, SpellMissInfo missInfo) const override
    {
        if (missInfo != SPELL_MISS_NONE || !spell->m_casterUnit)
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target || !target->IsAlive())
            return;

        FocusSearingTotem(spell->m_casterUnit, target);

        SpellAuraHolder* flameShock = GetCasterFlameShock(spell->m_casterUnit, target);
        if (!flameShock)
            return;

        int32 const damage = CalculateImprovedMoltenBlastDamage(spell->m_casterUnit, flameShock);
        flameShock->RefreshHolder();

        if (damage > 0)
            spell->m_casterUnit->CastCustomSpell(target, SPELL_SHAMAN_REKINDLED_FLAME, &damage, nullptr, nullptr, true);
    }
};

void CastEarthquakeAreaDamage(Unit* caster, Unit* center, uint32 spellId, int32 damage)
{
    if (!caster || !center || damage <= 0)
        return;

    caster->CastCustomSpell(center, spellId, &damage, nullptr, nullptr, true);
}

class EarthquakeAftershockEvent : public BasicEvent
{
public:
    EarthquakeAftershockEvent(Unit& caster, ObjectGuid targetGuid, int32 damage)
        : m_caster(caster), m_targetGuid(targetGuid), m_damage(damage) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        Unit* center = ObjectAccessor::GetUnit(m_caster, m_targetGuid);
        if (center)
            CastEarthquakeAreaDamage(&m_caster, center, SPELL_SHAMAN_EARTHQUAKE_AFTERSHOCK, m_damage);

        return true;
    }

private:
    Unit& m_caster;
    ObjectGuid m_targetGuid;
    int32 m_damage;
};

struct spell_shaman_earthquake : public SpellScript
{
    void OnTargetMapFilled(Spell* spell, SpellEffectIndex effIdx, uint32 /*targetMode*/, std::list<Unit*>& targets) const override
    {
        if (!spell || spell->m_spellInfo->Id != SPELL_SHAMAN_EARTHQUAKE_SPLASH || effIdx != EFFECT_INDEX_0)
            return;

        Unit* center = spell->GetUnitTarget();
        if (!center)
            return;

        ObjectGuid const centerGuid = center->GetObjectGuid();
        targets.remove_if([centerGuid](Unit const* target)
        {
            return target && target->GetObjectGuid() == centerGuid;
        });
    }

    void OnEffectExecuted(Spell* spell, SpellEffectIndex effIdx) const override
    {
        if (m_handled || effIdx != EFFECT_INDEX_0 || !spell || !spell->m_casterUnit || !IsEarthquakeSpell(spell->m_spellInfo->Id))
            return;

        Unit* target = spell->GetUnitTarget();
        if (!target)
            return;

        SpellEntry const* splash = sSpellMgr.GetSpellEntry(SPELL_SHAMAN_EARTHQUAKE_SPLASH);
        SpellEntry const* aftershock = sSpellMgr.GetSpellEntry(SPELL_SHAMAN_EARTHQUAKE_AFTERSHOCK);
        if (!splash || !aftershock)
            return;

        int32 const initialDamage = int32(spell->GetTotalEffectDamage());
        if (initialDamage <= 0)
            return;

        m_handled = true;

        int32 const splashDamage = initialDamage * splash->CalculateSimpleValue(EFFECT_INDEX_0) / 100;
        CastEarthquakeAreaDamage(spell->m_casterUnit, target, SPELL_SHAMAN_EARTHQUAKE_SPLASH, splashDamage);

        ObjectGuid const targetGuid = target->GetObjectGuid();
        int32 const aftershockDamage = initialDamage * aftershock->CalculateSimpleValue(EFFECT_INDEX_0) / 100;
        spell->m_casterUnit->m_Events.AddEvent(new EarthquakeAftershockEvent(*spell->m_casterUnit, targetGuid, aftershockDamage),
            spell->m_casterUnit->m_Events.CalculateTime(4000));
    }

private:
    mutable bool m_handled = false;
};

// Deprecated in patch 1.17.2
struct spell_shaman_mana_tide : public AuraScript
{
    void OnPeriodicTrigger(Aura* aura, Unit* /*caster*/, Unit* target, WorldObject* /*targetObject*/, SpellEntry const*& spellInfo) override
    {
        uint32 triggerSpellId = aura->GetSpellProto()->EffectTriggerSpell[aura->GetEffIndex()];
        if (!triggerSpellId)
            return;

        int32 amount = aura->GetModifier()->m_amount;
        target->CastCustomSpell(target, triggerSpellId, &amount, nullptr, nullptr, true, nullptr, aura);
        spellInfo = nullptr;
    }
};

class AncientRitesLungeDamageEvent : public BasicEvent
{
public:
    AncientRitesLungeDamageEvent(Creature& wolf, ObjectGuid targetGuid, bool startMovement)
        : m_wolf(wolf), m_targetGuid(targetGuid), m_startMovement(startMovement) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        if (!m_wolf.IsAlive())
            return true;

        Unit* target = m_wolf.GetMap()->GetUnit(m_targetGuid);
        if (!target || !target->IsAlive() || !m_wolf.IsValidAttackTarget(target))
            return true;

        if (m_startMovement)
        {
            float const distance = m_wolf.GetDistance(target);
            uint32 const lungeDelay = uint32(distance * 1000.0f / 24.0f) + 100;

            m_wolf.SetTargetGuid(target->GetObjectGuid());
            m_wolf.Attack(target, true);
            m_wolf.CastSpell(&m_wolf, SPELL_SHAMAN_ANCIENT_RITES_LUNGE, true);
            m_wolf.GetMotionMaster()->MoveCharge(target, 0, true);
            m_wolf.m_Events.AddEvent(new AncientRitesLungeDamageEvent(m_wolf, m_targetGuid, false),
                m_wolf.m_Events.CalculateTime(lungeDelay));
            return true;
        }

        m_wolf.CastSpell(target, SPELL_SHAMAN_ANCIENT_RITES_LUNGE_DAMAGE, true);
        m_wolf.Attack(target, true);
        return true;
    }

private:
    Creature& m_wolf;
    ObjectGuid m_targetGuid;
    bool m_startMovement;
};

struct spell_shaman_feral_spirit : public SpellScript
{
    void OnSummonBeforeAdd(Spell* spell, Pet* summon, uint32 summonIndex) const override
    {
        if (!spell->m_casterUnit)
            return;

        summon->InitStatsForLevel(spell->m_casterUnit->GetLevel());
        summon->SetFollowAngle(PET_FOLLOW_ANGLE + summonIndex * M_PI_F);
    }

    void OnSummon(Spell* spell, Creature* summon) const override
    {
        if (!spell->m_casterUnit || !summon)
            return;

        if (!spell->m_casterUnit->HasAura(SPELL_SHAMAN_ANCIENT_RITES_FERAL_SPIRIT))
            return;

        ObjectGuid targetGuid = spell->m_casterUnit->GetTargetGuid();
        if (Player* player = spell->m_casterUnit->ToPlayer())
            targetGuid = player->GetSelectionGuid();

        Unit* target = targetGuid ? spell->m_casterUnit->GetMap()->GetUnit(targetGuid) : nullptr;
        if (!target || !spell->m_casterUnit->IsValidAttackTarget(target) || !spell->m_casterUnit->IsWithinDistInMap(target, 30.0f))
            return;

        summon->m_Events.AddEvent(new AncientRitesLungeDamageEvent(*summon, target->GetObjectGuid(), true),
            summon->m_Events.CalculateTime(1));
    }
};

struct spell_shaman_ghost_wolf_speed : public AuraScript
{
    void OnAfterApply(Aura* aura, bool /*apply*/) override
    {
        Unit* target = aura->GetTarget();
        SpellAuraHolder* ghostWolf = target->GetSpellAuraHolder(2645);
        if (!ghostWolf)
            return;

        Aura* speedAura = ghostWolf->GetAuraByEffectIndex(EFFECT_INDEX_1);
        if (!speedAura)
            return;

        speedAura->GetModifier()->m_amount = target->CalculateSpellDamage(target, ghostWolf->GetSpellProto(), EFFECT_INDEX_1);
        target->UpdateSpeed(MOVE_RUN, false, target->GetSpeedRatePersistance(MOVE_RUN));
    }
};

struct spell_shaman_calming_river : public AuraScript
{
    void OnAuraInit(Aura* aura) override
    {
        aura->SetPeriodicTimer(5000);
    }

    void OnPeriodicDummy(Aura* aura) override
    {
        Unit* target = aura->GetTarget();
        if (target->HasAura(45527) && target->GetPower(POWER_MANA) != target->GetMaxPower(POWER_MANA))
            target->CastSpell(target, 47358, true, nullptr, aura);
    }
};

struct spell_shaman_totemic_recall : public SpellScript
{
    void OnDestroyTotem(Spell* spell, Totem* totem) const override
    {
        if (!spell->m_casterUnit || !totem)
            return;

        uint32 const spellId = totem->GetUInt32Value(UNIT_CREATED_BY_SPELL);
        SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellId);
        if (!spellInfo)
            return;

        int32 refundPct = spell->m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
        if (Player* modOwner = spell->m_casterUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(spell->m_spellInfo->Id, SPELLMOD_ALL_EFFECTS, refundPct, spell);

        uint32 const manaCost = Spell::CalculatePowerCost(spellInfo, spell->m_casterUnit);
        uint32 const refund = manaCost * refundPct / 100;
        if (refund)
            spell->m_casterUnit->EnergizeBySpell(spell->m_casterUnit, spell->m_spellInfo->Id, refund, POWER_MANA);
    }
};

struct spell_spirit_armor : public AuraScript
{
    void OnAfterApply(Aura* aura, bool /*apply*/) override
    {
        aura->GetTarget()->UpdateArmor();
    }
};
}

void AddSC_shaman_spell_scripts()
{
    RegisterSpellScript("spell_shaman_thunderhead", &GetSpellScript<spell_shaman_thunderhead>);
    RegisterSpellScript("spell_shaman_rockbiter_proc", &GetSpellScript<spell_shaman_rockbiter_proc>);
    RegisterSpellScript("spell_shaman_flametongue_proc", &GetSpellScript<spell_shaman_flametongue_proc>);
    RegisterSpellScript("spell_shaman_molten_blast", &GetSpellScript<spell_shaman_molten_blast>);
    RegisterSpellScript("spell_shaman_earthquake", &GetSpellScript<spell_shaman_earthquake>);
    RegisterSpellScript("spell_shaman_lightning_strike_shield", &GetSpellScript<spell_shaman_lightning_strike_shield>);
    RegisterSpellScript("spell_shaman_stormhowl_trigger_elemental_shield", &GetSpellScript<spell_shaman_stormhowl_trigger_elemental_shield>);
    RegisterSpellScript("spell_shaman_lightning_strike_nature_damage", &GetSpellScript<spell_shaman_lightning_strike_nature_damage>);
    RegisterSpellScript("spell_shaman_totemic_slam", &GetSpellScript<spell_shaman_totemic_slam>);
    RegisterSpellScript("spell_shaman_hex", &GetSpellScript<spell_shaman_hex>);
    RegisterAuraScript("spell_shaman_water_shield", &GetAuraScript<spell_shaman_water_shield>);
    RegisterAuraScript("spell_shaman_earth_shield", &GetAuraScript<spell_shaman_earth_shield>);
    RegisterAuraScript("spell_shaman_improved_fire_totems", &GetAuraScript<spell_shaman_improved_fire_totems>);
    RegisterAuraScript("spell_shaman_stoneskin", &GetAuraScript<spell_shaman_stoneskin>);
    RegisterAuraScript("spell_shaman_improved_water_shield", &GetAuraScript<spell_shaman_improved_water_shield>);
    RegisterAuraScript("spell_shaman_calming_winds", &GetAuraScript<spell_shaman_calming_winds>);
    RegisterAuraScript("spell_shaman_stormstrike", &GetAuraScript<spell_shaman_stormstrike>);
    RegisterAuraScript("spell_shaman_thundercall", &GetAuraScript<spell_shaman_thundercall>);
    RegisterAuraScript("spell_shaman_healing_way", &GetAuraScript<spell_shaman_healing_way>);
    RegisterAuraScript("spell_shaman_lightning_shield", &GetAuraScript<spell_shaman_lightning_shield>);
    RegisterAuraScript("spell_shaman_call_of_earth", &GetAuraScript<spell_shaman_call_of_earth>);
    RegisterAuraScript("spell_shaman_undertow", &GetAuraScript<spell_shaman_undertow>);
    RegisterAuraScript("spell_shaman_tidal_surge", &GetAuraScript<spell_shaman_tidal_surge>);
    RegisterAuraScript("spell_shaman_elemental_focus", &GetAuraScript<spell_shaman_elemental_focus>);
    RegisterAuraScript("spell_shaman_elemental_weapons", &GetAuraScript<spell_shaman_elemental_weapons>);
    RegisterAuraScript("spell_shaman_earthen_bulwark", &GetAuraScript<spell_shaman_earthen_bulwark>);
    RegisterSpellScript("spell_shaman_frostbrand_attack", &GetSpellScript<spell_shaman_frostbrand_attack>);
    RegisterAuraScript("spell_shaman_lightning_speed", &GetAuraScript<spell_shaman_lightning_speed>);
    RegisterAuraScript("spell_shaman_mana_tide", &GetAuraScript<spell_shaman_mana_tide>);
    RegisterSpellScript("spell_shaman_feral_spirit", &GetSpellScript<spell_shaman_feral_spirit>);
    RegisterAuraScript("spell_shaman_ghost_wolf_speed", &GetAuraScript<spell_shaman_ghost_wolf_speed>);
    RegisterAuraScript("spell_shaman_calming_river", &GetAuraScript<spell_shaman_calming_river>);
    RegisterSpellScript("spell_shaman_totemic_recall", &GetSpellScript<spell_shaman_totemic_recall>);
    RegisterAuraScript("spell_spirit_armor", &GetAuraScript<spell_spirit_armor>);
}
