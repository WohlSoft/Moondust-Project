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

#include "render_opengl31.h"

#include <common_features/logger.h>

#ifdef RENDER_SUPORT_OPENGL3

#include "../window.h"
#include <common_features/graphics_funcs.h>
#include <common_features/fmt_format_ne.h>

#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#ifdef __ANDROID__
#include <SDL2/SDL_opengles.h>
#endif
#include <SDL2/SDL_thread.h>

#include "../gl_debug.h"

#ifdef _WIN32
#define FREEIMAGE_LIB
#endif
#include <FreeImageLite.h>

Render_OpenGL31::Render_OpenGL31() : Render_Base("OpenGL 3.1"),
    //Texture render color levels
    color_binded_texture{1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f,
                         1.0f, 1.0f, 1.0f, 1.0f}
{}

void Render_OpenGL31::set_SDL_settings()
{
    SDL_GL_ResetAttributes();
    // Enabling double buffer, setting up colors...
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);

#ifdef __ANDROID__ //Android specific
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          6);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           5);
#else //Generic
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
#endif

#ifndef __ANDROID__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);//3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);//1
#endif

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);  //for GL 3.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);//FOR GL 2.1
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    //  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,          16);
    //  SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE,         32);
    //  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,      0);
    //  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE,    0);
    //  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,     0);
    //  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE,    0);
    //  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS,  1);
    //  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,  2);
    //  SDL_GL_SetAttribute(SDL_GL_RETAINED_BACKING, 0);
    //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
}

unsigned int Render_OpenGL31::SDL_InitFlags()
{
    return SDL_WINDOW_OPENGL;
}

bool Render_OpenGL31::init()
{
    LogDebug("Create OpenGL context...");
    //Creating of the OpenGL Context
    PGE_Window::glcontext = SDL_GL_CreateContext(PGE_Window::window);

    if(!PGE_Window::glcontext)
    {
        pLogWarning("GL 3.1: Failed to create context! %s", SDL_GetError());
        return false;
    }

    if(PGE_Window::isSdlError())
    {
        pLogWarning("GL 3.1: Failed to init context! %s", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(PGE_Window::window, PGE_Window::glcontext);

    if(PGE_Window::isSdlError())
    {
        pLogWarning("GL 3.1: Failed to set context as current! %s", SDL_GetError());
        return false;
    }

    glViewport(static_cast<GLint>(0), static_cast<GLint>(0), PGE_Window::Width, PGE_Window::Height);
    GLERRORCHECK();
    //Initialize clear color
    glClearColor(0.f, 0.f, 0.f, 1.f);
    GLERRORCHECK();
    glDisable(GL_DEPTH_TEST);
    GLERRORCHECK();
    glDepthFunc(GL_NEVER);
    GLERRORCHECK(); //Ignore depth values (Z) to cause drawing bottom to top
    glEnable(GL_BLEND);
    GLERRORCHECK();
    glEnable(GL_TEXTURE_2D);
    GLERRORCHECK();
    return true;
}

bool Render_OpenGL31::uninit()
{
    glDeleteTextures(1, &(_dummyTexture.texture));
    SDL_GL_DeleteContext(PGE_Window::glcontext);
    return true;
}

void Render_OpenGL31::initDummyTexture()
{
    FIBITMAP *image = GraphicsHelps::loadImageRC("_broken.png");

    if(!image)
    {
        std::string msg = fmt::format_ne("Can't initialize dummy texture!\n"
                                      "In file: {0}:{1}", __FILE__, __LINE__);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
                                 "OpenGL Error", msg.c_str(), NULL);
        abort();
    }

    uint32_t w = static_cast<uint32_t>(FreeImage_GetWidth(image));
    uint32_t h = static_cast<uint32_t>(FreeImage_GetHeight(image));
    _dummyTexture.nOfColors = GL_RGBA;
    _dummyTexture.format = GL_BGRA;
    _dummyTexture.w = static_cast<int>(w);
    _dummyTexture.h = static_cast<int>(h);
    GLubyte *textura = reinterpret_cast<GLubyte *>(FreeImage_GetBits(image));
    loadTexture(_dummyTexture, w, h, textura);
    GraphicsHelps::closeImage(image);
}

PGE_Texture Render_OpenGL31::getDummyTexture()
{
    return _dummyTexture;
}

void Render_OpenGL31::loadTexture(PGE_Texture &target, uint32_t width, uint32_t height, uint8_t *RGBApixels)
{
    // Have OpenGL generate a texture object handle for us
    glGenTextures(1, &(target.texture));
    GLERRORCHECK();
    // Bind the texture object
    glBindTexture(GL_TEXTURE_2D, target.texture);
    GLERRORCHECK();
    glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors,
                 static_cast<GLsizei>(width),
                 static_cast<GLsizei>(height),
                 0, target.format, GL_UNSIGNED_BYTE, reinterpret_cast<GLubyte *>(RGBApixels));
    GLERRORCHECK();
    glBindTexture(GL_TEXTURE_2D, 0);
    GLERRORCHECK();
    target.inited = true;
}

