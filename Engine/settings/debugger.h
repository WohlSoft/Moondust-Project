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

#ifndef DEBUGGER_H
#define DEBUGGER_H

#if defined(DEBUG_BUILD) && (defined(__gnu_linux__) || defined(_WIN32))
#define PGE_ENGINE_DEBUG
#endif

class Scene;

/*!
 * \brief The PGE_Debugger class contains flags to enable/disable special debug features of engine
 */
class PGE_Debugger
{
public:
    #ifdef PGE_ENGINE_DEBUG
    static int isDebuggerPresent();
    #endif

    /*!
     * \brief Spawn text input box where user can type a special command which will be executed
     * \param parent Pointer to scene where typed command will be executed
     */
    static void executeCommand(Scene* parent);
    //! Allow debug hot keys
    static bool cheat_debugkeys;
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
