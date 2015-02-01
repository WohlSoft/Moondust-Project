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

#ifndef DEFAULT_TRAIT_CALLER_H_
#	define DEFAULT_TRAIT_CALLER_H_

/** \cond INTERNAL*/

/**
	\file default_trait_caller.h
	\brief Default trait callers for constant and none constant member functions as well
	as C functions.
*/

struct lua_State;

namespace OOLUA
{
	namespace INTERNAL
	{
		/*Two structures are used here instead of one and specialising so that compile
		 errors are forced instead of a runtime error when an incorrect macro is used */
		/**
			\struct default_traits_caller
			\brief Caller for non constant member function using default traits.
			\details Determines the function return type and parameters for
			\ref OOLuaMinimalist "Minimalist" DSL usage. No traits can be provied by a user
			and it will instead use default traits, which are OOLUA::in_p for parameters
			and OOLUA::INTERNAL::function_return for the return type.
			\tparam func_type Specialiased function pointer type
			\tparam type None type template argument
		*/
		template<typename func_type, typename type = func_type>
		struct default_traits_caller;

		/**
			\struct default_traits_const_caller
			\brief Determines the function return type and parameters for
			\ref OOLuaMinimalist "Minimalist"  DSL usage
			\tparam func_type Specialiased constant member function pointer type
			\tparam type None type template argument
		*/
		template<typename func_type, typename type = func_type>
		struct default_traits_const_caller;

		/**
			\struct default_c_traits_caller
			\brief Determines the function return type and parameters for
			\ref OOLuaMinimalist "Minimalist" DSL usage.
			\tparam func_type Specialiased function pointer type
			\tparam type None type template argument
		*/
		template<typename func_type, typename type = func_type>
		struct default_c_traits_caller;

		template<typename this_type, typename func_type>
		int proxy_member_function_with_default_traits(lua_State* vm, this_type* this_, func_type mfptr)
		{
			return default_traits_caller<func_type>::call(vm, this_, mfptr);
		}

		template<typename this_type, typename func_type>
		int constproxy_member_function_with_default_traits(lua_State* vm, this_type* this_, func_type mfptr)
		{
			return default_traits_const_caller<func_type>::call(vm, this_, mfptr);
		}

		template<typename func_type>
		int proxy_c_function_with_default_traits(lua_State* vm, func_type fptr)
		{
			return default_c_traits_caller<func_type>::call(vm, fptr);
		}
	} // namespace INTERNAL // NOLINT
} // namespace OOLUA


/** \addtogroup OOLuaGeneratorTemplates
@{*/
/**
	\def OOLUA_GENERATE_DEFAULT_TRAIT_CALLER
	\hideinitializer
	\brief Generator template to produce default trait callers
	\details Template for OOLUA::INTERNAL::default_traits_caller which produces
	a static method call for class member and non member functions.
	\param NUM Number of parameters the macro will generate a default tratis caller for.
*/
#define OOLUA_GENERATE_DEFAULT_TRAIT_CALLER(NUM) \
namespace OOLUA \
{ \
	namespace INTERNAL \
	{ \
		template <class class_type, typename return_type OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM , typename func_type> \
		struct default_traits_caller<return_type (class_type::*)(OOLUA_COMMA_SEPERATED_TYPES_##NUM), func_type> \
		{ \
			typedef OOLUA::INTERNAL::return_type_traits<return_type > R; \
			static int call(lua_State* vm, class_type* this_, func_type mfptr) \
			{ \
				OOLUA_PARAMS_DEFAULT_INTERNAL_##NUM(1) \
				OOLUA::INTERNAL::Proxy_caller<R, class_type, LVD::is_void<return_type > ::value >:: \
					template call<func_type OOLUA_COMMA_PREFIXED_PARAM_TYPES_##NUM > \
						(vm, this_, mfptr OOLUA_CALL_PARAMS_##NUM); \
				return R::out; \
			} \
		}; \
		template <class class_type, typename return_type OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM , typename func_type> \
		struct default_traits_const_caller<return_type (class_type::*)(OOLUA_COMMA_SEPERATED_TYPES_##NUM)const, func_type> \
		{ \
			typedef OOLUA::INTERNAL::return_type_traits<return_type > R; \
			static int call(lua_State* vm, class_type* this_, func_type mfptr) \
			{ \
				OOLUA_PARAMS_DEFAULT_INTERNAL_##NUM(1) \
				OOLUA::INTERNAL::Proxy_caller<R, class_type, LVD::is_void<return_type >::value >\
					:: template call<func_type OOLUA_COMMA_PREFIXED_PARAM_TYPES_##NUM > \
						(vm, this_, mfptr OOLUA_CALL_PARAMS_##NUM); \
				return R::out; \
			} \
		}; \
		template <typename return_type OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM , typename func_type> \
		struct default_c_traits_caller<return_type (*)(OOLUA_COMMA_SEPERATED_TYPES_##NUM), func_type> \
		{ \
			typedef OOLUA::INTERNAL::return_type_traits<return_type > R; \
			static int call(lua_State* vm, func_type fptr) \
			{ \
				OOLUA_PARAMS_DEFAULT_INTERNAL_##NUM(0) \
				OOLUA::INTERNAL::Proxy_none_member_caller< R, LVD::is_void<return_type >::value >:: \
					template call<func_type OOLUA_COMMA_PREFIXED_PARAM_TYPES_##NUM > \
						(vm, fptr OOLUA_CALL_PARAMS_##NUM); \
				return R::out; \
			} \
		};\
	}/* namespace INTERNAL*/ /*NOLINT*/ \
}/* namespace OOLUA*/
/**@}*/

OOLUA_GENERATE_DEFAULT_TRAIT_CALLERS

/** \endcond */

#endif
