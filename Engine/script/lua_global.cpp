#include "lua_global.h"

namespace LuaGlobal {
    std::map<lua_State *, LuaEngine *> engines;

    void add(lua_State *L, LuaEngine *engine)
    {
        engines[L] = engine;
    }

    void remove(lua_State *L)
    {
        decltype(engines)::iterator it = engines.find(L);
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
        return dynamic_cast<LuaLevelEngine*>(getLevelEngine(L));
    }

}
