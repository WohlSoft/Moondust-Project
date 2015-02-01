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

#ifndef OOLUA_PROXY_STORAGE_H_
#	define OOLUA_PROXY_STORAGE_H_

#include "lua_includes.h"
#include "oolua_traits_fwd.h"
#include "oolua_stack_fwd.h"
#include "oolua_config.h"
#include "proxy_userdata.h"
#include "proxy_base_checker.h"
#include "proxy_class.h"

#include "lvd_types.h"
#include "class_from_stack.h"
#include "type_list.h"
#include "char_arrays.h"


namespace OOLUA
{
	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		typedef bool (*is_const_func_sig)(Lua_ud const* ud);
		template<int NotTheSameSize>
		struct VoidPointerSameSizeAsFunctionPointer;

		template<int NotTheSameSize>
		struct VoidPointerSameSizeAsFunctionPointer
		{
			static void getWeakTable(lua_State* vm)
			{
				lua_getfield(vm, LUA_REGISTRYINDEX, OOLUA::INTERNAL::weak_lookup_name);
			}
			static void setWeakTable(lua_State* vm, int value_index)
			{
				OOLUA_PUSH_CARRAY(vm, OOLUA::INTERNAL::weak_lookup_name);
				lua_pushvalue(vm, value_index);
				lua_rawset(vm, LUA_REGISTRYINDEX);
			}
		};


		template<>
		struct VoidPointerSameSizeAsFunctionPointer< sizeof(is_const_func_sig) >
		{
			static void getWeakTable(lua_State* vm)
			{
				//it is safe as the pointers are the same size
				//yet we need to stop warnings
				//NOTE: in 5.2 we can push a light c function here
				is_const_func_sig func = OOLUA::INTERNAL::userdata_is_constant;
				void** stopwarnings = reinterpret_cast<void**>(&func);
				lua_pushlightuserdata(vm, *stopwarnings);
				lua_rawget(vm, LUA_REGISTRYINDEX);
			}
			static void setWeakTable(lua_State* vm, int value_index)
			{
				//it is safe as the pointers are the same size
				//yet we need to stop warnings
				//NOTE: in 5.2 we can push a light c function here
				is_const_func_sig func = OOLUA::INTERNAL::userdata_is_constant;
				void** stopwarnings = reinterpret_cast<void**>(&func);
				lua_pushlightuserdata(vm, *stopwarnings);
				lua_pushvalue(vm, value_index);
				lua_rawset(vm, LUA_REGISTRYINDEX);
			}
		};

		typedef VoidPointerSameSizeAsFunctionPointer<sizeof(void*)> Weak_table; // NOLINT

		//pushes the weak top and returns its index
		int push_weak_table(lua_State* vm);
		template<typename T>Lua_ud* add_ptr(lua_State* vm, T* const ptr, bool is_const, Owner owner);

		template<typename T>Lua_ud* find_ud(lua_State* vm, T* ptr, bool is_const);

		bool is_there_an_entry_for_this_void_pointer(lua_State* vm, void* ptr);
		bool is_there_an_entry_for_this_void_pointer(lua_State* vm, void* ptr, int tableIndex);

		template<typename T>
		Lua_ud* reset_metatable(lua_State*  vm, T* ptr, bool is_const);

		void add_ptr_if_required(lua_State* const vm, void* ptr, int udIndex, int weakIndex);


		Lua_ud* new_userdata(lua_State* vm, void* ptr, bool is_const
							 , oolua_function_check_base base_checker, oolua_type_check_function type_check);
		void reset_userdata(Lua_ud* ud, void* ptr, bool is_const
							, oolua_function_check_base base_checker, oolua_type_check_function type_check);

		template<typename Type, typename Bases, int BaseIndex, typename BaseType>
		struct Add_ptr;

		template<typename Type, typename Bases, int BaseIndex, typename BaseType>
		struct Has_a_root_entry;

		template<typename T>
		int lua_set_owner(lua_State* vm);

		bool ud_at_index_is_const(lua_State* vm, int index);


		template<typename T>
		int lua_set_owner(lua_State*  vm)
		{
			T* p = check_index<T>(vm, 1);
			if(!p) return luaL_error(vm, "The self/this instance to '%s' is not of type '%s'"
									 , OOLUA::INTERNAL::set_owner_str
									 , OOLUA::Proxy_class<T>::class_name);

			Owner own(No_change);
			OOLUA::INTERNAL::LUA_CALLED::get(vm, 2, own);
			if(own != No_change) INTERNAL::userdata_gc_value(static_cast<INTERNAL::Lua_ud*>(lua_touserdata(vm, 1)), own == Cpp? false : true);
			return 0;
		}


