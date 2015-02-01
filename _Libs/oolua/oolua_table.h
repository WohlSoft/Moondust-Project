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
	\file oolua_table.h
	\details Wrapper around a table in Lua which allows easy usage.
*/

#ifndef OOLUA_TABLE_H_
#	define OOLUA_TABLE_H_

#	include "lua_includes.h"
#	include <string>
#	include "oolua_stack_fwd.h"
#	include "oolua_ref.h"
#	include "oolua_config.h"
#	include "oolua_error.h"


namespace OOLUA
{
	/**	\class Table
		\brief Wrapper around a table in Lua which allows easy usage
		\details
		\ref OOLUA::Table "Table" provides a simple typed C++ interface for the
		Lua unordered and ordered associative container of the same name. Operations
		which use the Lua stack ensure that the stack is the same on exit
		as it was on entry, OOLua tries to force a clean stack(\ref OOLuaAndTheStack).

		Any value can be retrieved or set from the table via the use of the template
		member functions \ref OOLUA::Table::set "set", \ref OOLUA::Table::at "at" or
		\ref OOLUA::Table::safe_at "safe_at".
		If the value asked for is not the correct type located in the position an
		error can be reported, the type of which depends on \ref OOLuaErrorReporting
		and the function which was called. See individual member function documentation
		for details.

		\note
		The member function \ref OOLUA::Table::try_at "try_at" is only defined when exceptions
		are enabled for the library.
	*/
	class Table
	{
	public:
		/**@{**/

		/** \brief Default creates an object on which a call to valid returns false*/
		Table();

		/** \brief
			Initialises the reference to be an instance of the same registry
			reference or an invalid table if ref.valid() == false
		*/
		explicit Table(Lua_table_ref const& ref);

		/** \brief Sets the lua_State and calls Lua_table::set_table */
		Table(lua_State*  const vm, std::string const& name);
		Table(Table const& rhs);

		/** \brief unimplemented*/
		Table& operator =(Table const& /*rhs*/);

		/**@}*/

		~Table()OOLUA_DEFAULT;

		/**@{*/
		/**
			\brief Associates the instance with the lua_State vm
			\details Associates the instance with the lua_State vm.
			If the table already has a lua_State bound to it
				\li If the Current bound instance is not equal to vm and the
				table has a valid reference, it releases the currently set reference
				and sets vm as the bound instance.
		*/
		void bind_script(lua_State*  const vm);

		/** \brief
			Order of trying to initialise :
			\li name.empty() == true: Creates an invalid object.
			\li name found as a table in Lua global: Swaps the internal Lua_func_ref
				with an instance initialised to an id obtained from the Lua registry.
			\li name found as a table in Lua registry: Swaps the internal Lua_func_ref
				with an instance initialised to an id obtained from the Lua registry.
			\li else Swaps the internal Lua_func_ref with an uninitialised instance.
		*/
		void set_table(std::string const& name);

		/** \brief Initailises the internal Lua_func_ref to the id ref*/
		void set_ref(lua_State* const vm, int const& ref);

		/** \brief Swaps the internal Lua_func_ref and rhs.m_table_ref*/
		void swap(Table & rhs);

		/**@}*/

		/**@{*/
#if OOLUA_USE_EXCEPTIONS == 1 || OOLUA_DOXYGEN_IS_PROCESSING == 999
		/**
			\brief Function which throws on an error
			\note This function is only defined when exceptions are enable for the library
			\tparam T Key type
			\tparam T1 Value type
			\param[in] key
			\param[out] value
		*/
		template<typename T, typename T1>void try_at(T const& key, T1& value);
#endif
		/**
			\brief A safe version of \ref OOLUA::Table::at "at", which will always return
			a boolean indicating the success of the function call.
			\details This function will not throw an exception when exceptions are enabled
			for the library.
			\tparam T Key type
			\tparam T1 Value type
			\param[in] key
			\param[out] value
		*/
		template<typename T, typename T1>bool safe_at(T const& key, T1& value);

		/**
			\brief
			\tparam T Key type
			\tparam T1 Value type
			\param[in] key
			\param[out] value
			zreturn The same instance as value
			\note
			No error checking.\n
			It is undefined to call this function when:
			\li table or the key are invalid
			\li table does not contain the key
			\li value is not the correct type

			\see Lua_table::safe_at \see Lua_table::try_at
		 */
		template<typename T, typename T1>T1& at(T const& key, T1& value);
		/**@}*/

		/**@{*/
		/** \brief
			Inserts the key value pair into the table if key is not present
			else it updates the table's key entry
		*/
		template<typename T, typename T1>void set(T const& key, T1 const& value);
		/** \brief
			Removes the key from the table by setting it's value to nil
		*/
		template<typename T>void remove(T const& key);
		/**@}*/

