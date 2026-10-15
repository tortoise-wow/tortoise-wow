#ifndef MODULE_SLOTS_H
#define MODULE_SLOTS_H

#include "Common.h"

// Per-player storage slots for modules.
//
// A module that has to hang state off a Player claims a slot here and reaches
// it through Player::GetModuleSlot / SetModuleSlot. The core allocates the
// space and never reads it: what a slot points at, who owns it and when it is
// freed are entirely the module's business.
//
// Claiming a slot is the one thing that needs a line in the core, so keep the
// list short and give each entry an owner. Two modules must never share a
// number - there is no runtime check.
//
// A flat array rather than a keyed map on purpose. The population module reads
// its slot on every tick of every driven character; at five hundred of them a
// hash lookup is measurable and a load from a fixed offset is not.

enum ModuleSlot : uint8
{
    MODULE_SLOT_BOT_AI  = 0,    // playerbots: PlayerbotAI for a driven character
    MODULE_SLOT_BOT_MGR = 1,    // playerbots: PlayerbotMgr for a human commanding bots

    MODULE_SLOT_MAX
};

#endif
