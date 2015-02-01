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

/*
	\file oolua_traits.h
	Traits for the DSL and public API functions.
	\remarks
	For the DSL if parameter traits are not supplied then it defaults to OOLUA::in_p.
	For function return values OOLUA::function_return is used (inside the generating
	function), this template is specialised for void types.
*/

#ifndef OOLUA_TRAITS_H_
#	define OOLUA_TRAITS_H_

#	include "oolua_traits_fwd.h"
#	include "lvd_types.h"
#	include "lvd_type_traits.h"
#	include "proxy_test.h"
#	include "oolua_string.h"

struct lua_State;
namespace OOLUA
{

// TODO :
/*
	pull_type is really an incorrect name for return types
	yet it has this name to be the same as input parameters.
	Consider changing the name of both to something more suitable.
*/

/** \addtogroup OOLuaTraits Traits
@{
\brief Provides direction and/or ownership information.
\details
The general naming convention for traits is:\n
\li \ref OOLuaFunctionParamTraits : end in "_p"
\li \ref OOLuaFunctionReturnTraits : end in "_return" or "_null"
\li \ref OOLuaStackTraits : end in "_ptr".
*/

	/** \addtogroup OOLuaFunctionParamTraits Parameter Traits
	@{
		\brief DSL Traits for function parameter types.
		\details Traits which allow control of ownership include in their name either
		"lua" or "cpp"; directional traits contain "in", "out" or a combination.
	*/
	/** \struct in_p
		\brief Input parameter trait
		\details
		The calling Lua procedure supplies the parameter to the proxied function.
		No change of ownership occurs.
		\note
		This is the default trait used for function parameters when no trait
		is supplied.
	*/
	template<typename T>struct in_p;

	/** \struct out_p
		\brief Output parameter trait
		\details
		The calling Lua procedure does not pass the parameter to the proxied
		function, instead one is created using the default constructor and
		passed to the proxied function. The result after the proxied call with
		be returned to the calling procedure. If this is a type which has a
		proxy then it will cause a heap allocation of the type, which Lua will
		own.
	*/
	template<typename T>struct out_p;

	/** \struct in_out_p
		\brief Input and output parameter trait
		\details
		The calling Lua procedure supplies the parameter to the proxied
		function, the value of the parameter after the proxied call will be
		passed back to the calling procedure as a return value.
		No change of ownership occurs.
	*/
	template<typename T>struct in_out_p;

	/** \struct cpp_in_p
		\brief Input parameter trait which will be owned by C++
		\details Parameter supplied via Lua changes ownership to C++.
	*/
	template<typename T>struct cpp_in_p;


	/** \struct lua_out_p
		\brief Output parameter trait which will be owned by Lua
		\details
		Lua code does not pass an instance to the C++ function, yet the pushed back
		value after the function call will be owned by Lua. This is meaningful only
		if called with a type which has a proxy and it is by reference, otherwise
		undefined.
	*/
	template<typename T>struct lua_out_p;

	/** \struct light_p
		\brief Input parameter trait
		\details
		The calling Lua procedure supplies a LUA_TLIGHTUSERDATA which will be cast to the
		requested T type. If T is not the correct type for the light userdata then
		the casting is undefined.
		A light userdata is never owned by Lua
	 */
	template<typename T>struct light_p;

	/** \struct OOLUA::calling_lua_state
		\brief Special parameter type
		\details This is different from all other traits as it does not take a type, yet
		is a type. It informs OOLua that the calling state is a parameter for a function
	*/
	struct calling_lua_state;
	/**@}*/

	/** \addtogroup OOLuaFunctionReturnTraits Function Return Traits
	@{
		\brief DSL traits for function return types.
		\details Some of the these traits allow for NULL pointers to be returned
		from functions, which was something commonly requested for the library.
		When such a trait is used and the runtime value is NULL, Lua's value of
		nil will be pushed to the stack.
	*/

	/** \struct light_return
		\brief Return trait for a light userdata type
		\details
		The type returned from the function is  either a void pointer or a pointer to another
		type. When the function returns, it will push a LUA_TLIGHTUSERDATA to the stack even
		when the pointer is NULL; therefore a NULL pointer using this traits is never converted
		to a Lua nil value. A light userdata is also never owned by Lua and OOLua does not
		store any type information for the it; light_return is a black box which when used
		incorrectly will invoke undefined behaviour.\n
		This is only valid for function return types.
	*/
	template<typename T>struct light_return;

