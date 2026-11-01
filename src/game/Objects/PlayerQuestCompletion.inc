// Shared production automatic quest completion and actor-owned busy retry.

void Player::CompleteQuest(uint32 quest_id)
{
    if (quest_id)
    {
        Quest const* qInfo = sObjectMgr.GetQuestTemplate(quest_id);
        const bool autoReward = qInfo && qInfo->HasQuestFlag(QUEST_FLAGS_AUTO_REWARDED);
        std::unique_ptr<AtomicPlayerSaveScope> automatic;
        if (autoReward)
        {
            automatic = std::make_unique<AtomicPlayerSaveScope>(std::initializer_list<Player*>{this});
            if (!*automatic)
            {
                // A busy DB builder must not strand a flag quest COMPLETE but
                // unrewarded: CanCompleteQuest intentionally rejects COMPLETE.
                SetQuestStatus(quest_id, QUEST_STATUS_INCOMPLETE);
                _ScheduleAutoQuestRewardRetry(quest_id);
                return;
            }
        }

        SetQuestStatus(quest_id, QUEST_STATUS_COMPLETE);

        uint16 log_slot = FindQuestSlot(quest_id);
        if (log_slot < MAX_QUEST_LOG_SIZE)
            SetQuestSlotState(log_slot, QUEST_STATE_COMPLETE);

        if (qInfo)
        {
            if (autoReward)
            {
                ObjectGuid playerGuid = GetObjectGuid();
                AtomicPlayerSaveScope::DeferAfterCommit([playerGuid, quest_id]()
                {
                    Player* player = sObjectAccessor.FindPlayer(playerGuid);
                    Quest const* quest = sObjectMgr.GetQuestTemplate(quest_id);
                    if (!player || !quest)
                        return;
                    ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST, [&](PlayerScript* script)
                    {
                        script->OnPlayerCompleteQuest(player, quest);
                    });
                });
                if (!RewardQuest(qInfo, 0, this, false))
                {
                    automatic->Fail();
                    return;
                }
                if (automatic->IsOwner())
                    automatic->Commit();
            }
            else
                ScriptRegistry<PlayerScript>::ForEachEnabledHook(PLAYERHOOK_ON_PLAYER_COMPLETE_QUEST, [&](PlayerScript* script)
                {
                    script->OnPlayerCompleteQuest(this, qInfo);
                });
        }
    }
}

void Player::_ScheduleAutoQuestRewardRetry(uint32 questId)
{
    if (!m_pendingAutoQuestRewards.insert(questId).second)
        return;
    // Player owns this event processor: destruction cancels the callback.
    m_Events.AddLambdaEventAtOffset([this, questId]()
    {
        m_pendingAutoQuestRewards.erase(questId);
        if (CharacterDatabase.HasOrderedPersistenceFault() || sWorld.IsStopped())
            return;
        if (GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE && CanCompleteQuest(questId))
            CompleteQuest(questId);
    }, 1);
}
