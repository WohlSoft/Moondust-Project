/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "controller_keyboard.h"

KeyboardController::KeyboardController() :
        Controller()
{
    kmap.jump.val       = SDL_SCANCODE_Z;
    kmap.jump_alt.val   = SDL_SCANCODE_A;
    kmap.run.val        = SDL_SCANCODE_X;
    kmap.run_alt.val    = SDL_SCANCODE_S;
    kmap.drop.val       = SDL_SCANCODE_LSHIFT;
    kmap.start.val      = SDL_SCANCODE_RETURN;
    kmap.left.val       = SDL_SCANCODE_LEFT;
    kmap.right.val      = SDL_SCANCODE_RIGHT;
    kmap.up.val         = SDL_SCANCODE_UP;
    kmap.down.val       = SDL_SCANCODE_DOWN;
}

static void updateKeyValue(bool &key, bool &key_pressed, const Uint8 &state)
{
    key_pressed = (static_cast<bool>(state) && !key);
    key = state;
}

void KeyboardController::update()
{
    const Uint8 *state = SDL_GetKeyboardState(nullptr);

    updateKeyValue(keys.jump, keys.jump_pressed, state[kmap.jump.val]);
    updateKeyValue(keys.alt_jump, keys.alt_jump_pressed, state[kmap.jump_alt.val]);

    updateKeyValue(keys.run, keys.run_pressed, state[kmap.run.val]);
    updateKeyValue(keys.alt_run, keys.alt_run_pressed, state[kmap.run_alt.val]);

    updateKeyValue(keys.right, keys.right_pressed, state[kmap.right.val]);
    updateKeyValue(keys.up, keys.up_pressed, state[kmap.up.val]);
    updateKeyValue(keys.down, keys.down_pressed, state[kmap.down.val]);
    updateKeyValue(keys.left, keys.left_pressed, state[kmap.left.val]);

    updateKeyValue(keys.drop, keys.drop_pressed, state[kmap.drop.val]);
    updateKeyValue(keys.start, keys.start_pressed, state[kmap.start.val]);
}
