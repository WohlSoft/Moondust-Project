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
	\file oolua_traits_fwd.h
	Forward declarations of \ref OOLuaTraits
*/

#ifndef OOLUA_TRAITS_FWD_H_
#	define OOLUA_TRAITS_FWD_H_

namespace OOLUA
{
	template<typename T>
	struct in_p;

	template<typename T>
	struct out_p;

	template<typename T>
	struct in_out_p;

	template<typename T>
	struct lua_out_p;

	template<typename T>
	struct light_p;

	struct calling_lua_state;

	template<typename T>
	struct light_return;

	template<typename T>
	struct lua_return;

	template<typename T>
	struct maybe_null;


	template<typename T>
	struct lua_maybe_null;

	template<typename T>
	struct cpp_acquire_ptr;


	template<typename T>
	struct lua_acquire_ptr;

	enum Owner
	{	No_change	/*!< No change of ownership*/
		, Cpp		/*!< Change in ownership, C++ will now own the instance*/
		, Lua		/*!< Change in ownership, Lua will now own the instance*/
	};

	/** \cond INTERNAL */
	namespace INTERNAL
	{
		template<typename T>
		struct Type_enum_defaults;

		template<typename T>
		struct function_return;

		template<typename T>
		struct param_type;

		template<typename T>
		struct return_type_traits;

		template<typename T>
		struct lua_return_count;

	} //namespace INTERNAL //NOLINT(readability/namespace)
	/** \endcond */
} // namespace OOLUA

#endif //OOLUA_TRAITS_FWD_H_
