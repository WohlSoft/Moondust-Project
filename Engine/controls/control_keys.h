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

#ifndef CONTROL_KEYS_H
#define CONTROL_KEYS_H

/*!
 * \brief Control key map structure. Contains a "is pressed" states of all available control keys
 */
struct controller_keys
{
    //! Start key
    bool start = false;
    //! Start key (One-shot on press)
    bool start_pressed = false;

    //! Left arrow
    bool left = false;
    //! Left arrow (One-shot on press)
    bool left_pressed = false;
    //! Right arrow
    bool right = false;
    //! Right arrow (One-shot on press)
    bool right_pressed = false;
    //! Up arrow
    bool up = false;
    //! Up arrow (One-shot on press)
    bool up_pressed = false;
    //! Down arrow
    bool down = false;
    //! Down arrow (One-shot on press)
    bool down_pressed = false;

    //! Run/shoot/whip/beat/attack
    bool run = false;
    //! Run/shoot/whip/beat/attack (One-shot on press)
    bool run_pressed = false;
    //! Jump/Swim up
    bool jump = false;
    //! Jump/Swim up (One-shot on press)
    bool jump_pressed = false;
    //! Alt-jump/Spin-jump/Unmount-vehicle
    bool alt_run = false;
    //! Alt-jump/Spin-jump/Unmount-vehicle (One-shot on press)
    bool alt_run_pressed = false;
    //! Alt-jump/Shoot/Whip/Attack
    bool alt_jump = false;
    //! Alt-jump/Shoot/Whip/Attack (One-shot on press)
    bool alt_jump_pressed = false;

    //! Drop a holden item from a reserve box or Choice & Use item in the stock (Weapon, Potion, Armor, Shield, Bomb, etc.)
    bool drop = false;
    //! Drop (One-shot on press)
    bool drop_pressed = false;

    //! If any key pressed
    bool any_key_pressed = false;
};

/*!
 * \brief Initializes a control key states map with unpressed key states
 * \return the initialized control key map structure with unpressed key states
 */
controller_keys ResetControlKeys();

#endif

