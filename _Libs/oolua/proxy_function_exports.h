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
	\file proxy_function_exports.h
	\date Wed Oct 23 17:57:13 2013
	\details 
	Configurable values as set when generating this file
	\li class_functions 15 - Maximum amount of class functions that can be registered for each proxied type	(Default 15)
	\note Warning this file was generated, edits to the file will not persist if it is regenerated.
*/

#ifndef PROXY_FUNCTION_EXPORTS_H_
#	define PROXY_FUNCTION_EXPORTS_H_
/** \cond INTERNAL */

#define LUA_MEMBER_FUNC_1(Class, func1) {#func1, &Class::func1},
#define LUA_MEMBER_FUNC_2(Class, func1, func2) LUA_MEMBER_FUNC_1(Class, func1)  LUA_MEMBER_FUNC_1(Class, func2)
#define LUA_MEMBER_FUNC_3(Class, func1, func2, func3) LUA_MEMBER_FUNC_2(Class, func1, func2)  LUA_MEMBER_FUNC_1(Class, func3)
#define LUA_MEMBER_FUNC_4(Class, func1, func2, func3, func4) LUA_MEMBER_FUNC_3(Class, func1, func2, func3)  LUA_MEMBER_FUNC_1(Class, func4)
#define LUA_MEMBER_FUNC_5(Class, func1, func2, func3, func4, func5) LUA_MEMBER_FUNC_4(Class, func1, func2, func3, func4)  LUA_MEMBER_FUNC_1(Class, func5)
#define LUA_MEMBER_FUNC_6(Class, func1, func2, func3, func4, func5, func6) LUA_MEMBER_FUNC_5(Class, func1, func2, func3, func4, func5)  LUA_MEMBER_FUNC_1(Class, func6)
#define LUA_MEMBER_FUNC_7(Class, func1, func2, func3, func4, func5, func6, func7) LUA_MEMBER_FUNC_6(Class, func1, func2, func3, func4, func5, func6)  LUA_MEMBER_FUNC_1(Class, func7)
#define LUA_MEMBER_FUNC_8(Class, func1, func2, func3, func4, func5, func6, func7, func8) LUA_MEMBER_FUNC_7(Class, func1, func2, func3, func4, func5, func6, func7)  LUA_MEMBER_FUNC_1(Class, func8)
#define LUA_MEMBER_FUNC_9(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9) LUA_MEMBER_FUNC_8(Class, func1, func2, func3, func4, func5, func6, func7, func8)  LUA_MEMBER_FUNC_1(Class, func9)
#define LUA_MEMBER_FUNC_10(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10) LUA_MEMBER_FUNC_9(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9)  LUA_MEMBER_FUNC_1(Class, func10)
#define LUA_MEMBER_FUNC_11(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11) LUA_MEMBER_FUNC_10(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10)  LUA_MEMBER_FUNC_1(Class, func11)
#define LUA_MEMBER_FUNC_12(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12) LUA_MEMBER_FUNC_11(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11)  LUA_MEMBER_FUNC_1(Class, func12)
#define LUA_MEMBER_FUNC_13(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12, func13) LUA_MEMBER_FUNC_12(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12)  LUA_MEMBER_FUNC_1(Class, func13)
#define LUA_MEMBER_FUNC_14(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12, func13, func14) LUA_MEMBER_FUNC_13(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12, func13)  LUA_MEMBER_FUNC_1(Class, func14)
#define LUA_MEMBER_FUNC_15(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12, func13, func14, func15) LUA_MEMBER_FUNC_14(Class, func1, func2, func3, func4, func5, func6, func7, func8, func9, func10, func11, func12, func13, func14)  LUA_MEMBER_FUNC_1(Class, func15)

/// @def end the assigning of functions to the array
#define CLASS_LIST_MEMBERS_END {0, 0}};}

