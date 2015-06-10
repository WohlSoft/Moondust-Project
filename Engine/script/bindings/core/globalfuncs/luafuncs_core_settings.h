#ifndef LUAFUNCS_CORE_SETTINGS_H
#define LUAFUNCS_CORE_SETTINGS_H

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class Binding_Core_GlobalFuncs_Settings
{
public:
    static bool isDebugInfoShown();
    static void setDebugInfoShown(bool debugInfoShown);

    static luabind::scope bindToLua();
};

#endif // LUAFUNCS_CORE_SETTINGS_H
