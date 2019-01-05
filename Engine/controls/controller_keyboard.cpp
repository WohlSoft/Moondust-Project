/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
