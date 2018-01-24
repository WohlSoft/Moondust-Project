#include "luafuncs_core_effect.h"

#include <scenes/scene.h>


void Binding_Core_GlobalFuncs_Effect::runStaticEffect(unsigned long effectID,
                                                      double startX,
                                                      double startY,
                                                      lua_State* L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->launchEffect(effectID,
                                                                startX,
                                                                startY,
                                                                1, 0, 0, 0, 0);
}

void Binding_Core_GlobalFuncs_Effect::runStaticEffectCentered(unsigned long effectID,
                                                              double startX,
                                                              double startY,
                                                              lua_State *L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->launchStaticEffectC(effectID,
                                                                startX,
                                                                startY,
                                                                1, 0, 0, 0, 0);
}


void Binding_Core_GlobalFuncs_Effect::runEffect(SpawnEffectDef &effectDef, bool centered, lua_State *L)
{
    LuaGlobal::getEngine(L)->getBaseScene()->launchEffect(effectDef, centered);
}

/***
Animated graphical effects
@module AnimatedEffects
*/

luabind::scope Binding_Core_GlobalFuncs_Effect::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Effect")[
            /***
            Launch statical (non-moving) effect animation with default capabilities
            @function Effect.runStaticEffect
            @tparam ulong effectID ID of effect registered in the config pack
            @tparam double startX Initial horizontal scene position of effect at left-top corner of sprite
            @tparam double startY Initial vertical scene position of effect at left-top corner of sprite
            */
            def("runStaticEffect", &Binding_Core_GlobalFuncs_Effect::runStaticEffect),

            /***
            Launch statical (non-moving) effect animation with default capabilities with center coordinates
            @function Effect.runStaticEffectCentered
            @tparam ulong effectID ID of effect registered in the config pack
            @tparam double startX Initial horizontal scene position of effect by center of sprite
            @tparam double startY Initial vertical scene position of effect by center of sprite
            */
            def("runStaticEffectCentered", &Binding_Core_GlobalFuncs_Effect::runStaticEffectCentered),

            /***
            Launch effect animation with custom capabilities by filling of @{EffectDef} structure
            @function Effect.runEffect
            @tparam EffectDef effectDef Effect definition structure with custom capabilities
            @tparam bool centered Use coordinates at center of effect sprite
            */
            def("runEffect", &Binding_Core_GlobalFuncs_Effect::runEffect)
        ];
}
