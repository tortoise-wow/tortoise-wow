// Exact character-row serializer shared by ordinary saves and atomic core rewards.

bool Player::_AppendCharacterRowToDB(bool online)
{
    static SqlStatementID insChar;

    SqlStatement uberInsert = CharacterDatabase.CreateStatement(insChar, "REPLACE INTO characters (guid,account,name,race,class,gender,level,xp,money,playerBytes,playerBytes2,playerFlags,"
                              "map, position_x, position_y, position_z, orientation, "
                              "taximask, online, cinematic, "
                              "totaltime, leveltime, rest_bonus, logout_time, is_logout_resting, resettalents_multiplier, resettalents_time, "
                              "trans_x, trans_y, trans_z, trans_o, transguid, extra_flags, stable_slots, at_login, zone, "
                              "death_expire_time, taxi_path, "
                              "honorRankPoints, honorHighestRank, honorStanding, honorLastWeekHK, honorLastWeekCP, honorStoredHK, honorStoredDK, "
                              "watchedFaction, drunk, health, power1, power2, power3, "
                              "power4, power5, exploredZones, equipmentCache, ammoId, actionBars, "
                              "area, world_phase_mask, customFlags, city_protector, ignore_titles, mortality_status, total_deaths, xp_gain, extraBonusTalentCount) "
                              "VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,"
                              "?, ?, ?, ?, ?, "
                              "?, ?, ?, "
                              "?, ?, ?, ?, ?, ?, ?, "
                              "?, ?, ?, ?, ?, ?, ?, ?, ?, "
                              "?, ?, "
                              "?, ?, ?, ?, ?, ?, ?, "
                              "?, ?, ?, ?, ?, ?, "
                              "?, ?, ?, ?, ?, ?, "
                              "?, ?, ?, ?, ?, ?, ?, ?, ?) ");

    uberInsert.addUInt32(GetGUIDLow());
    uberInsert.addUInt32(GetSession()->GetAccountId());
    uberInsert.addString(m_name);
    uberInsert.addUInt8(GetRace());
    uberInsert.addUInt8(GetClass());
    uberInsert.addUInt8(GetGender());
    uberInsert.addUInt32(GetLevel());
    uberInsert.addUInt32(GetUInt32Value(PLAYER_XP));
    uberInsert.addUInt32(GetMoney());
    uberInsert.addUInt32(GetUInt32Value(PLAYER_BYTES));
    uberInsert.addUInt32(GetUInt32Value(PLAYER_BYTES_2));

    // Nostalrius: Fix toggled PvP flag after relog.
    uint32 playerFlags = GetUInt32Value(PLAYER_FLAGS) & ~(PLAYER_FLAGS_PVP_DESIRED);
    if (IsPvP())
        playerFlags |= PLAYER_FLAGS_PVP_DESIRED;
    uberInsert.addUInt32(playerFlags);

    if (!IsBeingTeleported())
    {
        uberInsert.addUInt32(GetMapId());
        uberInsert.addFloat(finiteAlways(GetPositionX()));
        uberInsert.addFloat(finiteAlways(GetPositionY()));
        uberInsert.addFloat(finiteAlways(GetPositionZ()));
        uberInsert.addFloat(MapManager::NormalizeOrientation(finiteAlways(GetOrientation())));
    }
    else
    {
        uberInsert.addUInt32(GetTeleportDest().mapId);
        uberInsert.addFloat(finiteAlways(GetTeleportDest().x));
        uberInsert.addFloat(finiteAlways(GetTeleportDest().y));
        uberInsert.addFloat(finiteAlways(GetTeleportDest().z));
        uberInsert.addFloat(MapManager::NormalizeOrientation(finiteAlways(GetTeleportDest().o)));
    }

    std::ostringstream ss;
    ss << m_taxi;                                   // string with TaxiMaskSize numbers
    uberInsert.addString(ss);

    if (!IsInWorld())
        online = false;
    uberInsert.addUInt32(online);

    uberInsert.addUInt32(m_cinematic);

    uberInsert.addUInt32(m_Played_time[PLAYED_TIME_TOTAL]);
    uberInsert.addUInt32(m_Played_time[PLAYED_TIME_LEVEL]);

    uberInsert.addFloat(finiteAlways(m_rest_bonus));
    uberInsert.addUInt64(uint64(time(nullptr)));
    uberInsert.addUInt32(HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_RESTING) ? 1 : 0);
    //save, far from tavern/city
    //save, but in tavern/city
    uberInsert.addUInt32(m_resetTalentsMultiplier);
    uberInsert.addUInt64(uint64(m_resetTalentsTime));

    uberInsert.addFloat(finiteAlways(m_movementInfo.GetTransportPos().x));
    uberInsert.addFloat(finiteAlways(m_movementInfo.GetTransportPos().y));
    uberInsert.addFloat(finiteAlways(m_movementInfo.GetTransportPos().z));
    uberInsert.addFloat(MapManager::NormalizeOrientation(finiteAlways(m_movementInfo.GetTransportPos().o)));
    if (m_transport)
        uberInsert.addUInt32(m_transport->GetGUIDLow());
    else
        uberInsert.addUInt32(0);

    uberInsert.addUInt32(m_ExtraFlags);

    uberInsert.addUInt32(uint32(m_stableSlots));                    // to prevent save uint8 as char

    uberInsert.addUInt32(uint32(m_atLoginFlags));

    uberInsert.addUInt32(GetCachedZoneId());

    uberInsert.addUInt64(uint64(m_deathExpireTime));

    ss << m_taxi.SaveTaxiDestinationsToString();       //string
    uberInsert.addString(ss);

    // Honor stored data
    uberInsert.addFloat(finiteAlways(m_honorMgr.GetRankPoints()));
    uberInsert.addUInt32(uint32(m_honorMgr.GetHighestRank().rank));
    uberInsert.addUInt32(m_honorMgr.GetStanding());
    uberInsert.addUInt32(m_honorMgr.GetLastWeekHK());
    uberInsert.addFloat(finiteAlways(m_honorMgr.GetLastWeekCP()));
    uberInsert.addUInt32(m_honorMgr.GetStoredHK());
    uberInsert.addUInt32(m_honorMgr.GetStoredDK());

    // FIXME: at this moment send to DB as unsigned, including unit32(-1)
    uberInsert.addUInt32(GetUInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX));

    uberInsert.addUInt16(uint16(GetUInt32Value(PLAYER_BYTES_3) & 0xFFFE));

    uberInsert.addUInt32(GetHealth());

    for (uint32 i = 0; i < MAX_POWERS; ++i)
        uberInsert.addUInt32(GetPower(Powers(i)));

    for (uint32 i = 0; i < PLAYER_EXPLORED_ZONES_SIZE; ++i)         //string
        ss << GetUInt32Value(PLAYER_EXPLORED_ZONES_1 + i) << " ";
    uberInsert.addString(ss);

    for (uint32 i = 0; i < EQUIPMENT_SLOT_END; ++i)         //string: item id, ench (perm/temp)
    {
        ss << GetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + i * MAX_VISIBLE_ITEM_OFFSET) << " ";

        uint32 ench1 = GetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + i * MAX_VISIBLE_ITEM_OFFSET + 1 + PERM_ENCHANTMENT_SLOT);
        uint32 ench2 = GetUInt32Value(PLAYER_VISIBLE_ITEM_1_0 + i * MAX_VISIBLE_ITEM_OFFSET + 1 + TEMP_ENCHANTMENT_SLOT);
        ss << uint32(MAKE_PAIR32(ench1, ench2)) << " ";
    }

    // 1 in vanilla/tbc - 4 in wotlk
    for (uint32 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_START + 1; ++i) // string: item id, ench (perm/temp)
    {
        ss << (m_items[i] ? m_items[i]->GetEntry() : 0) << " ";
        ss << uint32(MAKE_PAIR32(0, 0)) << " ";
    }

    uberInsert.addString(ss);

    uberInsert.addUInt32(GetUInt32Value(PLAYER_AMMO_ID));

    uberInsert.addUInt32(uint32(GetByteValue(PLAYER_FIELD_BYTES, 2)));
    // Nostalrius
    uberInsert.addUInt32(GetAreaId());
    uberInsert.addUInt32(GetWorldMask());
    uberInsert.addUInt32(customFlags);
    uberInsert.addUInt8(IsCityProtector() ? 1 : 0);
    uberInsert.addUInt8(IsIgnoringTitles() ? 1 : 0);
    uberInsert.addUInt8(m_hardcoreStatus);
    uberInsert.addUInt32(GetTotalDeathCount());
    uberInsert.addUInt8(HasXPGainEnabled());
    uberInsert.addUInt32(m_extraBonusTalentCount);

    // The character row belongs to the same transaction as inventory/quest state.
    // Direct mode controls the final commit, not an out-of-transaction first write.
    if (!uberInsert.Execute())
    {
        return false;
    }

    return true;
}
