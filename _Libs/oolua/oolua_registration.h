
/**
	\file oolua_registration.h
	\details
	\copyright
	The MIT License\n
	Copyright (c) 2005 Leonardo Palozzi \n
	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in the
	Software without restriction, including without limitation the rights to use,
	copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
	Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:\n
	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.\n
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
	OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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

#ifndef OOLUA_REGISTRATION_H_
#	define OOLUA_REGISTRATION_H_

#include "lua_includes.h"
#include "proxy_class.h"
#include "proxy_userdata.h"
#include "proxy_operators.h"
#include "proxy_function_dispatch.h"
#include "proxy_storage.h"
#include "proxy_tags.h"
#include "proxy_tag_info.h"

#include "proxy_base_checker.h"
#include "class_from_stack.h"
#include "push_pointer_internal.h"
#include "oolua_table.h"
#include "oolua_config.h"
#include "char_arrays.h"
#include "lvd_types.h"


namespace OOLUA
{
	/**
		\brief Registers the class type T and it's bases with an instance of lua_State
		\tparam T Class type to register with OOLua
		\details Registers a class type T for which there is a Proxy_class and also
		registers it's bases, if it has any, with OOLua. It preforms a check to see
		if the type has already been registered with the instance.
		This is safe to be called multiple times with a Lua universe and safe to be
		called with a Proxy_class which has no base classes.
	*/
	template<typename T>void register_class(lua_State * vm);

	/**
		\brief
		Registers a key K and value V entry into class T
		\tparam T Class type to register the static for
		\tparam K Key
		\tparam V Value
	*/
	template<typename T, typename K, typename V>
	void register_class_static(lua_State * const vm, K const& k, V const& v);



	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		template<typename T>struct garbage_collect;

		/**
			\brief
			Implements the registering of a single class which is used by register_class
			\tparam T Class type to register
		 */
		template<typename T>void register_class_imp(lua_State * vm);

		template<typename T, typename B>struct Add_base_methods;
		template<typename T, typename TL, int index, typename B>struct Register_base_methods;

		template<typename T, int HasNoPublicDestructor>struct set_delete_function;
		template<typename T, bool HasNoPublicDestructor>struct set_owner_function;
		template<typename T, bool IsAbstractOrNoConstructors>struct set_create_function;

		//fwd not defined here
		template<typename T>
		int oolua_generic_default_constructor(lua_State* vm);
		//fwd not defined here

		void set_function_in_table_with_upvalue(lua_State* vm, char const * func_name, lua_CFunction func
														, int tableIndex, void* upvalue);
		void set_function_in_table(lua_State* vm, char const* func_name, lua_CFunction func, int tableIndex);
		void set_oolua_userdata_creation_key_value_in_table(lua_State* vm, int tableIndex);

		void set_key_value_in_table(lua_State* vm, char const* key_name, int valueIndex, int tableIndex);
		int check_for_key_in_stack_top(lua_State* vm);

		bool class_name_is_already_registered(lua_State* vm, char const* name);

		template<typename T, int HasRegisterEnumsTag>struct set_class_enums;

		int get_oolua_module(lua_State* vm);
		void register_oolua_type(lua_State* vm, char const* name, int const stack_index);
	} //namespace INTERNAL // NOLINT

	namespace INTERNAL
	{
		template<typename T>
		struct garbage_collect
		{
			static int gc(lua_State * vm)
			{
				Lua_ud *ud = static_cast<Lua_ud*>(lua_touserdata(vm, 1));
				if( ud->flags & GC_FLAG )delete static_cast<T*>(ud->void_class_ptr);
				return 0;
			}
		};

		template<typename T, int HasRegisterEnumsTag>
		struct set_class_enums
		{
			static void set(lua_State* /*vm*/){}//nop
		};

		template<typename T>
		struct set_class_enums<T, 1>
		{
			static void set(lua_State* vm)
			{
				Proxy_class<T>::oolua_enums(vm);
			}
		};

		template<typename T, int HasNoPublicDestructor>
		struct set_delete_function
		{
			static void set(lua_State* vm, int methods)
			{
				set_function_in_table(vm, "__gc", &INTERNAL::garbage_collect<T>::gc, methods);
			}
		};

		template<typename T>
		struct set_delete_function<T, 1>
		{
			static void set(lua_State* /*vm*/, int /*methods*/){}//no op
		};


		template<typename Base>
		struct Base_looker
		{
			static int findInBase(lua_State* vm)
			{
				//table keyString
				lua_getglobal(vm, Proxy_class<Base>::class_name);//table keyString baseTable
				return check_for_key_in_stack_top(vm);
			}
		};


		template<typename T, typename Bases, int Index, typename BaseAtIndex>
		struct R_Base_looker
		{
			static int findInBase(lua_State* vm)
			{
				if (Base_looker<BaseAtIndex>::findInBase(vm) )return 1;
				return R_Base_looker<T, Bases, Index+1
								, typename TYPELIST::At_default<Bases, Index+1, TYPE::Null_type>::Result
								>::findInBase(vm);
			}
		};

		template<typename T, typename Bases, int Index>
		struct R_Base_looker<T, Bases, Index, TYPE::Null_type>
		{
			static int findInBase(lua_State* /*vm*/) // NOLINT
			{
				return 0;
			}
		};

		template<typename T>
		int search_in_base_classes(lua_State* vm)
		{
			return R_Base_looker<T, typename Proxy_class<T>::Bases, 0
									, typename TYPELIST::At_default<typename Proxy_class<T>::Bases
									, 0
									, TYPE::Null_type>::Result >::findInBase(vm);
		}

		template<typename T>
		int search_in_base_classes_yet_prevent_new(lua_State* vm)
		{
			if( lua_type(vm, -1) == LUA_TSTRING )
			{
				OOLUA_PUSH_CARRAY(vm, OOLUA::INTERNAL::new_str);
				if( lua_rawequal(vm, -1, -2) )
				{
					/*
					The string key is the OOLua identifier for the constructor factory function,
					if the type had this identifier it would not enter this function to search
					the heirachy tree to find another.
					*/
					return 0;
				}
				else lua_pop(vm, 1);
			}
			return R_Base_looker<T, typename Proxy_class<T>::Bases, 0
									, typename TYPELIST::At_default<typename Proxy_class<T>::Bases
									, 0
									, TYPE::Null_type>::Result >::findInBase(vm);
		}

		template<typename T, int HasNoPublicConstructors>
		struct set_base_lookup_function
		{
			static void set(lua_State* vm, int methods)
			{
				set_function_in_table(vm, "__index", &search_in_base_classes<T>, methods);
			}
		};

		template<typename T>
		struct set_base_lookup_function<T, 1>
		{
			static void set(lua_State* vm, int methods)
			{
				set_function_in_table(vm, "__index", &search_in_base_classes_yet_prevent_new<T>, methods);
			}
		};


		template<typename T, typename B>
		struct Add_base_methods
		{
			void operator()(lua_State * const vm, int const methods)
			{
				for (typename Proxy_class<B >::Reg_type *r = Proxy_class<B >::class_methods; r->name; r++)
				{
					INTERNAL::set_function_in_table_with_upvalue(vm, r->name, &OOLUA::INTERNAL::member_caller<T, B >
																 , methods, reinterpret_cast<void*>(r));
				}

				for (typename Proxy_class<B >::Reg_type_const *r = Proxy_class<B >::class_methods_const; r->name; ++r)
				{
					INTERNAL::set_function_in_table_with_upvalue(vm, r->name, &OOLUA::INTERNAL::const_member_caller<T, B >
																 , methods, reinterpret_cast<void*>(r));
				}
				Register_base_methods<T
							, typename Proxy_class<B>::Bases
							, 0
							, typename TYPELIST::At_default< typename Proxy_class<B>::Bases, 0, TYPE::Null_type >::Result
				> b;
				b(vm, methods);
			}
		};
		template<typename T>
		struct Add_base_methods<T, TYPE::Null_type>
		{
			void operator()(lua_State *  const /*vm*/,int const/*methods*/){}///no-op
		};

		template<typename T, typename TL, int index, typename B>
		struct Register_base_methods
		{
			void operator()(lua_State * const vm, int const methods)
			{
				Add_base_methods<T, typename TYPELIST::At_default< TL, index, TYPE::Null_type >::Result> adder;
				adder(vm, methods);
				Register_base_methods<T, TL, index + 1, typename TYPELIST::At_default< TL, index + 1, TYPE::Null_type >::Result> base;
				base(vm, methods);
			}
		};

		template<typename T, typename TL, int index>
		struct Register_base_methods<T, TL, index, TYPE::Null_type>
		{
			void operator()(lua_State * const  /*vm*/, int const /*methods*/){}///no-op
		};

		template<typename T>
		struct class_or_base_has_ctor_block
		{
			template<typename U>
			static char(& check_for_ctor_block(typename OOLUA::Proxy_class<U>::ctor_block_check*))[1]; // NOLINT
			template<typename U>
			static char(& check_for_ctor_block(...))[2]; // NOLINT
			enum {value = sizeof(check_for_ctor_block<T >(0)) == 1 ? 1 : 0};
		};

		template< typename T, int CtorBlockMaybeInClass_or_MaybeInABase>
		struct ctor_block_is_same
		{
			enum {value = LVD::is_same< typename Proxy_class<T>::ctor_block_check, T >::value };
		};

		template< typename T>
		struct ctor_block_is_same<T, 0>
		{
			enum {value = 0};
		};

		template< typename T>
		struct proxy_class_has_correct_ctor_block
		{
			enum { value = ctor_block_is_same<T, class_or_base_has_ctor_block<T>::value >::value  };
		};

		template<typename T, bool IsAbstractOrNoConstructors>
		struct set_create_function
		{
			static void do_set(lua_State* const vm, int methods, LVD::Int2type<1> /*use factory function*/)
			{
				set_function_in_table(vm
									  , new_str
									  , &OOLUA::Proxy_class<T>::oolua_factory_function
									  , methods);
			}
			static void do_set(lua_State* const vm, int methods, LVD::Int2type<0> /*needs generic function*/)
			{
				set_function_in_table(vm
									  , new_str
									  , &oolua_generic_default_constructor<T>
									  , methods);
			}
			static void set(lua_State*  const vm, int methods)
			{
				do_set(vm, methods, LVD::Int2type<proxy_class_has_correct_ctor_block<T>::value>());
				// methods["new"] = some_method
			}
		};

		template<typename T>
		struct set_create_function<T, true>
		{
			static void set(lua_State*  const /*vm*/,int /*methods*/){}///no-op
		};


		template<typename T, bool HasNoDestructors>
		struct set_owner_function
		{
			static void set(lua_State*  const vm, int methods)
			{
				set_function_in_table(vm
									  , set_owner_str
									  , &INTERNAL::lua_set_owner<T>
									  , methods);
				//methods["set_owner"]=&lua_set_owner()
			}
		};

		template<typename T>
		struct set_owner_function<T, true>
		{
			static void set(lua_State*  const /*vm*/, int /*methods*/){}///no-op
		};


		template<typename T>
		inline void register_class_imp(lua_State * vm)
		{
			if(class_name_is_already_registered(vm, Proxy_class<T>::class_name))
				return;
			lua_newtable(vm);
			int methods = lua_gettop(vm);//methods

			luaL_newmetatable(vm, Proxy_class<T>::class_name);//methods mt
			//registry[name]= mt
			int mt = lua_gettop(vm);

			// store method table in globals so that scripts can add functions written in Lua.
			lua_pushvalue(vm, methods);
			lua_setglobal(vm, Proxy_class<T>::class_name);
			//global[name]=methods

			register_oolua_type(vm, Proxy_class<T>::class_name, methods);
			//OOLua[name] = methods

			set_oolua_userdata_creation_key_value_in_table(vm, mt);

			set_key_value_in_table(vm, "__index", methods, mt);
			//mt["__index"]= methods

			//allow statics and functions to be added to the userdatatype metatable
			set_key_value_in_table(vm, "__newindex", methods, mt);
			//mt["__newindex"]= methods

			set_base_lookup_function<T
					, LVD::if_or<has_tag<Proxy_class<T>, Abstract >::Result
						, has_tag<Proxy_class<T>, No_public_constructors >::Result
						>::value
				>::set(vm, methods);
			//methods["__index"] = function to search bases classes for the key

			set_delete_function<T, INTERNAL::has_tag<Proxy_class<T>, No_public_destructor >::Result>::set(vm, mt);

			set_create_function<T
					, LVD::if_or<has_tag<Proxy_class<T>, Abstract >::Result
						, has_tag<Proxy_class<T>, No_public_constructors >::Result
						>::value
				>::set(vm, methods);

			set_owner_function<T, has_tag<Proxy_class<T>, No_public_destructor >::Result>::set(vm, methods);

			set_equal_function<T, has_tag<Proxy_class<T>, Equal_op>::Result>::set(vm, mt);
			set_less_than_function<T, has_tag<Proxy_class<T>, Less_op>::Result>::set(vm, mt);
			set_less_than_or_equal_function<T, has_tag<Proxy_class<T>, Less_equal_op>::Result>::set(vm, mt);
			set_add_function<T, has_tag<Proxy_class<T>, Add_op>::Result>::set(vm, mt);
			set_sub_function<T, has_tag<Proxy_class<T>, Sub_op>::Result>::set(vm, mt);
			set_mul_function<T, has_tag<Proxy_class<T>, Mul_op>::Result>::set(vm, mt);
			set_div_function<T, has_tag<Proxy_class<T>, Div_op>::Result>::set(vm, mt);

			set_class_enums<T, INTERNAL::has_tag<Proxy_class<T>, Register_class_enums>::Result>::set(vm);

			// fill method table with methods from class Proxy_class<T>
			for (typename Proxy_class<T >::Reg_type *r = Proxy_class<T >::class_methods; r->name; r++)
			{
				set_function_in_table_with_upvalue(vm
												 , r->name
												 , &member_caller<Proxy_class<T>, T>
												 , methods
												 , reinterpret_cast<void*>(r));
			}

			// fill constant method table with methods from class Proxy_class<T>
			for (typename Proxy_class<T >::Reg_type_const *r = Proxy_class<T >::class_methods_const; r->name; ++r)
			{
				set_function_in_table_with_upvalue(vm
												 , r->name
												 , &const_member_caller<Proxy_class<T>, T>
												 , methods
												 , reinterpret_cast<void*>(r));
			}

			//recursively register any base class methods
			Register_base_methods<Proxy_class<T>
									, typename Proxy_class<T>::Bases
									, 0
									, typename TYPELIST::At_default<typename Proxy_class<T>::Bases, 0, TYPE::Null_type >::Result
								> recursive_worker;
			recursive_worker(vm, methods);

			lua_pushvalue(vm, methods);
			lua_setmetatable(vm, methods);//set methods as it's own metatable
			lua_pop(vm, 2);
		}
	} // namespace INTERNAL // NOLINT
	/**\endcond*/

	template<typename T, typename K, typename V>
	inline void register_class_static(lua_State * const vm, K const& k, V const& v)
	{
		Table meth(vm, Proxy_class<T>::class_name);
		meth.set(k, v);
	}

	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		template<int Index, typename Bases, typename Type>
		struct Register_bases_with_lua;

		template<int Index, typename Bases, typename Type>
		struct Register_bases_with_lua
		{
			static void work(lua_State * vm)
			{
				register_class_imp<Type>(vm);
				Register_bases_with_lua<Index+1
								, Bases
								, typename TYPELIST::At_default<Bases, Index+1, TYPE::Null_type>::Result
						>::work(vm);
			}
		};
		template<int index, typename TL>
		struct Register_bases_with_lua<index, TL, TYPE::Null_type>
		{
			static void work(lua_State * /*vm*/) // NOLINT
			{}
		};
	} // namespace INTERNAL // NOLINT
	/** \endcond*/

	template<typename T>
	inline void register_class(lua_State * vm)
	{
		INTERNAL::register_class_imp<T>(vm);
		INTERNAL::Register_bases_with_lua<0
								, typename OOLUA::Proxy_class<T>::AllBases
								, typename TYPELIST::At_default<typename OOLUA::Proxy_class<T>::AllBases, 0, TYPE::Null_type>::Result
						>::work(vm);
	}

} // namespace OOLUA
#endif
