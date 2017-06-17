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

#ifndef LVL_SUBTREE_H
#define LVL_SUBTREE_H

#include "lvl_base_object.h"

struct LVL_SubTree_private;
class LVL_SubTree : public PGE_Phys_Object
{
    public:
        LVL_SubTree(LevelScene *_parent = NULL);
        LVL_SubTree(const LVL_SubTree &st);
        ~LVL_SubTree();

        typedef bool (*t_resultCallback)(PGE_Phys_Object*, void *);
        typedef PGE_Phys_Object *PhysObjPtr;
        void registerElement(PhysObjPtr item);
        void unregisterElement(PhysObjPtr item);
        void    query(PGE_RectF &zone, t_resultCallback a_resultCallback, void *context);

        double  m_offsetX = 0.0;
        double  m_offsetY = 0.0;

        double  m_offsetXold = 0.0;
        double  m_offsetYold = 0.0;

    private:
        LVL_SubTree_private *p = nullptr;
};

#endif // LVL_SUBTREE_H
