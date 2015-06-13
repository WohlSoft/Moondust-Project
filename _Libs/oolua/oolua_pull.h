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
	\file oolua_pull.h
	Implements the Lua stack operation OOLUA::pull.
*/
#ifndef OOLUA_PULL_H_
#	define OOLUA_PULL_H_

#include "lua_includes.h"
#include "oolua_config.h"
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
		typedef int (*compare_lua_type_func_sig)(lua_State*, int);
		bool cpp_runtime_type_check_of_top(lua_State* vm, int expected_lua_type, char const * expected_type);
		bool cpp_runtime_type_check_of_top(lua_State* vm, compare_lua_type_func_sig compareFunc, char const * expected_type);
		void handle_cpp_pull_fail(lua_State* vm, char const * expected_type);

		namespace PULL
		{
			void set_index_gc_value_to_false(lua_State* vm, int idx);
		}
	} // namespace INTERNAL //NOLINT(readability/namespace)
	/** \endcond */


	//fwd
	template<typename T>class Proxy_class;

	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		//fwd
		template<typename T>
		T* class_from_stack_top(lua_State * vm);
		template<typename T>
		T* none_const_class_from_stack_top(lua_State * vm);
		//fwd

		template<typename T, int is_integral, int is_convertable_to_int>
		struct pull_basic_type;

		template<typename T>
		struct pull_basic_type<T, 0, 1>//enum
		{
			static bool pull(lua_State* const vm, T &  value)
			{
				//enumeration type so a static cast should be allowed else this
				//is being called with the wrong type
				typedef char dummy_can_convert [ LVD::can_convert_to_int<T>::value ? 1 : -1];
				//value = static_cast<T>( lua_tonumber( vm, -1) );
				if( !cpp_runtime_type_check_of_top(vm, lua_isnumber, "enum type") )
					return false;
				value = static_cast<T>(lua_tointeger(vm, -1));
				lua_pop(vm, 1);
				return true;
			}
		};

		template<int ID>
		struct pull_basic_type<Lua_ref<ID>, 0, 0>
		{
			static bool pull(lua_State* const vm, Lua_ref<ID> &  value)
			{
				return value.pull(vm);
			}
		};

		template<typename T>
		struct pull_basic_type<T, 1, 1>
		{
			static bool pull(lua_State* const vm, T &  value)
			{
				if( !cpp_runtime_type_check_of_top(vm, lua_isnumber, "integer compatabile type") )
					return false;
				value = static_cast<T>(lua_tointeger(vm, -1));
				lua_pop(vm, 1);
				return true;
			}
		};


		template<typename T>
		struct pull_basic_type<T, 0, 0>
		{
			static bool pull(lua_State* const vm, T&  value)
			{
				return OOLUA::STRING::pull(vm, value);
			}
		};

		///////////////////////////////////////////////////////////////////////////////
		///  @struct pull_ptr
		///  Pulls a pointer to C++ depending on the second template parameter. If it
		///  is true then the type is an integral type and one of the normal overloaded
		///  OOLUA::pull functions are called. If on the other hand the type is not
		///  integral then pull_class_type is used.
		///////////////////////////////////////////////////////////////////////////////
		template<typename T, bool IsIntegral>
		struct pull_ptr;

		template<typename Raw_type>
		inline void pull_class_type(lua_State *const vm, int Is_const, Raw_type*& class_type)
		{
MSC_PUSH_DISABLE_CONDITIONAL_CONSTANT_OOLUA
			if(Is_const) class_type = INTERNAL::class_from_stack_top< Raw_type >(vm);
			else class_type = INTERNAL::none_const_class_from_stack_top<Raw_type>(vm);
MSC_POP_COMPILER_WARNING_OOLUA
		}


		template<typename T>
		struct pull_ptr<T, false>
		{
			//this needs to return a bool as it was called from C++
			static bool pull(lua_State* const vm, T *&  value)
			{
				assert(vm);
				typename OOLUA::INTERNAL::param_type<T>::raw* class_ptr;
				pull_class_type<typename OOLUA::INTERNAL::param_type<T>::raw>
					(vm, OOLUA::INTERNAL::param_type<T*>::is_constant, class_ptr);

				if(!class_ptr)
				{
#	if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
					INTERNAL::handle_cpp_pull_fail(vm, OOLUA::INTERNAL::param_type<T*>::is_constant
												   ? Proxy_class<typename OOLUA::INTERNAL::param_type<T>::raw>::class_name_const
												   : Proxy_class<typename OOLUA::INTERNAL::param_type<T>::raw>::class_name);
#	elif OOLUA_DEBUG_CHECKS == 1
					assert(class_ptr);
#	endif
#	if OOLUA_USE_EXCEPTIONS == 0//prevent vs warnings
					value = 0;
					return false;
#	endif
				}
				value = class_ptr;
				lua_pop(vm, 1);
				return true;
			}
		};
		template<typename T>
		struct pull_ptr<T, true>;//disable

	} // namespace INTERNAL // NOLINT
	/** \endcond*/


	template<typename T>
	inline bool pull(lua_State* const vm, T& value)
	{
		return INTERNAL::pull_basic_type<T
						, LVD::is_integral_type<T>::value
						, LVD::can_convert_to_int<T>::value>::pull(vm, value);
	}


	//pulls a pointer from the stack which Cpp will then own and call delete on
	template<typename T>
	inline bool pull(lua_State* const vm, OOLUA::cpp_acquire_ptr<T>&  value)
	{
		INTERNAL::pull_class_type<typename OOLUA::cpp_acquire_ptr<T>::raw>
			(vm, OOLUA::cpp_acquire_ptr<T>::is_constant, value.m_ptr);

		if(!value.m_ptr)
		{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
			INTERNAL::handle_cpp_pull_fail(vm, OOLUA::cpp_acquire_ptr<T>::is_constant
										   ? Proxy_class<typename cpp_acquire_ptr<T>::raw>::class_name_const
										   : Proxy_class<typename cpp_acquire_ptr<T>::raw>::class_name);
#elif OOLUA_DEBUG_CHECKS == 1
			assert(value.m_ptr);
#endif
#	if OOLUA_USE_EXCEPTIONS == 0//prevent vs warnings
			return false;
#	endif
		}
		INTERNAL::PULL::set_index_gc_value_to_false(vm, -1);
		lua_pop(vm, 1);
		return true;
	}


	//  Checks if it is an integral type( LVD::is_integral_type ) or a type
	//  that should be registered to OOLua with Proxy_class and calls the correct function.
	template<typename T>
	inline bool pull(lua_State* const vm, T *&  value)
	{
		return INTERNAL::pull_ptr<T, LVD::is_integral_type<T>::value>::pull(vm, value);
	}

} //namespace OOLUA

#endif //OOLUA_PULL_H_
