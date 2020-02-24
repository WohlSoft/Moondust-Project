/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "controller.h"
#include "controllable_object.h"

Controller::Controller(ControllerType controllerType) :
    m_controllerType(controllerType)
{
    keys = noKeys();
}

Controller::ControllerType Controller::type() const
{
    return m_controllerType;
}

controller_keys Controller::noKeys()
{
    return ResetControlKeys();
}

void Controller::setKeyMap(KeyMap map)
{
    kmap = map;
}

void Controller::resetControls()
{
    keys = ResetControlKeys();
}

void Controller::sendControls()
{
    for(ControllableObject *x : m_objects)
        x->applyControls(keys);
}

void Controller::update()
{
    keys.any_key_pressed = false;
    keys.any_key_pressed |= keys.left_pressed;
    keys.any_key_pressed |= keys.right_pressed;
    keys.any_key_pressed |= keys.up_pressed;
    keys.any_key_pressed |= keys.down_pressed;
    keys.any_key_pressed |= keys.run_pressed;
    keys.any_key_pressed |= keys.jump_pressed;
    keys.any_key_pressed |= keys.alt_run_pressed;
    keys.any_key_pressed |= keys.alt_jump_pressed;
    keys.any_key_pressed |= keys.drop_pressed;
    keys.any_key_pressed |= keys.start_pressed;
}

void Controller::registerInControl(ControllableObject *obj)
{
    if(m_objects.find(obj) == m_objects.end())
        m_objects.insert(obj);
}

void Controller::removeFromControl(ControllableObject *obj)
{
    auto i = m_objects.find(obj);
    if(i != m_objects.end())
        m_objects.erase(i);
}
