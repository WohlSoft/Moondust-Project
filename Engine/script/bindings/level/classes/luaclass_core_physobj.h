#ifndef BINDING_LEVEL_CLASS_PHYSOBJ_H
#define BINDING_LEVEL_CLASS_PHYSOBJ_H

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Level_Class_PhysObj
{
public:
    static luabind::scope bindToLua();
};

#endif // BINDING_LEVEL_CLASS_PHYSOBJ_H