/// @def define the constants in the class, which are the the class name and the member function array
#define CLASS_LIST_MEMBERS_START_OOLUA_NON_CONST(Class)\
namespace OOLUA { \
char const OOLUA::Proxy_class< Class >::class_name[] = #Class;\
OOLUA::Proxy_class< Class >::Reg_type OOLUA::Proxy_class< Class >::class_methods[]={
// NOLINT

#define CLASS_LIST_MEMBERS_START_OOLUA_CONST(Class)\
namespace OOLUA { \
char const OOLUA::Proxy_class< Class >::class_name_const[] = #Class "_const";\
OOLUA::Proxy_class< Class >::Reg_type_const OOLUA::Proxy_class< Class >::class_methods_const[]={
// NOLINT

#define EXPORT_OOLUA_FUNCTIONS_0_(mod, Class)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_1_(mod, Class, p0)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_1(OOLUA::Proxy_class< Class > , p0)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_2_(mod, Class, p0, p1)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_2(OOLUA::Proxy_class< Class > , p0, p1)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_3_(mod, Class, p0, p1, p2)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_3(OOLUA::Proxy_class< Class > , p0, p1, p2)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_4_(mod, Class, p0, p1, p2, p3)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_4(OOLUA::Proxy_class< Class > , p0, p1, p2, p3)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_5_(mod, Class, p0, p1, p2, p3, p4)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_5(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_6_(mod, Class, p0, p1, p2, p3, p4, p5)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_6(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_7_(mod, Class, p0, p1, p2, p3, p4, p5, p6)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_7(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_8_(mod, Class, p0, p1, p2, p3, p4, p5, p6, p7)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_8(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6, p7)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_9_(mod, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_9(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6, p7, p8)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_10_(mod, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_10(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_11_(mod, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_11(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_12_(mod, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_12(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_13_(mod, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_13(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_14_(mod, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_14(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)\
	CLASS_LIST_MEMBERS_END
#define EXPORT_OOLUA_FUNCTIONS_15_(mod, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)\
	CLASS_LIST_MEMBERS_START_ ##mod(Class)\
	LUA_MEMBER_FUNC_15(OOLUA::Proxy_class< Class > , p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)\
	CLASS_LIST_MEMBERS_END

#define EXPORT_OOLUA_FUNCTIONS_0_CONST(Class) \
	EXPORT_OOLUA_FUNCTIONS_0_(OOLUA_CONST, Class)
#define EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Class) \
	EXPORT_OOLUA_FUNCTIONS_0_(OOLUA_NON_CONST, Class)
#define EXPORT_OOLUA_FUNCTIONS_1_CONST(Class, p0) \
	EXPORT_OOLUA_FUNCTIONS_1_(OOLUA_CONST, Class, p0)
#define EXPORT_OOLUA_FUNCTIONS_1_NON_CONST(Class, p0) \
	EXPORT_OOLUA_FUNCTIONS_1_(OOLUA_NON_CONST, Class, p0)
#define EXPORT_OOLUA_FUNCTIONS_2_CONST(Class, p0, p1) \
	EXPORT_OOLUA_FUNCTIONS_2_(OOLUA_CONST, Class, p0, p1)
#define EXPORT_OOLUA_FUNCTIONS_2_NON_CONST(Class, p0, p1) \
	EXPORT_OOLUA_FUNCTIONS_2_(OOLUA_NON_CONST, Class, p0, p1)
#define EXPORT_OOLUA_FUNCTIONS_3_CONST(Class, p0, p1, p2) \
	EXPORT_OOLUA_FUNCTIONS_3_(OOLUA_CONST, Class, p0, p1, p2)
#define EXPORT_OOLUA_FUNCTIONS_3_NON_CONST(Class, p0, p1, p2) \
	EXPORT_OOLUA_FUNCTIONS_3_(OOLUA_NON_CONST, Class, p0, p1, p2)
#define EXPORT_OOLUA_FUNCTIONS_4_CONST(Class, p0, p1, p2, p3) \
	EXPORT_OOLUA_FUNCTIONS_4_(OOLUA_CONST, Class, p0, p1, p2, p3)
#define EXPORT_OOLUA_FUNCTIONS_4_NON_CONST(Class, p0, p1, p2, p3) \
	EXPORT_OOLUA_FUNCTIONS_4_(OOLUA_NON_CONST, Class, p0, p1, p2, p3)
#define EXPORT_OOLUA_FUNCTIONS_5_CONST(Class, p0, p1, p2, p3, p4) \
	EXPORT_OOLUA_FUNCTIONS_5_(OOLUA_CONST, Class, p0, p1, p2, p3, p4)
#define EXPORT_OOLUA_FUNCTIONS_5_NON_CONST(Class, p0, p1, p2, p3, p4) \
	EXPORT_OOLUA_FUNCTIONS_5_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4)
#define EXPORT_OOLUA_FUNCTIONS_6_CONST(Class, p0, p1, p2, p3, p4, p5) \
	EXPORT_OOLUA_FUNCTIONS_6_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5)
#define EXPORT_OOLUA_FUNCTIONS_6_NON_CONST(Class, p0, p1, p2, p3, p4, p5) \
	EXPORT_OOLUA_FUNCTIONS_6_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5)
#define EXPORT_OOLUA_FUNCTIONS_7_CONST(Class, p0, p1, p2, p3, p4, p5, p6) \
	EXPORT_OOLUA_FUNCTIONS_7_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6)
