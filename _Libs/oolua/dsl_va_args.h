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

#ifndef OOLUA_DSL_VA_ARGS_H_
#	define OOLUA_DSL_VA_ARGS_H_

#	if defined __GNUC__
#		pragma GCC system_header
#	endif

/** \file dsl_va_args.h */

/** \cond INTERNAL
	The only difference between the code at the following URL and used below, is
	that macro names got an OOLUA prefix as per all macros in the library
	http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5
*/

/*No No, No No No No, No No No No, No No there's a limit! */
#	define OOLUA_INTERNAL_60_N \
		N, N, N, N, N, N, N, N, N, N \
		, N, N, N, N, N, N, N, N, N, N \
		, N, N, N, N, N, N, N, N, N, N \
		, N, N, N, N, N, N, N, N, N, N \
		, N, N, N, N, N, N, N, N, N, N \
		, N, N, N, N, N, N, N, N, N, N


#	define OOLUA_NARG(...)\
		OOLUA_NARG_DO(__VA_ARGS__, OOLUA_RSEQ_N)

#	define OOLUA_ARG_N(\
		_1, _2, _3, _4, _5, _6, _7, _8, _9, _10 \
		, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20 \
		, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30 \
		, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40 \
		, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50 \
		, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60 \
		, _61, _62, _63, N, ...) N

#	define OOLUA_RSEQ_N\
		63, 62, 61, 60 \
		, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50 \
		, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40 \
		, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30 \
		, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20 \
		, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10 \
		, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0


	/*====================================================
		For macros which require at least a specific amount of arguments.
		Translates to:
			0 if equal to amount
			N if more than the amount
		Compile time error if less than the required amount and greater than zero
	*/
#	define OOLUA_NARG_GREATER_THAN_ONE(...) \
		OOLUA_NARG_DO(__VA_ARGS__, OOLUA_INTERNAL_60_N, N, N, 0, ERROR_NOT_ENOUGH_PARAMETERS )

#	define OOLUA_NARG_GREATER_THAN_TWO(...) \
		OOLUA_NARG_DO(__VA_ARGS__, OOLUA_INTERNAL_60_N, N, 0, ERROR_NOT_ENOUGH_PARAMETERS, ERROR_NOT_ENOUGH_PARAMETERS)

#	define OOLUA_NARG_GREATER_THAN_THREE(...) \
		OOLUA_NARG_DO(__VA_ARGS__, OOLUA_INTERNAL_60_N, 0, ERROR_NOT_ENOUGH_PARAMETERS, ERROR_NOT_ENOUGH_PARAMETERS, ERROR_NOT_ENOUGH_PARAMETERS)

/*====================================================*/

/*	Concatenating normally only requires two macros to correctly expand yet
	when using OOLUA_NARG with Visual Studio it fails unless we go through
	a further macro to expand it.
*/
#	define DO_VA_CONCAT2(A, B)A##B
#	define DO_VA_CONCAT(A, B) DO_VA_CONCAT2(A, B)
#	define OOLUA_VA_CONCAT(A, B) DO_VA_CONCAT(A, B)


#	define OOLUA_PUBLIC_INHERIT_1(ClassType) \
			public Proxy_class<ClassType>

#	define OOLUA_PUBLIC_INHERIT_2(ClassType1, ClassType2) \
			OOLUA_PUBLIC_INHERIT_1(ClassType1) \
			, OOLUA_PUBLIC_INHERIT_1(ClassType2)

#	define OOLUA_PUBLIC_INHERIT_3(ClassType1, ClassType2, ClassType3) \
			OOLUA_PUBLIC_INHERIT_2(ClassType1, ClassType2) \
			, OOLUA_PUBLIC_INHERIT_1(ClassType3)

#	define OOLUA_PUBLIC_INHERIT_4(ClassType1, ClassType2, ClassType3, ClassType4) \
			OOLUA_PUBLIC_INHERIT_3(ClassType1, ClassType2, ClassType3) \
			, OOLUA_PUBLIC_INHERIT_1(ClassType4)

#	define OOLUA_PUBLIC_INHERIT_5(ClassType1, ClassType2, ClassType3, ClassType4, ClassType5) \
			OOLUA_PUBLIC_INHERIT_4(ClassType1, ClassType2, ClassType3, ClassType4)\
			, OOLUA_PUBLIC_INHERIT_1(ClassType4)

