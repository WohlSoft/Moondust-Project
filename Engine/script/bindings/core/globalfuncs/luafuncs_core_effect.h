#ifndef BINDING_CORE_GLOBALFUNCS_EFFECT_H
#define BINDING_CORE_GLOBALFUNCS_EFFECT_H

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

#include <script/lua_global.h>

class Binding_Core_GlobalFuncs_Effect
{
public:

    static void runStaticEffect(long effectID, float startX, float startY, lua_State *L);
    static void runStaticEffectCentered(long effectID, float startX, float startY, lua_State *L);

    static luabind::scope bindToLua();
};

#endif // BINDING_CORE_GLOBALFUNCS_EFFECT_H
