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
#include "pge_int_named_menuitem.h"

PGE_NamedIntMenuItem::PGE_NamedIntMenuItem() : PGE_Menuitem()
{
    intvalue = NULL;
    type = ITEM_NamedInt;
    curItem = 0;
    allowRotation = false;
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
    if(!intvalue)
        return;
    if(items.empty())
        return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    if(curItem == 0)
        curItem = allowRotation ? (items.size() - 1) : 0;
    else
        curItem--;
    *intvalue = items[curItem].value;
    extAction();
}

void PGE_NamedIntMenuItem::right()
{
    if(!intvalue)
        return;
    if(items.empty())
        return;
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerClimb);
    curItem++;
    if(curItem >= items.size())
        curItem = allowRotation ?  0  : (items.size() - 1);
    *intvalue = items[curItem].value;
    extAction();
}

void PGE_NamedIntMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    float colorLevel = m_enabled ? 1.0 : 0.5;
    if(!items.empty())
        FontManager::printText(items[curItem].label, x + valueOffset, y, _font_id, colorLevel, colorLevel, colorLevel);
}
