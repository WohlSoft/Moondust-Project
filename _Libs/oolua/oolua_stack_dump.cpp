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
#include "oolua_stack_dump.h"
#include <iostream> //NOLINT
#include <string>

namespace OOLUA
{

void lua_stack_dump(lua_State * const vm)
{
	int i;

	int top = lua_gettop(vm);
	std::cout <<"Lua stack dump - number of nodes: " <<top <<std::endl;

	for (i = 1; i <= top; i++)
	{  /* repeat for each level */
		int t = lua_type(vm, i);
		switch (t)
		{
		case LUA_TSTRING:
			{
				size_t len(0);
				char const* str = lua_tolstring(vm, -1, &len);
				std::string value(std::string(str, len) );
				std::cout <<"LUA_TSTRING :" <<value;
			}
			break;

		case LUA_TBOOLEAN:
			std::cout <<"LUA_TBOOLEAN :" <<(lua_toboolean(vm, i) ? "true" : "false");
			break;

		case LUA_TNUMBER:
			std::cout <<"LUA_TNUMBER :" <<lua_tonumber(vm, i);
			break;

		default:
			std::cout <<lua_typename(vm, t);
			break;
		}
		std::cout <<"  ";
	}
	std::cout <<std::endl;
}
} // namespace OOLUA

