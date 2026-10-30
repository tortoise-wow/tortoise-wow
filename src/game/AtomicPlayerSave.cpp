#include "AtomicPlayerSave.h"

#include "Player.h"
#include "Mail/Mail.h"
#include "Database/DatabaseEnv.h"
#include "World.h"
#include "Log.h"
#include <algorithm>
#include <map>
#include <vector>

struct AtomicPlayerSaveScope::State : MailPublicationCollector
{
    struct Participant
    {
        Player* player = nullptr;
        bool full = false;
        std::unique_ptr<Player::InventorySaveBatch> inventory;
        std::unique_ptr<Player::PlayerSaveBatch> snapshot;
    };

    std::map<uint32, Participant> participants;
    std::unique_ptr<Database::OwnedTransaction> sql;
    std::vector<std::unique_ptr<PreparedMail>> mail;
    std::vector<std::function<void()>> afterCommit;
    MailPublicationCollector* previousCollector = nullptr;
    bool failed = false;
    bool sealing = false;
    bool finished = false;

    bool Collect(std::unique_ptr<PreparedMail> prepared) override
    {
        if (!prepared || failed || sealing || finished)
        {
            Fail();
            return false;
        }
        mail.push_back(std::move(prepared));
        return true;
    }

    void Fail() override { failed = true; }

    void Deactivate()
    {
        if (AtomicPlayerSaveScope::s_current == this)
        {
            AtomicPlayerSaveScope::s_current = nullptr;
            MailDraft::SetPublicationCollector(previousCollector);
        }
    }

    void StopUnconfirmed()
    {
        failed = true;
        finished = true;
        Deactivate();
        sql.reset();
        CharacterDatabase.MarkOrderedPersistenceFault();
        sLog.outError("Atomic player save could not be completed safely; later character writes are fenced. Stopping realm.");
        World::StopNow(ERROR_EXIT_CODE);
    }
};

thread_local AtomicPlayerSaveScope::State* AtomicPlayerSaveScope::s_current = nullptr;

AtomicPlayerSaveScope::AtomicPlayerSaveScope(std::initializer_list<Player*> players)
{
    if (players.size() == 0)
        return;
    if (s_current)
    {
        m_state = s_current;
        for (Player* player : players)
        {
            if (!player || !player->IsInventorySaveEnabled() ||
                !CharacterDatabase.IsOwnedTransactionParticipant(player->GetGUIDLow()))
            {
                m_state->Fail();
                m_state = nullptr;
                return;
            }
        }
        return;
    }
    if (MailDraft::GetPublicationCollector())
        return;
    auto state = std::make_unique<State>();
    std::vector<uint32> guids;
    for (Player* player : players)
    {
        if (!player || !player->IsInventorySaveEnabled() ||
            !state->participants.emplace(player->GetGUIDLow(), State::Participant{player}).second)
            return;
        guids.push_back(player->GetGUIDLow());
    }
    state->sql = CharacterDatabase.BeginOwnedTransaction(std::move(guids));
    if (!state->sql)
        return;
    state->previousCollector = MailDraft::GetPublicationCollector();
    m_owner = std::move(state);
    m_state = m_owner.get();
    s_current = m_state;
    MailDraft::SetPublicationCollector(m_state);
}

AtomicPlayerSaveScope::~AtomicPlayerSaveScope()
{
    if (m_owner && !m_owner->finished)
        m_owner->StopUnconfirmed();
}

AtomicPlayerSaveScope::operator bool() const
{
    return m_state && !m_state->failed && !m_state->finished;
}

bool AtomicPlayerSaveScope::RequestFullSave(Player& player)
{
    if (!*this || m_state->sealing)
        return false;
    auto found = m_state->participants.find(player.GetGUIDLow());
    if (found == m_state->participants.end() || found->second.player != &player)
    {
        Fail();
        return false;
    }
    found->second.full = true;
    return true;
}

void AtomicPlayerSaveScope::Fail()
{
    if (m_state)
        m_state->Fail();
}

bool AtomicPlayerSaveScope::Active() { return s_current && !s_current->finished; }

void AtomicPlayerSaveScope::DeferAfterCommit(std::function<void()> action)
{
    if (!action)
        return;
    if (!s_current)
    {
        action();
        return;
    }
    if (s_current->failed || s_current->sealing || s_current->finished)
    {
        s_current->Fail();
        return;
    }
    s_current->afterCommit.push_back(std::move(action));
}

bool AtomicPlayerSaveScope::Commit()
{
    if (!m_owner)
        return bool(*this); // Nested rewards only stage work for the outer owner.
    State& state = *m_owner;
    if (state.finished)
        return false;
    try
    {
        state.sealing = true;
        if (!state.failed)
        {
            for (auto& entry : state.participants)
            {
                auto& participant = entry.second;
                bool appended;
                if (participant.full)
                {
                    participant.snapshot = std::make_unique<Player::PlayerSaveBatch>();
                    appended = participant.player->AppendFullPlayerSave(*participant.snapshot);
                }
                else
                {
                    participant.inventory = std::make_unique<Player::InventorySaveBatch>();
                    appended = participant.player->AppendInventoryAndGoldToDB(*participant.inventory);
                }
                if (!appended) { state.failed = true; break; }
            }
        }
        if (state.failed || !state.sql->Commit())
        {
            state.StopUnconfirmed();
            return false;
        }
        state.sql.reset();
        // No callbacks/publication may accidentally join the now-committed scope.
        state.Deactivate();
        for (auto& entry : state.participants)
        {
            auto& participant = entry.second;
            bool completed = participant.full ?
                participant.player->CompleteFullPlayerSave(*participant.snapshot) :
                participant.player->CompleteInventoryAndGoldSave(*participant.inventory);
            if (!completed) { state.StopUnconfirmed(); return false; }
        }
        for (auto& prepared : state.mail)
            if (!prepared->Publish()) { state.StopUnconfirmed(); return false; }
        state.finished = true;
        for (auto& action : state.afterCommit)
            action();
        return true;
    }
    catch (...)
    {
        state.StopUnconfirmed();
        return false;
    }
}
