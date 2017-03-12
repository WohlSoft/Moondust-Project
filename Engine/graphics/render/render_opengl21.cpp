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

#include "render_opengl21.h"

#ifndef __ANDROID__

#include "../window.h"
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <fmt/fmt_format.h>

#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_thread.h>

#include "../gl_debug.h"

#ifdef _WIN32
#define FREEIMAGE_LIB
#endif
#include <FreeImageLite.h>

static bool g_OpenGL2_convertToPowof2 = false;

static bool isNonPowOf2Supported()
{
    const GLubyte *sExtensions = glGetString(GL_EXTENSIONS);
    //ARB_texture_non_power_of_two
    return (strstr(reinterpret_cast<const char *>(sExtensions), "ARB_texture_non_power_of_two") != NULL);
}

inline uint32_t pow2roundup(uint32_t x)
{
    if(x == 0)
        return 0;

    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

inline int32_t pow2roundup(int32_t x)
{
    if(x < 0)
        return 0;

    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

static void toPowofTwo(FIBITMAP **image)
{
    unsigned int width = FreeImage_GetWidth(*image);
    unsigned int height = FreeImage_GetHeight(*image);
    width = pow2roundup(width);
    height = pow2roundup(height);
    FIBITMAP *newImage = FreeImage_Rescale(*image, static_cast<int>(width), static_cast<int>(height), FILTER_BOX);
    FreeImage_Unload(*image);
    *image = newImage;
}

Render_OpenGL21::Render_OpenGL21() : Render_Base("OpenGL 2.1"),
    //Virtual resolution of renderable zone
    window_w(800.0f),
    window_h(600.0f),
    //Scale of virtual and window resolutuins
    scale_x(1.0f),
    scale_y(1.0f),
    //Side offsets to keep ratio
    offset_x(0.0f),
    offset_y(0.0f),
    //current viewport
    viewport_x(0.0f),
    viewport_y(0.0f),
    //Need to calculate relative viewport position when screen was scaled
    viewport_scale_x(1.0f),
    viewport_scale_y(1.0f),
    //Resolution of viewport
    viewport_w(800.0f),
    viewport_h(600.0f),
    //Half values of viewport Resolution
    viewport_w_half(400.0f),
    viewport_h_half(300.0f),
    //Texture render color levels
    color_binded_texture{1.0f, 1.0f, 1.0f, 1.0f}
{}

Render_OpenGL21::~Render_OpenGL21()
{}

void Render_OpenGL21::set_SDL_settings()
{
    SDL_GL_ResetAttributes();
    // Enabling double buffer, setting up colors...
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);//3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);//1
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);  //for GL 3.1
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);//FOR GL 2.1
#endif
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

unsigned int Render_OpenGL21::SDL_InitFlags()
{
    return SDL_WINDOW_OPENGL;
}

