#ifndef BINDING_CORE_GLOBALFUNCS_PATHS_H
#define BINDING_CORE_GLOBALFUNCS_PATHS_H

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

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
