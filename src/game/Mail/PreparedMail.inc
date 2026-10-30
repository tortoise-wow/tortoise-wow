// Shared production mail preparation/publication implementation, exercised by
// deterministic actor/SQL-boundary tests without a database or game process.
namespace
{
thread_local MailPublicationCollector* mailPublicationCollector = nullptr;

void FailMailPublication()
{
    if (MailPublicationCollector* collector = MailDraft::GetPublicationCollector())
        collector->Fail();
    CharacterDatabase.MarkOrderedPersistenceFault();
}

bool MailContextAvailable()
{
    if (CharacterDatabase.HasOrderedPersistenceFault())
        return false;
    if (CharacterDatabase.InOwnedTransaction() && !MailDraft::GetPublicationCollector())
    {
        FailMailPublication();
        return false;
    }
    return true;
}

Player* MailMasterActor(MasterPlayer* master)
{
    return master && master->GetSession() ? master->GetSession()->GetPlayer() : nullptr;
}
}

MailPublicationCollector* MailDraft::GetPublicationCollector() { return mailPublicationCollector; }
MailPublicationCollector* MailDraft::SetPublicationCollector(MailPublicationCollector* collector)
{
    MailPublicationCollector* previous = mailPublicationCollector;
    mailPublicationCollector = collector;
    return previous;
}

MailDraft::MailDraft(uint16 mailTemplateId, bool need_items, LocaleConstant) :
    m_mailTemplateId(mailTemplateId), m_mailTemplateItemsNeed(need_items), m_bodyId(0), m_money(0), m_COD(0) {}

MailDraft::MailDraft(std::string subject, std::string text) :
    m_mailTemplateId(0), m_mailTemplateItemsNeed(false), m_subject(std::move(subject)),
    m_bodyId(0), m_bodyText(std::move(text)), m_money(0), m_COD(0) {}

MailDraft::~MailDraft()
{
    // External AddItem pointers are consumed on successful send, as before.
    // Only objects created by this draft are ours before preparation succeeds.
    for (uint32 guid : m_createdItems)
        if (m_items.count(guid)) delete m_items[guid];
}

MailDraft& MailDraft::SetSubjectAndBody(std::string subject, std::string text)
{
    MANGOS_ASSERT(!m_bodyId && m_bodyText.empty());
    m_subject = std::move(subject);
    m_bodyText = std::move(text);
    return *this;
}

MailDraft& MailDraft::AddItem(Item* item)
{
    if (!item) { m_preparationFailed = true; return *this; }
    m_items[item->GetGUIDLow()] = item;
    return *this;
}

bool MailDraft::prepareItems(Player* receiver)
{
    if (!m_mailTemplateId || !m_mailTemplateItemsNeed)
        return false;
    m_mailTemplateItemsNeed = false;
    Loot mailLoot(nullptr);
    mailLoot.FillLoot(m_mailTemplateId, LootTemplates_Mail, receiver, true, true);
    uint32 slots = mailLoot.GetMaxSlotInLootFor(receiver->GetGUIDLow());
    for (uint32 i = 0; m_items.empty() && i < slots; ++i)
        if (LootItem* lootitem = mailLoot.LootItemInSlot(i, receiver->GetGUIDLow()))
        {
            Item* item = Item::CreateItem(lootitem->itemid, lootitem->count, receiver);
            if (!item) { m_preparationFailed = true; break; }
            AddItem(item);
            m_createdItems.insert(item->GetGUIDLow());
        }
    // An online roll with zero items is still completed; never roll it again.
    return true;
}

void MailDraft::deleteIncludedItems(bool inDB)
{
    for (const auto& entry : m_items)
    {
        if (inDB) CharacterDatabase.PExecute("DELETE FROM item_instance WHERE guid='%u'", entry.first);
        delete entry.second;
    }
    m_items.clear();
    m_createdItems.clear();
}

void MailDraft::CloneFrom(MailDraft const& draft)
{
    m_mailTemplateId = draft.GetMailTemplateId();
    m_mailTemplateItemsNeed = draft.m_mailTemplateItemsNeed;
    m_subject = draft.GetSubject();
    MANGOS_ASSERT(!m_bodyId && m_bodyText.empty());
    m_bodyText = draft.m_bodyText;
    if (m_bodyText.empty() && draft.GetBodyId()) m_bodyText = sObjectMgr.GetItemText(draft.GetBodyId());
    m_money = draft.GetMoney();
    m_COD = draft.GetCOD();
    for (const auto& entry : draft.m_items)
    {
        Item* clone = entry.second->CloneItem(entry.second->GetCount());
        if (!clone) { m_preparationFailed = true; continue; }
        AddItem(clone);
        m_createdItems.insert(clone->GetGUIDLow());
    }
}