#	define OOLUA_PUBLIC_INHERIT_6(ClassType1, ClassType2, ClassType3, ClassType4, ClassType5, ClassType6) \
			OOLUA_PUBLIC_INHERIT_5(ClassType1, ClassType2, ClassType3, ClassType4, ClassType5) \
			, OOLUA_PUBLIC_INHERIT_1(ClassType6)


#	define OOLUA_PROXY_WITH_BASES_0(ClassType) \
			OOLUA_CLASS(ClassType) \
			OOLUA_BASIC \
			OOLUA_NO_BASES

#	define OOLUA_PROXY_WITH_BASES_N(ClassType, ...)\
			OOLUA_CLASS(ClassType) : OOLUA_PUBLIC_INHERIT(__VA_ARGS__) \
			OOLUA_BASIC \
			OOLUA_BASES_START __VA_ARGS__ OOLUA_BASES_END

/** \endcond */



//#if defined BROKEN_PREPROCESSOR
#	if defined _MSC_VER && _MSC_VER >= 1400 && _MSC_VER <= 1800 && !defined __INTEL_COMPILER
	/** \cond INTERNAL
		vs8(2005), vs9(2008), vs10(2010) and vs11(2012) should all be fine with the work around of the __VA_ARGS__ bug
		which the compiler team will never admit to it being wrong. The fix was previously documented at the following
		URL yet the page is no longer reachable.
		https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details

		Taken from the above bug report url and the proposed workaround

		For inline documentation see none MSC version below
	*/

#		define OOLUA_LEFT_PAREN (
#		define OOLUA_RIGHT_PAREN )
#		define OOLUA_NARG_DO(...) \
			OOLUA_ARG_N OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

		/*Function generation helper macros*/
#		define OOLUA_PARAMS_CONCAT(StackIndex, ...)			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_PARAMS_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN StackIndex, __VA_ARGS__  OOLUA_RIGHT_PAREN
#		define OOLUA_PARAM_TYPE_CONCAT(...)		OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_FUNCTION_PARAMS_TYPES_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN
#		define OOLUA_TPARAMS_CONCAT(...)		OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_COMMA_PREFIXED_PARAM_TYPES_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN
#		define OOLUA_PPARAMS_CONCAT(...)		OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_CALL_PARAMS_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN
#		define OOLUA_BACK_CONCAT(...)			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_BACK_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN
#		define OOLUA_RETURNS_CONCAT(...)		OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_COMMA_PREFIXED_PARAM_TYPES_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN

		/*Public API*/
#		define OOLUA_PROXY(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_PROXY_WITH_BASES_, OOLUA_NARG_GREATER_THAN_ONE OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MEM_FUNC(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MEM_FUNC_, OOLUA_NARG_GREATER_THAN_TWO OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN  OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MEM_FUNC_RENAME(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MEM_FUNC_RENAME_, OOLUA_NARG_GREATER_THAN_THREE OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MEM_FUNC_CONST(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MEM_FUNC_CONST_, OOLUA_NARG_GREATER_THAN_TWO OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MEM_FUNC_CONST_RENAME(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MEM_FUNC_CONST_RENAME_, OOLUA_NARG_GREATER_THAN_THREE OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_C_FUNCTION(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_C_FUNCTION_, OOLUA_NARG_GREATER_THAN_TWO OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MFUNC(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MFUNC_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN  OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MFUNC_CONST(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MFUNC_CONST_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_CFUNC(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_CFUNC_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN  OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_SFUNC(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_STATIC_FUNC_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN  OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MGET(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MGET_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MSET(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MSET_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_MGET_MSET(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_MGET_MSET_INTERNAL_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_EXPORT_FUNCTIONS(...)		OOLUA_VA_CONCAT OOLUA_LEFT_PAREN EXPORT_OOLUA_FUNCTIONS_, OOLUA_NARG_GREATER_THAN_ONE OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN OOLUA_NON_CONST, __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_EXPORT_FUNCTIONS_CONST(...)OOLUA_VA_CONCAT OOLUA_LEFT_PAREN EXPORT_OOLUA_FUNCTIONS_, OOLUA_NARG_GREATER_THAN_ONE OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN OOLUA_CONST, __VA_ARGS__ OOLUA_RIGHT_PAREN
		/*End Public API*/

#		define OOLUA_PUBLIC_INHERIT(...)		OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_PUBLIC_INHERIT_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define EXPORT_OOLUA_FUNCTIONS_0(mod, ClassType) \
			EXPORT_OOLUA_FUNCTIONS_0_(mod, ClassType)

