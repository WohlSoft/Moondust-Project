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
	\file oolua_open.h
	\brief Sets up the a Lua Universe to work with the library.
*/
#ifndef OOLUA_OPEN_H_
#	define OOLUA_OPEN_H_

struct lua_State;

namespace OOLUA
{
	/**
		\brief Sets up a lua_State to work with OOLua.
		\details If you want to use OOLua with a lua_State you already have active
		or supplied by some third party, then calling this function
		adds the necessary tables and globals for it to work with OOLua.
		\param[in] vm lua_State to be initialise by OOLua
	*/
	void setup_user_lua_state(lua_State* vm);

} // namespace OOLUA

#endif //OOLUA_OPEN_H_
