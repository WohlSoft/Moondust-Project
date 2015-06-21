// lua.hpp
// Lua header files for C++
// <<extern "C">> not supplied automatically because Lua also compiles as C++

extern "C" {
#ifdef USE_LUA_JIT
#include "lua_jit.h"
#include "lualib_jit.h"
#include "lauxlib_jit.h"
#include "luajit.h"
#else
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#endif
}
