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
#include "pge_int_menuitem.h"


PGE_IntMenuItem::PGE_IntMenuItem() : PGE_Menuitem()
{
    intvalue=NULL;
    type=ITEM_Int;
    min=0;
    max=0;
    allowRotation=false;
}

PGE_IntMenuItem::PGE_IntMenuItem(const PGE_IntMenuItem &it) : PGE_Menuitem(it)
{
    this->intvalue = it.intvalue;
    this->min = it.min;
    this->max = it.max;
    this->allowRotation = it.allowRotation;
}

PGE_IntMenuItem::~PGE_IntMenuItem()
{}

void PGE_IntMenuItem::left()
{
    if(!intvalue) return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    (*intvalue)--;
    if((*intvalue)<min)
        *intvalue=allowRotation?max:min;

    extAction();
}

void PGE_IntMenuItem::right()
{
    if(!intvalue) return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    (*intvalue)++;
    if((*intvalue)>max)
        *intvalue=allowRotation?min:max;

    extAction();
}

void PGE_IntMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    if(intvalue)
        FontManager::printText(QString::number(*intvalue), x+valueOffset, y, _font_id);
}
