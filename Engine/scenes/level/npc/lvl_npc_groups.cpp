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
#include "../lvl_scene_ptr.h"

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
    QList<LVL_Npc *> needtochec;
    buildLeaf(needtochec, buddies_list, this);
    do
    {
        for(int i=0; i<needtochec.size();i++)
        {
            buildLeaf(needtochec, buddies_list, this);
        }
    }
    while(!needtochec.isEmpty());
}

void LVL_Npc::updateBuddies(float tickTime)
{
    if(buddies_updated) return;
    for(int i=0;i<buddies_list->size();i++)
    {
        (*buddies_list)[i]->update(tickTime);
        (*buddies_list)[i]->buddies_updated=true;
    }
}

void LVL_Npc::buildLeaf(QList<LVL_Npc *> &needtochec, QList<LVL_Npc *> *&list, LVL_Npc *leader)
{
    Q_UNUSED(leader);
    QVector<PGE_Phys_Object*> bodies;
    PGE_RectF posRectC = posRect.withMargin(2.0);
    LvlSceneP::s->queryItems(posRectC, &bodies);
    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object* item=*it;
        if(item->type!=PGE_Phys_Object::LVLNPC) continue;
        LVL_Npc*body=dynamic_cast<LVL_Npc*>(item);
        if(!body) continue;
        if(!body->isVisible()) continue;
        if(body->data.id != data.id) continue;
        if(!body->buddies_list)
        {
            body->buddies_list=list;
            needtochec.push_back(body);
        }
        else needtochec.removeAll(body);
    }
}
