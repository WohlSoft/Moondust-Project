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

#include <fontman/font_manager.h>

#include "_pge_menuitem.h"

PGE_Menuitem::PGE_Menuitem()
{
    this->title = "";
    this->value = "";
    this->extAction = []()->void{};
    this->valueOffset=350;
    this->_font_id=0;
    this->_width=0;
}

PGE_Menuitem::~PGE_Menuitem()
{}

PGE_Menuitem::PGE_Menuitem(const PGE_Menuitem &_it)
{
    this->title = _it.title;
    this->value = _it.value;
    this->type = _it.type;
    this->extAction = _it.extAction;
    this->valueOffset = _it.valueOffset;
    this->_font_id = _it._font_id;
    this->_width=_it._width;
}

void PGE_Menuitem::left() {}

void PGE_Menuitem::right() {}

void PGE_Menuitem::toggle() {}

void PGE_Menuitem::render(int x, int y)
{
    FontManager::printText(title, x, y, _font_id);
    //FontManager::SDL_string_render2D(x,y, &textTexture);
}


