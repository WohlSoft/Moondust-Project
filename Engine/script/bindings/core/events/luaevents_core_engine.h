/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef BINDINGCORE_EVENTS_ENGINE_H
#define BINDINGCORE_EVENTS_ENGINE_H

#include "../../../lua_event.h"
#include <scenes/level/lvl_camera.h>
#include "../../level/classes/luaclass_level_playerstate.h"

class BindingCore_Events_Engine
{
public:
    static LuaEvent createInitEngineEvent(LuaEngine* engine);
    static LuaEvent createLoopEvent(LuaEngine* engine, double ticks);

    static LuaEvent createDrawLevelHUDEvent(LuaEngine* engine,
                                            PGE_LevelCamera* camera,
                                            lua_LevelPlayerState *playerState);
};

#endif // BINDINGCORE_EVENTS_ENGINE_H
