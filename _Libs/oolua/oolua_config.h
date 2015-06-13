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
	\file oolua_config.h
	Configuration options for the OOLua library.
*/
#ifndef OOLUA_CONFIG_H_
#	define OOLUA_CONFIG_H_

/** \addtogroup OOLuaConfig
@{ */

	/** \addtogroup OOLuaErrorReporting Error Reporting
	@{
		\brief
		Defines how any errors are reported
		\details Errors can be reported either by using exceptions or storing
		a retreivable error string, only one of these methods is allowed
		and this condition is enforced, yet also neither are required. If both
		are disabled then it depends on \ref OOLUA_DEBUG_CHECKS as to whether
		any error will be reported
	*/

	/**	\def OOLUA_USE_EXCEPTIONS
		\hideinitializer
        \brief \b Default: Disabled
		\details
		Throws exceptions from C++ code.
		This could be the return of a pcall, or from pulling an incorrect type
		off the stack when \ref OOLUA_RUNTIME_CHECKS_ENABLED is enabled. It also
		prevents exceptions escaping from function proxied by the library, enabling
		calls to such functions to be caught with pcall in Lua code.
		\param 0 Disabled
		\param 1 Enabled
	*/
#	ifndef OOLUA_USE_EXCEPTIONS
#		define OOLUA_USE_EXCEPTIONS 0
#	endif


	/** \def OOLUA_STORE_LAST_ERROR
		\hideinitializer
		\brief \b Default: Enabled
		\details
		Stores an error message in the registery overwriting any previous error,
		the last error to have occured is retrievable via \ref OOLUA::get_last_error
		\see OOLUA::get_last_error
		\see OOLUA::reset_error_value
		\param 0 Disabled
		\param 1 Enabled
	*/
//#define OOLUA_STORE_LAST_ERROR 0
#	ifndef OOLUA_STORE_LAST_ERROR
#		define OOLUA_STORE_LAST_ERROR 1
#	endif

#	if OOLUA_USE_EXCEPTIONS == 1 && OOLUA_STORE_LAST_ERROR == 1
#		error Only one of these settings can be on
#	endif

	/**@}*/


	/** \addtogroup OOLuaErrorChecking Error Checking
	@{
		\brief Defines the type of checks which will be performed
	*/

	/** \def OOLUA_RUNTIME_CHECKS_ENABLED
		\hideinitializer
		\brief \b Default: Enabled
		\details
		Checks that a type being pulled off the stack is of the correct type,
		if this is a proxy type, it also checks the userdata on the stack was
		created by OOLua
		\param 0 Disabled
		\param 1 Enabled
	*/
//#define OOLUA_RUNTIME_CHECKS_ENABLED 0
#	ifndef OOLUA_RUNTIME_CHECKS_ENABLED
#		define OOLUA_RUNTIME_CHECKS_ENABLED 1
#	endif

	/** \def OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA
		\hideinitializer
		\brief \b Default: Enabled
		\details
		Does what it says on the tin, only valid when
		\ref OOLUA_RUNTIME_CHECKS_ENABLED is enabled
		\param 0 Disabled
		\param 1 Enabled
	 */
//define OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA 0
#	if OOLUA_RUNTIME_CHECKS_ENABLED == 1
#		ifndef OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA
#			define OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA 1
#		endif
#	else
#		ifdef OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA
#			undef OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA
#		endif
#		define OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA  0
#	endif

	/** \def OOLUA_USERDATA_OPTIMISATION
		\hideinitializer
		\brief \b Default: Enabled
		\details
		Userdata optimisation which checks for a magic cookie to try and ensure
		it was created by OOLua, by default this is on when userdata checking
		is on. Turning this off by setting it to zero will use a slower yet
		correct (as correct as can be) method. \n
		Only meaningful when \ref OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA is
		enabled
		\param 0 Disabled
		\param 1 Enabled
	 */
#	if OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA == 1
#		ifndef OOLUA_USERDATA_OPTIMISATION
#			define OOLUA_USERDATA_OPTIMISATION 1
#		endif
#	endif

	/** \def OOLUA_DEBUG_CHECKS
		\hideinitializer
		\brief \b Default: Enabled when DEBUG or _DEBUG is defined
		\details
			\li Adds Checks for NULL pointers
			\li Adds a stack trace to errors reported by pcall
			\li Calls assert on errors if both \ref OOLUA_USE_EXCEPTIONS and
				\ref OOLUA_STORE_LAST_ERROR are both disabled
		\param 0 Disabled
		\param 1 Enabled
	*/
#	if defined DEBUG || defined _DEBUG
#		define OOLUA_DEBUG_CHECKS 1
#	else
#		define OOLUA_DEBUG_CHECKS 0
#	endif

	/** \def OOLUA_SANDBOX
		\hideinitializer
		\brief \b Default: Disabled
		\details check everything
		\param 0 Disabled
		\param 1 Enabled
	*/
#	define OOLUA_SANDBOX 0

