/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
#define GLEW_STATIC
#define GLEW_NO_GLU
#include <GL/glew.h>
*/

#include "gl_renderer.h"
#include "window.h"
#include "../common_features/app_path.h"

#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <common_features/gif-h/gif.h>
#include <gui/pge_msgbox.h>

#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_thread.h>

#include "gl_debug.h"

#ifdef _WIN32
#define FREEIMAGE_LIB
#endif
#include <FreeImageLite.h>

#include <audio/pge_audio.h>

#include "render/render_opengl21.h"
#include "render/render_opengl31.h"
#include "render/render_swsdl.h"

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <fmt/fmt_format.h>

#include <chrono>

#ifdef DEBUG_BUILD
#include <Utils/elapsed_timer.h>
#endif

static Render_dummy     g_dummy;//Empty renderer
static Render_OpenGL31  g_opengl31;
static Render_OpenGL21  g_opengl21;
static Render_SW_SDL    g_swsdl;

static Render_Base      *g_renderer = &g_dummy;


bool GlRenderer::m_isReady = false;
SDL_Thread *GlRenderer::m_screenshot_thread = NULL;

int     GlRenderer::m_viewport_w    = 800;
int     GlRenderer::m_viewport_h    = 600;
float   GlRenderer::m_offset_x  = 0.0f;
float   GlRenderer::m_offset_y  = 0.0f;

static bool isGL_Error()
{
    return glGetError() != GL_NO_ERROR;
}

static bool isGlExtensionSupported(const char *ext, const unsigned char *exts)
{
    return (strstr(reinterpret_cast<const char *>(exts), ext) != NULL);
}

