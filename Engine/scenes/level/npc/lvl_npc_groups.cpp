/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "../../scene_level.h"

/********************WIP***************************
   Joint NPC Group provides shared activation. Each NPC of this group
   will be kept activated while one of member NPC's will be in vizible space.
   Also this system providing a group controll. For example, logs are makes
   united raft which works as comples machine.

   P.S. Later will be refactored!
*/

void LVL_Npc::buildBuddieGroup()
{
    buddies_leader=this;
    std::vector<LVL_Npc *> needtochec;
    buildLeaf(needtochec, buddies_list, this);
    do
    {
        for(size_t i = 0; i < needtochec.size(); i++)
        {
            buildLeaf(needtochec, buddies_list, this);
        }
    }
    while(!needtochec.empty());
}

void LVL_Npc::updateBuddies(double tickTime)
{
    if(buddies_updated)
        return;
    for(size_t i = 0; i < buddies_list->size(); i++)
    {
        (*buddies_list)[i]->update(tickTime);
        (*buddies_list)[i]->buddies_updated=true;
    }
}

void LVL_Npc::buildLeaf(std::vector<LVL_Npc *> &needtochec, std::vector<LVL_Npc *> *&list, LVL_Npc *leader)
{
    (void)(leader);
    std::vector<PGE_Phys_Object*> bodies;
    PGE_RectF posRectC = m_momentum.rectF().withMargin(2.0);
    m_scene->queryItems(posRectC, &bodies);
    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object* item=*it;
        if(item->type!=PGE_Phys_Object::LVLNPC) continue;
        LVL_Npc*body=dynamic_cast<LVL_Npc*>(item);
        if(!body) continue;
        if(!body->isVisible()) continue;
        if(body->_npc_id != _npc_id) continue;
        if(!body->buddies_list)
        {
            body->buddies_list=list;
            needtochec.push_back(body);
        }
        else
            std::remove(needtochec.begin(), needtochec.end(), body);
    }
}
