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

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <scenes/scene.h>

/*!
 * \brief The PGE_Debugger class contains flags to enable/disable special debug features of engine
 */
class PGE_Debugger
{
public:
    /*!
     * \brief Spawn text input box where user can type a special command which will be executed
     * \param parent Pointer to scene where typed command will be executed
     */
    static void executeCommand(Scene* parent);

    //! Allows cheating features such a god mode, chuck-norris, superman, etc.
    static bool cheat_allowed;

    //! All Playable characters are will take no damage from enemies/bullets and there are will not burn in lava
    static bool cheat_pagangod;
    //! Allows to playable character destroy any nearest objects at front, at top or at bottom via Alt-Run key
    static bool cheat_chucknorris;
    //! Allows playable characterls unlimitely fly up via Alt-Jump key
    static bool cheat_superman;
    //! Allows playable character walk everywhere on world map with no limits
    static bool cheat_worldfreedom;

    /*!
     * \brief Sets all cheat flags to "false"
     */
    static void resetEverything();

    /*!
     * \brief Toggles restriction to execution of cheating commands
     * \param denyed enable restriction to execution of cheating commands
     */
    static void setRestriction(bool denyed);

};

#endif // DEBUGGER_H
