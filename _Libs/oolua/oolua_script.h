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
	\file oolua_script.h
	\brief Provides the helper class OOLUA::Script
*/

#ifndef OOLUA_SCRIPT_H_
#	define OOLUA_SCRIPT_H_

#include "lua_includes.h"
#include "oolua_stack_fwd.h"
#include "oolua_registration_fwd.h"
#include "oolua_function.h"
#include <string>

namespace OOLUA
{

	/** \class Script
		\brief OOLua helper class
		\details
		OOLua is purposely designed not to be dependent on the \ref OOLUA::Script "Script" class
		and therefore passes around it's dependency of a lua_State instance.
		The Script class is only a helper and anything you can do with it
		can be accomplished either via using a \ref OOLUA::Lua_function "Lua_function" struct,
		calling OOLUA namespaced functions or using the Lua C API.

		Script provides the following :
		\li Scopes a lua_State pointer
		\li Provides access to the lua_State pointer via a \ref OOLUA::Script::operator "cast operator" and \ref OOLUA::Script::state "function"
		\li Provides methods to \ref OOLUA::Script::register_class "register" types
		\li Binds a \ref OOLUA::Lua_function "Lua_function" instance to \ref OOLUA::Script::call "call" functions
		\li Has member functions for a little state management
		\li \ref OOLUA::setup_user_lua_state "Sets up" the state to work with OOLua

		\note
		This class is not copy constructible or assignable.
		To accomplish this a counted reference to the lua_State would need to be
		maintained.
		\note If you do not want to or can not use this class please see
		\ref OOLUA::setup_user_lua_state "setup_user_lua_state"
	*/
	class Script
	{
	public:
		/** Function object instance which can be used to call Lua functions*/
		Lua_function call;

		/**@{*/
		Script();
		~Script();
		/**@}*/

		/**@{*/
		/** \brief Returns the stack count from the lua_State */
		int stack_count(){ return lua_gettop(m_lua); }
		/** \brief Conversion operator so that a \ref OOLUA::Script "Script"
			instance can be passed in place of a lua_State pointer*/
		operator lua_State * () const {return m_lua;}
		/** \brief Sometimes you may want to be explicit \see Script::operator()  */
		lua_State * const & state() const {return m_lua;}
		/** \brief Performs a garbage collection on the state*/
		void gc();
		/**@}*/

		/**@{*/
		/** \brief Helper function \see OOLUA::register_class*/
		template<typename T>
		void register_class();
		/** \brief Helper function \see OOLUA::register_class*/
		template<typename T>
		void register_class(T* /*dummy*/); // NOLINT
		/** \brief Helper function \see OOLUA::register_class_static */
		template<typename T, typename K, typename V>
		void register_class_static(K const& k, V const& v);
		/**@}*/

		/**@{*/
		/** \brief Helper function \see OOLUA::run_file */
		bool run_file(std::string const & filename);
		/** \brief Helper function \see OOLUA::load_file */
		bool load_file(std::string const & filename);
		/** \brief Helper function \see OOLUA::load_chunk */
		bool load_chunk(std::string const& chunk);
		/** \brief Helper function \see OOLUA::run_chunk */
		bool run_chunk(std::string const& chunk);
		/**@}*/

		/**@{*/
		/** \brief Helper function \see OOLUA::pull */
		template<typename T>
		bool pull(T & t);
		/** \brief Helper function \see OOLUA::push */
		template<typename T>
		bool push(T const& t);
		/**@}*/

	protected:
		void close_down();
		Script(Script const&);
		Script& operator = (Script const&);
		lua_State* m_lua;
	};


	template<typename T>
	inline void Script::register_class()
	{
		OOLUA::register_class<T>(m_lua);
	}

	template<typename T>
	inline void Script::register_class(T* /*dummy*/) // NOLINT
	{
		register_class<T>();
	}

	template<typename T, typename K, typename V>
	inline void Script::register_class_static(K const& k, V const& v)
	{
			OOLUA::register_class_static<T>(m_lua, k, v);
	}

	template<typename T>
	inline bool Script::push(T const& value)
	{
		return OOLUA::push(*this, value);
	}

	template<typename T>
	inline bool Script::pull(T& value)
	{
		return OOLUA::pull(*this, value);
	}

} //namespace OOLUA

#endif //OOLUA_SCRIPT_H_
