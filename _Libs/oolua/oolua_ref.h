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
	\@file oolua_ref.h
	\detailsA wrapper for a lua registry reference in a struct
	so that the type is different to an int. Typedefs two types Lua_table_ref & \n
	Lua_ref.
*/

#ifndef OOLUA_REF_H_
#	define OOLUA_REF_H_

#include "lua_includes.h"

#ifdef __GNUC__
#	define OOLUA_DEFAULT __attribute__ ((visibility("default")))
#else
#	define OOLUA_DEFAULT
#endif

namespace OOLUA
{
	class Table;

	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		void pull_handle_invalid(lua_State* const vm, int id);
		bool push_reference_if_possible(int const ref, lua_State* const from, lua_State* const to);
	}//namespace INTERNAL //NOLINT
	/** \endcond */

	bool can_xmove(lua_State* vm0, lua_State* vm1);

//TODO consider renaming to OOLUA::Ref

	/**	\struct Lua_ref
		\brief A typed wrapper for a Lua reference
		\details
		The Lua_ref templated class stores a reference using Lua's reference system
		luaL_ref and luaL_unref, along with a lua_State. The reason this class stores
		the lua_State is to make it difficult to use the reference with another universe.
		A reference from the same Lua universe, even if it is from a different lua_State,
		is valid to be used in the universe.
		<p>
		The class takes ownership of any reference passed either to the
		\ref OOLUA::Lua_ref::Lua_ref(lua_State* const,int const&) "two argument constructor"
		or the \ref OOLUA::Lua_ref::set_ref "set_ref" function. On going out of scope
		a \ref OOLUA::Lua_ref::valid "valid" reference is guaranteed to be released,
		you may also force a release by passing an instance to \ref OOLUA::Lua_ref::swap
		"swap" for which \ref OOLUA::Lua_ref::valid "valid" returns false.
		<p>
		There are two special values for the reference which Lua provides,
		both of which OOLua will treat as an invalid reference:
			\li LUA_REFNIL luaL_ref return value to indicate it encountered a
				nil object at the location the ref was asked for
			\li LUA_NOREF guaranteed to be different from any reference return
				by luaL_ref
		\tparam ID Lua type as returned by lua_type
		\note
			\li Universe: A call to luaL_newstate or lua_newstate creates a
		Lua universe and a universe is completely independent of any other
		universe. lua_newthread and coroutine.create, create a lua_State
		in an already existing universe.\n
		Term first heard in a Lua mailing list post by Mark Hamburg.
	*/
	template<int ID>
	struct Lua_ref
	{
		/**
			\brief
			Sets the lua_State and reference for the instance
			\note this does not preform any validation on the parameters and it
			is perfectly acceptable to pass parameters such that a call to
			valid will return false.
		*/
		Lua_ref(lua_State* const vm, int const& ref);

		/**
			\brief
			Sets the lua_State for the instance and initialises the instance
			so that a call to valid will return false.
		*/
		explicit Lua_ref(lua_State* const vm);

		/**
			\brief
			Initialises the instance so that a call to valid will return false.
		 */
		Lua_ref();

		//unimplemented
		Lua_ref& operator = (Lua_ref const& /*rhs*/);

		/**
			\brief
			Creates a copy of rhs
			\details
			If rhs is valid then creates a new Lua reference to the value which rhs
			refers to, otherwise it initialises this instance so that a \ref Lua_ref::valid
			call returns false.
		 */
		Lua_ref(Lua_ref const& rhs) OOLUA_DEFAULT;

		/**
			\brief
			Destructor which releases a valid reference.
		*/
		~Lua_ref()OOLUA_DEFAULT;

		/**
			\brief
			Returns true if both the Lua instance is not NULL and the registry reference
			is not invalid.
		*/
		bool valid()const;
		/**
			\brief
			Sets the stored reference and state.
			\details
			Releases any currently stored reference and takes ownership of the passed
			reference.
		*/
		void set_ref(lua_State* const vm, int const& ref)OOLUA_DEFAULT;

		/**
			\brief
			Swaps the Lua instance and the registry reference with rhs.
			\details
			Swaps the lua_State and reference with rhs, this is a
			simple swap and does not call luaL_ref therefore it will not
			create any new references.
		*/
		void swap(Lua_ref& rhs);

		/** \cond INTERNAL*/
		bool push(lua_State* const vm)const;
		bool pull(lua_State* const vm) OOLUA_DEFAULT;
		bool lua_push(lua_State* const vm)const;
		bool lua_get(lua_State* const vm, int idx);
		/** \endcond*/

		/**
			\brief
			Returns the lua_State associated with the Lua reference.
		*/
		lua_State* state() const { return m_lua; }

		/**
			\brief
			Returns the integer Lua reference value.
		*/
		int const& ref()const;
	private:
		/** \cond INTERNAL Yes I know this is bad \endcond*/
		friend class Table;
		bool pull_if_valid(lua_State* vm);
		void release();
		lua_State* m_lua;
		int m_ref;
	};

	/** \cond INTERNAL*/
	template<int ID>
	Lua_ref<ID>::Lua_ref(lua_State* const vm, int const& ref)
		: m_lua(vm)
		, m_ref(ref)
	{}

	template<int ID>
	Lua_ref<ID>::Lua_ref(lua_State* const vm)
		: m_lua(vm)
		, m_ref(LUA_NOREF)
	{}

	template<int ID>
	Lua_ref<ID>::Lua_ref()
		: m_lua(0)
		, m_ref(LUA_NOREF)
	{}

	template<int ID>
	Lua_ref<ID>::Lua_ref(Lua_ref<ID> const& rhs)
		: m_lua(0)
		, m_ref(LUA_NOREF)
	{
		if (rhs.valid())
		{
			m_lua = rhs.m_lua;
			lua_rawgeti(m_lua, LUA_REGISTRYINDEX, rhs.m_ref);
			m_ref = luaL_ref(m_lua, LUA_REGISTRYINDEX);
		}
	}

	template<int ID>
	Lua_ref<ID>::~Lua_ref()
	{
		release();
	}

	template<int ID>
	bool Lua_ref<ID>::valid()const
	{
		return m_lua && m_ref != LUA_REFNIL && m_ref != LUA_NOREF;
	}

	template<int ID>
	int const& Lua_ref<ID>::ref()const
	{
		return m_ref;
	}

	template<int ID>
	inline void Lua_ref<ID>::set_ref(lua_State* const vm, int const& ref)
	{
		release();
		m_ref = ref;
		m_lua = vm;
	}

	template<int ID>
	void Lua_ref<ID>::release()
	{
		if( valid() )
		{
			luaL_unref(m_lua, LUA_REGISTRYINDEX, m_ref);
		}
		m_ref = LUA_NOREF;
	}

	template<int ID>
	void Lua_ref<ID>::swap(Lua_ref & rhs)
	{
		lua_State* tl(rhs.m_lua);
		int tr(rhs.m_ref);
		rhs.m_lua = m_lua;
		rhs.m_ref = m_ref;
		m_lua = tl;
		m_ref = tr;
	}

	template<int ID>
	bool Lua_ref<ID>::push(lua_State* const vm)const
	{
		if( !valid() )
		{
			lua_pushnil(vm);
			return true;
		}
		return  INTERNAL::push_reference_if_possible(m_ref, m_lua, vm)
					&& lua_type(vm, -1) == ID;
	}

	template<int ID>
	bool Lua_ref<ID>::lua_push(lua_State* const vm)const
	{
		if ( !valid() )
		{
			lua_pushnil(vm);
			return true;
		}
		else if ( vm != m_lua )
		{
			luaL_error(vm, "The reference is not valid for this Lua State");
			return false;
		}
		lua_rawgeti(m_lua, LUA_REGISTRYINDEX, m_ref);
		return  lua_type(m_lua, -1) == ID;
	}

	template<int ID>
	bool Lua_ref<ID>::pull_if_valid(lua_State* const vm)
	{
		if (lua_gettop(vm) == 0)  return false;
		const int type = lua_type(vm, -1);
		if( type == ID )
		{
			set_ref(vm, luaL_ref(vm, LUA_REGISTRYINDEX) );
			return true;
		}
		else if ( type == LUA_TNIL )
		{
			release();
			return true;
		}
		return false;
	}

	template<int ID>
	bool Lua_ref<ID>::pull(lua_State* const vm)
	{
		if( !pull_if_valid(vm) )
		{
			INTERNAL::pull_handle_invalid(vm, ID);
			return false;
		}
		return true;
	}

	template<int ID>
	bool Lua_ref<ID>::lua_get(lua_State* const vm, int idx)
	{
#define oolua_err_get() \
		luaL_error(vm \
			, "pulling incorrect type from stack. This is a ref to id %d, stack contains %s" \
			, ID \
			, lua_typename(vm, lua_type(vm, idx)) \
		); /*NOLINT*/ \
		return false/*never gets here*/


		if (lua_gettop(vm) < idx) { oolua_err_get(); }
		const int type = lua_type(vm, idx);
		if( type == ID )
		{
			lua_pushvalue(vm, idx);
			set_ref(vm, luaL_ref(vm, LUA_REGISTRYINDEX));
			return true;
		}
		else if ( type == LUA_TNIL )
		{
			release();
			return true;
		}
		oolua_err_get();
#undef oolua_err_get
	}
	/** \endcond*/

	/** \typedef Lua_table_ref
		\brief Typedef helper for a LUA_TTABLE registry reference
	 */
	typedef Lua_ref<LUA_TTABLE> Lua_table_ref;
	/** \typedef Lua_func_ref
		\brief Typedef helper for a LUA_TFUNCTION registry reference
	*/
	typedef Lua_ref<LUA_TFUNCTION> Lua_func_ref;

} // namespace OOLUA

#endif //OOLUA_REF_H_
