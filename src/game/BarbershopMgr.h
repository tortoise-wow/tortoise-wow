#ifndef BARBERSHOP_MGR_H
#define BARBERSHOP_MGR_H

#include "Common.h"
#include "ObjectGuid.h"

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>

class GameObject;
class Player;

/*
 * THE SERVER HALF OF THE BARBERSHOP.
 *
 * The client half ships in patch-4.mpq as
 * Interface\FrameXML\Turtle_BarbershopUI\Turtle_Barbershop.lua: a window, a
 * turntable model and three selectors. It draws nothing until the server sends
 * `TW_BARBERSHOP` / `S2C_OPEN_BARBER`. Without this class, sitting in a
 * Barbershop Chair (gameobject 105180) seats the player and does nothing else.
 *
 * The protocol, read from that file (fields split on ';'):
 *
 *   S2C_OPEN_BARBER;cost;style;;color;;facial   (the empty fields are the
 *                                                "available" lists the client
 *                                                has commented out)
 *   S2C_UPDATE_PREVIEW;cost;style;color;facial
 *   S2C_PURCHASE_RESULT;1;style;color;facial
 *   S2C_ERROR;n                                  (BARBERSHOP_ERROR1..5)
 *   S2C_CLOSE_BARBER
 *
 *   C2S_PREVIEW_STYLE;n  C2S_PREVIEW_COLOR;n  C2S_PREVIEW_FACIAL_HAIR;n
 *   C2S_PURCHASE  C2S_RESET  C2S_CLOSE
 *
 * All values are the 0-based bytes in PLAYER_BYTES / PLAYER_BYTES_2.
 *
 * A PREVIEW IS A REAL CHANGE TO THE CHARACTER, because the client's model
 * redraws from the player's own fields. So every way out of a session that is
 * not a purchase has to put the old bytes back: the Cancel button, standing
 * up, walking off, dying, entering combat and logging out. And the save writes
 * the ORIGINAL bytes while a session is open, so an autosave followed by a
 * crash cannot turn a preview into a free haircut.
 */
class BarbershopMgr
{
    public:
        static bool IsBarberChair(uint32 entry) { return entry == 105180; }

        // From the chair case of GameObject::Use, after the player is seated.
        void Open(Player* player, GameObject* chair);
        bool HandleAddonMessage(Player* player, uint32 type, std::string const& msg);

        // From Player::Update. Closes a session the player has walked out of.
        void Update(Player* player);
        // From WorldSession::LogoutPlayer, before the logout save.
        void OnLogout(Player* player);
        // From Player::SaveToDB: the bytes to write, originals while in a session.
        void GetSaveBytes(Player const* player, uint32& bytes, uint32& bytes2) const;

    private:
        // Hair style, hair colour and facial hair.
        struct Look
        {
            uint8 values[3] = {};
        };

        struct Session
        {
            ObjectGuid chair;
            Look original;
        };

        // Copper for going from one look to the other at the player's level.
        static uint32 GetCost(Player const* player, Look const& from, Look const& to);

        // Callers hold m_lock.
        void Close(Player* player, Session const& session, bool notify);
        bool GetSession(Player const* player, Session& out) const;

        static Look ReadLook(Player const* player);
        static void ApplyLook(Player* player, Look const& look);
        static bool IsAllowed(Player const* player, Look const& original, uint32 customization, uint8 value);
        static uint8 GetLimit(Player const* player, uint32 customization);

        static void Send(Player* player, std::string const& message);
        static void SendPreview(Player* player, Session const& session);

        mutable std::mutex m_lock;
        std::unordered_map<uint32, Session> m_sessions;
        // Player::Update calls in for every player on every tick; this keeps
        // that to one atomic read while nobody is in a chair.
        std::atomic<uint32> m_count{0};
};

extern BarbershopMgr sBarbershopMgr;

#endif
