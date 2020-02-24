/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
    float colorLevel = m_enabled ? 1.0 : 0.5;
    if(flag)
        FontManager::printText((*flag)?"ON":"OFF", x+valueOffset, y, _font_id, colorLevel, colorLevel, colorLevel);
}

void PGE_BoolMenuItem::toggle()
{
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    if(flag)
        *flag=!(*flag);
    extAction();
}
