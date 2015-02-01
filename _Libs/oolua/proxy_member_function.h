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
	\file proxy_member_function.h
	\brief Internal macros which generate proxy member functions
*/

#ifndef OOLUA_PROXY_MEMBER_FUNCTION_H_
#	define OOLUA_PROXY_MEMBER_FUNCTION_H_

#	include "oolua_traits_fwd.h"

#	include "oolua_boilerplate.h"
#	include "proxy_caller.h"
#	include "default_trait_caller.h"

#	include <cassert>
#	include "oolua_config.h"

#if defined __GNUC__ && defined __STRICT_ANSI__
#	pragma GCC system_header
#endif

/** \cond INTERNAL*/

/**
	\def OOLUA_MEMBER_BODY_N
	\hideinitializer
	\brief Generates a member function with at least one parameter
	\param mod Optional either const of empty
	\param func_name Name to be used for the proxy function
	\param return_value Return type
	\param func Function name which will be proxied
	\param ... __VA_ARGS__ list of parameters
*/
#define OOLUA_MEMBER_BODY_N(mod, func_name, return_value, func , ...) \
int func_name(lua_State* const vm) mod \
{ \
	assert(m_this); \
	OOLUA_PARAMS_CONCAT(2, __VA_ARGS__) \
	typedef INTERNAL::return_type_traits<return_value > R; \
	typedef R::type (class_::*funcType )( OOLUA_PARAM_TYPE_CONCAT(__VA_ARGS__) ) mod ; \
	OOLUA::INTERNAL::Proxy_caller<R, class_, LVD::is_void< R::type >::value > \
		::call<funcType OOLUA_TPARAMS_CONCAT(__VA_ARGS__) > \
			(vm, m_this, &class_::func OOLUA_PPARAMS_CONCAT(__VA_ARGS__)); \
	OOLUA_BACK_CONCAT(__VA_ARGS__) \
	return INTERNAL::lua_return_count< Type_list<R OOLUA_RETURNS_CONCAT(__VA_ARGS__) > \
		::type> ::out; \
}

/**
	\def OOLUA_MEMBER_BODY_0
	\hideinitializer
	\brief Generates a member function with no parameters
	\param mod Optional either const or empty
	\param func_name Name to be used for the proxy function
	\param return_value Return type
	\param func Function name which will be proxied
*/
#define OOLUA_MEMBER_BODY_0(mod, func_name, return_value, func) \
int func_name(lua_State* const vm) mod\
{ \
	assert(m_this); \
	typedef INTERNAL::return_type_traits<return_value > R; \
	typedef R::type (class_::*funcType )() mod ; \
	OOLUA::INTERNAL::Proxy_caller<R, class_, LVD::is_void< R::type >::value > \
		::call< funcType>(vm, m_this, &class_::func); \
	return INTERNAL::lua_return_count< Type_list<R >::type> ::out; \
}
//	OOLUA_MEM_FUNC_N and OOLUA_MEM_FUNC_0

/**
	@{
	/details
	A member function defined using OOLUA_MEM_FUNC*  (*=  |_CONST | _RENAME | _CONST_RENAME )
	will translate into a corresponding macro below using either a suffix of _N if there are
	function parameters else _0
*/

/**
	\def OOLUA_MEM_FUNC_N
	\brief Generates a none constant proxy which will have the same name as the proxied function
	\pre The proxied function will have one or more function parameters
	\hideinitializer
	\param return_value The return type of the proxied function possibily with \ref OOLuaTraits
	\param func Name to use for the proxied function and also the name of the proxied function
	\param ... __VA_ARGS__ of parameters which can also have traits
*/
#define OOLUA_MEM_FUNC_N(return_value, func , ...) \
	OOLUA_MEMBER_BODY_N(      , func, return_value, func , __VA_ARGS__ ) /*NOLINT*/

/**
	\def OOLUA_MEM_FUNC_CONST_N
	\brief Generates a constant proxy which will have the same name as the proxied function
	\pre The proxied function will have one or more function parameters
	\hideinitializer
	\param return_value The return type of the proxied function possibily with \ref OOLuaTraits
	\param func Name to use for the proxy function and also the name of the proxied function
	\param ... __VA_ARGS__ of parameters which can also have traits
*/
#define OOLUA_MEM_FUNC_CONST_N(return_value, func , ...) \
	OOLUA_MEMBER_BODY_N(const , func, return_value, func , __VA_ARGS__ )

/**
	\def OOLUA_MEM_FUNC_RENAME_N
	\brief Generates a none constant proxy which will have a differnt name from the proxied function
	\pre The proxied function will have one or more function parameters
	\hideinitializer
	\param name Name to use for the proxy function
	\param return_value The return type of the proxied function possibily with \ref OOLuaTraits
	\param func Name of the proxied function
	\param ... __VA_ARGS__ of parameters which can also have traits
*/
#define OOLUA_MEM_FUNC_RENAME_N(name, return_value, func , ...) \
	OOLUA_MEMBER_BODY_N(      , name, return_value, func , __VA_ARGS__ ) /*NOLINT*/

