#ifndef LUAGLOBAL
#define LUAGLOBAL

#include <map>
#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>
#include "lua_engine.h"
#include "lua_titlescreen_engine.h"
#include "lua_level_engine.h"

namespace LuaGlobal {
    void add(lua_State* L, LuaEngine* engine);
    void remove(lua_State* L);
    LuaEngine* getEngine(lua_State* L);
    LuaTitleScreenEngine* getTitleScreenEngine(lua_State* L);
    LuaLevelEngine* getLevelEngine(lua_State* L);
}

#endif // LUAGLOBAL

