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
	\file oolua_version.h
	\brief OOLua library version information for both the CPP and at run time
*/

#ifndef OOLUA_VERSION_H_
#	define OOLUA_VERSION_H_

#	define OOLUA_DO_STRINGISE(a) #a
#	define OOLUA_STRINGISE(a) OOLUA_DO_STRINGISE(a)

/**
	\def OOLUA_VERSION_MAJ
	\brief CPP major version number
*/
#	define OOLUA_VERSION_MAJ 2

/**
	\def OOLUA_VERSION_MIN
 	\brief CPP minor version number
 */
#	define OOLUA_VERSION_MIN 0

/**
	\def OOLUA_VERSION_PATCH
	\brief CPP patch version number
*/
#	define OOLUA_VERSION_PATCH 0

/**
	\def OOLUA_VERSION
	\showinitializer
	\brief CPP string detailing the library version
*/
#	define OOLUA_VERSION \
					OOLUA_STRINGISE(OOLUA_VERSION_MAJ) "." \
					OOLUA_STRINGISE(OOLUA_VERSION_MIN) "." \
					OOLUA_STRINGISE(OOLUA_VERSION_PATCH) " Beta 3"


namespace OOLUA
{
	/**
		\var version_str
		\showinitializer
		\brief OOLua version string
	*/
	static const char version_str[] = OOLUA_VERSION;
	/**
		\var version_number
		\brief OOLua version int
	*/
	static const int version_number = OOLUA_VERSION_MAJ*10000+OOLUA_VERSION_MIN*1000+OOLUA_VERSION_PATCH;
} // namespace OOLUA

#	undef OOLUA_DO_STRINGISE
#	undef OOLUA_STRINGISE

#endif
