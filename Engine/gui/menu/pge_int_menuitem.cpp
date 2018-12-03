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
#include <audio/pge_audio.h>
#include "pge_int_menuitem.h"

PGE_IntMenuItem::PGE_IntMenuItem() : PGE_Menuitem()
{
    intvalue = NULL;
    type = ITEM_Int;
    min = 0;
    max = 0;
    allowRotation = false;
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
    if((*intvalue) < min)
        *intvalue = allowRotation ? max : min;

    extAction();
}

void PGE_IntMenuItem::right()
{
    if(!intvalue) return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    (*intvalue)++;
    if((*intvalue) > max)
        *intvalue = allowRotation ? min : max;

    extAction();
}

void PGE_IntMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    float colorLevel = m_enabled ? 1.0 : 0.5;
    if(intvalue)
        FontManager::printText(std::to_string(*intvalue), x + valueOffset, y, _font_id, colorLevel, colorLevel, colorLevel);
}
