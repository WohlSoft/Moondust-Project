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

#ifndef PGE_INT_MENUITEM_H
#define PGE_INT_MENUITEM_H

#include "_pge_menuitem.h"

class PGE_IntMenuItem : public PGE_Menuitem
{
public:
    PGE_IntMenuItem();
    PGE_IntMenuItem(const PGE_IntMenuItem &it);
    ~PGE_IntMenuItem();
    void left();
    void right();
    void render(int x, int y);

private:
    int *intvalue;
    int min;
    int max;
    bool allowRotation;
    friend class PGE_Menu;
};


#endif // PGE_INT_MENUITEM_H

