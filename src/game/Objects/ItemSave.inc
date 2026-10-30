// Shared production item persistence implementation; also compiled by storage tests.

void Item::SaveToDB(bool direct)
{
    if (CharacterDatabase.HasOrderedPersistenceFault())
        return;
    if (CharacterDatabase.InOwnedTransaction())
    {
        // Legacy callers cannot consume an owned transaction's pending state.
        CharacterDatabase.MarkOrderedPersistenceFault();
        return;
    }

    if (AppendSaveToDB(direct, false))
        CompleteSaveToDB(false);
}

bool Item::AppendSaveToDB()
{
    if (CharacterDatabase.HasOrderedPersistenceFault() ||
        !CharacterDatabase.IsOwnedTransactionParticipant(GetOwnerGuid().GetCounter()))
        return false;

    return AppendSaveToDB(false, true);
}

bool Item::AppendSaveToDB(bool direct, bool deferred)
{
    uint32 guid = GetGUIDLow();
    switch (uState)
    {
        case ITEM_CHANGED:
        {
            static SqlStatementID updGifts;
            if (HasFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_WRAPPED))
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(updGifts, "UPDATE `character_gifts` SET `guid` = ? WHERE `item_guid` = ?");
                if (!stmt.PExecute(GetOwnerGuid().GetCounter(), GetGUIDLow()))
                    return false;
            }
        }
        // no break
        case ITEM_NEW:
        {
            static SqlStatementID insItem;
            static SqlStatementID updItem;

            SqlStatement stmt = (uState == ITEM_NEW) ?
                                CharacterDatabase.CreateStatement(insItem, "REPLACE INTO `item_instance` (`itemEntry`, `owner_guid`, `creatorGuid`, `giftCreatorGuid`, `count`, `duration`, `charges`, `flags`, `enchantments`, `randomPropertyId`, `transmogrifyId`, `durability`, `text`, `generated_loot`, `guid`) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)")
                                :
                                CharacterDatabase.CreateStatement(updItem, "UPDATE `item_instance` SET `itemEntry` = ?, `owner_guid` = ?, `creatorGuid` = ?, `giftCreatorGuid` = ?, `count` = ?, `duration` = ?, `charges` = ?, `flags` = ?, `enchantments` = ?, `randomPropertyId` = ?, `transmogrifyId` = ?, `durability` = ?, `text` = ?, `generated_loot` = ? WHERE `guid` = ?");
            stmt.addUInt32(GetEntry());
            stmt.addUInt32(GetOwnerGuid().GetCounter());
            stmt.addUInt32(GetGuidValue(ITEM_FIELD_CREATOR).GetCounter());
            stmt.addUInt32(GetGuidValue(ITEM_FIELD_GIFTCREATOR).GetCounter());
            stmt.addUInt32(GetCount());
            stmt.addUInt32(GetUInt32Value(ITEM_FIELD_DURATION));

            std::ostringstream ssSpells;
            for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                ssSpells << GetSpellCharges(i) << ' ';
            stmt.addString(ssSpells.str());

            stmt.addUInt32(GetUInt32Value(ITEM_FIELD_FLAGS));

            std::ostringstream ssEnchants;
            for (uint8 i = 0; i < MAX_ENCHANTMENT_SLOT; ++i)
            {
                ssEnchants << GetEnchantmentId(EnchantmentSlot(i)) << ' ';
                ssEnchants << GetEnchantmentDuration(EnchantmentSlot(i)) << ' ';
                ssEnchants << GetEnchantmentCharges(EnchantmentSlot(i)) << ' ';
            }
            stmt.addString(ssEnchants.str());

            stmt.addUInt16(GetItemRandomPropertyId());
            stmt.addUInt32(GetTransmogrification());
            stmt.addUInt16(GetUInt32Value(ITEM_FIELD_DURABILITY));
            stmt.addUInt32(GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID));
            stmt.addUInt8(generatedLoot); // can't use bool, SQL ERROR: Using unsupported buffer type: 16  (parameter: 13), todo, maybe.
            stmt.addUInt32(guid);
            if (!(direct ? stmt.DirectExecute() : stmt.Execute()))
                return false;
        }
        break;
        case ITEM_REMOVED:
        {
            static SqlStatementID delItemText;
            static SqlStatementID delInst ;
            static SqlStatementID delGifts ;
            static SqlStatementID delLoot ;

            // Cache invalidation belongs to game-thread completion. The deferred
            // path must put its database delete into the same owned transaction.
            if (deferred && GetTransmogrification() &&
                sObjectMgr.GetItemTransmogrifyTemplate(GetTransmogrification()) &&
                !CharacterDatabase.PExecute("DELETE FROM `item_transmogs` WHERE `ID`=%u", GetTransmogrification()))
                return false;

            if (uint32 item_text_id = GetUInt32Value(ITEM_FIELD_ITEM_TEXT_ID))
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(delItemText, "DELETE FROM `item_text` WHERE `id` = ?");
                if (!stmt.PExecute(item_text_id))
                    return false;
            }

            SqlStatement stmt = CharacterDatabase.CreateStatement(delInst, "DELETE FROM `item_instance` WHERE `guid` = ?");
            //sTransmog.DeleteTransmogItemFromDB(guid);
            if (!stmt.PExecute(guid))
                return false;

            if (HasFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_WRAPPED))
            {
                stmt = CharacterDatabase.CreateStatement(delGifts, "DELETE FROM `character_gifts` WHERE `item_guid` = ?");
                if (!stmt.PExecute(GetGUIDLow()))
                    return false;
            }

            if (HasSavedLoot())
            {
                stmt = CharacterDatabase.CreateStatement(delLoot, "DELETE FROM `item_loot` WHERE `guid` = ?");
                if (!stmt.PExecute(GetGUIDLow()))
                    return false;
            }
            return true;
        }
        case ITEM_UNCHANGED:
		case ITEM_STASHED:
            return true;
    }

    if (m_lootState == ITEM_LOOT_CHANGED || m_lootState == ITEM_LOOT_REMOVED)
    {
        static SqlStatementID delLoot ;

        SqlStatement stmt = CharacterDatabase.CreateStatement(delLoot, "DELETE FROM `item_loot` WHERE `guid` = ?");
        if (!stmt.PExecute(GetGUIDLow()))
            return false;
    }

    if (m_lootState == ITEM_LOOT_NEW || m_lootState == ITEM_LOOT_CHANGED)
    {
        if (auto ownerGuid = GetOwnerGuid())
        {
            static SqlStatementID saveGold ;
            static SqlStatementID saveLoot ;

            // save money as 0 itemid data
            if (loot.gold)
            {
                SqlStatement stmt = CharacterDatabase.CreateStatement(saveGold, "INSERT INTO `item_loot` (`guid`, `owner_guid`, `itemid`, `amount`, `property`) VALUES (?, ?, 0, ?, 0)");
                if (!stmt.PExecute(GetGUIDLow(), ownerGuid.GetCounter(), loot.gold))
                    return false;
            }

            SqlStatement stmt = CharacterDatabase.CreateStatement(saveLoot, "INSERT INTO `item_loot` (`guid`, `owner_guid`, `itemid`, `amount`, `property`) VALUES (?, ?, ?, ?, ?)");

            // save items and quest items (at load its all will added as normal, but this not important for item loot case)
            for (size_t i = 0; i < loot.GetMaxSlotInLootFor(ownerGuid); ++i)
            {
                QuestItem* qitem = nullptr;

                LootItem* item = loot.LootItemInSlot(i, ownerGuid, &qitem);
                if (!item)
                    continue;

                // questitems use the blocked field for other purposes
                if (!qitem && item->is_blocked)
                    continue;

                stmt.addUInt32(GetGUIDLow());
                stmt.addUInt32(ownerGuid);
                stmt.addUInt32(item->itemid);
                stmt.addUInt8(item->count);
                stmt.addInt32(item->randomPropertyId);

                if (!stmt.Execute())
                    return false;
            }
        }

    }

    return true;
}

void Item::CompleteSaveToDB()
{
    CompleteSaveToDB(true);
}

void Item::CompleteSaveToDB(bool deferred)
{
    if (uState == ITEM_UNCHANGED || uState == ITEM_STASHED)
        return;

    if (uState == ITEM_REMOVED)
    {
        if (GetTransmogrification())
            sObjectMgr.DeleteItemTransmogrifyTemplate(GetTransmogrification(), !deferred);
        if (IsInWorld())
            RemoveFromWorld();
        delete this;
        return;
    }

    if (m_lootState != ITEM_LOOT_NONE && m_lootState != ITEM_LOOT_TEMPORARY)
        SetLootState(ITEM_LOOT_UNCHANGED);

    SetState(ITEM_UNCHANGED);
}

