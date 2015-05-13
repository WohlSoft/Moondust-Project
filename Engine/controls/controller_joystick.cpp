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

#include "controller_joystick.h"
#include <QtDebug>


JoystickController::JoystickController() :
    Controller(),
    joystickController(0)
{
//    qDebug() << "Num of joysticks: " << SDL_NumJoysticks();
//    if(SDL_NumJoysticks() > 0){
//        //TODO: Select which controller you want to use.
//        joystickController =  SDL_JoystickOpen(0);
//        qDebug() << "Opened Controller \"" << SDL_JoystickName(joystickController) << "\"";
//    }

    kmap.jump       = 1;
    kmap.jump_alt   = 3;
    kmap.run        = 0;
    kmap.run_alt    = 2;
    kmap.drop       = 8;
    kmap.start      = 9;
    kmap.left       = 10;
    kmap.right      = 11;
    kmap.up         = 12;
    kmap.down       = 13;
}

JoystickController::~JoystickController()
{}

void JoystickController::setJoystickDevice(SDL_Joystick *jctrl)
{
    joystickController=jctrl;
}

SDL_Joystick *JoystickController::getJoystickDevice() const
{
    return joystickController;
}

void JoystickController::update()
{
    if(!joystickController)
        return;

    SDL_PumpEvents();

    keys.jump = SDL_JoystickGetButton(joystickController, kmap.jump );
    keys.alt_jump = SDL_JoystickGetButton(joystickController, kmap.jump_alt);

    keys.run = SDL_JoystickGetButton(joystickController, kmap.run);
    keys.alt_run = SDL_JoystickGetButton(joystickController, kmap.run_alt);

    Uint8 hatVal = SDL_JoystickGetHat(joystickController, 0);

    keys.right = (hatVal == SDL_HAT_RIGHT);
    keys.up = (hatVal == SDL_HAT_UP);
    keys.down = (hatVal == SDL_HAT_DOWN);
    keys.left = (hatVal == SDL_HAT_LEFT);

    keys.drop = SDL_JoystickGetButton(joystickController, 8);
    keys.start = SDL_JoystickGetButton(joystickController, 9);
}