#		define EXPORT_OOLUA_FUNCTIONS_N(mod, ClassType, ...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN OOLUA_VA_CONCAT OOLUA_LEFT_PAREN EXPORT_OOLUA_FUNCTIONS_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN , _ OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN mod, ClassType, __VA_ARGS__ OOLUA_RIGHT_PAREN

#		define OOLUA_VA_CONSTRUCTOR(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN INTERNAL::Constructor, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN

#		define VA_PARAM_TYPES(...) \
			OOLUA_VA_CONCAT OOLUA_LEFT_PAREN VA_, OOLUA_NARG OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN OOLUA_RIGHT_PAREN OOLUA_LEFT_PAREN __VA_ARGS__ OOLUA_RIGHT_PAREN

	/** \endcond */

#	else //Generic __VA_ARGS__

	/** \cond INTERNAL
		Assume the compiler supports __VA_ARGS__ and uses the standard implementation.
		The only difference between the code at the following URL and used below, is
		that macro names got an OOLUA prefix as per all macros in the library
		http://groups.google.com/group/comp.std.c/browse_thread/thread/77ee8c8f92e4a3fb/346fc464319b1ee5
	*/
#		define OOLUA_NARG_DO(...) \
			OOLUA_ARG_N(__VA_ARGS__)

		/*Function generation helper macros*/
#		define OOLUA_PARAMS_CONCAT(StackIndex, ...)			OOLUA_VA_CONCAT(OOLUA_PARAMS_INTERNAL_, OOLUA_NARG(__VA_ARGS__)) (StackIndex, __VA_ARGS__  )
#		define OOLUA_PARAM_TYPE_CONCAT(...)		OOLUA_VA_CONCAT(OOLUA_FUNCTION_PARAMS_TYPES_, OOLUA_NARG(__VA_ARGS__))
#		define OOLUA_TPARAMS_CONCAT(...)		OOLUA_VA_CONCAT(OOLUA_COMMA_PREFIXED_PARAM_TYPES_, OOLUA_NARG(__VA_ARGS__))
#		define OOLUA_PPARAMS_CONCAT(...)		OOLUA_VA_CONCAT(OOLUA_CALL_PARAMS_, OOLUA_NARG(__VA_ARGS__))
#		define OOLUA_BACK_CONCAT(...)			OOLUA_VA_CONCAT(OOLUA_BACK_INTERNAL_, OOLUA_NARG(__VA_ARGS__))
#		define OOLUA_RETURNS_CONCAT(...)		OOLUA_VA_CONCAT(OOLUA_COMMA_PREFIXED_PARAM_TYPES_, OOLUA_NARG(__VA_ARGS__))

	/** \endcond*/

/*Public API macros using NARG or variants*/

/** \addtogroup OOLuaDSL DSL
@{
\brief The Domain specific language used for generating C++ bindings for Lua
\details OOLua provides a DSL for defining C++ types which are to be made available to a
	Lua script. The intention of this DSL is to hide the details whilst providing a
	simple and rememberable interface for performing the actions required.
\note
	"Optional" here means that extra macro parameters are optional, up to the
	configuration \ref OOLuaConfigCppParams "max" for a specific operation.
*/

	/** \def OOLUA_PROXY
		\hideinitializer
		\brief Starts the generation a proxy class
		\details
		OOLUA_PROXY(ClassName, Optional)
		\param ClassName Class to be proxied
		\param Optional Comma seperated list of real base classes
		\pre Each class specified in Optional must be a real base class of ClassName
	*/
#		define OOLUA_PROXY(...) \
			OOLUA_VA_CONCAT(OOLUA_PROXY_WITH_BASES_, OOLUA_NARG_GREATER_THAN_ONE(__VA_ARGS__))(__VA_ARGS__)
		/**@{*/
	/** \def OOLUA_MGET
		\hideinitializer
		\brief Generates a getter, which is a constant function, to retreive a public instance.
		\details
		OOLUA_MGET(PublicName, Optional)
		\param PublicName Name of the public variable to be proxied.
		\param Optional GetterName. Defaults to get_PublicName
	*/
