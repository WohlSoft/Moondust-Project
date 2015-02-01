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

namespace OOLUA
{

	bool idxs_equal(lua_State* vm, int idx0, int idx1)
	{
#	if LUA_VERSION_NUM < 502
		return !!lua_equal(vm, idx0, idx1);
#	else
		return !!lua_compare(vm, idx0, idx1, LUA_OPEQ);
#	endif
	}


	bool can_xmove(lua_State* vm0, lua_State* vm1)
	{
		if(!vm0 || !vm1 || vm0 == vm1) return false;

		/*
		Threads that are related share the same registry
		G(vm0)->l_registry == G(vm1)->l_registry
		*/
		return lua_topointer(vm0, LUA_REGISTRYINDEX) == lua_topointer(vm1, LUA_REGISTRYINDEX);
	}

} // namespace OOLUA
