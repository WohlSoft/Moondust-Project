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

///////////////////////////////////////////////////////////////////////////////
///  @file proxy_stack_helper.h
///////////////////////////////////////////////////////////////////////////////
#ifndef OOLUA_PROXY_STACK_HELPER_H_
#	define OOLUA_PROXY_STACK_HELPER_H_

#	include "lua_includes.h"
#	include "oolua_stack_fwd.h"
#	include "oolua_string.h"

#	include <cassert>
#	include "push_pointer_internal.h"

namespace OOLUA
{
	/** \cond INTERNAL*/
	namespace
	{
		/*
			Member functions are allowed to do things which are not allowed when using
			the public API function OOLUA::push:
				\li Pushing a instance which has a Proxy_class using a value or reference
					instead of a pointer.
				\li Pushing an integral type by pointer
		*/
		template<typename Raw, typename T, int is_by_value = 0 >
		struct proxy_maybe_by_ref
		{
			static void push(lua_State* const vm, T& value, Owner owner)
			{
				OOLUA::INTERNAL::push_pointer_which_has_a_proxy_class<Raw>(vm, &value, owner);
			}
		};

		template<typename Raw, typename T>
		struct proxy_maybe_by_ref<Raw, T, 1>
		{
			static void push(lua_State* const vm, T& value, Owner /*owner*/)
			{
				Raw* ptr = new Raw(value);
				OOLUA::INTERNAL::push_pointer_which_has_a_proxy_class<Raw>(vm, ptr, Lua);
			}
		};


		template<typename T, typename WT, int is_integral = 1>
		struct maybe_integral
		{
			static void push(lua_State* const vm, T& value)
			{
				OOLUA::push(vm, value);
			}
		};

		//user defined type
		template<typename T, typename WT>
		struct maybe_integral<T, WT, 0>
		{
			static void push(lua_State* const vm, T& value)
			{
				proxy_maybe_by_ref<typename WT::raw, T, WT::is_by_value>::push(vm, value, (Owner)WT::owner);
			}
		};
	} // namespace //NOLINT
	/** \endcond */

	/** \cond INTERNAL */
	namespace INTERNAL
	{

		/**
			\brief Handles the pushing and pulling of types for member functions
			\tparam TypeWithTraits The wanted which is being exchanged with the stack. with
			traits applied to it.
			\tparam owner One of OOLUA::Owner entries which indicate which language owns the type
		*/
		template<typename TypeWithTraits, int owner>
		struct Proxy_stack_helper;

		/*
		Specialisation for the light_return type.
		*/
		template<typename LightUdType>
		struct Proxy_stack_helper<light_return<LightUdType>, No_change>
		{
			static void push(lua_State* vm, LightUdType ptr)
			{
				OOLUA::push(vm, static_cast<void*>(ptr));
			}
		};

		/*
		Specialisation for the light_p parameter type.
		*/
		template<typename LightUdType>
		struct Proxy_stack_helper<light_p<LightUdType>, No_change>
		{
			static void get(lua_State* const vm, int idx, void*& value)
			{
				OOLUA::INTERNAL::LUA_CALLED::get(vm, idx, value);
			}
			static void get(int& idx, lua_State* const vm, void*& value)
			{
				get(vm, idx, value);
				++idx;
			}
			static void push(lua_State* /*vm*/, void* /*ptr*/) // NOLINT(readability/casting)
			{
				assert(0 && "this function should not be called");
			}
		};

		/*
		Specialisation for the maybe_null type.
		If NULL it pushes nil to the stack else calls the normal helper static function.
		*/
		template<typename MaybeNullType>
		struct Proxy_stack_helper<maybe_null<MaybeNullType>, No_change>
		{
			static void push(lua_State* vm, MaybeNullType ptr)
			{
				if (ptr)
					Proxy_stack_helper<function_return<MaybeNullType>, No_change>::push(vm, ptr);
				else
					lua_pushnil(vm);
			}
		};

