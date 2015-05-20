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

#ifndef PGE_INT_NAMED_MENUITEM_H
#define PGE_INT_NAMED_MENUITEM_H

#include "_pge_menuitem.h"

struct NamedIntItem
{
    inline NamedIntItem() { value=0; }
    int value;
    QString label;
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
    int *intvalue;
    QList<NamedIntItem > items;
    int curItem;
    bool allowRotation;
    friend class PGE_Menu;
};


#endif // PGE_INT_NAMED_MENUITEM_H

