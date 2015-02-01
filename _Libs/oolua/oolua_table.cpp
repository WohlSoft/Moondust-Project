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

#	include "oolua_table.h"
//#	include "oolua_exception.h"
#	include "oolua_stack_fwd.h"

namespace OOLUA
{
	Table::Table()
	{}

	Table::Table(Lua_table_ref const& ref)
		:m_table_ref(ref)
	{}

	Table::Table(lua_State*  const vm, std::string const& name)
		:m_table_ref(vm)
	{
		set_table(name);
	}

	Table::Table(Table const& rhs)
		:m_table_ref(rhs.m_table_ref)
	{}

	void Table::bind_script(lua_State*  const vm)
	{
		if(m_table_ref.m_lua == vm)return;
		if(m_table_ref.valid() )
		{
			Lua_table_ref tempRef(vm);
			m_table_ref.swap(tempRef);
		}
		else m_table_ref.m_lua = vm;
	}

	void Table::set_table(std::string const& name)
	{
		if(name.empty())
		{
			Lua_table_ref t;
			m_table_ref.swap(t);
			return;
		}
		if(!m_table_ref.m_lua)return;

		lua_getglobal(m_table_ref.m_lua,  name.c_str() );
		if(lua_type(m_table_ref.m_lua, -1) != LUA_TTABLE)
		{
			lua_pop(m_table_ref.m_lua, 1);
			lua_getfield(m_table_ref.m_lua, LUA_REGISTRYINDEX, name.c_str() );
			if(lua_type(m_table_ref.m_lua, -1) != LUA_TTABLE)
			{
				lua_pop(m_table_ref.m_lua,  1);
				Lua_table_ref t;
				m_table_ref.swap(t);
				return;
			}
		}
		set_ref(m_table_ref.m_lua, luaL_ref(m_table_ref.m_lua, LUA_REGISTRYINDEX));
	}

	bool Table::valid()const
	{
		int const init_stack_top = initial_stack_size();
		bool result = get_table();
		restore_stack(init_stack_top);
		return result;
	}

	void Table::set_ref(lua_State* const vm, int const& ref)
	{
		m_table_ref.set_ref(vm, ref);
	}

	bool Table::get_table()const
	{
		bool result(false);
		if( !m_table_ref.valid() )return result;
		lua_rawgeti(m_table_ref.m_lua, LUA_REGISTRYINDEX, m_table_ref.ref() );
		return  lua_type(m_table_ref.m_lua, -1) == LUA_TTABLE;
	}

	bool Table::push_on_stack(lua_State* vm)const
	{
		return m_table_ref.push(vm);
	}

	bool Table::pull_from_stack(lua_State* vm)
	{
		return m_table_ref.pull(vm);
	}

	void Table::lua_get(lua_State* vm, int idx)
	{
		m_table_ref.lua_get(vm, idx);
	}

	void Table::restore_stack(int const & init_stack_size)const
	{
		//ok now we need to clean up the stack if there are left overs
		if(!m_table_ref.m_lua)return;
		int end_stack_size(lua_gettop(m_table_ref.m_lua));
		if(init_stack_size != end_stack_size)
		{
			lua_pop(m_table_ref.m_lua, end_stack_size - init_stack_size);
		}
	}

	int Table::initial_stack_size()const
	{
		return (!m_table_ref.m_lua) ? 0 : lua_gettop(m_table_ref.m_lua);
	}

	void Table::traverse(Table::traverse_do_function do_)
	{
		Table& t = *this;
		oolua_pairs(t)
		{
			(*do_)(lvm);
			lua_pop(lvm, 1);
		}
		oolua_pairs_end()
	}

	void Table::swap(Table & rhs)
	{
		m_table_ref.swap(rhs.m_table_ref);
	}

	void new_table(lua_State* vm, Table& t)
	{
		new_table(vm).swap(t);
	}

	Table new_table(lua_State* vm)
	{
		lua_newtable(vm);
		Table t;
		t.pull_from_stack(vm);
		return t;
	}

} // namespace OOLUA