#	if OOLUA_SANDBOX == 1
#		if defined OOLUA_RUNTIME_CHECKS_ENABLED && OOLUA_RUNTIME_CHECKS_ENABLED == 0
#			undef OOLUA_RUNTIME_CHECKS_ENABLED
#			define OOLUA_RUNTIME_CHECKS_ENABLED 1
#		endif

#		if defined OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA && OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA == 0
#			undef OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA
#		endif

#		ifndef OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA
#			define OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA 1
#		endif

//if neither are set then default to storing errors
#		if defined OOLUA_USE_EXCEPTIONS && OOLUA_USE_EXCEPTIONS == 0 \
				&& defined OOLUA_STORE_LAST_ERROR && OOLUA_STORE_LAST_ERROR == 0
#			undef OOLUA_STORE_LAST_ERROR
#			define OOLUA_STORE_LAST_ERROR 1
#		endif
#	endif

	/**@}*/


	/** \def OOLUA_STD_STRING_IS_INTEGRAL
		\hideinitializer
		\brief \b Default: Enabled
		\details
		Allows std::string to be a parameter or a return type for a function.
		\note This is always by value
		\param 0 Disabled
		\param 1 Enabled
	 **/
#	ifndef OOLUA_STD_STRING_IS_INTEGRAL
#		define OOLUA_STD_STRING_IS_INTEGRAL 1
#	endif

/**@}*/

/** \cond INTERNAL */

#ifdef _MSC_VER
#	define MSC_PUSH_DISABLE_CONDITIONAL_CONSTANT_OOLUA \
__pragma(warning(push)) /*NOLINT*/ \
__pragma(warning(disable : 4127)) /*NOLINT*/
#	define MSC_POP_COMPILER_WARNING_OOLUA \
__pragma(warning(pop)) /*NOLINT*/
#else
#	define MSC_PUSH_DISABLE_CONDITIONAL_CONSTANT_OOLUA
#	define MSC_POP_COMPILER_WARNING_OOLUA
#endif


#ifdef __GNUC__
/*
From GCC version 4.6 onwards you are meant to be able to push and pop diagnostic
settings which would allow turning variadic macro warnings off per file.
This was previously achieved by using the pramga GCC system_header; yet this may
hide other warnings when compiled with a combination of -ansi, -pedantic and -wall
From version 4.2 you can use :
pragma GCC diagnostic ignored [option]
yet this can effect more than the file in which it wants supressing and could effect
a users code base which is far from ideal.

GCC suggests to compile with -fdiagnostics-show-option which results in the following
warning for vaargs

warning: anonymous variadic macros were introduced in C99 [-Wvariadic-macros]

It would therefore seem reasonable that the following code could be used to prevent
vaarg warnings in a limited scope without effecting a user's compile options:

#if defined __GNUC__ && defined __STRICT_ANSI__
#	if __GNUC__ >4  || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 6)
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wvariadic-macros"
#	else
#		pragma GCC system_header
#	endif
#endif

 Code wanting to be protected here

#if defined __GNUC__ && defined __STRICT_ANSI__ && ( __GNUC__ >4  || ( __GNUC__ == 4 && __GNUC_MINOR__ >= 6) )
#	pragma GCC diagnostic push
#endif

However using a 4.6 version of GCC and the above code does not prevent the warnings

*/




#	define GCC_VERSION_OOLUA (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
//	SShhhhhhhhh stops gcc generating warnings about the variadic macros
#	define GCC_PRAGMA_DO(x) _Pragma(#x)
#	define GCC_PRAGMA(x) GCC_PRAGMA_DO(GCC x)

#	if GCC_VERSION_OOLUA >= 40600
#		define GCC_PUSH_DISABLE_VA_WARNINGS_OOLUA \
			GCC_PRAGMA(diagnostic push) \
			GCC_PRAGMA(diagnostic ignored "-Wvariadic-macros")
#		define GCC_POP_VA_WARNINGS_OOLUA GCC_PRAGMA(diagnostic pop)
#	else
//#		define GCC_PUSH_DISABLE_VA_WARNINGS_OOLUA GCC_PRAGMA_DO(system_header)
//#		define GCC_PUSH_DISABLE_VA_WARNINGS_OOLUA _Pragma("GCC system_header") //doesn't work boooooooooo
#		define GCC_POP_VA_WARNINGS_OOLUA
#	endif
#else
#	define GCC_PUSH_DISABLE_VA_WARNINGS_OOLUA
#	define GCC_POP_VA_WARNINGS_OOLUA
#endif


#if OOLUA_USE_EXCEPTIONS == 1
#	if defined __GNUC__ && ( ( !defined __EXCEPTIONS) || (defined __EXCEPTIONS && __EXCEPTIONS != 1) )
#			error OOLua has been compiled with exceptions yet they have been disabled for this build
#	elif defined _MSC_VER //&& !defined _HAS_EXCEPTIONS
#		if defined(_HAS_EXCEPTIONS) && (_HAS_EXCEPTIONS == 0)
#			error OOLua has been compiled with exceptions yet they have been disabled for this build
#		endif
#	endif
#endif

/** \endcond */

#endif
