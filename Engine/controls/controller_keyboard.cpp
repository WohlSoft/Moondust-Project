/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

KeyboardController::KeyboardController()
{}

void KeyboardController::update()
{
    SDL_PumpEvents();

    const Uint8* state = SDL_GetKeyboardState(NULL);

    keys.jump=state[SDL_SCANCODE_Z];
    keys.alt_jump=state[SDL_SCANCODE_S];

    keys.run=state[SDL_SCANCODE_X];
    keys.alt_run=state[SDL_SCANCODE_D];

    keys.right=state[SDL_SCANCODE_RIGHT];
    keys.up=state[SDL_SCANCODE_UP];
    keys.down=state[SDL_SCANCODE_DOWN];
    keys.left=state[SDL_SCANCODE_LEFT];

    keys.drop=state[SDL_SCANCODE_LSHIFT];
    keys.start=state[SDL_SCANCODE_RETURN];
}
