// Serialize rewards before this call; only owned packet bytes cross the ACK boundary.
void Player::_SendRewardPacket(WorldPacket packet, bool broadcast) const
{
    if (!AtomicPlayerSaveScope::Active())
    {
        if (broadcast && GetGroup())
            // The actor owns a mutable Group; its broadcast API only sends
            // packets, but predates const-correctness. Keep XP callers const.
            const_cast<Group*>(GetGroup())->BroadcastPacket(&packet, true);
        else
            GetSession()->SendPacket(&packet);
        return;
    }

    ObjectGuid playerGuid = GetObjectGuid();
    AtomicPlayerSaveScope::DeferAfterCommit([playerGuid, packet = std::move(packet), broadcast]() mutable
    {
        Player* player = sObjectAccessor.FindPlayer(playerGuid);
        if (!player)
            return;
        if (broadcast && player->GetGroup())
            player->GetGroup()->BroadcastPacket(&packet, true);
        else
            player->GetSession()->SendPacket(&packet);
    });
}
