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

void KeyboardController::update(SDL_Event &event)
{
    switch(event.type)
    {
        case SDL_KEYDOWN: // If pressed key
          switch(event.key.keysym.sym)
          {
            case SDLK_z:
                keys.jump=true;
            break;

            case SDLK_x:
                keys.run=true;
            break;

            case SDLK_RIGHT:
                keys.right=true;
            break;

            case SDLK_UP:
                keys.up=true;
            break;

            case SDLK_LEFT:
                keys.left=true;
            break;

            case SDLK_DOWN:
              keys.down=true;
            break;
          }
        break;

        case SDL_KEYUP:
            switch(event.key.keysym.sym)
            {
                case SDLK_z:
                    keys.jump=false;
                break;

                case SDLK_x:
                    keys.run=false;
                break;

                case SDLK_RIGHT:
                    keys.right=false;
                    break;

                case SDLK_UP:
                    keys.up=false;
                break;

                case SDLK_LEFT:
                    keys.left=false;
                    break;

                case SDLK_DOWN:
                    keys.down=false;
                break;
            }
        break;
    }
}
