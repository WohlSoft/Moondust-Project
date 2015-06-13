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

#ifndef PUSH_POINTER_INTERAL_H_
#	define PUSH_POINTER_INTERAL_H_

#include "proxy_userdata.h"
#include "proxy_storage.h"
#include "lvd_type_traits.h"

struct lua_State;

namespace OOLUA
{
	/** \cond INTERNAL*/
	namespace INTERNAL
	{
		template<typename Raw, typename TypeMaybeConst>
		void push_pointer_which_has_a_proxy_class(lua_State * vm, TypeMaybeConst * const ptr, Owner owner);

		void if_check_enabled_check_type_is_registered(lua_State* vm, char const* name);
		void set_owner_if_change(Owner owner, Lua_ud* ud);

		template<typename Raw, typename TypeMaybeConst>
		inline void push_pointer_which_has_a_proxy_class(lua_State * vm, TypeMaybeConst * const ptr, Owner owner)
		{
			if_check_enabled_check_type_is_registered(vm, Proxy_class<Raw>::class_name);
			Lua_ud* ud(find_ud(vm, (Raw*)ptr, !!LVD::is_const<TypeMaybeConst>::value));//NOLINT
			if(!ud) ud = add_ptr(vm, (Raw*)ptr, !!(LVD::is_const<TypeMaybeConst>::value), owner);//NOLINT
			else set_owner_if_change(owner, ud);
		}

	} // namespace INTERNAL // NOLINT
	/**\endcond*/
} // namespace OOLUA

#endif
