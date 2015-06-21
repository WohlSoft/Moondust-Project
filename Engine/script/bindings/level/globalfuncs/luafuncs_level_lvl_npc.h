#ifndef BINDING_LEVEL_GLOBALFUNCS_NPC_H
#define BINDING_LEVEL_GLOBALFUNCS_NPC_H

#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Level_GlobalFuncs_NPC
{
public:
    static luabind::object get(lua_State *L);

    static luabind::scope bindToLua();
};

#endif // BINDING_LEVEL_GLOBALFUNCS_NPC_H
