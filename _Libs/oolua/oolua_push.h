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
	\file oolua_push.h
	Implements the Lua stack operation OOLUA::pull.
*/
#ifndef OOLUA_PUSH_H
#	define OOLUA_PUSH_H

#include "lua_includes.h"
#include "oolua_stack_fwd.h"
#include "oolua_traits_fwd.h"
#include "oolua_string.h"
#include "lvd_types.h"
#include "lvd_type_traits.h"

#include <cassert>

namespace OOLUA
{
	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		//fwd
		template<typename Raw, typename TypeMaybeConst>
		void push_pointer_which_has_a_proxy_class(lua_State * vm, TypeMaybeConst * const ptr, Owner owner);
		//fwd

		template<typename T, int is_integral, int is_convertable_to_int>
		struct push_basic_type;

		template<typename T, bool is_integral>
		struct push_ptr;

		template<typename T>
		struct push_basic_type<T, 0, 1>
		{
			static bool push(lua_State* const  vm, T const&  value)
			{
				//enumeration type so a static cast must be allowed.
				//enums will be stronger in C++0x so this will need revisiting then
				typedef char dummy_can_convert [ LVD::can_convert_to_int<T>::value ? 1 : -1];
				lua_pushinteger(vm, static_cast<lua_Integer>(value) );
				return true;
			}
		};

		template<int ID>
		struct push_basic_type<Lua_ref<ID>, 0, 0>
		{
			static bool push(lua_State* const vm, Lua_ref<ID> const& value)
			{
				return value.push(vm);
			}
		};

		template<typename T>
		struct push_basic_type<T, 1, 1>
		{
			static bool push(lua_State* const vm, T const& value)
			{
				lua_pushinteger(vm, static_cast<lua_Integer>(value));
				return true;
			}
		};

		template<typename T>
		struct push_basic_type<T, 0, 0>
		{
			static bool push(lua_State* const vm, T const& value)
			{
				return OOLUA::STRING::push(vm, value);
			}
		};



		template<typename T>
		struct push_ptr<T, false>
		{
			static bool push(lua_State* const vm, T * const &  value, Owner owner)
			{
				assert(vm && value);
				OOLUA::INTERNAL::push_pointer_which_has_a_proxy_class<typename LVD::raw_type<T>::type>(vm, value, owner);
				return true;
			}

			static bool push(lua_State* const vm, T * const &  value)
			{
				return push_ptr<T, false>::push(vm, value, No_change);
			}
		};

		template<typename T>
		struct push_ptr<T, true>
		{
			//owner is here as it can be supplied but is ignored as the type is integral
			static bool push(lua_State* const vm, T * const &  value, Owner/* owner*/)
			{
				return push_ptr<T, true>::push(vm, value);
			}
			static bool push(lua_State* const vm, T * const &  value)
			{
				assert(vm && value);
				return OOLUA::push(vm, *value);
			}
		};

		///////////////////////////////////////////////////////////////////////////////
		///  Specialisation for C style strings
		///////////////////////////////////////////////////////////////////////////////
		template<>
		struct push_ptr<char, true>
		{
			static bool push(lua_State* const vm, char * const &  value, Owner/* owner*/)
			{
				assert(vm && value);
				lua_pushstring(vm, value);
				return true;
			}

			static bool push(lua_State* const /*vm*/, char * const &  /*value*/)
			{
				assert(0 && "this function should not be called");
				return false;
			}
		};
		template<>
		struct push_ptr<char const, true>
		{
			static bool push(lua_State* const vm, char const * const &  value, Owner/* owner*/)
			{
				assert(vm && value);
				lua_pushstring(vm, value);
				return true;
			}

			static bool push(lua_State* const /*vm*/, char const * const &  /*value*/)
			{
				assert(0 && "this function should not be called");
				return false;
			}
		};

	} // namespace INTERNAL // NOLINT
	/** \endcond*/



	template<typename T>
	inline bool push(lua_State* const  vm, T const&  value)
	{
		return INTERNAL::push_basic_type<T
							, LVD::is_integral_type<T>::value
							, LVD::can_convert_to_int<T>::value >::push(vm, value);
	}

	template<typename T>
	inline bool push(lua_State* const vm, OOLUA::lua_acquire_ptr<T>&  value)
	{
		assert(vm && value.m_ptr);
		INTERNAL::push_pointer_which_has_a_proxy_class<typename OOLUA::lua_acquire_ptr<T>::raw>(vm, value.m_ptr, Lua);
		return true;
	}

	template<typename T>
	inline bool push(lua_State* const vm, T * const &  value, Owner owner)
	{
		return INTERNAL::push_ptr<T, INTERNAL::Type_enum_defaults<T>::is_integral >::push(vm, value, owner);
	}
	template<typename T>
	inline bool push(lua_State* const vm, T * const &  value)
	{
		return INTERNAL::push_ptr<T, LVD::is_integral_type<T>::value>::push(vm, value);
	}

} // namespace OOLUA

#endif //OOLUA_PUSH_H
