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
	\file proxy_operators.h
	\details Defines operators which will be made available in scripts when a
	\ref OOLUA::Proxy_class contains \ref OperatorTags "operator tags"
*/

#ifndef OOLUA_PROXY_OPERATORS_H_
#	define OOLUA_PROXY_OPERATORS_H_

#	include "lua_includes.h"
#	include "proxy_userdata.h"
#	include "proxy_storage.h"
#	include "oolua_stack_fwd.h"
#	include "oolua_traits_fwd.h"
#	include "push_pointer_internal.h"
#	include "type_list.h"

namespace OOLUA
{
	/** \cond INTERNAL */
	namespace INTERNAL
	{
		//rhs is top of stack lhs is below (lhs op rhs)
		template<typename T>
		int lua_equal(lua_State*  const vm)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::get(vm, 1, lhs);
			INTERNAL::LUA_CALLED::get(vm, 2, rhs);
			bool result(*lhs == *rhs);
			OOLUA::push(vm, result);
			return 1;
		}

		template<typename T>
		int lua_less_than(lua_State* const vm)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::get(vm, 1, lhs);
			INTERNAL::LUA_CALLED::get(vm, 2, rhs);
			bool result(*lhs < *rhs);
			OOLUA::push(vm, result);
			return 1;
		}

		template<typename T>
		int lua_less_than_or_equal(lua_State* const vm)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::get(vm, 1, lhs);
			INTERNAL::LUA_CALLED::get(vm, 2, rhs);
			bool result(*lhs <= *rhs);
			OOLUA::push(vm, result);
			return 1;
		}

		//these following operator functions return a type that they are working on
		template<typename T>
		int lua_add(lua_State*  const vm)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::get(vm, 1, lhs);
			INTERNAL::LUA_CALLED::get(vm, 2, rhs);
			T* result(new T(*lhs + *rhs));
			INTERNAL::add_ptr<T>(vm, result, false, OOLUA::Lua);
			return 1;
		}


		template<typename T>
		int lua_sub(lua_State*  const vm)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::get(vm, 1, lhs);
			INTERNAL::LUA_CALLED::get(vm, 2, rhs);
			T* result(new T(*lhs - *rhs));
			INTERNAL::add_ptr<T>(vm, result, false, OOLUA::Lua);
			return 1;
		}

		template<typename T>
		int lua_mul(lua_State*  const vm)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::get(vm, 1, lhs);
			INTERNAL::LUA_CALLED::get(vm, 2, rhs);
			T* result(new T(*lhs * *rhs));
			INTERNAL::add_ptr<T>(vm, result, false, OOLUA::Lua);
			return 1;
		}

		template<typename T>
		int lua_div(lua_State*  const vm)
		{
			T const* lhs(0);
			T const* rhs(0);
			INTERNAL::LUA_CALLED::get(vm, 1, lhs);
			INTERNAL::LUA_CALLED::get(vm, 2, rhs);
			T* result(new T(*lhs / *rhs));
			INTERNAL::add_ptr<T>(vm, result, false, OOLUA::Lua);
			return 1;
		}

#define DEFINE_OOLUA_OPERATOR_FUNCTION_(operation, lua_string_op)\
		template<typename T, bool hasOperator >\
		struct set_ ## operation ## _function\
		{\
			static void set(lua_State* const /*vm*/, int /*metatable*/){}\
		};\
		template<typename T>\
		struct set_ ## operation ## _function<T, true> \
		{\
			static void set(lua_State* const vm, int metatable)\
			{\
				lua_pushliteral(vm, lua_string_op);\
				lua_pushcfunction(vm, lua_## operation<T>);\
				lua_rawset(vm, metatable);\
			}\
		};


DEFINE_OOLUA_OPERATOR_FUNCTION_(equal, "__eq")
DEFINE_OOLUA_OPERATOR_FUNCTION_(less_than, "__lt")
DEFINE_OOLUA_OPERATOR_FUNCTION_(less_than_or_equal, "__le")
DEFINE_OOLUA_OPERATOR_FUNCTION_(add, "__add")
DEFINE_OOLUA_OPERATOR_FUNCTION_(sub, "__sub")
DEFINE_OOLUA_OPERATOR_FUNCTION_(mul, "__mul")
DEFINE_OOLUA_OPERATOR_FUNCTION_(div, "__div")

#undef DEFINE_OOLUA_OPERATOR_FUNCTION_

	} // namespace INTERNAL // NOLINT
	/** \endcond  */
} // namespace OOLUA

#endif //OOLUA_PROXY_OPERATORS_H_
