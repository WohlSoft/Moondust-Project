/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef BINDING_LEVEL_GLOBALFUNCS_NPC_H
#define BINDING_LEVEL_GLOBALFUNCS_NPC_H

#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class Binding_Level_GlobalFuncs_NPC
{
public:
    static unsigned long count(lua_State *L);
    static luabind::object get(lua_State *L);
    static luabind::object getActive(lua_State *L);
    static luabind::scope bindToLua();
};

#endif // BINDING_LEVEL_GLOBALFUNCS_NPC_H
