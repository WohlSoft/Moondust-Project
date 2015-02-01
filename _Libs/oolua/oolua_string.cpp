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

#include "oolua_string.h"
#include "stack_get.h"
#include "oolua_push.h"
#include "oolua_pull.h"
#include "oolua_config.h"
#include "lua_includes.h"


#include <string>

namespace OOLUA
{
	namespace STRING
	{
		template<>
		bool push<std::string>(lua_State* const vm, std::string const& value)
		{
			lua_pushlstring(vm, value.data(), value.size());
			return true;
		}

		template<>
		bool pull<std::string>(lua_State* const vm, std::string& value)
		{
			if( !INTERNAL::cpp_runtime_type_check_of_top(vm, LUA_TSTRING, "std::string") ) return false;
			size_t len(0);
			char const* str = lua_tolstring(vm, -1, &len);
			value = std::string(str, len);
			lua_pop(vm, 1);
			return true;
		}

#if OOLUA_STD_STRING_IS_INTEGRAL == 1
		template<>
		void get<std::string>(lua_State* const vm, int idx, std::string &  value)
		{
#if OOLUA_RUNTIME_CHECKS_ENABLED  == 1
			if(lua_type(vm, idx) != LUA_TSTRING ) INTERNAL::LUA_CALLED::get_error(vm, idx, "std::string");
#endif
			size_t len(0);
			char const* str = lua_tolstring(vm, idx, &len);
			value = std::string(str, len);
		}
#endif

	} // namespace STRING //NOLINT(readability/namespace)
} // namespace OOLUA
