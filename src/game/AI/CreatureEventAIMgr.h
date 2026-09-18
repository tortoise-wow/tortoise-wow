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

#ifndef MANGOS_CREATURE_EAI_MGR_H
#define MANGOS_CREATURE_EAI_MGR_H

#include "Common.h"
#include "CreatureEventAI.h"
#include "ScriptMgr.h"

// Published only after loading completes. Existing creatures retain their
// event AND action-script generation across native script/event reloads.
struct CreatureEventAIGeneration
{
    ScriptMapMap scripts;
    CreatureEventAI_Event_Map events;
};

class CreatureEventAIMgr
{
    public:
        CreatureEventAIMgr(){};
        ~CreatureEventAIMgr(){};

        void LoadCreatureEventAI_Events();
        void ClearEventData() { std::atomic_store(&m_generation, std::make_shared<CreatureEventAIGeneration const>()); }

        std::shared_ptr<CreatureEventAIGeneration const> AcquireGeneration() const { return std::atomic_load(&m_generation); }

    private:
        std::shared_ptr<CreatureEventAIGeneration const> m_generation = std::make_shared<CreatureEventAIGeneration const>();
};

extern CreatureEventAIMgr sEventAIMgr;

#endif
