/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Object.h"
#include "Memory/MemoryLedger.h"
#include "DetailedWorkDiagnostics.h"
#include <shared_mutex>

#include "SharedDefines.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "World.h"
#include "Creature.h"
#include "Player.h"
#include "Totem.h"
#include "ObjectMgr.h"
#include "ObjectGuid.h"
#include "UpdateData.h"
#include "UpdateMask.h"
#include "Util.h"
#include "MapManager.h"
#include "Log.h"
#include "Transport.h"
#include "TargetedMovementGenerator.h"
#include "WaypointMovementGenerator.h"
#include "VMapFactory.h"
#include "CellImpl.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Language.h"
#include "Geometry.h"
#include "ObjectPosSelector.h"

#include "TemporarySummon.h"
#include "ZoneScriptMgr.h"
#include "InstanceData.h"
#include "Chat.h"
#include "Anticheat.h"
#include "ScriptObjects.h"
#include "SpellClassMask.h"

#include "packet_builder.h"
#include "MovementBroadcaster.h"
#include "PlayerBroadcaster.h"

////////////////////////////////////////////////////////////
// Methods of class MovementInfo

void MovementInfo::Read(ByteBuffer &data)
{
    stime = WorldTimer::getMSTime();
    data >> moveFlags;
    data >> ctime;
    data >> pos.x;
    data >> pos.y;
    data >> pos.z;
    data >> pos.o;

    if (HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        data >> t_guid;
        data >> t_pos.x;
        data >> t_pos.y;
        data >> t_pos.z;
        data >> t_pos.o;
    }
    if (HasMovementFlag(MOVEFLAG_SWIMMING))
        data >> s_pitch;

    data >> fallTime;

    if (HasMovementFlag(MOVEFLAG_JUMPING))
    {
        data >> jump.zspeed;
        data >> jump.cosAngle;
        data >> jump.sinAngle;
        data >> jump.xyspeed;
        if (!jump.startClientTime)
        {
            jump.startClientTime = ctime;
            jump.start = pos;
        }
    }
    else
        jump.startClientTime = 0;

    if (HasMovementFlag(MOVEFLAG_SPLINE_ELEVATION))
    {
        data >> splineElevation;                                     // unknown
    }
}

void MovementInfo::CorrectData(Unit* mover)
{
    // Nostalrius: remove incompatible flags, causing client freezes for example
#define REMOVE_VIOLATING_FLAGS(check, maskToRemove) \
        if (check) \
            RemoveMovementFlag(MovementFlags(maskToRemove));


    /*! This must be a packet spoofing attempt. MOVEFLAG_ROOT sent from the client is not valid
        in conjunction with any of the moving movement flags such as MOVEMENTFLAG_FORWARD.
        It will freeze clients that receive this player's movement info.
    */
    REMOVE_VIOLATING_FLAGS(HasMovementFlag(MOVEFLAG_ROOT) && HasMovementFlag(MOVEFLAG_MASK_MOVING),
                           MOVEFLAG_ROOT);

    //! Cannot hover without SPELL_AURA_HOVER
    //REMOVE_VIOLATING_FLAGS(HasMovementFlag(MOVEFLAG_HOVER) && !GetPlayer()->HasAuraType(SPELL_AURA_HOVER),
    //    MOVEFLAG_HOVER);

    //! Cannot move left and right at the same time
    REMOVE_VIOLATING_FLAGS(HasMovementFlag(MOVEFLAG_TURN_LEFT) && HasMovementFlag(MOVEFLAG_TURN_RIGHT),
                           MOVEFLAG_TURN_LEFT | MOVEFLAG_TURN_RIGHT);

    //! Cannot strafe left and right at the same time
    REMOVE_VIOLATING_FLAGS(HasMovementFlag(MOVEFLAG_STRAFE_LEFT) && HasMovementFlag(MOVEFLAG_STRAFE_RIGHT),
                           MOVEFLAG_STRAFE_LEFT | MOVEFLAG_STRAFE_RIGHT);

    //! Cannot pitch up and down at the same time
    REMOVE_VIOLATING_FLAGS(HasMovementFlag(MOVEFLAG_PITCH_UP) && HasMovementFlag(MOVEFLAG_PITCH_DOWN),
                           MOVEFLAG_PITCH_UP | MOVEFLAG_PITCH_DOWN);

    //! Cannot move forwards and backwards at the same time
    REMOVE_VIOLATING_FLAGS(HasMovementFlag(MOVEFLAG_FORWARD) && HasMovementFlag(MOVEFLAG_BACKWARD),
                           MOVEFLAG_FORWARD | MOVEFLAG_BACKWARD);

#undef REMOVE_VIOLATING_FLAGS
}

void MovementInfo::Write(ByteBuffer &data) const
{
    data << moveFlags;
    data << stime;
    data << pos.x;
    data << pos.y;
    data << pos.z;
    data << pos.o;

    if (HasMovementFlag(MOVEFLAG_ONTRANSPORT))
    {
        data << t_guid;
        data << t_pos.x;
        data << t_pos.y;
        data << t_pos.z;
        data << t_pos.o;
    }
    if (HasMovementFlag(MOVEFLAG_SWIMMING))
        data << s_pitch;

    data << fallTime;

    if (HasMovementFlag(MOVEFLAG_JUMPING))
    {
        data << jump.zspeed;
        data << jump.cosAngle;
        data << jump.sinAngle;
        data << jump.xyspeed;
    }

    if (HasMovementFlag(MOVEFLAG_SPLINE_ELEVATION))
    {
        data << splineElevation;                                     // unknown
    }
}


Object::Object() : m_updateFlag(0)
{
    m_objectTypeId      = TYPEID_OBJECT;
    m_objectType        = TYPEMASK_OBJECT;

    m_uint32Values      = nullptr;
    m_uint32Values_mirror = nullptr;
    m_valuesCount       = 0;

    m_inWorld           = false;
    m_isNewObject       = false;
    m_objectUpdated     = false;
    _deleted            = false;
    _delayedActions     = 0;
}

Object::~Object()
{
    if (IsInWorld())
    {
        ///- Do NOT call RemoveFromWorld here, if the object is a player it will crash
        sLog.outError("Object::~Object (GUID: %u TypeId: %u) deleted but still in world!!", GetGUIDLow(), GetTypeId());
        MANGOS_ASSERT(false);
    }

    if (m_objectUpdated)
    {
        sLog.outError("Object::~Object (GUID: %u TypeId: %u) deleted but still have updated status!!", GetGUIDLow(), GetTypeId());
        MANGOS_ASSERT(false);
    }

    if (m_uint32Values)
    {
        //DEBUG_LOG("Object desctr 1 check (%p)",(void*)this);
        ManTech::MemoryLedger::Remove(ManTech::MemoryKind::UpdateFields, 2 * m_valuesCount * sizeof(uint32));
        delete [] m_uint32Values;
        delete [] m_uint32Values_mirror;
        //DEBUG_LOG("Object desctr 2 check (%p)",(void*)this);
    }
}

void Object::_InitValues()
{
    m_uint32Values = new uint32[ m_valuesCount ];
    memset(m_uint32Values, 0, m_valuesCount * sizeof(uint32));

    m_uint32Values_mirror = new uint32[ m_valuesCount ];
    memset(m_uint32Values_mirror, 0, m_valuesCount * sizeof(uint32));
    ManTech::MemoryLedger::Add(ManTech::MemoryKind::UpdateFields, 2 * m_valuesCount * sizeof(uint32));

    m_objectUpdated = false;
}

void Object::_Create(uint32 guidlow, uint32 entry, HighGuid guidhigh)
{
    if (!m_uint32Values)
        _InitValues();

    ObjectGuid guid = ObjectGuid(guidhigh, entry, guidlow);
    SetGuidValue(OBJECT_FIELD_GUID, guid);
    SetUInt32Value(OBJECT_FIELD_TYPE, m_objectType);
    m_PackGUID.Set(guid);
}

void Object::SetObjectScale(float newScale)
{
    SetFloatValue(OBJECT_FIELD_SCALE_X, newScale);
}

void Object::SendForcedObjectUpdate()
{
    if (!m_inWorld || !m_objectUpdated)
        return;

    UpdateDataMapType update_players;

    BuildUpdateData(update_players);
    RemoveFromClientUpdateList();

    for (auto& itr : update_players)
        itr.second.Send(itr.first->GetSession());
}

void Object::BuildMovementUpdateBlock(UpdateData * data, uint8 flags) const
{
    ByteBuffer& buf = data->AddUpdateBlockAndGetBuffer();

    buf << uint8(UPDATETYPE_MOVEMENT);
    buf << GetObjectGuid();

    BuildMovementUpdate(&buf, flags);
}

void Object::BuildCreateUpdateBlockForPlayer(UpdateData *data, Player *target) const
{
    if (!target)
        return;

    uint8 updatetype = UPDATETYPE_CREATE_OBJECT;
    uint8 updateFlags = m_updateFlag;

    /** lower flag1 **/
    if (target == this)                                     // building packet for yourself
        updateFlags |= UPDATEFLAG_SELF;

    if (IsUnit() && static_cast<Unit const*>(this)->HasUnitState(UNIT_STAT_MELEE_ATTACKING) && static_cast<Unit const*>(this)->GetVictim())
        updateFlags |= UPDATEFLAG_MELEE_ATTACKING;

    if (m_isNewObject)
        updatetype = UPDATETYPE_CREATE_OBJECT2;

    if (isType(TYPEMASK_GAMEOBJECT) && static_cast<GameObject const*>(this)->GetGoType() == GAMEOBJECT_TYPE_TRANSPORT)
        updateFlags |= UPDATEFLAG_TRANSPORT;

    //DEBUG_LOG("BuildCreateUpdate: update-type: %u, object-type: %u got updateFlags: %X", updatetype, m_objectTypeId, updateFlags);

    ByteBuffer& buf = data->AddUpdateBlockAndGetBuffer();
    buf << (uint8)updatetype;
    buf << GetPackGUID();
    buf << uint8(m_objectTypeId);
    
    BuildMovementUpdate(&buf, updateFlags);

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);
    _SetCreateBits(&updateMask, target);
    BuildValuesUpdate(updatetype, &buf, &updateMask, target);
}

void Object::SendCreateUpdateToPlayer(Player* player)
{
    // send create update to player
    UpdateData upd;

    BuildCreateUpdateBlockForPlayer(&upd, player);
    upd.Send(player->GetSession());
}

void WorldObject::DirectSendPublicValueUpdate(uint32 index, uint32 count)
{
    // Do we need an update ?
    bool abort = true;
    for (int i = 0; i < count; i++)
    {
        if (m_uint32Values_mirror[index + i] != m_uint32Values[index + i])
        {
            abort = false;
            m_uint32Values_mirror[index + i] = m_uint32Values[index + i];
        }
    }

    if (abort)
        return;

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);
    for (int i = 0; i < count; i++)
        updateMask.SetBit(index + i);

    DirectSendPublicValueUpdate(updateMask);
}

void WorldObject::DirectSendPublicValueUpdate(std::initializer_list<uint32> indexes)
{
    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);
    for (auto const& index : indexes)
        updateMask.SetBit(index);

    DirectSendPublicValueUpdate(updateMask);
}

void WorldObject::DirectSendPublicValueUpdate(UpdateMask& updateMask)
{
    UpdateData data;

    ByteBuffer& buf = data.AddUpdateBlockAndGetBuffer();
    buf << uint8(UPDATETYPE_VALUES);
    buf << GetPackGUID();

    buf << (uint8)updateMask.GetBlockCount();
    buf.append(updateMask.GetMask(), updateMask.GetLength());

    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (updateMask.GetBit(index))
        {
            buf << uint32(m_uint32Values[index]);
            m_uint32Values_mirror[index] = m_uint32Values[index];
        }
    }

    WorldPacket packet;
    data.BuildPacket(&packet);
    SendObjectMessageToSet(&packet, true);
}

void Object::BuildValuesUpdateBlockForPlayer(UpdateData *data, Player *target) const
{
    ByteBuffer& buf = data->AddUpdateBlockAndGetBuffer();

    buf << uint8(UPDATETYPE_VALUES);
    buf << GetPackGUID();

    UpdateMask updateMask;
    updateMask.SetCount(m_valuesCount);

    _SetUpdateBits(&updateMask, target);
    BuildValuesUpdate(UPDATETYPE_VALUES, &buf, &updateMask, target);
}

void Object::BuildOutOfRangeUpdateBlock(UpdateData * data) const
{
    data->AddOutOfRangeGUID(GetObjectGuid());
}

void Object::SendOutOfRangeUpdateToPlayer(Player* player)
{
    UpdateData data;
    BuildOutOfRangeUpdateBlock(&data);
    WorldPacket packet;
    data.BuildPacket(&packet);
    player->SendDirectMessage(&packet);
}

void Object::DestroyForPlayer(Player *target) const
{
    MANGOS_ASSERT(target);

    WorldPacket data(SMSG_DESTROY_OBJECT, 8);
    data << GetObjectGuid();
    target->GetSession()->SendPacket(&data);
}

void Object::BuildMovementUpdate(ByteBuffer * data, uint8 updateFlags) const
{
    *data << uint8(updateFlags);                            // update flags

    if (updateFlags & UPDATEFLAG_LIVING)
    {
        Unit const* unit = ToUnit();
        ASSERT(unit);
        WorldObject const* wobject = (WorldObject*)this;
        MovementInfo m = wobject->m_movementInfo;
        if (!m.ctime)
        {
            m.stime = WorldTimer::getMSTime() + 1000;
            m.ChangePosition(wobject->GetPositionX(), wobject->GetPositionY(), wobject->GetPositionZ(), wobject->GetOrientation());
        }
        if (unit->ToCreature())
            m.moveFlags = m.moveFlags & ~MOVEFLAG_ROOT;
        *data << m;

        if (unit)
        {
            *data << float(unit->GetSpeed(MOVE_WALK));
            *data << float(unit->GetSpeed(MOVE_RUN));
            *data << float(unit->GetSpeed(MOVE_RUN_BACK));
            *data << float(unit->GetSpeed(MOVE_SWIM));
            *data << float(unit->GetSpeed(MOVE_SWIM_BACK));
            *data << float(unit->GetSpeed(MOVE_TURN_RATE));
            // Send current movement informations
            if (unit->m_movementInfo.moveFlags & MOVEFLAG_SPLINE_ENABLED)
                Movement::PacketBuilder::WriteCreate(*(unit->movespline), *data);
        }
        else
            for (int i = 0; i < MAX_MOVE_TYPE; ++i)
                *data << float(1.0f);
    }
    else
    {
        if (updateFlags & UPDATEFLAG_HAS_POSITION)                     // 0x40
        {
            // 0x02
            if (updateFlags & UPDATEFLAG_TRANSPORT)
            {
                GameObject const* go = static_cast<GameObject const*>(this);
                *data << float(go->GetStationaryX());
                *data << float(go->GetStationaryY());
                *data << float(go->GetStationaryZ());
                *data << float(go->GetStationaryO());
            }
            else
            {
                WorldObject const* wo = static_cast<WorldObject const*>(this);
                *data << float(wo->GetPositionX());
                *data << float(wo->GetPositionY());
                *data << float(wo->GetPositionZ());
                *data << float(wo->GetOrientation());
            }
        }
    }

    if (updateFlags & UPDATEFLAG_HIGHGUID)
    {
        // unk uint32.
        *data << uint32(0);
    }

    if (updateFlags & UPDATEFLAG_ALL)                       // 0x10
    {
        // unk uint32.
        *data << uint32(1);
    }

    if (updateFlags & UPDATEFLAG_MELEE_ATTACKING)
    {
        if (Unit const* me = ToUnit())
        {
            if (Unit const* victim = me->GetVictim())
                * data << victim->GetPackGUID();
            else
                *data << uint8(0); // Empty pack guid
        }
        else
            *data << uint8(0); // Empty pack guid
    }
    // 0x2
    if (updateFlags & UPDATEFLAG_TRANSPORT)
    {
        // transport progress or mstime.
        GameObject const* go = ToGameObject();
        /** @TODO Use IsTransport() to also handle type 11 (TRANSPORT)
            Currently grid objects are not updated if there are no nearby players,
            this causes clients to receive different PathProgress
            resulting in players seeing the object in a different position
        */
        if (go && go->ToTransport())
            *data << uint32(go->ToTransport()->GetPathProgress());
        else
            *data << uint32(WorldTimer::getMSTime());           // ms time
    }
}

void Object::BuildValuesUpdate(uint8 updatetype, ByteBuffer * data, UpdateMask *updateMask, Player *target) const
{
    if (!target)
        return;
    
    bool const ShowHealthValues = sWorld.getConfig(CONFIG_BOOL_OBJECT_HEALTH_VALUE_SHOW);

    bool IsActivateToQuest = false;

    if (updatetype == UPDATETYPE_CREATE_OBJECT || updatetype == UPDATETYPE_CREATE_OBJECT2)
    {
        if (isType(TYPEMASK_GAMEOBJECT) && !((GameObject*)this)->IsTransport())
        {
            if (((GameObject*)this)->ActivateToQuest(target) || target->IsGameMaster())
                IsActivateToQuest = true;

            updateMask->SetBit(GAMEOBJECT_DYN_FLAGS);
        }
        if (target->HasOption(PLAYER_VIDEO_MODE) && isType(TYPEMASK_UNIT))
            updateMask->SetBit(UNIT_FIELD_FLAGS);
    }
    else                                                    // case UPDATETYPE_VALUES
    {
        if (isType(TYPEMASK_GAMEOBJECT) && !((GameObject*)this)->IsTransport())
        {
            if (((GameObject*)this)->ActivateToQuest(target) || target->IsGameMaster())
                IsActivateToQuest = true;

            updateMask->SetBit(GAMEOBJECT_DYN_FLAGS);
            updateMask->SetBit(GAMEOBJECT_ANIMPROGRESS);
        }
    }
    if (isType(TYPEMASK_GAMEOBJECT))
    {
        std::unique_lock<std::mutex> lock(target->m_visibleGobjsQuestAct_lock);
        target->m_visibleGobjQuestActivated[GetObjectGuid()] = IsActivateToQuest;
    }

    MANGOS_ASSERT(updateMask && updateMask->GetCount() == m_valuesCount);

    *data << (uint8)updateMask->GetBlockCount();
    data->append(updateMask->GetMask(), updateMask->GetLength());

    // 2 specialized loops for speed optimization in non-unit case
    if (isType(TYPEMASK_UNIT))                              // unit (creature/player) case
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                if (index == UNIT_NPC_FLAGS)
                {
                    uint32 appendValue = m_uint32Values[index];

                    if (GetTypeId() == TYPEID_UNIT)
                    {
                        if (appendValue & UNIT_NPC_FLAG_TRAINER)
                        {
                            if (!((Creature*)this)->IsTrainerOf(target, false))
                                appendValue &= ~UNIT_NPC_FLAG_TRAINER;
                        }

                        if (appendValue & UNIT_NPC_FLAG_STABLEMASTER)
                        {
                            if (target->GetClass() != CLASS_HUNTER)
                                appendValue &= ~UNIT_NPC_FLAG_STABLEMASTER;
                        }

                        if (appendValue & UNIT_NPC_FLAG_FLIGHTMASTER)
                        {
                            QuestRelationsMapBounds bounds = sObjectMgr.GetCreatureQuestRelationsMapBounds(((Creature*)this)->GetEntry());
                            for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
                            {
                                Quest const* pQuest = sObjectMgr.GetQuestTemplate(itr->second);
                                if (target->CanSeeStartQuest(pQuest))
                                {
                                    appendValue &= ~UNIT_NPC_FLAG_FLIGHTMASTER;
                                    break;
                                }
                            }

                            if (appendValue & UNIT_NPC_FLAG_FLIGHTMASTER)
                            {
                                bounds = sObjectMgr.GetCreatureQuestInvolvedRelationsMapBounds(((Creature*)this)->GetEntry());
                                for (QuestRelationsMap::const_iterator itr = bounds.first; itr != bounds.second; ++itr)
                                {
                                    Quest const* pQuest = sObjectMgr.GetQuestTemplate(itr->second);
                                    if (target->CanRewardQuest(pQuest, false))
                                    {
                                        appendValue &= ~UNIT_NPC_FLAG_FLIGHTMASTER;
                                        break;
                                    }
                                }
                            }
                        }

                        if (appendValue & UNIT_NPC_FLAG_ITEMRESTORE)
                        {
                            appendValue &= ~UNIT_NPC_FLAG_ITEMRESTORE;
                            appendValue |= UNIT_NPC_FLAG_VENDOR;
                        }
                    }

                    *data << uint32(appendValue);
                }

                else if (index == UNIT_FIELD_DISPLAYID)
                {
                    if (GetTypeId() == TYPEID_PLAYER && ToPlayer()->hasIllusion && target->hasIllusionsDisabled)
                    {
                        *data << ToPlayer()->GetNativeDisplayId();
                    }
                    else
                        *data << m_uint32Values[UNIT_FIELD_DISPLAYID];
                }

                else if (index == OBJECT_FIELD_SCALE_X)
                {
                    //limit scale to 2.0f if none are GM
                    if (GetTypeId() == TYPEID_PLAYER && (!ToPlayer()->IsGameMaster() && !target->IsGameMaster()) && m_floatValues[index] > 2.0f)
                        *data << 2.0f;
                    else if (GetTypeId() == TYPEID_PLAYER && (!ToPlayer()->IsGameMaster() && !target->IsGameMaster()) && m_floatValues[index] < 0.5f)
                        *data << 0.5f;
                    else
                        *data << m_uint32Values[index];
                }
                // FIXME: Some values at server stored in float format but must be sent to client in uint32 format
                else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                {
                    // convert from float to uint32 and send
                    *data << uint32(m_floatValues[index] < 0 ? 0 : m_floatValues[index]);
                }

                // there are some float values which may be negative or can't get negative due to other checks
                else if ((index >= PLAYER_FIELD_NEGSTAT0    && index <= PLAYER_FIELD_NEGSTAT4) ||
                         (index >= PLAYER_FIELD_RES_BUFF_MODS_POSITIVE  && index <= (PLAYER_FIELD_RES_BUFF_MODS_POSITIVE + 6)) ||
                         (index >= PLAYER_FIELD_RES_BUFF_MODS_NEGATIVE  && index <= (PLAYER_FIELD_RES_BUFF_MODS_NEGATIVE + 6)) ||
                         (index >= PLAYER_FIELD_POSSTAT0    && index <= PLAYER_FIELD_POSSTAT4))
                    *data << uint32(m_floatValues[index]);
                // Video maker - hide unit name, etc ...
                else if (index == UNIT_FIELD_FLAGS && target->HasOption(PLAYER_VIDEO_MODE) && target != this)
                    *data << (m_uint32Values[index] | UNIT_FLAG_NOT_SELECTABLE);
                // Gamemasters should be always able to select units and view auras
                else if (index == UNIT_FIELD_FLAGS && target->IsGameMaster())
                    *data << ((m_uint32Values[index] | UNIT_FLAG_AURAS_VISIBLE) & ~UNIT_FLAG_NOT_SELECTABLE);
                // hide lootable animation for unallowed players
                else if (index == UNIT_DYNAMIC_FLAGS)
                {
                    uint32 dynamicFlags = m_uint32Values[index];
                    if (HasFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT))
                        if (Unit const * unit = ToUnit())
                        {
                            Unit::AuraList auras = unit->GetAurasByType(SPELL_AURA_MOD_STALKED);
                            if (std::find_if(auras.begin(), auras.end(),[target](Aura *a){
                                return target->GetObjectGuid() == a->GetCasterGuid();
                            }) == auras.end())
                                dynamicFlags &= ~UNIT_DYNFLAG_TRACK_UNIT;
                        }
                    if (Creature const* creature = ToCreature())
                    {
                        if (creature->HasLootRecipient())
                        {
                            if (creature->IsTappedBy(target))
                                dynamicFlags |= (UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER);
                            else
                            {
                                dynamicFlags |= UNIT_DYNFLAG_TAPPED;
                                dynamicFlags &= ~UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                            }
                        }
                        else
                        {
                            dynamicFlags &= ~UNIT_DYNFLAG_TAPPED;
                            dynamicFlags &= ~UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                        }

                        if (!target->IsAllowedToLoot(creature))
                            dynamicFlags &= ~UNIT_DYNFLAG_LOOTABLE;
                    }
                    *data << dynamicFlags;
                }
                // RAID ally-horde - Faction
                else if (index == UNIT_FIELD_FACTIONTEMPLATE)
                {
                    Unit const* owner = ((Unit*)this)->GetCharmerOrOwner();
                    if (!owner)
                        owner = ToPlayer();
                    bool forceFriendly = false;
                    if (owner && owner->IsPlayer())
                    {
                        FactionTemplateEntry const *ft1, *ft2;
                        ft1 = owner->GetFactionTemplateEntry();
                        ft2 = target->GetFactionTemplateEntry();
                        if (ft1 && ft2 && !ft1->IsFriendlyTo(*ft2) && static_cast<Player const*>(owner)->IsInSameRaidWith(target))
                            if (static_cast<Player const*>(owner)->IsInInterFactionMode() && target->IsInInterFactionMode())
                                forceFriendly = true;
                    }
                    uint32 faction = m_uint32Values[index];
                    if (forceFriendly)
                        faction = target->GetFactionTemplateId();

                    *data << uint32(faction);
                }
                // RAID ally-horde : pas de flag FFA
                else if (index == PLAYER_FLAGS && (m_uint32Values[index] & PLAYER_FLAGS_FFA_PVP))
                {
                    Player* owner = ((Unit*)this)->GetCharmerOrOwnerPlayerOrPlayerItself();
                    if (owner && owner != target && owner->IsInSameRaidWith(target))
                        *data << uint32(m_uint32Values[index] & ~PLAYER_FLAGS_FFA_PVP);
                    else
                        *data << uint32(m_uint32Values[index]);
                }
                // Hide real health value. Send a percent instead. See ShowHealthValues option in mangosd.conf
                else if (!ShowHealthValues && (index == UNIT_FIELD_HEALTH || index == UNIT_FIELD_MAXHEALTH))
                {
                    if (target->CanSeeHealthOf((Unit*)this))
                        *data << m_uint32Values[index];
                    else // Hide
                    {
                        if (index == UNIT_FIELD_MAXHEALTH)
                            *data << uint32(100);
                        else
                        {
                            uint32 pct = 0;
                            if (m_uint32Values[UNIT_FIELD_HEALTH])
                            {
                                pct = uint32((m_uint32Values[UNIT_FIELD_HEALTH] * 100.0f) / m_uint32Values[UNIT_FIELD_MAXHEALTH]);
                                if (pct > 100)
                                    pct = 100;
                                if (!pct)
                                    pct = 1;
                            }
                            *data << pct;
                        }
                    }
                }
                else if (target == this && (index == PLAYER_TRACK_CREATURES || index == PLAYER_TRACK_RESOURCES))
                {
                    //if (WardenInterface* base = target->GetSession()->GetWarden())
                        //base->TrackingUpdateSent(index, m_uint32Values[index]);
                    *data << m_uint32Values[index];
                }
                // This is done to make creatures face the target they are casting on.
                else if (index == UNIT_FIELD_TARGET)
                {
                    if (Creature const* pCreature = ToCreature())
                    {
                        if (pCreature->m_castingTargetGuid)
                        {
                            *data << *((uint32*)&pCreature->m_castingTargetGuid);
                            continue;
                        }
                    }
                    *data << m_uint32Values[index];     
                }
                else if (index == UNIT_FIELD_TARGET+1)
                {
                    if (Creature const* pCreature = ToCreature())
                    {
                        if (pCreature->m_castingTargetGuid)
                        {
                            *data << *(((uint32*)&pCreature->m_castingTargetGuid) + 1);
                            continue;
                        }
                    }
                    *data << m_uint32Values[index];
                }
                else if (index == UNIT_MOD_CAST_SPEED)
                {
                    if (m_floatValues[index] < 0.001f)
                        *data << float(0.0f);
                    else
                        *data << m_floatValues[index];
                }
                else
                {
                    // send in current format (float as float, uint32 as uint32)
                    *data << m_uint32Values[index];
                }
            }
        }
    }
    else if (isType(TYPEMASK_GAMEOBJECT))                   // gameobject case
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                // send in current format (float as float, uint32 as uint32)
                if (index == GAMEOBJECT_DYN_FLAGS)
                {
                    if (IsActivateToQuest)
                    {
                        switch (((GameObject*)this)->GetGoType())
                        {
                            case GAMEOBJECT_TYPE_QUESTGIVER:
                            case GAMEOBJECT_TYPE_CHEST:
                            case GAMEOBJECT_TYPE_GENERIC:
                            case GAMEOBJECT_TYPE_SPELL_FOCUS:
                            case GAMEOBJECT_TYPE_GOOBER:
                                *data << uint16(GO_DYNFLAG_LO_ACTIVATE);
                                *data << uint16(0);
                                break;
                            default:
                                *data << uint32(0);         // unknown, not happen.
                                break;
                        }
                    }
                    else
                        *data << uint32(0);                 // disable quest object
                }
                else
                    *data << m_uint32Values[index];         // other cases
            }
        }
    }
    else if (isType(TYPEMASK_ITEM))
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                if (index == ITEM_FIELD_FLAGS)
                {
                    uint32 dynFlags = m_uint32Values[ITEM_FIELD_FLAGS];
                    if ((dynFlags & ITEM_DYNFLAG_BINDED) && static_cast<Item const*>(this)->CanBeTradedEvenIfSoulBound())
                        dynFlags &= ~ITEM_DYNFLAG_BINDED;
                    *data << dynFlags;
                }
                else
                    // send in current format (float as float, uint32 as uint32)
                    *data << m_uint32Values[index];
            }
        }
    }
    else                                                    // other objects case (no special index checks)
    {
        for (uint16 index = 0; index < m_valuesCount; ++index)
        {
            if (updateMask->GetBit(index))
            {
                if (index == CORPSE_FIELD_DYNAMIC_FLAGS)
                {
                    uint32 dynFlags = m_uint32Values[CORPSE_FIELD_DYNAMIC_FLAGS];
                    if (Corpse const* corpse = ToCorpse())
                    {
                        const Loot* loot = &corpse->loot;
                        if (loot->isLooted()) // nothing to loot or everything looted.
                            dynFlags &= ~CORPSE_DYNFLAG_LOOTABLE;
                        if (dynFlags & CORPSE_DYNFLAG_LOOTABLE)
                            if (corpse->IsFriendlyTo(target))
                                dynFlags &= ~CORPSE_DYNFLAG_LOOTABLE;
                    }
                    *data << dynFlags;
                }
                else
                    // send in current format (float as float, uint32 as uint32)
                    *data << m_uint32Values[index];
            }
        }
    }
}

