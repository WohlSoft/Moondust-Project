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

#include <fontman/font_manager.h>

#include "_pge_menuitem.h"

PGE_Menuitem::PGE_Menuitem()
{
    this->extAction = []()->void{};
}

PGE_Menuitem::~PGE_Menuitem()
{}

PGE_Menuitem::PGE_Menuitem(const PGE_Menuitem &_it)
{
    this->title         = _it.title;
    this->item_key      = _it.item_key;
    this->type          = _it.type;
    this->extAction     = _it.extAction;
    this->valueOffset   = _it.valueOffset;
    this->m_enabled     = _it.m_enabled;
    this->_font_id      = _it._font_id;
    this->_width        = _it._width;
}

void PGE_Menuitem::left() {}

void PGE_Menuitem::right() {}

void PGE_Menuitem::toggle() {}

void PGE_Menuitem::render(int x, int y)
{
    float colorLevel = m_enabled ? 1.0 : 0.5;
    FontManager::printText(title, x, y, _font_id, colorLevel, colorLevel, colorLevel, 1.0f, 18);
}


