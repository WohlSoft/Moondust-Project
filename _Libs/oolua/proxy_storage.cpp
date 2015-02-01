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

#include "proxy_storage.h"
#include "lua_includes.h"
#include "proxy_userdata.h"
#include "oolua_stack.h"


#if OOLUA_DEBUG_CHECKS == 1
#	include <cassert>
#endif

namespace OOLUA
{

	/** \cond INTERNAL*/
	namespace INTERNAL
	{

		//pushes the weak table on top and returns its absolute index
		//The weak table is a table in the Lua registry specific to OOLua,
		//which has void pointer keys and values of userdata pointers.
		int push_weak_table(lua_State* vm)
		{
			Weak_table::getWeakTable(vm);
			return lua_gettop(vm);
		}

		//if found it is left on the top of the stack and returns true
		//else the stack is same as on entrance to the function and false returned
		bool is_there_an_entry_for_this_void_pointer(lua_State* vm, void* ptr)
		{
			int wt = push_weak_table(vm);
			bool result = is_there_an_entry_for_this_void_pointer(vm, ptr, wt);
			lua_remove(vm, wt);
			return result;
		}

		bool is_there_an_entry_for_this_void_pointer(lua_State* vm, void* ptr, int tableIndex)
		{
			lua_pushlightuserdata(vm, ptr);//weakTable ... ptr
			lua_rawget(vm, tableIndex);//weakTable .... (full user data or nil)
			if( !lua_isnil(vm, -1) )
			{
				return true;//leave ud on top
			}
			lua_pop(vm, 1);//pop nil
			return false;
		}

		//on entering user data and weaktable are on the stack
		void add_ptr_if_required(lua_State* const vm, void* ptr, int udIndex, int weakIndex)
		{
			lua_pushlightuserdata(vm, ptr);//ptr
			lua_rawget(vm, weakIndex);//(null or ptr)
			if( lua_isnil(vm, -1) == 0 )
			{
				lua_pop(vm, 1);//pop the ud
				return;
			}
			lua_pop(vm, 1);//pop the null

			lua_pushlightuserdata(vm, ptr);//key
			lua_pushvalue(vm, udIndex);//key ud
			lua_rawset(vm, weakIndex);//table[key]=value
		}

		bool ud_at_index_is_const(lua_State* vm, int index)
		{
			return INTERNAL::userdata_is_constant( static_cast<Lua_ud *>( lua_touserdata(vm, index) ) );
		}

		Lua_ud* new_userdata(lua_State* vm, void* ptr, bool is_const, oolua_function_check_base base_checker, oolua_type_check_function type_check)
		{
			Lua_ud* ud = static_cast<Lua_ud*>(lua_newuserdata(vm, sizeof(Lua_ud)));
			ud->flags = 0;
			reset_userdata(ud, ptr, is_const, base_checker, type_check);
#if OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA == 1 && OOLUA_USERDATA_OPTIMISATION == 1
			OOLUA_SET_COOKIE(ud->flags);
#endif
			return ud;
		}

		void reset_userdata(Lua_ud* ud, void* ptr, bool is_const, oolua_function_check_base base_checker, oolua_type_check_function type_check)
		{
			ud->void_class_ptr = ptr;
			ud->base_checker = base_checker;
			ud->type_check = type_check;
			userdata_const_value(ud, is_const);
		}

	} // namespace INTERNAL //NOLINT
	/**\endcond*/
} // namespace OOLUA