void Object::ClearUpdateMask(bool remove)
{
    if (m_uint32Values)
        memcpy(m_uint32Values_mirror, m_uint32Values, sizeof(uint32) * m_valuesCount);

    if (m_objectUpdated)
    {
        if (remove)
            RemoveFromClientUpdateList();
        m_objectUpdated = false;
    }
    _delayedActions &= ~OBJECT_DELAYED_MARK_CLIENT_UPDATE;
}

bool Object::LoadValues(const char* data)
{
    if (!m_uint32Values) _InitValues();

    Tokens tokens = StrSplit(data, " ");

    if (tokens.size() != m_valuesCount)
        return false;

    Tokens::iterator iter;
    int index;
    for (iter = tokens.begin(), index = 0; index < m_valuesCount; ++iter, ++index)
        m_uint32Values[index] = atol((*iter).c_str());

    return true;
}

void Object::_LoadIntoDataField(std::string const& data, uint32 startOffset, uint32 count)
{
    if (data.empty())
        return;

    Tokenizer tokens(data, ' ', count);

    if (tokens.size() != count)
        return;

    for (uint32 index = 0; index < count; ++index)
    {
        m_uint32Values[startOffset + index] = strtoul(tokens[index], nullptr, 10);
        m_uint32Values_mirror[startOffset + index] = m_uint32Values[startOffset + index] + 1;
    }
}

void Object::_SetUpdateBits(UpdateMask *updateMask, Player* /*target*/) const
{
    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (m_uint32Values_mirror[index] != m_uint32Values[index])
            updateMask->SetBit(index);
    }
}

void Object::_SetCreateBits(UpdateMask *updateMask, Player* /*target*/) const
{
    for (uint16 index = 0; index < m_valuesCount; ++index)
    {
        if (GetUInt32Value(index) != 0)
            updateMask->SetBit(index);
    }
}

void Object::SetInt32Value(uint16 index, int32 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_int32Values[ index ] != value)
    {
        m_int32Values[ index ] = value;
        MarkForClientUpdate();
    }
}

void Object::SetUInt32Value(uint16 index, uint32 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_uint32Values[ index ] != value)
    {
        m_uint32Values[ index ] = value;
        MarkForClientUpdate();
    }
}

void Object::SetUInt64Value(uint16 index, const uint64 &value)
{
    MANGOS_ASSERT(index + 1 < m_valuesCount || PrintIndexError(index, true));
    if (*((uint64*) & (m_uint32Values[ index ])) != value)
    {
        uint32 first = m_uint32Values[index] = *((uint32*)&value);
        uint32 second = m_uint32Values[index + 1] = *(((uint32*)&value) + 1);

        // Force an update at both mirrored values, even if only one index was changed
        // If we don't update the second index, it may become perpetually stuck and
        // lead to weird client behaviour such as not displaying a target (only the
        // first part is networked). This is typically only an issue for units which
        // have these values set at create time, as the client will ignore unpacked
        // 64bit values that arent fully networked in the create stage, yet accept
        // them in the update stage (presumably it defaults to 0 for both bytes if
        // not present, making it "OK" if we only send one in the future).
        // Note that this behaviour is inconsistent as well, sometimes it works
        // with only one part whereas other times it does not. It appears to be
        // related to the number of (player) units in the vicinity.
        // The first update will correct any malformed 64bit data.
        m_uint32Values_mirror[index] = first + 1;
        m_uint32Values_mirror[index + 1] = second + 1;
        MarkForClientUpdate();
    }
}

void Object::SetFloatValue(uint16 index, float value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (m_floatValues[ index ] != value)
    {
        m_floatValues[ index ] = value;
        MarkForClientUpdate();
    }
}

void Object::SetByteValue(uint16 index, uint8 offset, uint8 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::SetByteValue: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[ index ] >> (offset * 8)) != value)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(0xFF) << (offset * 8));
        m_uint32Values[ index ] |= uint32(uint32(value) << (offset * 8));
        MarkForClientUpdate();
    }
}

void Object::SetUInt16Value(uint16 index, uint8 offset, uint16 value)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 2)
    {
        sLog.outError("Object::SetUInt16Value: wrong offset %u", offset);
        return;
    }

    if (uint16(m_uint32Values[ index ] >> (offset * 16)) != value)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(0xFFFF) << (offset * 16));
        m_uint32Values[ index ] |= uint32(uint32(value) << (offset * 16));
        MarkForClientUpdate();
    }
}

void Object::SetStatFloatValue(uint16 index, float value)
{
    if (value < 0)
        value = 0.0f;

    SetFloatValue(index, value);
}

void Object::SetStatInt32Value(uint16 index, int32 value)
{
    if (value < 0)
        value = 0;

    SetUInt32Value(index, uint32(value));
}

void Object::ApplyModUInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetUInt32Value(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetUInt32Value(index, cur);
}

void Object::ApplyModInt32Value(uint16 index, int32 val, bool apply)
{
    int32 cur = GetInt32Value(index);
    cur += (apply ? val : -val);
    SetInt32Value(index, cur);
}

void Object::ApplyModSignedFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    SetFloatValue(index, cur);
}

void Object::ApplyModPositiveFloatValue(uint16 index, float  val, bool apply)
{
    float cur = GetFloatValue(index);
    cur += (apply ? val : -val);
    if (cur < 0)
        cur = 0;
    SetFloatValue(index, cur);
}

void Object::SetFlag(uint16 index, uint32 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[ index ];
    uint32 newval = oldval | newFlag;

    if (oldval != newval)
    {
        m_uint32Values[ index ] = newval;
        MarkForClientUpdate();
    }
}

void Object::RemoveFlag(uint16 index, uint32 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));
    uint32 oldval = m_uint32Values[ index ];
    uint32 newval = oldval & ~oldFlag;

    if (oldval != newval)
    {
        m_uint32Values[ index ] = newval;
        MarkForClientUpdate();
    }
}

void Object::SetByteFlag(uint16 index, uint8 offset, uint8 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::SetByteFlag: wrong offset %u", offset);
        return;
    }

    if (!(uint8(m_uint32Values[ index ] >> (offset * 8)) & newFlag))
    {
        m_uint32Values[ index ] |= uint32(uint32(newFlag) << (offset * 8));
        MarkForClientUpdate();
    }
}

void Object::RemoveByteFlag(uint16 index, uint8 offset, uint8 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (offset > 4)
    {
        sLog.outError("Object::RemoveByteFlag: wrong offset %u", offset);
        return;
    }

    if (uint8(m_uint32Values[ index ] >> (offset * 8)) & oldFlag)
    {
        m_uint32Values[ index ] &= ~uint32(uint32(oldFlag) << (offset * 8));
        MarkForClientUpdate();
    }
}

void Object::SetShortFlag(uint16 index, bool highpart, uint16 newFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (!(uint16(m_uint32Values[index] >> (highpart ? 16 : 0)) & newFlag))
    {
        m_uint32Values[index] |= uint32(uint32(newFlag) << (highpart ? 16 : 0));
        MarkForClientUpdate();
    }
}

void Object::RemoveShortFlag(uint16 index, bool highpart, uint16 oldFlag)
{
    MANGOS_ASSERT(index < m_valuesCount || PrintIndexError(index, true));

    if (uint16(m_uint32Values[index] >> (highpart ? 16 : 0)) & oldFlag)
    {
        m_uint32Values[index] &= ~uint32(uint32(oldFlag) << (highpart ? 16 : 0));
        MarkForClientUpdate();
    }
}

bool Object::PrintIndexError(uint32 index, bool set) const
{
    sLog.outInfo("%s nonexistent value field: %u (count: %u) for object typeid: %u type mask: %u",
                    (set ? "set value to" : "get value from"), index, m_valuesCount, GetTypeId(), m_objectType);

    // ASSERT must fail after function call
    return false;
}

void Object::BuildUpdateDataForPlayer(Player* pl, UpdateDataMapType& update_players)
{
    UpdateDataMapType::iterator iter = update_players.find(pl);

    if (iter == update_players.end())
    {
        std::pair<UpdateDataMapType::iterator, bool> p = update_players.insert(UpdateDataMapType::value_type(pl, UpdateData()));
        MANGOS_ASSERT(p.second);
        iter = p.first;
    }

    BuildValuesUpdateBlockForPlayer(&iter->second, iter->first);
}

void Object::AddToClientUpdateList()
{
    sLog.outError("Unexpected call of Object::AddToClientUpdateList for object (TypeId: %u Update fields: %u)", GetTypeId(), m_valuesCount);
    MANGOS_ASSERT(false);
}

void Object::RemoveFromClientUpdateList()
{
    sLog.outError("Unexpected call of Object::RemoveFromClientUpdateList for object (TypeId: %u Update fields: %u)", GetTypeId(), m_valuesCount);
    MANGOS_ASSERT(false);
}

void Object::BuildUpdateData(UpdateDataMapType& /*update_players */)
{
    sLog.outError("Unexpected call of Object::BuildUpdateData for object (TypeId: %u Update fields: %u)", GetTypeId(), m_valuesCount);
    MANGOS_ASSERT(false);
}

void Object::MarkForClientUpdate()
{
    if (!m_inWorld)
        return;
    if (GetTypeId() == TYPEID_ITEM || GetTypeId() == TYPEID_CONTAINER)
    {
        if (m_inWorld)
        {
            if (!m_objectUpdated)
            {
                AddToClientUpdateList();
                m_objectUpdated = true;
            }
        }
    }
    else
        AddDelayedAction(OBJECT_DELAYED_MARK_CLIENT_UPDATE);
}

void Object::ExecuteDelayedActions()
{
    if (_delayedActions & OBJECT_DELAYED_MARK_CLIENT_UPDATE)
    {
        if (m_inWorld && !_deleted)
        {
            if (!m_objectUpdated)
            {
                AddToClientUpdateList();
                m_objectUpdated = true;
            }
        }
        _delayedActions &= ~OBJECT_DELAYED_MARK_CLIENT_UPDATE;
    }
    if (_delayedActions & OBJECT_DELAYED_ADD_TO_REMOVE_LIST)
    {
        if (!IsDeleted() && IsInWorld())
            ((WorldObject*)this)->AddObjectToRemoveList();
        _delayedActions &= ~OBJECT_DELAYED_ADD_TO_REMOVE_LIST;
    }
}

bool WorldObject::IsWithinLootXPDist(WorldObject const * objToLoot) const
{
    if (objToLoot && IsInMap(objToLoot) && objToLoot->GetMap()->IsRaid())
        return true;

    // Bosses have increased loot distance.
    float lootDistance = sWorld.getConfig(CONFIG_FLOAT_GROUP_XP_DISTANCE);
    if (objToLoot->IsCreature() && (static_cast<Creature const*>(objToLoot)->GetCreatureInfo()->rank == CREATURE_ELITE_WORLDBOSS))
        lootDistance += 150.0f;

    if (objToLoot->IsCreature())
        objToLoot->ToCreature()->CheckLootDistance(lootDistance);

    return objToLoot && IsInMap(objToLoot) && _IsWithinDist(objToLoot, lootDistance, false);
}

float WorldObject::GetVisibilityModifier() const
{
    // Only active objects can have increased visibility, since they are updated outside
    // of typical draw distance. Other units are not, so having a non-standard visibility
    // on them equals B.A.D.
    if (!m_isActiveObject)
        return 0.0f;

    return m_visibilityModifier;
}

void WorldObject::SetVisibilityModifier(float f)
{
    m_visibilityModifier = f;
}

WorldObject::WorldObject() :
    m_isActiveObject(false), m_visibilityModifier(DEFAULT_VISIBILITY_MODIFIER), m_currMap(nullptr), m_mapId(0), m_InstanceId(0), m_summonLimitAlert(0)
{
    // Phasing
    worldMask = WORLD_DEFAULT_OBJECT;
    m_zoneScript = nullptr;
    m_transport = nullptr;
    m_movementInfo.stime = WorldTimer::getMSTime();
}

void WorldObject::CleanupsBeforeDelete()
{
    RemoveFromWorld();

    if (Transport* transport = GetTransport())
        transport->RemovePassenger(this);
}

void WorldObject::_Create(uint32 guidlow, HighGuid guidhigh)
{
    Object::_Create(guidlow, 0, guidhigh);
}

void WorldObject::Relocate(float x, float y, float z, float orientation)
{
    ASSERT(MaNGOS::IsValidMapCoord(x, y, z));

    m_position.x = x;
    m_position.y = y;
    m_position.z = z;
    m_position.o = orientation;

    m_movementInfo.ChangePosition(x, y, z, orientation);
    m_movementInfo.UpdateTime(WorldTimer::getMSTime());
    /*if (Transport* t = GetTransport())
    {
        t->CalculatePassengerOffset(x, y, z);
        m_movementInfo.t_pos.x = x;
        m_movementInfo.t_pos.y = y;
        m_movementInfo.t_pos.z = z;
    }*/
}

void WorldObject::Relocate(float x, float y, float z)
{
    Relocate(x, y, z, GetOrientation());
}

void WorldObject::SetOrientation(float orientation)
{
    m_position.o = orientation;

    if (Unit* unit = ToUnit())
        unit->m_movementInfo.ChangeOrientation(orientation);
}

float WorldObject::GetVisibilityDistance() const
{
    if (sWorld.getConfig(CONFIG_BOOL_ENABLE_DYNAMIC_VISIBILITIES))
    {
        auto optVis = sDynamicVisMgr.GetDynamicVisibility(GetCachedAreaId());
        if (optVis)
            return optVis.value();
    }
    return GetMap()->GetVisibilityDistance();
}

float WorldObject::GetGridActivationDistance() const
{
    if (sWorld.getConfig(CONFIG_BOOL_ENABLE_DYNAMIC_VISIBILITIES))
    {
        auto optVis = sDynamicVisMgr.GetDynamicVisibility(GetCachedAreaId());
        if (optVis)
            return optVis.value();
    }
    return GetMap()->GetGridActivationDistance();
}

uint32 WorldObject::GetZoneId() const
{
    return m_currMap ? GetTerrain()->GetZoneId(m_position.x, m_position.y, m_position.z) : 0;
}

uint32 WorldObject::GetAreaId() const
{
    return m_currMap ? GetTerrain()->GetAreaId(m_position.x, m_position.y, m_position.z) : 0;
}

void WorldObject::GetZoneAndAreaId(uint32& zoneid, uint32& areaid) const
{
    if (m_currMap)
        GetTerrain()->GetZoneAndAreaId(zoneid, areaid, m_position.x, m_position.y, m_position.z);
}

InstanceData* WorldObject::GetInstanceData() const
{
    return GetMap()->GetInstanceData();
}

float WorldObject::GetSizeFactorForDistance(WorldObject const* obj, SizeFactor distcalc) const
{
    float sizefactor;
    switch (distcalc)
    {
        case SizeFactor::BoundingRadius:
        {
            sizefactor = GetObjectBoundingRadius();
            if (obj)
                sizefactor += obj->GetObjectBoundingRadius();
            break;
        }
        case SizeFactor::CombatReach:
        {
            sizefactor = GetCombatReach();
            if (obj)
                sizefactor += obj->GetCombatReach();
            break;
        }
        case SizeFactor::CombatReachWithMelee:
        {
            sizefactor = std::max(1.5f, GetCombatReach());
            if (obj)
                sizefactor += std::max(1.5f, obj->GetCombatReach());
            break;
        }
        default:
        {
            sizefactor = 0.0f;
            break;
        }
    }
    return sizefactor;
}

float WorldObject::GetDistance(const WorldObject* obj, SizeFactor distcalc) const
{
    ASSERT(obj);
    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float dz = GetPositionZ() - obj->GetPositionZ();
    float dist = sqrt((dx * dx) + (dy * dy) + (dz * dz)) - GetSizeFactorForDistance(obj, distcalc);
    return (dist > 0 ? dist : 0);
}

float WorldObject::GetDistance2d(float x, float y, SizeFactor distcalc) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dist = sqrt((dx * dx) + (dy * dy)) - GetSizeFactorForDistance(nullptr, distcalc);
    return (dist > 0 ? dist : 0);
}

float WorldObject::GetDistance(float x, float y, float z, SizeFactor distcalc) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dz = GetPositionZ() - z;
    float dist = sqrt((dx * dx) + (dy * dy) + (dz * dz)) - GetSizeFactorForDistance(nullptr, distcalc);
    return (dist > 0 ? dist : 0);
}

float WorldObject::GetDistanceSqr(float x, float y, float z) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dz = GetPositionZ() - z;
    float sizefactor = 1.0f; //GetObjectSize();
    float dist = dx * dx + dy * dy + dz * dz - sizefactor;
    return (dist > 0 ? dist : 0);
}

float WorldObject::GetDistance2d(WorldObject const* obj, SizeFactor distcalc) const
{
    ASSERT(obj);
    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float dist = sqrt((dx * dx) + (dy * dy)) - GetSizeFactorForDistance(obj, distcalc);
    return (dist > 0 ? dist : 0);
}

float WorldObject::GetDistanceZ(WorldObject const* obj, SizeFactor distcalc) const
{
    ASSERT(obj);
    float dz = fabs(GetPositionZ() - obj->GetPositionZ());
    float dist = dz - GetSizeFactorForDistance(obj, distcalc);
    return (dist > 0 ? dist : 0);
}

bool WorldObject::IsWithinDist3d(float x, float y, float z, float dist2compare, SizeFactor distcalc) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dz = GetPositionZ() - z;
    float distsq = dx * dx + dy * dy + dz * dz;

    float maxdist = dist2compare + GetSizeFactorForDistance(nullptr, distcalc);
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsWithinDist2d(float x, float y, float dist2compare, SizeFactor distcalc) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float distsq = dx * dx + dy * dy;

    float maxdist = dist2compare + GetSizeFactorForDistance(nullptr, distcalc);
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInMap(WorldObject const* obj) const
{
    return IsInWorld() && obj->IsInWorld() && (GetMap() == obj->GetMap());
}

bool WorldObject::_IsWithinDist(WorldObject const* obj, float const dist2compare, const bool is3D, SizeFactor distcalc) const
{
    ASSERT(obj);
    float const dx = GetPositionX() - obj->GetPositionX();
    float const dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx * dx + dy * dy;
    if (is3D)
    {
        float const dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz * dz;
    }

    float const maxdist = dist2compare + GetSizeFactorForDistance(obj, distcalc);
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsWithinLOSInMap(WorldObject const* obj, bool checkDynLos) const
{
    ASSERT(obj);
    if (!IsInMap(obj))
        return false;
    if (IsWithinDist(obj, 0.0f))
        return true;
    float ox, oy, oz;
    obj->GetLosCheckPosition(ox, oy, oz);
    return (IsWithinLOS(ox, oy, oz, checkDynLos, 0.0f));
}

bool WorldObject::IsWithinLOSAtPosition(float ownX, float ownY, float ownZ, float targetX, float targetY, float targetZ, bool checkDynLos, float targetHeight) const
{
    if (IsInWorld())
    {
        float height = IsUnit() ? static_cast<Unit const*>(this)->GetCollisionHeight() : 2.0f;
        return GetMap()->isInLineOfSight(ownX, ownY, ownZ + height, targetX, targetY, targetZ + targetHeight, checkDynLos);
    }

    return true;
}

void WorldObject::GetLosCheckPosition(float& x, float& y, float& z) const
{
    GetPosition(x, y, z);
    z += 1.0f;
}

bool WorldObject::GetDistanceOrder(WorldObject const* obj1, WorldObject const* obj2, bool is3D /* = true */) const
{
    ASSERT(obj1 && obj2);
    float dx1 = GetPositionX() - obj1->GetPositionX();
    float dy1 = GetPositionY() - obj1->GetPositionY();
    float distsq1 = dx1 * dx1 + dy1 * dy1;
    if (is3D)
    {
        float dz1 = GetPositionZ() - obj1->GetPositionZ();
        distsq1 += dz1 * dz1;
    }

    float dx2 = GetPositionX() - obj2->GetPositionX();
    float dy2 = GetPositionY() - obj2->GetPositionY();
    float distsq2 = dx2 * dx2 + dy2 * dy2;
    if (is3D)
    {
        float dz2 = GetPositionZ() - obj2->GetPositionZ();
        distsq2 += dz2 * dz2;
    }

    return distsq1 < distsq2;
}

bool WorldObject::IsInRange(WorldObject const* obj, float minRange, float maxRange, bool is3D /* = true */, SizeFactor distcalc) const
{
    ASSERT(obj);
    float dx = GetPositionX() - obj->GetPositionX();
    float dy = GetPositionY() - obj->GetPositionY();
    float distsq = dx * dx + dy * dy;
    if (is3D)
    {
        float dz = GetPositionZ() - obj->GetPositionZ();
        distsq += dz * dz;
    }

    float sizefactor = GetSizeFactorForDistance(obj, distcalc);

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange2d(float x, float y, float minRange, float maxRange, SizeFactor distcalc) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float distsq = dx * dx + dy * dy;

    float sizefactor = GetSizeFactorForDistance(nullptr, distcalc);

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::IsInRange3d(float x, float y, float z, float minRange, float maxRange, SizeFactor distcalc) const
{
    float dx = GetPositionX() - x;
    float dy = GetPositionY() - y;
    float dz = GetPositionZ() - z;
    float distsq = dx * dx + dy * dy + dz * dz;

    float sizefactor = GetSizeFactorForDistance(nullptr, distcalc);

    // check only for real range
    if (minRange > 0.0f)
    {
        float mindist = minRange + sizefactor;
        if (distsq < mindist * mindist)
            return false;
    }

    float maxdist = maxRange + sizefactor;
    return distsq < maxdist * maxdist;
}

bool WorldObject::CanReachWithMeleeSpellAttack(WorldObject const* pVictim, float flat_mod /*= 0.0f*/) const
{
    if (!pVictim || !pVictim->IsInWorld())
        return false;

    float reach = IsUnit() && pVictim->IsUnit() ?
        static_cast<Unit const*>(this)->GetCombatReach(static_cast<Unit const*>(pVictim), true, flat_mod) : ATTACK_DISTANCE;

    // This check is not related to bounding radius
    float dx = GetPositionX() - pVictim->GetPositionX();
    float dy = GetPositionY() - pVictim->GetPositionY();

    // melee spells ignore Z-axis checks
    return dx * dx + dy * dy < reach * reach;
}

float WorldObject::GetAngle(WorldObject const* obj) const
{
    if (!obj) return 0;
    return GetAngle(obj->GetPositionX(), obj->GetPositionY());
}

// Return angle in range 0..2*pi
float WorldObject::GetAngle(const float x, const float y) const
{
    float dx = x - GetPositionX();
    float dy = y - GetPositionY();

    float ang = atan2(dy, dx);
    ang = (ang >= 0) ? ang : 2 * M_PI_F + ang;
    return ang;
}

bool WorldObject::HasInArc(const float arcangle, const float x, const float y) const
{
    // always have self in arc
    if (x == m_position.x && y == m_position.y)
        return true;

    float arc = arcangle;

    // move arc to range 0.. 2*pi
    while (arc >= 2.0f * M_PI_F)
        arc -=  2.0f * M_PI_F;
    while (arc < 0)
        arc +=  2.0f * M_PI_F;

    float angle = GetAngle(x, y);
    angle -= m_position.o;

    // move angle to range -pi ... +pi
    while (angle > M_PI_F)
        angle -= 2.0f * M_PI_F;
    while (angle < -M_PI_F)
        angle += 2.0f * M_PI_F;

    float lborder =  -1 * (arc / 2.0f);                     // in range -pi..0
    float rborder = (arc / 2.0f);                           // in range 0..pi
    return ((angle >= lborder) && (angle <= rborder));
}

bool WorldObject::HasInArc(WorldObject const* target, float const arcangle, float offset) const
{
    // always have self in arc
    if (target == this)
        return true;

    float arc = arcangle;

    // move arc to range 0.. 2*pi
    arc = MapManager::NormalizeOrientation(arc);

    float angle = GetAngle(target);
    angle -= m_position.o + offset;

    // move angle to range -pi ... +pi
    angle = MapManager::NormalizeOrientation(angle);
    if (angle > M_PI_F)
        angle -= 2.0f * M_PI_F;

    float lborder =  -1 * (arc / 2.0f);                     // in range -pi..0
    float rborder = (arc / 2.0f);                           // in range 0..pi
    return ((angle >= lborder) && (angle <= rborder));
}

bool WorldObject::IsFacingTarget(WorldObject const* target) const
{
    return (GetDistance2dToCenter(target) < NO_FACING_CHECKS_DISTANCE) || HasInArc(target, M_PI_F);
}

bool WorldObject::GetRandomPoint(float x, float y, float z, float distance, float &rand_x, float &rand_y, float &rand_z, bool allowStraightPath) const
{
    if (distance < 0.1f)
    {
        rand_x = x;
        rand_y = y;
        rand_z = z;
        return true;
    }
    Map const* pMap = GetMap();
    Unit const* pUnit = ToUnit();

    // 1st case we can fly => Position in the air, easy.
    if (pUnit && pUnit->CanFly())
    {
        float randAngle1 = rand_norm_f() * 2 * M_PI;
        float randAngle2 = rand_norm_f() * 2 * M_PI;
        float randDist = rand_norm_f() * distance;
        rand_x = x + randDist * cos(randAngle1) * sin(randAngle2);
        rand_y = y + randDist * sin(randAngle2) * sin(randAngle2);
        rand_z = z + randDist * sin(randAngle2);
        // May happen in the border of the map
        if (!MaNGOS::IsValidMapCoord(x, y, z) || !MaNGOS::IsValidMapCoord(rand_x, rand_y, rand_z))
            return false;
        pMap->GetLosHitPosition(x, y, z, rand_x, rand_y, rand_z, -0.5f);
        return true;
    }

    // Get swimming position using just vmaps.
    if (pUnit && pUnit->CanSwim() && pUnit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_USE_SWIM_ANIMATION))
    {
        GridMapLiquidData liquid_status;
        GridMapLiquidStatus res = pMap->GetTerrain()->getLiquidStatus(x, y, z, MAP_ALL_LIQUIDS, &liquid_status);
        if (res & (LIQUID_MAP_UNDER_WATER | LIQUID_MAP_IN_WATER))
        {
            rand_x = x;
            rand_y = y;
            rand_z = z;
            if (pMap->GetSwimRandomPosition(rand_x, rand_y, rand_z, distance, liquid_status, true))
            {
                pMap->GetLosHitPosition(x, y, z, rand_x, rand_y, rand_z, -0.5f);
                return true;
            }
        }
    }

    {
        // Otherwise, we find a position on the ground, or in water, or in lava (not for players)
        uint32 moveAllowed = NAV_GROUND | NAV_WATER;
        if (GetTypeId() != TYPEID_PLAYER)
            moveAllowed |= NAV_MAGMA | NAV_SLIME;
        rand_x = x;
        rand_y = y;
        rand_z = z;
        if (pMap->GetWalkRandomPosition(GetTransport(), rand_x, rand_y, rand_z, distance, allowStraightPath, moveAllowed))
        {
            // Giant type creatures walk underwater
            if ((pUnit && !pUnit->CanSwim()) ||
                (IsCreature() && !pUnit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_USE_SWIM_ANIMATION)))
                return true;

            // The position returned by the pathfinding is at the bottom of the water. We're randomizing it a bit...
            float ground = 0.0f;
            float waterSurface = GetTerrain()->GetWaterLevel(x, y, z, &ground);
            if (waterSurface == VMAP_INVALID_HEIGHT_VALUE)
                return true;
            if (ground > waterSurface) // Possible ?
                return true;
            rand_z += rand_norm_f() * distance / 2.0f;
            if (rand_z < ground)
                rand_z = ground;
            // Flying case checked before that, so we stay UNDER water.
            if (rand_z > waterSurface)
                rand_z = waterSurface;
            return true;
        }
        rand_x = x;
        rand_y = y;
        rand_z = z;
        return false;
    }
}

