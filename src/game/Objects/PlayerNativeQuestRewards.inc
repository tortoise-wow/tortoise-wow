// Only immediate effects confined to this player join the durable reward core.
// Validated single-effect far teleports join via the scope-deferred map transfer.
// Other world/event/pet/trigger spells retain ordinary post-ACK dispatch.
namespace
{
Unit* NativeQuestRewardCaster(Player* player, WorldObject* ender, SpellEntry const* spell)
{
    if (ender->GetTypeId() == TYPEID_UNIT)
        for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
            if (spell->Effect[i] == SPELL_EFFECT_LEARN_SPELL || spell->Effect[i] == SPELL_EFFECT_CREATE_ITEM ||
                spell->EffectImplicitTargetA[i] == TARGET_UNIT || spell->EffectImplicitTargetA[i] == TARGET_UNIT_FRIEND)
                return static_cast<Unit*>(ender);
    return player;
}

bool IsLocalNativeQuestReward(SpellEntry const* spell, Unit* caster, Player* player)
{
    if (spell->speed != 0.0f || spell->IsChanneledSpell() || spell->IsCCSpell())
        return false;
    if (spell->Targets & (TARGET_FLAG_DEST_LOCATION | TARGET_FLAG_SOURCE_LOCATION))
        return false;
    uint8 effects = 0;
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (spell->Effect[i] != SPELL_EFFECT_NONE)
            ++effects;
    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        switch (spell->Effect[i])
        {
            case SPELL_EFFECT_NONE:
                continue;
            case SPELL_EFFECT_LEARN_SPELL:
            case SPELL_EFFECT_SKILL_STEP:
            case SPELL_EFFECT_CREATE_ITEM:
            case SPELL_EFFECT_APPLY_AURA:
            case SPELL_EFFECT_ADD_HONOR:
            case SPELL_EFFECT_TRADE_SKILL:
                break;
            case SPELL_EFFECT_TELEPORT_UNITS:
            {
                SpellTargetPosition const* position = sSpellMgr.GetSpellTargetPosition(spell->Id);
                if (effects != 1 || spell->EffectImplicitTargetB[i] != TARGET_LOCATION_DATABASE ||
                    spell->GetCastTime(caster, nullptr) != 0 || !position || position->mapId == player->GetMapId())
                    return false;
                break;
            }
            default:
                return false;
        }
        // Area, pet, scripted and caster-only NPC targets are not player-local.
        for (uint32 target : {spell->EffectImplicitTargetA[i], spell->EffectImplicitTargetB[i]})
            if (target != TARGET_NONE && target != TARGET_UNIT && target != TARGET_UNIT_FRIEND &&
                !(spell->Effect[i] == SPELL_EFFECT_TELEPORT_UNITS && target == TARGET_LOCATION_DATABASE) &&
                !(target == TARGET_UNIT_CASTER && caster == player))
                return false;
    }
    return true;
}

bool CastLocalNativeQuestReward(Player* player, Unit* caster, SpellEntry const* entry)
{
    Spell* spell = new Spell(caster, entry, true);
    SpellCastTargets targets;
    targets.setUnitTarget(player);
    // prepare registers its SpellEvent, which owns the spell even on failure.
    if (spell->prepare(std::move(targets)) != SPELL_CAST_OK)
        return false;
    // Teaching spells may have a data cast time despite being triggered. Finish
    // their already-checked, nonchanneled, zero-speed cast before sealing SQL.
    if (spell->getState() == SPELL_STATE_PREPARING)
        spell->cast(true);
    return spell->getState() == SPELL_STATE_FINISHED && spell->isSuccessCast();
}

void DispatchWorldQuestRewardAuras(Player* player, uint32 questId)
{
    uint32 zone = 0, area = 0;
    SpellAreaForQuestMapBounds bounds = sSpellMgr.GetSpellAreaForQuestMapBounds(questId, false);
    if (bounds.first == bounds.second)
        return;
    player->GetZoneAndAreaId(zone, area);
    for (auto itr = bounds.first; itr != bounds.second; ++itr)
    {
        SpellEntry const* entry = sSpellMgr.GetSpellEntry(itr->second->spellId);
        if (itr->second->autocast && itr->second->IsFitToRequirements(player, zone, area) &&
            !player->HasAura(itr->second->spellId, EFFECT_INDEX_0) &&
            (!entry || !IsLocalNativeQuestReward(entry, player, player)))
            player->CastSpell(player, itr->second->spellId, true);
    }
}
}

bool Player::_ApplyNativeQuestRewards(Quest const* pQuest, WorldObject* questEnder, bool& worldSpell)
{
    worldSpell = false;
    uint32 quest_id = pQuest->GetQuestId();
    uint32 spellId = pQuest->GetRewSpellCast() ? pQuest->GetRewSpellCast() : pQuest->GetRewSpell();
    if (spellId)
        if (SpellEntry const* entry = sSpellMgr.GetSpellEntry(spellId))
        {
            Unit* caster = NativeQuestRewardCaster(this, questEnder, entry);
            if (IsLocalNativeQuestReward(entry, caster, this))
            {
                if (!CastLocalNativeQuestReward(this, caster, entry))
                    return false;
            }
            else
                worldSpell = true;
        }

    uint32 zone = 0;
    uint32 area = 0;
    SpellAreaForQuestMapBounds endBounds = sSpellMgr.GetSpellAreaForQuestEndMapBounds(quest_id);
    if (endBounds.first != endBounds.second)
    {
        GetZoneAndAreaId(zone, area);
        for (auto itr = endBounds.first; itr != endBounds.second; ++itr)
            if (!itr->second->IsFitToRequirements(this, zone, area))
                RemoveAurasDueToSpell(itr->second->spellId);
    }

    SpellAreaForQuestMapBounds bounds = sSpellMgr.GetSpellAreaForQuestMapBounds(quest_id, false);
    if (bounds.first != bounds.second)
    {
        if (!zone || !area)
            GetZoneAndAreaId(zone, area);
        for (auto itr = bounds.first; itr != bounds.second; ++itr)
        {
            if (!itr->second->autocast || !itr->second->IsFitToRequirements(this, zone, area) ||
                HasAura(itr->second->spellId, EFFECT_INDEX_0))
                continue;
            SpellEntry const* entry = sSpellMgr.GetSpellEntry(itr->second->spellId);
            if (entry && IsLocalNativeQuestReward(entry, this, this))
            {
                if (!CastLocalNativeQuestReward(this, this, entry))
                    return false;
            }
            // Other aura/world effects retain their original position after
            // the external quest hook and world reward spell, following ACK.
        }
    }

    if (quest_id == 7787)
        AwardTitle(TITLE_STORMWIELDER);
    if (quest_id == 9270 || quest_id == 9269 || quest_id == 9271 || quest_id == 9257)
        AwardTitle(TITLE_GUARDIAN_OF_TIRISFAL);
    if (HasEarnedTitle(TITLE_LOREKEEPER))
        AwardTitle(TITLE_LOREKEEPER);
    if (HasEarnedTitle(TITLE_SEEKER_OF_KNOWLEDGE))
    {
        AwardTitle(TITLE_SEEKER_OF_KNOWLEDGE);
        std::string name = GetName();
        AtomicPlayerSaveScope::DeferAfterCommit([name]() { sWorld.SendWorldText(50305, name.c_str()); });
    }
    return true;
}
