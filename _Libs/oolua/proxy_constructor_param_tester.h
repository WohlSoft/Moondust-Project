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
	\file proxy_constructor_param_tester.h
	\brief Helps test that a constructor parameter is of the requested type
	so that a matching constructor can be called
*/

#ifndef OOLUA_PROXY_CONSTRUCTOR_PARAM_TESTER_H_
#	define OOLUA_PROXY_CONSTRUCTOR_PARAM_TESTER_H_

#	include "proxy_userdata.h"
#	include "lua_includes.h"
#	include "class_from_stack.h"
#	include "oolua_config.h"
#	include "type_list.h"
#	include "oolua_string.h"
#	include "oolua_traits_fwd.h"

namespace OOLUA
{
	/** \cond INTERNAL */
	//fwd declare
	template<int ID>
	struct Lua_ref;

	namespace INTERNAL
	{
		/*
		#define LUA_TNIL		0
		#define LUA_TBOOLEAN		1
		#define LUA_TLIGHTUSERDATA	2
		#define LUA_TNUMBER		3
		#define LUA_TSTRING		4
		#define LUA_TTABLE		5
		#define LUA_TFUNCTION		6
		#define LUA_TUSERDATA		7
		#define LUA_TTHREAD		8
		*/

		//Used for constructors to check parameters on the stack
		template<typename Cpp_type, int Lua_type>
		struct lua_type_is_cpp_type;

		/*
		Specialisation
		This is required as Type_enum_defaults will strip the pointer and then can_convert_to_int
		uses the resulting type with a reference applied. This means it will try to use
		void& which is illegal.
		*/
		template<>
		struct lua_type_is_cpp_type<void*, LUA_TNUMBER>
		{
			enum {value = 0 };
		};

		template<typename Cpp_type>
		struct lua_type_is_cpp_type<Cpp_type, LUA_TNUMBER>
		{
			enum {value = Type_enum_defaults<Cpp_type>::is_integral
					&& !LVD::is_same<bool, Cpp_type>::value };
		};

		template<typename Cpp_type>
		struct lua_type_is_cpp_type<Cpp_type, LUA_TSTRING>
		{
			enum { value = LVD::is_same<char*, Cpp_type>::value
					|| OOLUA::STRING::is_integral_string_class<Cpp_type>::value };
		};

		template<typename Cpp_type>
		struct lua_type_is_cpp_type<Cpp_type, LUA_TBOOLEAN>
		{
			enum {value = LVD::is_same<bool, Cpp_type>::value};
		};

		template<typename Cpp_type>
		struct lua_type_is_cpp_type<Cpp_type, LUA_TFUNCTION>
		{
			enum {value = LVD::is_same<Lua_ref<LUA_TFUNCTION>, Cpp_type>::value};
		};

		template<typename Cpp_type>
		struct lua_type_is_cpp_type<Cpp_type, LUA_TTABLE>
		{
			typedef Type_list<
			Lua_ref<LUA_TTABLE>, Table
			>::type Table_types;
			enum {value = TYPELIST::IndexOf<Table_types, Cpp_type>::value == -1 ? 0 : 1};
		};

		template<typename Cpp_type>
		struct lua_type_is_cpp_type<Cpp_type, LUA_TLIGHTUSERDATA>
		{
			enum {value = LVD::is_same<void*, Cpp_type>::value};
		};




		/**
			\struct index_can_convert_to_type
			\brief Implements the testing of a stack index type against a requested
			OOLUA::Proxy_class userdata type
			\details If Is_integral is none zero then translates to a nop, otherwise
			\ref OOLUA::INTERNAL::index_can_convert_to_type::valid tests the userdata
			taking into accont the constness of the types and if stack type is correct
			for the required type.
			\tparam TypeWithTraits
			\tparam Is_integral
		*/
		template<typename TypeWithTraits, int Is_integral>
		struct index_can_convert_to_type
		{
			static int valid(lua_State* /*vm*/,int const& /*index*/){return 0;}//noop
		};

		template<typename TypeWithTraits>
		struct index_can_convert_to_type<TypeWithTraits, 0>
		{
			static int valid(lua_State* vm, int index)
			{
MSC_PUSH_DISABLE_CONDITIONAL_CONSTANT_OOLUA
				if( OOLUA::INTERNAL::check_index<typename TypeWithTraits::raw>(vm, index) )
				{
					return !( !TypeWithTraits::is_constant
									&& INTERNAL::userdata_is_constant(static_cast<INTERNAL::Lua_ud *>(lua_touserdata(vm, index))) );
				}
				return 0;
MSC_POP_COMPILER_WARNING_OOLUA
			}
		};

		/**
			\struct Param_helper
			\brief Tests a requested type against a stack index type.
			\details Constructors support overloading, therefore there is a need to
			test the type at a specific stack location against the required type for the
			constructor parameter; this structure with it's static method implements some
			of that checking.
			As it is possible for the type to not actually be on the stack
			(see OOLUA::calling_lua_state) it also needs to increment the stack index
			when the type is correct, this ensures that any following parameters use the
			correct stack index.
			\tparam ParamWithTraits The current requested parameter type, with traits applied,
			for the stack index.
		*/
		template<typename ParamWithTraits>
		struct Param_helper;

		/**
			\struct Param_helper<OOLUA::INTERNAL::param_type<OOLUA::calling_lua_state> >
			Specialisation for OOLUA::calling_lua_state which is never a type on the stack,
			it therefore never increments the stack index and valid always returns
			a valid result.
		*/
		template<>
		struct Param_helper<param_type<calling_lua_state> >
		{
			static int param_is_of_type(lua_State* /*vm*/, int& /*index*/) //NOLINT
			{
				return 1;
			}
		};

		template<typename ParamWithTraits>
		struct Param_helper
		{
			static int param_is_of_type(lua_State* vm, int & index)
			{
				switch ( lua_type(vm, index) )
				{
					case LUA_TBOOLEAN :
						return lua_type_is_cpp_type<typename ParamWithTraits::raw, LUA_TBOOLEAN>::value && ++index ? 1 : 0;
					case LUA_TNUMBER :
						return lua_type_is_cpp_type<typename ParamWithTraits::raw, LUA_TNUMBER>::value && ++index ? 1 : 0;
					case LUA_TSTRING :
						return lua_type_is_cpp_type<typename ParamWithTraits::raw, LUA_TSTRING>::value && ++index ? 1 : 0;
					case LUA_TFUNCTION :
						return lua_type_is_cpp_type<typename ParamWithTraits::raw, LUA_TFUNCTION>::value && ++index ? 1 : 0;
					case LUA_TTABLE :
						return lua_type_is_cpp_type<typename ParamWithTraits::raw, LUA_TTABLE>::value && ++index ? 1 : 0;
					case LUA_TUSERDATA:
						if( index_can_convert_to_type<ParamWithTraits, ParamWithTraits::is_integral>::valid(vm, index) )
						{
							++index;
							return 1;
						}
						return 0;
					case LUA_TLIGHTUSERDATA:
						return lua_type_is_cpp_type<typename ParamWithTraits::raw, LUA_TLIGHTUSERDATA>::value && ++index ? 1 : 0;
					default:
						return 0;
						break;
				}
			}
		};
	} // namespace INTERNAL // NOLINT
	/** \endcond */
} // namespace OOLUA

#endif //OOLUA_PROXY_CONSTRUCTOR_PARAM_TESTER_H_
