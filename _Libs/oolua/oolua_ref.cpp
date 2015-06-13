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

#include "oolua_ref.h"
#include "oolua_config.h"
#include "oolua_error.h"
#include "oolua_helpers.h"

namespace OOLUA
{
	namespace INTERNAL
	{
		bool handle_unrelated_states(lua_State* const from, lua_State* const to)
		{
#if OOLUA_RUNTIME_CHECKS_ENABLED == 1
#	if OOLUA_USE_EXCEPTIONS ==1
			lua_pushfstring(to, "Can not push a valid Lua reference from lua_State(%p) to an unrelated lua_State(%p)", from, to);
			throw OOLUA::Runtime_error(to, (OOLUA::ERROR::PopTheStack*)0);
#	endif
#	if OOLUA_STORE_LAST_ERROR ==1
			lua_pushfstring(to, "Can not push a valid Lua reference from lua_State(%p) to an unrelated lua_State(%p)", from, to);
			OOLUA::INTERNAL::set_error_from_top_of_stack_and_pop_the_error(to);
#	endif
#	if OOLUA_DEBUG_CHECKS == 1
			//assert(0 && "Can not push a valid Lua reference from one lua_State to a different state");
#	endif
#	if OOLUA_USE_EXCEPTIONS == 0
			return false;
#	endif
#else
			(void)from;
			(void)to;
			return true;
#endif
		}


		void pull_handle_invalid(lua_State* const vm, int id)
		{
#if OOLUA_STORE_LAST_ERROR == 1 || 	OOLUA_USE_EXCEPTIONS == 1
			lua_pushfstring(vm
							, "pulling incorrect type from stack. This is a Lua registry reference to %s, stack contains %s"
							, lua_typename(vm, id)
							, lua_gettop(vm) ? lua_typename(vm, lua_type(vm, -1)) : "empty stack");
#	if OOLUA_USE_EXCEPTIONS == 1
			throw OOLUA::Runtime_error(vm, (OOLUA::ERROR::PopTheStack*)0);
#	else // OOLUA_STORE_LAST_ERROR == 1
			OOLUA::INTERNAL::set_error_from_top_of_stack_and_pop_the_error(vm);
#	endif
#elif OOLUA_DEBUG_CHECKS == 1
			assert(0 && "pulling incorrect type from stack");
#endif
		}

		bool push_reference_if_possible(int const ref, lua_State* const from, lua_State* const to)
		{
			if (from == to || OOLUA::can_xmove(from, to) )
			{
				lua_rawgeti(to, LUA_REGISTRYINDEX, ref);
				return true;
			}
			return handle_unrelated_states(from, to);
		}
	} // namespace INTERNAL //NOLINT
} // namespace OOLUA
