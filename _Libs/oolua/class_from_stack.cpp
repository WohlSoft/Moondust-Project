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

/**
	\file class_from_stack.cpp
	\brief Implement none inline index_is_userdata functions and error generator
*/
#include "class_from_stack.h"
#include "lua_includes.h"
#include "lvd_types.h"

/** \cond INTERNAL */
namespace OOLUA
{
	namespace INTERNAL
	{

#if OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA == 1
#	if OOLUA_USERDATA_OPTIMISATION == 1
		bool index_is_userdata(lua_State* vm, int index, Lua_ud*& ud)
		{
#if defined LUA_VERSION_NUM && LUA_VERSION_NUM >= 502
/*lua_objlen may or may not be a macro for lua_rawlen in luaconfig.h for 5.2
 so lets just work regardless of the configuration used*/
#	define _oolua_len_ lua_rawlen
#else
#	define _oolua_len_ lua_objlen
#endif
			ud = static_cast<Lua_ud *>(lua_touserdata(vm, index));
			return ud && _oolua_len_(vm, index) == sizeof(Lua_ud) && OOLUA_CHECK_COOKIE(ud->flags);
#undef _oolua_len_
		}
#	else

		bool index_is_userdata(lua_State* vm, int index, Lua_ud*& ud)
		{
			ud = static_cast<Lua_ud *>(lua_touserdata(vm, index));
			if(!ud || !lua_getmetatable(vm, index)) return false;
			lua_pushlightuserdata(vm, lua_topointer(vm, LUA_REGISTRYINDEX));
			lua_rawget(vm, -2);
			bool result = lua_isnil(vm, -1) == 0;
			lua_pop(vm, 2);
			return result;
		}

#	endif

#endif

		void report_error_userdata_is_constant(lua_State* const vm, char const* pulling_class_type)
		{
			luaL_error(vm, "%s \"%s\" %s", "Tried to pull a none constant"
						, pulling_class_type
						, "pointer from a const pointer");
		}

	} // namespace INTERNAL //NOLINT
} // namespace OOLUA
/** \endcond  */
