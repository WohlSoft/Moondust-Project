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

#	include "oolua_check_result.h"
#	include "lua_includes.h"
#	include "oolua_error.h"
#	include "oolua_config.h"

namespace OOLUA
{
	namespace INTERNAL
	{
		bool protected_call_check_result(lua_State* vm, int pcall_result)
		{
			if (pcall_result == 0)return true;

#if OOLUA_STORE_LAST_ERROR == 1
			if (pcall_result != LUA_ERRMEM)set_error_from_top_of_stack_and_pop_the_error(vm);
#elif OOLUA_USE_EXCEPTIONS == 1
			if (pcall_result == LUA_ERRRUN)
				throw OOLUA::Runtime_error(vm, (OOLUA::ERROR::PopTheStack*)0);
			else if (pcall_result == LUA_ERRMEM)
				throw OOLUA::Memory_error(vm, (OOLUA::ERROR::PopTheStack*)0);
			else if (pcall_result == LUA_ERRERR)
				throw OOLUA::Runtime_error(vm, (OOLUA::ERROR::PopTheStack*)0);
#elif OOLUA_DEBUG_CHECKS == 1
			(void)vm;
			if (pcall_result == LUA_ERRRUN)
				assert(0 && "LUA_ERRRUN");
			else if (pcall_result == LUA_ERRMEM)
				assert(0 && "LUA_ERRMEM");
			else if (pcall_result == LUA_ERRERR)
				assert(0 && "LUA_ERRERR");
#else
			(void)vm;
#endif
			return false;
		}


		bool load_buffer_check_result(lua_State* vm, int result)
		{
			if (result == 0)return true;
#if OOLUA_STORE_LAST_ERROR == 1
			if (result != LUA_ERRMEM)set_error_from_top_of_stack_and_pop_the_error(vm);
#elif OOLUA_USE_EXCEPTIONS == 1
			if (result == LUA_ERRFILE)
				throw OOLUA::File_error(vm, (OOLUA::ERROR::PopTheStack*)0);
			else if (result == LUA_ERRSYNTAX)
				throw OOLUA::Syntax_error(vm, (OOLUA::ERROR::PopTheStack*)0);
			else if (result == LUA_ERRMEM )
				throw OOLUA::Memory_error(vm, (OOLUA::ERROR::PopTheStack*)0);
#elif OOLUA_DEBUG_CHECKS == 1
			(void)vm;
			if (result == LUA_ERRSYNTAX)
				assert(0 && "syntax error");
			else if (result == LUA_ERRMEM)
				assert(0 && "memory error");
			else if (result == LUA_ERRFILE)
					assert(0 && "file error");
#else
			(void)vm;
#endif
			return false;
		}
	} // namespace INTERNAL //NOLINT
} // namespace OOLUA
