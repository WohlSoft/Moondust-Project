/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
