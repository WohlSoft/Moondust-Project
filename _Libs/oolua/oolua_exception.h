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
	\file oolua_exception.h
	Declares the exceptions which are used by OOLua when \ref OOLUA_USE_EXCEPTIONS
	is set to one.
	\see \ref OOLuaConfig
	\see \ref OOLuaExceptionClasses
*/
#ifndef OOLUA_EXCEPTION_H_
#	define OOLUA_EXCEPTION_H_


#include "oolua_config.h"

/** \addtogroup OOLuaErrorReporting
@{*/
	/** \addtogroup OOLuaExceptionClasses Exception classes
		@{
	*/
	/** \struct std::exception */

	/** \struct OOLUA::Exception
		\brief Base class for OOLua exceptions
		\extends std::exception
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::Syntax_error
		\brief Reports LUA_ERRSYNTAX
		\extends OOLUA::Exception
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::Runtime_error
		\brief Reports LUA_ERRRUN
		\extends OOLUA::Exception
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::Memory_error
		\brief Reports LUA_ERRMEM
		\extends OOLUA::Exception
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::File_error
		\brief Reports LUA_ERRFILE
		\extends OOLUA::Exception
		\see OOLuaErrorReporting
	 */

	/** \struct OOLUA::Type_error
		\brief Reports that a type pulled from the stack was not the type that was asked for
		\extends OOLUA::Exception
		\see OOLuaErrorReporting
		\note Implicit casts such as a derived class to a base class are not type errors
	 */
		/**@}*/

/**@}*/

#	if OOLUA_USE_EXCEPTIONS == 1

#	include <exception>
	struct lua_State;

	namespace OOLUA
	{
		namespace ERROR
		{
			struct PopTheStack{};
			enum exception_buffer {size = 512};
		} // namespace ERROR

		struct Exception : std::exception
		{
			Exception(lua_State* vm);
			Exception(lua_State* vm, ERROR::PopTheStack*);
			Exception(char const* msg);
			Exception(Exception const& rhs);
			Exception& operator = (Exception const& rhs) throw();
			char const* what() const throw();

			size_t m_len;
			char m_buffer[ERROR::size];
		};

		struct Syntax_error : Exception
		{
			Syntax_error(lua_State* vm);
			Syntax_error(lua_State* vm, ERROR::PopTheStack* specialisation);
		};

		struct Runtime_error : Exception
		{
			Runtime_error(lua_State* vm);
			Runtime_error(lua_State* vm, ERROR::PopTheStack* specialisation);
			Runtime_error(char const* msg);
		};

		struct Memory_error : Exception
		{
			Memory_error(lua_State* vm);
			Memory_error(lua_State* vm, ERROR::PopTheStack* specialisation);
		};

		struct File_error : Exception
		{
			File_error(lua_State* vm);
			File_error(lua_State* vm, ERROR::PopTheStack* specialisation);
		};

		struct Type_error : Exception
		{
			Type_error(lua_State* vm, ERROR::PopTheStack* specialisation);
		};

	} // namespace OOLUA // NOLINT

#	endif

#endif
