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

#include "oolua_script.h"
#include "oolua_chunk.h"
#include "oolua_open.h"
#include "oolua_registration.h"

namespace OOLUA
{

	Script::Script()
		:call(), m_lua(0)
	{
		m_lua = luaL_newstate();
		luaL_openlibs(m_lua);
		call.bind_script(m_lua);//bind the lua state to the function caller
		setup_user_lua_state(m_lua);
	}

	Script::~Script()
	{
		close_down();
	}

	void Script::gc()
	{
		lua_gc(m_lua, LUA_GCCOLLECT, 0);
	}

	void Script::close_down()
	{
		if(m_lua)
		{
			lua_gc(m_lua, LUA_GCCOLLECT, 0);
			lua_close(m_lua);
			m_lua = 0;
		}
	}

	bool Script::load_chunk(std::string const& chunk)
	{
		return OOLUA::load_chunk(m_lua, chunk);
	}

	bool Script::run_chunk(std::string const& chunk)
	{
		return OOLUA::run_chunk(m_lua, chunk);
	}

	bool Script::run_file(std::string const & filename)
	{
		return OOLUA::run_file(m_lua, filename);
	}

	bool Script::load_file(std::string const & filename)
	{
		return OOLUA::load_file(m_lua, filename);
	}

} // namespace OOLUA