		//It is possible for a base class and a derived class pointer to have no offset.
		//if found it is left on the top of the stack and returns the Lua_ud ptr
		//else the stack is same as on entrance to the function and null is returned
		template<typename T>
		inline Lua_ud* find_ud(lua_State*  vm, T* ptr, bool const is_const)
		{
			bool has_entry = is_there_an_entry_for_this_void_pointer(vm, ptr);//(ud or no addition to the stack)
			Lua_ud* ud(0);
			if(has_entry )//ud
			{
				/*
				possibilities:
					top of stack is the representation of the T ptr
					top of stack is derived from T with no offset pointer and it can be upcast to T
					top of stack is a registered base class of T with no offset pointer
				*/
				ud = static_cast<Lua_ud *>(lua_touserdata(vm, -1));
				bool const was_const = OOLUA::INTERNAL::userdata_is_constant(ud);

				if (is_const)
				{
					if(class_from_stack_top<T>(vm)) return ud;
				}
				else if (was_const)//change
				{
					if(class_from_stack_top<T>(vm))
					{
						INTERNAL::userdata_const_value(ud, false);
						return ud;
					}
				}
				else //was not const and is not const
				{
					if( none_const_class_from_stack_top<T>(vm) )
						return ud;
				}

				//if T was a base of the stack or T was the stack it has been returned
				//top of stack is a registered base class of T with no offset pointer
				return reset_metatable(vm, ptr, was_const && is_const);
			}
			else
			{
				/*
				possibilities:
					a base class is stored.
					none of the hierarchy is stored
				*/

				int weak_table = push_weak_table(vm);
				bool base_is_stored(false);
				Has_a_root_entry<
						T
						, typename FindRootBases<T>::Result
						, 0
						, typename TYPELIST::At_default< typename FindRootBases<T>::Result, 0, TYPE::Null_type >::Result
					> checkRoots;
				checkRoots(vm, ptr, weak_table, base_is_stored);
				lua_remove(vm, weak_table);
				if(base_is_stored)
				{
					bool was_const = ud_at_index_is_const(vm, -1);
					ud = reset_metatable(vm, ptr, was_const && is_const);
				}
			}
			return ud;
		}

		template<typename T>
		inline Lua_ud* reset_metatable(lua_State* vm, T* ptr, bool is_const)
		{
			Lua_ud *ud = static_cast<Lua_ud *>(lua_touserdata(vm, -1));//ud
			reset_userdata(ud, ptr, is_const, &requested_ud_is_a_base<T>, &register_class_imp<T>);
			//change the metatable associated with the ud
			lua_getfield(vm, LUA_REGISTRYINDEX, OOLUA::Proxy_class<T>::class_name);

			lua_setmetatable(vm, -2);//set ud's metatable to this

			int weak_index = push_weak_table(vm);//ud weakTable
			//then register all the bases that need it
			Add_ptr<T
					, typename OOLUA::Proxy_class<T>::AllBases
					, 0
					, typename TYPELIST::At_default< typename OOLUA::Proxy_class<T>::AllBases, 0, TYPE::Null_type >::Result
				> addThisTypesBases;
			addThisTypesBases(vm, ptr, weak_index-1, weak_index);
			lua_pop(vm, 1);//ud
			return ud;
		}

		template<typename T>
		inline Lua_ud* add_ptr(lua_State* const vm, T* const ptr, bool is_const, Owner owner)
		{
			Lua_ud* ud = new_userdata(vm, ptr, is_const, &requested_ud_is_a_base<T>, &register_class_imp<T>);
			if(owner != No_change)userdata_gc_value(ud, owner == Lua);

			lua_getfield(vm, LUA_REGISTRYINDEX, OOLUA::Proxy_class<T>::class_name);

#if	OOLUA_DEBUG_CHECKS == 1
			assert(lua_isnoneornil(vm, -1) == 0 && "no metatable of this name found in registry");
#endif
			////Pops a table from the stack and sets it as the new metatable for the value at the given acceptable index
			lua_setmetatable(vm, -2);

			int weakIndex = push_weak_table(vm);//ud,weakTable
			int udIndex = weakIndex -1;

			add_ptr_if_required(vm, ptr, udIndex, weakIndex);//it is required

			Add_ptr<T
					, typename OOLUA::Proxy_class<T>::AllBases
					, 0
					, typename TYPELIST::At_default< typename OOLUA::Proxy_class<T>::AllBases, 0, TYPE::Null_type >::Result
				> addThisTypesBases;
			addThisTypesBases(vm, ptr, udIndex, weakIndex);

			lua_pop(vm, 1);//ud
			return ud;
		}

		template<typename Type, typename Bases, int BaseIndex, typename BaseType>
		struct Add_ptr
		{
			void operator()(lua_State * const vm, Type* ptr, int udIndex, int weakIndex)
			{
				//add this type if needed
				add_ptr_if_required(vm, static_cast<BaseType*>(ptr), udIndex, weakIndex);
				//add the next in the type list if needed
				Add_ptr<
						Type
						, Bases
						, BaseIndex + 1
						, typename TYPELIST::At_default< Bases, BaseIndex + 1, TYPE::Null_type >::Result
					> addBaseNextPtr;
				addBaseNextPtr(vm, ptr, udIndex, weakIndex);
			}
		};

		template<typename Type, typename Bases, int BaseIndex>
		struct Add_ptr<Type, Bases, BaseIndex, TYPE::Null_type>
		{
			void operator()(lua_State * const /*vm*/, Type* /*ptr*/, int /*udIndex*/, int /*weakIndex*/)const
			{}
		};

		template<typename Type, typename Bases, int BaseIndex, typename BaseType>
		struct Has_a_root_entry
		{
			void operator()(lua_State * const vm, Type* ptr, int weakIndex, bool& result)
			{
				if(result)return;
				result = is_there_an_entry_for_this_void_pointer(vm, static_cast<BaseType*>(ptr), weakIndex);
				if(result)return;
				Has_a_root_entry<
						Type
						, Bases
						, BaseIndex + 1
						, typename TYPELIST::At_default< Bases, BaseIndex + 1, TYPE::Null_type >::Result
					> checkNextBase;
				checkNextBase(vm, ptr, weakIndex, result);
			}
		};

		template<typename Type, typename Bases, int BaseIndex>
		struct Has_a_root_entry<Type, Bases, BaseIndex, TYPE::Null_type>
		{
			void operator()(lua_State * const /*vm*/, Type* /*ptr*/, int /*weakIndex*/, bool& /*result*/)const
			{}
		};


	} // namespace INTERNAL // NOLINT
	/**\endcond*/


} // namespace OOLUA

#endif //OOLUA_PROXY_STORAGE_H_
