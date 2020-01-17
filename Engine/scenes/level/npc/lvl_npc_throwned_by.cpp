/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "../lvl_npc.h"
#include "../lvl_player.h"

void LVL_Npc::resetThrowned()
{
    throwned_by_npc=0;
    throwned_by_npc_obj=nullptr;
    throwned_by_player=0;
    throwned_by_player_obj=nullptr;
}

void LVL_Npc::setThrownedByNpc(long npcID, LVL_Npc *npcObj)
{
    throwned_by_npc=npcID;
    throwned_by_npc_obj=npcObj;
    throwned_by_player=0;
    throwned_by_player_obj=nullptr;
}

void LVL_Npc::setThrownedByPlayer(long playerID, LVL_Player *npcObj)
{
    throwned_by_npc=0;
    throwned_by_npc_obj=nullptr;
    throwned_by_player=playerID;
    throwned_by_player_obj=npcObj;
}

long LVL_Npc::thrownedByNpc()
{
    return throwned_by_npc;
}

LVL_Npc *LVL_Npc::thrownedByNpcObj()
{
    return throwned_by_npc_obj;
}

long LVL_Npc::thrownedByPlayer()
{
    return throwned_by_player;
}

LVL_Player *LVL_Npc::thrownedByPlayerObj()
{
    return throwned_by_player_obj;
}