		/** \brief
			Returns a boolean which is the result of checking the state of the internal Lua_func_ref.
		*/
		bool valid()const;

		typedef void(*traverse_do_function)(lua_State* );//NOLINT
		/** \deprecated*/
		void traverse(traverse_do_function do_);

		/**
			\brief Provides access to the associated lua_State
		*/
		lua_State* state() const { return m_table_ref.m_lua; }


		/** \cond INTERNAL*/
		bool push_on_stack(lua_State* vm)const;
		bool pull_from_stack(lua_State* vm);
		void lua_get(lua_State* vm, int idx);
		/** \endcond*/
	private:
		bool get_table()const;
		void restore_stack(int const & init_stack_size)const;
		int initial_stack_size()const;
		Lua_table_ref m_table_ref;
	};

	inline Table::~Table(){}


	//no error checking
	//undefined if lua is null or (table or key is invalid) or value is not correct type
	template<typename T, typename T1>
	inline T1& Table::at(T const& key, T1& value)
	{
		get_table();//table
		push(m_table_ref.m_lua, key);//table key
		lua_gettable(m_table_ref.m_lua, -2);//table value
		pull(m_table_ref.m_lua, value);//table
		lua_pop(m_table_ref.m_lua, 1);
		return value;
	}

	template<typename T, typename T1>
	inline void Table::set(T const& key, T1 const& value)
	{
		//record the stack size as we want to put the stack into the
		//same state that it was before entering here
		int const init_stack_size = initial_stack_size();
		if(!get_table())return;
		push(m_table_ref.m_lua, key);
		//table is now at -2 (key is at -1)
		//push the new value onto the stack
		push(m_table_ref.m_lua, value);
		//table is not at -3 set the table
		lua_settable(m_table_ref.m_lua, -3);

		restore_stack(init_stack_size);
	}

	template<typename T>
	inline void Table::remove(T const& key)
	{
		//record the stack size as we want to put the stack into the
		//same state that it was before entering here
		int const init_stack_size = initial_stack_size();
		if(!get_table())return;
		push(m_table_ref.m_lua, key);
		//table is now at -2 (key is at -1)
		//push the new value onto the stack
		lua_pushnil(m_table_ref.m_lua);
		//table is not at -3 set the table
		lua_settable(m_table_ref.m_lua, -3);

		restore_stack(init_stack_size);
	}

#if OOLUA_USE_EXCEPTIONS == 1
	template<typename T, typename T1>
	inline void Table::try_at(T const& key, T1& value)
	{
		int const init_stack_size = initial_stack_size();
		try
		{
			if(!get_table())throw OOLUA::Runtime_error("Table is invalid");
			push(m_table_ref.m_lua, key);
			lua_gettable(m_table_ref.m_lua, -2);
			if(lua_type(m_table_ref.m_lua, -1) == LUA_TNIL )
			{
				throw OOLUA::Runtime_error("key is not present in table");
			}
			pull(m_table_ref.m_lua, value);
			restore_stack(init_stack_size);
		}
		catch(...)
		{
			restore_stack(init_stack_size);
			throw;
		}
	}

	template<typename T, typename T1>
	inline bool Table::safe_at(T const& key, T1& value)
	{
		try
		{
			try_at(key, value);
		}
		catch(...)
		{
			return false;
		}
		return true;
	}
#else

	template<typename T, typename T1>
	inline bool Table::safe_at(T const& key, T1& value)
	{
		//record the stack size as we want to put the stack into the
		//same state that it was before entering here
		int const init_stack_size = initial_stack_size();
		if( !get_table() )return false;
		if( !push(m_table_ref.m_lua, key))
		{
			restore_stack(init_stack_size);
			return false;
		}
		//table is now at -2 (key is at -1). lua_gettable now pops the key off
		//the stack and then puts the data found at the key location on the stack
		lua_gettable(m_table_ref.m_lua, -2);
		if(lua_type(m_table_ref.m_lua, -1) == LUA_TNIL )
		{
			restore_stack(init_stack_size);
			return false;
		}
		pull(m_table_ref.m_lua, value);
		restore_stack(init_stack_size);

		return true;
	}
#endif


	/**	\brief
		The table is at table_index which can be either absolute or pseudo in the stack
		table is left at the index.
	*/
	template<typename T, typename T1>
	inline void table_set_value(lua_State* vm, int table_index, T const& key, T1 const& value)
	{
		push(vm, key);
		push(vm, value);
		lua_settable(vm, table_index < 0 ? table_index-2 : table_index);
	}