		/*
		Specialisation for the lua_maybe_null type.
		If NULL it pushes nil to the stack else calls the normal helper static function.
		*/
		template<typename MaybeNullType>
		struct Proxy_stack_helper<lua_maybe_null<MaybeNullType>, Lua>
		{
			static void push(lua_State* vm, MaybeNullType ptr)
			{
				if (ptr)
					Proxy_stack_helper<lua_return<MaybeNullType>, Lua>::push(vm, ptr);
				else
					lua_pushnil(vm);
			}
		};

		template<typename TypeWithTraits>
		struct Proxy_stack_helper<TypeWithTraits, No_change>
		{
			template<typename T>
			static void get(lua_State* const vm, int idx, T& value)
			{
				OOLUA::INTERNAL::LUA_CALLED::get(vm, idx, value);
			}

			template<typename T>
			static void get(int& idx, lua_State* const vm, T& value)
			{
				get(vm, idx, value);
				++idx;
			}

			static void get(lua_State* const vm, int /*idx*/, lua_State*& vm1)
			{
				typedef char type_has_to_be_calling_lua_state[LVD::is_same<in_p<calling_lua_state>, TypeWithTraits>::value ? 1 : -1];
				vm1 = vm;
			}

			static void get(int& /*idx*/, lua_State* const vm, lua_State*& vm1)
			{
				typedef char type_has_to_be_calling_lua_state[LVD::is_same<in_p<calling_lua_state>, TypeWithTraits>::value ? 1 : -1];
				vm1 = vm;
			}

			template<typename T>
			static void push(lua_State* const vm, T& value)
			{
				maybe_integral<T, TypeWithTraits, TypeWithTraits::is_integral>::push(vm, value);
			}

			template<typename T>
			static void push(lua_State* const vm, T*& value)
			{
				OOLUA::push(vm, value, No_change);
			}

			///special case "T* const" and "T const * const"
			template<typename T>
			static void push(lua_State* const vm, T*const& value)
			{
				OOLUA::push(vm, value, No_change);
			}

			static void push(lua_State* const , lua_State *& )
			{
				assert(0 && "this function should not be called");
			}
		};

		template<typename TypeWithTraits>
		struct Proxy_stack_helper<TypeWithTraits, Cpp>
		{
			template<typename T>
			static void get(lua_State* const vm, int idx, T*& value)
			{
				OOLUA::cpp_acquire_ptr<typename TypeWithTraits::type > p;
				OOLUA::INTERNAL::LUA_CALLED::get(vm, idx, p);
				value = p.m_ptr;
			}

			template<typename T>
			static void get(int& idx, lua_State* const vm, T*& value)
			{
				get(vm, idx, value);
				++idx;
			}

			template<typename T>
			static void push(lua_State* const /*s*/, T*& /*value*/)//noop
			{
				assert(0 && "this function should never be called");
			}
		};

		template<typename TypeWithTraits>
		struct Proxy_stack_helper<TypeWithTraits, Lua>
		{
			template<typename T>
			static void push(lua_State* const vm, T*& value)
			{
				OOLUA::lua_acquire_ptr<typename TypeWithTraits::type> p(value);
				OOLUA::push(vm, p);
			}

			template<typename T>
			static void push(lua_State* const vm, T& value)
			{
				proxy_maybe_by_ref<typename TypeWithTraits::raw
								, T
								, TypeWithTraits::is_by_value>
							::push(vm, value, (Owner)TypeWithTraits::owner);
			}

			template<typename T>
			static void get(lua_State* const /*s*/, int /*idx*/, T*& /*value*/)//noop
			{
				assert(0 && "this function should never be called");
			}

			template<typename T>
			static void get(int& /*idx*/,lua_State* const /*s*/,  T& /*value*/)//noop
			{
				assert(0 && "this function should never be called");
			}
		};

	} // namespace INTERNAL //NOLINT
	/** \endcond */
} // namespace OOLUA

#endif //OOLUA_PROXY_STACK_HELPER_H_
