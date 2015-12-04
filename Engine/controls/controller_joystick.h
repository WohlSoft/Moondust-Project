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

#ifndef CONTROLLER_JOYSTICK_H
#define CONTROLLER_JOYSTICK_H

#include "controller.h"
#include <SDL2/SDL.h>
#include <controls/controller_key_map.h>

/*!
 * \brief A Joystick controller which reads key states from a joystick device
 */
class JoystickController : public Controller
{
public:
    /*!
     * \brief Constructor
     */
    JoystickController();

    /*!
     * \brief Destructir
     */
    ~JoystickController();

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
     * \param mkey Joystick key info
     */
    void updateKey(bool &key, KM_Key &mkey);

    /*!
     * \brief Read states of all control keys of a joystick device
     */
    void update();


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
