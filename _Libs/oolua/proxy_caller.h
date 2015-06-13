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

#ifndef PROXY_CALLER_H_
#	define PROXY_CALLER_H_

/**
	\file proxy_caller.h
	\brief Provides implementations which actually call the member or stand
	alone function, it also pushes a function return to the stack if the
	fubction has one.
*/

#	include "oolua_boilerplate.h"
#	include "oolua_traits_fwd.h"
#	include "type_converters.h"
#	include "proxy_stack_helper.h"
#	include "lua_includes.h"
#	include "oolua_config.h"

#if	OOLUA_USE_EXCEPTIONS ==1
#	include "oolua_exception.h"

#	define OOLUA_NONE_MEMBER_CATCH_RESPONSE(exception_type, what_message) \
		luaL_error(vm, "Type of exception: %s.\n what(): %s.\n When calling function on C function\n" \
					, exception_type \
					, what_message);

#	define OOLUA_NONE_MEMBER_FUNCTION_TRY \
	try \
	{ /*start try block*/
// NOLINT

#	define OOLUA_NONE_MEMBER_FUNCTION_CATCH \
	} /*end try block */ \
	catch(std::exception const& e) \
	{ \
		OOLUA_NONE_MEMBER_CATCH_RESPONSE("std::exception", e.what()) \
	} \
	catch(...) \
	{ \
		OOLUA_NONE_MEMBER_CATCH_RESPONSE("Unknown type", " ") \
	}
#else
#	define OOLUA_NONE_MEMBER_FUNCTION_TRY
#	define OOLUA_NONE_MEMBER_FUNCTION_CATCH (void)vm;
#endif


/** \cond INTERNAL*/
/**	\addtogroup OOLuaGeneratorTemplates
@{*/

/**	\def OOLUA_INTERNAL_MEMBER_CALLER_NUM
	\hideinitializer
	\param NUM
*/
#	define OOLUA_INTERNAL_MEMBER_CALLER_NUM(NUM) \
	template<typename FuncType  OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	static void call(lua_State* const vm, C* m_this, FuncType ptr2mem OOLUA_PULL_TYPE_PARAMS_##NUM) \
	{ \
		OOLUA_CONVERTER_##NUM \
		typename R::type r( (m_this->*ptr2mem)(OOLUA_CONVERTER_PARAMS_##NUM) ); \
		Proxy_stack_helper<typename R::traits, R::owner>::push(vm, r); \
	}

/**	\def OOLUA_INTERNAL_MEMBER_CALLER_NO_RETURN_NUM
	\hideinitializer
	\param NUM
*/
#	define OOLUA_INTERNAL_MEMBER_CALLER_NO_RETURN_NUM(NUM) \
	template<typename FuncType  OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	static void call(lua_State* const /*vm*/, C* m_this, FuncType ptr2mem OOLUA_PULL_TYPE_PARAMS_##NUM) \
	{ \
		OOLUA_CONVERTER_##NUM \
		(m_this->*ptr2mem)(OOLUA_CONVERTER_PARAMS_##NUM); \
	}

/**	\def OOLUA_INTERNAL_C_CALLER_NUM
	\hideinitializer
	\param NUM
*/
#	define OOLUA_INTERNAL_C_CALLER_NUM(NUM) \
	template<typename FuncType  OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	static void call(lua_State* const vm, FuncType ptr2func OOLUA_PULL_TYPE_PARAMS_##NUM) \
	{ \
		OOLUA_CONVERTER_##NUM \
		OOLUA_NONE_MEMBER_FUNCTION_TRY \
			typename R::type r( (*ptr2func)(OOLUA_CONVERTER_PARAMS_##NUM) ); \
			Proxy_stack_helper<typename R::traits, R::owner>::push(vm, r); \
		OOLUA_NONE_MEMBER_FUNCTION_CATCH \
	}

/**	\def OOLUA_INTERNAL_C_CALLER_NO_RETURN_NUM
	\hideinitializer
	\param NUM
*/
#	define OOLUA_INTERNAL_C_CALLER_NO_RETURN_NUM(NUM) \
	template<typename FuncType  OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
	static void call(lua_State* const vm, FuncType ptr2func OOLUA_PULL_TYPE_PARAMS_##NUM) \
	{ \
		OOLUA_CONVERTER_##NUM \
		OOLUA_NONE_MEMBER_FUNCTION_TRY \
			(*ptr2func)(OOLUA_CONVERTER_PARAMS_##NUM); \
		OOLUA_NONE_MEMBER_FUNCTION_CATCH \
	}

/** @}*/

namespace OOLUA
{
	namespace INTERNAL
	{

		/**
			\struct Proxy_caller
			\tparam Return
			\tparam Class
			\tparam ReturnIsVoid
		*/
		template <typename Return, typename Class, int ReturnIsVoid>
		struct Proxy_caller;

		/**
			\struct Proxy_none_member_caller
			\tparam Return
			\tparam ReturnIsVoid
		*/
		template <typename Return, int ReturnIsVoid>
		struct Proxy_none_member_caller;

		template <typename R, typename C>
		struct Proxy_caller<R, C, 0 >
		{
			OOLUA_INTERNAL_MEMBER_CALLER_FUNCS
		};

		template <typename R, typename C>
		struct Proxy_caller<R, C, 1 >
		{
			OOLUA_INTERNAL_MEMBER_CALLER_NO_RETURN_FUNCS
		};

		template <typename R>
		struct Proxy_none_member_caller<R, 0 >
		{
			OOLUA_INTERNAL_C_CALLER_FUNCS
		};

		template <typename R >
		struct Proxy_none_member_caller<R, 1 >
		{
			OOLUA_INTERNAL_C_CALLER_NO_RETURN_FUNCS
		};

	} // namespace INTERNAL // NOLINT
} // namespace OOLUA

#undef OOLUA_INTERNAL_MEMBER_CALLER_NUM
#undef OOLUA_INTERNAL_MEMBER_CALLER_NO_RETURN_NUM
#undef OOLUA_INTERNAL_C_CALLER_NUM
#undef OOLUA_INTERNAL_C_CALLER_NO_RETURN_NUM
#undef OOLUA_NONE_MEMBER_FUNCTION_TRY
#undef OOLUA_NONE_MEMBER_FUNCTION_CATCH
#undef OOLUA_NONE_MEMBER_CATCH_RESPONSE

/**\endcond */
#endif