void Render_OpenGL31::deleteTexture(PGE_Texture &tx)
{
    glDeleteTextures(1, &(tx.texture));
}

bool Render_OpenGL31::isTopDown()
{
    return true;
}

void Render_OpenGL31::getScreenPixels(int x, int y, int w, int h, unsigned char *pixels)
{
    glReadPixels(x, y, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels);
}

void Render_OpenGL31::getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels)
{
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void Render_OpenGL31::setViewport(int x, int y, int w, int h)
{
    glViewport(static_cast<GLint>(offset_x_draw + x * viewport_scale_x_draw),
               static_cast<GLint>(offset_y_draw + (window_h - (y + h)) * viewport_scale_y_draw),
               static_cast<GLsizei>(w * viewport_scale_x_draw),
               static_cast<GLsizei>(h * viewport_scale_y_draw));
    GLERRORCHECK();
    viewport_x = x;
    viewport_y = y;
    setViewportSize(w, h);
}

void Render_OpenGL31::resetViewport()
{
    float w, w1, wd1, h, h1, hd1, wd, hd;
    int   wi, hi, wid, hid;
    SDL_GetWindowSize(PGE_Window::window, &wi, &hi);
    SDL_GL_GetDrawableSize(PGE_Window::window, &wid, &hid);
    //Real size of window
    w = wi;
    h = hi;
    //Real renderable area size
    wd = wid;
    hd = hid;
    //Scaled window size
    w1 = w;
    h1 = h;
    //Scaled renderable area size
    wd1 = wd;
    hd1 = hd;
    scale_x = w / window_w;
    scale_y = h / window_h;
    scale_x_draw = wd / window_w;
    scale_y_draw = hd / window_h;
    viewport_scale_x = scale_x;
    viewport_scale_y = scale_y;
    viewport_scale_x_draw = scale_x_draw;
    viewport_scale_y_draw = scale_y_draw;

    if(scale_x_draw > scale_y_draw)
    {
        w1 = scale_y * window_w;
        wd1 = scale_y_draw * window_w;
        viewport_scale_x = w1 / window_w;
        viewport_scale_x_draw = wd1 / window_w;
    }
    else if(scale_x_draw < scale_y_draw)
    {
        h1 = scale_x * window_h;
        hd1 = scale_x_draw * window_h;
        viewport_scale_y = h1 / window_h;
        viewport_scale_y_draw = hd1 / window_h;
    }

    offset_x = (w - w1) / 2.0f;
    offset_y = (h - h1) / 2.0f;
    offset_x_draw = (wd - wd1) / 2.0f;
    offset_y_draw = (hd - hd1) / 2.0f;
    glViewport(static_cast<GLint>(offset_x_draw),
               static_cast<GLint>(offset_y_draw),
               static_cast<GLsizei>(wd1),
               static_cast<GLsizei>(hd1));
    GLERRORCHECK();
    setViewportSize(window_w, window_h);
}

void Render_OpenGL31::setViewportSize(int w, int h)
{
    setViewportSize(static_cast<float>(w), static_cast<float>(h));
}

void Render_OpenGL31::setViewportSize(float w, float h)
{
    viewport_w = w;
    viewport_h = h;
    viewport_w_half = w / 2.0f;
    viewport_h_half = h / 2.0f;
}

void Render_OpenGL31::setVirtualSurfaceSize(int w, int h)
{
    window_w = static_cast<float>(w);
    window_h = static_cast<float>(h);
}

void Render_OpenGL31::flush()
{
    glFlush();
}

void Render_OpenGL31::repaint()
{
    SDL_GL_SwapWindow(PGE_Window::window);
}

void Render_OpenGL31::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    GLERRORCHECK();
}

void Render_OpenGL31::clearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLERRORCHECK();
}



static inline void setRenderColors()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    GLERRORCHECK();
    glEnableClientState(GL_VERTEX_ARRAY);
    GLERRORCHECK();
    glEnableClientState(GL_COLOR_ARRAY);
    GLERRORCHECK();
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    GLERRORCHECK();
}

