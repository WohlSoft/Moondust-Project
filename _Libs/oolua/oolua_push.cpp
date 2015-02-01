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

#include "oolua_push.h"
#include "oolua_table.h"

#include "push_pointer_internal.h"

namespace OOLUA
{
	bool push(lua_State* const vm, void * lightud)
	{
		lua_pushlightuserdata(vm, lightud);
		return true;
	}

	bool push(lua_State* const vm, bool const& value)
	{
		assert(vm);
		lua_pushboolean(vm, (value? 1 : 0));
		return true;
	}

	bool push(lua_State* const vm, char const * const& value)
	{
		assert(vm && value);
		lua_pushstring(vm, value);
		return true;
	}

	bool push(lua_State* const vm, char * const& value)
	{
		assert(vm && value);
		lua_pushstring(vm, value);
		return true;
	}

	bool push(lua_State* const vm, double const& value)
	{
		assert(vm);
		lua_pushnumber(vm, value);
		return true;
	}

	bool push(lua_State* const vm, float const&  value)
	{
		assert(vm);
		lua_pushnumber(vm, value);
		return true;
	}

	bool push(lua_State* const vm, lua_CFunction const &  value)
	{
		assert(vm);
		lua_pushcclosure(vm, value, 0);
		return true;
	}

	bool push(lua_State* const vm, Table const &  value)
	{
		assert(vm);
		return value.push_on_stack(vm);
	}

} //namespace OOLUA