PreparedMail::PreparedMail() : m_mail(new Mail{}) {}
PreparedMail::~PreparedMail()
{
    for (const auto& entry : m_items)
        if (m_deleteOnAbort.count(entry.first)) delete entry.second;
}
ObjectGuid PreparedMail::GetReceiverGuid() const { return m_mail->receiverGuid; }
uint32 PreparedMail::GetMailId() const { return m_mail->messageID; }

std::unique_ptr<PreparedMail> MailDraft::Prepare(MailReceiver const& receiver, MailSender const& sender,
    MailCheckMask checked, uint32 deliver_delay, uint32 expire_delay)
{
    if (CharacterDatabase.InOwnedTransaction() &&
        !CharacterDatabase.IsOwnedTransactionParticipant(receiver.GetPlayerGuid().GetCounter())) return nullptr;
    Player* receiverActor = receiver.GetPlayer();
    if (CharacterDatabase.InOwnedTransaction())
    {
        if (!receiverActor) receiverActor = MailMasterActor(sObjectAccessor.FindMasterPlayer(receiver.GetPlayerGuid()));
        // Without the registered actor we cannot prove a pointer is detached
        // from a clean, unbagged inventory (all state flags may be clear).
        if (!receiverActor || receiverActor->GetObjectGuid() != receiver.GetPlayerGuid()) return nullptr;
    }
    bool hasItems = !m_items.empty();
    if (receiver.GetPlayer() && prepareItems(receiver.GetPlayer())) hasItems = true;
    if (m_preparationFailed || receiver.GetPlayerGuid().IsEmpty()) return nullptr;
    std::unique_ptr<PreparedMail> prepared(new PreparedMail);
    Mail& mail = *prepared->m_mail;
    mail.messageID = sObjectMgr.GenerateMailID();
    mail.messageType = uint8(sender.GetMailMessageType());
    mail.stationery = uint8(sender.GetStationery());
    mail.mailTemplateId = m_mailTemplateId;
    mail.sender = sender.GetSenderId();
    mail.receiverGuid = receiver.GetPlayerGuid();
    mail.subject = m_subject;
    mail.itemTextId = m_bodyId;
    if (!m_bodyText.empty())
    {
        m_bodyId = mail.itemTextId = sObjectMgr.GenerateItemTextID();
        prepared->m_bodyText = m_bodyText;
    }
    mail.has_items = hasItems;
    mail.deliver_time = time(nullptr) + deliver_delay;
    if (!expire_delay)
        expire_delay = (sender.GetMailMessageType() == MAIL_AUCTION && m_items.empty() && !m_money) ?
            HOUR : (m_COD ? 3 * DAY : 30 * DAY);
    mail.expire_time = mail.deliver_time + expire_delay;
    mail.money = m_money; mail.COD = m_COD; mail.checked = checked;
    prepared->m_returnToSender = m_returnToSender;
    prepared->m_deleteOnAbort = m_createdItems;
    prepared->m_items.swap(m_items);
    m_createdItems.clear();
    for (const auto& entry : prepared->m_items)
    {
        Item* item = entry.second;
        // Draft-created clones have no owner until a destination is known.
        if (CharacterDatabase.InOwnedTransaction() && item->GetOwnerGuid().IsEmpty() &&
            item->GetState() == ITEM_NEW && prepared->m_deleteOnAbort.count(entry.first))
            item->SetOwnerGuid(mail.receiverGuid);
        if (CharacterDatabase.InOwnedTransaction())
        {
            // Never adopt/delete a live inventory or existing mailbox object on
            // preparation failure; its current actor container still owns it.
            MasterPlayer* master = sObjectAccessor.FindMasterPlayer(mail.receiverGuid);
            MasterPlayer* ownerMaster = sObjectAccessor.FindMasterPlayer(item->GetOwnerGuid());
            Player* ownerActor = item->GetOwnerGuid() == mail.receiverGuid ? receiverActor : MailMasterActor(ownerMaster);
            if (item->IsInWorld() || item->GetContainer() || item->IsInUpdateQueue() ||
                item->GetState() == ITEM_REMOVED ||
                !CharacterDatabase.IsOwnedTransactionParticipant(item->GetOwnerGuid().GetCounter()) ||
                !ownerActor || ownerActor->GetObjectGuid() != item->GetOwnerGuid() ||
                receiverActor->GetItemByGuid(item->GetObjectGuid()) == item ||
                ownerActor->GetItemByGuid(item->GetObjectGuid()) == item ||
                (master && master->GetMItem(entry.first) == item) ||
                (ownerMaster && ownerMaster->GetMItem(entry.first) == item)) return nullptr;
            prepared->m_deleteOnAbort.insert(entry.first);
        }
        mail.AddItem(entry.first, item->GetEntry());
    }
    return prepared;
}

