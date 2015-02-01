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
	\file oolua_chunk.h
	\brief Provides methods for running and loading chunks.
*/
#ifndef OOLUA_CHUNK_H_
#	define OOLUA_CHUNK_H_

#	include <string>

struct lua_State;

namespace OOLUA
{
	/** \brief Loads a chunk leaving the resulting function on the stack
		\param[in] vm \copydoc lua_State
		\param[in] chunk
	*/
	bool load_chunk(lua_State* vm, std::string const& chunk);

	/** \brief Loads and runs a chunk of code
		\param[in] vm \copydoc lua_State
		\param[in] chunk
	*/
	bool run_chunk(lua_State* vm, std::string const& chunk);

	/** \brief Loads a file leaving the resulting function on the stack
		\param[in] vm \copydoc lua_State
		\param[in] filename
	 */
	bool load_file(lua_State* vm, std::string const & filename);

	/** \brief Loads and runs the file
		\param[in] vm \copydoc lua_State
		\param[in] filename
	*/
	bool run_file(lua_State* vm, std::string const & filename);

} // namespace OOLUA

#endif
