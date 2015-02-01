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

#ifndef OOLUA_PROXY_TEST_H_
#	define OOLUA_PROXY_TEST_H_
namespace OOLUA
{
	template<typename T>
	class Proxy_class;
	/** \cond INTERNAL */
	namespace INTERNAL
	{
		template<typename Type>
		struct has_a_proxy_type
		{
			typedef OOLUA::Proxy_class<Type> proxy_type;
			template <typename U>
			static char (& has_none_proxy_typedef(typename U::OoluaNoneProxy*))[1];

			template <typename U>
			static char (& has_none_proxy_typedef(...))[2];

			enum {value = sizeof(has_none_proxy_typedef<proxy_type>(0)) == 1 ? 0 : 1};
		};
	} // namespace INTERNAL //NOLINT(readability/namespace)
	/** \endcond*/
} // namespace OOLUA

#endif
