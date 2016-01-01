#ifndef GL_DEBUG_H
#define GL_DEBUG_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL_opengl.h>
#include <QMessageBox>
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
    if (error != GL_NO_ERROR) {
        std::ostringstream errMsg;
        errMsg << "OpenGL Error in " << func << " (at " << fn << ":" << line << ")\r\n";
        errMsg << "\r\n";
        errMsg << "Error code: "<< getGlErrorStr(error) << " (0x" << std::hex << (unsigned int)error << ")";
        QMessageBox::warning(nullptr, "OpenGL Error", errMsg.str().c_str());
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
static inline void _GLShowError(QString msg, const char* fn, int line, const char* func)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::ostringstream errMsg;
        errMsg << msg.toStdString();
        errMsg << "\nOpenGL Error in " << func << " (at " << fn << ":" << line << ")\r\n";
        errMsg << "\r\n";
        QMessageBox::warning(nullptr, "OpenGL Error", errMsg.str().c_str());
        abort();
    }
}

#endif // GL_DEBUG_H

