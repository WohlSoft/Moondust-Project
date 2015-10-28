#ifndef BINDING_GLOBAL_CONSTANTS_H
#define BINDING_GLOBAL_CONSTANTS_H

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Global_Constants
{
public:
    static void bindToLua(lua_State *L);
};

#endif // BINDING_GLOBAL_CONSTANTS_H
