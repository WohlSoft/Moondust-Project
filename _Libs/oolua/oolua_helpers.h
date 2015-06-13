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
	\file oolua_helpers.h
*/
#ifndef OOLUA_HELPERS_H_
#	define OOLUA_HELPERS_H_

struct lua_State;

namespace OOLUA
{
	/**
		Compares two valid indices on the stack of vm.
		\details This takes into consideration metamethods for the indices
		\param[in] vm The lua_State in which to prefer the operation
		\param[in] idx0 Valid stack index
		\param[in] idx1 Valid stack index
		\return boolean Result of the comparison
	*/
	bool idxs_equal(lua_State* vm, int idx0, int idx1);

	/** \brief Uses the Lua C API to check if it is valid to move data between the states
		\details
		lua_xmove returns without doing any work if the two pointers are the same
		and fails when using LUA_USE_APICHECK and the states do not share the same
		global_State.

		It may be fine to move numbers between different unrelated states when Lua
		was not compiled with LUA_USE_APICHECK but this function would still return
		false for that scenario.

		\param[in] vm0
		\param[in] vm1
		\returns true is vm0 and vm1 are different yet none NULL related states, else false
	*/
	bool can_xmove(lua_State* vm0, lua_State* vm1);
} //namespace OOLUA

#endif //OOLUA_HELPERS_H_