/**
 * \brief Gets a point behind the object.
 * \param location The location to fill with the point behind the target.
 * \param distance The distance away from the object.
 */
void WorldObject::GetPointBehindObject(WorldLocation& location, float distance) const
{
    // Get our current location.
    WorldLocation myLocation, behindLocation;
    GetPosition(myLocation);
    GetPosition(behindLocation);

    // Calculate an XY point behind us.
    behindLocation.x -= distance * cos(myLocation.o);
    behindLocation.y -= distance * sin(myLocation.o);

    // Snap the Z axis to the correct level.
    behindLocation.z = GetTerrain()->GetWaterOrGroundLevel(
        behindLocation.x,
        behindLocation.y,
        behindLocation.z);

    // Get the LOS hit point between our position and the generated point.
    GetMap()->GetLosHitPosition(
        myLocation.x,
        myLocation.y,
        myLocation.z,
        behindLocation.x,
        behindLocation.y,
        behindLocation.z,
        -0.5f);

    location.CopyFrom(behindLocation);
}

void WorldObject::UpdateGroundPositionZ(float x, float y, float &z) const
{
    float new_z = GetMap()->GetHeight(x, y, z, true);
    if (new_z > INVALID_HEIGHT)
        z = new_z + 0.05f;                                  // just to be sure that we are not a few pixel under the surface
}

void WorldObject::UpdateAllowedPositionZ(float x, float y, float &z) const
{
    if (GetTransport())
        return;

    switch (GetTypeId())
    {
        case TYPEID_UNIT:
        {
            // non fly unit don't must be in air
            // non swim unit must be at ground (mostly speedup, because it don't must be in water and water level check less fast
            if (!((Creature const*)this)->CanFly())
            {
                bool canSwim = ((Creature const*)this)->CanSwim();
                float ground_z = z;
                float max_z = canSwim
                              ? GetTerrain()->GetWaterOrGroundLevel(x, y, z, &ground_z, !((Unit const*)this)->HasAuraType(SPELL_AURA_WATER_WALK))
                              : ((ground_z = GetMap()->GetHeight(x, y, z, true)));
                if (max_z > INVALID_HEIGHT)
                {
                    if (z > max_z)
                        z = max_z;
                    else if (z < ground_z)
                        z = ground_z;
                }
            }
            else
            {
                float ground_z = GetMap()->GetHeight(x, y, z, true);
                if (z < ground_z)
                    z = ground_z;
            }
            break;
        }
        case TYPEID_PLAYER:
        {
            // for server controlled moves playr work same as creature (but it can always swim)
            {
                float ground_z = z;
                float max_z = GetTerrain()->GetWaterOrGroundLevel(x, y, z, &ground_z, !((Unit const*)this)->HasAuraType(SPELL_AURA_WATER_WALK));
                if (max_z > INVALID_HEIGHT)
                {
                    if (z > max_z)
                        z = max_z;
                    else if (z < ground_z)
                        z = ground_z;
                }
            }
            break;
        }
        default:
        {
            float ground_z = GetMap()->GetHeight(x, y, z, true);
            if (ground_z > INVALID_HEIGHT)
                z = ground_z;
            break;
        }
    }
}

bool WorldObject::IsPositionValid() const
{
    return MaNGOS::IsValidMapCoord(m_position.x, m_position.y, m_position.z, m_position.o);
}

void WorldObject::SendMessageToSet(WorldPacket *data, bool /*bToSelf*/) const
{
    //if object is in world, map for it already created!
    if (IsInWorld())
        GetMap()->MessageBroadcast(this, data);
}

struct ObjectViewersDeliverer
{
    WorldPacket* i_message;
    WorldObject const* i_sender;
    WorldObject const* i_except;
    explicit ObjectViewersDeliverer(WorldObject const* sender, WorldPacket *msg, WorldObject const* except) : i_message(msg), i_sender(sender), i_except(except) {}
    void Visit(CameraMapType &m)
    {
        for (const auto& iter : m)
            if (Player* player = iter.getSource()->GetOwner())
                if (player != i_except && player != i_sender)
                    if (player->IsInVisibleList_Unsafe(i_sender))
                        player->GetSession()->SendPacket(i_message);
    }
    template<class SKIP> void Visit(GridRefManager<SKIP> &) {}
};

void WorldObject::SendObjectMessageToSet(WorldPacket *data, bool self, WorldObject const* except) const
{
    if (self && this != except)
        if (Player const* me = ToPlayer())
            me->GetSession()->SendPacket(data);

    if (!IsInWorld())
        return;

    CellPair p = MaNGOS::ComputeCellPair(GetPositionX(), GetPositionY());

    if (p.x_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP || p.y_coord >= TOTAL_NUMBER_OF_CELLS_PER_MAP)
        return;

    Cell cell(p);
    cell.SetNoCreate();

    if (!GetMap()->IsLoaded(GetPositionX(), GetPositionY()))
        return;

    ObjectViewersDeliverer post_man(this, data, except);
    TypeContainerVisitor<ObjectViewersDeliverer, WorldTypeMapContainer> message(post_man);
    cell.Visit(p, message, *GetMap(), *this, std::max(GetVisibilityDistance(), GetVisibilityModifier()));
}

void WorldObject::SendMovementMessageToSet(WorldPacket data, bool self, WorldObject const* except)
{
    DetailedWork::Scope deliveryWork(DetailedWork::MovementDelivery, GetGUIDLow());
    if (IsCreature())
    {
        if (!IsInWorld())
            return;
        // CMaNGOS sends NPC movement to its existing observers instead of
        // searching camera cells again for each spline packet. Bot sessions
        // still receive their normal SendPacket hooks. Transport gameobjects
        // and the native player broadcaster retain their own delivery paths.
        for (ObjectGuid guid : m_movementViewers.Snapshot())
            if (Player* viewer = GetMap()->GetPlayer(guid))
                if (viewer != except && viewer->IsInWorld() && viewer->IsInVisibleList(this))
                    viewer->GetSession()->SendPacket(&data);
        return;
    }
    if (!IsPlayer() || !sWorld.GetBroadcaster()->IsEnabled())
        SendObjectMessageToSet(&data, true, except);
    else
    {
        auto player_broadcast = ToPlayer()->m_broadcaster;

        if (player_broadcast)
            player_broadcast->QueuePacket(std::move(data), self, except ? except->GetObjectGuid() : ObjectGuid());
    }
}

void WorldObject::SendMessageToSetInRange(WorldPacket *data, float dist, bool /*bToSelf*/) const
{
    //if object is in world, map for it already created!
    if (IsInWorld())
        GetMap()->MessageDistBroadcast(this, data, dist);
}

void WorldObject::SendMessageToSetExcept(WorldPacket *data, Player const* skipped_receiver) const
{
    //if object is in world, map for it already created!
    if (IsInWorld())
    {
        MaNGOS::MessageDelivererExcept notifier(data, skipped_receiver);
        Cell::VisitWorldObjects(this, notifier, std::max(GetVisibilityDistance(), GetVisibilityModifier()));
    }
}

void WorldObject::SendObjectSpawnAnim() const
{
    WorldPacket data(SMSG_GAMEOBJECT_SPAWN_ANIM, 8);
    data << GetObjectGuid();
    SendObjectMessageToSet(&data, true);
}

void WorldObject::SendObjectDeSpawnAnim() const
{
    WorldPacket data(SMSG_GAMEOBJECT_DESPAWN_ANIM, 8);
    data << GetObjectGuid();
    SendObjectMessageToSet(&data, true);
}

bool WorldObject::isWithinVisibilityDistanceOf(Unit const* viewer, WorldObject const* viewPoint, bool inVisibleList) const
{
	if (!ExclusiveVisibleGuid.IsEmpty())
	{
		return viewer->GetObjectGuid() == ExclusiveVisibleGuid;
	}
    if (viewer->IsTaxiFlying())
    {
        float distance = std::max(World::GetMaxVisibleDistanceInFlight() + (inVisibleList ? World::GetVisibleObjectGreyDistance() : 0.0f), GetVisibilityModifier());

        // use object grey distance for all (only see objects any way)
        if (!IsWithinDistInMap(viewPoint, distance, false))
            return false;
    }
    else if (!GetTransport() || GetTransport() != viewer->GetTransport())
    {
        float distance = std::max(GetVisibilityDistance() + (inVisibleList ? World::GetVisibleUnitGreyDistance() : 0.0f), GetVisibilityModifier());

        // Any units far than max visible distance for viewer or not in our map are not visible too
        if (!IsWithinDistInMap(viewPoint, distance, false))
            return false;
    }
    return true;
}

void WorldObject::SetMap(Map * map)
{
    MANGOS_ASSERT(map);
    if (m_currMap != map)
        m_movementViewers.Clear();
    m_currMap = map;
    //lets save current map's Id/instanceId
    m_mapId = map->GetId();
    m_InstanceId = map->GetInstanceId();


    // Order is important, must be done after m_currMap is set
    SetZoneScript();
}

Map* WorldObject::GetMap() const
{
    MANGOS_ASSERT(m_currMap);
    return m_currMap;
}

void WorldObject::ResetMap()
{
    m_currMap = nullptr;
    m_zoneScript = nullptr;
}

TerrainInfo const* WorldObject::GetTerrain() const
{
    MANGOS_ASSERT(m_currMap);
    return m_currMap->GetTerrain();
}

void WorldObject::AddObjectToRemoveList()
{
    if (_deleted) // Already in the remove list
        return;

    Map* map = FindMap();

    if (!map)
    {
        sLog.outError("CRASH: Object %s at attempt add to move list not have valid map (Id: %u).", GetObjectGuid().GetString().c_str(), GetMapId());
        return;
    }

    map->AddObjectToRemoveList(this);
    _deleted = true;
}

uint32 Map::GetSummonLimitForObject(uint64 guid) const
{
    const auto itr = m_mCreatureSummonLimit.find(guid);
    if (itr != m_mCreatureSummonLimit.end())
        return itr->second;

    return sWorld.getConfig(CONFIG_UINT32_CREATURE_SUMMON_LIMIT);
}

uint32 WorldObject::GetCreatureSummonLimit() const
{
    if (FindMap())
        return FindMap()->GetSummonLimitForObject(GetGUID());
    return sWorld.getConfig(CONFIG_UINT32_CREATURE_SUMMON_LIMIT);
}

void Map::SetSummonLimitForObject(uint64 guid, uint32 limit)
{
    m_mCreatureSummonLimit[guid] = limit;
}

void WorldObject::SetCreatureSummonLimit(uint32 limit)
{
    if (FindMap())
        return FindMap()->SetSummonLimitForObject(GetGUID(), limit);
    else
        sLog.outError("Attempt to set summon limit for %s but object is not added to map yet!", GetObjectGuid().GetString().c_str());
}

uint32 Map::GetSummonCountForObject(uint64 guid) const
{
    const auto itr = m_mCreatureSummonCount.find(guid);
    if (itr != m_mCreatureSummonCount.end())
        return itr->second;

    return 0;
}

uint32 WorldObject::GetCreatureSummonCount() const
{
    if (FindMap())
        return FindMap()->GetSummonCountForObject(GetGUID());

    return 0;
}

void Map::DecrementSummonCountForObject(uint64 guid)
{
    const auto itr = m_mCreatureSummonCount.find(guid);
    if (itr != m_mCreatureSummonCount.end())
        if (itr->second != 0)
            itr->second--;
}

void WorldObject::DecrementSummonCounter()
{
    if (FindMap())
    {
        FindMap()->DecrementSummonCountForObject(GetGUID());

        // Stop the alert if all the minions despawned
        if (!FindMap()->GetSummonCountForObject(GetGUID()))
            m_summonLimitAlert = 0;
    }
    else
        sLog.outError("Attempt to decrement summon count for %s but object is not added to map yet!", GetObjectGuid().GetString().c_str());
}

void Map::IncrementSummonCountForObject(uint64 guid)
{
    m_mCreatureSummonCount[guid]++;
}

void WorldObject::IncrementSummonCounter()
{
    if (FindMap())
        FindMap()->IncrementSummonCountForObject(GetGUID());
    else
        sLog.outError("Attempt to increment summon count for %s but object is not added to map yet!", GetObjectGuid().GetString().c_str());
}

Creature *Map::SummonCreature(uint32 entry, float x, float y, float z, float ang, TempSummonType spwtype, uint32 despwtime, bool asActiveObject)
{
    CreatureInfo const* pInf = sObjectMgr.GetCreatureTemplate(entry);
    if (!pInf)
        return nullptr;

    TemporarySummon* pCreature = new TemporarySummon();

    CreatureCreatePos pos(this, x, y, z, ang);

    if (!pCreature->Create(GenerateLocalLowGuid(HIGHGUID_UNIT), pos, pInf, entry))
    {
        delete pCreature;
        return nullptr;
    }

    pCreature->SetSummonPoint(pos);

    // Active state set before added to map
    pCreature->SetActiveObjectState(asActiveObject);
    pCreature->Summon(spwtype, despwtime);
    
    // Creature Linking, Initial load is handled like respawn
    if (pCreature->IsLinkingEventTrigger())
        GetCreatureLinkingHolder()->DoCreatureLinkingEvent(LINKING_EVENT_RESPAWN, pCreature);

    // return the creature therewith the summoner has access to it
    return pCreature;
}

Creature* WorldObject::SummonCreature(uint32 id, float x, float y, float z, float ang, TempSummonType spwtype, uint32 despwtime, bool asActiveObject, uint32 pacifiedTimer, CreatureAiSetter pFuncAiSetter, bool attach)
{
    CreatureInfo const *cinfo = sObjectMgr.GetCreatureTemplate(id);
    if (!cinfo)
    {
        sLog.outErrorDb("WorldObject::SummonCreature: Creature (Entry: %u) not existed for summoner: %s. ", id, GetGuidStr().c_str());
        return nullptr;
    }

    uint32 const currentSummonCount = GetCreatureSummonCount();
    if (currentSummonCount >= GetCreatureSummonLimit() && attach)
    {
        sLog.outInfo("WorldObject::SummonCreature: %s in (map %u, instance %u) attempted to summon Creature (Entry: %u), but already has %u active summons",
            GetGuidStr().c_str(), GetMapId(), GetInstanceId(), id, currentSummonCount);

        // Alert GMs in the next tick if we don't already have an alert scheduled
        if (!m_summonLimitAlert)
            m_summonLimitAlert = 1;

        return nullptr;
    }

    TemporarySummon* pCreature = new TemporarySummon(GetObjectGuid());

    CreatureCreatePos pos(GetMap(), x, y, z, ang);

    if (x == 0.0f && y == 0.0f && z == 0.0f)
        pos = CreatureCreatePos(this, GetOrientation(), CONTACT_DISTANCE, ang);

    if (!pCreature->Create(GetMap()->GenerateLocalLowGuid(cinfo->GetHighGuid()), pos, cinfo, id))
    {
        delete pCreature;
        return nullptr;
    }

    pCreature->SetTempPacified(pacifiedTimer);
    pCreature->SetSummonPoint(pos);

    // Active state set before added to map
    pCreature->SetActiveObjectState(asActiveObject);

    pCreature->Summon(spwtype, despwtime, pFuncAiSetter);

    if (GetTypeId() == TYPEID_UNIT && ((Creature*)this)->AI())
        ((Creature*)this)->AI()->JustSummoned(pCreature);

    // Creature Linking, Initial load is handled like respawn
    if (pCreature->IsLinkingEventTrigger())
        GetMap()->GetCreatureLinkingHolder()->DoCreatureLinkingEvent(LINKING_EVENT_RESPAWN, pCreature);

    pCreature->SetWorldMask(GetWorldMask());
    // return the creature therewith the summoner has access to it

    if (attach)
        IncrementSummonCounter();
    return pCreature;
}

// Nostalrius
GameObject* WorldObject::SummonGameObject(const uint32 entry, const float x, const float y, const float z, const float ang,
    const float rotation0, const float rotation1, const float rotation2, const float rotation3, const uint32 respawnTime, const bool attach)
{
    if (!IsInWorld())
        return nullptr;

    GameObjectInfo const* goinfo = sObjectMgr.GetGameObjectInfo(entry);
    if (!goinfo)
    {
        sLog.outErrorDb("Gameobject template %u not found in database!", entry);
        return nullptr;
    }

    Map *map = GetMap();
    GameObject *go = new GameObject();

    if (!go->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), entry, map, x, y, z, ang, rotation0, rotation1, rotation2, rotation3, 100, GO_STATE_READY))
    {
        delete go;
        return nullptr;
    }

    go->SetRespawnTime(respawnTime);

    if (attach && (GetTypeId() == TYPEID_PLAYER || GetTypeId() == TYPEID_UNIT)) // Not sure how to handle this
        ((Unit*)this)->AddGameObject(go);
    else
        go->SetSpawnedByDefault(false);

    if (GetTypeId() == TYPEID_UNIT && ((Creature*)this)->AI())
        ((Creature*)this)->AI()->JustSummoned(go);

    map->Add(go);
    go->SetWorldMask(GetWorldMask());

    return go;
}

void WorldObject::SetZoneScript()
{
    m_zoneScript = nullptr;
    if (Map* pMap = FindMap())
    {
        if (pMap->IsDungeon())
            m_zoneScript = pMap->GetInstanceData();
        else if (!pMap->IsBattleGround())
            m_zoneScript = sZoneScriptMgr.GetZoneScript(GetZoneId());
    }
}
namespace MaNGOS
{
class NearUsedPosDo
{
public:
    NearUsedPosDo(WorldObject const& obj, float objX, float objY, WorldObject const* searcher, float angle, ObjectPosSelector& selector)
        : i_object(obj), i_objectX(objX), i_objectY(objY), i_searcher(searcher), i_angle(angle), i_selector(selector) {}

    void operator()(Corpse*) const {}
    void operator()(DynamicObject*) const {}

    void operator()(Creature* c) const
    {
        // skip self or target
        if (c == i_searcher || c == &i_object)
            return;

        float x, y, z;

        if (!c->IsAlive() || c->HasUnitState(UNIT_STAT_NOT_MOVE) ||
                !c->GetMotionMaster()->GetDestination(x, y, z))
        {
            x = c->GetPositionX();
            y = c->GetPositionY();

            add(c, x, y);
        }
    }

    template<class T>
    void operator()(T* u) const
    {
        // skip self or target
        if (u == i_searcher || u == &i_object)
            return;

        float x, y;

        x = u->GetPositionX();
        y = u->GetPositionY();

        add(u, x, y);
    }

    // we must add used pos that can fill places around center
    void add(WorldObject* u, float x, float y) const
    {
        // u is too nearest/far away to i_object
        if (!Geometry::IsInRange2D(i_objectX, i_objectY, x, y, i_selector.m_dist - i_selector.m_size + i_object.GetObjectBoundingRadius(), i_selector.m_dist + i_selector.m_size + i_object.GetObjectBoundingRadius()))
            return;

        float angle = Geometry::GetAngle(i_objectX, i_objectY, u->GetPositionX(), u->GetPositionY()) - i_angle;

        // move angle to range -pi ... +pi
        angle = MapManager::NormalizeOrientation(angle);

        // dist include size of u
        float dist2d = std::max(Geometry::GetDistance2D(i_objectX, i_objectY, x, y) - i_object.GetObjectBoundingRadius(), 0.0f);
        i_selector.AddUsedPos(u->GetObjectBoundingRadius(), angle, dist2d + i_object.GetObjectBoundingRadius());
    }
private:
    WorldObject const& i_object;
    float i_objectX;
    float i_objectY;
    WorldObject const* i_searcher;
    float              i_angle;
    ObjectPosSelector& i_selector;
};
}                                                           // namespace MaNGOS

//===================================================================================================

void WorldObject::GetNearPoint2DAroundPosition(float ownX, float ownY, float& x, float& y, float distance2d, float absAngle) const
{
    x = ownX + (GetObjectBoundingRadius() + distance2d) * cos(absAngle);
    y = ownY + (GetObjectBoundingRadius() + distance2d) * sin(absAngle);

    MaNGOS::NormalizeMapCoord(x);
    MaNGOS::NormalizeMapCoord(y);
}


void WorldObject::GetNearPoint(WorldObject const* searcher, float& x, float& y, float& z, float searcher_bounding_radius, float distance2d, float absAngle) const
{
    GetPosition(x, y, z);
    GetNearPointAroundPosition(searcher, x, y, z, searcher_bounding_radius, distance2d, absAngle);
}

void WorldObject::GetNearPointAroundPosition(WorldObject const* searcher, float& x, float& y, float& z, float searcher_bounding_radius, float distance2d, float absAngle) const
{
    float startX = x;
    float startY = y;
    float startZ = z;

    GetNearPoint2DAroundPosition(startX, startY, x, y, distance2d + searcher_bounding_radius, absAngle);

    // if detection disabled, return first point
    if (!sWorld.getConfig(CONFIG_BOOL_DETECT_POS_COLLISION))
    {
        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z);      // update to LOS height if available
        else
            UpdateGroundPositionZ(x, y, z);
        return;
    }

    // or remember first point
    float first_x = x;
    float first_y = y;
    bool first_los_conflict = false;                        // first point LOS problems

    // prepare selector for work
    ObjectPosSelector selector(startX, startY, GetObjectBoundingRadius(), distance2d + searcher_bounding_radius);

    // adding used positions around object
    {
        MaNGOS::NearUsedPosDo u_do(*this, startX, startY, searcher, absAngle, selector);
        MaNGOS::WorldObjectWorker<MaNGOS::NearUsedPosDo> worker(u_do);

        Cell::VisitAllObjects(this, worker, distance2d);
    }

    // maybe can just place in primary position
    if (selector.CheckOriginal())
    {
        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z);      // update to LOS height if available
        else
            UpdateGroundPositionZ(x, y, z);

        if (IsWithinLOSAtPosition(startX, startY, startZ, x, y, z))
            return;

        first_los_conflict = true;                          // first point have LOS problems
    }

    float angle;                                            // candidate of angle for free pos

    // special case when one from list empty and then empty side preferred
    if (selector.FirstAngle(angle))
    {
        GetNearPoint2DAroundPosition(startX, startY, x, y, distance2d, absAngle + angle);
        z = startZ;

        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z);      // update to LOS height if available
        else
            UpdateGroundPositionZ(x, y, z);

        if (IsWithinLOSAtPosition(startX, startY, startZ, x, y, z))
            return;
    }

    // set first used pos in lists
    selector.InitializeAngle();

    // select in positions after current nodes (selection one by one)
    while (selector.NextAngle(angle))                       // angle for free pos
    {
        GetNearPoint2DAroundPosition(startX, startY, x, y, distance2d, absAngle + angle);
        z = startZ;

        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z);      // update to LOS height if available
        else
            UpdateGroundPositionZ(x, y, z);

        if (IsWithinLOSAtPosition(startX, startY, startZ, x, y, z))
            return;
    }

    // BAD NEWS: not free pos (or used or have LOS problems)
    // Attempt find _used_ pos without LOS problem

    if (!first_los_conflict)
    {
        x = first_x;
        y = first_y;

        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z);      // update to LOS height if available
        else
            UpdateGroundPositionZ(x, y, z);

        return;
    }

    // special case when one from list empty and then empty side preferred
    if (selector.IsNonBalanced())
    {
        if (!selector.FirstAngle(angle))                    // _used_ pos
        {
            GetNearPoint2DAroundPosition(startX, startY, x, y, distance2d, absAngle + angle);
            z = startZ;

            if (searcher)
                searcher->UpdateAllowedPositionZ(x, y, z);      // update to LOS height if available
            else
                UpdateGroundPositionZ(x, y, z);

            if (IsWithinLOSAtPosition(startX, startY, startZ, x, y, z))
                return;
        }
    }

    // set first used pos in lists
    selector.InitializeAngle();

    // select in positions after current nodes (selection one by one)
    while (selector.NextUsedAngle(angle)) // Angle for used pos but maybe without LOS problem
    {
        GetNearPoint2DAroundPosition(startX, startY, x, y, distance2d, absAngle + angle);
        z = startZ;

        if (searcher)
            searcher->UpdateAllowedPositionZ(x, y, z); // Update to LOS height if available
        else
            UpdateGroundPositionZ(x, y, z);

        if (IsWithinLOSAtPosition(startX, startY, startZ, x, y, z))
            return;
    }

    // BAD BAD NEWS: all found pos (free and used) have LOS problem :(
    x = first_x;
    y = first_y;

    if (searcher)
        searcher->UpdateAllowedPositionZ(x, y, z);          // update to LOS height if available
    else
        UpdateGroundPositionZ(x, y, z);
}

