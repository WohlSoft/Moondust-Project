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

#include "lua_global.h"

namespace LuaGlobal {
    std::map<lua_State *, LuaEngine *> engines;

    void add(lua_State *L, LuaEngine *engine)
    {
        engines[L] = engine;
    }

    void remove(lua_State *L)
    {
        auto it = engines.find(L);
        if(it != engines.end())
            engines.erase(it);
    }

    LuaEngine *getEngine(lua_State *L)
    {
        if(engines.find(L) != engines.end())
            return engines[L];
        return nullptr;
    }

    LuaTitleScreenEngine *getTitleScreenEngine(lua_State *L)
    {
        return dynamic_cast<LuaTitleScreenEngine*>(getEngine(L));
    }

    LuaLevelEngine *getLevelEngine(lua_State *L)
    {
        return dynamic_cast<LuaLevelEngine*>(getEngine(L));
    }

    LuaWorldEngine *getWorldEngine(lua_State *L)
    {
        return dynamic_cast<LuaWorldEngine*>(getEngine(L));
    }

}
