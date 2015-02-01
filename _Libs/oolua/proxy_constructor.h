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

#ifndef OOLUA_PROXY_CONSTRUCTOR_H_
#	define OOLUA_PROXY_CONSTRUCTOR_H_

/**
	\file proxy_constructor.h
*/

#	include "lua_includes.h"
#	include "oolua_traits_fwd.h"
#	include "proxy_tags.h"
#	include "proxy_storage.h"
#	include "proxy_tag_info.h"
#	include "proxy_userdata.h"
#	include "proxy_stack_helper.h"
#	include "proxy_constructor_param_tester.h"
#	include "type_converters.h"

#include "oolua_boilerplate.h"

#if defined __GNUC__ && defined __STRICT_ANSI__
/*shhhhh about va args and c99*/
#	pragma GCC system_header
#endif

/** \cond INTERNAL*/


#if	OOLUA_USE_EXCEPTIONS ==1
#	include "oolua_exception.h"

#	define OOLUA_CONSTRUCTOR_TRY \
	try {
// NOLINT

#	define OOLUA_CONSTRUCTOR_RESPONSE(ExceptionType, Class, ArgNums) \
	luaL_error(vm, "%s exception in %d argument %s constructor. what() : %s" \
			, ExceptionType \
			, ArgNums \
			, OOLUA::Proxy_class<Class>::class_name \
			, err.what());

#	define OOLUA_CONSTRUCTOR_CATCH(Class, Num) \
	} \
	catch(OOLUA::Runtime_error const& err) \
	{ \
		OOLUA_CONSTRUCTOR_RESPONSE("OOLUA::Runtime_error", Class, Num)\
	}\
	catch(std::exception const& err) \
	{ \
		OOLUA_CONSTRUCTOR_RESPONSE("std::exception", Class, Num)\
	}\
	catch(...) \
	{ \
		luaL_error(vm, "unknown exception in %s %d argument constructor", OOLUA::Proxy_class<Class>::class_name, Num); \
	}
#else
#	define OOLUA_CONSTRUCTOR_TRY
#	define OOLUA_CONSTRUCTOR_CATCH(Class, Num)
#endif


namespace OOLUA
{
	namespace INTERNAL
	{
		/**\addtogroup OOLuaDeveloperDocs
		@{*/

		/**
			\brief Default constructor for a type
			\tparam Type Type to construct
			\tparam HasNoDefaultTypedef None zero indicates the type does not have the
				\ref No_default_constructor tag
			\pre Will never be called on a type which has the \ref No_public_constructors tag
		*/
		template<typename Type, int HasNoDefaultTypedef>
		struct Constructor
		{
			static int construct(lua_State * vm)
			{
				OOLUA_CONSTRUCTOR_TRY
				Type* obj = new Type;
				add_ptr(vm, obj, false, Lua);
				OOLUA_CONSTRUCTOR_CATCH(Type, 0)
				return 1;
			}
		};

		/**
		 	\brief Specialisation which generates a Lua error
		 	\details The static function construct will only be called when
		 		\li The type does not have a default constructor
		 		\li and
		 		\li There were no parameters passed to the new call.
		*/
		template<typename Type>
		struct Constructor<Type, 1>
		{
			static int construct(lua_State * vm)
			{
				return luaL_error(vm, "%s %s %s", "No parameters passed to the constructor of the type"
						   , OOLUA::Proxy_class<Type>::class_name
						   , "which does not have a default constructor.");
			}
		};

		/**
			\brief Default constructor handler
		*/
		template<typename T>
		inline int oolua_generic_default_constructor(lua_State* vm)
		{
			int const stack_count = lua_gettop(vm);
			if(stack_count == 0 )
			{
				return Constructor<T, has_tag<OOLUA::Proxy_class<T>, OOLUA::No_default_constructor>::Result>::construct(vm);
			}
			return luaL_error(vm, "%s %d %s %s", "Could not match", stack_count, "parameter constructor for type", OOLUA::Proxy_class<T>::class_name);
		}
		/**@}*/
	} // namespace INTERNAL // NOLINT
} // namespace OOLUA