static inline void setRenderTexture(GLuint &tID)
{
    glBindTexture(GL_TEXTURE_2D, tID);
    GLERRORCHECK();
    glEnableClientState(GL_VERTEX_ARRAY);
    GLERRORCHECK();
    glEnableClientState(GL_COLOR_ARRAY);
    GLERRORCHECK();
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GLERRORCHECK();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GLERRORCHECK();
}

static inline void setUnbindTexture()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    GLERRORCHECK();
}

static inline void setAlphaBlending()
{
#ifdef GL_GLEXT_PROTOTYPES
    glBlendEquation(GL_FUNC_ADD);
    GLERRORCHECK();
#endif
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GLERRORCHECK();
}

void Render_OpenGL31::getPixelData(const PGE_Texture *tx, unsigned char *pixelData)
{
    if(!tx)
        return;

#ifndef __ANDROID__ // OpenGL 3.1
    setRenderTexture(const_cast<PGE_Texture *>(tx)->texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixelData);
    setUnbindTexture();
#else //OpenGL ES
    //FIXME: Implement correct texture pixel data capture!!!
#endif
}

void Render_OpenGL31::renderRect(float x, float y, float w, float h, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, bool filled)
{
    PGE_RectF rect = MapToGl(x, y, w, h);
    setRenderColors();
    setAlphaBlending();
    GLdouble Vertices[] =
    {
        rect.left(),  rect.top(), 0,
        rect.right(), rect.top(), 0,
        rect.right(), rect.bottom(), 0,
        rect.left(),  rect.bottom(), 0
    };
    GLfloat Colors[] = { red, green, blue, alpha,
                         red, green, blue, alpha,
                         red, green, blue, alpha,
                         red, green, blue, alpha
                       };
    glVertexPointer(3, GL_DOUBLE, 0, Vertices);
    GLERRORCHECK();
    glColorPointer(4, GL_FLOAT, 0, Colors);
    GLERRORCHECK();

    if(filled)
    {
        GLubyte indices[] =
        {
            0, 1, 2,
            0, 2, 3
        };
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
        GLERRORCHECK();
    }
    else
    {
        glDrawArrays(GL_LINE_LOOP, 0, 4);
        GLERRORCHECK();
    }
}

void Render_OpenGL31::renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    PGE_RectF rect = MapToGlSI(_left, _top, _right, _bottom);
    setRenderColors();
    setAlphaBlending();
    GLdouble Vertices[] =
    {
        rect.left(),  rect.top(), 0,
        rect.right(), rect.top(), 0,
        rect.right(), rect.bottom(), 0,
        rect.left(),  rect.bottom(), 0
    };
    GLfloat Colors[] = { red, green, blue, alpha,
                         red, green, blue, alpha,
                         red, green, blue, alpha,
                         red, green, blue, alpha
                       };
    GLubyte indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    glVertexPointer(3, GL_DOUBLE, 0, Vertices);
    GLERRORCHECK();
    glColorPointer(4, GL_FLOAT, 0, Colors);
    GLERRORCHECK();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
    GLERRORCHECK();
}

void Render_OpenGL31::renderTexture(PGE_Texture *texture, float x, float y)
{
    if(!texture) return;

    PGE_RectF rect = MapToGl(x, y, static_cast<float>(texture->w), static_cast<float>(texture->h));
    setRenderTexture(texture->texture);
    setAlphaBlending();
    GLdouble Vertices[] =
    {
        rect.left(),  rect.top(), 0,
        rect.right(), rect.top(), 0,
        rect.right(), rect.bottom(), 0,
        rect.left(),  rect.bottom(), 0
    };
    GLfloat TexCoord[] =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    GLubyte indices[] =
    {
        0, 1, 3, 2
    };
    glColorPointer(4, GL_FLOAT, 0, color_binded_texture);
    GLERRORCHECK();
    glVertexPointer(3, GL_DOUBLE, 0, Vertices);
    GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);
    GLERRORCHECK();
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);
    GLERRORCHECK();
    setUnbindTexture();
}

