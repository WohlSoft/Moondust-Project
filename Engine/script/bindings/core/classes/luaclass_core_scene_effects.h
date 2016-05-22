#ifndef BINDING_CORE_SCENE_EFFECTS_H
#define BINDING_CORE_SCENE_EFFECTS_H

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Core_Scene_Effects
{
public:
    static luabind::scope bindToLua();
};

#endif // BINDING_CORE_SCENE_EFFECTS_H