	/**	\brief
	 The table is at table_index which can be either absolute or pseudo in the stack
	 table is left at the index.
	*/
    template<typename T, typename T1>
	inline bool table_at(lua_State* vm, int const table_index, T const& key, T1& value)
	{
		push(vm, key);//table key
		lua_gettable(vm, table_index < 0 ? table_index-1 : table_index);//table value
		return pull(vm, value);//table
	}

	/**
		\brief Creates a new valid \ref OOLUA::Table
		\param [in] vm
		\param [inout] t
		\post stack is the same on exit as entry

	*/
	void new_table(lua_State* vm, OOLUA::Table& t);

	/**
		\brief Creates a new valid \ref OOLUA::Table "Table"
		\post stack is the same on exit as entry
	*/
	OOLUA::Table new_table(lua_State* vm);

	/**	\def oolua_ipairs (table)
		\hideinitializer
		\brief Helper for iterating over the sequence part of a table.
		\param table
		\details
		Declares:
		\li \verbatim int  _i_index_ \endverbatim	: Current index into the array
		\li \verbatim int const _oolua_array_index_ \endverbatim	: Stack index at which table is located
		\li \verbatim lua_State* lvm \endverbatim	: The vm associated with the table

		\note
		Returning from inside of the loop will not leave the stack clean
		unless you reset it.
		usage:
		\code{.cpp}
		oolua_ipairs(table)
		{
			if(_i_index_ == 99)
			{
				lua_settop(lvm,_oolua_array_index-1);
				return "red balloons";
			}
		}
		oolua_ipairs_end()
		return "Not enough balloons to go bang."
		\endcode
	 */
#	define oolua_ipairs(table) \
	if( table.valid() ) \
	{ \
		lua_State* lvm = table.state(); \
		lua_checkstack(lvm, 2);\
		OOLUA::push(lvm, table); \
		int const _oolua_array_index_ = lua_gettop(lvm); \
		int _i_index_ = 1; \
		lua_rawgeti(lvm, _oolua_array_index_, _i_index_); \
		while (lua_type(lvm, -1)  != LUA_TNIL) \
		{
// NOLINT

	/** \def oolua_ipairs_end()
		\hideinitializer
		\see oolua_ipairs
	*/
#	define oolua_ipairs_end()\
			lua_settop(lvm, _oolua_array_index_); \
			lua_rawgeti(lvm, _oolua_array_index_, ++_i_index_); \
		} \
		lua_settop(lvm, _oolua_array_index_-1); \
	}


	/** \def oolua_pairs(table)
		\hideinitializer
		\param table
		\brief Helper for iterating over a table.
		\details
		When iterating over a table, for the next iteration to work you must leave the key
		on the top of the stack. If you need to work with the key, it is a good idea to use
		lua_pushvalue to duplicate it on the stack. This is because if the type is not a
		string and you retrieve a string from the stack with lua_tostring, this will alter
		the vm's stack entry.\n
		Declares:
		\li \verbatim int const  _oolua_table_index_ \endverbatim : Stack index at which table is located
		\li \verbatim lua_State* lvm \endverbatim : The vm associated with the table

		usage:
		\code{.cpp}
		oolua_pairs(table)
		{
			\\do what ever
			lua_pop(vm);\\Pop the value, leaving the key at the top of stack
		}
		oolua_pairs_end()
		\endcode
	*/

#	define oolua_pairs(table) \
	if( table.valid() ) \
	{ \
		lua_State* lvm = table.state(); \
		OOLUA::push(lvm, table); \
		int	const _oolua_table_index_ = lua_gettop(lvm); \
		lua_pushnil(lvm); \
		while (lua_next(lvm, _oolua_table_index_) != 0)

	/** \def oolua_pairs_end()
		\hideinitializer
		\see oolua_pairs
	*/
#	define oolua_pairs_end() \
		lua_pop(lvm, 1); \
	}

	/*
		\brief Iterates over a table using oolua_pairs calling a member function for each iteration
		\param[in] table The OOLUA::Table over which you want to iterate
		\param[in] instance Class instance on which to call the callback
		\param[in] func Member function call back for each iteration.

		\details
		You must remove the value from the stack and leave the key
		do not call anything which may call tostring on the actual key
		duplicate it instead with lua_pushvalue then call the operation on the copy
	*/
	template<typename ClassType>
	inline void for_each_key_value(OOLUA::Table& table, ClassType* instance, void(ClassType::*func)(lua_State*) ) //NOLINT
	{
		oolua_pairs(table)
		{
			(instance->*(func))(lvm);
		}
		oolua_pairs_end()
	}
} // namespace OOLUA

#endif //OOLUA_TABLE_H_