/**
	\def OOLUA_MEM_FUNC_CONST_RENAME_N
	\brief Generates a constant proxy which will have a different name from the proxied function
	\pre The proxied function will have one or more function parameters
	\hideinitializer
	\param name Name to use for the proxy function
	\param return_value The return type of the proxied function possibily with \ref OOLuaTraits
	\param func Name of the proxied function
	\param ... __VA_ARGS__ of parameters which can also have traits
*/
#define OOLUA_MEM_FUNC_CONST_RENAME_N(name, return_value, func , ...) \
	OOLUA_MEMBER_BODY_N(const , name, return_value, func , __VA_ARGS__ )

/**
	\def OOLUA_MEM_FUNC_0
	\brief Generates a none constant proxy which will have the same name as the proxied function
	\pre The proxied function will have no function parameters
	\hideinitializer
	\param return_value The return type of the proxied function possibily with \ref OOLuaTraits
	\param func Name to use for the proxy function and also the name of the proxied function
	\param ... __VA_ARGS__ of parameters which can also have traits
*/
#define OOLUA_MEM_FUNC_0(return_value, func) \
	OOLUA_MEMBER_BODY_0(      , func, return_value, func) /*NOLINT*/

/**
	\def OOLUA_MEM_FUNC_CONST_0
	\brief Generates a constant proxy which will have the same name as the proxied function
	\pre The proxied function will have no function parameters
	\hideinitializer
	\param return_value The return type of the proxied function possibily with \ref OOLuaTraits
	\param func Name to use for the proxy function and also the name of the proxied function
*/
#define OOLUA_MEM_FUNC_CONST_0(return_value, func) \
	OOLUA_MEMBER_BODY_0(const , func, return_value, func)

/**
	\def OOLUA_MEM_FUNC_RENAME_0
	\brief Generates a none constant proxy which will have a different name from the proxied function
	\pre The proxied function will have no function parameters
	\hideinitializer
	\param name Name to use for the proxy function
	\param return_value The return type of the proxied function possibily with \ref OOLuaTraits
	\param func Name of the proxied function
*/
#define OOLUA_MEM_FUNC_RENAME_0(name, return_value, func) \
	OOLUA_MEMBER_BODY_0(      , name, return_value, func) /*NOLINT*/

/**
	\def OOLUA_MEM_FUNC_CONST_RENAME_0
	\brief Generates a constant proxy which will have a different name from the proxied function
	\pre The proxied function will have no function parameters
	\hideinitializer
	\param name Name to use for the proxy function
	\param return_value The return type of the proxied function possibily with \ref OOLuaTraits
	\param func Name of the proxied function
	\param ... __VA_ARGS__ of parameters which can also have traits
*/
#define OOLUA_MEM_FUNC_CONST_RENAME_0(name, return_value, func) \
	OOLUA_MEMBER_BODY_0(const , name, return_value, func)
/**	@}*/

/**
	\def OOLUA_MFUNC_GENERIC
	\hideinitializer
	\brief Generates a generic member function for OOLUA::Proxy_class
	\details Internal macro used by the Minimal DSL, it will generate either
	a constant or none constant member function for the Proxy_class depending
	on the mod parameter. As this proxy function is part of the Minimal DSL
	it does not allow specifiying traits from the function return value or
	parameters, insted it uses default traits for all.
	\param mod Optional parameter which is either const or empty
	\param ProxyName Name that will be given to the proxy member function
	\param FunctionName Name of the member function to be proxied
*/
#define OOLUA_MFUNC_GENERIC(mod, ProxyName, FunctionName) \
	int ProxyName(lua_State* vm) mod \
	{ \
		return OOLUA::INTERNAL::mod##proxy_member_function_with_default_traits(vm, m_this, &class_::FunctionName); \
	}

/**
	@{
 	\details
 	OOLUA_MFUNC(_CONST)_INTERNAL_2 and OOLUA_MFUNC(_CONST)_INTERNAL_1
 	A member function defined using OOLUA_MFUNC*  (*=  |_CONST )
 	will translate into a corresponding macro below using and then to OOLUA_MFUNC_GENERIC
 */
#define OOLUA_MFUNC_INTERNAL_2(FunctionName, ProxyName)		OOLUA_MFUNC_GENERIC( , ProxyName, FunctionName) /*NOLINT*/
#define OOLUA_MFUNC_INTERNAL_1(FunctionName)				OOLUA_MFUNC_INTERNAL_2(FunctionName, FunctionName)

#define OOLUA_MFUNC_CONST_INTERNAL_2(FunctionName, ProxyName)	OOLUA_MFUNC_GENERIC(const, ProxyName, FunctionName)
#define OOLUA_MFUNC_CONST_INTERNAL_1(FunctionName)				OOLUA_MFUNC_CONST_INTERNAL_2(FunctionName, FunctionName)
/**@}*/

/** \endcond*/

#endif //OOLUA_PROXY_MEMBER_FUNCTION_H_