#		define OOLUA_MGET(...) \
			OOLUA_VA_CONCAT(OOLUA_MGET_INTERNAL_, OOLUA_NARG(__VA_ARGS__)) (__VA_ARGS__)

	/** \def OOLUA_MSET
		\hideinitializer
		\brief Generates a setter, which is a none constant function, to set the public instance.
		\details
		OOLUA_MSET(PublicName, Optional)
		\param PublicName Name of the public variable to be proxied.
		\param Optional SetterName. Defaults to set_PublicName
	*/
#		define OOLUA_MSET(...) \
			OOLUA_VA_CONCAT(OOLUA_MSET_INTERNAL_, OOLUA_NARG(__VA_ARGS__)) (__VA_ARGS__)

	/** \def OOLUA_MGET_MSET
		\hideinitializer
		\brief Generates a getter and setter for a public instance.
		\details
		OOLUA_MGET_MSET(PublicName, Optional1, Optional2)
		\param PublicName Name of the public variable to be proxied.
		\param Optional1 GetterName. Defaults to get_PublicName
		\param Optional2 SetterName. Defaults to set_PublicName
		\see OOLUA_MGET and OOLUA_MSET
		\note If one optional parameter is supplied then both must be given.
	*/
#		define OOLUA_MGET_MSET(...) \
			OOLUA_VA_CONCAT(OOLUA_MGET_MSET_INTERNAL_, OOLUA_NARG(__VA_ARGS__)) (__VA_ARGS__)
		/**@}*/
	/** \addtogroup OOLuaExpressive Expressive
	@{
	\brief Generates code where all details are expressed
	\details
	Generates a function for which the user has expressed all the parameters for a function
	these may additionally have \ref OOLuaTraits.
	*/
		/**@{*/
	/**	\def OOLUA_MEM_FUNC
		\hideinitializer
		\brief Generates a member function proxy which will also be the named FunctionName
		\details
		OOLUA_MEM_FUNC( FunctionReturnType, FunctionName, Optional)
		\param FunctionReturnType
		\param FunctionName
		\param Optional : Comma seperated list of function parameter types
		\see \ref OOLuaConfigCppParams "cpp_params"
	*/
#		define OOLUA_MEM_FUNC(...) \
			OOLUA_VA_CONCAT(OOLUA_MEM_FUNC_, OOLUA_NARG_GREATER_THAN_TWO(__VA_ARGS__)) (__VA_ARGS__)

	/** \def OOLUA_MEM_FUNC_RENAME
		\hideinitializer
		\brief Generates a member function proxy which will be the named ProxyFunctionName
		\details
		OOLUA_MEM_FUNC_RENAME( ProxyFunctionName, FunctionReturnType,FunctionName, Optional)
		\param ProxyFunctionName
		\param FunctionReturnType
		\param FunctionName
		\param Optional : Comma seperated list of function parameter types
		\see \ref OOLuaConfigCppParams "cpp_params"
	*/
#		define OOLUA_MEM_FUNC_RENAME(...) \
			OOLUA_VA_CONCAT(OOLUA_MEM_FUNC_RENAME_, OOLUA_NARG_GREATER_THAN_THREE(__VA_ARGS__))(__VA_ARGS__)

	/** \def OOLUA_MEM_FUNC_CONST
		\hideinitializer
		\brief Generates a constant member function proxy which will also be the named FunctionName
		\details
		OOLUA_MEM_FUNC_CONST( FunctionReturnType,FunctionName,Optional)
		\param FunctionReturnType
		\param FunctionName
		\param Optional Comma seperated list of function parameter types
		\see \ref OOLuaConfigCppParams "cpp_params"
	*/
#		define OOLUA_MEM_FUNC_CONST(...) \
			OOLUA_VA_CONCAT(OOLUA_MEM_FUNC_CONST_, OOLUA_NARG_GREATER_THAN_TWO(__VA_ARGS__))(__VA_ARGS__)

	/** \def OOLUA_MEM_FUNC_CONST_RENAME
		\hideinitializer
		\brief Generates a constant member function which will be named ProxyFunctionName
		\details
		OOLUA_MEM_FUNC_CONST_RENAME( ProxyFunctionName, FunctionReturnType, FunctionName,Optional)
		\param ProxyFunctionName
		\param FunctionReturnType
		\param FunctionName
		\param Optional Comma seperated list of function parameter types
		\see \ref OOLuaConfigCppParams "cpp_params"
	*/
