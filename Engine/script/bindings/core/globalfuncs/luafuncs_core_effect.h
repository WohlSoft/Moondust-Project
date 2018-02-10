#ifndef BINDING_CORE_GLOBALFUNCS_EFFECT_H
#define BINDING_CORE_GLOBALFUNCS_EFFECT_H

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

#include <script/lua_global.h>
#include <data_configs/spawn_effect_def.h>

class Binding_Core_GlobalFuncs_Effect
{
public:
    static void runStaticEffect(unsigned long effectID, double startX, double startY, lua_State *L);
    static void runStaticEffectCentered(unsigned long effectID, double startX, double startY, lua_State *L);

    static void runEffect(SpawnEffectDef &effectDef, bool centered, lua_State *L);

    static luabind::scope bindToLua();
};

#endif // BINDING_CORE_GLOBALFUNCS_EFFECT_H
