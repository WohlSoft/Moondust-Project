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

#ifndef BINDING_LEVEL_COMMONFUNCS_H
#define BINDING_LEVEL_COMMONFUNCS_H

#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class Binding_Level_CommonFuncs
{
public:
    static void Lua_ToggleSwitch(lua_State *L, int switchID);
    static bool Lua_getSwitchState(lua_State *L, uint32_t switchID);
    static void Lua_triggerEvent(lua_State *L, std::string eventName);
    static void Lua_ShakeScreen(lua_State *L, double forceX, double forceY, double decX, double decY);
    static void Lua_ShakeScreenX(lua_State *L, double forceX, double decX);
    static void Lua_ShakeScreenY(lua_State *L, double forceY, double decY);
    static luabind::scope bindToLua();
};

#endif // BINDING_LEVEL_COMMONFUNCS_H
