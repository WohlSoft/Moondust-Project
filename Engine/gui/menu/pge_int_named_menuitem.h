/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PGE_INT_NAMED_MENUITEM_H
#define PGE_INT_NAMED_MENUITEM_H

#include "_pge_menuitem.h"
#include <vector>

struct NamedIntItem
{
    int         value = 0;
    std::string label;
};

class PGE_NamedIntMenuItem : public PGE_Menuitem
{
public:
    PGE_NamedIntMenuItem();
    PGE_NamedIntMenuItem(const PGE_NamedIntMenuItem &it);
    ~PGE_NamedIntMenuItem();
    void left();
    void right();
    void render(int x, int y);

private:
    int     *intvalue;
    std::vector<NamedIntItem > items;
    size_t  curItem;
    bool    allowRotation;
    friend class PGE_Menu;
};


#endif // PGE_INT_NAMED_MENUITEM_H

