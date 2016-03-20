/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <gui/pge_msgbox.h>

#define SDL_MAIN_HANDLED
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

#include <QDir>
#include <QImage>
#include <QDateTime>
#include <QMessageBox>
#include <QtDebug>

#ifdef DEBUG_BUILD
#include <QElapsedTimer>
#endif

static Render_Base      g_dummy;//Empty renderer
static Render_OpenGL31  g_opengl31;
static Render_OpenGL21  g_opengl21;
static Render_SW_SDL    g_swsdl;

Render_Base      *g_renderer=&g_dummy;


bool GlRenderer::_isReady=false;
SDL_Thread *GlRenderer::thread = NULL;

int GlRenderer::window_w=800;
int GlRenderer::window_h=600;
float GlRenderer::offset_x=0.0f;
float GlRenderer::offset_y=0.0f;

static bool detectOpenGL2()
{
    SDL_GL_ResetAttributes();
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,            8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,           8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,          8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);//3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);//1
    //SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
#ifdef __APPLE__
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);//FOR GL 2.1
#endif

    SDL_Window* dummy = SDL_CreateWindow("OpenGL 2 probe dummy window",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         10, 10, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if(!dummy)
        return false;

    SDL_GLContext glcontext = SDL_GL_CreateContext(dummy);
    if(!glcontext)
    {
        LogDebug(QString("SDL Error of OpenGL 2 probe: ")+SDL_GetError());
        SDL_DestroyWindow(dummy);
        return false;
    }
    const char* sdl_error = SDL_GetError();
    if(sdl_error[0]!='\0')
    {
        LogDebug(QString("SDL Error of OpenGL 2 probe: ")+sdl_error);
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(dummy);
        SDL_ClearError();
        return false;
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(dummy);

    SDL_ClearError();
    return true;
}

#ifndef __APPLE__
static bool detectOpenGL3()
{
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

    SDL_Window* dummy = SDL_CreateWindow("OpenGL 3 probe dummy window",
                                         SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,
                                         10, 10, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
    if(!dummy)
        return false;

    SDL_GLContext glcontext = SDL_GL_CreateContext(dummy);
    if(!glcontext)
    {
        LogDebug(QString("SDL Error of OpenGL 3 probe: ")+SDL_GetError());
        SDL_DestroyWindow(dummy);
        return false;
    }
    const char* sdl_error = SDL_GetError();
    if(sdl_error[0]!='\0')
    {
        LogDebug(QString("SDL Error of OpenGL 3 probe: ")+sdl_error);
        SDL_GL_DeleteContext(glcontext);
        SDL_DestroyWindow(dummy);
        SDL_ClearError();
        return false;
    }

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(dummy);

    SDL_ClearError();
    return true;
}
#endif


GlRenderer::RenderEngineType GlRenderer::setRenderer(GlRenderer::RenderEngineType rtype)
{
    if( rtype == RENDER_AUTO )
    {
        #ifndef __APPLE__
        if(detectOpenGL3())
        {
            rtype=RENDER_OPENGL_3_1;
            LogDebug("OpenGL 3.1 detected!");
        }
        else
        #endif
        #ifndef __ANDROID__
        if(detectOpenGL2())
        {
            rtype=RENDER_OPENGL_2_1;
            LogDebug("OpenGL 2.1 detected!");
        }//RENDER_SDL2
        else
        #endif
        {
            rtype=RENDER_INVALID;
            LogCritical("OpenGL not detected!");
        }
    }
    return rtype;
}

void GlRenderer::setup_OpenGL31()
{
    g_renderer=&g_opengl31;
    g_renderer->set_SDL_settings();
}

unsigned int GlRenderer::SDL_InitFlags()
{
    return g_renderer->SDL_InitFlags();
}

void GlRenderer::setup_OpenGL21()
{
    g_renderer=&g_opengl21;
    g_renderer->set_SDL_settings();
}

void GlRenderer::setup_SW_SDL()
{
    g_renderer = &g_swsdl;
    g_renderer->set_SDL_settings();
}


bool GlRenderer::init()
{
    if(!PGE_Window::isReady())
        return false;

    /*
    glewExperimental = GL_TRUE; // Needed for a Core-mode OpenGL
    if( glewInit() != GLEW_OK )
    {
        GLERROR("Impossible to initialize GLEW");
        return false;
    }
    GLERRORCHECK();*/

    g_renderer->init();

//    glViewport( 0.f, 0.f, PGE_Window::Width, PGE_Window::Height ); GLERRORCHECK();

//    //Initialize clear color
//    glClearColor( 0.f, 0.f, 0.f, 1.f ); GLERRORCHECK();

//    glDisable( GL_DEPTH_TEST ); GLERRORCHECK();
//    glDepthFunc(GL_NEVER); GLERRORCHECK(); //Ignore depth values (Z) to cause drawing bottom to top

//    glEnable(GL_BLEND); GLERRORCHECK();
//    #ifndef PGE_USE_OpenGL_3_2
//    glEnable(GL_TEXTURE_2D); GLERRORCHECK();
//    #endif

    ScreenshotPath = AppPathManager::userAppDir()+"/screenshots/";
    _isReady=true;

    g_renderer->resetViewport();

    //Init dummy texture;
    g_renderer->initDummyTexture();

    return true;
}

bool GlRenderer::uninit()
{
    //glDisable(GL_TEXTURE_2D);
    //glDeleteTextures( 1, &(_dummyTexture.texture) );
    g_renderer->uninit();
    return false;
}

PGE_Texture GlRenderer::loadTexture(QString path, QString maskPath)
{
    PGE_Texture target;
    loadTextureP(target, path, maskPath);
    return target;
}

void GlRenderer::loadTextureP(PGE_Texture &target, QString path, QString maskPath)
{
    //SDL_Surface * sourceImage;
    FIBITMAP* sourceImage;

    if(path.isEmpty())
        return;

    // Load the OpenGL texture
    //sourceImage = GraphicsHelps::loadQImage(path); // Gives us the information to make the texture
    if(path[0]==QChar(':'))
        sourceImage = GraphicsHelps::loadImageRC(path);
    else
        sourceImage = GraphicsHelps::loadImage(path);

    //Don't load mask if PNG image is used
    if(path.endsWith(".png", Qt::CaseInsensitive)) maskPath.clear();

    if(!sourceImage)
    {
        LogWarning(QString("Error loading of image file: \n%1\nReason: %2.")
            .arg(path).arg(QFileInfo(path).exists()?"wrong image format":"file not exist"));
        target = g_renderer->getDummyTexture();
        return;
    }

    #ifdef DEBUG_BUILD
    QElapsedTimer totalTime;
    QElapsedTimer maskMergingTime;
    QElapsedTimer bindingTime;
    QElapsedTimer unloadTime;
    totalTime.start();
    int  maskElapsed=0;
    int bindElapsed=0;
    int unloadElapsed=0;
    #endif

    //Apply Alpha mask
    if(!maskPath.isEmpty() && QFileInfo(maskPath).exists())
    {
        #ifdef DEBUG_BUILD
        maskMergingTime.start();
        #endif
        GraphicsHelps::mergeWithMask(sourceImage, maskPath);
        #ifdef DEBUG_BUILD
        maskElapsed = maskMergingTime.elapsed();
        #endif
    }

    int w = FreeImage_GetWidth(sourceImage);
    int h = FreeImage_GetHeight(sourceImage);

    if((w<=0) || (h<=0))
    {
        FreeImage_Unload(sourceImage);
        LogWarning(QString("Error loading of image file: \n%1\nReason: %2.")
            .arg(path).arg("Zero image size!"));
        target = g_renderer->getDummyTexture();
        return;
    }

    #ifdef DEBUG_BUILD
    bindingTime.start();
    #endif
    RGBQUAD upperColor;
    FreeImage_GetPixelColor(sourceImage, 0, 0, &upperColor);
    target.ColorUpper.r = float(upperColor.rgbRed)/255.0f;
    target.ColorUpper.b = float(upperColor.rgbBlue)/255.0f;
    target.ColorUpper.g = float(upperColor.rgbGreen)/255.0f;

    RGBQUAD lowerColor;
    FreeImage_GetPixelColor(sourceImage, 0, h-1, &lowerColor);
    target.ColorLower.r = float(lowerColor.rgbRed)/255.0f;
    target.ColorLower.b = float(lowerColor.rgbBlue)/255.0f;
    target.ColorLower.g = float(lowerColor.rgbGreen)/255.0f;

    FreeImage_FlipVertical(sourceImage);

    target.nOfColors = GL_RGBA;
    target.format = GL_BGRA;
    target.w = w;
    target.h = h;
    target.frame_w = w;
    target.frame_h = h;

//    #ifdef PGE_USE_OpenGL_2_1
//    glEnable(GL_TEXTURE_2D);
//    #endif
//    // Have OpenGL generate a texture object handle for us
//    glGenTextures( 1, &(target.texture) ); GLERRORCHECK();
//    // Bind the texture object
//    glBindTexture( GL_TEXTURE_2D, target.texture ); GLERRORCHECK();

    GLubyte* textura= (GLubyte*)FreeImage_GetBits(sourceImage);

    g_renderer->loadTexture(target, w, h, textura);

// //    glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors, sourceImage.width(), sourceImage.height(),
// //         0, target.format, GL_UNSIGNED_BYTE, sourceImage.bits() );
//    glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors, w, h,
//           0, target.format, GL_UNSIGNED_BYTE, textura ); GLERRORCHECK();
//    glBindTexture( GL_TEXTURE_2D, 0); GLERRORCHECK();
//    target.inited = true;

    #ifdef DEBUG_BUILD
    bindElapsed=bindingTime.elapsed();
    unloadTime.start();
    #endif

    //SDL_FreeSurface(sourceImage);
    GraphicsHelps::closeImage(sourceImage);

    #ifdef DEBUG_BUILD
    unloadElapsed=unloadTime.elapsed();
    #endif

    #ifdef DEBUG_BUILD
    LogDebug(QString("Mask merging of %1 passed in %2 milliseconds").arg(path).arg(maskElapsed));
    LogDebug(QString("Binding time of %1 passed in %2 milliseconds").arg(path).arg(bindElapsed));
    LogDebug(QString("Unload time of %1 passed in %2 milliseconds").arg(path).arg(unloadElapsed));
    LogDebug(QString("Total Loading of texture %1 passed in %2 milliseconds (%3x%4)")
               .arg(path).arg(totalTime.elapsed())
               .arg(w).arg(h));
    #endif

    return;
}

GLuint GlRenderer::QImage2Texture(QImage *img, PGE_Texture &tex)
{
    if(!img)
        return 0;
    QImage text_image = GraphicsHelps::convertToGLFormat(*img).mirrored(false, true);

    if(tex.inited)
        deleteTexture(tex);

    QRgb upperColor = text_image.pixel(0,0);
    QRgb lowerColor = text_image.pixel(0,text_image.height()-1);

    tex.ColorUpper.r = float(qRed(upperColor))/255.0f;
    tex.ColorUpper.b = float(qBlue(upperColor))/255.0f;
    tex.ColorUpper.g = float(qGreen(upperColor))/255.0f;

    tex.ColorLower.r = float(qRed(lowerColor))/255.0f;
    tex.ColorLower.b = float(qBlue(lowerColor))/255.0f;
    tex.ColorLower.g = float(qGreen(lowerColor))/255.0f;

    tex.nOfColors = GL_RGBA;
    tex.format = GL_BGRA;
    tex.w = text_image.width();
    tex.h = text_image.height();
    tex.frame_w = tex.w;
    tex.frame_h = tex.h;

    g_renderer->loadTexture(tex, tex.w, tex.h, text_image.bits());
//    glGenTextures(1, &tex.texture);  GLERRORCHECK();
//    glBindTexture(GL_TEXTURE_2D, tex.texture);  GLERRORCHECK();
//    glTexImage2D(GL_TEXTURE_2D, 0,  4,
//                 tex.w,
//                 tex.h,
//                 0, tex.nOfColors, GL_UNSIGNED_BYTE,
//                 text_image.bits() );  GLERRORCHECK();
//    tex.inited = true;
//    glBindTexture( GL_TEXTURE_2D, 0); GLERRORCHECK();
    return tex.texture;
}

void GlRenderer::deleteTexture(PGE_Texture &tx)
{
    if( (tx.inited) && (tx.texture != g_renderer->getDummyTexture().texture))
    {
        g_renderer->deleteTexture(tx);
//        #ifdef PGE_USE_OpenGL_2_1
//        glDisable(GL_TEXTURE_2D);
//        #endif
//        glDeleteTextures( 1, &(tx.texture) );
    }
    tx.inited = false;
    tx.inited=false;
    tx.w=0;
    tx.h=0;
    tx.frame_w=0;
    tx.frame_h=0;
    tx.texture_layout=NULL; tx.format=0;tx.nOfColors=0;
    tx.ColorUpper.r=0; tx.ColorUpper.g=0; tx.ColorUpper.b=0;
    tx.ColorLower.r=0; tx.ColorLower.g=0; tx.ColorLower.b=0;
}

void GlRenderer::deleteTexture(GLuint tx)
{
    g_renderer->deleteTexture(tx);
}

QString GlRenderer::ScreenshotPath = "";

struct PGE_GL_shoot{
    uchar* pixels;
    GLsizei w,h;
};

void GlRenderer::makeShot()
{
    if(!_isReady) return;

    // Make the BYTE array, factor of 3 because it's RBG.
    int w, h;
    SDL_GetWindowSize(PGE_Window::window, &w, &h);
    if((w==0) || (h==0))
    {
        PGE_Audio::playSoundByRole(obj_sound_role::WeaponFire);
        return;
    }

    w=w-offset_x*2;
    h=h-offset_y*2;

    uchar* pixels = new uchar[4*w*h];
    g_renderer->getScreenPixels(offset_x, offset_y, w, h, pixels);

    PGE_GL_shoot *shoot=new PGE_GL_shoot();
    shoot->pixels=pixels;
    shoot->w=w;
    shoot->h=h;
    thread = SDL_CreateThread( makeShot_action, "scrn_maker", (void*)shoot );

    PGE_Audio::playSoundByRole(obj_sound_role::PlayerTakeItem);
}

int GlRenderer::makeShot_action(void *_pixels)
{
    PGE_GL_shoot *shoot = (PGE_GL_shoot*)_pixels;

    FIBITMAP* shotImg = FreeImage_ConvertFromRawBits((BYTE*)shoot->pixels, shoot->w, shoot->h,
                                     3*shoot->w+shoot->w%4, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);
    if(!shotImg)
    {
        delete []shoot->pixels;
        shoot->pixels=NULL;
        delete []shoot;
        return 0;
    }

    FIBITMAP* temp;
    temp = FreeImage_ConvertTo32Bits(shotImg);
    if(!temp)
    {
        FreeImage_Unload(shotImg);
        delete []shoot->pixels;
        shoot->pixels=NULL;
        delete []shoot;
        return 0;
    }
    FreeImage_Unload(shotImg);
    shotImg = temp;

    if((shoot->w!=window_w)||(shoot->h!=window_h))
    {
        FIBITMAP* temp;
        temp = FreeImage_Rescale(shotImg, window_w, window_h, FILTER_BOX);
        if(!temp) {
            FreeImage_Unload(shotImg);
            delete []shoot->pixels;
            shoot->pixels=NULL;
            delete []shoot;
            return 0;
        }
        FreeImage_Unload(shotImg);
        shotImg = temp;
    }

    if(!QDir(ScreenshotPath).exists()) QDir().mkpath(ScreenshotPath);

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();

    QString saveTo = QString("%1Scr_%2_%3_%4_%5_%6_%7_%8.png").arg(ScreenshotPath)
            .arg(date.year()).arg(date.month()).arg(date.day())
            .arg(time.hour()).arg(time.minute()).arg(time.second()).arg(time.msec());

    qDebug() << saveTo << shoot->w << shoot->h;

    if(FreeImage_HasPixels(shotImg) == FALSE) {
        qWarning() <<"Can't save screenshot: no pixel data!";
    } else {
        FreeImage_Save(FIF_PNG, shotImg, saveTo.toUtf8().data(), PNG_Z_BEST_COMPRESSION);
    }

    FreeImage_Unload(shotImg);

    delete []shoot->pixels;
    shoot->pixels=NULL;
    delete []shoot;

    return 0;
}

bool GlRenderer::ready()
{
    return _isReady;
}

void GlRenderer::flush()
{
    g_renderer->flush();
}

void GlRenderer::repaint()
{
    g_renderer->repaint();
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

void GlRenderer::renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    g_renderer->renderRectBR(_left, _top, _right, _bottom,
                            red,  green, blue,  alpha);
}

void GlRenderer::renderTexture(PGE_Texture *texture, float x, float y)
{
    if(!texture) return;
    g_renderer->renderTexture(texture, x, y);
}

void GlRenderer::renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    if(!texture) return;
    g_renderer->renderTexture(texture, x, y, w, h, ani_top, ani_bottom, ani_left, ani_right );
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

