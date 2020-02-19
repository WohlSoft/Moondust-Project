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

#ifndef BINDING_CORE_GLOBALFUNCS_PATHS_H
#define BINDING_CORE_GLOBALFUNCS_PATHS_H

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class Binding_Core_GlobalFuncs_Paths
{
public:
    static std::string path_commongfx();
    static std::string path_level_block();
    static std::string path_level_bgo();
    static std::string path_level_npc();
    static std::string path_effect();
    static std::string path_world_terrain();
    static std::string path_world_scene();
    static std::string path_world_path();
    static std::string path_world_level();
    static std::string path_music();
    static std::string path_sound();
    static luabind::scope bindToLua();
};

#endif // BINDING_CORE_GLOBALFUNCS_PATHS_H
