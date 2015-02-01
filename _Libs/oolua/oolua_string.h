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

#ifndef OOLUA_STRING_H_
#	define OOLUA_STRING_H_

#include "lvd_type_traits.h"
#include "proxy_test.h"

struct lua_State;

namespace OOLUA
{
	/**
		\namespace OOLUA::STRING
		\brief Defines which type of string classes can be pulled and pushed from
		the stack with the public API and the DSL.
		\details
		I would really like to be able to forward declare string types in a cross
		platform way; for example when using GCC we could, but really shouldn't,
		use bits/stringfwd.h
	*/
	namespace STRING
	{
		template<typename StringType>
		bool push(lua_State* const vm, StringType const& value);

		template<typename StringType>
		bool pull(lua_State* const vm, StringType& value);

		template<typename StringType>
		void get(lua_State* const vm, int idx, StringType &  value);
	} // namespace STRING //NOLINT(readability/namespace)
} // namespace OOLUA


namespace OOLUA
{
	namespace STRING
	{

#define OOLUA_CLASS_OR_BASE_CONTAINS_METHOD(StructName, MethodSignature, MethodName) \
		template<typename Type> \
		struct StructName \
		{ \
			typedef char yes[1]; \
			typedef char no [2]; \
			template<typename FuncT, FuncT> struct checker; \
			template <typename U> \
			static yes &check(checker<MethodSignature, &U::MethodName>*); \
			template <typename U> \
			static no &check(...); \
			enum {value = sizeof(check<Type>(0)) == sizeof(yes)}; \
		};

/*
	std::string
*/
OOLUA_CLASS_OR_BASE_CONTAINS_METHOD(only_std_string_conforming_with_c_str_method
								, char const* (U::*)()const
								, c_str)

/*this will pass for all std:: string types (string, wstring, u16string and u32string)*/
/*
OOLUA_CLASS_OR_BASE_CONTAINS_METHOD(std_conforming_with_c_str_method
								, typename U::const_pointer (U::*)()const
								, c_str)
*/

/*this will pass for all std:: string types (string wstring u16string u32string) and wxString*/
/*
OOLUA_CLASS_OR_BASE_CONTAINS_METHOD(std_conforming_with_substr_method
								, U(U::*)(typename U::size_type,typename U::size_type)const
								, substr)
*/

/*
	wxString
	bool ToCULong (unsigned long *val, int base=10) const
*/
/*
OOLUA_CLASS_OR_BASE_CONTAINS_METHOD(is_wxstring
								, (bool(U::*)(unsigned long*,int)const)
								, ToCULong)
*/

/*
	QString
	QString toCaseFolded () const
*/
/*
OOLUA_CLASS_OR_BASE_CONTAINS_METHOD(is_qstring
								, U(U::*)()const
								, toCaseFolded);
*/

		/**
			\brief Preforms the check on the type without including the string header
			\details To add a different string class type, see the commented out macros
			in oolua_string.h.
		*/
		template<typename T>
		struct is_integral_string_class
		{
			typedef typename LVD::raw_type<T>::type raw_type;
			enum { value =
#if OOLUA_STD_STRING_IS_INTEGRAL == 1
						INTERNAL::has_a_proxy_type<raw_type>::value == 0
						&& only_std_string_conforming_with_c_str_method<raw_type>::value == 1
#else
						0
#endif
				};
		};
	} // namespace STRING //NOLINT(readability/namespace)
} // namespace OOLUA

#endif //OOLUA_STRING_H_
