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
	\file lua_includes.h
	\brief Prevents name mangling and provides a potential location to enable
	compatibility when new Lua versions are released.
	\details No part of OOLua directly includes any Lua header files, instead
	when required they include this header. Contrary to what some people may think,
	this is by design. There is no way to know if a user's version of the Lua
	library was compiled as C++ or C.
*/

#ifndef LUA_INCLUDES_H_
#	define LUA_INCLUDES_H_

//Prevent name mangling
extern "C"
{
#	include "lua/lua.h"
#	include "lua/lauxlib.h"
#	include "lua/lualib.h"
}

#endif //LUA_INCLUDES_H_
