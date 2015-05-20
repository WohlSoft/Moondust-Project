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
#include <audio/pge_audio.h>
#include "pge_bool_menuitem.h"

PGE_BoolMenuItem::PGE_BoolMenuItem() : PGE_Menuitem()
{
    flag=NULL;
    type=ITEM_Bool;
}

PGE_BoolMenuItem::PGE_BoolMenuItem(const PGE_BoolMenuItem &it) : PGE_Menuitem(it)
{
    this->flag = it.flag;
}

PGE_BoolMenuItem::~PGE_BoolMenuItem() {}

void PGE_BoolMenuItem::left()
{
    toggle();
}

void PGE_BoolMenuItem::right()
{
    toggle();
}

void PGE_BoolMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    if(flag)
        FontManager::printText((*flag)?"ON":"OFF", x+valueOffset, y, _font_id);
}

void PGE_BoolMenuItem::toggle()
{
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    if(flag)
        *flag=!(*flag);
    extAction();
}
