#ifndef LUAUTILS_H
#define LUAUTILS_H

#include <lauxlib.h>

#define luautil_loadlua(state, code, codelength, buffername) luaL_loadbuffer(state, code, codelength, buffername) || lua_pcall(state, 0, LUA_MULTRET, 0);
#define luautil_loadclass(state, code, codelength, buffername) luaL_loadbuffer(state, code, codelength, buffername) || lua_pcall(state, 0, 1, 0);

#endif // LUAUTILS_H
