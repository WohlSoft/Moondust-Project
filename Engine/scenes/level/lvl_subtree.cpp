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

#include "lvl_subtree.h"
#include <common_features/QuadTree/LooseQuadtree.h>


class QTreePGE_SubTree_Extractor
{
public:
    static void ExtractBoundingBox(const PGE_Phys_Object *object, loose_quadtree::BoundingBox<double> *bbox)
    {
        bbox->left      = object->m_momentum_relative.x;
        bbox->top       = object->m_momentum_relative.y;
        bbox->width     = object->m_momentum_relative.w;
        bbox->height    = object->m_momentum_relative.h;
        if(bbox->width <= 0.0)
            bbox->width = 1.0;
        if(bbox->height <= 0.0)
            bbox->height = 1.0;
    }
};

struct LVL_SubTree_private
{
    typedef loose_quadtree::LooseQuadtree<double, PGE_Phys_Object, QTreePGE_SubTree_Extractor> IndexTreeQ;
    IndexTreeQ tree;
};


LVL_SubTree::LVL_SubTree(LevelScene *_parent) :
    PGE_Phys_Object(_parent),
    p(new LVL_SubTree_private)
{
    type =          LVLSubTree;
    m_bodytype =    Body_STATIC;
}

//LVL_SubTree::LVL_SubTree(const LVL_SubTree &st):
//    PGE_Phys_Object(st.m_scene),
//    p(new LVL_SubTree_private)
//{
//    p->tree = st.p->tree;
//}

LVL_SubTree::~LVL_SubTree()
{
    if(p)
    {
        p->tree.Clear();
        p.reset(nullptr);
    }
}

void LVL_SubTree::insert(LVL_SubTree::PhysObjPtr item)
{
    p->tree.Insert(item);
}

void LVL_SubTree::update(LVL_SubTree::PhysObjPtr item)
{
    p->tree.Update(item);
}

void LVL_SubTree::remove(LVL_SubTree::PhysObjPtr item)
{
    p->tree.Remove(item);
}

void LVL_SubTree::clear()
{
    p->tree.Clear();
}

void LVL_SubTree::query(PGE_RectF &zone, LVL_SubTree::t_resultCallback a_resultCallback, void *context)
{
    LVL_SubTree_private::IndexTreeQ::Query q = p->tree.QueryIntersectsRegion(loose_quadtree::BoundingBox<double>(zone.x(), zone.y(), zone.width(), zone.height()));
    while(!q.EndOfQuery())
    {
        a_resultCallback(q.GetCurrent(), context);
        q.Next();
    }
}