void WorldObject::PlayDistanceSound(uint32 sound_id, Player const* target /*= nullptr*/) const
{
    // Nostalrius: ignored by client if unit is not loaded
    WorldPacket data(SMSG_PLAY_OBJECT_SOUND, 4 + 8);
    data << uint32(sound_id);
    data << GetObjectGuid();
    if (target)
        target->SendDirectMessage(&data);
    else
        SendObjectMessageToSet(&data, true);
}

void WorldObject::PlayDirectSound(uint32 sound_id, Player const* target /*= nullptr*/) const
{
    WorldPacket data(SMSG_PLAY_SOUND, 4);
    data << uint32(sound_id);
    if (target)
        target->SendDirectMessage(&data);
    else
        SendMessageToSet(&data, true);
}

void WorldObject::PlayDirectMusic(uint32 music_id, Player const* target /*= nullptr*/) const
{
    WorldPacket data(SMSG_PLAY_MUSIC, 4);
    data << uint32(music_id);
    if (target)
        target->SendDirectMessage(&data);
    else
        SendMessageToSet(&data, true);
}

void WorldObject::UpdateVisibilityAndView()
{
    GetViewPoint().Call_UpdateVisibilityForOwner();
    UpdateObjectVisibility();
    GetViewPoint().Event_ViewPointVisibilityChanged();
}

void WorldObject::UpdateObjectVisibility()
{
    CellPair p = MaNGOS::ComputeCellPair(GetPositionX(), GetPositionY());
    Cell cell(p);

    GetMap()->UpdateObjectVisibility(this, cell, p);
}

void WorldObject::AddToClientUpdateList()
{
    GetMap()->AddUpdateObject(this);
}

void WorldObject::RemoveFromClientUpdateList()
{
    GetMap()->RemoveUpdateObject(this);
}

struct WorldObjectChangeAccumulator
{
    UpdateDataMapType &i_updateDatas;
    WorldObject &i_object;
    WorldObjectChangeAccumulator(WorldObject &obj, UpdateDataMapType &d) : i_updateDatas(d), i_object(obj)
    {
        // send self fields changes in another way, otherwise
        // with new camera system when player's camera too far from player, camera wouldn't receive packets and changes from player
        if (i_object.isType(TYPEMASK_PLAYER))
        {
            Player* player = static_cast<Player*>(&i_object);
            if (player->GetSession() && player->GetSession()->GetSocket())
                i_object.BuildUpdateDataForPlayer(player, i_updateDatas);
        }
    }

    void Visit(CameraMapType &m)
    {
        for (const auto& iter : m)
        {
            Player* owner = iter.getSource()->GetOwner();
            // A socketless playerbot consumes game state directly from the
            // server and has no handler for SMSG_(COMPRESSED_)UPDATE_OBJECT.
            // Do not spend CPU serialising and compressing client-only field
            // updates that WorldSession would discard immediately.
            if (owner != &i_object && owner->GetSession() && owner->GetSession()->GetSocket() &&
                owner->IsInVisibleList_Unsafe(&i_object))
                i_object.BuildUpdateDataForPlayer(owner, i_updateDatas);
        }
    }

    template<class SKIP> void Visit(GridRefManager<SKIP> &) {}
};

void WorldObject::BuildUpdateData(UpdateDataMapType & update_players)
{
    WorldObjectChangeAccumulator notifier(*this, update_players);
    // Update with modifier for long range players
    Cell::VisitWorldObjects(this, notifier, std::max(GetVisibilityDistance(), GetVisibilityModifier()));

    ClearUpdateMask(false);
}

Creature* WorldObject::SummonCreature(uint32_t id, const Movement::Location& location, TempSummonType spwtype,
    uint32 despwtime, bool asActiveObject, uint32 pacifiedTimer, CreatureAiSetter pFuncAiSetter, bool attach)
{
    return SummonCreature(id, location.x, location.y, location.z, location.orientation, spwtype, despwtime, asActiveObject, pacifiedTimer, pFuncAiSetter, attach);
}

bool WorldObject::IsControlledByPlayer() const
{
    switch (GetTypeId())
    {
        case TYPEID_GAMEOBJECT:
            return ((GameObject*)this)->GetOwnerGuid().IsPlayer();
        case TYPEID_UNIT:
        case TYPEID_PLAYER:
            return ((Unit*)this)->IsCharmerOrOwnerPlayerOrPlayerItself();
        case TYPEID_DYNAMICOBJECT:
            return ((DynamicObject*)this)->GetCasterGuid().IsPlayer();
        case TYPEID_CORPSE:
            return true;
        default:
            return false;
    }
}

// Nostalrius
void Object::ForceValuesUpdateAtIndex(uint16 i)
{
    m_uint32Values_mirror[i] = GetUInt32Value(i) + 1; // makes server think the field changed
    AddDelayedAction(OBJECT_DELAYED_MARK_CLIENT_UPDATE);
}

void WorldObject::SetWorldMask(uint32 newMask)
{
    worldMask = newMask;
}

bool WorldObject::CanSeeInWorld(WorldObject const* other) const
{
    // Les GMs voient tout
    if (GetTypeId() == TYPEID_PLAYER &&
            ((Player*)this)->IsGameMaster())
        return true;
    if (GetGUID() == other->GetGUID())
        return true;

    return CanSeeInWorld(other->worldMask);
}

bool WorldObject::CanSeeInWorld(uint32 otherPhaseMask) const
{
    // Les GMs voient tout
    if (GetTypeId() == TYPEID_PLAYER &&
            ((Player*)this)->IsGameMaster())
        return true;
    // Un monde en commun ?
    if (worldMask & otherPhaseMask)
        return true;
    if (otherPhaseMask & worldMask)
        return true;
    return false;
}

// Called by Creature::DisappearAndDie
void WorldObject::DestroyForNearbyPlayers()
{
    if (!IsInWorld())
        return;

    std::list<Player*> targets;
    // Use visibility modifier for long range players
    MaNGOS::AnyPlayerInObjectRangeCheck check(this, std::max(GetVisibilityDistance(), GetVisibilityModifier()));
    MaNGOS::PlayerListSearcher<MaNGOS::AnyPlayerInObjectRangeCheck> searcher(targets, check);
    Cell::VisitWorldObjects(this, searcher, std::max(GetVisibilityDistance(), GetVisibilityModifier()));
    for (Player* plr : targets)
    {
        if (plr == this)
            continue;

        if (!plr->IsInVisibleList_Unsafe(this))
            continue;

        if (isType(TYPEMASK_UNIT) && ((Unit*)this)->GetCharmerGuid() == plr->GetObjectGuid()) // TODO: this is for puppet
            continue;

        DestroyForPlayer(plr);
        // The unguarded writer that corrupted the buckets. DestroyForPlayer
        // does network work, so it stays OUTSIDE the lock - only the erase
        // needs it.
        {
            std::unique_lock<std::shared_mutex> lock(plr->m_visibleGUIDs_lock);
            plr->m_visibleGUIDs.erase(GetGUID());
            RemoveMovementViewer(plr->GetObjectGuid());
        }


        if (ToPlayer() && ToPlayer()->m_broadcaster)
            ToPlayer()->m_broadcaster->RemoveListener(plr);
    }
}

Creature* WorldObject::FindNearestCreature(uint32 entry, float range, bool alive, Creature const* except) const
{
    Creature* pCreature = nullptr;

    CellPair pair(MaNGOS::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*this, entry, alive, range, except);
    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pCreature, creature_check);

    TypeContainerVisitor<MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck>, GridTypeMapContainer> creature_searcher(searcher);

    cell.Visit(pair, creature_searcher, *(GetMap()), *this, range);

    return pCreature;
}

Creature* WorldObject::FindRandomCreature(uint32 entry, float range, bool alive, Creature const* except) const
{
    std::list<Creature*> targets;
    GetCreatureListWithEntryInGrid(targets, entry, range);

    // remove current target
    if (except)
        targets.remove((Creature*)except);

    for (std::list<Creature*>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if ((alive && !(*tIter)->IsAlive()) || (!alive && (*tIter)->IsAlive()))
        {
            std::list<Creature*>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }

    // no appropriate targets
    if (targets.empty())
        return nullptr;

    // select random
    uint32 rIdx = urand(0, targets.size() - 1);
    std::list<Creature*>::const_iterator tcIter = targets.begin();
    for (uint32 i = 0; i < rIdx; ++i)
        ++tcIter;

    return *tcIter;
}

GameObject* WorldObject::FindNearestGameObject(uint32 entry, float range) const
{
    GameObject* pGo = nullptr;

    CellPair pair(MaNGOS::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    MaNGOS::NearestGameObjectEntryInObjectRangeCheck go_check(*this, entry, range);
    MaNGOS::GameObjectLastSearcher<MaNGOS::NearestGameObjectEntryInObjectRangeCheck> searcher(pGo, go_check);

    TypeContainerVisitor<MaNGOS::GameObjectLastSearcher<MaNGOS::NearestGameObjectEntryInObjectRangeCheck>, GridTypeMapContainer> go_searcher(searcher);

    cell.Visit(pair, go_searcher, *(GetMap()), *this, range);

    return pGo;
}

GameObject* WorldObject::FindRandomGameObject(uint32 entry, float range) const
{
    std::list<GameObject*> targets;
    GetGameObjectListWithEntryInGrid(targets, entry, range);

    for (std::list<GameObject*>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if (!(*tIter)->isSpawned())
        {
            std::list<GameObject*>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }

    // no appropriate targets
    if (targets.empty())
        return nullptr;

    // select random
    uint32 rIdx = urand(0, targets.size() - 1);
    std::list<GameObject*>::const_iterator tcIter = targets.begin();
    for (uint32 i = 0; i < rIdx; ++i)
        ++tcIter;

    return *tcIter;
}

Player* WorldObject::FindNearestPlayer(float range) const
{
    Player* target = nullptr;
    MaNGOS::NearestAlivePlayerCheck check(this, range);
    MaNGOS::PlayerLastSearcher<MaNGOS::NearestAlivePlayerCheck> searcher(target, check);
    Cell::VisitWorldObjects(this, searcher, range);

    return target;
}

void WorldObject::GetGameObjectListWithEntryInGrid(std::list<GameObject*>& lList, uint32 uiEntry, float fMaxSearchRange) const
{
    CellPair pair(MaNGOS::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    MaNGOS::AllGameObjectsWithEntryInRange check(this, uiEntry, fMaxSearchRange);
    MaNGOS::GameObjectListSearcher<MaNGOS::AllGameObjectsWithEntryInRange> searcher(lList, check);
    TypeContainerVisitor<MaNGOS::GameObjectListSearcher<MaNGOS::AllGameObjectsWithEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(GetMap()), *this, fMaxSearchRange);
}

void WorldObject::GetCreatureListWithEntryInGrid(std::list<Creature*>& lList, uint32 uiEntry, float fMaxSearchRange) const
{
    CellPair pair(MaNGOS::ComputeCellPair(GetPositionX(), GetPositionY()));
    Cell cell(pair);
    cell.SetNoCreate();

    MaNGOS::AllCreaturesOfEntryInRange check(this, uiEntry, fMaxSearchRange);
    MaNGOS::CreatureListSearcher<MaNGOS::AllCreaturesOfEntryInRange> searcher(lList, check);
    TypeContainerVisitor<MaNGOS::CreatureListSearcher<MaNGOS::AllCreaturesOfEntryInRange>, GridTypeMapContainer> visitor(searcher);

    cell.Visit(pair, visitor, *(GetMap()), *this, fMaxSearchRange);
}

void WorldObject::GetAlivePlayerListInRange(WorldObject const* pSource, std::list<Player*>& lList, float fMaxSearchRange) const
{
    MaNGOS::AnyPlayerInObjectRangeCheck check(pSource, fMaxSearchRange);
    MaNGOS::PlayerListSearcher<MaNGOS::AnyPlayerInObjectRangeCheck> searcher(lList, check);
    Cell::VisitWorldObjects(pSource, searcher, fMaxSearchRange);
}

void WorldObject::GetRelativePositions(float fForwardBackward, float fLeftRight, float fUpDown, float &x, float &y, float &z)
{
    float orientation = GetOrientation() + M_PI / 2.0f;

    float x_coef = cos(orientation);
    float y_coef = sin(orientation);

    float x_range_add = cos(GetOrientation()) * fForwardBackward;
    float y_range_add = sin(GetOrientation()) * fForwardBackward;

    x = GetPositionX() + x_coef * fLeftRight + x_range_add;
    y = GetPositionY() + y_coef * fLeftRight + y_range_add;
    z = GetPositionZ() + fUpDown;
}

void WorldObject::GetInCirclePositions(float dist, uint32 curr, uint32 total, float &x, float &y, float &z, float &o)
{
    float circleAng = (float(curr) / float(total)) * (M_PI * 2);
    x = GetPositionX() + (cos(circleAng) * dist);
    y = GetPositionY() + (sin(circleAng) * dist);
    z = GetPositionZ();
    o = circleAng - M_PI;
}

void WorldObject::GetNearRandomPositions(float distance, float &x, float &y, float &z)
{
    x = rand_norm_f() * distance;
    y = rand_norm_f() * distance;
    z = GetPositionZ();
}

void WorldObject::GetFirstCollision(float dist, float angle, float &x, float &y, float &z)
{
    x = GetPositionX();
    y = GetPositionY();
    z = GetPositionZ();

    angle += m_position.o;
    float destx, desty, destz, ground, floor;

    destx = x + dist * cos(angle);
    desty = y + dist * sin(angle);
    ground = GetMap()->GetHeight(destx, desty, MAX_HEIGHT, true);
    floor = GetMap()->GetHeight(destx, desty, z, true);
    destz = fabs(ground - z) <= fabs(floor - z) ? ground : floor;

    // check static+dynamic collision
    bool col = GetMap()->GetLosHitPosition(x, y, z + 0.5f, destx, desty, destz, -0.5f);

    // Collided
    if (col)
    {
        destx -= CONTACT_DISTANCE * cos(angle);
        desty -= CONTACT_DISTANCE * sin(angle);
        dist = sqrt((x - destx) * (x - destx) + (y - desty) * (y - desty));
    }
    float step = dist / 10.0f;

    for (uint8 j = 0; j < 10; ++j)
    {
        // do not allow too big z changes
        if (fabs(z - destz) > 6)
        {
            destx -= step * cos(angle);
            desty -= step * sin(angle);
            ground = GetMap()->GetHeight(destx, desty, MAX_HEIGHT, true);
            floor = GetMap()->GetHeight(destx, desty, z, true);
            destz = fabs(ground - z) <= fabs(floor - z) ? ground : floor;
        }
        // we have correct destz now
        else
        {
            x = destx;
            y = desty;
            z = destz;
            break;
        }
    }

    MaNGOS::NormalizeMapCoord(x);
    MaNGOS::NormalizeMapCoord(y);
    UpdateGroundPositionZ(x, y, z);
}

bool Object::IsPet() const
{
    return IsCreature() && static_cast<Creature const*>(this)->IsPet();
}

Pet const* Object::ToPet() const
{
    return IsPet() ? static_cast<Pet const*>(this) : nullptr;
}

Pet* Object::ToPet()
{
    return IsPet() ? static_cast<Pet*>(this) : nullptr;
}

bool Object::IsTotem() const
{
    return IsCreature() && static_cast<Creature const*>(this)->IsTotem();
}

Totem const* Object::ToTotem() const
{
    return IsTotem() ? static_cast<Totem const*>(this) : nullptr;
}

Totem* Object::ToTotem()
{
    return IsTotem() ? static_cast<Totem*>(this) : nullptr;
}

bool WorldObject::IsLikePlayer() const
{
    if (IsPlayer())
        return true;

    if (Pet const* pPet = ToPet())
        return pPet->isControlled() && pPet->GetOwnerGuid().IsPlayer();

    return false;
}

bool WorldObject::PrintCoordinatesError(float x, float y, float z, char const* descr) const
{
    sLog.outError("%s with invalid %s coordinates: mapid = %uu, x = %f, y = %f, z = %f", GetGuidStr().c_str(), descr, GetMapId(), x, y, z);
    return false;                                           // always false for continue assert fail
}

void WorldObject::SetActiveObjectState(bool on)
{
    if (m_isActiveObject == on)
        return;

    ASSERT(GetTypeId() == TYPEID_UNIT || GetTypeId() == TYPEID_GAMEOBJECT);

    bool world = IsInWorld();

    Map* map;
    if (world)
    {
        map = GetMap();
        if (GetTypeId() == TYPEID_UNIT)
            map->Remove((Creature*)this, false);
        else
            map->Remove((GameObject*)this, false);
    }

    m_isActiveObject = on;

    if (world)
    {
        if (GetTypeId() == TYPEID_UNIT)
            map->Add((Creature*)this);
        else
            map->Add((GameObject*)this);
    }
}

namespace MaNGOS
{
    class MonsterChatBuilderFormat
    {
    public:
        MonsterChatBuilderFormat(WorldObject const& obj, ChatMsg msgtype, int32 textId, Language language, Unit const* target, va_list* vaList = nullptr)
            : i_source(obj), i_msgtype(msgtype), i_textId(textId), i_language(language), i_target(target), i_vaList(vaList) {}
        void operator()(WorldPacket& data, int32 loc_idx)
        {
            char const* text = i_textId > 0 ? sObjectMgr.GetBroadcastText(i_textId, loc_idx, i_source.GetGender()) : sObjectMgr.GetMangosString(i_textId, loc_idx);
            char textFinal[2048];
            va_list argsCpy;
            va_copy(argsCpy, *i_vaList);
            vsnprintf(textFinal, 2048, text, argsCpy);
            va_end(argsCpy);
            ChatHandler::BuildChatPacket(data, i_msgtype, text, i_language, CHAT_TAG_NONE, i_source.GetObjectGuid(), i_source.GetNameForLocaleIdx(loc_idx),
                i_target ? i_target->GetObjectGuid() : ObjectGuid(), i_target ? i_target->GetNameForLocaleIdx(loc_idx) : "");
        }

    private:
        WorldObject const& i_source;
        ChatMsg i_msgtype;
        int32 i_textId;
        Language i_language;
        Unit const* i_target;
        va_list* i_vaList;
    };
}

namespace MaNGOS
{
    class MonsterChatBuilder
    {
    public:
        MonsterChatBuilder(WorldObject const& obj, ChatMsg msgtype, int32 textId, Language language, Unit const* target)
            : i_source(obj), i_msgtype(msgtype), i_textId(textId), i_language(language), i_target(target) {}
        void operator()(WorldPacket& data, int32 loc_idx) const
        {
            char const* text = i_textId > 0 ? sObjectMgr.GetBroadcastText(i_textId, loc_idx, i_source.GetGender()) : sObjectMgr.GetMangosString(i_textId, loc_idx);

            ChatHandler::BuildChatPacket(data, i_msgtype, text, i_language, CHAT_TAG_NONE, i_source.GetObjectGuid(), i_source.GetNameForLocaleIdx(loc_idx),
                i_target ? i_target->GetObjectGuid() : ObjectGuid(), i_target ? i_target->GetNameForLocaleIdx(loc_idx) : "");
        }

    private:
        WorldObject const& i_source;
        ChatMsg i_msgtype;
        int32 i_textId;
        Language i_language;
        Unit const* i_target;
    };
}                                                           // namespace MaNGOS

void WorldObject::PMonsterSay(int32 textId, ...) const
{
    va_list ap;
    va_start(ap, textId);
    float range = sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_SAY);
    MaNGOS::MonsterChatBuilderFormat say_build(*this, CHAT_MSG_MONSTER_SAY, textId, LANG_UNIVERSAL, nullptr, &ap);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilderFormat> say_do(say_build);
    MaNGOS::CameraDistWorker<MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilderFormat> > say_worker(this, range, say_do);
    Cell::VisitWorldObjects(this, say_worker, range);
    va_end(ap);
}

void WorldObject::PMonsterSay(const char* text, ...) const
{
    va_list ap;
    char str[2048];
    va_start(ap, text);
    vsnprintf(str, 2048, text, ap);
    va_end(ap);
    MonsterSay(str);
}

void WorldObject::PMonsterYell(int32 textId, ...) const
{
    va_list ap;
    va_start(ap, textId);
    float range = sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_YELL);
    MaNGOS::MonsterChatBuilderFormat say_build(*this, CHAT_MSG_MONSTER_SAY, textId, LANG_UNIVERSAL, nullptr, &ap);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilderFormat> say_do(say_build);
    MaNGOS::CameraDistWorker<MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilderFormat> > say_worker(this, range, say_do);
    Cell::VisitWorldObjects(this, say_worker, range);
    va_end(ap);
}

void WorldObject::PMonsterYell(const char* text, ...) const
{
    va_list ap;
    char str[2048];
    va_start(ap, text);
    vsnprintf(str, 2048, text, ap);
    va_end(ap);
    MonsterYell(str);
}

void WorldObject::MonsterSay(const std::string& text, uint32 language, Unit const* target) const
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_MONSTER_SAY, text, Language(language), CHAT_TAG_NONE, GetObjectGuid(), GetName(),
        target ? target->GetObjectGuid() : ObjectGuid(), target ? target->GetName() : "");
    SendMessageToSetInRange(&data, sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_SAY), true);
}

void WorldObject::MonsterYell(const std::string& text, uint32 language, Unit const* target) const
{
    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_MONSTER_YELL, text, Language(language), CHAT_TAG_NONE, GetObjectGuid(), GetName(),
        target ? target->GetObjectGuid() : ObjectGuid(), target ? target->GetName() : "");
    SendMessageToSetInRange(&data, sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_YELL), true);
}

void WorldObject::MonsterTextEmote(const char* text, Unit const* target, bool IsBossEmote, float rangeOverride) const
{
    float range = 0.0f;
    if (rangeOverride > 0.0f)
        range = rangeOverride;
    else
        range = sWorld.getConfig(IsBossEmote ? CONFIG_FLOAT_LISTEN_RANGE_YELL : CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE);

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, IsBossEmote ? CHAT_MSG_RAID_BOSS_EMOTE : CHAT_MSG_MONSTER_EMOTE, text, LANG_UNIVERSAL, CHAT_TAG_NONE, GetObjectGuid(), GetName(),
        target ? target->GetObjectGuid() : ObjectGuid(), target ? target->GetName() : "");
    SendMessageToSetInRange(&data, range, true);
}

void WorldObject::MonsterWhisper(const char* text, Unit const* target, bool IsBossWhisper) const
{
    if (!target || target->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, IsBossWhisper ? CHAT_MSG_RAID_BOSS_WHISPER : CHAT_MSG_MONSTER_WHISPER, text, LANG_UNIVERSAL, CHAT_TAG_NONE, GetObjectGuid(), GetName(),
        target->GetObjectGuid(), target->GetName());
    ((Player*)target)->GetSession()->SendPacket(&data);
}

void WorldObject::MonsterSayToPlayer(const char* text, Unit const* target) const
{
    if (!target || target->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_MONSTER_SAY, text, LANG_UNIVERSAL, CHAT_TAG_NONE, GetObjectGuid(), GetName(),
        target->GetObjectGuid(), target->GetName());
    ((Player*)target)->GetSession()->SendPacket(&data);
}

void WorldObject::MonsterSay(int32 textId, uint32 language, Unit const* target) const
{
    float range = sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_SAY);
    MaNGOS::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_SAY, textId, Language(language), target);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
    MaNGOS::CameraDistWorker<MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> > say_worker(this, range, say_do);
    Cell::VisitWorldObjects(this, say_worker, range);
}

void WorldObject::MonsterYell(int32 textId, uint32 language, Unit const* target) const
{
    float range = sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_YELL);
    MaNGOS::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_YELL, textId, Language(language), target);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
    MaNGOS::CameraDistWorker<MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> > say_worker(this, range, say_do);
    Cell::VisitWorldObjects(this, say_worker, range);
}

/**
 * \brief Sends text from a monster to all players within a zone.
 * \param text The text to send.
 * \param messageType The type of message to send (say, yell, etc.)
 * \param language The language to send. Defaults to LANG_UNIVERSAL.
 * \param target The target of the message (used to fill name, class, gender, etc.)
 * \param senderName The sender name to display (for message types that support it.) If nullptr, will use our name.
 */
void WorldObject::MonsterSendTextToZone(const char* text, const ChatMsg messageType, const Language language, Unit* const target, const char* senderName) const
{
    WorldPacket worldPacket;
    ChatHandler::BuildChatPacket(
        worldPacket,
        messageType,
        text,
        language,
        CHAT_TAG_NONE,
        GetObjectGuid(),
        senderName == nullptr ? GetName() : senderName,
        target 
            ? target->GetObjectGuid()
            : ObjectGuid(),
        target ? target->GetName() : "");

    if (const auto &players = GetMap()->GetPlayers(); !players.isEmpty())
    {
        (*players.begin())->SendToPlayersInZone(&worldPacket, GetZoneId());
    }
}

void WorldObject::MonsterYellToZone(int32 textId, uint32 language, Unit const* target) const
{
    MaNGOS::MonsterChatBuilder say_build(*this, CHAT_MSG_MONSTER_YELL, textId, Language(language), target);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);

    uint32 zoneid = GetZoneId();

    auto const& pList = GetMap()->GetPlayers();
    for (const auto& itr : pList)
        if (itr.getSource()->GetZoneId() == zoneid)
            say_do(itr.getSource());
}

void WorldObject::MonsterScriptToZone(int32 textId, ChatMsg type, uint32 language, Unit const* target) const
{
    MaNGOS::MonsterChatBuilder say_build(*this, type, textId, Language(language), target);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);

    uint32 zoneid = GetZoneId();

    auto const& pList = GetMap()->GetPlayers();
    for (const auto& itr : pList)
        if (itr.getSource()->GetZoneId() == zoneid)
            say_do(itr.getSource());
}

void WorldObject::MonsterTextEmote(int32 textId, Unit const* target, bool IsBossEmote, float rangeOverride) const
{
    float range = 0.0f;
    if (rangeOverride > 0.0f)
        range = rangeOverride;
    else
        range = sWorld.getConfig(IsBossEmote ? CONFIG_FLOAT_LISTEN_RANGE_YELL : CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE);

    MaNGOS::MonsterChatBuilder say_build(*this, IsBossEmote ? CHAT_MSG_RAID_BOSS_EMOTE : CHAT_MSG_MONSTER_EMOTE, textId, LANG_UNIVERSAL, target);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> say_do(say_build);
    MaNGOS::CameraDistWorker<MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilder> > say_worker(this, range, say_do);
    Cell::VisitWorldObjects(this, say_worker, range);
}

void WorldObject::MonsterWhisper(int32 textId, Unit const* target, bool IsBossWhisper) const
{
    if (!target || target->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 loc_idx = ((Player*)target)->GetSession()->GetSessionDbLocaleIndex();
    char const* text = textId > 0 ? sObjectMgr.GetBroadcastText(textId, loc_idx, GetGender()) : sObjectMgr.GetMangosString(textId, loc_idx);

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, IsBossWhisper ? CHAT_MSG_RAID_BOSS_WHISPER : CHAT_MSG_MONSTER_WHISPER, text, LANG_UNIVERSAL, CHAT_TAG_NONE, GetObjectGuid(), GetName(),
        target->GetObjectGuid(), target->GetName());

    ((Player*)target)->GetSession()->SendPacket(&data);
}

