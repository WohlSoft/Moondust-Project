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

#ifndef OOLUA_CHAR_ARRAYS_H_
#	define OOLUA_CHAR_ARRAYS_H_

/** \cond INTERNAL*/
/**
	\file char_arrays.h
	\brief This file is to do away with magic string literals whilst also not
	paying the cost of a strlen
*/

namespace OOLUA
{
	namespace INTERNAL
	{
		static const char change_mt_to_none_const_field[] = "__change_mt_to_none_const";
		static const char set_owner_str[] = "set_owner";
		static const char lua_owns_str[] = "Lua_owns";
		static const char cpp_owns_str[] = "Cpp_owns";
		static const char weak_lookup_name [] = "__weak_lookup";
		static const char new_str [] = "new";
		static const char oolua_str [] = "OOLua";
	} // namespace INTERNAL
} // namespace OOLUA

#define OOLUA_PUSH_CARRAY(lua, carray) lua_pushlstring(lua, carray, sizeof(carray)-1)

/** \endcond */
#endif
