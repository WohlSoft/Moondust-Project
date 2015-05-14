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

#ifndef CONTROLLER_JOYSTICK_H
#define CONTROLLER_JOYSTICK_H

#include "controller.h"
#include <SDL2/SDL.h>
#include <controls/controller_key_map.h>

class JoystickController : public Controller
{
public:
    JoystickController();
    ~JoystickController();
    void setJoystickDevice(SDL_Joystick* jctrl);
    void setJoyCtrlMap(KeyMapJoyCtrls ids, KeyMapJoyCtrls types);
    SDL_Joystick* getJoystickDevice() const;

    void updateKey(bool &key, int &keyValue, int &keyID, int &keyType);
    void update();

private:
    SDL_Joystick* joystickController;
    KeyMapJoyCtrls _ctrls_ids;
    KeyMapJoyCtrls _ctrls_types;
};

#endif // CONTROLLER_JOYSTICK_H
