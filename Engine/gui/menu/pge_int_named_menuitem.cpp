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
#include "pge_int_named_menuitem.h"

PGE_NamedIntMenuItem::PGE_NamedIntMenuItem() : PGE_Menuitem()
{
    intvalue=NULL;
    type=ITEM_NamedInt;
    curItem=0;
    allowRotation=false;
}

PGE_NamedIntMenuItem::PGE_NamedIntMenuItem(const PGE_NamedIntMenuItem &it) : PGE_Menuitem(it)
{
    this->intvalue = it.intvalue;
    this->items = it.items;
    this->curItem = it.curItem;
    this->allowRotation = it.allowRotation;
}

PGE_NamedIntMenuItem::~PGE_NamedIntMenuItem()
{}

void PGE_NamedIntMenuItem::left()
{
    if(!intvalue) return;
    if(items.isEmpty()) return;

    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    curItem--;
    if(curItem<0)
        curItem = allowRotation?(items.size()-1):0;
    *intvalue=items[curItem].value;
    extAction();
}

void PGE_NamedIntMenuItem::right()
{
    if(!intvalue) return;
    if(items.isEmpty()) return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    curItem++;
    if(curItem>=items.size())
        curItem = allowRotation?  0  : (items.size()-1);

    *intvalue=items[curItem].value;

    extAction();
}

void PGE_NamedIntMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    if(!items.isEmpty())
        FontManager::printText(items[curItem].label, x+valueOffset, y, _font_id);
}

