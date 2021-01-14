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
