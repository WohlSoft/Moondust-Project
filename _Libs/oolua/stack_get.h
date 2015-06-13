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

#ifndef OOLUA_STACK_GET_H_
#define OOLUA_STACK_GET_H_

#include "lua_includes.h"
#include "oolua_traits_fwd.h"
#include "oolua_stack_fwd.h"
#include "oolua_string.h"
#include "lvd_types.h"
#include "lvd_type_traits.h"

namespace OOLUA
{
	//fwd
	template<typename T>class Proxy_class;
	//fwd

	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		//fwd
		template<typename T>
		T* check_index(lua_State * vm, int index);

		template<typename T>
		T* check_index_no_const(lua_State * vm, int index);


		namespace PULL
		{
			void set_index_gc_value_to_false(lua_State* vm, int idx);
		}
		//fwd

		namespace LUA_CALLED
		{

			void get_class_type_error(lua_State* const vm, char const* expected_type);
			void get_error(lua_State* vm, int idx, char const* expected_type);

			template<typename Raw_type, int is_const>
			struct stack_class_type
			{
				static void get(lua_State* const vm, int idx, Raw_type*& class_type)
				{
					class_type = check_index<Raw_type>(vm, idx);
				}
			};

			template<typename Raw_type>
			struct stack_class_type<Raw_type, 0>
			{
				static void get(lua_State* const vm, int idx, Raw_type*& class_type)
				{
					class_type = check_index_no_const<Raw_type>(vm, idx);
				}
			};

			template<typename T, int is_integral, int is_convertable_to_int>
			struct get_basic_type;

			template<typename T>
			struct get_basic_type<T, 0, 1>//enum
			{
				static void get(lua_State* const vm, int idx, T &  value)
				{
					//enumeration type so a static cast should be allowed else this
					//is being called with the wrong type
					typedef char dummy_can_convert [ LVD::can_convert_to_int<T>::value ? 1 : -1];
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
					if( !lua_isnumber(vm, idx) ) get_error(vm, idx, "enum type");
#endif
					value = static_cast<T>(lua_tointeger(vm, idx));
				}
			};

			template<int ID>
			struct get_basic_type<Lua_ref<ID>, 0, 0>
			{
				static void get(lua_State* const vm, int idx, Lua_ref<ID> &  value)
				{
					value.lua_get(vm, idx);
				}
			};
			template<typename T>
			struct get_basic_type<T, 1, 1>
			{
				static void get(lua_State* const vm, int idx, T &  value)
				{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
					if( !lua_isnumber(vm, idx) ) get_error(vm, idx, "integer compatabile type");
#endif
					value = static_cast<T>(lua_tointeger(vm, idx));
				}
			};

			template<typename StringType>
			struct get_basic_type<StringType, 0, 0>
			{
				static void get(lua_State* const vm, int idx, StringType &  value)
				{
					OOLUA::STRING::get(vm, idx, value);
				}
			};


			template<typename T, int is_integral>
			struct get_ptr;

			template<typename T>
			struct get_ptr<T, false>
			{
				static void get(lua_State* const vm, int idx, T *&  value)
				{
					typename OOLUA::INTERNAL::param_type<T>::raw* class_ptr;
					stack_class_type<typename OOLUA::INTERNAL::param_type<T>::raw
													, OOLUA::INTERNAL::param_type<T*>::is_constant
												>::get(vm, idx, class_ptr);
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
					if(!class_ptr)
					{
						get_class_type_error(vm, OOLUA::INTERNAL::param_type<T*>::is_constant
											  ? Proxy_class<typename OOLUA::INTERNAL::param_type<T>::raw>::class_name_const
											  : Proxy_class<typename OOLUA::INTERNAL::param_type<T>::raw>::class_name);
					}
#endif
					value = class_ptr;
				}
			};
			template<typename T>
			struct get_ptr<T, true>
			{
				static void get(lua_State* const vm, int idx, T *&  value)
				{
#if OOLUA_DEBUG_CHECKS == 1
					if(!value)
					{
						get_error(vm, idx, "pulling pointer to integral type and was passed NULL. OOLua can not dereference it");
					}
#endif
					LUA_CALLED::get(vm, idx, *value);
				}
			};



			template<typename T>
			inline void get(lua_State* const vm, int idx, T& value)
			{
				get_basic_type<T
							, LVD::is_integral_type<T>::value
							, LVD::can_convert_to_int<T>::value >::get(vm, idx, value);
			}


			template<typename T>
			inline void get(lua_State* const vm, int idx, T *&  value)
			{
				get_ptr<T, LVD::is_integral_type<T>::value>::get(vm, idx, value);
			}


			//pulls a pointer from the stack which Cpp will then own and call delete on
			template<typename T>
			inline void get(lua_State* const vm, int idx, OOLUA::cpp_acquire_ptr<T>&  value)
			{
				typedef cpp_acquire_ptr<T> Type;
				typedef typename Type::raw raw;
				stack_class_type<raw, Type::is_constant>::get(vm, idx, value.m_ptr);

#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
				if(!value.m_ptr)
				{
					get_class_type_error(vm, Type::is_constant
										  ? Proxy_class<raw>::class_name_const
										  : Proxy_class<raw>::class_name);
				}
#endif
				INTERNAL::PULL::set_index_gc_value_to_false(vm, idx);
			}

		} // namespace LUA_CALLED // NOLINT
	} // namespace INTERNAL // NOLINT
	/**\endcond*/


} // namespace OOLUA

#endif //OOLUA_STACK_GET_H_