#define EXPORT_OOLUA_FUNCTIONS_7_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6) \
	EXPORT_OOLUA_FUNCTIONS_7_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6)
#define EXPORT_OOLUA_FUNCTIONS_8_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7) \
	EXPORT_OOLUA_FUNCTIONS_8_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7)
#define EXPORT_OOLUA_FUNCTIONS_8_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7) \
	EXPORT_OOLUA_FUNCTIONS_8_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7)
#define EXPORT_OOLUA_FUNCTIONS_9_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8) \
	EXPORT_OOLUA_FUNCTIONS_9_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8)
#define EXPORT_OOLUA_FUNCTIONS_9_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8) \
	EXPORT_OOLUA_FUNCTIONS_9_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8)
#define EXPORT_OOLUA_FUNCTIONS_10_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) \
	EXPORT_OOLUA_FUNCTIONS_10_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define EXPORT_OOLUA_FUNCTIONS_10_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9) \
	EXPORT_OOLUA_FUNCTIONS_10_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9)
#define EXPORT_OOLUA_FUNCTIONS_11_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) \
	EXPORT_OOLUA_FUNCTIONS_11_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)
#define EXPORT_OOLUA_FUNCTIONS_11_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10) \
	EXPORT_OOLUA_FUNCTIONS_11_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)
#define EXPORT_OOLUA_FUNCTIONS_12_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) \
	EXPORT_OOLUA_FUNCTIONS_12_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)
#define EXPORT_OOLUA_FUNCTIONS_12_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11) \
	EXPORT_OOLUA_FUNCTIONS_12_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)
#define EXPORT_OOLUA_FUNCTIONS_13_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) \
	EXPORT_OOLUA_FUNCTIONS_13_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)
#define EXPORT_OOLUA_FUNCTIONS_13_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12) \
	EXPORT_OOLUA_FUNCTIONS_13_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)
#define EXPORT_OOLUA_FUNCTIONS_14_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) \
	EXPORT_OOLUA_FUNCTIONS_14_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)
#define EXPORT_OOLUA_FUNCTIONS_14_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13) \
	EXPORT_OOLUA_FUNCTIONS_14_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)
#define EXPORT_OOLUA_FUNCTIONS_15_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) \
	EXPORT_OOLUA_FUNCTIONS_15_(OOLUA_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)
#define EXPORT_OOLUA_FUNCTIONS_15_NON_CONST(Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14) \
	EXPORT_OOLUA_FUNCTIONS_15_(OOLUA_NON_CONST, Class, p0, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)
/** \endcond */

/** \addtogroup OOLuaExporting
@{
	\def OOLUA_EXPORT_NO_FUNCTIONS
	\hideinitializer
	\brief Inform that there are no functions of interest
	\param Class
*/
#define OOLUA_EXPORT_NO_FUNCTIONS(Class)\
	EXPORT_OOLUA_FUNCTIONS_0_NON_CONST(Class)\
	EXPORT_OOLUA_FUNCTIONS_0_CONST(Class)

/**@}*/

#endif
