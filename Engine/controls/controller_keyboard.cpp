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

#include "controller_keyboard.h"

KeyboardController::KeyboardController() :
    Controller()
{
    kmap.jump       = SDL_SCANCODE_Z;
    kmap.jump_alt   = SDL_SCANCODE_A;
    kmap.run        = SDL_SCANCODE_X;
    kmap.run_alt    = SDL_SCANCODE_S;
    kmap.drop       = SDL_SCANCODE_LSHIFT;
    kmap.start      = SDL_SCANCODE_RETURN;
    kmap.left       = SDL_SCANCODE_LEFT;
    kmap.right      = SDL_SCANCODE_RIGHT;
    kmap.up         = SDL_SCANCODE_UP;
    kmap.down       = SDL_SCANCODE_DOWN;
}

KeyboardController::~KeyboardController()
{}

void KeyboardController::update()
{
    SDL_PumpEvents();

    const Uint8* state = SDL_GetKeyboardState(NULL);

    keys.jump=state[kmap.jump];
    keys.alt_jump=state[kmap.jump_alt];

    keys.run=state[kmap.run];
    keys.alt_run=state[kmap.run_alt];

    keys.right=state[kmap.right];
    keys.up=state[kmap.up];
    keys.down=state[kmap.down];
    keys.left=state[kmap.left];

    keys.drop=state[kmap.drop];
    keys.start=state[kmap.start];
}
