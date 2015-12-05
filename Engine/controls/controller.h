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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QVector>
#include "control_keys.h"
#include "controllable_object.h"
#include "controller_key_map.h"

/*!
 * \brief The Controller class provides proxy interface between controllable objects array and
 *        physical controller (keyboard/joystick) or vitual controller (for example cut-scene actor
 *        controller at lua-side)
 */
class Controller
{
public:
    /*!
     * \brief Constructor
     */
    Controller();
    /*!
     * \brief Desctructor
     */
    virtual ~Controller();

    /*!
     * \brief Control key command codes
     */
    enum commands
    {
        key_start=0,
        key_left,
        key_right,
        key_up,
        key_down,
        key_run,
        key_jump,
        key_altrun,
        key_altjump,
        key_drop
    };

    /*!
     * \brief Initializes a control key states map with unpressed key states
     * \return the initialized control key map structure with unpressed key states
     */
    static controller_keys noKeys();

    /*!
     * \brief Accepts control keys map which will associates physical keys of control device and command code
     * \param Control keys map
     */
    virtual void setKeyMap(KeyMap map);

    /*!
     * \brief Sends "unpressed" control key states map to every registered controllable object
     */
    void resetControls();

    /*!
     * \brief Sends current control key states map to every registered controllable object
     */
    void sendControls();

    /*!
     * \brief Updates controllable object: read key states from controllable device
     *        and apply gotten key states to every registered controllable object
     */
    virtual void update();

    /*!
     * \brief Register controllable object to this controller.
     * \param obj Pointer to controllable object
     */
    void registerInControl(ControllableObject* obj);

    /*!
     * \brief Remove controllable object from this controller
     * \param obj Pointer to controllable object which must be removed from this controller
     */
    void removeFromControl(ControllableObject* obj);

    //! Current state of control keys
    controller_keys keys;

protected:
    //! Current control keys map
    KeyMap kmap;

private:
    //! Array of registered controllabl objects
    QVector<ControllableObject* > m_objects;
};

#endif // CONTROLLER_H
