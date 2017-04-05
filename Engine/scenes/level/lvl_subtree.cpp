/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_subtree.h"
#include <common_features/RTree/RTree.h>

struct LVL_SubTree_private
{
    typedef RTree<PGE_Phys_Object*, double, 2, double > IndexTree;
    IndexTree tree;
};

LVL_SubTree::LVL_SubTree(LevelScene *_parent) :
    PGE_Phys_Object(_parent)
{
    type =          LVLSubTree;
    m_bodytype =    Body_STATIC;
    p = new LVL_SubTree_private;
}

LVL_SubTree::LVL_SubTree(const LVL_SubTree &st):
    PGE_Phys_Object(st.m_scene)
{
    p = new LVL_SubTree_private;
    p->tree = st.p->tree;
}

LVL_SubTree::~LVL_SubTree()
{
    if(p)
    {
        p->tree.RemoveAll();
        delete p;
    }
}

void LVL_SubTree::registerElement(LVL_SubTree::PhysObjPtr item)
{
    double lt[2]={item->m_posX_registered, item->m_posY_registered};
    double rb[2]={item->m_posX_registered+item->m_width_registered, item->m_posY_registered+item->m_height_registered};
    if(item->m_width_registered<=0) { rb[0]=item->m_posX_registered+1;}
    if(item->m_height_registered<=0) { rb[1]=item->m_posY_registered+1;}
    p->tree.Insert(lt, rb, item);
}

void LVL_SubTree::unregisterElement(LVL_SubTree::PhysObjPtr item)
{
    double lt[2]={item->m_posX_registered, item->m_posY_registered};
    double rb[2]={item->m_posX_registered+item->m_width_registered, item->m_posY_registered+item->m_height_registered};
    if(item->m_width_registered<=0) { rb[0]=item->m_posX_registered+1;}
    if(item->m_height_registered<=0) { rb[1]=item->m_posY_registered+1;}
    p->tree.Remove(lt, rb, item);
}

void LVL_SubTree::query(PGE_RectF &zone, LVL_SubTree::t_resultCallback a_resultCallback, void *context)
{
    double lt[2] = { zone.left(),  zone.top() };
    double rb[2] = { zone.right(), zone.bottom() };
    p->tree.Search(lt, rb, a_resultCallback, context);
}
