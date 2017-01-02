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

#ifndef CONTROLLABLE_OBJECT_H
#define CONTROLLABLE_OBJECT_H

#include <vector>
#include "control_keys.h"

#include "controllable_object.h"

/*!
 * \brief Provides controller input interface for a physical objects (for example, playable characters)
 */
class ControllableObject
{
public:
    enum KeyType
    {
        KEY_START=0,
        KEY_DROP,
        KEY_LEFT,
        KEY_RIGHT,
        KEY_UP,
        KEY_DOWN,
        KEY_RUN,
        KEY_JUMP,
        KEY_ALT_RUN,
        KEY_ALT_JUMP
    };
    /*!
     * \brief Constructor
     */
    ControllableObject();

    /*!
     * \brief Input slot which accepts control key states map from a controller
     * \param Control key states map structure
     */
    void applyControls(controller_keys sentkeys);

    /*!
     * \brief Sets all key states into "unpressed" state
     */
    void resetControls();

    /*!
     * \brief Returns a state of key type
     * \param keyType Type of key
     * \return true if key pressed, false if released
     */
    bool getKeyState(int keyType);

protected:
    //! Recently accepted control key states
    controller_keys keys;
};

#endif // CONTROLLABLE_OBJECT_H
