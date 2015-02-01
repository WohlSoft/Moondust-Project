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
///  @file platform_check.h
///  Preforms a check of platform defines and defines a macro
///  @remarks
///  Information available via http://predef.sourceforge.net/preos.html
///////////////////////////////////////////////////////////////////////////////

#ifndef PLATFORM_CHECK_H_
#	define PLATFORM_CHECK_H_

#	ifndef PLATFORM_CHECKED
#		define PLATFORM_CHECKED
#		if (defined(__CYGWIN__))
//#			error Cygwin is currently not supported
#			define UNIX_BUILD			1
#		else
			/// windows
#			if (defined(__WIN32__) || defined(_WIN32) || defined(WIN32))
#				define WINDOWS_BUILD	1
			/// os2
#			elif(defined(__OS2__) || defined(_OS2)  || defined(OS2) || defined(Macintosh) || \
				defined(macintosh) || defined(__MACOSX__) || defined(__APPLE__))
#				define MAC_BUILD		1
			/// nix
#			elif(defined(unix) || defined(_unix) || defined(__unix) || defined(__unix__) || \
				defined(linux) || defined(__linux))
#				define UNIX_BUILD		1

#			endif///! CYGWIN
#		endif///CYGWIN

#	endif///PLATFORM_CHECKED

#endif ///PLATFORM_CHECK_H_
