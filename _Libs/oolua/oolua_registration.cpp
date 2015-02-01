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

#include "oolua_registration.h"
#include "proxy_constructor.h"

namespace OOLUA
{
	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		int get_oolua_module(lua_State* vm)
		{
			lua_getfield(vm, LUA_REGISTRYINDEX, OOLUA::INTERNAL::oolua_str);
			return 1;
		}
		void register_oolua_type(lua_State* vm, char const* name, int const stack_index)
		{
			get_oolua_module(vm);
			lua_pushstring(vm, name);
			lua_pushvalue(vm, stack_index);
			lua_rawset(vm, -3);
			lua_pop(vm, 1);
		}

		void set_function_in_table_with_upvalue(lua_State* vm, char const * func_name, lua_CFunction func
													, int tableIndex, void* upvalue)
		{
			lua_pushstring(vm, func_name);
			lua_pushlightuserdata(vm, upvalue);
			lua_pushcclosure(vm, func, 1);
			lua_settable(vm, tableIndex);
		}
		void set_function_in_table(lua_State* vm, char const* func_name, lua_CFunction func, int tableIndex)
		{
			lua_pushstring(vm, func_name);
			lua_pushcclosure(vm, func, 0);
			lua_settable(vm, tableIndex);
		}

		void set_oolua_userdata_creation_key_value_in_table(lua_State* vm, int tableIndex)
		{
#if OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA == 1 && OOLUA_USERDATA_OPTIMISATION == 0
			lua_pushlightuserdata(vm, lua_topointer(vm, LUA_REGISTRYINDEX));
			lua_pushboolean(vm, 1);
			lua_settable(vm, tableIndex);
#else
			(void)vm;
			(void)tableIndex;
#endif
		}

		void set_key_value_in_table(lua_State* vm, char const* key_name, int valueIndex, int tableIndex)
		{
			lua_pushstring(vm, key_name);
			lua_pushvalue(vm, valueIndex);
			lua_settable(vm, tableIndex);
		}

		int check_for_key_in_stack_top(lua_State* vm)
		{
			//on entry stack is : table keyString basetable
			lua_pushvalue(vm, -2);//table keyString baseTable keyString
			lua_gettable(vm, -2);//check for keyString in baseTable
			if( lua_isnil(vm, -1) == 1)//table string baseTable valueOrNil
			{
				lua_pop(vm, 2);//table keyString
				return 0;
			}

			lua_remove(vm, -2);//table keyString TheValueThatWeWereLookingFor
			return 1;
		}

		bool class_name_is_already_registered(lua_State* vm, char const* name)
		{
			lua_getfield(vm, LUA_REGISTRYINDEX, name);
			if( lua_isnil(vm, -1) == 0 )// entryOrNil
			{
				lua_pop(vm, 1);//pop the table
				return true;
			}

			lua_pop(vm, 1);//pop the null
			return false;
		}

	} // namespace INTERNAL // NOLINT
	/**\endcond*/
} // namespace OOLUA

