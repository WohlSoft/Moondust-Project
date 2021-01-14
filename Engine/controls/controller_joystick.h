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

#ifndef CONTROLLER_JOYSTICK_H
#define CONTROLLER_JOYSTICK_H

#include "controller.h"
#include <SDL2/SDL.h>
#include <controls/controller_key_map.h>

/*!
 * \brief A Joystick controller which reads key states from a joystick device
 */
class JoystickController final : public Controller
{
public:
    /*!
     * \brief Constructor
     */
    JoystickController();

    /*!
     * \brief Destructir
     */
    ~JoystickController() override = default;

    /*!
     * \brief Sets joystick device descriptor which will be used by this controller
     * \param Joystick device descriptor
     */
    void setJoystickDevice(SDL_Joystick* jctrl);

    /*!
     * \brief Returns current joystick device descriptor
     * \return Current joystic device descriptor currently associated with this controller. NULL if no joysticks was plugged
     */
    SDL_Joystick* getJoystickDevice() const;

    /*!
     * \brief Takes a state of specified joystick key
     * \param key Output key state which will be copied from actual joystick key state
     * \param key_pressed Output one-shot key state which will be copied from actual joystick key state
     * \param mkey Joystick key info
     */
    void updateKey(bool &key, bool &key_pressed, KM_Key &mkey);

    /*!
     * \brief Read states of all control keys of a joystick device
     */
    void update() override;


    /*!
     * \brief Detects a pressed key type and it's from a given joystick device and writes info into a given key description structure
     * \param joy Josytick device descriptor
     * \param k Joystick key description
     * \return true if pressed key was been detected and returned through key description structure, false if no pressed keys
     */
    static bool bindJoystickKey(SDL_Joystick* joy, KM_Key &k);

private:
    //! Joystick device descriptor
    SDL_Joystick* m_joystickController;
};

#endif // CONTROLLER_JOYSTICK_H
