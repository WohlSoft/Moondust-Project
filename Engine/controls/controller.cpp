/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

Controller::Controller()
{
    keys = noKeys();
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

void Controller::update() {}

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
