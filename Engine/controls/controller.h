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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QVector>
#include "control_keys.h"
#include "controllable_object.h"
#include "controller_key_map.h"

class Controller
{
public:
    Controller();
    enum commands
    {
        key_start=0,
        key_left,
        key_right,
        key_up,
        key_down,
        key_run,
        key_jump,
        key_altrun,
        key_altjump,
        key_drop
    };
    static controller_keys noKeys();
    virtual void setKeyMap(KeyMap map);

    void resetControls();
    void sendControls();

    virtual void update();
    void registerInControl(ControllableObject* obj);
    void removeFromControl(ControllableObject* obj);

    KeyMap kmap;
    controller_keys keys;
    QVector<ControllableObject* > objects;
};

#endif // CONTROLLER_H
