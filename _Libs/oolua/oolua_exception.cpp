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

#include "oolua_exception.h"
#	if OOLUA_USE_EXCEPTIONS == 1

#	include "lua_includes.h"
#	include <cstring>

namespace
{
	void copy_buffer(char* to, char const* from, size_t& sz)
	{
		sz = sz + 1 < OOLUA::ERROR::size ? sz : OOLUA::ERROR::size-2;
		memcpy(to, from, sz);
		to[sz < OOLUA::ERROR::size -2 ? sz : OOLUA::ERROR::size -1] = '\0';
	}
	size_t copy_buffer(char* to, char const* from)
	{
		size_t sz = strlen(from);
		copy_buffer(to, from, sz);
		return sz;
	}
} // namespace

namespace OOLUA
{
	Exception::Exception(lua_State* vm)
		: m_len(0)
	{
		char const* str = lua_tolstring(vm, -1, &m_len);
		copy_buffer(m_buffer, str, m_len);
	}

	Exception::Exception(lua_State* vm, ERROR::PopTheStack*)
		: m_len(0)
	{
		char const* str = lua_tolstring(vm, -1, &m_len);
		copy_buffer(m_buffer, str, m_len);
		lua_pop(vm, 1);
	}

	Exception::Exception(char const* msg)
		: m_len(0)
	{
		m_len = copy_buffer(m_buffer, msg);
	}

	Exception::Exception(Exception const& rhs)
		: std::exception(rhs)
		, m_len(rhs.m_len)
	{
		copy_buffer(m_buffer, rhs.m_buffer, m_len);
	}

	Exception& Exception::operator = (Exception const& rhs) throw()
	{
		m_len = rhs.m_len;
		copy_buffer(m_buffer, rhs.m_buffer, m_len);
		return *this;
	}

	char const* Exception::what() const throw()
	{
		return &m_buffer[0];
	}


	Syntax_error::Syntax_error(lua_State* vm)
		: Exception(vm)
	{}
	Syntax_error::Syntax_error(lua_State* vm, ERROR::PopTheStack* specialisation)
		: Exception(vm, specialisation)
	{}

	Runtime_error::Runtime_error(lua_State* vm)
		: Exception(vm)
	{}
	Runtime_error::Runtime_error(lua_State* vm, ERROR::PopTheStack* specialisation)
		: Exception(vm, specialisation)
	{}
	Runtime_error::Runtime_error(char const* msg)
		: Exception(msg)
	{}

	Memory_error::Memory_error(lua_State* vm)
		: Exception(vm)
	{}
	Memory_error::Memory_error(lua_State* vm, ERROR::PopTheStack* specialisation)
		: Exception(vm, specialisation)
	{}

	File_error::File_error(lua_State* vm)
		: Exception(vm)
	{}
	File_error::File_error(lua_State* vm, ERROR::PopTheStack* specialisation)
		: Exception(vm, specialisation)
	{}

	Type_error::Type_error(lua_State* vm, ERROR::PopTheStack* specialisation)
		:Exception(vm, specialisation)
	{}
} // namespace OOLUA

#endif //OOLUA_USE_EXCEPTIONS