	/** \struct lua_return
		\brief Return trait for a type which will be owned by Lua
		\details
		The type returned from the function is a heap allocated instance whose
		ownership will be controlled by Lua. This is only valid for function
		return types.
	*/
	template<typename T>struct lua_return;

	/** \struct maybe_null
		\brief Return trait for a pointer which at runtime maybe NULL.
		\details
		The type returned from the function is a pointer instance whose
		runtime value maybe NULL. If it is NULL then lua_pushnil will be called
		else the pointer will be pushed as normal. No change of ownership will
		occur for the type. This is only valid for function return types.
		\note To be consistent in naming this should really be called
		maybe_null_return, however I feel this would be too long a name for the
		trait so "return" has been dropped.
	*/
	template<typename T>struct maybe_null;

	/** \struct lua_maybe_null
		\brief Return trait for a pointer which at runtime maybe NULL and also
		allowing transfer of ownership.
		\details
		The type returned from the function is a pointer instance whose
		runtime value maybe NULL. If it is NULL then lua_pushnil will be called
		else the pointer will be pushed and transfer ownership of the instance
		to Lua. This is only valid for function return types.
		\note To be consistent in naming this should really be called
		lua_maybe_null_return, however I feel this would be too long a name for the
		trait so "return" has been dropped.
	*/
	template<typename T>struct lua_maybe_null;
	/**@}*/

	/** \addtogroup OOLuaStackTraits Stack Traits
	@{
		\brief Public API traits which control a change of ownership
		\details Valid to usage for the \ref OOLUA::INTERNAL "Public API" which interact
		with the Lua stack.
	*/
	/** \struct cpp_acquire_ptr
		\brief Change of ownership to C++
		\details
		Informs the library that C++ will take control of the pointer being used
		and call delete on it when appropriate. This is only valid for public API
		functions which \ref OOLUA::pull from the stack.
	*/
	template<typename T>struct cpp_acquire_ptr;

	/** \struct lua_acquire_ptr
		\brief Change of ownership to Lua
		\details
		Informs the library that Lua will take control of the pointer being used
		and call delete on it when appropriate. This is only valid for public API
		functions which \ref OOLUA::push to the stack.
	*/
	template<typename T>struct lua_acquire_ptr;
	/**@}*/

/**@}*/


	/** \cond INTERNAL*/
	template<typename T>
	class Proxy_class;

	class Table;
	template<int ID>struct Lua_ref;

	namespace INTERNAL
	{
		template<typename T>
		struct Type_enum_defaults
		{
			enum { is_by_value = !LVD::by_reference<typename LVD::remove_all_const<T>::type >::value };
			enum { is_constant =  LVD::is_const<T>::value };
			enum
			{
				is_integral = LVD::is_integral_type< typename LVD::raw_type<T>::raw >::value
					|| /*enum*/(!has_a_proxy_type<typename LVD::raw_type<T>::raw>::value
								&& LVD::can_convert_to_int<typename LVD::raw_type<T>::raw>::value)
			};
		};

		template<typename T, int is_integral>
		struct Pull_type;

		template<typename T>
		struct Pull_type<T, 0>
		{
			typedef T* type;
		};

		template<typename T>
		struct Pull_type<T, 1>
		{
			typedef T type;
		};

		template<typename T, typename Original_Type, int is_integral>struct Pull_type_;

		template<typename Original_Type, typename T>
		struct pull_type_maybe_const
		{
			typedef typename LVD::if_else<
											LVD::is_const<Original_Type>::value
											|| INTERNAL::Type_enum_defaults<Original_Type>::is_by_value
											, T const*
											, T*
										>::type type;
		};

		/*specialisation to prevent Visual Studio warnings, that const applied to a function is ignored*/
		template<typename Original_Type>
		struct pull_type_maybe_const<Original_Type, int (lua_State*)> // NOLINT
		{
			typedef int(*type)(lua_State*); // NOLINT
		};

