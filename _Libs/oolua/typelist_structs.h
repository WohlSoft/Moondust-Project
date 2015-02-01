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

#ifndef TYPELIST_STRUCTS_H_
#	define TYPELIST_STRUCTS_H_

///////////////////////////////////////////////////////////////////////////////
///  @file typelist_structs.h
///  @remarks This file was auto generated
///////////////////////////////////////////////////////////////////////////////

/** \cond INTERNAL*/

namespace TYPE
{
	struct Null_type{};
} // namespace TYPE

/// the compile time list, where a standard list with raw pointers uses NULL for no type
/// this list has Null_type, which is an empty struct
///a node in the lists
///like a classic list node it contains a value for this node and the next
template <typename T, typename N>
struct Type_node
{
	///the type of this entry
	typedef T This;
	///the type for the next entry which is normally a Type_node, or a Null_type
	typedef N Next;
};

template
<
typename T01 = TYPE::Null_type, typename T02 = TYPE::Null_type, typename T03 = TYPE::Null_type, typename T04 = TYPE::Null_type, typename T05 = TYPE::Null_type,
typename T06 = TYPE::Null_type, typename T07 = TYPE::Null_type, typename T08 = TYPE::Null_type, typename T09 = TYPE::Null_type, typename T10 = TYPE::Null_type,
typename T11 = TYPE::Null_type, typename T12 = TYPE::Null_type, typename T13 = TYPE::Null_type, typename T14 = TYPE::Null_type, typename T15 = TYPE::Null_type,
typename T16 = TYPE::Null_type, typename T17 = TYPE::Null_type, typename T18 = TYPE::Null_type, typename T19 = TYPE::Null_type, typename T20 = TYPE::Null_type,
typename T21 = TYPE::Null_type, typename T22 = TYPE::Null_type, typename T23 = TYPE::Null_type, typename T24 = TYPE::Null_type, typename T25 = TYPE::Null_type,
typename T26 = TYPE::Null_type, typename T27 = TYPE::Null_type, typename T28 = TYPE::Null_type, typename T29 = TYPE::Null_type, typename T30 = TYPE::Null_type,
typename T31 = TYPE::Null_type, typename T32 = TYPE::Null_type, typename T33 = TYPE::Null_type, typename T34 = TYPE::Null_type, typename T35 = TYPE::Null_type,
typename T36 = TYPE::Null_type, typename T37 = TYPE::Null_type, typename T38 = TYPE::Null_type, typename T39 = TYPE::Null_type, typename T40 = TYPE::Null_type,
typename T41 = TYPE::Null_type, typename T42 = TYPE::Null_type, typename T43 = TYPE::Null_type, typename T44 = TYPE::Null_type, typename T45 = TYPE::Null_type,
typename T46 = TYPE::Null_type, typename T47 = TYPE::Null_type, typename T48 = TYPE::Null_type, typename T49 = TYPE::Null_type, typename T50 = TYPE::Null_type
>
struct Type_list
{
private:
	typedef typename Type_list
		<
			 T02, T03, T04, T05, T06, T07, T08, T09, T10,
		T11, T12, T13, T14, T15, T16, T17, T18, T19, T20,
		T21, T22, T23, T24, T25, T26, T27, T28, T29, T30,
		T31, T32, T33, T34, T35, T36, T37, T38, T39, T40,
		T41, T42, T43, T44, T45, T46, T47, T48, T49, T50
		>
		::type TailResult;

public:
	typedef Type_node<T01, TailResult> type;
};

template<>
struct Type_list<>
{
	typedef TYPE::Null_type type;
};

/**\endcond*/

#endif//TYPELIST_STRUCTS_H_
