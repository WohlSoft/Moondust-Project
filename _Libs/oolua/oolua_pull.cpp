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

#include "oolua_pull.h"
#include "oolua_error.h"
#include "oolua_table.h"

#include "class_from_stack.h"
#include "proxy_userdata.h"

namespace OOLUA
{
	namespace INTERNAL
	{
		namespace PULL
		{
			void set_index_gc_value_to_false(lua_State* vm, int idx)
			{
				OOLUA::INTERNAL::userdata_gc_value(static_cast<INTERNAL::Lua_ud*>(lua_touserdata(vm, idx)), false);
			}
		}
		void handle_cpp_pull_fail(lua_State* vm, char const * expected_type)
		{
#	if OOLUA_USE_EXCEPTIONS == 1 || OOLUA_STORE_LAST_ERROR == 1
			lua_pushfstring(vm, "Stack type is not a %s, yet %s"
							, expected_type
							, lua_gettop(vm) ? lua_typename(vm, lua_type(vm, -1) ) : "empty stack");
#	endif
#	if OOLUA_USE_EXCEPTIONS == 1
			throw OOLUA::Type_error(vm, static_cast<ERROR::PopTheStack*>(0));
#	elif OOLUA_STORE_LAST_ERROR == 1
			OOLUA::INTERNAL::set_error_from_top_of_stack_and_pop_the_error(vm);
			return;
#	else
			(void)vm;
			(void)expected_type;
			return;
#	endif
		}

		bool cpp_runtime_type_check_of_top(lua_State* vm, int expected_lua_type, char const * expected_type)
		{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
			if( lua_gettop(vm) == 0 || lua_type(vm, -1) != expected_lua_type )
			{
				handle_cpp_pull_fail(vm, expected_type);
#	if OOLUA_USE_EXCEPTIONS == 0//prevent vs warnings
				return false;//dependant on config if the flow can return through this path
#	endif
			}
#else
			(void)vm;
			(void)expected_lua_type;
			(void)expected_type;
#endif
			return true;
		}

		bool cpp_runtime_type_check_of_top(lua_State* vm, compare_lua_type_func_sig compareFunc, char const * expected_type)
		{
#if OOLUA_RUNTIME_CHECKS_ENABLED == 1
			if(!compareFunc(vm, -1))
			{
				handle_cpp_pull_fail(vm, expected_type);
#	if OOLUA_USE_EXCEPTIONS == 0//prevent vs warnings
				return false;//dependant on config if the flow can return through this path
#	endif
			}
#else
			(void)vm;
			(void)compareFunc;
			(void)expected_type;
#endif
			return true;
		}

	} // namespace INTERNAL //NOLINT

	bool pull(lua_State* const vm, void*& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TLIGHTUSERDATA, "light userdata") ) return false;
		value = lua_touserdata(vm, -1);
		return true;
	}

	bool pull(lua_State* const vm, bool& value)
	{
		/*
		If it is allowed to pull a bool from an int, check for number instead of boolean
		if(! INTERNAL::cpp_runtime_type_check_of_top(s,lua_isnumber,"bool") ) return false;
		 */
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TBOOLEAN, "bool") ) return false;
		value = lua_toboolean(vm, -1) ? true : false;
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, double& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TNUMBER, "double") ) return false;
		value = static_cast<double>(lua_tonumber(vm, -1));
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, float& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TNUMBER, "float") ) return false;
		value = static_cast<float>(lua_tonumber(vm, -1));
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, lua_CFunction& value)
	{
		if( !INTERNAL::cpp_runtime_type_check_of_top(vm, lua_iscfunction, "lua_CFunction") ) return false;
		value = lua_tocfunction(vm, -1);
		lua_pop(vm, 1);
		return true;
	}

	bool pull(lua_State* const vm, Table& value)
	{
		return value.pull_from_stack(vm);
	}

} //namespace OOLUA
