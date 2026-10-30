#include "inventory_save_environment.h"

// These are the real production map/reputation serializers and ACK consumers.
// Character-row transport is stubbed here; the exact extracted character binds
// are compiled by the full server and verified by native quest integration tests.
#include "../../src/game/ReputationSave.inc"
#include "../../src/game/Objects/PlayerCoreSave.inc"

void ResetCore()
{
    CharacterDatabase = TestDatabase{};
    sObjectMgr = TestObjectMgr{};
    Item::deleted = 0;
}

struct CoreFixture
{
    Player player {1};
    Item* fresh;
    Item* removed;
    CoreFixture()
    {
        fresh = new Item(10, &player, ITEM_NEW, 1);
        player.m_items[1] = fresh;
        fresh->AddToUpdateQueueOf(&player);
        removed = new Item(20, &player, ITEM_REMOVED, 2);
        removed->AddToUpdateQueueOf(&player);
        for (uint32 id = 100; id != 104; ++id)
        {
            QuestStatusData quest {};
            quest.uState = QuestUpdateState(id - 100);
            player.mQuestStatus.emplace(id, quest);
        }
        player.mSkillStatus.emplace(200, SkillStatusData{0, SKILL_NEW});
        player.mSkillStatus.emplace(201, SkillStatusData{1, SKILL_CHANGED});
        player.mSkillStatus.emplace(202, SkillStatusData{2, SKILL_DELETED});
        player.mSkillStatus.emplace(203, SkillStatusData{3, SKILL_UNCHANGED});
        player.skillValues[100] = (225 << 16) | 100;
        player.skillValues[101] = (300 << 16) | 125;
        player.m_mForgottenSkills[204] = 75;
        player.m_spells.emplace(300, PlayerSpell{PLAYERSPELL_NEW, true, false, false});
        player.m_spells.emplace(301, PlayerSpell{PLAYERSPELL_CHANGED, false, false, true});
        player.m_spells.emplace(302, PlayerSpell{PLAYERSPELL_REMOVED});
        player.m_spells.emplace(303, PlayerSpell{PLAYERSPELL_UNCHANGED});
        player.m_spells.emplace(304, PlayerSpell{PLAYERSPELL_NEW, true, true, false});
        player.m_reputationMgr.m_factions.emplace(0, FactionState{72, 3, -250, true});
        player.m_reputationMgr.m_factions.emplace(1, FactionState{73, 4, 500, false});
        player.m_variables.emplace(1, "O'Brien\\path; core-reward-value");
        player.m_unsavedItemLogs.push_back({});
        player.m_unsavedItemLogs.push_back({});
    }
    ~CoreFixture() { delete fresh; if (removed) delete removed; }
    void AssertUnconsumed() const
    {
        assert(Item::deleted == 0 && fresh->uState == ITEM_NEW && removed->uState == ITEM_REMOVED);
        assert(player.m_itemUpdateQueue.size() == 2);
        assert(player.mQuestStatus.size() == 4 && player.mQuestStatus.at(100).uState == QUEST_NEW);
        assert(player.mQuestStatus.at(101).uState == QUEST_CHANGED && player.mQuestStatus.at(103).uState == QUEST_DELETED);
        assert(player.mSkillStatus.size() == 4 && player.mSkillStatus.at(200).uState == SKILL_NEW);
        assert(player.mSkillStatus.at(201).uState == SKILL_CHANGED && player.mSkillStatus.at(202).uState == SKILL_DELETED);
        assert(player.m_spells.size() == 5 && player.m_spells.at(300).state == PLAYERSPELL_NEW);
        assert(player.m_spells.at(301).state == PLAYERSPELL_CHANGED && player.m_spells.at(302).state == PLAYERSPELL_REMOVED);
        assert(player.m_reputationMgr.m_factions.at(0).needSave && player.m_unsavedItemLogs.size() == 2);
    }
};

size_t CoreSqlCount(std::string const& needle)
{
    return std::count_if(CharacterDatabase.queued.begin(), CharacterDatabase.queued.end(),
        [&](RecordedSql const& sql) { return sql.text.find(needle) != std::string::npos; });
}

