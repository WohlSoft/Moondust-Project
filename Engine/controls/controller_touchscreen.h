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

#ifndef CONTROLLER_TOUCHSCREEN_H
#define CONTROLLER_TOUCHSCREEN_H

#include "controller.h"
#include <SDL2/SDL.h>
#include <map>

/*!
 * \brief A mobile touch-screen controller which reads state of the keyboard device
 */
class TouchScreenController final : public Controller
{
    //! Count of touch devices
    int m_touchDevicesCount = 0;
    //! Physical screen width
    int m_screenWidth = 0;
    //! Physical screen height
    int m_screenHeight = 0;
    //! Actual touch device to use
    int m_actualDevice = -1;
public:
    struct FingerState
    {
        bool alive = false;
        bool heldKey[Controller::commands::key_END] = {};
        bool heldKeyPrev[Controller::commands::key_END] = {};

        FingerState();
        FingerState(const FingerState &fs);
        FingerState &operator=(const FingerState &fs);
    };

private:
    //! Held finger states
    std::map<SDL_FingerID, FingerState> m_fingers;

public:
    /*!
     * \brief Constructor
     */
    TouchScreenController();

    /*!
     * \brief Destructor
     */
    ~TouchScreenController() override = default;

    /*!
     * \brief Read current state of keyboard controller
     */
    void update() override;

    void processTouchDevice(int dev_i);
};


#endif // CONTROLLER_TOUCHSCREEN_H
