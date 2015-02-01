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

#include "oolua_chunk.h"
#include "oolua_check_result.h"
#include "lua_includes.h"

namespace OOLUA
{

	bool load_chunk(lua_State* vm, std::string const& chunk)
	{
#ifdef DEBUG
#	define chunk_name chunk.c_str()
#else
#	define chunk_name "userChunk"
#endif

		int res = luaL_loadbuffer(vm, chunk.c_str(), chunk.size(), chunk_name);
#undef chunk_name
		return INTERNAL::load_buffer_check_result(vm, res);
	}

	bool run_chunk(lua_State* vm, std::string const& chunk)
	{
		if( !load_chunk(vm, chunk) ) return false;
		int result = lua_pcall(vm, 0, LUA_MULTRET, 0);
		return INTERNAL::protected_call_check_result(vm, result);
	}

	bool run_file(lua_State* vm, std::string const & filename)
	{
		bool status = load_file(vm, filename);
		if(!status)return false;
		int result = lua_pcall(vm, 0, LUA_MULTRET, 0);
		return INTERNAL::protected_call_check_result(vm, result);
	}

	bool load_file(lua_State* vm, std::string const & filename)
	{
		int result = luaL_loadfile(vm, filename.c_str() );
		return INTERNAL::load_buffer_check_result(vm, result);;
	}

} //namespace OOLUA
