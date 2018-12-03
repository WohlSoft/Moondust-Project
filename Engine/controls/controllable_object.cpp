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

#include "controllable_object.h"

ControllableObject::ControllableObject()
{
    keys = ResetControlKeys();
}

void ControllableObject::applyControls(controller_keys sentkeys)
{
    keys = sentkeys;
}

void ControllableObject::resetControls()
{
    keys = ResetControlKeys();
}

bool ControllableObject::getKeyState(int keyType)
{
    switch(keyType)
    {
        case KEY_LEFT:      return keys.left;
        case KEY_RIGHT:     return keys.right;
        case KEY_UP:        return keys.up;
        case KEY_DOWN:      return keys.down;
        case KEY_RUN:       return keys.run;
        case KEY_JUMP:      return keys.jump;
        case KEY_ALT_RUN:   return keys.alt_run;
        case KEY_ALT_JUMP:  return keys.alt_jump;
        case KEY_DROP:      return keys.drop;
        case KEY_START:     return keys.start;
        default: return false;
    }
}