bool PreparedMail::AppendToDB()
{
    if (m_appended || CharacterDatabase.HasOrderedPersistenceFault() || !CharacterDatabase.InTransaction()) return false;
    bool owned = CharacterDatabase.InOwnedTransaction();
    if (owned && !CharacterDatabase.IsOwnedTransactionParticipant(m_mail->receiverGuid.GetCounter())) return false;
    for (const auto& entry : m_items)
    {
        Item* item = entry.second;
        if (item->GetState() == ITEM_REMOVED) return false;
        if (owned && (item->IsInWorld() || item->GetContainer() || item->IsInUpdateQueue() ||
            !CharacterDatabase.IsOwnedTransactionParticipant(item->GetOwnerGuid().GetCounter()))) return false;
    }
    if (!m_bodyText.empty())
    {
        static SqlStatementID insertText;
        auto stmt = CharacterDatabase.CreateStatement(insertText, "INSERT INTO item_text (id, text) VALUES (?, ?)");
        stmt.addUInt32(m_mail->itemTextId); stmt.addString(m_bodyText);
        if (!stmt.Execute()) return false;
    }
    for (const auto& entry : m_items)
    {
        Item* item = entry.second;
        if (owned)
        {
            if (!item->AppendSaveToDB()) return false;
        }
        else if (item->GetState() == ITEM_NEW || m_returnToSender)
        {
            ItemUpdateState before = item->GetState();
            item->SaveToDB();
            if ((before == ITEM_NEW || before == ITEM_CHANGED) && item->GetState() != ITEM_UNCHANGED) return false;
        }
        // Existing legacy callers already saved/overrode the owner before Send.
        // Do not re-save those attachments and silently undo their DB override.
        if (m_returnToSender && !CharacterDatabase.PExecute("UPDATE item_instance SET owner_guid = '%u' WHERE guid='%u'",
            m_mail->receiverGuid.GetCounter(), item->GetGUIDLow())) return false;
    }
    if (m_existingMail)
    {
        if (!CharacterDatabase.PExecute("UPDATE mail SET has_items = 1 WHERE id = %u", m_mail->messageID)) return false;
    }
    else
    {
        static SqlStatementID insertMail;
        auto stmt = CharacterDatabase.CreateStatement(insertMail,
            "INSERT INTO mail (id,messageType,stationery,mailTemplateId,sender,receiver,subject,itemTextId,has_items,expire_time,deliver_time,money,cod,checked) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?)");
        stmt.addUInt32(m_mail->messageID); stmt.addUInt8(m_mail->messageType); stmt.addUInt8(m_mail->stationery);
        stmt.addUInt16(m_mail->mailTemplateId); stmt.addUInt32(m_mail->sender); stmt.addUInt32(m_mail->receiverGuid.GetCounter());
        stmt.addString(m_mail->subject); stmt.addUInt32(m_mail->itemTextId); stmt.addUInt8(m_mail->has_items ? 1 : 0);
        stmt.addUInt64(m_mail->expire_time); stmt.addUInt64(m_mail->deliver_time); stmt.addUInt32(m_mail->money);
        stmt.addUInt32(m_mail->COD); stmt.addUInt32(m_mail->checked);
        if (!stmt.Execute()) return false;
    }
    for (const auto& entry : m_items)
        if (!CharacterDatabase.PExecute("INSERT INTO mail_items (mail_id,item_guid,item_template,receiver) VALUES ('%u','%u','%u','%u')",
            m_mail->messageID, entry.first, entry.second->GetEntry(), m_mail->receiverGuid.GetCounter())) return false;
    m_appended = true;
    return true;
}

bool PreparedMail::Publish()
{
    if (!m_appended || m_published || CharacterDatabase.InOwnedTransaction() || CharacterDatabase.HasOrderedPersistenceFault()) return false;
    if (!m_bodyText.empty()) sObjectMgr.AddItemText(m_mail->itemTextId, m_bodyText);
    MasterPlayer* receiver = sObjectAccessor.FindMasterPlayer(m_mail->receiverGuid);
    if (receiver)
    {
        std::unique_ptr<Mail> publication;
        if (!m_existingMail) publication.reset(new Mail(*m_mail));
        Mail* mail = m_existingMail ? receiver->GetMail(m_mail->messageID) : publication.get();
        if (!mail) return false;
        if (m_existingMail)
        {
            mail->has_items = true;
            for (const auto& entry : m_items) mail->AddItem(entry.first, entry.second->GetEntry());
        }
        while (!m_items.empty())
        {
            auto entry = m_items.begin(); Item* item = entry->second;
            item->CompleteSaveToDB();
            receiver->AddMItem(item);
            m_deleteOnAbort.erase(entry->first); m_items.erase(entry);
        }
        if (!m_existingMail)
        {
            receiver->AddMail(mail);
            publication.release();
            receiver->AddNewMailDeliverTime(mail->deliver_time);
        }
    }
    else
    {
        for (const auto& entry : m_items) delete entry.second;
        m_items.clear(); m_deleteOnAbort.clear();
    }
    m_published = true;
    return true;
}