#		define OOLUA_MEM_FUNC_CONST_RENAME(...) \
			OOLUA_VA_CONCAT(OOLUA_MEM_FUNC_CONST_RENAME_, OOLUA_NARG_GREATER_THAN_THREE(__VA_ARGS__))(__VA_ARGS__)
		/**@}*/

	/** \def OOLUA_C_FUNCTION
		\hideinitializer
		\brief Generates a block which will call the C function FunctionName
		\details
		OOLUA_C_FUNCTION(FunctionReturnType,FunctionName, Optional)
		\param FunctionReturnType
		\param FunctionName
		\param Optional Comma seperated list of function parameter types
		\see \ref OOLuaConfigCppParams "cpp_params"
		\pre The function in which this macro is contained must declare a lua_State pointer
		which can be identified by the name "vm"
		\code{.cpp}
		extern void foo(int);
		int l_foo(lua_State* vm)
		{
			OOLUA_C_FUNCTION(void,foo,int)
		}
		\endcode
		\note
		This macro should ideally be used as the last operation of a function body as
		control will return to the caller. Notice there is no return statement in l_foo
	*/
#		define OOLUA_C_FUNCTION(...) \
			OOLUA_VA_CONCAT(OOLUA_C_FUNCTION_, OOLUA_NARG_GREATER_THAN_TWO(__VA_ARGS__))(__VA_ARGS__)
	/**	@}*/



	/** \addtogroup OOLuaMinimalist Minimalist
 	@{
	\brief Generates code using the minimal information
	\details
	Generates a proxy function using the only the minimal of information which is generally
	the name of the thing being proxied and possibly a new name for the proxy.
	As with taking the address of any C++ function, if there is any ambiguity it will fail
	to compile, in which case a user should help the compiler by specifying more information
	using the matching, yet longer named \ref OOLuaExpressive DSL entry.\n
	The longer DSL name requires more information. \n
	\note No \ref OOLuaTraits can be expressed with this DSL group.
	*/
		/**@{*/
	/** \def OOLUA_MFUNC
		\hideinitializer
		\brief Deduce and generate a proxy for a member function
		\details
		OOLUA_MFUNC(FunctionName, Optional)
		\param FunctionName Name of the member function to be proxied
		\param Optional ProxyFunctionName. Defaults to FunctionName
		\see \ref OOLuaConfigCppParams "cpp_params" \n
		\ref OOLUA_MEM_FUNC \n
		\ref OOLUA_MEM_FUNC_RENAME
	*/
#		define OOLUA_MFUNC(...) \
			OOLUA_VA_CONCAT(OOLUA_MFUNC_INTERNAL_, OOLUA_NARG(__VA_ARGS__)) (__VA_ARGS__)

	/** \def OOLUA_MFUNC_CONST
		\hideinitializer
		\brief Deduce and generate a proxy for a constant member function
		\details
		OOLUA_MFUNC_CONST(FunctionName, Optional)
		\param FunctionName Name of the constant function to be proxied
		\param Optional ProxyFunctionName. Defaults to FunctionName
		\see \ref OOLuaConfigCppParams "cpp_params" \n
		\ref OOLUA_MEM_FUNC_CONST \n
		\ref OOLUA_MEM_FUNC_CONST_RENAME
	*/
#		define OOLUA_MFUNC_CONST(...) \
			OOLUA_VA_CONCAT(OOLUA_MFUNC_CONST_INTERNAL_, OOLUA_NARG(__VA_ARGS__)) (__VA_ARGS__)
		/**@}*/

	/** \def OOLUA_CFUNC
		\hideinitializer
		\brief Deduce and generate a proxy for a C function
		\details
		OOLUA_CFUNC(FunctionName, ProxyFunctionName)
		\param FunctionName Name of the C function to be proxied
		\param ProxyFunctionName Name of the function to generate which will proxy FunctionName
		\see \ref OOLuaConfigCppParams "cpp_params" \n
		\ref OOLUA_C_FUNCTION
	*/
#		define OOLUA_CFUNC(...) \
			OOLUA_VA_CONCAT(OOLUA_CFUNC_INTERNAL_, OOLUA_NARG(__VA_ARGS__)) (__VA_ARGS__)

	/** \def OOLUA_SFUNC
		\hideinitializer
		\brief Deduce and generate a proxy for a class static function
		\details
		OOLUA_SFUNC(FunctionName, Optional)
		\param FunctionName Name of the static function to be proxied
		\param Optional ProxyFunctionName. Defaults to FunctionName
		\note This function will not be exported and needs to be registered
		with OOLua see OOLUA::register_class_static
		\see \ref OOLuaConfigCppParams "cpp_params"
	 */
