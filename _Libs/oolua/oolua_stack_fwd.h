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
	\file oolua_stack_fwd.h
 	Forward declarations of the push and pull methods, which provide simple
 	interaction with the Lua stack.
*/


#ifndef OOLUA_STACK_FWD_H_
#	define OOLUA_STACK_FWD_H_

#include "oolua_traits_fwd.h" //required for OOLUA::Owner

struct lua_State;

namespace OOLUA
{

	template<int ID>
	struct Lua_ref;

	class Table;

	//this does not take into account calling convention!!
	typedef int(*oolua_CFunction)(lua_State* vm); //NOLINT(readability/casting)

	/**@{*/
	/**
		\brief Pushes an instance to top of the Lua stack
		\return
		If \ref OOLUA_STORE_LAST_ERROR is set to one then the the return value will
		indicate success or failure, if \ref OOLUA_USE_EXCEPTIONS is set to one then
		failure will always be reported by throwing an exception.
		\note Although all push methods return a boolean, most simply return true.
		The only versions which can return false are full userdata aswell as values
		which are associated with a Lua universe.
		\see \ref OOLUA::can_xmove
		\see OOLuaErrorReporting
		\see OOLuaExceptionClasses
	*/
	bool push(lua_State* const vm, void* lightud);
	bool push(lua_State* const vm, bool const& value);
	bool push(lua_State* const vm, char * const& value);
	bool push(lua_State* const vm, char const * const& value);
	bool push(lua_State* const vm, double const& value);
	bool push(lua_State* const vm, float const&  value);
	bool push(lua_State* const vm, oolua_CFunction const &  value);
	bool push(lua_State* const vm, Table const &  value);

	template<typename T>
	bool push(lua_State* const vm, T * const &  value);
	template<typename T>
	bool push(lua_State* const vm, lua_acquire_ptr<T>&  value);
	template<typename T>
	bool push(lua_State* const vm, T const &  value);
	/**@}*/

	/** \cond INTERNAL */
	template<typename T>
	bool push(lua_State* const vm, T * const &  value, OOLUA::Owner);
	/** \endcond */

	/**
		@{
		\brief Pulls the top element off the stack and pops it.
		\details In stack terms this is a top followed by pop.
		\return
		If \ref OOLUA_STORE_LAST_ERROR is set to one then the the return value will
		indicate success or failure, if \ref OOLUA_USE_EXCEPTIONS is set to one then
		failure will always be reported by throwing an exception.
		\see OOLuaErrorReporting
		\see OOLuaExceptionClasses
	*/
	bool pull(lua_State* const vm, void*& lightud);
	bool pull(lua_State* const vm, bool& value);
	bool pull(lua_State* const vm, double& value);
	bool pull(lua_State* const vm, float& value);
	bool pull(lua_State* const vm, oolua_CFunction& value);
	bool pull(lua_State* const vm, Table&  value);

	template<typename T>
	bool pull(lua_State* const vm, T *&  value);
	template<typename T>
	bool pull(lua_State* const vm, T&  value);
	template<typename T>
	bool pull(lua_State* const vm, cpp_acquire_ptr<T>&  value);
	/**	@}*/

	/** \cond INTERNAL */
	namespace INTERNAL
	{
		namespace LUA_CALLED
		{
			/**
				@{
				\brief Internal function used to get a value from the stack
				\details These functions differ from the public API pull methods by:
					\li Using a stack index on which to operate and by not popping the value
					\li Reporting errors differently.
				<p>
				By treating the stack differently, it ensures that the value is not garbage collected
				whilst in the process of a C++ proxied function call.
			*/
			void get(lua_State* const vm, int idx, void*& value);
			void get(lua_State* const vm, int idx, bool& value);
			void get(lua_State* const vm, int idx, char const*& value);
			void get(lua_State* const vm, int idx, char *& value);
			void get(lua_State* const vm, int idx, double& value);
			void get(lua_State* const vm, int idx, float& value);
			void get(lua_State* const vm, int idx, oolua_CFunction& value);
			void get(lua_State* const vm, int idx, Table&  value);

			template<typename T>
			void get(lua_State* const vm, int idx, T& value);

			template<typename T>
			void get(lua_State* const vm, int idx, T *&  value);

			template<typename T>
			void get(lua_State* const vm, int idx, OOLUA::cpp_acquire_ptr<T>&  value);
			/**	@}*/

		} // namespace LUA_CALLED // NOLINT

	} // namespace INTERNAL // NOLINT
	/** \endcond */
} // namespace OOLUA

#endif //OOLUA_STACK_FWD_H_
