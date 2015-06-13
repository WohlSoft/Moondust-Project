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
	\file oolua_error.h
	\brief Generic header to be included when handling errors.
	\details When the library is compiled with \ref OOLUA_USE_EXCEPTIONS == 1
	it will include the \ref oolua_exception.h header and provide dummy
	implemenations for OOLUA::get_last_error and OOLUA::reset_error_value.
	When compiled with \ref OOLUA_STORE_LAST_ERROR == 1 it provides implements
	for \ref OOLUA::get_last_error and \ref OOLUA::reset_error_value.
	\see \ref OOLuaConfig
*/
#ifndef OOLUA_ERROR_H_
#	define OOLUA_ERROR_H_

#include "oolua_config.h"

#if OOLUA_USE_EXCEPTIONS == 1
#	include "oolua_exception.h"
#endif

#if OOLUA_DEBUG_CHECKS == 1
#   include <cassert>
#endif

#	include <string>

struct lua_State;
namespace OOLUA
{
	/** \addtogroup OOLuaErrorReporting
	@{*/

	/** \brief Reset the error state such that a call to OOLUA::get_last_error
		will return an empty string.
		\see OOLuaErrorReporting
		\note This function is a nop when \ref OOLUA_STORE_LAST_ERROR is not enabled
	*/
	void reset_error_value(lua_State* vm);
	/** \brief Returns the last stored error
		\return empty string if there is not an error else the error message
		\see OOLuaErrorReporting
		\note This function is a nop when \ref OOLUA_STORE_LAST_ERROR is not enabled
	*/
	std::string get_last_error(lua_State* vm);
	/**@}*/

	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		void set_error_from_top_of_stack_and_pop_the_error(lua_State* vm);
	} // namespace INTERNAL
	/** \endcond */

} // namespace OOLUA

#endif
