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

#ifndef LUAFUNCS_CORE_SETTINGS_H
#define LUAFUNCS_CORE_SETTINGS_H

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class Binding_Core_GlobalFuncs_Settings
{
public:
    static bool     isFullscreenActive();
    static int      getScreenWidth();
    static int      getScreenHeight();
    //static int    getMaxFPS();
    static double   getFrameRate();
    static double   getFrameDelay();
    static bool     useFrameSkip();
    static int      getSoundVolume();
    static int      getMusicVolume();

    static bool     isDebugInfoShown();
    static void     setDebugInfoShown(bool debugInfoShown);

    static luabind::scope bindToLua();
};

#endif // LUAFUNCS_CORE_SETTINGS_H