#ifndef __ANDROID__
static bool detectOpenGL2()
{
    const char *errorPlace = "";
    SDL_GLContext glcontext = NULL;
    SDL_Window *dummy = NULL;
    GLubyte *sExtensions = NULL;
    GLuint test_texture = 0;
    unsigned char dummy_texture[] =
    {
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0
    };
    SDL_ClearError();
    SDL_GL_ResetAttributes();
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    #ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);//FOR GL 2.1
    #endif
    pLogDebug("GL2PROBE: Create hidden window");
    dummy = SDL_CreateWindow("OpenGL 2 probe dummy window",
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             10, 10, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

    if(!dummy)
    {
        errorPlace = "on window creation";
        goto sdl_error;
    }

    pLogDebug("GL2PROBE: Create context");
    glcontext = SDL_GL_CreateContext(dummy);

    if(!glcontext)
    {
        errorPlace = "on context creating";
        goto sdl_error;
    }

    if(PGE_Window::isSdlError())
    {
        errorPlace = "after context creating";
        goto sdl_error;
    }

    pLogDebug("GL2PROBE: take extensions list");
    sExtensions = const_cast<GLubyte *>(glGetString(GL_EXTENSIONS));

    if(!sExtensions)
    {
        errorPlace = "on extensions list taking";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check GL_EXT_bgra");

    if(!isGlExtensionSupported("GL_EXT_bgra", sExtensions))
    {
        errorPlace = "(GL_EXT_bgra is missing)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glEnable(GL_BLEND)");
    glEnable(GL_BLEND);

    if(isGL_Error())
    {
        errorPlace = "on glEnable(GL_BLEND)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glEnable(GL_TEXTURE_2D)");
    glEnable(GL_TEXTURE_2D);

    if(isGL_Error())
    {
        errorPlace = "on glEnable(GL_TEXTURE_2D)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glGenTextures");
    glGenTextures(1, &test_texture);

    if(isGL_Error())
    {
        errorPlace = "on glGenTextures( 1, &test_texture )";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glBindTexture");
    glBindTexture(GL_TEXTURE_2D, test_texture);

    if(isGL_Error())
    {
        errorPlace = "on glBindTexture( GL_TEXTURE_2D, test_texture  (#1))";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glTexImage2D");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, dummy_texture);

    if(isGL_Error())
    {
        errorPlace = "on glTexImage2D(GL_TEXTURE_2D, ..... )";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glBindTexture");
    glBindTexture(GL_TEXTURE_2D, 0);

    if(isGL_Error())
    {
        errorPlace = "on glBindTexture( GL_TEXTURE_2D, 0 )";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glBindTexture");
    glBindTexture(GL_TEXTURE_2D, test_texture);

    if(isGL_Error())
    {
        errorPlace = "on glBindTexture( GL_TEXTURE_2D, test_texture ) (#2)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    if(isGL_Error())
    {
        errorPlace = "on glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(isGL_Error())
    {
        errorPlace = "on glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(isGL_Error())
    {
        errorPlace = "on glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if(isGL_Error())
    {
        errorPlace = "on glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glBlendFunc");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(isGL_Error())
    {
        errorPlace = "on glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glColor4f");
    glColor4f(0.5f, 0.5f, 0.5f, 0.5f);

    if(isGL_Error())
    {
        errorPlace = "on glColor4f(0.5f, 0.5f, 0.5f, 0.5f)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glBegin,glEnd");
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(0.0f, 1.0f);
    glEnd();

    if(isGL_Error())
    {
        errorPlace = "on glBegin,glEnd";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glBindTexture");
    glBindTexture(GL_TEXTURE_2D, 0);

    if(isGL_Error())
    {
        errorPlace = "on glBindTexture( GL_TEXTURE_2D, 0 ) (#2)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glDisable(GL_TEXTURE_2D)");
    glDisable(GL_TEXTURE_2D);

    if(isGL_Error())
    {
        errorPlace = "on glDisable(GL_TEXTURE_2D)";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: check glDeleteTextures");
    glDeleteTextures(1, &test_texture);

    if(isGL_Error())
    {
        errorPlace = "on glDeleteTextures( 1, &test_texture )";
        goto gl_error;
    }

    pLogDebug("GL2PROBE: All tests passed");
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(dummy);
    SDL_ClearError();
    SDL_GL_ResetAttributes();
    return true;
gl_error:
    pLogDebug("GL Error of the OpenGL 2 probe %s: %s", errorPlace, getGlErrorStr(glGetError()));

    if(glcontext)
        SDL_GL_DeleteContext(glcontext);

    if(dummy)
        SDL_DestroyWindow(dummy);

    SDL_ClearError();
    SDL_GL_ResetAttributes();
    return false;
sdl_error:
    pLogDebug("SDL Error of OpenGL 2 probe %s: %s", errorPlace, SDL_GetError());

    if(glcontext)
        SDL_GL_DeleteContext(glcontext);

    if(dummy)
        SDL_DestroyWindow(dummy);

    SDL_ClearError();
    SDL_GL_ResetAttributes();
    return false;
}
#endif

#ifndef __APPLE__
static bool detectOpenGL3()
{
    const char *errorPlace = "";
    SDL_GLContext glcontext = NULL;
    SDL_Window *dummy = NULL;
    GLubyte *sExtensions = NULL;
    GLuint test_texture = 0;
    unsigned char dummy_texture[] =
    {
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0,
        0, 0, 0, 0,  0, 0, 0, 0,   0, 0, 0, 0,  0, 0, 0, 0
    };
    GLdouble Vertices[] =
    {
        0.0, 0.0, 0.0,
        1.0, 0.0, 0.0,
        1.0, 1.0, 0.0,
        0.0, 1.0, 0.0
    };
    GLfloat TexCoord[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    GLfloat Colors[] = { 0.5, 0.5, 0.5, 0.5,
                         0.5, 0.5, 0.5, 0.5,
                         0.5, 0.5, 0.5, 0.5,
                         0.5, 0.5, 0.5, 0.5
                       };
    GLubyte indices[] =
    {
        0, 1, 3, 2
    };

    SDL_ClearError();
    SDL_GL_ResetAttributes();
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);//3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);//1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);  //for GL 3.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    pLogDebug("GL3PROBE: Create hidden window");
    dummy = SDL_CreateWindow("OpenGL 3 probe dummy window",
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             10, 10, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);

    if(!dummy)
    {
        errorPlace = "on window creation";
        goto sdl_error;
    }

    pLogDebug("GL3PROBE: Create context");
    glcontext = SDL_GL_CreateContext(dummy);

    if(!glcontext)
    {
        errorPlace = "on context creating";
        goto sdl_error;
    }

    if(PGE_Window::isSdlError())
    {
        errorPlace = "after context creating";
        goto sdl_error;
    }

    pLogDebug("GL3PROBE: take extensions list");
    sExtensions = const_cast<GLubyte *>(glGetString(GL_EXTENSIONS));

    if(!sExtensions)
    {
        errorPlace = "on extensions list taking";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check GL_EXT_bgra");

    if(!isGlExtensionSupported("GL_EXT_bgra", sExtensions))
    {
        errorPlace = "(GL_EXT_bgra is missing)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check GL_ARB_texture_non_power_of_two");

    if(!isGlExtensionSupported("GL_ARB_texture_non_power_of_two", sExtensions))
    {
        errorPlace = "(GL_ARB_texture_non_power_of_two is missing)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glEnable(GL_BLEND)");
    glEnable(GL_BLEND);

    if(isGL_Error())
    {
        errorPlace = "on glEnable(GL_BLEND)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glEnable(GL_TEXTURE_2D)");
    glEnable(GL_TEXTURE_2D);

    if(isGL_Error())
    {
        errorPlace = "on glEnable(GL_TEXTURE_2D)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glEnableClientState");
    //Deep test of OpenGL functions
    glEnableClientState(GL_VERTEX_ARRAY);

    if(isGL_Error())
    {
        errorPlace = "on glEnableClientState(GL_VERTEX_ARRAY)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glEnableClientState");
    glEnableClientState(GL_COLOR_ARRAY);

    if(isGL_Error())
    {
        errorPlace = "on glEnableClientState(GL_COLOR_ARRAY)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glEnableClientState");
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if(isGL_Error())
    {
        errorPlace = "on glEnableClientState(GL_TEXTURE_COORD_ARRAY)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glGenTextures");
    glGenTextures(1, &test_texture);

    if(isGL_Error())
    {
        errorPlace = "on glGenTextures( 1, &test_texture )";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glBindTexture");
    glBindTexture(GL_TEXTURE_2D, test_texture);

    if(isGL_Error())
    {
        errorPlace = "on glBindTexture( GL_TEXTURE_2D, test_texture  (#1))";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glTexImage2D");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, dummy_texture);

    if(isGL_Error())
    {
        errorPlace = "on glTexImage2D(GL_TEXTURE_2D, ..... )";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glBindTexture");
    glBindTexture(GL_TEXTURE_2D, 0);

    if(isGL_Error())
    {
        errorPlace = "on glBindTexture( GL_TEXTURE_2D, 0 )";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glBindTexture");
    glBindTexture(GL_TEXTURE_2D, test_texture);

    if(isGL_Error())
    {
        errorPlace = "on glBindTexture( GL_TEXTURE_2D, test_texture ) (#2)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    if(isGL_Error())
    {
        errorPlace = "on glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(isGL_Error())
    {
        errorPlace = "on glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(isGL_Error())
    {
        errorPlace = "on glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glTexParameteri");
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    if(isGL_Error())
    {
        errorPlace = "on glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glBlendFunc");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(isGL_Error())
    {
        errorPlace = "on glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glColorPointer");
    glColorPointer(4, GL_FLOAT, 0, Colors);

    if(isGL_Error())
    {
        errorPlace = "on glColorPointer(4, GL_FLOAT, 0, Colors)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glVertexPointer");
    glVertexPointer(3, GL_DOUBLE, 0, Vertices);

    if(isGL_Error())
    {
        errorPlace = "on glVertexPointer(3, GL_DOUBLE, 0, Vertices)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glTexCoordPointer");
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);

    if(isGL_Error())
    {
        errorPlace = "on glTexCoordPointer(2, GL_FLOAT, 0, TexCoord)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glDrawElements");
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);

    if(isGL_Error())
    {
        errorPlace = "on glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glBindTexture");
    glBindTexture(GL_TEXTURE_2D, 0);

    if(isGL_Error())
    {
        errorPlace = "on glBindTexture( GL_TEXTURE_2D, 0 ) (#2)";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: check glDeleteTextures");
    glDeleteTextures(1, &test_texture);

    if(isGL_Error())
    {
        errorPlace = "on glDeleteTextures( 1, &test_texture )";
        goto gl_error;
    }

    pLogDebug("GL3PROBE: All tests passed");
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(dummy);
    SDL_ClearError();
    SDL_GL_ResetAttributes();
    return true;
gl_error:
    pLogDebug("GL Error of the OpenGL 3 probe %s: %s", errorPlace, getGlErrorStr(glGetError()));

    if(glcontext)
        SDL_GL_DeleteContext(glcontext);

    if(dummy)
        SDL_DestroyWindow(dummy);

    SDL_ClearError();
    SDL_GL_ResetAttributes();
    return false;
sdl_error:
    pLogDebug("SDL Error of OpenGL 3 probe %s: %s", errorPlace, SDL_GetError());

    if(glcontext)
        SDL_GL_DeleteContext(glcontext);

    if(dummy)
        SDL_DestroyWindow(dummy);

    SDL_ClearError();
    SDL_GL_ResetAttributes();
    return false;
}
#endif


GlRenderer::RenderEngineType GlRenderer::setRenderer(GlRenderer::RenderEngineType rtype)
{
    if(rtype == RENDER_AUTO)
    {
        #ifndef __APPLE__
        if(detectOpenGL3())
        {
            rtype = RENDER_OPENGL_3_1;
            pLogDebug("OpenGL 3.1 detected!");
        }
        else
        #endif
        #ifndef __ANDROID__
            if(detectOpenGL2())
            {
                rtype = RENDER_OPENGL_2_1;
                pLogDebug("OpenGL 2.1 detected!");
            }//RENDER_SDL2
            else
        #endif
            {
                rtype = RENDER_INVALID;
                pLogCritical("OpenGL not detected!");
            }
    }

    #ifndef __APPLE__
    else if(rtype == RENDER_OPENGL_3_1)
    {
        if(detectOpenGL3())
            pLogDebug("OpenGL 3.1 selected and detected!");
        else
        {
            rtype = RENDER_INVALID;
            pLogWarning("OpenGL 3.1 selected, but proble failed!");
        }
    }
    #endif

    #ifndef __ANDROID__
    else if(rtype == RENDER_OPENGL_2_1)
    {
        if(detectOpenGL2())
            pLogDebug("OpenGL 2.1 selected and detected!");
        else
        {
            rtype = RENDER_INVALID;
            pLogWarning("OpenGL 2.1 selected, but proble failed!");
        }
    }
    #endif
    else if(rtype == RENDER_SW_SDL)
        pLogDebug("SDL Software renderer selected!");

    return rtype;
}

void GlRenderer::setup_OpenGL31()
{
    g_renderer = &g_opengl31;
    g_renderer->set_SDL_settings();
}

unsigned int GlRenderer::SDL_InitFlags()
{
    return g_renderer->SDL_InitFlags();
}

void GlRenderer::setup_OpenGL21()
{
    g_renderer = &g_opengl21;
    g_renderer->set_SDL_settings();
}

void GlRenderer::setup_SW_SDL()
{
    g_renderer = &g_swsdl;
    g_renderer->set_SDL_settings();
}

std::string GlRenderer::engineName()
{
    return g_renderer->name();
}

bool GlRenderer::init()
{
    if(!PGE_Window::isReady())
        return false;

    ScreenshotPath = AppPathManager::screenshotsDir() + "/";
    m_isReady = g_renderer->init();

    if(m_isReady)
    {
        g_renderer->resetViewport();
        g_renderer->initDummyTexture();
    }

    return m_isReady;
}

bool GlRenderer::uninit()
{
    return g_renderer->uninit();
}

PGE_Texture GlRenderer::loadTexture(std::string path, std::string maskPath)
{
    PGE_Texture target;
    loadTextureP(target, path, maskPath);
    return target;
}

void GlRenderer::loadTextureP(PGE_Texture& target, std::string path, std::string maskPath)
{
    //SDL_Surface * sourceImage;
    FIBITMAP *sourceImage;

    if(path.empty())
        return;

    // Load the OpenGL texture
    //sourceImage = GraphicsHelps::loadQImage(path); // Gives us the information to make the texture
    if(path[0] == ':')
    {
        path.erase(0, 1);
        sourceImage = GraphicsHelps::loadImageRC(path.c_str());
    }
    else
        sourceImage = GraphicsHelps::loadImage(path);

    //Don't load mask if PNG image is used
    if(Files::hasSuffix(path, ".png"))
        maskPath.clear();

    if(!sourceImage)
    {
        pLogWarning("Error loading of image file:\n"
                    "%s\n"
                    "Reason: %s.",
                    path.c_str(),
                    (Files::fileExists(path) ? "wrong image format" : "file not exist"));
        target = g_renderer->getDummyTexture();
        return;
    }

    #ifdef DEBUG_BUILD
    ElapsedTimer totalTime;
    ElapsedTimer maskMergingTime;
    ElapsedTimer bindingTime;
    ElapsedTimer unloadTime;
    totalTime.start();
    int64_t maskElapsed = 0;
    int64_t bindElapsed = 0;
    int64_t unloadElapsed = 0;
    #endif

    //Apply Alpha mask
    if(!maskPath.empty() && Files::fileExists(maskPath))
    {
        #ifdef DEBUG_BUILD
        maskMergingTime.start();
        #endif
        GraphicsHelps::mergeWithMask(sourceImage, maskPath);
        #ifdef DEBUG_BUILD
        maskElapsed = maskMergingTime.nanoelapsed();
        #endif
    }

    uint32_t w = static_cast<uint32_t>(FreeImage_GetWidth(sourceImage));
    uint32_t h = static_cast<uint32_t>(FreeImage_GetHeight(sourceImage));

    if((w == 0) || (h == 0))
    {
        FreeImage_Unload(sourceImage);
        pLogWarning("Error loading of image file:\n"
                    "%s\n"
                    "Reason: %s.",
                    path.c_str(),
                    "Zero image size!");
        target = g_renderer->getDummyTexture();
        return;
    }

    #ifdef DEBUG_BUILD
    bindingTime.start();
    #endif
    RGBQUAD upperColor;
    FreeImage_GetPixelColor(sourceImage, 0, 0, &upperColor);
    target.ColorUpper.r = float(upperColor.rgbRed) / 255.0f;
    target.ColorUpper.b = float(upperColor.rgbBlue) / 255.0f;
    target.ColorUpper.g = float(upperColor.rgbGreen) / 255.0f;
    RGBQUAD lowerColor;
    FreeImage_GetPixelColor(sourceImage, 0, static_cast<unsigned int>(h - 1), &lowerColor);
    target.ColorLower.r = float(lowerColor.rgbRed) / 255.0f;
    target.ColorLower.b = float(lowerColor.rgbBlue) / 255.0f;
    target.ColorLower.g = float(lowerColor.rgbGreen) / 255.0f;
    FreeImage_FlipVertical(sourceImage);
    target.nOfColors = GL_RGBA;
    target.format = GL_BGRA;
    target.w = static_cast<int>(w);
    target.h = static_cast<int>(h);
    target.frame_w = static_cast<int>(w);
    target.frame_h = static_cast<int>(h);
    //    #ifdef PGE_USE_OpenGL_2_1
    //    glEnable(GL_TEXTURE_2D);
    //    #endif
    //    // Have OpenGL generate a texture object handle for us
    //    glGenTextures( 1, &(target.texture) ); GLERRORCHECK();
    //    // Bind the texture object
    //    glBindTexture( GL_TEXTURE_2D, target.texture ); GLERRORCHECK();
    GLubyte *textura = reinterpret_cast<GLubyte *>(FreeImage_GetBits(sourceImage));
    g_renderer->loadTexture(target, w, h, textura);
    // //    glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors, sourceImage.width(), sourceImage.height(),
    // //         0, target.format, GL_UNSIGNED_BYTE, sourceImage.bits() );
    //    glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors, w, h,
    //           0, target.format, GL_UNSIGNED_BYTE, textura ); GLERRORCHECK();
    //    glBindTexture( GL_TEXTURE_2D, 0); GLERRORCHECK();
    //    target.inited = true;
    #ifdef DEBUG_BUILD
    bindElapsed = bindingTime.nanoelapsed();
    unloadTime.start();
    #endif
    //SDL_FreeSurface(sourceImage);
    GraphicsHelps::closeImage(sourceImage);
    #ifdef DEBUG_BUILD
    unloadElapsed = unloadTime.nanoelapsed();
    #endif
    #ifdef DEBUG_BUILD
    pLogDebug("Mask merging of %s passed in %d nanoseconds", path.c_str(), static_cast<int>(maskElapsed));
    pLogDebug("Binding time of %s passed in %d nanoseconds", path.c_str(), static_cast<int>(bindElapsed));
    pLogDebug("Unload time of %s passed in %d nanoseconds", path.c_str(), static_cast<int>(unloadElapsed));
    pLogDebug("Total Loading of texture %s passed in %d nanoseconds (%dx%d)",
              path.c_str(),
              static_cast<int>(totalTime.nanoelapsed()),
              static_cast<int>(w),
              static_cast<int>(h));
    #endif
    return;
}

void GlRenderer::loadRawTextureP(PGE_Texture &target, uint8_t *pixels, uint32_t width, uint32_t height)
{
    target.w = static_cast<int>(width);
    target.h = static_cast<int>(height);
    target.frame_w = static_cast<int>(width);
    target.frame_h = static_cast<int>(height);
    g_renderer->loadTexture(target, width, height, pixels);
}

void GlRenderer::deleteTexture(PGE_Texture &tx)
{
    if((tx.inited) && (tx.texture != g_renderer->getDummyTexture().texture))
        g_renderer->deleteTexture(tx);

    tx.inited = false;
    tx.inited = false;
    tx.w = 0;
    tx.h = 0;
    tx.frame_w = 0;
    tx.frame_h = 0;
    tx.texture_layout = NULL;
    tx.format = 0;
    tx.nOfColors = 0;
    tx.ColorUpper.r = 0;
    tx.ColorUpper.g = 0;
    tx.ColorUpper.b = 0;
    tx.ColorLower.r = 0;
    tx.ColorLower.g = 0;
    tx.ColorLower.b = 0;
}

bool GlRenderer::isTopDown()
{
    return g_renderer->isTopDown();
}

int GlRenderer::getPixelDataSize(const PGE_Texture *tx)
{
    if(!tx)
        return 0;

    return (tx->w * tx->h * 4);
}

void GlRenderer::getPixelData(const PGE_Texture *tx, unsigned char *pixelData)
{
    g_renderer->getPixelData(tx, pixelData);
}

std::string GlRenderer::ScreenshotPath = "";

struct PGE_GL_shoot
{
    uint8_t *pixels;
    GLsizei w, h;
};

void GlRenderer::makeShot()
{
    if(!m_isReady) return;

    // Make the BYTE array, factor of 3 because it's RBG.
    int w, h;
    float wF, hF;
    SDL_GetWindowSize(PGE_Window::window, &w, &h);

    if((w == 0) || (h == 0))
    {
        PGE_Audio::playSoundByRole(obj_sound_role::WeaponFire);
        return;
    }

    wF = static_cast<float>(w);
    hF = static_cast<float>(h);
    wF = wF - m_offset_x * 2.0f;
    hF = hF - m_offset_y * 2.0f;
    w = static_cast<int>(wF);
    h = static_cast<int>(hF);
    uint8_t *pixels = new uint8_t[size_t(4 * w * h)];
    g_renderer->getScreenPixels(static_cast<int>(m_offset_x), static_cast<int>(m_offset_y), w, h, pixels);
    PGE_GL_shoot *shoot = new PGE_GL_shoot();
    shoot->pixels = pixels;
    shoot->w = w;
    shoot->h = h;
    m_screenshot_thread = SDL_CreateThread(makeShot_action, "scrn_maker", reinterpret_cast<void *>(shoot));
    PGE_Audio::playSoundByRole(obj_sound_role::PlayerTakeItem);
}

int GlRenderer::makeShot_action(void *_pixels)
{
    PGE_GL_shoot *shoot = reinterpret_cast<PGE_GL_shoot *>(_pixels);
    FIBITMAP *shotImg = FreeImage_ConvertFromRawBits(reinterpret_cast<BYTE *>(shoot->pixels), shoot->w, shoot->h,
                        3 * shoot->w + shoot->w % 4, 24, 0xFF0000, 0x00FF00, 0x0000FF, !g_renderer->isTopDown());

    if(!shotImg)
    {
        delete []shoot->pixels;
        shoot->pixels = NULL;
        delete []shoot;
        return 0;
    }

    FIBITMAP *temp;
    temp = FreeImage_ConvertTo32Bits(shotImg);

    if(!temp)
    {
        FreeImage_Unload(shotImg);
        delete []shoot->pixels;
        shoot->pixels = NULL;
        delete []shoot;
        return 0;
    }

    FreeImage_Unload(shotImg);
    shotImg = temp;

    if((shoot->w != m_viewport_w) || (shoot->h != m_viewport_h))
    {
        FIBITMAP *temp;
        temp = FreeImage_Rescale(shotImg, m_viewport_w, m_viewport_h, FILTER_BOX);

        if(!temp)
        {
            FreeImage_Unload(shotImg);
            delete []shoot->pixels;
            shoot->pixels = NULL;
            delete []shoot;
            return 0;
        }

        FreeImage_Unload(shotImg);
        shotImg = temp;
    }

    if(!DirMan::exists(ScreenshotPath))
        DirMan::mkAbsDir(ScreenshotPath);


    auto now = std::chrono::system_clock::now();
    std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
    tm *t = std::localtime(&in_time_t);
    static int prevSec = 0;
    static int prevSecCounter = 0;
    if(prevSec != t->tm_sec)
    {
        prevSec = t->tm_sec;
        prevSecCounter = 0;
    }
    else
        prevSecCounter++;

    std::string saveTo = fmt::format("{0}Scr_{1}_{2}_{3}_{4}_{5}_{6}_{7}.png", ScreenshotPath,
                                    t->tm_year, t->tm_mon, t->tm_mday,
                                    t->tm_hour, t->tm_min, t->tm_sec, prevSecCounter);
    pLogDebug("%s %d %d", saveTo.c_str(), shoot->w, shoot->h);

    if(FreeImage_HasPixels(shotImg) == FALSE)
        pLogWarning("Can't save screenshot: no pixel data!");
    else
        FreeImage_Save(FIF_PNG, shotImg, saveTo.data(), PNG_Z_BEST_COMPRESSION);

    FreeImage_Unload(shotImg);
    delete []shoot->pixels;
    shoot->pixels = NULL;
    delete []shoot;
    return 0;
}


static struct gifRecord
{
    GifWriter   writer      = {nullptr, nullptr, true};
    SDL_Thread *worker      = nullptr;
    SDL_mutex  *mutex       = nullptr;
    uint32_t    delay       = 3;
    double      delayTimer  = 0.0;
    bool        enabled     = false;
    unsigned char padding[7]= {0, 0, 0, 0, 0, 0, 0};
} g_gif;

bool GlRenderer::recordInProcess()
{
    return g_gif.enabled;
}

void GlRenderer::toggleRecorder()
{
    if(!g_gif.enabled)
    {
        //g_gif.mutex = SDL_CreateMutex();
        auto now = std::chrono::system_clock::now();
        std::time_t in_time_t = std::chrono::system_clock::to_time_t(now);
        tm *t = std::localtime(&in_time_t);

        std::string saveTo = fmt::format("{0}Scr_{1}_{2}_{3}_{4}_{5}_{6}.gif",
                                         ScreenshotPath,
                                         t->tm_year, t->tm_mon, t->tm_mday,
                                         t->tm_hour, t->tm_min, t->tm_sec);

        if(GifBegin(&g_gif.writer,
                    saveTo.data(),
                    static_cast<uint32_t>(m_viewport_w),
                    static_cast<uint32_t>(m_viewport_h), g_gif.delay, 8, false))
        {
            g_gif.enabled = true;
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerGrow);
        }
    }
    else
    {
        if(g_gif.worker)
            SDL_WaitThread(g_gif.worker, NULL);
        g_gif.worker = nullptr;
        //if(g_gif.mutex)
        //    SDL_DestroyMutex(g_gif.mutex);
        //g_gif.mutex = nullptr;
        GifEnd(&g_gif.writer);
        g_gif.enabled = false;
        PGE_Audio::playSoundByRole(obj_sound_role::PlayerShrink);
    }
}

void GlRenderer::processRecorder(double ticktime)
{
    if(g_gif.enabled)
    {
        g_gif.delayTimer += ticktime;
        if(g_gif.delayTimer >= double(g_gif.delay * 10))
            g_gif.delayTimer = 0.0;
        if(g_gif.delayTimer != 0.0)
            return;

        // Make the BYTE array, factor of 3 because it's RBG.
        int w, h;
        SDL_GetWindowSize(PGE_Window::window, &w, &h);

        if((w == 0) || (h == 0))
        {
            PGE_Audio::playSoundByRole(obj_sound_role::WeaponFire);
            return;
        }

        w = w - static_cast<int>(m_offset_x) * 2;
        h = h - static_cast<int>(m_offset_y) * 2;
        uint8_t *pixels = new uint8_t[size_t(4 * w * h)];
        g_renderer->getScreenPixelsRGBA(static_cast<int>(m_offset_x), static_cast<int>(m_offset_y), w, h, pixels);

        PGE_GL_shoot *shoot = new PGE_GL_shoot();
        shoot->pixels = pixels;
        shoot->w = w;
        shoot->h = h;
        if(g_gif.worker)
            SDL_WaitThread(g_gif.worker, NULL);
        g_gif.worker = SDL_CreateThread(processRecorder_action, "gif_recorder", reinterpret_cast<void *>(shoot));
    }
}

int GlRenderer::processRecorder_action(void *_pixels)
{
    //SDL_LockMutex(g_gif.mutex);
    PGE_GL_shoot *shoot = reinterpret_cast<PGE_GL_shoot *>(_pixels);
    FIBITMAP *shotImg = FreeImage_ConvertFromRawBitsEx(false, reinterpret_cast<BYTE *>(shoot->pixels), FIT_BITMAP,
                                                       shoot->w, shoot->h,
                                                        4 * shoot->w, 32,
                                                       0xFF000000, 0x00FF0000, 0x0000FF00, g_renderer->isTopDown());
    if((shoot->w != m_viewport_w) || (shoot->h != m_viewport_h))
    {
        FIBITMAP *temp;
        temp = FreeImage_Rescale(shotImg, m_viewport_w, m_viewport_h, FILTER_BOX);
        if(!temp)
        {
            FreeImage_Unload(shotImg);
            delete []shoot->pixels;
            shoot->pixels = nullptr;
            delete []shoot;
            SDL_UnlockMutex(g_gif.mutex);
            return 0;
        }
        FreeImage_Unload(shotImg);
        shotImg = temp;
    }

    if(FreeImage_HasPixels(shotImg) == FALSE)
        pLogWarning("Can't save gif frame: no pixel data!");

    uint8_t *img = FreeImage_GetBits(shotImg);
    GifWriteFrame(&g_gif.writer, img,
                  static_cast<uint32_t>(m_viewport_w),
                  static_cast<uint32_t>(m_viewport_h),
                  g_gif.delay/*uint32_t((ticktime)/10.0)*/, 8, false);
    FreeImage_Unload(shotImg);
    delete[] shoot->pixels;
    shoot->pixels = nullptr;
    delete []shoot;
    //SDL_UnlockMutex(g_gif.mutex);
    return 0;
}



bool GlRenderer::ready()
{
    return m_isReady;
}

void GlRenderer::flush()
{
    g_renderer->flush();
}

void GlRenderer::repaint()
{
    g_renderer->repaint();
    GlRenderer::processRecorder(static_cast<double>(PGE_Window::TimeOfFrame));
}

void GlRenderer::setClearColor(float r, float g, float b, float a)
{
    g_renderer->setClearColor(r, g, b, a);
}

void GlRenderer::clearScreen()
{
    g_renderer->clearScreen();
}

PGE_Point GlRenderer::MapToScr(PGE_Point point)
{
    return g_renderer->MapToScr(point.x(), point.y());
}

PGE_Point GlRenderer::MapToScr(int x, int y)
{
    return g_renderer->MapToScr(x, y);
}

int GlRenderer::alignToCenter(int x, int w)
{
    return g_renderer->alignToCenter(x, w);
}

void GlRenderer::setViewport(int x, int y, int w, int h)
{
    g_renderer->setViewport(x, y, w, h);
}

void GlRenderer::resetViewport()
{
    g_renderer->resetViewport();
}

void GlRenderer::setViewportSize(int w, int h)
{
    g_renderer->setViewportSize(w, h);
}

void GlRenderer::setWindowSize(int w, int h)
{
    g_renderer->setWindowSize(w, h);
}

void GlRenderer::renderRect(float x, float y, float w, float h, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, bool filled)
{
    g_renderer->renderRect(x, y, w, h, red, green, blue, alpha, filled);
}

void GlRenderer::renderRectBR(double _left, double _top, double _right, double _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    g_renderer->renderRectBR(static_cast<float>(_left),
                             static_cast<float>(_top),
                             static_cast<float>(_right),
                             static_cast<float>(_bottom),
                             red,  green, blue,  alpha);
}

void GlRenderer::renderTexture(PGE_Texture *texture, float x, float y)
{
    SDL_assert_release(texture);
    g_renderer->renderTexture(texture, x, y);
}

void GlRenderer::renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    SDL_assert_release(texture);
    g_renderer->renderTexture(texture, x, y, w, h, ani_top, ani_bottom, ani_left, ani_right);
}


void GlRenderer::BindTexture(PGE_Texture *texture)
{
    g_renderer->BindTexture(texture);
}

void GlRenderer::setTextureColor(float Red, float Green, float Blue, float Alpha)
{
    g_renderer->setTextureColor(Red, Green, Blue, Alpha);
}

void GlRenderer::renderTextureCur(float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    g_renderer->renderTextureCur(x, y, w, h, ani_top, ani_bottom, ani_left, ani_right);
}

void GlRenderer::UnBindTexture()
{
    g_renderer->UnBindTexture();
}
