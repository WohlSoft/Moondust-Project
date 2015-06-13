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
	\file proxy_userdata.h
	\brief Contains the internal userdata type used by OOLua to represent C++ class types,
	also contains inlined functions for checking and setting flags in the userdata.
*/
#ifndef OOLUA_PROXY_USERDATA_H_
#	define OOLUA_PROXY_USERDATA_H_

#	include "oolua_config.h"
#	include "lvd_types.h"
struct lua_State;
namespace OOLUA
{
	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		struct Lua_ud;
		typedef void (*oolua_function_check_base)(INTERNAL::Lua_ud* __restrict, INTERNAL::Lua_ud const* __restrict);
		typedef void (*oolua_type_check_function)(lua_State * vm);

		/**
			\brief The internal type which is used by the library to represent C++ classes.
		*/
		struct Lua_ud
		{
			void* void_class_ptr;
			oolua_function_check_base base_checker;
			oolua_type_check_function type_check;
			LVD::uint32 flags;
		};

#if OOLUA_CHECK_EVERY_USERDATA_IS_CREATED_BY_OOLUA == 1 && OOLUA_USERDATA_OPTIMISATION == 1
	/*lowest nibble is reserved for flags*/
#	define OOLUA_MAGIC_COOKIE	0xfC105Ef0
#	define OOLUA_CHECK_COOKIE(flags)(((flags) & 0xfffffff0) == OOLUA_MAGIC_COOKIE)
#	define OOLUA_SET_COOKIE(flags)	( (flags) = (OOLUA_MAGIC_COOKIE | ((flags) & 0xf)) )
#endif

		void userdata_const_value(Lua_ud* ud, bool value);
		bool userdata_is_constant(Lua_ud const* ud);
		bool userdata_is_to_be_gced(Lua_ud const * ud);
		void userdata_gc_value(Lua_ud* ud, bool value);

		enum UD_FLAGS {CONST_FLAG = 0x01, GC_FLAG = 0x02};

		inline void userdata_const_value(Lua_ud* ud, bool value)
		{
			if(value)ud->flags |= CONST_FLAG;
			else ud->flags &= (~CONST_FLAG);
		}
		inline bool userdata_is_constant(Lua_ud const* ud)
		{
			return (ud->flags & CONST_FLAG) != 0;
		}
		inline bool userdata_is_to_be_gced(Lua_ud const* ud)
		{
			return (ud->flags & GC_FLAG ) !=0;
		}
		inline void userdata_gc_value(Lua_ud* ud, bool value)
		{
			if(value) ud->flags |= GC_FLAG;
			else ud->flags &= (~GC_FLAG);
		}

	} // namespace INTERNAL //NOLINT
	/**\endcond */

} // namespace OOLUA

#endif //OOLUA_PROXY_USERDATA_H_
