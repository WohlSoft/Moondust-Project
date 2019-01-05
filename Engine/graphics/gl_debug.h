/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef GL_DEBUG_H
#define GL_DEBUG_H

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_messagebox.h>
#include <sstream>

static const char*getGlErrorStr(GLenum error)
{
    switch(error)
    {
        case GL_NO_ERROR:
            return "";
        case GL_INVALID_ENUM:
            return "Invalid enumerator";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid value";
        case GL_STACK_OVERFLOW:
            return "Stack overflow";
        case GL_STACK_UNDERFLOW:
            return "Stack underflow";
        case GL_OUT_OF_MEMORY:
            return "Stack underflow";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "Invalid framebuffer operation";
        default:
            return "Unknown error";
    }
}

//! Checks for OpenGL error and spawns warning message with aborting application
#define GLERRORCHECK() _GLErrorCheck(__FILE__, __LINE__, __FUNCTION__)

//! Spawns warning message and aborts application
#define GLERROR(msg) _GLShowError(msg, __FILE__, __LINE__, __FUNCTION__)

/*!
 * \brief Checks for error of OpenGL and on error spawns warning message and sends abort signal
 * \param fn File name where this check was executed
 * \param line Line number of file where this check was executed
 * \param func Function name where this function was executed
 */
static inline void _GLErrorCheck(const char* fn, int line, const char* func)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::ostringstream errMsg;
        errMsg << "OpenGL Error in " << func << " (at " << fn << ":" << line << ")\r\n";
        errMsg << "\r\n";
        errMsg << "Error code: "<< getGlErrorStr(error) << " (0x" << std::hex << (unsigned int)error << ")";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
                                 "OpenGL Error",
                                 errMsg.str().c_str(),
                                 NULL);
        abort();
    }
}

/*!
 * \brief Shows error message and aborts application
 * \param Custom error message
 * \param fn File name where this check was executed
 * \param line Line number of file where this check was executed
 * \param func Function name where this function was executed
 */
static inline void _GLShowError(const std::string &msg, const char* fn, int line, const char* func)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        std::ostringstream errMsg;
        errMsg << msg;
        errMsg << "\nOpenGL Error in " << func << " (at " << fn << ":" << line << ")\r\n";
        errMsg << "\r\n";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
                                 "OpenGL Error",
                                 errMsg.str().c_str(),
                                 NULL);
        abort();
    }
}

#endif // GL_DEBUG_H