bool Render_OpenGL21::init()
{
    pLogDebug("Create OpenGL context...");
    //Creating of the OpenGL Context
    PGE_Window::glcontext = SDL_GL_CreateContext(PGE_Window::window);

    if(!PGE_Window::glcontext)
    {
        pLogWarning("GL 2.1: Failed to create context! %s", SDL_GetError());
        return false;
    }

    if(PGE_Window::isSdlError())
    {
        pLogWarning("GL 2.1: Failed to init context! %s", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(PGE_Window::window, PGE_Window::glcontext);

    if(PGE_Window::isSdlError())
    {
        pLogWarning("GL 2.1: Failed to set context as current! %s", SDL_GetError());
        return false;
    }

    glViewport(0.f, 0.f, PGE_Window::Width, PGE_Window::Height);
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
    g_OpenGL2_convertToPowof2 = isNonPowOf2Supported();
    pLogDebug("OpenGL 2.1: Non-Pow-of-two textures supported: %d", g_OpenGL2_convertToPowof2);
    return true;
}

bool Render_OpenGL21::uninit()
{
    glDeleteTextures(1, &(_dummyTexture.texture));
    SDL_GL_DeleteContext(PGE_Window::glcontext);
    return true;
}

void Render_OpenGL21::initDummyTexture()
{
    FIBITMAP *image = GraphicsHelps::loadImageRC("_broken.png");

    if(!image)
    {
        std::string msg = fmt::format("Can't initialize dummy texture!\n"
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

PGE_Texture Render_OpenGL21::getDummyTexture()
{
    return _dummyTexture;
}

void Render_OpenGL21::loadTexture(PGE_Texture &target, uint32_t width, uint32_t height, uint8_t *RGBApixels)
{
    FIBITMAP *tempImage = NULL;

    if(!g_OpenGL2_convertToPowof2)
    {
        uint32_t p2_w = pow2roundup(width);
        uint32_t p2_h = pow2roundup(height);

        if((width != p2_w) || (height != p2_h))
        {
            tempImage = FreeImage_ConvertFromRawBits(RGBApixels,
                        static_cast<int>(width),
                        static_cast<int>(height),
                        static_cast<int>(width * 4),
                        32,
                        FI_RGBA_RED_MASK,
                        FI_RGBA_GREEN_MASK,
                        FI_RGBA_BLUE_MASK,
                        0);
            toPowofTwo(&tempImage);
            width = p2_w;
            height = p2_h;
            RGBApixels = FreeImage_GetBits(tempImage);
        }
    }

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

    if(tempImage)
        FreeImage_Unload(tempImage);
}

void Render_OpenGL21::deleteTexture(PGE_Texture &tx)
{
    glDeleteTextures(1, &(tx.texture));
}

bool Render_OpenGL21::isTopDown()
{
    return true;
}

void Render_OpenGL21::getScreenPixels(int x, int y, int w, int h, unsigned char *pixels)
{
    glReadPixels(x, y, w, h, GL_BGR, GL_UNSIGNED_BYTE, pixels);
}

void Render_OpenGL21::getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels)
{
    glReadPixels(x, y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

void Render_OpenGL21::setViewport(int x, int y, int w, int h)
{
    glViewport(static_cast<GLint>(offset_x + x * viewport_scale_x),
               static_cast<GLint>(offset_y + (window_h - (y + h))*viewport_scale_y),
               static_cast<GLsizei>(w * viewport_scale_x),
               static_cast<GLsizei>(h * viewport_scale_y));
    GLERRORCHECK();
    viewport_x = x;
    viewport_y = y;
    setViewportSize(w, h);
}

void Render_OpenGL21::resetViewport()
{
    float w, w1, h, h1;
    int   wi, hi;
    SDL_GetWindowSize(PGE_Window::window, &wi, &hi);
    w = wi;
    h = hi;
    w1 = w;
    h1 = h;
    scale_x = w / window_w;
    scale_y = h / window_h;
    viewport_scale_x = scale_x;
    viewport_scale_y = scale_y;

    if(scale_x > scale_y)
    {
        w1 = scale_y * window_w;
        viewport_scale_x = w1 / window_w;
    }
    else if(scale_x < scale_y)
    {
        h1 = scale_x * window_h;
        viewport_scale_y = h1 / window_h;
    }

    offset_x = (w - w1) / 2;
    offset_y = (h - h1) / 2;
    glViewport(static_cast<GLint>(offset_x),
               static_cast<GLint>(offset_y),
               static_cast<GLsizei>(w1),
               static_cast<GLsizei>(h1));
    GLERRORCHECK();
    setViewportSize(window_w, window_h);
}

void Render_OpenGL21::setViewportSize(int w, int h)
{
    setViewportSize(static_cast<float>(w), static_cast<float>(h));
}

void Render_OpenGL21::setViewportSize(float w, float h)
{
    viewport_w = w;
    viewport_h = h;
    viewport_w_half = w / 2.0f;
    viewport_h_half = h / 2.0f;
}

void Render_OpenGL21::setWindowSize(int w, int h)
{
    window_w = w;
    window_h = h;
    resetViewport();
}

void Render_OpenGL21::flush()
{
    glFlush();
}

void Render_OpenGL21::repaint()
{
    SDL_GL_SwapWindow(PGE_Window::window);
}

void Render_OpenGL21::setClearColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    GLERRORCHECK();
}

void Render_OpenGL21::clearScreen()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    GLERRORCHECK();
}

static inline void setRenderColors()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    GLERRORCHECK();
}

static inline void setRenderTexture(GLuint &tID)
{
    glBindTexture(GL_TEXTURE_2D, tID);
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

void Render_OpenGL21::getPixelData(const PGE_Texture *tx, unsigned char *pixelData)
{
    if(!tx)
        return;

    setRenderTexture((const_cast<PGE_Texture *>(tx))->texture);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixelData);
    setUnbindTexture();
}

void Render_OpenGL21::renderRect(float x, float y, float w, float h, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha, bool filled)
{
    PGE_RectF rect = MapToGl(x, y, w, h);
    setRenderColors();
    setAlphaBlending();
    glColor4f(red, green, blue, alpha);

    if(filled)
        glBegin(GL_QUADS);
    else
        glBegin(GL_LINE_LOOP);

    glVertex2d(rect.left(),  rect.top());
    glVertex2d(rect.right(), rect.top());
    glVertex2d(rect.right(), rect.bottom());
    glVertex2d(rect.left(),  rect.bottom());
    glEnd();
    GLERRORCHECK();
}

void Render_OpenGL21::renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    PGE_RectF rect = MapToGlSI(_left, _top, _right, _bottom);
    setRenderColors();
    setAlphaBlending();
    glColor4f(red, green, blue, alpha);
    glBegin(GL_QUADS);
    glVertex2d(rect.left(),  rect.top());
    glVertex2d(rect.right(), rect.top());
    glVertex2d(rect.right(), rect.bottom());
    glVertex2d(rect.left(),  rect.bottom());
    glEnd();
    GLERRORCHECK();
}

void Render_OpenGL21::renderTexture(PGE_Texture *texture, float x, float y)
{
    if(!texture) return;

    PGE_RectF rect = MapToGl(x, y, static_cast<float>(texture->w), static_cast<float>(texture->h));
    setRenderTexture(texture->texture);
    setAlphaBlending();
    glColor4f(color_binded_texture[0], color_binded_texture[1], color_binded_texture[2], color_binded_texture[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2d(rect.left(),  rect.top());
    glTexCoord2f(1.0f, 0.0f);
    glVertex2d(rect.right(), rect.top());
    glTexCoord2f(1.0f, 1.0f);
    glVertex2d(rect.right(), rect.bottom());
    glTexCoord2f(0.0f, 1.0f);
    glVertex2d(rect.left(),  rect.bottom());
    glEnd();
    GLERRORCHECK();
    setUnbindTexture();
}

void Render_OpenGL21::renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    if(!texture) return;

    PGE_RectF rect = MapToGl(x, y, w, h);
    setRenderTexture(texture->texture);
    setAlphaBlending();
    glColor4f(color_binded_texture[0], color_binded_texture[1], color_binded_texture[2], color_binded_texture[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(ani_left, ani_top);
    glVertex2d(rect.left(),  rect.top());
    glTexCoord2f(ani_right, ani_top);
    glVertex2d(rect.right(), rect.top());
    glTexCoord2f(ani_right, ani_bottom);
    glVertex2d(rect.right(), rect.bottom());
    glTexCoord2f(ani_left, ani_bottom);
    glVertex2d(rect.left(),  rect.bottom());
    glEnd();
    GLERRORCHECK();
    setUnbindTexture();
}

void Render_OpenGL21::renderTextureCur(float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    PGE_RectF rect = MapToGl(x, y, w, h);
    glColor4f(color_binded_texture[0], color_binded_texture[1], color_binded_texture[2], color_binded_texture[3]);
    glBegin(GL_QUADS);
    glTexCoord2f(ani_left, ani_top);
    glVertex2d(rect.left(), rect.top());
    glTexCoord2f(ani_right, ani_top);
    glVertex2d(rect.right(), rect.top());
    glTexCoord2f(ani_right, ani_bottom);
    glVertex2d(rect.right(), rect.bottom());
    glTexCoord2f(ani_left, ani_bottom);
    glVertex2d(rect.left(), rect.bottom());
    glEnd();
    GLERRORCHECK();
}

void Render_OpenGL21::BindTexture(PGE_Texture *texture)
{
    setRenderTexture(texture->texture);
    setAlphaBlending();
}

void Render_OpenGL21::setTextureColor(float Red, float Green, float Blue, float Alpha)
{
    color_binded_texture[0] = Red;
    color_binded_texture[1] = Green;
    color_binded_texture[2] = Blue;
    color_binded_texture[3] = Alpha;
}

void Render_OpenGL21::UnBindTexture()
{
    setUnbindTexture();
}

PGE_RectF Render_OpenGL21::MapToGl(float x, float y, float w, float h)
{
    PGE_RectF rect;
    rect.setLeft(static_cast<double>(roundf(x) / (viewport_w_half) - 1.0f));
    rect.setTop(static_cast<double>((viewport_h - (roundf(y))) / viewport_h_half - 1.0f));
    rect.setRight(static_cast<double>(roundf(x + w) / (viewport_w_half) - 1.0f));
    rect.setBottom(static_cast<double>((viewport_h - (roundf(y + h))) / viewport_h_half - 1.0f));
    return rect;
}

PGE_RectF Render_OpenGL21::MapToGlSI(float left, float top, float right, float bottom)
{
    PGE_RectF rect;
    rect.setLeft(static_cast<double>(roundf(left) / (viewport_w_half) - 1.0f));
    rect.setTop(static_cast<double>((viewport_h - (roundf(top))) / viewport_h_half - 1.0f));
    rect.setRight(static_cast<double>(roundf(right) / (viewport_w_half) - 1.0f));
    rect.setBottom(static_cast<double>((viewport_h - (roundf(bottom))) / viewport_h_half - 1.0f));
    return rect;
}

PGE_Point Render_OpenGL21::MapToScr(PGE_Point point)
{
    return MapToScr(point.x(), point.y());
}

PGE_Point Render_OpenGL21::MapToScr(int x, int y)
{
    return PGE_Point(
               static_cast<int>(((static_cast<float>(x)) / viewport_scale_x) - offset_x),
               static_cast<int>(((static_cast<float>(y)) / viewport_scale_y) - offset_y)
           );
}

int Render_OpenGL21::alignToCenter(int x, int w)
{
    return x + (static_cast<int>(viewport_w_half) - (w / 2));
}

#else

bool Render_OpenGL21::init()
{
    pLogWarning("GL 2.1: this renderer is not supported!");
    return false;
}

bool Render_OpenGL21::uninit()
{
    pLogWarning("GL 2.1: this renderer is not supported!");
    return false;
}
#endif
