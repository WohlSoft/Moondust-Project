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

#ifndef CONTROLLER_KEYBOARD_H
#define CONTROLLER_KEYBOARD_H

#include "controller.h"
#include <SDL2/SDL.h>

/*!
 * \brief A Keyboard controller which reads state of the keyboard device
 */
class KeyboardController : public Controller
{
public:
    /*!
     * \brief Constructor
     */
    KeyboardController();

    /*!
     * \brief Destructor
     */
    ~KeyboardController();

    /*!
     * \brief Read current state of keyboard controller
     */
    void update();

};

#endif // CONTROLLER_KEYBOARD_H
