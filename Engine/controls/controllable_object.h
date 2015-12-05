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

protected:
    //! Recently accepted control key states
    controller_keys keys;
};

#endif // CONTROLLABLE_OBJECT_H
