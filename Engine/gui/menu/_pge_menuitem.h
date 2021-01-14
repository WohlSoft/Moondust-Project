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

#ifndef MENUITEM_H_
#define MENUITEM_H_

#include <string>
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
        this->item_key = _it.item_key;
        this->_width = _it._width;
        this->type = _it.type;
        this->valueOffset = _it.valueOffset;
        this->m_enabled = _it.m_enabled;
        this->_font_id = _it._font_id;
        this->extAction = _it.extAction;
        return *this;
    }

    virtual void left();
    virtual void right();
    virtual void toggle();

    virtual void render(int x, int y);

    std::string title = "";
    std::string item_key = "";
    enum itemType{
        ITEM_Normal=0,
        ITEM_Bool,
        ITEM_Int,
        ITEM_NamedInt,
        ITEM_KeyGrab,
        ITEM_StrList
    };

    itemType type = ITEM_Normal;
    //! X-offset where must be rendered value label
    int valueOffset = 350;
    //! Is menuitem enabled
    bool m_enabled = true;
protected:
    std::function<void()> extAction;
    int _width = 0;
    int _font_id = 0;

private:
    friend class PGE_Menu;
};


#endif // MENUITEM_H_