bool MailDraft::DispatchPrepared(std::unique_ptr<PreparedMail> mail, bool ownsLegacyTransaction, bool direct)
{
    if (!mail || !mail->AppendToDB())
    {
        if (ownsLegacyTransaction && CharacterDatabase.InTransaction()) CharacterDatabase.RollbackTransaction();
        FailMailPublication(); return false;
    }
    if (CharacterDatabase.InOwnedTransaction())
    {
        MailPublicationCollector* collector = GetPublicationCollector();
        if (!collector || !collector->Collect(std::move(mail))) { FailMailPublication(); return false; }
        return true;
    }
    if (ownsLegacyTransaction && !(direct ? CharacterDatabase.CommitTransactionDirect() : CharacterDatabase.CommitTransaction()))
    {
        FailMailPublication(); return false;
    }
    // Borrowed/ordinary asynchronous legacy sends retain optimistic publication;
    // durable failure retention for those callers remains A06, not this owned path.
    if (!mail->Publish()) { FailMailPublication(); return false; }
    return true;
}

void MailDraft::SendMailTo(MailReceiver const& receiver, MailSender const& sender, MailCheckMask checked,
    uint32 deliver_delay, uint32 expire_delay, bool direct)
{
    if (!MailContextAvailable()) return;
    bool ownsLegacyTransaction = !CharacterDatabase.InTransaction();
    if (ownsLegacyTransaction && !CharacterDatabase.BeginTransaction(receiver.GetPlayerGuid().GetCounter())) { FailMailPublication(); return; }
    try
    {
        ScriptRegistry<MailScript>::ForEach([&](MailScript* script) { script->OnBeforeMailDraftSendMailTo(this, receiver, sender); });
        DispatchPrepared(Prepare(receiver, sender, checked, deliver_delay, expire_delay), ownsLegacyTransaction, direct);
    }
    catch (...)
    {
        if (ownsLegacyTransaction && CharacterDatabase.InTransaction()) CharacterDatabase.RollbackTransaction();
        FailMailPublication();
    }
}

void MailDraft::SendReturnToSender(uint32, ObjectGuid sender_guid, ObjectGuid receiver_guid)
{
    if (!MailContextAvailable()) return;
    Player* receiver = sObjectMgr.GetPlayer(receiver_guid);
    uint32 account = receiver ? 0 : sObjectMgr.GetPlayerAccountIdByGUID(receiver_guid);
    if (!receiver && !account)
    {
        if (CharacterDatabase.InOwnedTransaction()) { FailMailPublication(); return; }
        deleteIncludedItems(true); return;
    }
    m_returnToSender = true;
    SendMailTo(MailReceiver(receiver, receiver_guid), MailSender(MAIL_NORMAL, sender_guid.GetCounter()), MAIL_CHECK_MASK_RETURNED, 0);
}

void Mail::prepareTemplateItems(Player* receiver)
{
    MasterPlayer* master = receiver->GetSession()->GetMasterPlayer();
    if (!mailTemplateId || has_items || !items.empty() || !master || !MailContextAvailable()) return;
    bool ownsLegacyTransaction = !CharacterDatabase.InTransaction();
    if (ownsLegacyTransaction && !CharacterDatabase.BeginTransaction(receiver->GetGUIDLow())) { FailMailPublication(); return; }
    try
    {
        MailDraft draft(mailTemplateId, true);
        draft.prepareItems(receiver);
        if (draft.m_preparationFailed) { FailMailPublication(); if (ownsLegacyTransaction) CharacterDatabase.RollbackTransaction(); return; }
        std::unique_ptr<PreparedMail> prepared(new PreparedMail);
        *prepared->m_mail = *this;
        prepared->m_mail->has_items = true;
        prepared->m_existingMail = true;
        prepared->m_items.swap(draft.m_items);
        prepared->m_deleteOnAbort.swap(draft.m_createdItems);
        MailDraft::DispatchPrepared(std::move(prepared), ownsLegacyTransaction, false);
    }
    catch (...)
    {
        if (ownsLegacyTransaction && CharacterDatabase.InTransaction()) CharacterDatabase.RollbackTransaction();
        FailMailPublication();
    }
}