void WorldObject::MonsterSayToPlayer(int32 textId, Unit const* target) const
{
    if (!target || target->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 loc_idx = ((Player*)target)->GetSession()->GetSessionDbLocaleIndex();
    char const* text = textId > 0 ? sObjectMgr.GetBroadcastText(textId, loc_idx, GetGender()) : sObjectMgr.GetMangosString(textId, loc_idx);

    WorldPacket data;
    ChatHandler::BuildChatPacket(data, CHAT_MSG_MONSTER_SAY, text, LANG_UNIVERSAL, CHAT_TAG_NONE, GetObjectGuid(), GetName(),
        target->GetObjectGuid(), target->GetName());

    ((Player*)target)->GetSession()->SendPacket(&data);
}

void WorldObject::GetPosition(float &x, float &y, float &z, Transport* t) const
{
    if (t && m_movementInfo.t_guid == t->GetObjectGuid())
    {
        x = m_movementInfo.t_pos.x;
        y = m_movementInfo.t_pos.y;
        z = m_movementInfo.t_pos.z;
        return;
    }
    x = m_position.x;
    y = m_position.y;
    z = m_position.z;
    if (t)
        t->CalculatePassengerOffset(x, y, z);
}

void WorldObject::Update(uint32 update_diff, uint32 /*time_diff*/)
{
    if (m_summonLimitAlert)
    {
        if (m_summonLimitAlert <= update_diff)
        {
            std::stringstream message;
            message << "SummonCreature: " << GetGuidStr().c_str()
                    << " in (map " << GetMapId() << ", instance " << GetInstanceId() << ")"
                    << " has " << GetCreatureSummonCount() << " active summons,"
                    << " and the limit is " << GetCreatureSummonLimit();
            sWorld.SendGMText(LANG_GM_ANNOUNCE_COLOR, "SummonAlert", message.str().c_str());

            m_summonLimitAlert = 5 * MINUTE * IN_MILLISECONDS;
        }
        else
            m_summonLimitAlert -= update_diff;
    }

    ExecuteDelayedActions();
}

class NULLNotifier
{
public:
    template<class T> void Visit(GridRefManager<T> &m) {}
    void Visit(CameraMapType&) {}
};

void WorldObject::LoadMapCellsAround(float dist) const
{
    ASSERT(IsInWorld());
    NULLNotifier notifier = NULLNotifier();
    Cell::VisitAllObjects(this, notifier, dist, false);
}

Unit* WorldObject::SelectMagnetTarget(Unit *victim, Spell* spell, SpellEffectIndex eff)
{
    if (!victim)
        return nullptr;

    SpellEntry const* pProto = nullptr;
    if (spell)
        pProto = spell->m_spellInfo;

    // Magic case

    if (pProto && pProto->HasAttribute(SPELL_ATTR_EX2_NO_INITIAL_THREAT))
        return victim;

    if (spell && pProto && (pProto->DmgClass == SPELL_DAMAGE_CLASS_MAGIC || pProto->SpellVisual == 7250) && pProto->Dispel != DISPEL_POISON && !(pProto->Attributes & SPELL_ATTR_IS_ABILITY))
    {
        Unit::AuraList const& magnetAuras = victim->GetAurasByType(SPELL_AURA_SPELL_MAGNET);
        for (const auto magnetAura : magnetAuras)
        {
            if (Unit* magnet = magnetAura->GetCaster())
            {
                if (magnet->IsAlive() && magnet->IsInMap(this) && spell->CheckTarget(magnet, eff))
                {
                    if (SpellAuraHolder* holder = magnetAura->GetHolder())
                        if (holder->DropAuraCharge())
                        {
                            magnet->RemoveAurasDueToSpell(holder->GetId()); // Remove from grounding totem also
                            victim->RemoveSpellAuraHolder(holder);
                        }
                    return magnet;
                }
            }
        }
    }
    // Melee && ranged case
    else
    {
        if (spell && pProto && pProto->HasAttribute(SPELL_ATTR_EX_CANT_BE_REDIRECTED))
            return victim;

        auto const& hitTriggerAuras = victim->GetAurasByType(SPELL_AURA_ADD_CASTER_HIT_TRIGGER);
        for (auto& hitTriggerAura : hitTriggerAuras)
        {
            if (Unit* magnet = hitTriggerAura->GetCaster())
            {
                if (magnet->IsAlive() && IsValidAttackTarget(magnet))
                {
                    if (roll_chance_i(hitTriggerAura->GetModifier()->m_amount))
                    {
                        if (SpellAuraHolder* holder = hitTriggerAura->GetHolder())
                            if (holder->DropAuraCharge())
                                victim->RemoveSpellAuraHolder(holder);
                        return magnet;
                    }
                }
            }
        }
    }

    return victim;
}

FactionTemplateEntry const* WorldObject::GetFactionTemplateEntry() const
{
    FactionTemplateEntry const* entry = sObjectMgr.GetFactionTemplateEntry(GetFactionTemplateId());
    if (!entry)
    {
        static ObjectGuid guid;                             // prevent repeating spam same faction problem

        if (GetObjectGuid() != guid)
        {
            sLog.outError("%s have invalid faction (faction template id) #%u", GetGuidStr().c_str(), GetFactionTemplateId());
            guid = GetObjectGuid();
        }
    }
    return entry;
}

FactionEntry const* WorldObject::GetFactionEntry() const
{
    if (FactionTemplateEntry const* pTemplate = GetFactionTemplateEntry())
        return sObjectMgr.GetFactionEntry(pTemplate->faction);

    return nullptr;
}

uint32 WorldObject::GetFactionId() const
{
    if (FactionTemplateEntry const* pTemplate = GetFactionTemplateEntry())
        return pTemplate->faction;

    return 0;
}

// function based on function Unit::UnitReaction from 13850 client
ReputationRank WorldObject::GetReactionTo(WorldObject const* target) const
{
    // possible crashfix
    if (!target)
        return REP_NEUTRAL;

    // always friendly to self
    if (this == target)
        return REP_FRIENDLY;

    // always friendly to charmer or owner
    if (IsUnit() && target->IsUnit() && ToUnit()->GetCharmerOrOwnerOrSelf() == target->ToUnit()->GetCharmerOrOwnerOrSelf())
        return REP_FRIENDLY;

    Player const* selfPlayerOwner = GetAffectingPlayer();
    Player const* targetPlayerOwner = target->GetAffectingPlayer();

    // check forced reputation to support SPELL_AURA_FORCE_REACTION
    if (selfPlayerOwner)
    {
        if (selfPlayerOwner->IsGameMaster())
            return REP_NEUTRAL;

        if (FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry())
            if (ReputationRank const* repRank = selfPlayerOwner->GetReputationMgr().GetForcedRankIfAny(targetFactionTemplateEntry))
                return *repRank;
    }
    else if (targetPlayerOwner)
    {
        if (targetPlayerOwner->IsGameMaster())
            return REP_NEUTRAL;

        if (FactionTemplateEntry const* selfFactionTemplateEntry = GetFactionTemplateEntry())
            if (ReputationRank const* repRank = targetPlayerOwner->GetReputationMgr().GetForcedRankIfAny(selfFactionTemplateEntry))
                return *repRank;
    }

    bool b_IsPossessed = HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_POSSESSED) || target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_POSSESSED);
    if (IsUnit() && (HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) || b_IsPossessed))
    {
        if (target->IsUnit() && (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED) || b_IsPossessed))
        {
            if (selfPlayerOwner && targetPlayerOwner)
            {
                // always friendly to other unit controlled by player, or to the player himself
                if (selfPlayerOwner == targetPlayerOwner)
                    return REP_FRIENDLY;

                // m_duel - always hostile to opponent
                if (selfPlayerOwner->m_duel && selfPlayerOwner->m_duel->opponent == targetPlayerOwner->GetObjectGuid() && selfPlayerOwner->m_duel->startTime != 0 && !selfPlayerOwner->m_duel->finished)
                    return REP_HOSTILE;

                // same group - checks dependant only on our faction - skip FFA_PVP for example
                if (selfPlayerOwner->IsInRaidWith(targetPlayerOwner))
                    return REP_FRIENDLY;

                if (selfPlayerOwner->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_SANCTUARY) && targetPlayerOwner->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_SANCTUARY))
                    return REP_FRIENDLY;

                // Hackfix because UNIT_BYTE2_FLAG_FFA_PVP is not implemented yet.
                if (selfPlayerOwner->IsFFAPvP() && targetPlayerOwner->IsFFAPvP())
                    return REP_HOSTILE;
            }
        }

        if (selfPlayerOwner)
        {
            if (FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry())
            {
                if (ReputationRank const* repRank = selfPlayerOwner->GetReputationMgr().GetForcedRankIfAny(targetFactionTemplateEntry))
                    return *repRank;

                if (FactionEntry const* targetFactionEntry = sObjectMgr.GetFactionEntry(targetFactionTemplateEntry->faction))
                {
                    if (targetFactionEntry->CanHaveReputation())
                    {
                        // check contested flags
                        if (targetFactionTemplateEntry->factionFlags & FACTION_TEMPLATE_FLAG_ATTACK_PVP_ACTIVE_PLAYERS && selfPlayerOwner->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
                            return REP_HOSTILE;

                        // if faction has reputation, hostile state depends only from AtWar state
                        if (FactionState const* factionState = selfPlayerOwner->GetReputationMgr().GetState(targetFactionEntry))
                            if (factionState->Flags & FACTION_FLAG_AT_WAR)
                                return REP_HOSTILE;

                        return REP_FRIENDLY;
                    }
                }
            }
        }
    }
    // do checks dependant only on our faction
    return GetFactionReactionTo(GetFactionTemplateEntry(), target);
}

ReputationRank WorldObject::GetFactionReactionTo(FactionTemplateEntry const* factionTemplateEntry, WorldObject const* target)
{
    // always neutral when no template entry found
    if (!factionTemplateEntry)
        return REP_NEUTRAL;

    FactionTemplateEntry const* targetFactionTemplateEntry = target->GetFactionTemplateEntry();
    if (!targetFactionTemplateEntry)
        return REP_NEUTRAL;

    if (Player const* targetPlayerOwner = target->GetAffectingPlayer())
    {
        // check contested flags
        if (factionTemplateEntry->IsContestedGuardFaction()
                && targetPlayerOwner->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
            return REP_HOSTILE;
        if (ReputationRank const* repRank = targetPlayerOwner->GetReputationMgr().GetForcedRankIfAny(factionTemplateEntry))
            return *repRank;
        if (FactionEntry const* factionEntry = sObjectMgr.GetFactionEntry(factionTemplateEntry->faction))
        {
            if (factionEntry->CanHaveReputation())
            {
                // CvP case - check reputation, don't allow state higher than neutral when at war
                ReputationRank repRank = targetPlayerOwner->GetReputationMgr().GetRank(factionEntry);
                if (FactionState const* factionState = targetPlayerOwner->GetReputationMgr().GetState(factionEntry))
                    if (factionState->Flags & FACTION_FLAG_AT_WAR)
                        repRank = std::min(REP_NEUTRAL, repRank);
                return repRank;
            }
        }
    }

    // common faction based check
    if (factionTemplateEntry->IsHostileTo(*targetFactionTemplateEntry))
        return REP_HOSTILE;
    if (factionTemplateEntry->IsFriendlyTo(*targetFactionTemplateEntry))
        return REP_FRIENDLY;
    if (targetFactionTemplateEntry->IsFriendlyTo(*factionTemplateEntry))
        return REP_FRIENDLY;

    // neutral by default
    return REP_NEUTRAL;
}

uint32 WorldObject::GetLevelForTarget(WorldObject const* target) const
{
    // Bosses are 3 levels higher than players.
    if (Creature const* pCreature = ToCreature())
    {
        if (!pCreature->IsWorldBoss() || !target || !target->IsUnit())
            return pCreature->GetLevel();

        uint32 level = static_cast<Unit const*>(target)->GetLevel() + sWorld.getConfig(CONFIG_UINT32_WORLD_BOSS_LEVEL_DIFF);
        if (level < 1)
            return 1;
        if (level > 255)
            return 255;
        return level;
    }

    if (Unit const* pUnit = ToUnit())
        return pUnit->GetLevel();

    if (GameObject const* pGo = ToGameObject())
    {
        uint32 level = 0;

        switch (pGo->GetGOInfo()->type)
        {
            case GAMEOBJECT_TYPE_CHEST:
                level = pGo->GetGOInfo()->chest.level;
                break;
            case GAMEOBJECT_TYPE_TRAP:
                level = pGo->GetGOInfo()->trap.level;
                break;
        }

        if (!level)
            level = pGo->GetUInt32Value(GAMEOBJECT_LEVEL);

        if (level)
            return level;
    }

    if (Unit const* pUnit = ::ToUnit(target))
        return pUnit->GetLevel();

    return PLAYER_MAX_LEVEL;
}

uint32 WorldObject::GetWeaponSkillValue(WeaponAttackType attType, WorldObject const* target) const
{
    if (Player const* pPlayer = ToPlayer())
    {
        Item* item = pPlayer->GetWeaponForAttack(attType, true, true);

        // feral or unarmed skill only for base attack
        if (attType != BASE_ATTACK && !item)
            return 0;

        if (pPlayer->IsInFeralForm())
            return GetSkillMaxForLevel();              // always maximized SKILL_FERAL_COMBAT in fact

        // weapon skill or (unarmed for base attack)
        uint32 skill = item ? item->GetProto()->GetProficiencySkill() : SKILL_UNARMED;

// Daemon: pas en preBC !
//        // in PvP use full skill instead current skill value
//        value = (target && target->GetTypeId() == TYPEID_PLAYER)
//            ? ((Player*)this)->GetSkillMax(skill)
//            : ((Player*)this)->GetSkillValue(skill);
        return pPlayer->GetSkillValue(skill);
    }

    uint32 skill = GetUnitMeleeSkill(target);
    if (Pet const* pet = ToPet())
    {
        if (Unit const* owner = pet->GetOwner())
        {
            if (owner->HasAura(51555))      // Bestial Precision Rank 2
                skill += 10;
            else if (owner->HasAura(51554)) // Bestial Precision Rank 1
                skill += 5;
        }
    }

    return skill;
}

uint32 WorldObject::GetDefenseSkillValue(WorldObject const* target) const
{
    if (Player const* pPlayer = ToPlayer())
    {
        // in PvP use full skill instead current skill value
        uint32 value = (target && target->IsPlayer())
            ? pPlayer->GetSkillMax(SKILL_DEFENSE)
            : pPlayer->GetSkillValue(SKILL_DEFENSE);
        return value;
    }

    return GetUnitMeleeSkill(target);
}

// Calculate spell hit result can be:
// Every spell can: Evade/Immune/Reflect/Sucesful hit
// For melee based spells:
//   Miss
//   Dodge
//   Parry
// For spells
//   Resist
SpellMissInfo WorldObject::SpellHitResult(Unit* pVictim, SpellEntry const* spell, SpellEffectIndex effIndex, bool CanReflect, Spell* spellPtr)
{
    // Return evade for units in evade mode
    if (pVictim->GetTypeId() == TYPEID_UNIT && ((Creature*)pVictim)->IsInEvadeMode())
        return SPELL_MISS_EVADE;

    // Check for immune (use charges)
    if (/* pVictim != this && */
       !spell->HasAttribute(SPELL_ATTR_UNAFFECTED_BY_INVULNERABILITY) &&
        pVictim->IsImmuneToSpell(spell, pVictim == this))
        return SPELL_MISS_IMMUNE;

    // All positive spells can`t miss
    // TODO: client not show miss log for this spells - so need find info for this in dbc and use it!
    if (spell->IsPositiveSpell(this, pVictim) || spell->IsPositiveEffect(effIndex, this, pVictim))
        return SPELL_MISS_NONE;

    // Check for immune (use charges)
    SpellSchoolMask schoolMask;
    if (spellPtr)
        schoolMask = spellPtr->m_spellSchoolMask;
    else
        schoolMask = spell->GetSpellSchoolMask();

    if (pVictim != this && pVictim->IsImmuneToDamage(schoolMask, spell))
        return SPELL_MISS_IMMUNE;

    // Try victim reflect spell
    if (CanReflect)
    {
        int32 reflectchance = pVictim->GetTotalAuraModifier(SPELL_AURA_REFLECT_SPELLS);
        Unit::AuraList const& mReflectSpellsSchool = pVictim->GetAurasByType(SPELL_AURA_REFLECT_SPELLS_SCHOOL);
        for (const auto i : mReflectSpellsSchool)
            if (i->GetModifier()->m_miscvalue & spell->GetSpellSchoolMask())
                reflectchance += i->GetModifier()->m_amount;

        if (reflectchance > 0 && roll_chance_i(reflectchance))
        {
            // Start triggers for remove charges if need (trigger only for victim, and mark as active spell)
            ProcDamageAndSpell(pVictim, PROC_FLAG_NONE, PROC_FLAG_TAKE_HARMFUL_SPELL, PROC_EX_REFLECT, 1, 1, BASE_ATTACK, spell);
            return SPELL_MISS_REFLECT;
        }
    }

    switch (spell->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_NONE:
            return SPELL_MISS_NONE;
        case SPELL_DAMAGE_CLASS_MAGIC:
            return MagicSpellHitResult(pVictim, spell, spellPtr);
        case SPELL_DAMAGE_CLASS_MELEE:
        case SPELL_DAMAGE_CLASS_RANGED:
            return MeleeSpellHitResult(pVictim, spell, spellPtr);
    }
    return SPELL_MISS_NONE;
}

void WorldObject::ProcDamageAndSpell(Unit *pVictim, uint32 procAttacker, uint32 procVictim, uint32 procExtra, uint32 amount, int32 originalAmount, WeaponAttackType attType, SpellEntry const *procSpell, Spell* spell)
{
    if ((pVictim && !IsInMap(pVictim)) || !IsInWorld())
        return;

    ProcTriggeredList procTriggered;

    // Not much to do if no flags are set.
    if (procAttacker)
        if (Unit* pUnit = ToUnit())
            pUnit->ProcDamageAndSpellFor(false, pVictim, procAttacker, procExtra, attType, procSpell, amount, procTriggered, spell);

    // Now go on with a victim's events'n'auras
    // Not much to do if no flags are set or there is no victim
    if (pVictim && pVictim->IsAlive() && procVictim)
    {
        // http://blue.cardplace.com/cache/wow-paladin/1069149.htm
        // "Charges will not generate off auto attacks or npc attacks by trying"
        // "to sit down and force a crit. However, ability crits from physical"
        // "abilities such as Sinister Strike, Hamstring, Auto-shot, Aimed shot,"
        // "etc will generate a charge if you're sitting."
        pVictim->ProcDamageAndSpellFor(true, IsUnit() ? static_cast<Unit*>(this) : pVictim, procVictim, !procSpell && !pVictim->IsStandingUpForProc() ? procExtra & ~PROC_EX_CRITICAL_HIT : procExtra, attType, procSpell, amount, procTriggered, spell);
    }

    if (Unit* pUnit = ToUnit())
        pUnit->HandleTriggers(pVictim, procExtra, amount, originalAmount, procSpell, procTriggered);
}

// Melee based spells can be miss, parry or dodge on this step
// Crit or block - determined on damage calculation phase! (and can be both in some time)
float WorldObject::MeleeSpellMissChance(Unit* pVictim, WeaponAttackType attType, int32 skillDiff, SpellEntry const* spell, Spell* spellPtr)
{
    if (!pVictim || !pVictim->IsStandingUp())
        return 0.0f;

    // Calculate hit chance (more correct for chance mod)
    float hitChance = 0.0f;
    float missChance = 0.0f;

    // PvP - PvE melee chances
    if (pVictim->GetTypeId() == TYPEID_PLAYER)
        missChance = 5.0f - skillDiff * 0.04f;
    else
        missChance = 5.0f - skillDiff * 0.2f;

    // Low level reduction
    if (!pVictim->IsPlayer() && pVictim->GetLevel() < 10)
        missChance *= pVictim->GetLevel() / 10.0f;

    if (Unit* pUnit = ToUnit())
    {
        // Spellmod from SPELLMOD_RESIST_MISS_CHANCE
        if (Player * modOwner = pUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(spell->Id, SPELLMOD_RESIST_MISS_CHANCE, hitChance, spellPtr);

        // Bonuses from attacker aura and ratings
        if (attType == RANGED_ATTACK)
            hitChance += pUnit->m_modRangedHitChance;
        else
            hitChance += pUnit->m_modMeleeHitChance;

        if (pUnit->IsPet())
        {
            if (Unit* owner = pUnit->GetOwner())
            {
                Unit::AuraList const& petMeleeHitAuras = pUnit->GetAurasByType(SPELL_AURA_MOD_PET_MELEE_HIT_PERCENT_OF_OWNER);
                for (const auto aura : petMeleeHitAuras)
                    hitChance += owner->m_modSpellHitChance * aura->GetModifier()->m_amount / 100.0f;

                Unit::AuraList const& ownerPetMeleeHitAuras = owner->GetAurasByType(SPELL_AURA_MOD_PET_MELEE_HIT_PERCENT_OF_OWNER);
                for (const auto aura : ownerPetMeleeHitAuras)
                    hitChance += owner->m_modSpellHitChance * aura->GetModifier()->m_amount / 100.0f;
            }
        }
    } 

    // Hit chance depends from victim auras
    if (attType == RANGED_ATTACK)
        hitChance += pVictim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE);
    else
        hitChance += pVictim->GetTotalAuraModifier(SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE);

    missChance -= hitChance;

    // Limit miss chance from 0 to 60%
    if (missChance < 0.0f)
        return 0.0f;
    if (missChance > 60.0f)
        return 60.0f;
    return missChance;
}

// Melee based spells hit result calculations
SpellMissInfo WorldObject::MeleeSpellHitResult(Unit* pVictim, SpellEntry const* spell, Spell* spellPtr)
{
    WeaponAttackType attType = spell->DmgClass == SPELL_DAMAGE_CLASS_RANGED ? RANGED_ATTACK : BASE_ATTACK;

    // Warrior spell Execute (5308) should never dodge, miss, resist ... Only the trigger can (20647)
    if (spell->IsFitToFamily<SPELLFAMILY_WARRIOR, CF_WARRIOR_EXECUTE>() && spell->Id != 20647)
        return SPELL_MISS_NONE;

    // Hammer of Wrath should not use weapon skill, but Bloodthirst should.
    // bonus from skills is 0.04% per skill Diff
    int32 attackerWeaponSkill = (spell->rangeIndex == SPELL_RANGE_IDX_COMBAT || spell->EquippedItemClass == ITEM_CLASS_WEAPON) ?
                                int32(GetWeaponSkillValue(attType, pVictim)) : GetSkillMaxForLevel();
    int32 skillDiff = attackerWeaponSkill - int32(pVictim->GetSkillMaxForLevel(this));
    int32 fullSkillDiff = attackerWeaponSkill - int32(pVictim->GetDefenseSkillValue(this));
    int32 minWeaponSkill = GetSkillMaxForLevel(pVictim) < attackerWeaponSkill ? GetSkillMaxForLevel(pVictim) : attackerWeaponSkill;
    int32 cappedSkillDiff = minWeaponSkill - pVictim->GetSkillMaxForLevel(this);

    uint32 roll = urand(0, 9999);

    uint32 missChance = uint32(MeleeSpellMissChance(pVictim, attType, fullSkillDiff, spell, spellPtr) * 100.0f);
    // Roll miss
    uint32 tmp = spell->AttributesEx3 & SPELL_ATTR_EX3_ALWAYS_HIT ? 0 : missChance;
    if (roll < tmp)
        return SPELL_MISS_MISS;

    // Chance resist mechanic for spell (effect resistance handled later)
    int32 resist_mech = 0;
    if (spell->Mechanic)
        resist_mech = pVictim->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MECHANIC_RESISTANCE, spell->Mechanic) * 100;
    // Roll chance
    tmp += resist_mech;
    if (roll < tmp)
        return SPELL_MISS_RESIST;

    bool canDodge = true;
    bool canParry = true;
    bool canBlock = spell->HasAttribute(SPELL_ATTR_EX3_COMPLETELY_BLOCKED);

    // Same spells cannot be parry/dodge
    if (spell->Attributes & SPELL_ATTR_IMPOSSIBLE_DODGE_PARRY_BLOCK)
        return SPELL_MISS_NONE;

    // Ranged attack cannot be parry/dodge
    if (attType == RANGED_ATTACK)
        return SPELL_MISS_NONE;

    bool from_behind = !pVictim->HasInArc(this, M_PI_F);

    // Check for attack from behind
    if (from_behind)
    {
        // Can`t dodge from behind in PvP (but its possible in PvE)
        if (GetTypeId() == TYPEID_PLAYER && pVictim->GetTypeId() == TYPEID_PLAYER)
            canDodge = false;

        // Can`t parry or block
        canParry = false;
        canBlock = false;
    }
    // Check creatures flags_extra for disable parry
    if (Creature* pCreatureVictim = pVictim->ToCreature())
    { 
        if (pCreatureVictim->HasExtraFlag(CREATURE_FLAG_EXTRA_NO_PARRY))
            canParry = false;
        if (pCreatureVictim->HasExtraFlag(CREATURE_FLAG_EXTRA_NO_BLOCK))
            canBlock = false;
    }
    // Check if the player can parry
    else
    {
        if (!((Player*)pVictim)->CanParry())
            canParry = false;
        if (!((Player*)pVictim)->CanBlock())
            canBlock = false;
    }

    if (canDodge)
    {
        // Roll dodge
        int32 dodgeModifier = pVictim->IsPlayer() ? skillDiff * 4 : skillDiff * 10;
        int32 dodgeChance = int32(pVictim->GetUnitDodgeChance() * 100.0f) - dodgeModifier;

        if (dodgeChance < 0)
            dodgeChance = 0;

        // Low level reduction
        if (!pVictim->IsPlayer() && pVictim->GetLevel() < 10)
            dodgeChance *= pVictim->GetLevel() / 10.0f;

        tmp += dodgeChance;
        if (roll < tmp)
            return SPELL_MISS_DODGE;
    }

    if (canParry)
    {
        // Roll parry
        int32 parryModifier = pVictim->IsPlayer() ? skillDiff * 4 : cappedSkillDiff < -10 ? 60 * cappedSkillDiff : 20 * cappedSkillDiff;
        int32 parryChance = int32(pVictim->GetUnitParryChance() * 100.0f) - parryModifier;

        // Can`t parry from behind
        if (parryChance < 0)
            parryChance = 0;

        // Low level reduction
        if (!pVictim->IsPlayer() && pVictim->GetLevel() < 10)
            parryChance *= pVictim->GetLevel() / 10.0f;

        tmp += parryChance;
        if (roll < tmp)
            return SPELL_MISS_PARRY;
    }

    // There are 2 types of ability blocks: partial and full
    // Fully blockable spells have a specific attribute, which generates a miss instead of a partial block
    // Spells with an attribute must be rolled for block once on spell hit die
    // Spells without an attribute must be rolled for partial block only inside damage calculation
    if (canBlock && pVictim->RollSpellBlockChanceOutcome(this, attType))
        return SPELL_MISS_BLOCK;

    return SPELL_MISS_NONE;
}

SpellMissInfo WorldObject::MagicSpellHitResult(Unit* pVictim, SpellEntry const* spell, Spell* spellPtr)
{
    // Can`t miss on dead target (on skinning for example)
    if (!pVictim->IsAlive())
        return SPELL_MISS_NONE;

    // Spell cannot be resisted (not exist on dbc, custom flag)
    if (spell->AttributesEx4 & SPELL_ATTR_EX4_IGNORE_RESISTANCES)
        return SPELL_MISS_NONE;

    int32 hitChance = MagicSpellHitChance(pVictim, spell, spellPtr);
    int32 missChance = 10000 - hitChance;
    int32 rand = irand(0, 10000);

    if ((IsPlayer() && ToPlayer()->HasOption(PLAYER_CHEAT_UNRANDOMIZE)) ||
        (pVictim->IsPlayer() && pVictim->ToPlayer()->HasOption(PLAYER_CHEAT_UNRANDOMIZE)))
        missChance = 0;

    if (rand < missChance)
        return SPELL_MISS_RESIST;

    return SPELL_MISS_NONE;
}

