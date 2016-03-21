/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