#		define OOLUA_SFUNC(...) \
			OOLUA_VA_CONCAT(OOLUA_STATIC_FUNC_INTERNAL_, OOLUA_NARG(__VA_ARGS__)) (__VA_ARGS__)

	/**	@}*/


	/** \addtogroup OOLuaExporting Exporting
	@{
		\brief Exports member functions
		\details Exporting defines which member functions will be registered with Lua when the
		class type is registered. Even when there are no member functions to be exported you still
		need to inform OOLua about this. Calling an \ref OOLuaExporting "OOLUA_EXPORT*"
		procedure in a header file is an error that will fail to compile.

		\see OOLUA_EXPORT_FUNCTIONS \n OOLUA_EXPORT_FUNCTIONS_CONST \n OOLUA_EXPORT_NO_FUNCTIONS
	 */


	/**	\def OOLUA_EXPORT_FUNCTIONS
		\hideinitializer
		\brief Exports zero or more member functions which will be registered with Lua
		\details
		OOLUA_EXPORT_FUNCTIONS(ClassName,Optional)
		\param ClassName Name of class to which the function belong to
		\param Optional Comma seperated list of member function names
		\see \ref OOLuaConfigClassFunctions "class_functions"
	*/
#		define OOLUA_EXPORT_FUNCTIONS(...)		OOLUA_VA_CONCAT(EXPORT_OOLUA_FUNCTIONS_, OOLUA_NARG_GREATER_THAN_ONE(__VA_ARGS__))(OOLUA_NON_CONST, __VA_ARGS__)

	/** \def OOLUA_EXPORT_FUNCTIONS_CONST
		\hideinitializer
		\brief Exports zero or more const member functions which will be registered with Lua
		\details
		OOLUA_EXPORT_FUNCTIONS_CONST(ClassName,Optional)
		\param ClassName Name of class to which the function belong to
		\param Optional Comma seperated list of constant member function names
		\see \ref OOLuaConfigClassFunctions "class_functions"
	*/
#		define OOLUA_EXPORT_FUNCTIONS_CONST(...)OOLUA_VA_CONCAT(EXPORT_OOLUA_FUNCTIONS_, OOLUA_NARG_GREATER_THAN_ONE(__VA_ARGS__))(OOLUA_CONST, __VA_ARGS__)
	/**	@}*/

/**@}*/

/*End public API*/


	/** \cond INTERNAL*/
#		define OOLUA_PUBLIC_INHERIT(...)		OOLUA_VA_CONCAT(OOLUA_PUBLIC_INHERIT_, OOLUA_NARG(__VA_ARGS__))(__VA_ARGS__)

	/*This is here because EXPORT_OOLUA_FUNCTIONS_N uses OOLUA_NARG*/
#		define EXPORT_OOLUA_FUNCTIONS_0(mod, ClassType) \
			EXPORT_OOLUA_FUNCTIONS_0_(mod, ClassType)

#		define EXPORT_OOLUA_FUNCTIONS_N(mod, ClassType, ...) \
			OOLUA_VA_CONCAT(OOLUA_VA_CONCAT(EXPORT_OOLUA_FUNCTIONS_, OOLUA_NARG(__VA_ARGS__)), _)(mod, ClassType, __VA_ARGS__)

#		define OOLUA_VA_CONSTRUCTOR(...) \
			OOLUA_VA_CONCAT(INTERNAL::Constructor, OOLUA_NARG(__VA_ARGS__))

#		define VA_PARAM_TYPES(...) \
			OOLUA_VA_CONCAT(VA_, OOLUA_NARG(__VA_ARGS__))(__VA_ARGS__)

/** \endcond*/

#	endif

/** \addtogroup OOLuaDSL DSL
@{
		\def OOLUA_TAGS
		\hideinitializer
		\brief Allows more information to be specified about the proxy class
		\details Tags specifiy more information about the class which should be exposed,
		such as:
		\li Does the class support any operators?
		\li Is it abstract ?
		\li Does the class have enumerations?
		<p>
		OOLUA_TAGS(TagList)
		\param TagList Comma seperated list of \ref OOLuaTags

		\note An OOLUA_TAGS list without any \ref OOLuaTags entries is invalid.
	 */
#		define OOLUA_TAGS(...) OOLUA_TAGS_START __VA_ARGS__ OOLUA_TAGS_END
/*@}*/


#endif //OOLUA_DSL_VA_ARGS_H_