int32 WorldObject::MagicSpellHitChance(Unit* pVictim, SpellEntry const* spell, Spell* spellPtr)
{
     if (spell->AttributesEx3 & SPELL_ATTR_EX3_ALWAYS_HIT)
        return 10000;

    SpellSchoolMask schoolMask = spell->GetSpellSchoolMask();

    // PvP - PvE spell misschances per leveldif > 2
    int32 lchance = pVictim->IsPlayer() ? 7 : 11;
    int32 leveldif = int32(pVictim->GetLevelForTarget(this)) - int32(GetLevelForTarget(pVictim));

    // Base hit chance from attacker and victim levels
    float modHitChance;
    if (leveldif < 3)
        modHitChance = 96 - leveldif;
    else
        modHitChance = 94 - (leveldif - 2) * lchance;

    // Turtle: Cap level based miss chance to 1 in 3.
    if (IsPlayer() && modHitChance < 33)
        modHitChance = 33;

    //DEBUG_UNIT(this, DEBUG_SPELL_COMPUTE_RESISTS, "%s [%u] : Binary [%s]. Base hit chance %f, level diff: %d", spell->SpellName[2].c_str(), spell->Id, spell->IsBinary() ? "YES" : "NO", modHitChance, leveldif);

    // Spellmod from SPELLMOD_RESIST_MISS_CHANCE
    if (Unit* pUnit = ToUnit())
    {
        if (Player * modOwner = pUnit->GetSpellModOwner())
        {
            modOwner->ApplySpellMod(spell->Id, SPELLMOD_RESIST_MISS_CHANCE, modHitChance, spellPtr);
            //DEBUG_UNIT(this, DEBUG_SPELL_COMPUTE_RESISTS, "SPELLMOD_RESIST_MISS_CHANCE : %f", modHitChance);
        }
    }

    // Chance hit from victim SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE auras
    modHitChance += pVictim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE, schoolMask);
    //DEBUG_UNIT(this, DEBUG_SPELL_COMPUTE_RESISTS, "SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE (+ %i) : %f", pVictim->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE, schoolMask), modHitChance);

    // Reduce spell hit chance for Area of effect spells from victim SPELL_AURA_MOD_AOE_AVOIDANCE aura
    if (spell->IsAreaOfEffectSpell())
    {
        modHitChance -= pVictim->GetTotalAuraModifier(SPELL_AURA_MOD_AOE_AVOIDANCE);
        //DEBUG_UNIT(this, DEBUG_SPELL_COMPUTE_RESISTS, "SPELL_AURA_MOD_AOE_AVOIDANCE (- %i) : %f", pVictim->GetTotalAuraModifier(SPELL_AURA_MOD_AOE_AVOIDANCE), modHitChance);
    }

    // Chance resist mechanic for spell (effect resistance handled later)
    int32 resist_mech = 0;
    if (spell->Mechanic)
        resist_mech = pVictim->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_MECHANIC_RESISTANCE, spell->Mechanic);
    // Apply mod
    modHitChance -= resist_mech;
    //DEBUG_UNIT(this, DEBUG_SPELL_COMPUTE_RESISTS, "SPELL_AURA_MOD_MECHANIC_RESISTANCE (- %i) : %f", resist_mech, modHitChance);

    // Chance resist debuff
    modHitChance -= pVictim->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_DEBUFF_RESISTANCE, int32(spell->Dispel));
    //DEBUG_UNIT(this, DEBUG_SPELL_COMPUTE_RESISTS, "SPELL_AURA_MOD_DEBUFF_RESISTANCE (- %i) : %f", pVictim->GetTotalAuraModifierByMiscValue(SPELL_AURA_MOD_DEBUFF_RESISTANCE, int32(spell->Dispel)), modHitChance);

    // Increase hit chance from attacker SPELL_AURA_MOD_SPELL_HIT_CHANCE and attacker ratings
    if (Unit* pUnit = ToUnit())
    {
        modHitChance += int32(pUnit->m_modSpellHitChance);
        if (Creature const* creature = pUnit->ToCreature())
        {
            Totem const* totem = creature->IsTotem() ? creature->ToTotem() : nullptr;
            Unit* owner = totem && totem->GetTotemType() != TOTEM_STATUE ? pUnit->GetOwner() : nullptr;
            Player const* playerOwner = owner ? owner->ToPlayer() : nullptr;

            if (playerOwner && playerOwner->GetClass() == CLASS_SHAMAN &&
                playerOwner->GetTotem(TOTEM_SLOT_FIRE) == totem &&
                (spell->GetSpellSchoolMask() & SPELL_SCHOOL_MASK_FIRE) &&
                spell->HasEffect(SPELL_EFFECT_SCHOOL_DAMAGE))
            {
                modHitChance += int32(playerOwner->m_modSpellHitChance);
            }
        }

        if (pUnit->IsPet())
        {
            if (Unit* owner = pUnit->GetOwner())
            {
                Unit::AuraList const& petSpellHitAuras = pUnit->GetAurasByType(SPELL_AURA_MOD_PET_SPELL_HIT_PERCENT_OF_OWNER);
                for (const auto aura : petSpellHitAuras)
                    modHitChance += owner->m_modSpellHitChance * aura->GetModifier()->m_amount / 100.0f;

                Unit::AuraList const& ownerPetSpellHitAuras = owner->GetAurasByType(SPELL_AURA_MOD_PET_SPELL_HIT_PERCENT_OF_OWNER);
                for (const auto aura : ownerPetSpellHitAuras)
                    modHitChance += owner->m_modSpellHitChance * aura->GetModifier()->m_amount / 100.0f;
            }
        }
    }
    //DEBUG_UNIT(this, DEBUG_SPELL_COMPUTE_RESISTS, "SPELL_AURA_MOD_SPELL_HIT_CHANCE (+ %i) : %f", int32(m_modSpellHitChance), modHitChance);

    // Nostalrius: sorts binaires.
    if (spell->IsBinary())
    {
        // Get base victim resistance for school
        float resistModHitChance = GetSpellResistChance(pVictim, schoolMask, false);
        modHitChance *= (1 - resistModHitChance);
        //DEBUG_UNIT(this, DEBUG_SPELL_COMPUTE_RESISTS, "x %f : HitChance = %f", (1 - resistModHitChance), modHitChance);
    }

    int32 HitChance = modHitChance * 100;
    if (HitChance < 100) HitChance = 100;
    if (HitChance > 9900) HitChance = 9900;
    return HitChance;
}

float WorldObject::GetSpellResistChance(Unit const* victim, uint32 schoolMask, bool innateResists) const
{
    Unit const* pUnit = ToUnit();
    // Get base victim resistance for school
    float const baseResistance = victim->GetResistance(GetFirstSchoolInMask(SpellSchoolMask(schoolMask)));
    // Get attacker spell penetration from SPELL_AURA_MOD_TARGET_RESISTANCE aura
    float const selfResistance = pUnit ? pUnit->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, schoolMask) : 0;

    float resistModHitChance = baseResistance + selfResistance;

    if ((resistModHitChance < 0.0f) && (baseResistance >= 0.0f))
        resistModHitChance = 0.0f;

    // Magic vulnerability calculation
    if (resistModHitChance < 0.0f)
    {
        // Victim's level based skill, penalize when calculating for low levels (< 20):
        const float skill = std::max(GetSkillMaxForLevel(victim), uint16(100));
        // Convert resistance value to vulnerability percentage through comparision with skill
        resistModHitChance = (float(resistModHitChance) / skill) * 100;
        return (resistModHitChance * 0.01f);
    }

    // Turtle: no resist chance for holy school
    if (schoolMask & SPELL_SCHOOL_MASK_HOLY)
        return 0.0f;

    uint32 const uiLevel = GetLevel();

    // Computing innate resists, resistance bonus when attacking a creature higher level. Not affected by modifiers.
    if (innateResists && victim->GetTypeId() == TYPEID_UNIT)
    {
        int32 leveldiff = int32(victim->GetLevelForTarget(this)) - int32(GetLevelForTarget(victim));
        resistModHitChance += int32((8.0f * leveldiff) * uiLevel / 63.0f);
    }
    resistModHitChance *= (float)(0.15f / uiLevel);

    if (resistModHitChance < 0.0f)
        resistModHitChance = 0.0f;
    if (resistModHitChance > 0.75f)
        resistModHitChance = 0.75f;

    return resistModHitChance;
}

void WorldObject::SendSpellMiss(Unit *target, uint32 spellID, SpellMissInfo missInfo) const
{
    WorldPacket data(SMSG_SPELLLOGMISS, (4 + 8 + 1 + 4 + 8 + 1));
    data << uint32(spellID);
    data << GetObjectGuid();
    data << uint8(0);                                       // unk8
    data << uint32(1);                                      // target count
    // for(i = 0; i < target count; ++i)
    data << target->GetObjectGuid();                        // target GUID
    data << uint8(missInfo);
    // Nostalrius: + 2 * float if unk8=1
    // end loop
    SendObjectMessageToSet(&data, true);
}

void WorldObject::SendSpellDamageResist(Unit* target, uint32 spellId) const
{
    WorldPacket data(SMSG_PROCRESIST, 8 + 8 + 4 + 1);
    data << GetObjectGuid();
    data << target->GetObjectGuid();
    data << uint32(spellId);
    data << uint8(0); // bool - log format: 0-default, 1-debug
    SendMessageToSet(&data, true);
}

void WorldObject::SendSpellOrDamageImmune(Unit* target, uint32 spellID) const
{
    WorldPacket data(SMSG_SPELLORDAMAGE_IMMUNE, (8 + 8 + 4 + 1));
    data << GetObjectGuid();
    data << target->GetObjectGuid();
    data << uint32(spellID);
    data << uint8(0);
    SendMessageToSet(&data, true);
}

uint32 WorldObject::SpellCriticalDamageBonus(SpellEntry const *spellProto, uint32 damage, Unit *pVictim, Spell* spell)
{
    // Calculate critical bonus
    int32 crit_bonus;
    switch (spellProto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:                      // for melee based spells is 100%
        case SPELL_DAMAGE_CLASS_RANGED:
            crit_bonus = damage;
            break;
        default:
            crit_bonus = damage / 2;                        // for spells is 50%
            break;
    }

    Unit const* pUnit = ToUnit();

    // adds additional damage to crit_bonus (from talents)
    if (pUnit)
    {
        if (Player* modOwner = pUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CRIT_DAMAGE_BONUS, crit_bonus, spell);
    }


    if (!pVictim)
        return damage += crit_bonus;

    uint32 creatureTypeMask = pVictim->GetCreatureTypeMask();

    float critPctDamageMod = pUnit ? pUnit->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_PERCENT_VERSUS, creatureTypeMask) : 1;
    critPctDamageMod *= pVictim->GetTotalAuraMultiplier(SPELL_AURA_MOD_CRIT_DAMAGE_BONUS_TAKEN);

    damage = (damage + crit_bonus) * critPctDamageMod;

    return damage;
}

uint32 WorldObject::SpellCriticalHealingBonus(SpellEntry const *spellProto, uint32 damage, Unit const* pVictim) const
{
    // Calculate critical bonus
    int32 crit_bonus;
    switch (spellProto->DmgClass)
    {
        case SPELL_DAMAGE_CLASS_MELEE:                      // for melee based spells is 100%
        case SPELL_DAMAGE_CLASS_RANGED:
            // TODO: write here full calculation for melee/ranged spells
            crit_bonus = damage;
            break;
        default:
            crit_bonus = damage / 2;                        // for spells is 50%
            break;
    }

    if (pVictim)
    {
        if (Unit const* pUnit = ToUnit())
        {
            uint32 creatureTypeMask = pVictim->GetCreatureTypeMask();
            crit_bonus = int32(crit_bonus * pUnit->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_CRIT_PERCENT_VERSUS, creatureTypeMask));
        }
    }

    if (crit_bonus > 0)
        damage += crit_bonus;

    return damage;
}

int32 WorldObject::DealHeal(Unit *pVictim, uint32 addhealth, SpellEntry const *spellProto, bool critical)
{
    Unit* pUnit = ToUnit();

    // Script Event HealedBy
    if (pVictim->AI() && pUnit)
        pVictim->AI()->HealedBy(pUnit, addhealth);

    if (pUnit)
    {
        ScriptRegistry<UnitScript>::ForEachEnabledHook(UNITHOOK_ON_HEAL, [&](UnitScript* script)
        {
            script->OnHeal(pUnit, pVictim, addhealth);
        });
    }

    int32 gain = pVictim->ModifyHealth(int32(addhealth));

    WorldObject* pHealer = this;

    if (IsCreature() && ((Creature*)this)->IsTotem() && ((Totem*)this)->GetTotemType() != TOTEM_STATUE)
        pHealer = pUnit->GetOwner();

    if (IsPlayer() || pVictim->IsPlayer())
        pHealer->SendHealSpellLog(pVictim, spellProto->Id, addhealth, critical);

    return gain;
}

void WorldObject::SendHealSpellLog(Unit const* pVictim, uint32 SpellID, uint32 Damage, bool critical) const
{
    // we guess size
    WorldPacket data(SMSG_SPELLHEALLOG, (8 + 8 + 4 + 4 + 1));
    data << pVictim->GetPackGUID();
    data << GetPackGUID();
    data << uint32(SpellID);
    data << uint32(Damage);
    data << uint8(critical ? 1 : 0);
    // data << uint8(0);                                    // [-ZERO]
    SendMessageToSet(&data, true);
}

void WorldObject::EnergizeBySpell(Unit* pVictim, uint32 spellId, uint32 amount, Powers powerType)
{
    uint32 logAmount = amount;
    if (powerType == POWER_MANA && logAmount)
    {
        int32 const manaGainMod = pVictim->GetTotalAuraModifier(SPELL_AURA_MOD_MANA_GAIN_PERCENT);
        if (manaGainMod)
        {
            float const multiplier = std::max(0.0f, (100.0f + manaGainMod) / 100.0f);
            logAmount = uint32(float(logAmount) * multiplier);
        }
    }

    SendEnergizeSpellLog(pVictim, spellId, logAmount, powerType);

    // Turtle: threat from power gains as per RMJ's explanations
    if (Unit* pUnit = ToUnit())
    {
        float multiplier;
        switch (powerType)
        {
            case POWER_ENERGY:
                multiplier = 5.0f;
                break;
            default:
                multiplier = 0.5f;
                break;
        }
        pVictim->GetHostileRefManager().threatAssist(pUnit, logAmount * multiplier, sSpellMgr.GetSpellEntry(spellId));
    }

    // needs to be called after sending spell log
    pVictim->ModifyPower(powerType, amount);
}

void WorldObject::SendEnergizeSpellLog(Unit const* pVictim, uint32 SpellID, uint32 Damage, Powers powertype) const
{
    WorldPacket data(SMSG_SPELLENERGIZELOG, (8 + 8 + 4 + 4 + 4 + 1));
    data << pVictim->GetPackGUID();
    data << GetPackGUID();
    data << uint32(SpellID);
    data << uint32(powertype);
    data << uint32(Damage);
    SendMessageToSet(&data, true);
}

void WorldObject::SendSpellNonMeleeDamageLog(SpellNonMeleeDamage *log) const
{
    WorldPacket data(SMSG_SPELLNONMELEEDAMAGELOG, (16 + 4 + 4 + 1 + 4 + 4 + 1 + 1 + 4 + 4 + 1)); // we guess size
    data << log->target->GetPackGUID();
    data << log->attacker->GetPackGUID();
    data << uint32(log->SpellID);
    data << uint32(log->damage);                            // damage amount
    data << uint8(log->school);                             // damage school
    data << uint32(log->absorb);                            // AbsorbedDamage
    data << int32(log->resist);                             // resist
    data << uint8(log->periodicLog);                        // if 1, then client show spell name (example: %s's ranged shot hit %s for %u school or %s suffers %u school damage from %s's spell_name
    data << uint8(false);                                   // unused
    data << uint32(log->blocked);                           // blocked
    data << uint32(log->HitInfo);
    data << uint8(0);                                       // flag to use extend data
    SendMessageToSet(&data, true);
}

void WorldObject::SendSpellNonMeleeDamageLog(Unit* target, uint32 spellID, uint32 damage, SpellSchoolMask damageSchoolMask, uint32 absorbedDamage, int32 resist, bool isPeriodic, uint32 blocked, bool criticalHit, bool split)
{
    SpellNonMeleeDamage log(this, target, spellID, GetFirstSchoolInMask(damageSchoolMask));
    log.damage = damage;
    log.damage += (resist < 0 ? uint32(std::abs(resist)) : 0);
    log.damage -= (absorbedDamage + (resist > 0 ? uint32(resist) : 0) + blocked);
    log.absorb = absorbedDamage;
    log.resist = resist;
    log.periodicLog = isPeriodic;
    log.blocked = blocked;
    log.HitInfo = 0;
    if (criticalHit)
        log.HitInfo |= SPELL_HIT_TYPE_CRIT;
    if (split)
        log.HitInfo |= SPELL_HIT_TYPE_SPLIT;
    SendSpellNonMeleeDamageLog(&log);
}

SpellSchoolMask WorldObject::GetMeleeDamageSchoolMask() const
{
    return SPELL_SCHOOL_MASK_NORMAL;
}

uint32 WorldObject::CalcArmorReducedDamage(Unit* pVictim, const uint32 damage) const
{
    uint32 newdamage = 0;
    float armor = (float)pVictim->GetArmor();
    Unit const* pUnit = ToUnit();

    // Ignore enemy armor by SPELL_AURA_MOD_TARGET_RESISTANCE aura
    if (pUnit)
    {
        armor += pUnit->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_TARGET_RESISTANCE, SPELL_SCHOOL_MASK_NORMAL);
        armor *= std::max(0.0f, (100.0f - pUnit->GetTotalAuraModifier(SPELL_AURA_MOD_IGNORE_TARGET_ARMOR)) / 100.0f);
    }

    if (armor < 0.0f)
        armor = 0.0f;

    float tmpvalue = 0.1f * armor / (8.5f * float(GetLevel()) + 40.0f);
    tmpvalue = tmpvalue / (1.0f + tmpvalue);

    if (tmpvalue < 0.0f)
        tmpvalue = 0.0f;
    if (tmpvalue > 0.75f)
        tmpvalue = 0.75f;

    newdamage = uint32(roundf(damage - (damage * tmpvalue)));

    return (newdamage > 1) ? newdamage : 1;
}

int32 WorldObject::CalculateSpellDamage(Unit const* target, SpellEntry const* spellProto, SpellEffectIndex effect_index, int32 const* effBasePoints, Spell* spell) const
{
    Unit const* pUnit = ToUnit();
    Player const* pPlayer = ToPlayer();

    uint8 comboPoints = pPlayer ? pPlayer->GetComboPoints() : 0;

    int32 level = GetLevel();

    if (level > (int32)spellProto->maxLevel && spellProto->maxLevel > 0)
        level = (int32)spellProto->maxLevel;
    else if (level < (int32)spellProto->baseLevel)
        level = (int32)spellProto->baseLevel;
    level -= (int32)spellProto->spellLevel;

    int32 baseDice = int32(spellProto->EffectBaseDice[effect_index]);
    float basePointsPerLevel = spellProto->EffectRealPointsPerLevel[effect_index];
    float randomPointsPerLevel = spellProto->EffectDicePerLevel[effect_index];
    int32 basePoints = effBasePoints
                       ? *effBasePoints - baseDice
                       : spellProto->EffectBasePoints[effect_index];

    basePoints += int32(level * basePointsPerLevel);
    int32 randomPoints = int32(spellProto->EffectDieSides[effect_index] + level * randomPointsPerLevel);
    float comboDamage = spellProto->EffectPointsPerComboPoint[effect_index];

    switch (randomPoints)
    {
        case 0:
        case 1:
            basePoints += baseDice;
            break;              // range 1..1
        default:
        {
            // range can have positive (1..rand) and negative (rand..1) values, so order its for irand
            int32 randvalue = baseDice >= randomPoints
                              ? irand(randomPoints, baseDice)
                              : irand(baseDice, randomPoints);

            if (IsPlayer() && ToPlayer()->HasOption(PLAYER_CHEAT_NO_DAMAGE_RNG))
                randvalue = 0;

            basePoints += randvalue;
            break;
        }
    }

    int32 value = basePoints;

    // random damage
    if (comboDamage != 0 && pPlayer && target && (target->GetObjectGuid() == pPlayer->GetComboTargetGuid()))
        value += (int32)(comboDamage * comboPoints);

    if (pUnit)
    {
        if (Player* modOwner = pUnit->GetSpellModOwner())
        {
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_ALL_EFFECTS, value, spell);

            if (effect_index == EFFECT_INDEX_2 && spellProto->IsFitToFamily<SPELLFAMILY_ROGUE, CF_ROGUE_HEMORRHAGE>())
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_EFFECT_PAST_FIRST, value, spell);

            // Apply speed aura mods at cast time.
            // Fixes Curse of Exhaustion not removing Amplify Curse.
            switch (spellProto->EffectApplyAuraName[effect_index])
            {
                case SPELL_AURA_MOD_INCREASE_SPEED:
                case SPELL_AURA_MOD_SPEED_ALWAYS:
                case SPELL_AURA_MOD_SPEED_NOT_STACK:
                case SPELL_AURA_MOD_DECREASE_SPEED:
                    modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_SPEED, value, spell);
                    break;
            }
        }
    }

    if (spellProto->Attributes & SPELL_ATTR_LEVEL_DAMAGE_CALCULATION && spellProto->spellLevel &&
            spellProto->Effect[effect_index] != SPELL_EFFECT_WEAPON_PERCENT_DAMAGE &&
            spellProto->Effect[effect_index] != SPELL_EFFECT_KNOCK_BACK &&
            (spellProto->Effect[effect_index] != SPELL_EFFECT_APPLY_AURA || spellProto->EffectApplyAuraName[effect_index] != SPELL_AURA_MOD_DECREASE_SPEED))
        value = int32(value * 0.25f * exp(GetLevel() * (70 - spellProto->spellLevel) / 1000.0f));

    return value;
}

void WorldObject::CalculateSpellDamage(SpellNonMeleeDamage* damageInfo, int32 damage, SpellEntry const* spellInfo, SpellEffectIndex effectIndex, WeaponAttackType attackType, Spell* spell, bool crit)
{
    SpellSchoolMask damageSchoolMask = GetSchoolMask(damageInfo->school);
    Unit *pVictim = damageInfo->target;
    if (!pVictim)
        return;

    if (damage < 0)
        return;

    if (!pVictim->IsAlive())
        return;

    // damage bonus (per damage class)
    switch (spellInfo->DmgClass)
    {
        // Melee and Ranged Spells
        case SPELL_DAMAGE_CLASS_RANGED:
        case SPELL_DAMAGE_CLASS_MELEE:
        {
            //Calculate damage bonus
            damage = MeleeDamageBonusDone(pVictim, damage, attackType, spellInfo, effectIndex, SPELL_DIRECT_DAMAGE, 1, spell);
            damage = pVictim->MeleeDamageBonusTaken(this, damage, attackType, spellInfo, effectIndex, SPELL_DIRECT_DAMAGE, 1, spell);

            // if crit add critical bonus
            if (crit && !spellInfo->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS))
            {
                damageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;
                damage = SpellCriticalDamageBonus(spellInfo, damage, pVictim, spell);
            }
            break;
        }
        // Magical Attacks
        case SPELL_DAMAGE_CLASS_NONE:
        case SPELL_DAMAGE_CLASS_MAGIC:
        {
            // Calculate damage bonus
            damage = SpellDamageBonusDone(pVictim, spellInfo, effectIndex, damage, SPELL_DIRECT_DAMAGE, 1, spell);
            damage = pVictim->SpellDamageBonusTaken(this, spellInfo, effectIndex, damage, SPELL_DIRECT_DAMAGE, 1, spell);

            // If crit add critical bonus
            if (crit && !spellInfo->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS))
            {
                damageInfo->HitInfo |= SPELL_HIT_TYPE_CRIT;
                damage = SpellCriticalDamageBonus(spellInfo, damage, pVictim, spell);
            }
            break;
        }
    }

    // damage mitigation
    if (damage > 0)
    {
        // physical damage => armor
        if (damageSchoolMask & SPELL_SCHOOL_MASK_NORMAL && !(spellInfo->Custom & SPELL_CUSTOM_IGNORE_ARMOR))
            damage = CalcArmorReducedDamage(pVictim, damage);
    }
    else
        damage = 0;
    damageInfo->damage = damage;
}

/**
 * Calculates caster part of melee damage bonuses,
 * also includes different bonuses dependent from target auras
 */
