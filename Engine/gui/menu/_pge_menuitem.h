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

#ifndef _MENUITEM_H
#define _MENUITEM_H

#include <QString>
#include <functional>

class PGE_Menu;

class PGE_Menuitem
{
public:
    PGE_Menuitem();
    virtual ~PGE_Menuitem();
    PGE_Menuitem(const PGE_Menuitem &_it);
    PGE_Menuitem operator=(const PGE_Menuitem &_it)
    {
        this->title = _it.title;
        this->value = _it.value;
        this->_width = _it._width;
        this->type = _it.type;
        this->valueOffset = _it.valueOffset;
        this->_font_id = _it._font_id;
        this->extAction = _it.extAction;
        return *this;
    }

    virtual void left();
    virtual void right();
    virtual void toggle();

    virtual void render(int x, int y);

    QString title;
    QString value;
    enum itemType{
        ITEM_Normal=0,
        ITEM_Bool,
        ITEM_Int,
        ITEM_NamedInt,
        ITEM_KeyGrab,
        ITEM_StrList
    };

    itemType type;
    int valueOffset;//!< X-offset where must be rendered value label
protected:
    std::function<void()> extAction;
    int _width;
    int    _font_id;

private:
    friend class PGE_Menu;
};


#endif // _MENUITEM_H

