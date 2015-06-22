/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