void Render_OpenGL31::renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    if(!texture) return;

    PGE_RectF rect = MapToGl(x, y, w, h);
    setRenderTexture(texture->texture);
    setAlphaBlending();
    GLdouble Vertices[] =
    {
        rect.left(),  rect.top(), 0,
        rect.right(), rect.top(), 0,
        rect.right(), rect.bottom(), 0,
        rect.left(),  rect.bottom(), 0
    };
    GLfloat TexCoord[] =
    {
        ani_left, ani_top,
        ani_right, ani_top,
        ani_right, ani_bottom,
        ani_left, ani_bottom
    };
    GLubyte indices[] =
    {
        0, 1, 3, 2
    };
    glColorPointer(4, GL_FLOAT, 0, color_binded_texture);
    GLERRORCHECK();
    glVertexPointer(3, GL_DOUBLE, 0, Vertices);
    GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);
    GLERRORCHECK();
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);
    GLERRORCHECK();
    setUnbindTexture();
}

void Render_OpenGL31::renderTextureCur(float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    PGE_RectF rect = MapToGl(x, y, w, h);
    GLdouble Vertices[] =
    {
        rect.left(),  rect.top(), 0,
        rect.right(), rect.top(), 0,
        rect.right(), rect.bottom(), 0,
        rect.left(),  rect.bottom(), 0
    };
    GLfloat TexCoord[] =
    {
        ani_left, ani_top,
        ani_right, ani_top,
        ani_right, ani_bottom,
        ani_left, ani_bottom
    };
    GLubyte indices[] = { 0, 1, 3, 2 };
    glColorPointer(4, GL_FLOAT, 0, color_binded_texture);
    GLERRORCHECK();
    glVertexPointer(3, GL_DOUBLE, 0, Vertices);
    GLERRORCHECK();
    glTexCoordPointer(2, GL_FLOAT, 0, TexCoord);
    GLERRORCHECK();
    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, indices);
    GLERRORCHECK();
}

void Render_OpenGL31::BindTexture(PGE_Texture *texture)
{
    setRenderTexture(texture->texture);
    setAlphaBlending();
}

void Render_OpenGL31::setTextureColor(float Red, float Green, float Blue, float Alpha)
{
    for(int i = 0; i < 4; i++)
    {
        color_binded_texture[i * 4 + 0] = Red;
        color_binded_texture[i * 4 + 1] = Green;
        color_binded_texture[i * 4 + 2] = Blue;
        color_binded_texture[i * 4 + 3] = Alpha;
    }
}

void Render_OpenGL31::UnBindTexture()
{
    setUnbindTexture();
}

PGE_RectF Render_OpenGL31::MapToGl(float x, float y, float w, float h)
{
    PGE_RectF rect;
    rect.setLeft(static_cast<double>(roundf(x) / (viewport_w_half) - 1.0f));
    rect.setTop(static_cast<double>((viewport_h - (roundf(y))) / viewport_h_half - 1.0f));
    rect.setRight(static_cast<double>(roundf(x + w) / (viewport_w_half) - 1.0f));
    rect.setBottom(static_cast<double>((viewport_h - (roundf(y + h))) / viewport_h_half - 1.0f));
    return rect;
}

PGE_RectF Render_OpenGL31::MapToGlSI(float left, float top, float right, float bottom)
{
    PGE_RectF rect;
    rect.setLeft(static_cast<double>(roundf(left) / (viewport_w_half) - 1.0f));
    rect.setTop(static_cast<double>((viewport_h - (roundf(top))) / viewport_h_half - 1.0f));
    rect.setRight(static_cast<double>(roundf(right) / (viewport_w_half) - 1.0f));
    rect.setBottom(static_cast<double>((viewport_h - (roundf(bottom))) / viewport_h_half - 1.0f));
    return rect;
}

PGE_Point Render_OpenGL31::MapToScr(PGE_Point point)
{
    return MapToScr(point.x(), point.y());
}

PGE_Point Render_OpenGL31::MapToScr(int x, int y)
{
    return PGE_Point(
               static_cast<int>( (static_cast<float>(x) - offset_x) / viewport_scale_x),
               static_cast<int>( (static_cast<float>(y) - offset_y) / viewport_scale_y)
           );
}

int Render_OpenGL31::alignToCenterW(int x, int w)
{
    return x + (static_cast<int>(viewport_w_half) - (w / 2));
}

int Render_OpenGL31::alignToCenterH(int y, int h)
{
    return y + (static_cast<int>(viewport_h_half) - (h / 2));
}

#else //RENDER_SUPORT_OPENGL3

bool Render_OpenGL31::init()
{
    pLogWarning("GL 3.1: this renderer is not supported!");
    return false;
}

bool Render_OpenGL31::uninit()
{
    pLogWarning("GL 3.1: this renderer is not supported!");
    return false;
}

#endif //RENDER_SUPORT_OPENGL3
