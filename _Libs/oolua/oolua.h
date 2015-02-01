/**
	\file oolua.h
	Header file for Object Oriented Lua.
*/

/**
	\mainpage

	\section OOLuaIntroduction Introduction
	\subsection OOLuaHipster Hipster
	OOLua is cross platform, test driven, dependancy free Open Source library which uses C++03
	template meta-programming and pre-processor magic to generate non intrusive proxies that
	provide a fast bridge for the interaction of C++ classes with Lua; in addition it also
	provides a thin abstraction layer for interfacing with the Lua stack. It supports multiple
	inheritance C++ classes without using C++ RTTI and does not use exceptions by default
	although they are easily enabled.

	\subsection OOLuaNormal Normal
	OOLua is a library which makes it easy to use C++ classes in Lua and also operating
	on the stack using a typed interface for common operations.

	This is not a fully original work, instead it builds on ideas from binding
	classes using <a href="http://lua-users.org/wiki/CppBindingWithLunar" target="_blank">Lunar</a>
	and <a href="http://www.lua.org/notes/ltn005.html" target="_blank">Lua Technical Note 5</a>.

	\section OOLuaCompatibility Lua compatibility
	This version of the library is compatible with the following Lua implementations
		\li Rio Lua 5.1 and 5.2 http://www.lua.org
		\li LuaJIT 1.1.8 and 2.0 http://www.luajit.org/

	\section OOLuaLinks Links
		\li Project Home  http://oolua.org
		\li Library documentation http://oolua.org/docs
		\li Issue tracker http://oolua.org/issues
		\li Mailing list http://oolua.org/mailinglist

	\section OOLuaLicence Licence
	OOLua:
	\copyright
	The MIT License\n
	Copyright (c) 2009 - 2013 Liam Devine\n
	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:\n
 	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.\n\n
	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.\n

	Loki Type lists:
	\copydetails type_list.h

	Luna :
	\copydetails oolua_registration.h
*/


#ifndef OOLUA_H_
#	define OOLUA_H_


#	include "lua_includes.h"
#	include "oolua_dsl.h"
#	include "proxy_function_exports.h"
#	include "oolua_version.h"
#	include "oolua_error.h"
#	include "oolua_stack.h"
#	include "oolua_script.h"
#	include "oolua_open.h"
#	include "oolua_chunk.h"
#	include "oolua_registration.h"
#	include "oolua_table.h"
#	include "oolua_ref.h"
#	include "oolua_helpers.h"

/**
	\namespace OOLUA
	\brief This is the root namespace of the Library
	\details There are sub namespaces contained in OOLUA yet mostly these are not
	meant for general usage, instead this namespace contains all Public API functions,
	structures etc.
*/
namespace OOLUA
{
	/**
		\brief Helper function to set a Lua global variable.
		\tparam T Type for instance
		\param[in] vm lua_State
		\param[in] name Global name to set
		\param[in] instance Any variable which is valid to push to the stack
		\return Boolean indicating if the operation was successful \see OOLuaErrorReporting
	*/
	template<typename T>
	bool set_global(lua_State* vm, char const* name, T& instance)
	{
		if ( !OOLUA::push(vm, instance) ) return false;
		lua_setglobal(vm, name);
		return true;
	}

	/**
		\brief None template version
		\details Enables setting a global with a value of lua_CFunction without
		requiring you make a reference to the function.
		\param[in] vm The lua_State to work on
		\param[in] name String which is used for the global name
		\param[in] instance The lua_CFuntion which will be set at the global value for name
	*/
	bool set_global(lua_State* vm, char const* name, lua_CFunction instance);

	/**
		\brief Helper function to set a Lua global variable to nil.
		\param[in] vm lua_State
		\param[in] name Global name to set
	*/
	void set_global_to_nil(lua_State* vm, char const * name);

	/**
		\brief Helper function to set a Lua global variable.
		\tparam T Type for instance
		\param[in] vm lua_State
		\param[in] name Global name to query
		\param[out] instance Any variable which is valid to pull from the stack
		\return Boolean indicating if the operation was successful \see OOLuaErrorReporting
	*/
	template<typename T>
	bool get_global(lua_State* vm, char const* name, T& instance)
	{
		lua_getglobal(vm, name);
		return OOLUA::pull(vm, instance);
	}

} // namespace OOLUA

#endif //OOLUA_H_
