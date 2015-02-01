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

#ifndef OOLUA_PROXY_TAG_INFO_H_
#	define OOLUA_PROXY_TAG_INFO_H_

#	include "proxy_class.h"
#	include "type_list.h"
#	include "lvd_type_traits.h"

namespace OOLUA
{
	namespace INTERNAL
	{
		/** \cond INTERNAL */
		/**
			\addtogroup OOLuaDeveloperDocs
		@{*/

			/**  \addtogroup OOLuaTagChecking Tag Checking
			@{
				\brief Provides a way to check for a \ref OOLuaTags "tag" in a specific Proxy_class
				\details Allows for the checking of a tag in a Proxy_class. There is no requirement
				for a Proxy_class to contain a tag block; it therefore first determines if the class
				, and not one of it's bases, contains such a block and then check for the presence
				of the tag.
			*/

		/**
			\brief Checks if a type of a base of the type has a tag block
			\details If a Proxy_class does not contain a tag block yet one of it's bases does,
			then this is the one which will be identified. We use the result from this template
			in a further test to determine if it is a base tag or the correct type tag.
			\see tag_block_is_same
		*/
		template<typename ClassType>
		struct class_or_base_has_tag_block
		{
			template <typename U>
			static char (&check_for_tag_block(typename OOLUA::Proxy_class<U>::tag_block_check*))[1];//NOLINT
			template <typename U>
			static char (&check_for_tag_block(...))[2];//NOLINT
			enum {value = sizeof(check_for_tag_block<ClassType >(0)) == 1 ? 1 : 0};
		};

		/**
			\brief Checks a potential tag block against the requested type
			\details As a tag block is a typedef for the class type, we can use the result of
			class_or_base_has_tag_block to check if the type is the same as the wanted type
			\tparam ClassType The class type which must be the same of the tag block if it is
				contained in Proxy_class<ClassType>
			\tparam TagBlockMaybeInClass_or_MaybeInABase specialisation which is the result of
			class_or_base_has_tag_block, which when is zero uses SFINAE to store a result
				which indicates it has no tag block.
		*/
		template< typename ClassType, int TagBlockMaybeInClass_or_MaybeInABase>
		struct tag_block_is_same
		{
			enum {value = LVD::is_same< typename Proxy_class<ClassType>::tag_block_check
					, ClassType >::value };
		};
		template< typename ClassType>
		struct tag_block_is_same<ClassType, 0>
		{
			enum {value = 0};
		};

		/**
			\brief Either typedefs the the tags typelist or an empty typelist which can
			be checked for the presence of a type, without producing a compile time failure.
			\tparam ClassType the Proxy_class<ClassType> which information is wanted from
			\tparam has_proxy_tags Allows specialisation and SFINAE
		*/
		template<typename ClassType, int has_proxy_tags>
		struct proxy_tag_type
		{
			typedef typename Proxy_class<ClassType>::Tags type;
		};

		template<typename ClassType>
		struct proxy_tag_type<ClassType, 0>
		{
			typedef TYPE::Null_type type;
		};

		/**
			\brief Uses the other templates in \ref OOLuaTagChecking to typedef a typelist
			which can then be checked safely.
			\tparam ClassType The Proxy_class<ClassType> from which tag information is wanted
		*/
		template<typename ClassType>
		struct tag_type
		{
			enum { has_proxy_tags = tag_block_is_same<ClassType, class_or_base_has_tag_block<ClassType>::value >::value };
			typedef typename proxy_tag_type<ClassType, has_proxy_tags>::type type;
		};

		/**
			\brief A compile time safe method of determining if a Proxy_class tag list contains a tag
			\tparam ProxyType The proxy_class to check
			\tparam Tag The specific tag which will be checked for in ProxyType
		*/
		template<typename ProxyType, typename Tag>
		struct has_tag
		{
			enum {Result = TYPELIST::IndexOf<typename tag_type<typename ProxyType::class_>::type, Tag>::value == -1 ? 0 : 1};
		};

			/**@}*/
		/**@}*/
		/** \endcond */
	} // namespace INTERNAL // NOLINT
} // namespace OOLUA

#endif //OOLUA_PROXY_TAG_INFO_H_