uint32 WorldObject::MeleeDamageBonusDone(Unit* pVictim, uint32 pdamage, WeaponAttackType attType, SpellEntry const* spellProto, SpellEffectIndex effectIndex, DamageEffectType damagetype, uint32 stack, Spell* spell, bool flat)
{
    if (!pVictim || pdamage == 0)
        return pdamage;

    // Some spells don't benefit from done mods
    if (spellProto && spellProto->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS))
        return pdamage;

    // differentiate for weapon damage based spells
    bool isWeaponDamageBasedSpell = !(spellProto && (damagetype == DOT || spellProto->HasEffect(SPELL_EFFECT_SCHOOL_DAMAGE)));
    Item*  pWeapon          = GetTypeId() == TYPEID_PLAYER ? ((Player*)this)->GetWeaponForAttack(attType, true, false) : nullptr;
    uint32 creatureTypeMask = pVictim->GetCreatureTypeMask();
    uint32 schoolMask       = spellProto ? spellProto->GetSpellSchoolMask() : GetMeleeDamageSchoolMask();

    // FLAT damage bonus auras
    // =======================
    int32 DoneFlat  = 0;
    int32 APbonus   = 0;

    Unit* pUnit = ToUnit();

    // ..done flat, already included in weapon damage based spells
    if (pUnit && !isWeaponDamageBasedSpell)
    {
        Unit::AuraList const& mModDamageDone = pUnit->GetAurasByType(SPELL_AURA_MOD_DAMAGE_DONE);
        for (const auto i : mModDamageDone)
        {
            if (i->GetModifier()->m_miscvalue & schoolMask &&                         // schoolmask has to fit with the intrinsic spell school
                    (i->GetModifier()->m_miscvalue & GetMeleeDamageSchoolMask() ||    // AND schoolmask has to fit with weapon damage school (essential for non-physical spells)
                     spellProto->DmgClass == SPELL_DAMAGE_CLASS_RANGED) &&               // Some ranged physical attacks use magic damage, ex. Arcane Shot
                    ((i->GetSpellProto()->EquippedItemClass == -1) ||                     // general, weapon independent
                     (pWeapon && pWeapon->IsFitToSpellRequirements(i->GetSpellProto()))))  // OR used weapon fits aura requirements
                DoneFlat += i->GetModifier()->m_amount;
        }

        // Pets just add their bonus damage to their melee damage
        if (GetTypeId() == TYPEID_UNIT && ((Creature*)this)->IsPet())
            DoneFlat += ((Pet*)this)->GetBonusDamage();
    }

    // ..done flat (by creature type mask)
    if (pUnit)
        DoneFlat += pUnit->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_DAMAGE_DONE_CREATURE, creatureTypeMask);

    // ..done flat (base at attack power for marked target and base at attack power for creature type)
    if (attType == RANGED_ATTACK)
    {
        APbonus += pVictim->GetTotalAuraModifier(SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS);
        if (pUnit)
            APbonus += pUnit->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS, creatureTypeMask);
    }
    else
    {
        APbonus += pVictim->GetTotalAuraModifier(SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS);
        if (pUnit)
            APbonus += pUnit->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS, creatureTypeMask);
    }

    // PERCENT damage auras
    // ====================
    float DonePercent   = 1.0f;

    // ..done pct, already included in weapon damage based spells
    if (pUnit && !isWeaponDamageBasedSpell)
    {
        Unit::AuraList const& mModDamagePercentDone = pUnit->GetAurasByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        for (const auto i : mModDamagePercentDone)
        {
            if (i->GetModifier()->m_miscvalue & schoolMask &&                         // schoolmask has to fit with the intrinsic spell school
                    i->GetModifier()->m_miscvalue & GetMeleeDamageSchoolMask() &&         // AND schoolmask has to fit with weapon damage school (essential for non-physical spells)
                    ((i->GetSpellProto()->EquippedItemClass == -1) ||                     // general, weapon independent
                     (pWeapon && pWeapon->IsFitToSpellRequirements(i->GetSpellProto()))))  // OR used weapon fits aura requirements
                DonePercent *= (i->GetModifier()->m_amount + 100.0f) / 100.0f;
        }

        if (pUnit && attType == OFF_ATTACK)
            DonePercent *= pUnit->GetModifierValue(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT);                    // no school check required
    }

    // Pet happiness increases damage of Hunter pet melee spells
    if (IsPet() && ((Pet*)this)->getPetType() == HUNTER_PET)
    {
        if (Pet* pet = ((Pet*)this))
        {
            switch (pet->GetHappinessState())
            {
                case HAPPY:     DonePercent *= 1.25; break;
                case CONTENT:   break;
                case UNHAPPY:   DonePercent *= 0.75; break;
            }
        }
    }

    // ..done pct (by creature type mask)
    if (pUnit)
        DonePercent *= pUnit->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS, creatureTypeMask);

    if (pUnit && pUnit->IsPet())
    {
        if (Unit* owner = pUnit->GetOwner())
        {
            Unit::AuraList const& petDamageTakenAuras = pVictim->GetAurasByType(SPELL_AURA_MOD_DAMAGE_TAKEN_FROM_CASTER_PET);
            for (const auto aura : petDamageTakenAuras)
            {
                if (aura->GetCasterGuid() == owner->GetObjectGuid())
                    DonePercent *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
            }
        }
    }

    // final calculation
    // =================

    float DoneTotal = 0.0f;

    // scaling of non weapon based spells
    if (!isWeaponDamageBasedSpell)
    {
        // apply ap bonus and benefit affected by spell power implicit coeffs and spell level penalties
        DoneTotal = SpellBonusWithCoeffs(spellProto, effectIndex, DoneTotal, DoneFlat, APbonus, damagetype, true, this, spell);
    }
    // weapon damage based spells
    else if (APbonus || DoneFlat)
    {
        bool normalized = spellProto ? spellProto->HasEffect(SPELL_EFFECT_NORMALIZED_WEAPON_DMG) : false;
        DoneTotal += int32(APbonus / 14.0f * GetAPMultiplier(attType, normalized));

        // for weapon damage based spells we still have to apply damage done percent mods
        // (that are already included into pdamage) to not-yet included DoneFlat
        // e.g. from doneVersusCreature, apBonusVs...
        UnitMods unitMod;
        switch (attType)
        {
            default:
            case BASE_ATTACK:
                unitMod = UNIT_MOD_DAMAGE_MAINHAND;
                break;
            case OFF_ATTACK:
                unitMod = UNIT_MOD_DAMAGE_OFFHAND;
                break;
            case RANGED_ATTACK:
                unitMod = UNIT_MOD_DAMAGE_RANGED;
                break;
        }

        DoneTotal += DoneFlat;

        if (pUnit)
            DoneTotal *= pUnit->GetModifierValue(unitMod, TOTAL_PCT);
    }

    if (!flat)
        DoneTotal = 0.0f;

    float tmpDamage = float(int32(pdamage) + DoneTotal * int32(stack)) * DonePercent;

    // apply spellmod to Done damage
    if (spellProto && pUnit)
    {
        if (Player* modOwner = pUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, tmpDamage, spell);
    }

    // bonus result can be negative
    return tmpDamage > 0 ? uint32(tmpDamage) : 0;
}

/**
 * Calculates caster part of healing spell bonuses,
 * also includes different bonuses dependent from target auras
 */
uint32 WorldObject::SpellHealingBonusDone(Unit* pVictim, SpellEntry const* spellProto, SpellEffectIndex effectIndex, int32 healamount, DamageEffectType damagetype, uint32 stack, Spell* spell)
{
    Unit* pUnit = ToUnit();

    // For totems get healing bonus from owner (statue isn't totem in fact)
    if (GetTypeId() == TYPEID_UNIT && ((Creature*)this)->IsTotem() && ((Totem*)this)->GetTotemType() != TOTEM_STATUE)
        if (Unit* owner = pUnit->GetOwner())
            return owner->SpellHealingBonusDone(pVictim, spellProto, effectIndex, healamount, damagetype, stack, spell);

    // No heal amount for this class spells
    if (((spellProto->DmgClass == SPELL_DAMAGE_CLASS_NONE) && spellProto->HasAttribute(SPELL_ATTR_PASSIVE)) ||
        (spellProto->Custom & SPELL_CUSTOM_FIXED_DAMAGE) || spellProto->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS))
    {
        //DEBUG_UNIT(this, DEBUG_SPELLS_DAMAGE, "SpellHealingBonusDone[spell=%u]: has fixed damage (SPELL_DAMAGE_CLASS_NONE)", spellProto->Id);
        return healamount < 0 ? 0 : healamount;
    }

    // Healing Done
    // Done total percent damage auras
    float  DoneTotalMod = 1.0f;
    int32  DoneTotal = 0;

    if (pUnit)
    {
        // Healing done percent
        Unit::AuraList const& mHealingDonePct = pUnit->GetAurasByType(SPELL_AURA_MOD_HEALING_DONE_PERCENT);
        for (const auto i : mHealingDonePct)
            DoneTotalMod *= (100.0f + i->GetModifier()->m_amount) / 100.0f;

        // done scripted mod (take it from owner)
        Unit *owner = pUnit->GetOwner();
        if (!owner)
            owner = pUnit;

        Unit::AuraList const& mOverrideClassScript = owner->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (const auto i : mOverrideClassScript)
        {
            if (!i->isAffectedOnSpell(spellProto))
                continue;
            switch (i->GetModifier()->m_miscvalue)
            {
                case 4415: // Increased Rejuvenation Healing
                    DoneTotal += i->GetModifier()->m_amount / 4; // 4 ticks
                    break;
                case 3736: // Hateful Totem of the Third Wind / Increased Lesser Healing Wave / Savage Totem of the Third Wind
                    DoneTotal += i->GetModifier()->m_amount;
                    break;
                case 5069: // Spiritual Healing
                    DoneTotalMod *= (100.0f + i->GetModifier()->m_amount) / 100.0f;
                    break;
                case 5065: // Empowered Recovery
                {
                    if (!pVictim)
                        break;

                    Unit::AuraList const& periodicHeals = pVictim->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
                    for (Aura const* aura : periodicHeals)
                    {
                        if (aura->GetSpellProto()->IsFitToFamily<SPELLFAMILY_PRIEST, CF_PRIEST_RENEW>())
                        {
                            DoneTotalMod *= (100.0f + i->GetModifier()->m_amount) / 100.0f;
                            break;
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }

    // Done fixed damage bonus auras
    int32 DoneAdvertisedBenefit  = SpellBaseHealingBonusDone(spellProto->GetSpellSchoolMask());

    // apply ap bonus and benefit affected by spell power implicit coeffs and spell level penalties
    DoneTotal = SpellBonusWithCoeffs(spellProto, effectIndex, DoneTotal, DoneAdvertisedBenefit, 0, damagetype, true, this, spell);

    // use float as more appropriate for negative values and percent applying
    float heal = (healamount + DoneTotal * int32(stack)) * DoneTotalMod;

    if (pUnit)
    {
        // apply spellmod to Done amount
        if (Player* modOwner = pUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, heal, spell);
    }

    //DEBUG_UNIT(this, DEBUG_SPELLS_DAMAGE, "SpellHealingBonusDone[spell=%u]: (base=%u + %i) * %f. HealingPwr=%i", spellProto->Id, healamount, DoneTotal, DoneTotalMod, DoneAdvertisedBenefit);
    return heal < 0 ? 0 : uint32(heal);
}

int32 WorldObject::SpellBaseHealingBonusDone(SpellSchoolMask schoolMask)
{
    int32 AdvertisedBenefit = 0;

    if (Unit* pUnit = ToUnit())
    {
        Unit::AuraList const& mHealingDone = pUnit->GetAurasByType(SPELL_AURA_MOD_HEALING_DONE);
        for (const auto i : mHealingDone)
            if ((i->GetModifier()->m_miscvalue & schoolMask) != 0)
                AdvertisedBenefit += i->GetModifier()->m_amount;

        Unit::AuraList const& healingFromArmor = pUnit->GetAurasByType(SPELL_AURA_MOD_SPELL_HEALING_OF_ARMOR_PERCENT);
        for (const auto aura : healingFromArmor)
            if ((aura->GetModifier()->m_miscvalue & schoolMask) != 0)
                AdvertisedBenefit += int32(float(pUnit->GetArmor()) * aura->GetModifier()->m_amount / 100.0f);

        if (pUnit->IsPet())
        {
            if (Unit* owner = pUnit->GetOwner())
            {
                Unit::AuraList const& petSpellPowerAuras = pUnit->GetAurasByType(SPELL_AURA_MOD_PET_SPELL_DAMAGE_PERCENT_OF_OWNER);
                for (const auto aura : petSpellPowerAuras)
                    if ((aura->GetModifier()->m_miscvalue & schoolMask) != 0)
                        AdvertisedBenefit += int32(owner->GetTotalAttackPowerValue(RANGED_ATTACK) * aura->GetModifier()->m_amount / 100.0f);

                Unit::AuraList const& ownerPetSpellPowerAuras = owner->GetAurasByType(SPELL_AURA_MOD_PET_SPELL_DAMAGE_PERCENT_OF_OWNER);
                for (const auto aura : ownerPetSpellPowerAuras)
                    if ((aura->GetModifier()->m_miscvalue & schoolMask) != 0)
                        AdvertisedBenefit += int32(owner->GetTotalAttackPowerValue(RANGED_ATTACK) * aura->GetModifier()->m_amount / 100.0f);
            }
        }

        // Healing bonus of spirit, intellect and strength
        if (GetTypeId() == TYPEID_PLAYER)
        {
            // Healing bonus from stats
            Unit::AuraList const& mHealingDoneOfStatPercent = pUnit->GetAurasByType(SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT);
            for (const auto i : mHealingDoneOfStatPercent)
            {
                // 1.12.* have only 1 stat type support
                Stats usedStat = STAT_SPIRIT;
                AdvertisedBenefit += int32(pUnit->GetStat(usedStat) * i->GetModifier()->m_amount / 100.0f);
            }
        }
    }

    return AdvertisedBenefit;
}

/**
 * Calculates caster part of spell damage bonuses,
 * also includes different bonuses dependent from target auras
 */
uint32 WorldObject::SpellDamageBonusDone(Unit* pVictim, SpellEntry const* spellProto, SpellEffectIndex effectIndex, uint32 pdamage, DamageEffectType damagetype, uint32 stack, Spell* spell)
{
    if (!spellProto || !pVictim || damagetype == DIRECT_DAMAGE)
        return pdamage;

    if (spellProto->Custom & SPELL_CUSTOM_FIXED_DAMAGE)
        return pdamage;
    if (spellProto->HasAttribute(SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS))
        return pdamage;

    // Ignite damage already includes modifiers
    if (spellProto->IsFitToFamily<SPELLFAMILY_MAGE, CF_MAGE_IGNITE>())
        return pdamage;

    Unit* pUnit = ToUnit();

    // For totems get damage bonus from owner (statue isn't totem in fact)
    if (pUnit && GetTypeId() == TYPEID_UNIT && ((Creature*)this)->IsTotem() && ((Totem*)this)->GetTotemType() != TOTEM_STATUE)
    {
        if (Unit* owner = pUnit->GetOwner())
            return owner->SpellDamageBonusDone(pVictim, spellProto, effectIndex, pdamage, damagetype, stack, spell);
    }

    float DoneTotalMod = 1.0f;
    int32 DoneTotal = 0;
    int32 DoneTotalNoCoeff = 0;
    Item*  pWeapon = GetTypeId() == TYPEID_PLAYER ? ((Player*)this)->GetWeaponForAttack(BASE_ATTACK, true, false) : nullptr;

    // Creature damage
    if (GetTypeId() == TYPEID_UNIT && !((Creature*)this)->IsPet())
        DoneTotalMod *= Creature::_GetSpellDamageMod(((Creature*)this)->GetCreatureInfo()->rank);

    if (pUnit)
    {
        Unit::AuraList const& mModDamagePercentDone = pUnit->GetAurasByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        for (const auto i : mModDamagePercentDone)
        {
            if ((i->GetModifier()->m_miscvalue & spellProto->GetSpellSchoolMask()) &&
                // no benefit for wands
                i->GetSpellProto()->EquippedItemClass == -1 &&
                i->GetSpellProto()->EquippedItemInventoryTypeMask == 0 &&
                !(spellProto->EquippedItemClass == ITEM_CLASS_WEAPON && spellProto->EquippedItemSubClassMask & (1 << ITEM_SUBCLASS_WEAPON_WAND)))
            {
                DoneTotalMod *= (i->GetModifier()->m_amount + 100.0f) / 100.0f;
            }
            // Paladin seals benefit from weapon modifiers
            else if (i->GetModifier()->m_miscvalue & GetMeleeDamageSchoolMask() &&
                spellProto->SpellFamilyName == SPELLFAMILY_PALADIN && spellProto->IsFitToFamilyMask<CF_PALADIN_SEALS>() &&
                ((i->GetSpellProto()->EquippedItemClass == -1) ||
                (pWeapon && pWeapon->IsFitToSpellRequirements(i->GetSpellProto()))))
            {
                DoneTotalMod *= (i->GetModifier()->m_amount + 100.0f) / 100.0f;
            }
        }
    }

    uint32 creatureTypeMask = pVictim->GetCreatureTypeMask();

    // Add pct bonus from spell damage versus
    if (pUnit)
        DoneTotalMod *= pUnit->GetTotalAuraMultiplierByMiscMask(SPELL_AURA_MOD_DAMAGE_DONE_VERSUS, creatureTypeMask);

    if (pUnit && pUnit->IsPet())
    {
        if (Unit* owner = pUnit->GetOwner())
        {
            Unit::AuraList const& petDamageTakenAuras = pVictim->GetAurasByType(SPELL_AURA_MOD_DAMAGE_TAKEN_FROM_CASTER_PET);
            for (const auto aura : petDamageTakenAuras)
            {
                if (aura->GetCasterGuid() == owner->GetObjectGuid())
                    DoneTotalMod *= (aura->GetModifier()->m_amount + 100.0f) / 100.0f;
            }
        }
    }

    // Add flat bonus from spell damage creature
    if (pUnit)
        DoneTotal += pUnit->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_DAMAGE_DONE_CREATURE, creatureTypeMask);

    // done scripted mod (take it from owner)
    Unit *owner = pUnit ? pUnit->GetOwner() : nullptr;
    if (!owner) owner = pUnit;
    if (owner)
    {
        Unit::AuraList const& mOverrideClassScript = owner->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
        for (const auto i : mOverrideClassScript)
        {
            if (!i->isAffectedOnSpell(spellProto))
                continue;
            switch (i->GetModifier()->m_miscvalue)
            {
                case 5066:
                {
                    if (!owner->HasAura(51578))
                        DoneTotalMod += i->GetModifier()->m_amount / 100.0f;
                    break;
                }
                case 5067: // Trap Mastery
                {
                    DoneTotalMod += i->GetModifier()->m_amount / 100.0f;
                    break;
                }
                case 5068: // Untamed Trapper
                {
                    float const attackPower = owner->GetTotalAttackPowerValue(BASE_ATTACK);
                    switch (spellProto->Id)
                    {
                        case 13797:
                        case 14298:
                        case 14299:
                        case 14300:
                        case 14301:
                            if (effectIndex == EFFECT_INDEX_0)
                                DoneTotalNoCoeff += int32(attackPower / 10.0f);
                            break;
                        case 13812:
                        case 14314:
                        case 14315:
                            if (effectIndex == EFFECT_INDEX_0)
                                DoneTotalNoCoeff += int32(attackPower / 6.5f);
                            else if (effectIndex == EFFECT_INDEX_1)
                                DoneTotalNoCoeff += int32(attackPower / 30.0f);
                            break;
                    }
                    break;
                }
                case 4418: // Increased Shock Damage
                case 4554: // Increased Lightning Damage
                {
                    DoneTotal += i->GetModifier()->m_amount;
                    break;
                }
                case 4555: // Improved Moonfire
                {
                    DoneTotalMod += i->GetModifier()->m_amount / 100.0f;
                    break;
                }
            }
        }
    }

    // Pet happiness increases damage of Hunter pet spells (e.g. Lightning Breath)
    if (IsPet() && ((Pet*)this)->getPetType() == HUNTER_PET)
    {
        if (Pet* pet = ((Pet*)this))
        {
            switch (pet->GetHappinessState())
            {
                case HAPPY:     DoneTotalMod *= 1.25; break;
                case CONTENT:   break;
                case UNHAPPY:   DoneTotalMod *= 0.75; break;
            }
        }
    }

    // Done fixed damage bonus auras
    int32 DoneAdvertisedBenefit = SpellBaseDamageBonusDone(spellProto->GetSpellSchoolMask());

    // Add flat bonus from spell damage versus
    if (pUnit)
        DoneAdvertisedBenefit += pUnit->GetTotalAuraModifierByMiscMask(SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS, creatureTypeMask);

    // Pets just add their bonus damage to their spell damage
    // note that their spell damage is just gain of their own auras
    if (GetTypeId() == TYPEID_UNIT && ((Creature*)this)->IsPet())
        DoneAdvertisedBenefit += ((Pet*)this)->GetBonusDamage();

    // apply ap bonus and benefit affected by spell power implicit coeffs and spell level penalties
    DoneTotal = SpellBonusWithCoeffs(spellProto, effectIndex, DoneTotal, DoneAdvertisedBenefit, 0, damagetype, true, this, spell);

    float tmpDamage = (int32(pdamage) + (DoneTotal + DoneTotalNoCoeff) * int32(stack)) * DoneTotalMod;
    // apply spellmod to Done damage (flat and pct)
    if (pUnit)
    {
        if (Player* modOwner = pUnit->GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, damagetype == DOT ? SPELLMOD_DOT : SPELLMOD_DAMAGE, tmpDamage, spell);
    }

    //DEBUG_UNIT(this, DEBUG_SPELLS_DAMAGE, "SpellDmgBonus[spell=%u]: (base=%u + %i) * %f. SP=%i", spellProto->Id, pdamage, DoneTotal, DoneTotalMod, DoneAdvertisedBenefit);
    return tmpDamage > 0 ? uint32(tmpDamage) : 0;
}

int32 WorldObject::SpellBaseDamageBonusDone(SpellSchoolMask schoolMask)
{
    int32 DoneAdvertisedBenefit = 0;

    if (Unit* pUnit = ToUnit())
    {
        Unit::AuraList const& mDamageDone = pUnit->GetAurasByType(SPELL_AURA_MOD_DAMAGE_DONE);
        for (const auto i : mDamageDone)
        {
            if ((i->GetModifier()->m_miscvalue & schoolMask) != 0 &&
                i->GetSpellProto()->EquippedItemClass == -1 &&                   // -1 == any item class (not wand then)
                i->GetSpellProto()->EquippedItemInventoryTypeMask == 0)          //  0 == any inventory type (not wand then)
                DoneAdvertisedBenefit += i->GetModifier()->m_amount;
        }

        if (pUnit->IsPet())
        {
            if (Unit* owner = pUnit->GetOwner())
            {
                Unit::AuraList const& petSpellPowerAuras = pUnit->GetAurasByType(SPELL_AURA_MOD_PET_SPELL_DAMAGE_PERCENT_OF_OWNER);
                for (const auto aura : petSpellPowerAuras)
                    if ((aura->GetModifier()->m_miscvalue & schoolMask) != 0)
                        DoneAdvertisedBenefit += int32(owner->GetTotalAttackPowerValue(RANGED_ATTACK) * aura->GetModifier()->m_amount / 100.0f);

                Unit::AuraList const& ownerPetSpellPowerAuras = owner->GetAurasByType(SPELL_AURA_MOD_PET_SPELL_DAMAGE_PERCENT_OF_OWNER);
                for (const auto aura : ownerPetSpellPowerAuras)
                    if ((aura->GetModifier()->m_miscvalue & schoolMask) != 0)
                        DoneAdvertisedBenefit += int32(owner->GetTotalAttackPowerValue(RANGED_ATTACK) * aura->GetModifier()->m_amount / 100.0f);
            }
        }

        Unit::AuraList const& spellDamageOfStatAuras = pUnit->GetAurasByType(SPELL_AURA_MOD_SPELL_DAMAGE_OF_INTELLECT_PERCENT);
        for (const auto aura : spellDamageOfStatAuras)
            if ((aura->GetModifier()->m_miscvalue & schoolMask) != 0)
                DoneAdvertisedBenefit += int32(pUnit->GetStat(STAT_INTELLECT) * aura->GetModifier()->m_amount / 100.0f);

        if (GetTypeId() == TYPEID_PLAYER)
        {
            // Damage bonus from stats
            Unit::AuraList const& mDamageDoneOfStatPercent = pUnit->GetAurasByType(SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT);
            for (const auto i : mDamageDoneOfStatPercent)
            {
                if (i->GetModifier()->m_miscvalue & schoolMask)
                {
                    // stat used stored in miscValueB for this aura
                    Stats usedStat = STAT_SPIRIT;
                    DoneAdvertisedBenefit += int32(pUnit->GetStat(usedStat) * i->GetModifier()->m_amount / 100.0f);
                }
            }
        }
    }

    return DoneAdvertisedBenefit;
}

int32 WorldObject::SpellBonusWithCoeffs(SpellEntry const* spellProto, SpellEffectIndex effectIndex, int32 total, int32 benefit, int32 ap_benefit, DamageEffectType damagetype, bool donePart, WorldObject* pCaster, Spell* spell) const
{
    if (donePart && spellProto->Custom & SPELL_CUSTOM_BONUS_COEFF_USES_AP)
    {
        Unit const* caster = pCaster ? pCaster->ToUnit() : ToUnit();
        if (caster)
        {
            WeaponAttackType attackType = spellProto->IsSpellRequiresRangedAP() ? RANGED_ATTACK : BASE_ATTACK;
            benefit = int32(caster->GetTotalAttackPowerValue(attackType)) + ap_benefit;
        }
        else
            benefit = 0;
    }

    if (benefit)
    {
        float coeff;

        // Check for table values
        if (spellProto->EffectBonusCoefficient[effectIndex] >= 0.0f)
            coeff = spellProto->EffectBonusCoefficient[effectIndex];
        // Calculate default coefficient
        else
            coeff = spellProto->CalculateDefaultCoefficient(damagetype);

        // Calculate level penalty only if spell does not have coefficient set in template,
        // since the coefficients already have the level penalty accounted for.
        const float lvlPenalty = (spellProto->EffectBonusCoefficient[effectIndex] >= 0.0f) ? 1.0f : CalculateLevelPenalty(spellProto);

        // Calculate custom coefficient
        coeff = spellProto->CalculateCustomCoefficient(pCaster, damagetype, coeff, spell, donePart);

        // Spellmod SpellDamage
        if (Unit const* pUnit = ToUnit())
        {
            if (Player* modOwner = pUnit->GetSpellModOwner())
            {
                coeff *= 100.0f;
                modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_SPELL_BONUS_DAMAGE, coeff, spell);
                coeff /= 100.0f;
            }
        }

        total += int32(benefit * coeff * lvlPenalty);
    }

    return total;
};

void WorldObject::DealDamageMods(Unit *victim, uint32 &damage, uint32* absorb)
{
    Unit* pUnit = ToUnit();
    // Don't allow Spirit of Redemption to take damage
    if (!victim->IsAlive() ||
        victim->IsTaxiFlying() ||
        (victim->IsCreature() && static_cast<Creature*>(victim)->IsInEvadeMode()) ||
        (pUnit && pUnit->GetClass() == CLASS_PRIEST && pUnit->HasAura(27827)))
    {
        if (absorb)
            *absorb += damage;
        damage = 0;
        return;
    }

    uint32 originalDamage = damage;

    //Script Event damage Deal
    if (Creature* pCreature = ToCreature())
        if (pCreature->AI())
            pCreature->AI()->DamageDeal(victim, damage);

    //Script Event damage taken
    if (Creature* pCreature = victim->ToCreature())
        if (pCreature->AI() && pUnit)
            pCreature->AI()->DamageTaken(pUnit, damage);

    if (absorb && originalDamage > damage)
        *absorb += (originalDamage - damage);
}

float WorldObject::CalculateLevelPenalty(SpellEntry const* spellProto)
{
    // Fix Nostalrius. La formule de MaNGOSZero est BC.
    uint32 spellLevel = spellProto->spellLevel;
    if (spellLevel <= 0 || spellLevel > 20)
        return 1.0f;
    return 1 - ((20.0f - spellLevel) * 0.0375f);

    /*
        float LvlPenalty = 0.0f;
        if(spellLevel < 20)
            LvlPenalty = 20.0f - spellLevel * 3.75f;
        float LvlFactor = (float(spellLevel) + 6.0f) / float(GetLevel());
        if(LvlFactor > 1.0f)
            LvlFactor = 1.0f;
        return (100.0f - LvlPenalty) * LvlFactor / 100.0f;
    */
}

float WorldObject::GetAPMultiplier(WeaponAttackType attType, bool normalized) const
{
    Unit const* pUnit = ToUnit();
    if (!pUnit)
        return 0;

    if (!normalized || GetTypeId() != TYPEID_PLAYER)
        return float(pUnit->GetAttackTime(attType)) / 1000.0f;

    Item *Weapon = ((Player*)this)->GetWeaponForAttack(attType, true, false);
    if (!Weapon)
        return 2.4f;                                         // fist attack

    switch (Weapon->GetProto()->InventoryType)
    {
        case INVTYPE_2HWEAPON:
            return 3.3f;
        case INVTYPE_RANGED:
        case INVTYPE_RANGEDRIGHT:
        case INVTYPE_THROWN:
            return 2.8f;
        case INVTYPE_WEAPON:
        case INVTYPE_WEAPONMAINHAND:
        case INVTYPE_WEAPONOFFHAND:
        default:
            return Weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER ? 1.7f : 2.4f;
    }
}

void WorldObject::DealSpellDamage(SpellNonMeleeDamage *damageInfo, bool durabilityLoss)
{
    if (!damageInfo)
        return;

    Unit *pVictim = damageInfo->target;

    if (!pVictim)
        return;

    if (!pVictim->IsAlive() || pVictim->IsTaxiFlying() || (pVictim->GetTypeId() == TYPEID_UNIT && ((Creature*)pVictim)->IsInEvadeMode()))
        return;

    SpellEntry const *spellProto = sSpellMgr.GetSpellEntry(damageInfo->SpellID);
    if (spellProto == nullptr)
    {
        sLog.outError("WorldObject::DealSpellDamage have wrong damageInfo->SpellID: %u", damageInfo->SpellID);
        return;
    }

    // Call default DealDamage (send critical in hit info for threat calculation)
    CleanDamage cleanDamage(0, BASE_ATTACK, damageInfo->HitInfo & SPELL_HIT_TYPE_CRIT ? MELEE_HIT_CRIT : MELEE_HIT_NORMAL, damageInfo->absorb, damageInfo->resist);
    DealDamage(pVictim, damageInfo->damage, &cleanDamage, spellProto->HasAttribute(SPELL_ATTR_EX3_TREAT_AS_PERIODIC) ? DOT : SPELL_DIRECT_DAMAGE, GetSchoolMask(damageInfo->school), spellProto, durabilityLoss, damageInfo->spell, true, damageInfo->reflected);
}

uint32 WorldObject::DealDamage(Unit *pVictim, uint32 damage, CleanDamage const* cleanDamage, DamageEffectType damagetype, SpellSchoolMask damageSchoolMask, SpellEntry const *spellProto, bool durabilityLoss, Spell* spell, bool addThreat, bool reflected)
{
    // Should never happen since DealDamage is overriden in Unit class.
    if (pVictim == this)
        return 0;

    return pVictim->DealDamage(pVictim, damage, cleanDamage, damagetype, damageSchoolMask, spellProto, durabilityLoss, spell, addThreat, reflected);
}

bool WorldObject::CheckAndIncreaseCastCounter()
{
    uint32 maxCasts = sWorld.getConfig(CONFIG_UINT32_MAX_SPELL_CASTS_IN_CHAIN);

    if (maxCasts && m_castCounter >= maxCasts)
        return false;

    ++m_castCounter;
    return true;
}

void WorldObject::MoveChannelledSpellWithCastTime(Spell* pSpell)
{
    MANGOS_ASSERT(pSpell);
    MANGOS_ASSERT(pSpell->m_spellInfo->IsChanneledSpell() && pSpell->m_casttime && !pSpell->m_IsTriggeredSpell && pSpell->getState() == SPELL_STATE_CASTING);
    MANGOS_ASSERT(m_currentSpells[CURRENT_GENERIC_SPELL] == pSpell);

    if (Spell* pChannelled = m_currentSpells[CURRENT_CHANNELED_SPELL])
    {
        MANGOS_ASSERT(pChannelled->m_spellInfo->Id == pSpell->m_spellInfo->Id);
        InterruptSpell(CURRENT_CHANNELED_SPELL, true);
    }

    m_currentSpells[CURRENT_GENERIC_SPELL] = nullptr;
    m_currentSpells[CURRENT_CHANNELED_SPELL] = pSpell;
    pSpell->m_selfContainer = &(m_currentSpells[CURRENT_CHANNELED_SPELL]);
}

void WorldObject::SetCurrentCastedSpell(Spell * pSpell)
{
    MANGOS_ASSERT(pSpell);                                  // nullptr may be never passed here, use InterruptSpell or InterruptNonMeleeSpells

    CurrentSpellTypes CSpellType = pSpell->GetCurrentContainer();

    if (pSpell == m_currentSpells[CSpellType]) return;      // avoid breaking self
    // break same type spell if it is not delayed
    InterruptSpell(CSpellType, false);

    // special breakage effects:
    switch (CSpellType)
    {
        case CURRENT_GENERIC_SPELL:
        {
            // a channelled spell with a cast time is considered generic before channeling starts,
            // but it does not break itself if you start to recast it, only once channeling starts
            if (!pSpell->m_spellInfo->IsChanneledSpell() || (m_currentSpells[CURRENT_CHANNELED_SPELL] && m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->Id != pSpell->m_spellInfo->Id))
                InterruptSpell(CURRENT_CHANNELED_SPELL, false);

            // autorepeat breaking
            if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
            {
                // break autorepeat if not Auto Shot
                if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Category == 351)
                    InterruptSpell(CURRENT_AUTOREPEAT_SPELL);

                if (Unit* pUnit = ToUnit())
                    pUnit->m_AutoRepeatFirstCast = true;
            }
        }
        break;

        case CURRENT_CHANNELED_SPELL:
        {
            // channel spells always break generic non-delayed and any channeled spells
            InterruptSpell(CURRENT_GENERIC_SPELL, false);
            InterruptSpell(CURRENT_CHANNELED_SPELL);

            // it also does break autorepeat if not Auto Shot
            if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL] &&
                    m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Category == 351)
                InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
        }
        break;

        case CURRENT_AUTOREPEAT_SPELL:
        {
            // only Auto Shoot does not break anything
            if (pSpell->m_spellInfo->Category == 351)
            {
                // generic autorepeats break generic non-delayed and channeled non-delayed spells
                InterruptSpell(CURRENT_GENERIC_SPELL, false);
                InterruptSpell(CURRENT_CHANNELED_SPELL, false);
            }
            // special action: set first cast flag
            if (Unit* pUnit = ToUnit())
                pUnit->m_AutoRepeatFirstCast = true;
        }
        break;

        default:
        {
            // other spell types don't break anything now
        } break;
    }

    // current spell (if it is still here) may be safely deleted now
    if (m_currentSpells[CSpellType])
        m_currentSpells[CSpellType]->SetReferencedFromCurrent(false);

    // set new current spell
    m_currentSpells[CSpellType] = pSpell;
    pSpell->SetReferencedFromCurrent(true);

    pSpell->m_selfContainer = &(m_currentSpells[pSpell->GetCurrentContainer()]);
}

