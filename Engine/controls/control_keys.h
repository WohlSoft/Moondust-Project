/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CONTROL_KEYS_H
#define CONTROL_KEYS_H

/*!
 * \brief Control key map structure. Contains a "is pressed" states of all available control keys
 */
struct controller_keys
{
    //! Start key
    bool start = false;

    //! Left arrow
    bool left = false;
    //! Right arrow
    bool right = false;
    //! Up arrow
    bool up = false;
    //! Down arrow
    bool down = false;

    //! Run/shoot/whip/beat/attack
    bool run = false;
    //! Jump/Swim up
    bool jump = false;
    //! Alt-jump/Spin-jump/Unmount-vehicle
    bool alt_run = false;
    //! Alt-jump/Shoot/Whip/Attack
    bool alt_jump = false;

    //! Drop a holden item from a reserve box or Choice & Use item in the stock (Weapon, Potion, Armor, Shield, Bomb, etc.)
    bool drop = false;
};

/*!
 * \brief Initializes a control key states map with unpressed key states
 * \return the initialized control key map structure with unpressed key states
 */
controller_keys ResetControlKeys();

#endif

