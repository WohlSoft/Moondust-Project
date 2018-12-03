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

#ifndef CONTROLLER_KEYBOARD_H
#define CONTROLLER_KEYBOARD_H

#include "controller.h"
#include <SDL2/SDL.h>

/*!
 * \brief A Keyboard controller which reads state of the keyboard device
 */
class KeyboardController final : public Controller
{
public:
    /*!
     * \brief Constructor
     */
    KeyboardController();

    /*!
     * \brief Destructor
     */
    ~KeyboardController() override = default;

    /*!
     * \brief Read current state of keyboard controller
     */
    void update() override;

};

#endif // CONTROLLER_KEYBOARD_H