Spell* WorldObject::FindCurrentSpellBySpellId(uint32 spell_id) const
{
    for (uint32 i = 0; i < CURRENT_MAX_SPELL; ++i)
        if (m_currentSpells[i] && m_currentSpells[i]->m_spellInfo->Id == spell_id)
            return m_currentSpells[i];
    return nullptr;
}

bool WorldObject::IsNonMeleeSpellCasted(bool withDelayed, bool skipChanneled, bool skipAutorepeat) const
{
    // We don't do loop here to explicitly show that melee spell is excluded.
    // Maybe later some special spells will be excluded too.

    // generic spells are casted when they are not finished and not delayed
    if (m_currentSpells[CURRENT_GENERIC_SPELL] &&
            (m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_FINISHED) &&
            (withDelayed || m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_DELAYED))
        return (true);

    // channeled spells may be delayed, but they are still considered casted
    else if (!skipChanneled && m_currentSpells[CURRENT_CHANNELED_SPELL] &&
             (m_currentSpells[CURRENT_CHANNELED_SPELL]->getState() != SPELL_STATE_FINISHED))
        return (true);

    // autorepeat spells may be finished or delayed, but they are still considered casted
    else if (!skipAutorepeat && m_currentSpells[CURRENT_AUTOREPEAT_SPELL])
        return (true);

    return (false);
}

bool WorldObject::IsNextSwingSpellCasted() const
{
    if (m_currentSpells[CURRENT_MELEE_SPELL] && m_currentSpells[CURRENT_MELEE_SPELL]->IsNextMeleeSwingSpell())
        return (true);

    return (false);
}

bool WorldObject::IsNoMovementSpellCasted() const
{
    if (m_currentSpells[CURRENT_GENERIC_SPELL] &&
            (m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_FINISHED) &&
             m_currentSpells[CURRENT_GENERIC_SPELL]->getState() != SPELL_STATE_DELAYED &&
             m_currentSpells[CURRENT_GENERIC_SPELL]->m_spellInfo->HasSpellInterruptFlag(SPELL_INTERRUPT_FLAG_MOVEMENT))
        return (true);
    else if (m_currentSpells[CURRENT_CHANNELED_SPELL] &&
             m_currentSpells[CURRENT_CHANNELED_SPELL]->getState() != SPELL_STATE_FINISHED &&
            (m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->HasSpellInterruptFlag(SPELL_INTERRUPT_FLAG_MOVEMENT) ||
             m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->HasChannelInterruptFlag(AURA_INTERRUPT_FLAG_MOVE)))
        return (true);
    // don't need to check for AUTOREPEAT_SPELL

    return (false);
}

void WorldObject::InterruptSpellsWithInterruptFlags(uint32 flags, uint32 except)
{
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
        if (Spell* spell = GetCurrentSpell(CurrentSpellTypes(i)))
            if (spell->getState() == SPELL_STATE_PREPARING && spell->GetCastedTime())
                if (!spell->IsNextMeleeSwingSpell() && !spell->IsAutoRepeat() && !spell->IsTriggered() && (spell->m_spellInfo->InterruptFlags & flags) && spell->m_spellInfo->Id != except)
                    InterruptSpell(CurrentSpellTypes(i));
}


void WorldObject::InterruptNonMeleeSpells(bool withDelayed, uint32 spell_id)
{
    // generic spells are interrupted if they are not finished or delayed
    if (m_currentSpells[CURRENT_GENERIC_SPELL] && (!spell_id || m_currentSpells[CURRENT_GENERIC_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_GENERIC_SPELL, withDelayed);

    // autorepeat spells are interrupted if they are not finished or delayed
    if (m_currentSpells[CURRENT_AUTOREPEAT_SPELL] && (!spell_id || m_currentSpells[CURRENT_AUTOREPEAT_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_AUTOREPEAT_SPELL, withDelayed);

    // channeled spells are interrupted if they are not finished, even if they are delayed
    if (m_currentSpells[CURRENT_CHANNELED_SPELL] && (!spell_id || m_currentSpells[CURRENT_CHANNELED_SPELL]->m_spellInfo->Id == spell_id))
        InterruptSpell(CURRENT_CHANNELED_SPELL, true);
}

void WorldObject::InterruptSpell(CurrentSpellTypes spellType, bool withDelayed)
{
    MANGOS_ASSERT(spellType < CURRENT_MAX_SPELL);

    Spell* targetSpell = m_currentSpells[spellType];

    if (targetSpell && (withDelayed || targetSpell->getState() != SPELL_STATE_DELAYED))
    {
        // Remove Insignia spell is very special, and should'nt be interrupted usually
        if (targetSpell->m_spellInfo->Id == 22027)
        {
            return;
        }
        // send autorepeat cancel message for autorepeat spells
        if (spellType == CURRENT_AUTOREPEAT_SPELL)
        {
            if (GetTypeId() == TYPEID_PLAYER)
                ((Player*)this)->SendAutoRepeatCancel();
        }

        if (targetSpell->getState() != SPELL_STATE_FINISHED)
            targetSpell->cancel();

        // cancel can interrupt spell already (caster cancel ->target aura remove -> caster iterrupt)
        if (m_currentSpells[spellType])
        {
            m_currentSpells[spellType]->SetReferencedFromCurrent(false);
            m_currentSpells[spellType] = nullptr;
            targetSpell = nullptr;
        }
    }
}

void WorldObject::FinishSpell(CurrentSpellTypes spellType, bool ok /*= true*/)
{
    Spell* spell = m_currentSpells[spellType];
    if (!spell)
        return;

    spell->finish(ok);

    if (spellType == CURRENT_CHANNELED_SPELL)
        spell->SendChannelUpdate(0);
}

void WorldObject::GetDynObjects(uint32 spellId, SpellEffectIndex effectIndex, std::vector<DynamicObject*>& dynObjsOut) const
{
    for (auto const& guid : m_spellDynObjects)
    {
        DynamicObject* dynObj = GetMap()->GetDynamicObject(guid);
        if (!dynObj)
            continue;

        if (dynObj->GetSpellId() == spellId && dynObj->GetEffIndex() == effectIndex)
            dynObjsOut.push_back(dynObj);
    }
}

DynamicObject * WorldObject::GetDynObject(uint32 spellId, SpellEffectIndex effIndex) const
{
    for (auto const& guid : m_spellDynObjects)
    {
        DynamicObject* dynObj = GetMap()->GetDynamicObject(guid);
        if (!dynObj)
            continue;

        if (dynObj->GetSpellId() == spellId && dynObj->GetEffIndex() == effIndex)
            return dynObj;
    }
    return nullptr;
}

DynamicObject * WorldObject::GetDynObject(uint32 spellId) const
{
    for (auto const& guid : m_spellDynObjects)
    {
        DynamicObject* dynObj = GetMap()->GetDynamicObject(guid);
        if (!dynObj)
            continue;

        if (dynObj->GetSpellId() == spellId)
            return dynObj;
    }
    return nullptr;
}

void WorldObject::AddDynObject(DynamicObject* dynObj)
{
    m_spellDynObjects.push_back(dynObj->GetObjectGuid());
    dynObj->SetWorldMask(GetWorldMask()); // Nostalrius : phasing
}

void WorldObject::RemoveDynObject(uint32 spellid)
{
    if (m_spellDynObjects.empty())
        return;

    for (auto i = m_spellDynObjects.begin(); i != m_spellDynObjects.end();)
    {
        DynamicObject* dynObj = GetMap()->GetDynamicObject(*i);
        if (!dynObj)
            i = m_spellDynObjects.erase(i);
        else if (spellid == 0 || dynObj->GetSpellId() == spellid)
        {
            dynObj->Delete();
            i = m_spellDynObjects.erase(i);
        }
        else
            ++i;
    }
}

void WorldObject::RemoveDynObjectWithGUID(ObjectGuid guid)
{
    for (auto itr = m_spellDynObjects.begin(); itr != m_spellDynObjects.end();)
    {
        if ((*itr) == guid)
            itr = m_spellDynObjects.erase(itr);
        else
            ++itr;
    }
}

void WorldObject::RemoveAllDynObjects()
{
    for (auto const& guid : m_spellDynObjects)
    {
        if (DynamicObject* dynObj = GetMap()->GetDynamicObject(guid))
            dynObj->Delete();
    }
    m_spellDynObjects.clear();
}

SpellCastResult WorldObject::CastSpell(Unit* pTarget, uint32 spellId, bool triggered, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, SpellEntry const* triggeredBy, SpellEntry const* triggeredByParent, bool bCanIgnoreLOS /*= false*/)
{
    SpellEntry const* spellInfo = sSpellMgr.GetSpellEntry(spellId);

    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastSpell: unknown spell id %i by caster: %s triggered by aura %u (eff %u)", spellId, GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastSpell: unknown spell id %i by caster: %s", spellId, GetGuidStr().c_str());
        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    return CastSpell(pTarget, spellInfo, triggered, castItem, triggeredByAura, originalCaster, triggeredBy, triggeredByParent, bCanIgnoreLOS);
}

SpellCastResult WorldObject::CastSpell(GameObject* pTarget, uint32 spellId, bool triggered, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, SpellEntry const* triggeredBy, SpellEntry const* triggeredByParent)
{
    SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(spellId);

    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastSpell: unknown spell id %i by caster: %s triggered by aura %u (eff %u)", spellId, GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastSpell: unknown spell id %i by caster: %s", spellId, GetGuidStr().c_str());
        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    return CastSpell(pTarget, spellInfo, triggered, castItem, triggeredByAura, originalCaster, triggeredBy, triggeredByParent);
}

SpellCastResult WorldObject::CastSpell(Unit* pTarget, SpellEntry const* spellInfo, bool triggered, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, SpellEntry const* triggeredBy, SpellEntry const* triggeredByParent, bool bCanIgnoreLOS /*= false*/)
{
    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastSpell: unknown spell by caster: %s triggered by aura %u (eff %u)", GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastSpell: unknown spell by caster: %s", GetGuidStr().c_str());
        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    if (castItem)
        DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", spellInfo->Id);

    if (triggeredByAura)
    {
        if (!originalCaster)
            originalCaster = triggeredByAura->GetCasterGuid();

        triggeredBy = triggeredByAura->GetSpellProto();
    }

    Spell* spell;

    if (Unit* pUnit = ToUnit())
        spell = new Spell(pUnit, spellInfo, triggered, originalCaster, triggeredBy, nullptr, triggeredByParent, bCanIgnoreLOS);
    else if (GameObject* pGameObject = ToGameObject())
        spell = new Spell(pGameObject, spellInfo, triggered, originalCaster, triggeredBy, nullptr, triggeredByParent, bCanIgnoreLOS);
    else
        return SPELL_FAILED_ERROR;

    SpellCastTargets targets;


    if (pTarget)
    {
        // Don't set unit target on destination target based spells, otherwise the spell will cancel
        // as soon as the target dies or leaves the area of the effect
        if (spellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
            targets.setDestination(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());
        else if (Unit* pUnitTarget = pTarget->ToUnit())
            targets.setUnitTarget(pUnitTarget);
        else
            return SPELL_FAILED_ERROR;
    }

    if (spellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION)
        if (WorldObject* caster = spell->GetCastingObject())
            targets.setSource(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());

    spell->SetCastItem(castItem);
    return spell->prepare(std::move(targets), triggeredByAura);
}

SpellCastResult WorldObject::CastSpell(GameObject* pTarget, SpellEntry const* spellInfo, bool triggered, Item* castItem, Aura* triggeredByAura, ObjectGuid originalCaster, SpellEntry const* triggeredBy, SpellEntry const* triggeredByParent)
{
    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastSpell: unknown spell by caster: %s triggered by aura %u (eff %u)", GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastSpell: unknown spell by caster: %s", GetGuidStr().c_str());
        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    if (castItem)
        DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", spellInfo->Id);

    if (triggeredByAura)
    {
        if (!originalCaster)
            originalCaster = triggeredByAura->GetCasterGuid();

        triggeredBy = triggeredByAura->GetSpellProto();
    }

    Spell *spell;

    if (Unit* pUnit = ToUnit())
        spell = new Spell(pUnit, spellInfo, triggered, originalCaster, triggeredBy, nullptr, triggeredByParent);
    else if (GameObject* pGameObject = ToGameObject())
        spell = new Spell(pGameObject, spellInfo, triggered, originalCaster, triggeredBy, nullptr, triggeredByParent);
    else
        return SPELL_FAILED_ERROR;

    SpellCastTargets targets;

    // Don't set unit target on destination target based spells, otherwise the spell will cancel
    // as soon as the target dies or leaves the area of the effect
    if (spellInfo->Targets & TARGET_FLAG_DEST_LOCATION)
        targets.setDestination(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());
    else
        targets.setGOTarget(pTarget);

    if (spellInfo->Targets & TARGET_FLAG_SOURCE_LOCATION)
        if (WorldObject* caster = spell->GetCastingObject())
            targets.setSource(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());

    spell->SetCastItem(castItem);
    return spell->prepare(std::move(targets), triggeredByAura);
}

void WorldObject::CastCustomSpell(Unit* pTarget, uint32 spellId, int32 const* bp0, int32 const* bp1, int32 const* bp2, bool triggered, Item *castItem, Aura* triggeredByAura, bool addThreat, ObjectGuid originalCaster, SpellEntry const* triggeredBy)
{
    SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(spellId);

    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastCustomSpell: unknown spell id %i by caster: %s triggered by aura %u (eff %u)", spellId, GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastCustomSpell: unknown spell id %i by caster: %s", spellId, GetGuidStr().c_str());
        return;
    }

    CastCustomSpell(pTarget, spellInfo, bp0, bp1, bp2, triggered, castItem, triggeredByAura, addThreat, originalCaster, triggeredBy);
}

void WorldObject::CastCustomSpell(Unit* pTarget, SpellEntry const *spellInfo, int32 const* bp0, int32 const* bp1, int32 const* bp2, bool triggered, Item *castItem, Aura* triggeredByAura, bool addThreat, ObjectGuid originalCaster, SpellEntry const* triggeredBy)
{
    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastCustomSpell: unknown spell by caster: %s triggered by aura %u (eff %u)", GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastCustomSpell: unknown spell by caster: %s", GetGuidStr().c_str());
        return;
    }

    if (castItem)
        DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", spellInfo->Id);

    if (triggeredByAura)
    {
        if (!originalCaster)
            originalCaster = triggeredByAura->GetCasterGuid();

        triggeredBy = triggeredByAura->GetSpellProto();
    }

    Spell *spell;

    if (Unit* pUnit = ToUnit())
        spell = new Spell(pUnit, spellInfo, triggered, originalCaster, triggeredBy);
    else if (GameObject* pGameObject = ToGameObject())
        spell = new Spell(pGameObject, spellInfo, triggered, originalCaster, triggeredBy);
    else
        return;

    if (bp0)
        spell->m_currentBasePoints[EFFECT_INDEX_0] = *bp0;

    if (bp1)
        spell->m_currentBasePoints[EFFECT_INDEX_1] = *bp1;

    if (bp2)
        spell->m_currentBasePoints[EFFECT_INDEX_2] = *bp2;

    SpellCastTargets targets;
    targets.setUnitTarget(pTarget);
    spell->SetCastItem(castItem);
    spell->m_addThreat = addThreat;
    spell->prepare(std::move(targets), triggeredByAura);
}

void WorldObject::CastCustomSpell(Unit* target, SpellEntry const* customInfo, bool triggered)
{
    Spell* spell;

    if (Unit* pUnit = ToUnit())
        spell = new Spell(pUnit, customInfo, triggered);
    else if (GameObject* pGameObject = ToGameObject())
        spell = new Spell(pGameObject, customInfo, triggered);
    else
        return;

    SpellCastTargets targets;
    targets.setUnitTarget(target);
    spell->MarkCustom();
    spell->prepare(std::move(targets));
}

// used for scripting
SpellCastResult WorldObject::CastSpell(float x, float y, float z, uint32 spellId, bool triggered, Item *castItem, Aura* triggeredByAura, ObjectGuid originalCaster, SpellEntry const* triggeredBy)
{
    SpellEntry const *spellInfo = sSpellMgr.GetSpellEntry(spellId);

    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastSpell(x,y,z): unknown spell id %i by caster: %s triggered by aura %u (eff %u)", spellId, GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastSpell(x,y,z): unknown spell id %i by caster: %s", spellId, GetGuidStr().c_str());
        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    return CastSpell(x, y, z, spellInfo, triggered, castItem, triggeredByAura, originalCaster, triggeredBy);
}

// used for scripting
SpellCastResult WorldObject::CastSpell(float x, float y, float z, SpellEntry const *spellInfo, bool triggered, Item *castItem, Aura* triggeredByAura, ObjectGuid originalCaster, SpellEntry const* triggeredBy)
{
    if (!spellInfo)
    {
        if (triggeredByAura)
            sLog.outError("CastSpell(x,y,z): unknown spell by caster: %s triggered by aura %u (eff %u)", GetGuidStr().c_str(), triggeredByAura->GetId(), triggeredByAura->GetEffIndex());
        else
            sLog.outError("CastSpell(x,y,z): unknown spell by caster: %s", GetGuidStr().c_str());
        return SPELL_FAILED_SPELL_UNAVAILABLE;
    }

    if (castItem)
        DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", spellInfo->Id);

    if (triggeredByAura)
    {
        if (!originalCaster)
            originalCaster = triggeredByAura->GetCasterGuid();

        triggeredBy = triggeredByAura->GetSpellProto();
    }

    Spell *spell;

    if (Unit* pUnit = ToUnit())
        spell = new Spell(pUnit, spellInfo, triggered, originalCaster, triggeredBy);
    else if (GameObject* pGameObject = ToGameObject())
        spell = new Spell(pGameObject, spellInfo, triggered, originalCaster, triggeredBy);
    else
        return SPELL_FAILED_ERROR;

    SpellCastTargets targets;
    targets.setDestination(x, y, z);
    spell->SetCastItem(castItem);
    return spell->prepare(std::move(targets), triggeredByAura);
}

bool WorldObject::isVisibleFor(Player const* u, WorldObject const* viewPoint) const
{
    return IsVisibleForInState(u, viewPoint, false);
}

void WorldObject::PMonsterEmote(const char *text, Unit const* target, bool IsBossEmote, ...) const
{
    va_list ap;
    char str[2048];
    va_start(ap, IsBossEmote);
    vsnprintf(str, 2048, text, ap);
    va_end(ap);
    MonsterTextEmote(str, target, IsBossEmote);
}

void WorldObject::PMonsterEmote(int32 textId, Unit const* target, bool IsBossEmote, ...) const
{
    va_list ap;
    va_start(ap, IsBossEmote);
    float range = sWorld.getConfig(CONFIG_FLOAT_LISTEN_RANGE_TEXTEMOTE);
    MaNGOS::MonsterChatBuilderFormat emote_build(*this, CHAT_MSG_MONSTER_EMOTE, textId, LANG_UNIVERSAL, nullptr, &ap);
    MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilderFormat> emote_do(emote_build);
    MaNGOS::CameraDistWorker<MaNGOS::LocalizedPacketDo<MaNGOS::MonsterChatBuilderFormat> > emote_worker(this, range, emote_do);
    Cell::VisitWorldObjects(this, emote_worker, range);
    va_end(ap);
}

bool WorldObject::isFacing(const Position location, const float tolerance) const {
    const auto requiredAngle = atan2(location.y - m_position.y, location.x - m_position.x);
    auto minAngle = requiredAngle - tolerance;
    if (minAngle < 0) {
        minAngle += (M_PI_F * 2);
    }
    auto maxAngle = requiredAngle + tolerance;
    if (maxAngle > (M_PI_F * 2)) {
        maxAngle -= M_PI_F;
    }

    bool isWithinTolerance = false;
    if (maxAngle < minAngle) {
        // We wrapped around.
        isWithinTolerance = m_position.o <= maxAngle || m_position.o >= minAngle;
    } else {
        isWithinTolerance = m_position.o <= maxAngle && m_position.o >= minAngle;
    }
    return isWithinTolerance;
}

// function based on function Unit::CanAttack from 13850 client
bool WorldObject::IsValidAttackTarget(Unit const* target, bool checkAlive) const
{
    ASSERT(target);

    // can't attack self
    if (this == target)
        return false;

    if (FindMap() != target->FindMap())
        return false;

    if (!target->IsTargetable(true, IsCharmerOrOwnerPlayerOrPlayerItself(), false, checkAlive))
        return false;

    Unit const* pThisUnit = ToUnit();

    // CvC case - can attack each other only when one of them is hostile
    if ((!pThisUnit || !pThisUnit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED)) && !target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED))
        return GetReactionTo(target) <= REP_HOSTILE || target->GetReactionTo(this) <= REP_HOSTILE;

    // PvP, PvC, CvP case
    // can't attack friendly targets
    if (GetReactionTo(target) > REP_NEUTRAL
        || target->GetReactionTo(this) > REP_NEUTRAL)
        return false;

    Player const* playerAffectingAttacker = GetAffectingPlayer();
    Player const* playerAffectingTarget = target->GetAffectingPlayer();

    // Not all neutral creatures can be attacked
    if (GetReactionTo(target) == REP_NEUTRAL &&
        target->GetReactionTo(this) == REP_NEUTRAL)
    {
        if (!(playerAffectingTarget && playerAffectingAttacker) &&
            !(!playerAffectingTarget && !playerAffectingAttacker))
        {
            Player const* player = playerAffectingTarget ? playerAffectingTarget : playerAffectingAttacker;
            WorldObject const* object = playerAffectingTarget ? this : target;

            if (FactionTemplateEntry const* factionTemplate = object->GetFactionTemplateEntry())
            {
                if (!(player->GetReputationMgr().GetForcedRankIfAny(factionTemplate)))
                    if (FactionEntry const* factionEntry = sObjectMgr.GetFactionEntry(factionTemplate->faction))
                        if (FactionState const* repState = player->GetReputationMgr().GetState(factionEntry))
                            if (!(repState->Flags & FACTION_FLAG_AT_WAR))
                                return false;

            }
        }
    }

    // PvP checks
    if (playerAffectingAttacker && playerAffectingTarget)
    {

        if (playerAffectingAttacker->GetGroup() && playerAffectingTarget->GetGroup())
        {
            if (playerAffectingAttacker->GetGroup() == playerAffectingTarget->GetGroup())
                return false;
        }

        if (playerAffectingAttacker->m_duel && playerAffectingAttacker->m_duel->opponent == playerAffectingTarget->GetObjectGuid() && playerAffectingAttacker->m_duel->startTime != 0)
            return true;

        if (playerAffectingTarget->IsPvP())
        {
            if (playerAffectingAttacker->IsHardcore() && !playerAffectingAttacker->IsPvP()) // custom check, disable HC accidentally flagging.
                return false;
            return true;
        }

        if ((playerAffectingAttacker->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP)
            && (playerAffectingTarget->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_FFA_PVP))
            return true;

        if (playerAffectingAttacker->IsFFAPvP() && playerAffectingTarget->IsFFAPvP())
            return true;

        return (playerAffectingAttacker->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_UNK1)
            || (playerAffectingTarget->GetByteValue(UNIT_FIELD_BYTES_2, 1) & UNIT_BYTE2_FLAG_UNK1);
    }

    return true;
}
