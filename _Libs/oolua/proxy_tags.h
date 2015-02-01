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

///////////////////////////////////////////////////////////////////////////////
///  @file proxy_tags.h
///  Possible members for the Proxy_class \ref OOLUA_TAGS "Tag block"
///////////////////////////////////////////////////////////////////////////////
#ifndef OOLUA_PROXY_TAGS_H_
# define OOLUA_PROXY_TAGS_H_

/**  \addtogroup OOLuaTags Tags
@{
\brief
Possible members for \ref OOLUA_TAGS which help express more information about
a class which is to be proxied.

	\struct OOLUA::Abstract
	\brief The class being mirrored is an abstract class.
	\details
	When OOLua encounters the \ref OOLUA::Abstract "Abstract" tag it will not
	look for any constructors for the type and the type will not be constructable
	from Lua. Specifying an \ref OOLUA_CTORS block will have no effect and such a
	block will be ignored.

	\addtogroup OperatorTags Operator Tags
	@{
		\brief Informs that a class has an operator exposed
		\details \ref OperatorTags inform OOLua that a class exposes one or more of the
		operators supported:
			\li \ref OOLUA::Less_op "Less_op"
			\li \ref OOLUA::Equal_op "Equal_op"
			\li \ref OOLUA::Not_equal_op "Not_equal_op"
			\li \ref OOLUA::Less_equal_op "Less_equal_op"
			\li	\ref OOLUA::Div_op "Div_op"
			\li \ref OOLUA::Mul_op "Mul_op"
			\li \ref OOLUA::Sub_op "Sub_op"
			\li \ref OOLUA::Add_op "Add_op"



	\struct OOLUA::Less_op
	\brief Less than operator is defined for the type.

	\struct OOLUA::Equal_op
	\brief Equal operator is defined for the type.

	\struct OOLUA::Not_equal_op
	\brief Not equal operator is defined for the type.

	\struct OOLUA::Less_equal_op
	\brief Less than or equal operator is defined for the type.

	\struct OOLUA::Div_op
	\brief Division operator is defined for the type.

	\struct OOLUA::Mul_op
	\brief Multiplication operator is defined for the type.

	\struct OOLUA::Sub_op
	\brief Subtraction operator is defined for the type.

	\struct OOLUA::Add_op
	\brief Addition operator is defined for the type.
	@}

	\struct OOLUA::No_default_constructor
	\brief
	There is not a default constructor in the public interface yet there are other
	constructors.
	\details
	There is not a public default constructor or you do not wish to expose such a
	constructor, yet there are other constructors which will be specified by
	\ref OOLUA_CTOR entries inside a \ref OOLUA_CTOR block.

	\struct OOLUA::No_public_constructors
	\brief There are no constructors in the public interface.
	\details
	When OOLua encounters this tag it will not look for any constructors for the
	type and the type will not be constructable from Lua. Specifying an OOLUA_CTORS
	block will have no effect and such a block will be ignored.

	\struct OOLUA::No_public_destructor
	\brief
	There is not a destructor in the public interface and OOLua will not attempt to
	delete an instance of this type

	\struct OOLUA::Register_class_enums
	\brief The class has enums to register
	\details
	The class has enums which are specified inside the OOLUA_ENUMS block, these
	entries will be registered with a lua_State when the proxy type is.
*/
namespace OOLUA
{

	struct Abstract{};
	struct Less_op{};
	struct Equal_op{};
	struct Not_equal_op{};
	struct Less_equal_op{};
	struct Div_op{};
	struct Mul_op{};
	struct Sub_op{};
	struct Add_op{};
	struct No_default_constructor{};
	struct No_public_constructors{};
	struct No_public_destructor{};
	struct Register_class_enums{};
} // namespace OOLUA
/**@}*/
#endif //OOLUA_PROXY_TAGS_H_
