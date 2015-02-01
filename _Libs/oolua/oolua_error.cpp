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

#include "oolua_error.h"
#include "oolua_config.h"

#if OOLUA_STORE_LAST_ERROR == 1

#	include "lua_includes.h"

namespace OOLUA
{
	char const last_error_string[] = {"oolua_last_error"};
	void push_error_id_str(lua_State* vm)
	{
		lua_pushlstring(vm, last_error_string, (sizeof(last_error_string)/sizeof(char))-1);
	}
	void reset_error_value(lua_State* vm)
	{
		push_error_id_str(vm);
		lua_pushnil(vm);
		lua_settable(vm, LUA_REGISTRYINDEX);
	}
	std::string get_last_error(lua_State* vm)
	{
		push_error_id_str(vm);
		lua_gettable(vm, LUA_REGISTRYINDEX);
		std::string error;
		if ( (!lua_isnil(vm, -1)) && (lua_type(vm, -1) == LUA_TSTRING) )
			error = lua_tolstring(vm, -1, 0);
		lua_pop(vm, 1);
		return error;
	}
	namespace INTERNAL
	{
		void set_error_from_top_of_stack_and_pop_the_error(lua_State* vm)
		{
			int error_index = lua_gettop(vm);
			push_error_id_str(vm);
			lua_pushvalue(vm, error_index);
			lua_settable(vm, LUA_REGISTRYINDEX);
			lua_pop(vm, 1);
		}
	} // namespace INTERNAL // NOLINT
} // namespace OOLUA
#else
//default implementations
struct lua_State;

namespace OOLUA
{
	void reset_error_value(lua_State* /*vm*/) // NOLINT
	{}

	std::string get_last_error(lua_State* /*vm*/) // NOLINT
	{
		return std::string();
	}
	namespace INTERNAL
	{
		void set_error_from_top_of_stack_and_pop_the_error(lua_State* /*vm*/) // NOLINT
		{}
	} // namespace INTERNAL
} // namespace OOLUA

#endif
