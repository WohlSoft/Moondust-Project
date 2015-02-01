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

#include "stack_get.h"
#include "oolua_pull.h"//set_tops_gc_value_to_false
#include "oolua_table.h"
#include "class_from_stack.h"

#include <string>

namespace OOLUA
{
	namespace INTERNAL
	{
		namespace LUA_CALLED
		{
			void get_class_type_error(lua_State* const vm, char const* expected_type)
			{
				luaL_error(vm, "%s %s %s", "tried to pull type"
							, expected_type
							, "which is not the type or a base of the type on the stack");
			}

			void get_error(lua_State* vm, int idx, char const* expected_type)
			{
				luaL_error(vm, "trying to pull %s when %s is on stack"
							, expected_type
							, lua_typename(vm, lua_type(vm, idx)) );
			}

			void get(lua_State* const vm, int idx, void*& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( lua_type(vm, idx) != LUA_TLIGHTUSERDATA ) get_error(vm, idx, "light userdata");
#endif
				value = lua_touserdata(vm, idx);
			}

			void get(lua_State* const vm, int idx, bool& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_isboolean(vm, idx) ) get_error(vm, idx, "bool");
#endif
				value = lua_toboolean(vm, idx) ? true : false;
			}

			void get(lua_State* const vm, int idx, char const*& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( lua_type(vm, idx) != LUA_TSTRING ) get_error(vm, idx, "char const*");
#endif
				value = lua_tolstring(vm, idx, 0);
			}

			void get(lua_State* const vm, int idx, char *& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_isstring(vm, idx) ) get_error(vm, idx, "char*");
#endif
				value = const_cast<char*>(lua_tolstring(vm, idx, 0));
			}

			void get(lua_State* const vm, int idx, double& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_isnumber(vm, idx) ) get_error(vm, idx, "double");
#endif
				value = static_cast<double>(lua_tonumber(vm, idx));
			}

			void get(lua_State* const vm, int idx, float& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_isnumber(vm, idx) ) get_error(vm, idx, "float");
#endif
				value = static_cast<float>(lua_tonumber(vm, idx));
			}

			void get(lua_State* const vm, int idx, lua_CFunction& value)
			{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if( !lua_iscfunction(vm, idx) ) get_error(vm, idx, "lua_CFunction");
#endif
				value = lua_tocfunction(vm, idx);
			}

			void get(lua_State* const vm, int idx, Table& value)
			{
				value.lua_get(vm, idx);
			}

		} // namespace LUA_CALLED //NOLINT
	} // namespace INTERNAL //NOLINT
} // namespace OOLUA
