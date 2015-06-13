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

#include "lua_includes.h"
#include "push_pointer_internal.h"
#include "oolua_config.h"

namespace OOLUA
{
	/** \cond INTERNAL*/
	namespace INTERNAL
	{
#if OOLUA_DEBUG_CHECKS == 1
		void if_check_enabled_check_type_is_registered(lua_State* vm, char const* name)
		{
			lua_getfield(vm, LUA_REGISTRYINDEX, name);
			if( lua_isnil(vm, -1) )
			{
				lua_pop(vm, 1);
				luaL_error(vm, "%s %s %s", "the type", name, "is not registered with this Lua State");
			}
			else lua_pop(vm, 1);
		}
#else
		void if_check_enabled_check_type_is_registered(lua_State* /*vm*/, char const* /*name*/){}
#endif

		void set_owner_if_change(Owner owner, Lua_ud* ud)
		{
			if(owner != No_change)
			{
				userdata_gc_value(ud, owner == Lua ? true : false);
			}
		}

	} // namespace INTERNAL //NOLINT
	/** \endcond*/
} // namespace OOLUA
