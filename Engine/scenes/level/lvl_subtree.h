/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