/** \addtogroup OOLuaGeneratorTemplates
@{
*/
/**
	\def OOLUA_CONSTRUCTOR_GENERATE_NUM
	\hideinitializer
	\brief Constructor generator template
	\details
	\param NUM Number of parameters this macro will generate a constructor handler for
*/
#define OOLUA_CONSTRUCTOR_GENERATE_NUM(NUM) \
namespace OOLUA \
{ \
	namespace INTERNAL \
	{ \
		template<typename Class OOLUA_COMMA_PREFIXED_TYPENAMES_##NUM > \
		struct Constructor##NUM \
		{ \
			static int construct(lua_State* vm) \
			{ \
				int index(1); \
				if(OOLUA_CONSTRUCTOR_PARAM_IS_OF_TYPE_##NUM) \
				{ \
					valid_construct(vm); \
					return 1; \
				} \
				return 0; \
			} \
			static void valid_construct(lua_State* vm) \
			{ \
				int index(1); \
				OOLUA_CONSTRUCTOR_PARAM_##NUM \
				OOLUA_CONSTRUCTOR_TRY \
				Class* obj = new Class(OOLUA_CONVERTER_PARAMS_##NUM); \
				add_ptr(vm, obj, false, Lua); \
				OOLUA_CONSTRUCTOR_CATCH(Class, NUM) \
			} \
		}; \
	} /* namespace INTERNAL*/ /*NOLINT*/\
} /* namespace OOLUA*/
/*@}*/


OOLUA_INTERNAL_CONSTRUCTORS_GEN

/**
	\addtogroup OOLuaDeveloperDocs
@{*/

/**
	\def OOLUA_CONSTRUCTORS_BEGIN
	\hideinitializer
	\brief Starts the generation of the static factory function for an OOLUA::Proxy_class
*/
#define OOLUA_CONSTRUCTORS_BEGIN \
static int oolua_factory_function(lua_State* vm) \
{ \
	int const stack_count = lua_gettop(vm);
/**@}*/
/** \endcond*/

/** \addtogroup OOLuaDSL
 @{
	\def OOLUA_CTOR
	\hideinitializer
	\brief Generates a constructor in a constructor block \see OOLUA_CTORS
	\details
	OOLUA_CTOR( ConstructorParameterList)
	\param ConstructorParameterList Comma separated list of parameters
	\pre Size of ConstructorParameterList >0 and <= \ref OOLuaConfigConstructorParams "\"constructor_params\""
	\see \ref OOLuaConfigConstructorParams "constructor_params"
 */
#define OOLUA_CTOR(...) \
	MSC_PUSH_DISABLE_CONDITIONAL_CONSTANT_OOLUA \
	if( (stack_count == OOLUA_NARG(__VA_ARGS__) && TYPELIST::IndexOf<Type_list< __VA_ARGS__ >::type, calling_lua_state>::value == -1) \
	|| (stack_count == OOLUA_NARG(__VA_ARGS__)-1 && TYPELIST::IndexOf<Type_list< __VA_ARGS__ >::type, calling_lua_state>::value != -1) ) \
	{ \
		if( OOLUA_VA_CONSTRUCTOR(__VA_ARGS__)<class_ VA_PARAM_TYPES(__VA_ARGS__) >::construct(vm) ) return 1; \
	} \
	MSC_POP_COMPILER_WARNING_OOLUA
/**@}*/

/** \cond INTERNAL*/
/**
	\addtogroup OOLuaDeveloperDocs
@{*/

/**
	\def OOLUA_CONSTRUCTORS_END
	\hideinitializer
	\brief Ends the generation of the static factory function for an OOLUA::Proxy_class
	\details This macro ends the factory function for an OOLUA::Proxy_class and also places
	into the type a typedef named ctor_block_check, which is an alias for the type being proxied.
	The presence of this alias can be checked for at a later point to determine if the type
	does indeed have a factory function. After trying all constructors for which the stack count
	is valid for, if it was unable to find a matching constructor then a Lua error is thrown.
*/
#define OOLUA_CONSTRUCTORS_END \
	if(stack_count == 0 ) \
	{ \
		return INTERNAL::Constructor<class_, INTERNAL::has_tag<this_type, No_default_constructor>::Result>::construct(vm); \
	} \
	return luaL_error(vm, "%s %d %s %s", "Could not match", stack_count, "parameter constructor for type", class_name); \
} \
	typedef class_ ctor_block_check;
/**@}*/
/** \endcond*/

/** \addtogroup OOLuaDSL
@{
	\def OOLUA_CTORS
	\hideinitializer
	\brief Creates a block into which none default constructors can be defined with \ref OOLUA_CTOR
	\details OOLUA_CTORS(ConstructorEntriesList)
	\param ConstructorEntriesList List of \ref OOLUA_CTOR
	<p>
	To enable the construction of an instance which is a specific type, there must be
	constructor(s) for that type registered with OOLua. \ref OOLUA_CTORS is the block into
	which you can define none default constructor entries using \ref OOLUA_CTOR.
	<p>
	Constructors are the only real type of overloading which is permitted by OOLua
	and there is an important point which should be noted. OOLua will try and match
	the number of parameters on the stack with the amount required by each OOLUA_CTOR
	entry and will look in the order they were defined. When interacting with the Lua
	stack certain types can not be differentiated between, these include some integral
	types such as float, int, double etc and types which are of a proxy class type or
	derived from that type. OOLua implicitly converts between classes in a hierarchy
	even if a reference is required. This means for example that if there are constructors
	such as Foo::Foo(int) and Foo::Foo(float) it will depend on which was defined first
	in the OOLUA_CTORS block as to which will be invoked for a call such as Foo.new(1).

	\see \ref OOLUA::No_default_constructor "No_default_constructor"
	\note An OOLUA_CTORS block without any \ref OOLUA_CTOR entries is invalid.
*/
#	define OOLUA_CTORS(ConstructorEntriesList) OOLUA_CONSTRUCTORS_BEGIN ConstructorEntriesList OOLUA_CONSTRUCTORS_END
/**@}*/


#endif
