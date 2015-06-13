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
///	\file proxy_none_member_function.h
/// \brief Contains internal macros for proxing none member functions.
///////////////////////////////////////////////////////////////////////////////

#ifndef OOLUA_PROXY_NONE_MEMBER_FUNCTION_H_
#	define OOLUA_PROXY_NONE_MEMBER_FUNCTION_H_

#	include "oolua_traits_fwd.h"
#	include "oolua_boilerplate.h"
#	include "proxy_caller.h"
#	include "default_trait_caller.h"
#	include "oolua_config.h"

//* \cond INTERNAL*/

#if defined __GNUC__ && defined __STRICT_ANSI__
#	pragma GCC system_header
#endif

//proxy implementations
/**
	\def OOLUA_C_FUNCTION_N
	\brief Internal macro which generates the function body for a proxied C Function
	\hideinitializer
	\pre Function being proxied has at least one parameter
	\param return_value The type returned by the function or the type with traits
	\param func Function name which is to be proxied
	\param ... __VA_ARGS__ of function parameters which may have traits
*/
#define OOLUA_C_FUNCTION_N(return_value, func, ...) \
	OOLUA_PARAMS_CONCAT(1, __VA_ARGS__) \
	typedef OOLUA::INTERNAL::return_type_traits<return_value > R; \
	typedef R::type (funcType)( OOLUA_PARAM_TYPE_CONCAT(__VA_ARGS__) ) ; \
	OOLUA::INTERNAL::Proxy_none_member_caller<R, LVD::is_void< R::type >::value >:: \
		call<funcType OOLUA_TPARAMS_CONCAT(__VA_ARGS__)> \
			(vm, &func OOLUA_PPARAMS_CONCAT(__VA_ARGS__)); \
	OOLUA_BACK_CONCAT(__VA_ARGS__) \
	return OOLUA::INTERNAL::lua_return_count< Type_list<R OOLUA_RETURNS_CONCAT(__VA_ARGS__) >::type> ::out;

/**
	\def OOLUA_C_FUNCTION_0
	\brief Internal macro which generates the function body for a proxied C Function
	\hideinitializer
	\pre Function being proxied has no parameters
	\param return_value The type returned by the function or the type with traits
	\param func Function name which is to be proxied
*/
#define OOLUA_C_FUNCTION_0(return_value, func) \
	typedef OOLUA::INTERNAL::return_type_traits<return_value > R; \
	typedef R::type (funcType)() ; \
	OOLUA::INTERNAL::Proxy_none_member_caller<R, LVD::is_void< R::type >::value >:: \
		call<funcType>(vm, &func); \
	return R::out;

/**
	\def OOLUA_CFUNC_INTERNAL_2
	\brief Internal macro which creates a generic proxy function and body
	\hideinitializer
	\param FunctionName Name of the function to be proxied
	\param ProxyName Name that will be given to the proxy function
*/
#define OOLUA_CFUNC_INTERNAL_2(FunctionName, ProxyName) \
int ProxyName(lua_State* vm) \
{ \
	return OOLUA::INTERNAL::proxy_c_function_with_default_traits(vm, FunctionName); \
}

/**
	\def OOLUA_CFUNC_INTERNAL_1
	\brief Internal macro where only a function name has been suppplied
	\hideinitializer
	\param FunctionName Name used both for the proxied function and the proxy function.
*/
#define OOLUA_CFUNC_INTERNAL_1(FunctionName) \
	OOLUA_CFUNC_INTERNAL_2(FunctionName, FunctionName)

/**
	\def OOLUA_STATIC_FUNC_INTERNAL_2
	\brief Internal macro for class static function
	\hideinitializer
	\details Proxies a class static function which has the name FunctionName, using
	the ProxyName which is how it will be known in Lua
	\param FunctionName Name of the function to be proxied
	\param ProxyName Name of the proxy function
*/
#define OOLUA_STATIC_FUNC_INTERNAL_2(FunctionName, ProxyName) \
	static OOLUA_CFUNC_INTERNAL_2(class_::FunctionName, ProxyName)

/**
	\def OOLUA_STATIC_FUNC_INTERNAL_1
	\brief Internal macro for a class static function
	\hideinitializer
	\details Proxies a class static function using FunctionName for both the function
	that will be proxied and the proxy function's name.
	\param FunctionName Name of the static function to proxy and the name of the proxy function
*/
#define OOLUA_STATIC_FUNC_INTERNAL_1(FunctionName) \
	OOLUA_STATIC_FUNC_INTERNAL_2(FunctionName, FunctionName)

/** \endcond*/

#endif //OOLUA_PROXY_NONE_MEMBER_FUNCTION_H_
