/*
The MIT License

Copyright (c) 2009 - 2013 Liam Devine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "oolua_open.h"
#include "lua_includes.h"
#include "proxy_storage.h"
#include "char_arrays.h"
#include "oolua_traits.h"
#include "oolua_registration.h" //get_oolua_module
namespace
{
	void add_weaklookup_table(lua_State* vm)
	{
		lua_newtable(vm);//tb
		int weakTable = lua_gettop(vm);
		luaL_newmetatable(vm, "weak_mt");
		int weak_mt = lua_gettop(vm);

		lua_pushstring(vm, "__mode");//tb mt key
		lua_pushstring(vm, "v");//tb mt key value
		lua_settable(vm, weak_mt);//tb mt
		//weak_mt["__mode"]="v"

		lua_setmetatable(vm, weakTable);//tb
		//weakTable["__mt"]=weak_mt

		OOLUA::INTERNAL::Weak_table::setWeakTable(vm, -2);
		//registry[weak_lookup_name]=weakTable

		lua_pop(vm, 1);//empty
	}
	void add_ownership_globals(lua_State* vm)
	{
		lua_pushinteger(vm, OOLUA::Cpp);//int
		lua_setglobal(vm, OOLUA::INTERNAL::cpp_owns_str);//globals[string]=int

		lua_pushinteger(vm, OOLUA::Lua);//int
		lua_setglobal(vm, OOLUA::INTERNAL::lua_owns_str);//globals[string]=int

		OOLUA::INTERNAL::get_oolua_module(vm);
		lua_pushinteger(vm, OOLUA::Cpp);//int
		OOLUA_PUSH_CARRAY(vm, OOLUA::INTERNAL::cpp_owns_str);
		lua_rawset(vm, -3);

		lua_pushinteger(vm, OOLUA::Lua);//int
		OOLUA_PUSH_CARRAY(vm, OOLUA::INTERNAL::lua_owns_str);
		lua_rawset(vm, -3);

		lua_pop(vm, 1);
	}

	void get_preload_table(lua_State* vm)
	{
#if LUA_VERSION_NUM < 502
		lua_getglobal(vm, "package");
		lua_getfield(vm, -1, "preload");
#else
		lua_getfield(vm, LUA_REGISTRYINDEX, "_PRELOAD");
#endif
		if( lua_type(vm, -1) != LUA_TTABLE )
			luaL_error(vm, "Lua %d get_preload_table failed to retrieve the preload table. Stack top is %s\n"
					   , LUA_VERSION_NUM
					   , lua_typename(vm, -1));
	}

	void register_oolua_module(lua_State *vm)
	{
		int const top = lua_gettop(vm);
		get_preload_table(vm);

		OOLUA_PUSH_CARRAY(vm, OOLUA::INTERNAL::oolua_str);
		lua_pushcclosure(vm, OOLUA::INTERNAL::get_oolua_module, 0);
		lua_settable(vm, -3);

		OOLUA_PUSH_CARRAY(vm, OOLUA::INTERNAL::oolua_str);
		lua_createtable(vm, 0, 2);//starts with two entries cpp_own and lua_owns
		lua_rawset(vm, LUA_REGISTRYINDEX);

		lua_settop(vm, top);
	}
} // namespace

namespace OOLUA
{
	void setup_user_lua_state(lua_State* vm)
	{
		add_weaklookup_table(vm);
		register_oolua_module(vm);
		add_ownership_globals(vm);
	}
} // namespace OOLUA
