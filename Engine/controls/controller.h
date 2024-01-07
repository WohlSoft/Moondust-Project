/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <set>
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
    /**
     * \brief Type of controller
     */
    enum ControllerType
    {
        //! Is a keyboard
                type_keyboard = 0,
        //! Is a touch screen of mobile device
                type_touchscreen,
        //! Any other controller: joystick, gamepad, etc.
                type_other
    };

    /*!
     * \brief Constructor
     */
    explicit Controller(ControllerType type = type_other);

    /*!
     * \brief Destructor
     */
    virtual ~Controller() = default;

    /**
     * \brief Get type of current controller
     * @return type of controller
     */
    ControllerType type() const;

    /*!
     * \brief Control key command codes
     */
    enum commands
    {
        key_BEGIN = 0,
        key_start = 0,
        key_left,
        key_right,
        key_up,
        key_down,
        key_run,
        key_jump,
        key_altrun,
        key_altjump,
        key_drop,
        key_END
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
    void registerInControl(ControllableObject *obj);

    /*!
     * \brief Remove controllable object from this controller
     * \param obj Pointer to controllable object which must be removed from this controller
     */
    void removeFromControl(ControllableObject *obj);

    //! Current state of control keys
    controller_keys keys;

protected:
    //! Current control keys map
    KeyMap kmap;
    //! Type of controller
    const ControllerType m_controllerType;

private:
    //! Array of registered controllabl objects
    std::set<ControllableObject * > m_objects;
};

#endif // CONTROLLER_H
