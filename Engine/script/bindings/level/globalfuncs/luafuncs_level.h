#ifndef BINDING_LEVEL_COMMONFUNCS_H
#define BINDING_LEVEL_COMMONFUNCS_H

#include <script/lua_global.h>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Level_CommonFuncs
{
public:
    static void Lua_ToggleSwitch(lua_State *L, int switchID);
    static luabind::scope bindToLua();
};

#endif // BINDING_LEVEL_COMMONFUNCS_H