void CoreSnapshotAndAcknowledgement()
{
    ResetCore(); CoreFixture fixture; Player::PlayerSaveBatch batch;
    assert(fixture.player.AppendFullPlayerSave(batch));
    fixture.AssertUnconsumed();
    assert(CoreSqlCount("CHARACTER_ROW_TRANSPORT_BOUNDARY") == 1);
    assert(CoreSqlCount("INSERT INTO character_inventory") == 1);
    assert(CoreSqlCount("DELETE FROM character_inventory") == 1);
    assert(CoreSqlCount("character_queststatus") == 3);
    assert(CoreSqlCount("character_skills") == 3);
    assert(CoreSqlCount("character_forgotten_skills") == 1);
    assert(CoreSqlCount("character_spell ") == 4); // dependent NEW is not persisted
    assert(CoreSqlCount("character_reputation") == 2);
    assert(CoreSqlCount("character_variables") == 1 && CoreSqlCount("character_item_logs") == 2);
    for (auto const& sql : CharacterDatabase.queued)
    {
        if (sql.text.find("REPLACE INTO character_queststatus") != std::string::npos)
            assert(sql.values.at(3) == "1" && sql.values.back() == "42");
        if (sql.text.find("UPDATE character_skills") != std::string::npos)
            assert(sql.values == std::vector<std::string>({"125", "300", "1", "201"}));
        if (sql.text.find("INSERT INTO character_reputation") != std::string::npos)
            assert(sql.values == std::vector<std::string>({"1", "72", "-250", "3"}));
        if (sql.text.find("character_variables") != std::string::npos)
        {
            assert(sql.text == "REPLACE INTO `character_variables` (`lowGuid`, `variableType`, `value`) VALUES (?, ?, ?)");
            assert(sql.values == std::vector<std::string>({"1", "1", "O'Brien\\path; core-reward-value"}));
            assert(sql.text.find("O'Brien") == std::string::npos);
        }
    }
    size_t statements = CharacterDatabase.queued.size();
    // The outer owner calls completion only after its actual COMMIT ACK.
    assert(fixture.player.CompleteFullPlayerSave(batch));
    fixture.removed = nullptr;
    assert(Item::deleted == 1 && fixture.fresh->uState == ITEM_UNCHANGED);
    assert(fixture.player.mQuestStatus.size() == 3 && !fixture.player.mQuestStatus.count(103));
    assert(fixture.player.mQuestStatus.at(100).uState == QUEST_UNCHANGED);
    assert(fixture.player.mSkillStatus.size() == 3 && !fixture.player.mSkillStatus.count(202));
    assert(fixture.player.mSkillStatus.at(200).uState == SKILL_UNCHANGED);
    assert(fixture.player.m_spells.size() == 4 && !fixture.player.m_spells.count(302));
    assert(fixture.player.m_spells.at(300).state == PLAYERSPELL_UNCHANGED);
    assert(fixture.player.m_spells.at(304).state == PLAYERSPELL_UNCHANGED);
    assert(!fixture.player.m_reputationMgr.m_factions.at(0).needSave);
    assert(fixture.player.m_unsavedItemLogs.empty() && CharacterDatabase.queued.size() == statements);
    assert(!fixture.player.CompleteFullPlayerSave(batch));
}

void EveryCoreAppendFailureRetainsAllDirtyState()
{
    size_t total = 0;
    for (size_t fail = 0; fail <= total; ++fail)
    {
        ResetCore(); CoreFixture fixture; Player::PlayerSaveBatch batch;
        CharacterDatabase.failAt = fail;
        bool ok = fixture.player.AppendFullPlayerSave(batch);
        if (fail == 0) { assert(ok); total = CharacterDatabase.attempts; }
        else assert(!ok);
        fixture.AssertUnconsumed();
        // Rejection/failure/unknown COMMIT does not consume the capture on destruction.
    }
    assert(total > 20);
}

void GuardsAndCompletionValidation()
{
    for (int guard=0; guard != 4; ++guard)
    {
        ResetCore(); CoreFixture fixture; Player::PlayerSaveBatch batch;
        if (guard == 0) fixture.player.m_DbSaveDisabled = true;
        if (guard == 1) fixture.player.teleported = true;
        if (guard == 2) CharacterDatabase.fault = true;
        if (guard == 3) CharacterDatabase.participants.clear();
        assert(!fixture.player.AppendFullPlayerSave(batch) && CharacterDatabase.queued.empty());
        fixture.AssertUnconsumed();
    }
    ResetCore(); CoreFixture fixture; Player::PlayerSaveBatch batch;
    assert(fixture.player.AppendFullPlayerSave(batch));
    size_t statements = CharacterDatabase.queued.size();
    assert(!fixture.player.AppendFullPlayerSave(batch) && CharacterDatabase.queued.size() == statements);
    fixture.player.mQuestStatus.at(100).uState = QUEST_CHANGED;
    assert(!fixture.player.CompleteFullPlayerSave(batch));
    assert(Item::deleted == 0 && fixture.player.m_unsavedItemLogs.size() == 2);
    fixture.player.mQuestStatus.at(100).uState = QUEST_NEW;
    fixture.AssertUnconsumed();

    ResetCore(); Player player(1); ReputationMgr::SaveBatch reputation;
    player.m_reputationMgr.m_factions.emplace(0, FactionState{72, 0, 1, true});
    player.m_reputationMgr.SaveToDB(); // legacy consuming API refuses the owned context
    assert(CharacterDatabase.fault && player.m_reputationMgr.m_factions.at(0).needSave);
}

void OrdinaryMapSerializerCompatibility()
{
    ResetCore(); CoreFixture fixture;
    CharacterDatabase.owned = false;
    assert(fixture.player._SaveQuestStatus());
    assert(fixture.player._SaveSkills());
    assert(fixture.player._SaveSpells());
    fixture.player.m_reputationMgr.SaveToDB();
    assert(!fixture.player.mQuestStatus.count(103) && fixture.player.mQuestStatus.at(100).uState == QUEST_UNCHANGED);
    assert(!fixture.player.mSkillStatus.count(202) && fixture.player.mSkillStatus.at(200).uState == SKILL_UNCHANGED);
    assert(!fixture.player.m_spells.count(302) && fixture.player.m_spells.at(300).state == PLAYERSPELL_UNCHANGED);
    assert(!fixture.player.m_reputationMgr.m_factions.at(0).needSave);
    assert(Item::deleted == 0 && fixture.fresh->uState == ITEM_NEW);
}

int main()
{
    CoreSnapshotAndAcknowledgement();
    EveryCoreAppendFailureRetainsAllDirtyState();
    GuardsAndCompletionValidation();
    OrdinaryMapSerializerCompatibility();
    std::cout << "Production core player save tests passed.\n";
}
