/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

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