		/* should rename Pull_maybe_const */
		template<typename T, typename Original_Type>
		struct Pull_type_<T, Original_Type, 0>
		{
			typedef typename pull_type_maybe_const<Original_Type, T>::type type;
		};

		template<typename T, typename Original_Type>
		struct Pull_type_<T, Original_Type, 1>
		{
			typedef T type;
		};

		template<typename T>
		struct is_lua_ref
		{
			enum{value = 0};
		};
		template<int LuaTypeId>
		struct is_lua_ref< OOLUA::Lua_ref<LuaTypeId> >
		{
			enum{value = 1};
		};

		template<typename T>
		struct is_false_integral
		{
			typedef typename LVD::raw_type<T>::type raw;
			enum {value = STRING::is_integral_string_class<raw>::value
						|| INTERNAL::is_lua_ref<raw>::value
						|| LVD::is_same<OOLUA::Table, raw>::value
					};
		};
	} // namespace INTERNAL // NOLINT
	/** \endcond */

	template<typename T>
	struct in_p
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type_<raw, T
								, INTERNAL::Type_enum_defaults<type>::is_integral
								|| INTERNAL::is_false_integral<type>::value>::type pull_type;
		enum {in = 1};
		enum {out = 0};
		enum {owner = No_change};
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral
							|| INTERNAL::is_false_integral<type>::value };
	};

	template<typename T>
	struct in_out_p
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type<raw
									, LVD::is_integral_type<raw>::value
									|| INTERNAL::is_false_integral<type>::value>::type pull_type;
		enum { in = 1};
		enum { out = 1};
		enum { owner = No_change};
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral
								|| INTERNAL::is_false_integral<type>::value };
		typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
	};


	template<>
	struct out_p<void>;

	template<typename T>
	struct out_p
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef raw pull_type;
		enum { in = 0};
		enum { out = 1};
		enum { owner = INTERNAL::Type_enum_defaults<raw>::is_integral
						|| INTERNAL::is_false_integral<type>::value ? No_change : Lua};
		enum { is_by_value = 1};//yes OOLua creates on the stack
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral
							|| INTERNAL::is_false_integral<type>::value };
		typedef char type_has_to_be_by_reference [INTERNAL::Type_enum_defaults<type>::is_by_value ? -1 : 1 ];
	};


	//cpp takes ownership
	template<typename T>
	struct cpp_in_p
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type<raw, LVD::is_integral_type<raw>::value >::type pull_type;
		enum { in = 1};
		enum { out = 0};
		enum { owner = Cpp};
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral  };
		typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
		typedef char type_can_not_be_integral [is_integral ? -1 : 1 ];
		typedef char type_can_not_be_just_a_reference_to_type [	LVD::is_same<raw&, type>::value ? -1 : 1];
		typedef char type_can_not_be_just_a_reference_to_const_type [	LVD::is_same<raw const&, type>::value ? -1 : 1];
	};

	template<typename T>
	struct light_p
	{
		typedef T type;
		typedef void* raw;
		typedef void* pull_type;
		enum { in = 1 };
		enum { out = 0 };
		enum { owner = No_change };
		enum { is_by_value = 0 };
		enum { is_constant = 0 };
		enum { is_integral = 1 };
	};

	template<typename T>
	struct ref_to_const_ptr
	{
		enum{value = 0};
	};
	template<typename T>
	struct ref_to_const_ptr<T*const&>
	{
		enum{value = 1};
	};
	template<typename T>
	struct ref_to_ptr_const
	{
		enum{value = 0};
	};
	template<typename T>
	struct ref_to_ptr_const<T const*&>
	{
		enum{value = 1};
	};

	template<typename T>
	struct lua_out_p
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type_<raw, T, LVD::is_integral_type<raw>::value >::type pull_type;
		enum { in = 0};
		enum { out = 1};
		enum { owner = Lua};
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral  };
		typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
		typedef char type_can_not_be_integral [is_integral ? -1 : 1 ];
		typedef char must_be_assignable [ (is_constant && !ref_to_ptr_const<type>::value)
										 || (ref_to_const_ptr<type>::value)
											? -1 : 1];
		typedef char type_can_not_be_just_a_reference_to_type [	LVD::is_same<raw&, type>::value ? -1 : 1];
		typedef char type_can_not_be_just_a_pointer_to_type [LVD::is_same<raw*, type>::value ? -1 : 1];
	};

	template<typename T>
	struct lua_return
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type_<raw, T, LVD::is_integral_type<raw>::value >::type pull_type;
		enum { in = 0};
		enum { out = 1};
		enum { owner = Lua};
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral  };
		typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
		typedef char type_can_not_be_integral [is_integral ? -1 : 1 ];
		typedef char type_can_not_be_just_a_reference_to_type [	LVD::is_same<raw&, type>::value ? -1 : 1];
		typedef char type_can_not_be_just_a_const_reference_to_type [ LVD::is_same<raw const&, type>::value ? -1 : 1];
	};


	template<typename T>
	struct light_return
	{
		typedef T type;
		typedef void* raw;
		typedef void* pull_type;
		enum { in = 0 };
		enum { out = 1 };
		enum { owner = No_change };
		enum { is_by_value = 0 };
		enum { is_constant = 0 };
		enum { is_integral = 1 };
	};

	template<typename T>
	struct maybe_null
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type_<raw, T, LVD::is_integral_type<raw>::value >::type pull_type;
		enum { in = 0};
		enum { out = 1};
		enum { owner = No_change};
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral  };
		typedef char type_can_not_be_integral [is_integral ? -1 : 1 ];
		typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
		typedef char type_can_not_be_just_a_reference_to_type [	LVD::is_same<raw&, type>::value ? -1 : 1];
		typedef char type_can_not_be_just_a_const_reference_to_type [ LVD::is_same<raw const&, type>::value ? -1 : 1];
		/*Reference to pointer:
		this could be valid in some situations, until such a time as it is required
		or requested disable it*/
		typedef char type_can_not_be_a_reference_to_ptr [ LVD::is_same<raw *&, type>::value ? -1 : 1];
		typedef char type_can_not_be_a_reference_to_const_ptr [ LVD::is_same<raw *const&, type>::value ? -1 : 1];
		typedef char type_can_not_be_a_reference_to_const_ptr_const [ LVD::is_same<raw const*const&, type>::value ? -1 : 1];
		typedef char type_can_not_be_a_reference_to_ptr_const [ LVD::is_same<raw const*&, type>::value ? -1 : 1];
	};


	template<typename T>
	struct lua_maybe_null
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type_<raw, T, LVD::is_integral_type<raw>::value >::type pull_type;
		enum { in = 0};
		enum { out = 1};
		enum { owner = Lua};
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral  };
		typedef char type_can_not_be_integral [is_integral ? -1 : 1 ];
		typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
		typedef char type_can_not_be_just_a_reference_to_type [	LVD::is_same<raw&, type>::value ? -1 : 1];
		typedef char type_can_not_be_just_a_const_reference_to_type [ LVD::is_same<raw const&, type>::value ? -1 : 1];
		/*Reference to pointer:
		this could be valid in some situations, until such a time as it is required
		or requested disable it*/
		typedef char type_can_not_be_a_reference_to_ptr [ LVD::is_same<raw *&, type>::value ? -1 : 1];
		typedef char type_can_not_be_a_reference_to_const_ptr [ LVD::is_same<raw *const&, type>::value ? -1 : 1];
		typedef char type_can_not_be_a_reference_to_const_ptr_const [ LVD::is_same<raw const*const&, type>::value ? -1 : 1];
		typedef char type_can_not_be_a_reference_to_ptr_const [ LVD::is_same<raw const*&, type>::value ? -1 : 1];
	};

	template<typename T>
	struct cpp_acquire_ptr
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type_<raw, T, LVD::is_integral_type<raw>::value>::type pull_type;
		enum { in = 1 };
		enum { out = 0 };
		enum { owner = Cpp };
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral  };

		cpp_acquire_ptr():m_ptr(0){}

		raw* m_ptr;
		typedef char type_can_not_be_integral [is_integral ? -1 : 1 ];
		typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
		typedef char type_can_not_be_just_a_reference_to_type [	LVD::is_same<raw&, type>::value ? -1 : 1];
	};


	template<typename T>
	struct lua_acquire_ptr
	{
		typedef T type;
		typedef typename LVD::raw_type<T>::type raw;
		typedef typename INTERNAL::Pull_type_<raw, T, LVD::is_integral_type<raw>::value >::type pull_type;
		enum { in = 0};
		enum { out = 1};
		enum { owner = Lua};
		enum { is_by_value = INTERNAL::Type_enum_defaults<type>::is_by_value  };
		enum { is_constant = INTERNAL::Type_enum_defaults<type>::is_constant  };
		enum { is_integral = INTERNAL::Type_enum_defaults<type>::is_integral  };

		lua_acquire_ptr(pull_type ptr):m_ptr(ptr){}
		lua_acquire_ptr():m_ptr(0){}
		pull_type m_ptr;
		typedef char type_can_not_be_integral [is_integral ? -1 : 1 ];
		typedef char type_has_to_be_by_reference [is_by_value ? -1 : 1 ];
		typedef char type_can_not_be_just_a_reference_to_type [	LVD::is_same<raw&, type>::value ? -1 : 1];
	};

	/** \cond INTERNAL*/
	namespace INTERNAL
	{

		///////////////////////////////////////////////////////////////////////////////
		///  @struct lua_return_count
		///  Adds together the out values of the traits in the typelist
		///  which is an enum in the type
		///////////////////////////////////////////////////////////////////////////////
		template<typename T>struct lua_return_count;

		template <>
		struct lua_return_count<TYPE::Null_type>
		{
			enum { out = 0 };
		};

		template<typename T, typename T1>
		struct lua_return_count< Type_node<T, T1> >
		{
			enum { out = T::out + lua_return_count<T1>::out };
		};

		///////////////////////////////////////////////////////////////////////////////
		///  @struct has_param_traits
		///  Checks if the type is a trait type or not.
		///////////////////////////////////////////////////////////////////////////////
		template<typename T>struct has_param_traits;

		template<typename T>
		struct has_param_traits
		{
			enum {value = 0};
		};

		template<typename T>
		struct has_param_traits< in_p<T> >
		{
			enum {value = 1};
		};

		template<typename T>
		struct has_param_traits< out_p<T> >
		{
			enum {value = 1};
		};

		template<typename T>
		struct has_param_traits< in_out_p<T> >
		{
			enum {value = 1};
		};
		template<typename T>
		struct has_param_traits< lua_out_p<T> >
		{
			enum {value = 1};
		};
		template<typename T>
		struct has_param_traits< cpp_in_p<T> >
		{
			enum {value = 1};
		};

		template<typename T>
		struct has_param_traits< light_p<T> >
		{
			enum {value = 1};
		};

		///////////////////////////////////////////////////////////////////////////////
		///  @struct param_type_typedef
		///  Typedefs the raw underlying type weather it is already a raw type or
		///  a type wrapped in a trait.
		///////////////////////////////////////////////////////////////////////////////
		template<typename T, bool True>
		struct param_type_typedef
		{
			typedef T traits;
		};

		template<typename T>
		struct param_type_typedef<T, false>
		{
			typedef in_p<T> traits;
		};

		///////////////////////////////////////////////////////////////////////////////
		///  @struct param_type
		///  Uses param_type_typedef and has_param_traits to define the type and its
		///  in_p in_out_p or out_p traits.
		///////////////////////////////////////////////////////////////////////////////
		template<typename T>
		struct param_type
		{
			typedef typename param_type_typedef<T, has_param_traits<T>::value>::traits traits;
			typedef typename traits::type  type;
			typedef typename traits::pull_type pull_type;
			typedef typename traits::raw raw;

			enum { in = traits::in };
			enum { out = traits::out };
			enum { owner = traits::owner };
			enum { is_by_value = traits::is_by_value };
			enum { is_constant = traits::is_constant };
			enum { is_integral = traits::is_integral };
		};

		/**
			\brief Function return trait
			\details Default traits for function return types when none are provied
		*/
		template<typename T>
		struct function_return
		{
			typedef T type;//real type
			typedef typename LVD::raw_type<T>::type raw;//all modifiers removed
			typedef typename Pull_type<raw
								, LVD::is_integral_type<raw>::value
								|| INTERNAL::is_false_integral<type>::value >::type pull_type;
			enum { in = 0};
			enum { out = 1};
			enum { owner = No_change};
			enum { is_by_value = Type_enum_defaults<type>::is_by_value  };
			enum { is_constant = Type_enum_defaults<type>::is_constant  };
			enum { is_integral = Type_enum_defaults<type>::is_integral
									|| INTERNAL::is_false_integral<type>::value  };
		};

		template<>
		struct function_return<void>
		{
			typedef void type;
			typedef void pull_type;
			typedef void raw;
			enum { in = 0};
			enum { out = 0};
			enum { owner = No_change};
			enum { is_by_value = 1 };
			enum { is_constant = 0 };
			enum { is_integral = 1 };
		};

		template<typename T, bool True>
		struct return_type_typedef
		{
			typedef T traits;
		};

		//uses default traits
		template<typename T>
		struct return_type_typedef<T, false>
		{
			typedef function_return<T> traits;
		};


		template<typename T>
		struct has_return_traits
		{
			enum{value = 0};
		};

		template<typename T>
		struct has_return_traits< lua_return<T> >
		{
			enum {value = 1};
		};

		template<typename T>
		struct has_return_traits< light_return<T> >
		{
			enum {value = 1};
		};

		template<typename T>
		struct has_return_traits< maybe_null<T> >
		{
			enum {value = 1};
		};

		template<typename T>
		struct has_return_traits< lua_maybe_null<T> >
		{
			enum {value = 1};
		};

		template<typename T>
		struct return_type_traits
		{
			typedef typename return_type_typedef<T, has_return_traits<T>::value>::traits traits;
			typedef typename traits::type  type;
			typedef typename traits::pull_type pull_type;
			typedef typename traits::raw raw;

			enum { in = traits::in };
			enum { out = traits::out };
			enum { owner = traits::owner };
			enum { is_by_value = traits::is_by_value  };
			enum { is_constant = traits::is_constant  };
			enum { is_integral = traits::is_integral  };
		};

	} // namespace INTERNAL // NOLINT
	/** \endcond */

	///////////////////////////////////////////////////////////////////////////////
	///  Specialisation for the calling lua_State
	///////////////////////////////////////////////////////////////////////////////

	//so this must be a coroutine on the stack
	template struct in_p<lua_State*>;//disable it

	struct calling_lua_state {};

	template<>
	struct in_p<calling_lua_state>
	{
		typedef lua_State* type;
		typedef lua_State* raw;
		typedef lua_State* pull_type;
		enum {in = 1};
		enum {out = 0};
		enum {owner = No_change};
		enum { is_by_value = 0 };
		enum { is_constant = 0 };
		enum { is_integral = 1 };
	};

	///////////////////////////////////////////////////////////////////////////////
	///  Specialisation for C style strings
	///////////////////////////////////////////////////////////////////////////////
	template<>
	struct in_p<char*>
	{
		typedef char* type;
		typedef char* raw;//we are an integral types but very different from the char type
		typedef char const* pull_type;//we pull as the correct type, as cast will occur
		enum { in = 1 };
		enum { out = 0 };
		enum { owner = No_change };
		enum { is_by_value = 0 };
		enum { is_constant = 0 };
		enum { is_integral = 1 };
	};

	template<>
	struct in_p<char*&>;//disabled

	template<>
	struct in_p<char const*>
	{
		typedef char const* type;
		typedef char* raw;//integral yet very different to char
		typedef char const* pull_type;
		enum { in = 1 };
		enum { out = 0 };
		enum { owner = No_change };
		enum { is_by_value = 0 };
		enum { is_constant = 1 };
		enum { is_integral = 1 };
	};

	template<>
	struct in_p<char const*&>
	{
		typedef char const*& type;
		typedef char* raw;//integral yet very different to char
		typedef char const* pull_type;
		enum { in = 1 };
		enum { out = 0 };
		enum { owner = No_change };
		enum { is_by_value = 0 };
		enum { is_constant = 1 };
		enum { is_integral = 1 };
	};

} // namespace OOLUA

#endif //OOLUA_TRAITS_H_
