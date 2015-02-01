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
	\file proxy_function_dispatch.h
*/
#ifndef OOLUA_PROXY_FUNCTION_DISPATCH_H_
#	define OOLUA_PROXY_FUNCTION_DISPATCH_H_

#include "lua_includes.h"
#include "proxy_class.h"
#include "class_from_stack.h"
#include "oolua_config.h"


/** \cond INTERNAL */

#if	OOLUA_USE_EXCEPTIONS ==1
#	include "oolua_exception.h"

#	define OOLUA_PROXY_CALL_CATCH_RESPONSE(exception_type, what_message) \
		luaL_error(vm, "Type of exception: %s.\n what(): %s.\n When calling function on proxy type: %s\n" \
					, exception_type \
					, what_message   \
					, Proxy_type::class_name);



#	define OOLUA_MEMBER_FUNCTION_TRY \
	try \
	{ /*start try block*/
// NOLINT


#	define OOLUA_MEMBER_FUNCTION_CATCH \
	} /*end try block */ \
	catch(OOLUA::Type_error const& e) \
	{ \
		OOLUA_PROXY_CALL_CATCH_RESPONSE("OOLUA::Type_error", e.what()) \
	} \
	catch(std::exception const& e) \
	{ \
		OOLUA_PROXY_CALL_CATCH_RESPONSE("std::exception", e.what()) \
	} \
	catch(...) \
	{ \
		OOLUA_PROXY_CALL_CATCH_RESPONSE("Unknown type", " ") \
	} \
	/*can not return here yet required for the function signature */ \
	/* as it would have either returned in the */ \
	/*try block or called luaL_error which never returns */ \
	return 0;

#endif

#if OOLUA_RUNTIME_CHECKS_ENABLED == 1
#	define OOLUA_SELF_CHECK(obj, mod, reg_type, which_name) \
			if(!obj) \
				luaL_error(vm, "%s \"%s::%s\"%s" \
										, "The self/this instance in the function" \
										, Proxy_type::which_name \
										, (static_cast<typename Proxy_class<Base_type >::reg_type*>(lua_touserdata(vm, lua_upvalueindex(1))))->name \
										, mod " is either NULL or the wrong type is on the stack." \
				); /*NOLINT*/
#else
#	define OOLUA_SELF_CHECK(obj, mod, reg_type, which_name)
#endif



namespace OOLUA
{
	namespace INTERNAL
	{

		template<typename Proxy_type, typename Base_type>int member_caller(lua_State* vm);
		template<typename Proxy_type, typename Base_type>int const_member_caller(lua_State* vm);


		///////////////////////////////////////////////////////////////////////////////
		///  \brief Member function dispatcher
		///	 \details This is the member function dispatcher which has the signature
		///  of a \ref lua_CFunction and will be bound to a \ref lua_State by
		///  OOLUA::register_class, to enable the calling of none constant member
		///  functions. It receives the member function which it will call as an upvalue,
		///  which is set a the time the function is bound.
		///  @param[in] vm The lua_State on which to operate
		///  @return int The number of returns from the function to Lua.
		///////////////////////////////////////////////////////////////////////////////
		template<typename Proxy_type, typename Base_type>
		inline int member_caller(lua_State * vm)
		{
#if	OOLUA_USE_EXCEPTIONS == 1
			OOLUA_MEMBER_FUNCTION_TRY
#endif
				typename Proxy_type::class_ *obj = INTERNAL::check_index_no_const<typename Proxy_type::class_>(vm, 1);
				OOLUA_SELF_CHECK(obj, " ", Reg_type, class_name)
				//get member function from upvalue
				typename Proxy_class<Base_type >::Reg_type* r =
						static_cast<typename Proxy_class<Base_type >::Reg_type*>(lua_touserdata(vm, lua_upvalueindex(1)));
				Proxy_type P(obj);
				return (P.*(r->mfunc))(vm); //call member function
#if	OOLUA_USE_EXCEPTIONS == 1
			OOLUA_MEMBER_FUNCTION_CATCH
#endif
		}

		/**
			\brief Dispatcher for constant member functions
		*/
		template<typename Proxy_type, typename Base_type>
		inline int const_member_caller(lua_State * vm)
		{
#if	OOLUA_USE_EXCEPTIONS == 1
			OOLUA_MEMBER_FUNCTION_TRY
#endif
				typename Proxy_type::class_ *obj = INTERNAL::check_index<typename Proxy_type::class_>(vm, 1);
				OOLUA_SELF_CHECK(obj, "const", Reg_type_const, class_name_const)
				//get member function from upvalue
				typename Proxy_class<Base_type >::Reg_type_const* r =
						static_cast<typename Proxy_class<Base_type >::Reg_type_const*>(lua_touserdata(vm, lua_upvalueindex(1)));
				Proxy_type P(obj);
				return (P.*(r->mfunc))(vm); //call member function
#if	OOLUA_USE_EXCEPTIONS == 1
			OOLUA_MEMBER_FUNCTION_CATCH
#endif
		}
	} // namespace INTERNAL //NOLINT

} // namespace OOLUA




#if	OOLUA_USE_EXCEPTIONS == 1
#	undef OOLUA_PROXY_CALL_CATCH_RESPONSE
#	undef OOLUA_MEMBER_FUNCTION_TRY
#	undef OOLUA_MEMBER_FUNCTION_CATCH
#endif

#	undef OOLUA_GET_NONE_CONST_SELF
#	undef OOLUA_GET_CONST_SELF
#	undef OOLUA_SELF_CHECK


/** \endcond */

#endif //OOLUA_PROXY_FUNCTION_DISPATCH_H_
