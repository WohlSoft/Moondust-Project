/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "gl_renderer.h"
#include "window.h"
#include "../common_features/app_path.h"

#include <common_features/graphics_funcs.h>
#include <QtOpenGL/QGLWidget>

#include <gui/pge_msgbox.h>

#undef main
#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_thread.h>
#ifdef __APPLE__
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif
#undef main

#include <audio/pge_audio.h>

#include <QDir>
#include <QImage>
#include <QDateTime>
#include <QMessageBox>
#include <QGLWidget>
#include <QtDebug>

bool GlRenderer::_isReady=false;
SDL_Thread *GlRenderer::thread = NULL;

int GlRenderer::window_w=800;
int GlRenderer::window_h=600;
float GlRenderer::scale_x=1.0f;
float GlRenderer::scale_y=1.0f;
float GlRenderer::offset_x=0.0f;
float GlRenderer::offset_y=0.0f;
float GlRenderer::viewport_x=0;
float GlRenderer::viewport_y=0;
float GlRenderer::viewport_scale_x=1.0f;
float GlRenderer::viewport_scale_y=1.0f;
float GlRenderer::viewport_w=800;
float GlRenderer::viewport_h=600;
float GlRenderer::viewport_w_half=400;
float GlRenderer::viewport_h_half=300;

float GlRenderer::color_level_red=1.0;
float GlRenderer::color_level_green=1.0;
float GlRenderer::color_level_blue=1.0;
float GlRenderer::color_level_alpha=1.0;

PGE_Texture GlRenderer::_dummyTexture;

bool GlRenderer::init()
{
    if(!PGE_Window::isReady())
        return false;

    // Initializing OpenGL
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glViewport( 0.f, 0.f, PGE_Window::Width, PGE_Window::Height );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    glEnable( GL_TEXTURE_2D ); // Need this to display a texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glDisable( GL_DEPTH_TEST );
    glDepthFunc(GL_NEVER);// Ignore depth values (Z) to cause drawing bottom to top

    glDisable( GL_LIGHTING );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
        QMessageBox::critical(NULL, "OpenGL Error",
            QString("Error initializing OpenGL!\n%1")
            .arg( (char*)gluErrorString( error ) ), QMessageBox::Ok);
       //printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
       return false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color

    ScreenshotPath = AppPathManager::userAppDir()+"/screenshots/";
    _isReady=true;

    resetViewport();

    //Init dummy texture;
    initDummyTexture();

    return true;
}

bool GlRenderer::uninit()
{
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures( 1, &(_dummyTexture.texture) );
    return false;
}

void GlRenderer::initDummyTexture()
{
    loadTextureP(_dummyTexture, "://images/_broken.png");
}

PGE_Texture GlRenderer::loadTexture(QString path, QString maskPath)
{
    PGE_Texture target;
    loadTextureP(target, path, maskPath);
    return target;
}

void GlRenderer::loadTextureP(PGE_Texture &target, QString path, QString maskPath)
{
    QImage sourceImage;
    // Load the OpenGL texture
    sourceImage = GraphicsHelps::loadQImage(path); // Gives us the information to make the texture

    if(sourceImage.isNull())
    {
//        SDL_Quit();
//        //if(ErrorCheck::hardMode)
//        //{
            PGE_MsgBox::error(
                QString("Error loading of image file: \n%1\nReason: %2.")
                .arg(path).arg(QFileInfo(path).exists()?"wrong image format":"file not exist"));
//            exit(1);
        //}
        target = _dummyTexture;
        return;
    }

    //Apply Alpha mask
    if(!maskPath.isEmpty() && QFileInfo(maskPath).exists())
    {
        QImage maskImage = GraphicsHelps::loadQImage(maskPath);
        sourceImage = GraphicsHelps::setAlphaMask(sourceImage, maskImage);
    }

    sourceImage=sourceImage.convertToFormat(QImage::Format_ARGB32);
    QRgb upperColor = sourceImage.pixel(0,0);
    target.ColorUpper.r = float(qRed(upperColor))/255.0f;
    target.ColorUpper.g = float(qGreen(upperColor))/255.0f;
    target.ColorUpper.b = float(qBlue(upperColor))/255.0f;

    QRgb lowerColor = sourceImage.pixel(0, sourceImage.height()-1);
    target.ColorLower.r = float(qRed(lowerColor))/255.0f;
    target.ColorLower.g = float(qGreen(lowerColor))/255.0f;
    target.ColorLower.b = float(qBlue(lowerColor))/255.0f;

    //qDebug() << path << sourceImage.size();

    sourceImage = QGLWidget::convertToGLFormat(sourceImage).mirrored(false, true);

    target.nOfColors = 4;
    target.format = GL_RGBA;

    glEnable(GL_TEXTURE_2D);
    // Have OpenGL generate a texture object handle for us
    glGenTextures( 1, &(target.texture) );

    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, target.texture );

    // Edit the texture object's image data using the information SDL_Surface gives us
    target.w = sourceImage.width();
    target.h = sourceImage.height();
    // Set the texture's stretching properties

    // Bind the texture object
    glBindTexture( GL_TEXTURE_2D, target.texture );

    glTexImage2D(GL_TEXTURE_2D, 0, target.nOfColors, sourceImage.width(), sourceImage.height(),
         0, target.format, GL_UNSIGNED_BYTE, sourceImage.bits() );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glDisable(GL_TEXTURE_2D);

    target.inited = true;

    return;
}

void GlRenderer::deleteTexture(PGE_Texture &tx)
{
    if( (tx.inited) && (tx.texture != _dummyTexture.texture))
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures( 1, &(tx.texture) );
    }
    tx.inited = false;
    tx.inited=false;
    tx.w=0;
    tx.h=0;
    tx.texture_layout=NULL; tx.format=0;tx.nOfColors=0;
    tx.ColorUpper.r=0; tx.ColorUpper.g=0; tx.ColorUpper.b=0;
    tx.ColorLower.r=0; tx.ColorLower.g=0; tx.ColorLower.b=0;
}










PGE_PointF GlRenderer::mapToOpengl(PGE_Point s)
{
    qreal nx  =  s.x() - qreal(PGE_Window::Width)  /  2;
    qreal ny  =  s.y() - qreal(PGE_Window::Height)  /  2;
    return PGE_PointF(nx, ny);
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
    glLoadIdentity();
    glReadPixels(offset_x, offset_y, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    PGE_GL_shoot *shoot=new PGE_GL_shoot();
    shoot->pixels=pixels;
    shoot->w=w;
    shoot->h=h;
    thread = SDL_CreateThread( makeShot_action, "scrn_maker", (void*)shoot );

    PGE_Audio::playSoundByRole(obj_sound_role::PlayerTakeItem);
}

int GlRenderer::makeShot_action(void *_pixels)
{
    PGE_GL_shoot *shoot=(PGE_GL_shoot*)_pixels;

    QImage shotImg(shoot->pixels, shoot->w, shoot->h, QImage::Format_RGB888);
    shotImg=shotImg.scaled(window_w, window_h).mirrored(false, true);
    if(!QDir(ScreenshotPath).exists()) QDir().mkpath(ScreenshotPath);

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();

    QString saveTo = QString("%1Scr_%2_%3_%4_%5_%6_%7_%8.png").arg(ScreenshotPath)
            .arg(date.year()).arg(date.month()).arg(date.day())
            .arg(time.hour()).arg(time.minute()).arg(time.second()).arg(time.msec());

    qDebug() << saveTo << shotImg.width() << shotImg.height();
    shotImg.save(saveTo, "PNG");

    delete []shoot->pixels;
    shoot->pixels=NULL;
    delete []shoot;

    return 0;
}

bool GlRenderer::ready()
{
    return _isReady;
}


void GlRenderer::setRGB(float Red, float Green, float Blue, float Alpha)
{
    color_level_red=Red;
    color_level_green=Green;
    color_level_blue=Blue;
    color_level_alpha=Alpha;
}

void GlRenderer::resetRGB()
{
    color_level_red=1.f;
    color_level_green=1.f;
    color_level_blue=1.f;
    color_level_alpha=1.f;
}


PGE_PointF GlRenderer::MapToGl(PGE_Point point)
{
    return MapToGl(point.x(), point.y());
}

PGE_PointF GlRenderer::MapToGl(float x, float y)
{
    double nx1 = roundf(x)/(viewport_w_half)-1.0;
    double ny1 = (viewport_h-(roundf(y)))/viewport_h_half-1.0;
    return PGE_PointF(nx1, ny1);
}

PGE_Point GlRenderer::MapToScr(PGE_Point point)
{
    return MapToScr(point.x(), point.y());
}

PGE_Point GlRenderer::MapToScr(int x, int y)
{
    return PGE_Point(((float(x))/viewport_scale_x)-offset_x, ((float(y))/viewport_scale_y)-offset_y);
}

int GlRenderer::alignToCenter(int x, int w)
{
    return x+(viewport_w_half-(w/2));
}

void GlRenderer::setViewport(int x, int y, int w, int h)
{
    glViewport(offset_x+x*viewport_scale_x,
               offset_y+(window_h-(y+h))*viewport_scale_y,
               w*viewport_scale_x, h*viewport_scale_y);
    viewport_x=x;
    viewport_y=y;
    setViewportSize(w, h);
}

void GlRenderer::resetViewport()
{
    float w, w1, h, h1;
    int   wi, hi;
    SDL_GetWindowSize(PGE_Window::window, &wi, &hi);
    w=wi;h=hi; w1=w;h1=h;
    scale_x=(float)((float)(w)/(float)window_w);
    scale_y=(float)((float)(h)/(float)window_h);
    viewport_scale_x = scale_x;
    viewport_scale_y = scale_y;
    if(scale_x>scale_y)
    {
        w1=scale_y*window_w;
        viewport_scale_x=w1/window_w;
    }
    else if(scale_x<scale_y)
    {
        h1=scale_x*window_h;
        viewport_scale_y=h1/window_h;
    }

    offset_x=(w-w1)/2;
    offset_y=(h-h1)/2;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(offset_x, offset_y, (GLsizei)w1, (GLsizei)h1);
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
    setViewportSize(window_w, window_h);
}

void GlRenderer::setViewportSize(int w, int h)
{
    viewport_w=w;
    viewport_h=h;
    viewport_w_half=w/2;
    viewport_h_half=h/2;
}

void GlRenderer::setWindowSize(int w, int h)
{
    window_w=w;
    window_h=h;
    resetViewport();
}

void GlRenderer::renderRect(float x, float y, float w, float h, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    glDisable(GL_TEXTURE_2D);
    glColor4f( red, green, blue, alpha);
    glBegin( GL_TRIANGLES );
        glVertex2f( left, top);
        glVertex2f( right, top);
        glVertex2f( right, bottom);

        glVertex2f( left, top);
        glVertex2f( left, bottom);
        glVertex2f( right,  bottom);
    glEnd();
}

void GlRenderer::renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    PGE_PointF point;
        point = MapToGl(_left, _top);
    float left = point.x();
    float top = point.y();
        point = MapToGl(_right, _bottom);
    float right = point.x();
    float bottom = point.y();

    glDisable(GL_TEXTURE_2D);
    glColor4f( red, green, blue, alpha);
    glBegin( GL_TRIANGLES );
        glVertex2f( left, top);
        glVertex2f( right, top);
        glVertex2f( right, bottom);

        glVertex2f( left, top);
        glVertex2f( left, bottom);
        glVertex2f( right,  bottom);
    glEnd();
}


void GlRenderer::renderTexture(PGE_Texture *texture, float x, float y)
{
    if(!texture) return;

    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+texture->w, y+texture->h);
    float right = point.x();
    float bottom = point.y();

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP);

    glColor4f( color_level_red, color_level_green, color_level_blue, color_level_alpha);
    glBindTexture( GL_TEXTURE_2D, texture->texture );

    glBegin( GL_TRIANGLES );
        glTexCoord2f( 0, 0 );
        glVertex2f( left, top);
        glTexCoord2f( 1, 0 );
        glVertex2f(  right, top);
        glTexCoord2f( 1, 1 );
        glVertex2f(  right, bottom);

        glTexCoord2f( 0, 0 );
        glVertex2f( left, top);
        glTexCoord2f( 0, 1 );
        glVertex2f( left,  bottom);
        glTexCoord2f( 1, 1 );
        glVertex2f(  right, bottom);
        glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GlRenderer::renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    if(!texture) return;
    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP);

    glColor4f( color_level_red, color_level_green, color_level_blue, color_level_alpha);
    glBindTexture( GL_TEXTURE_2D, texture->texture );

    glBegin( GL_TRIANGLES );
        glTexCoord2f( ani_left, ani_top );
        glVertex2f( left, top);
        glTexCoord2f( ani_right, ani_top );
        glVertex2f(  right, top);
        glTexCoord2f( ani_right, ani_bottom );
        glVertex2f(  right, bottom);

        glTexCoord2f( ani_left, ani_top );
        glVertex2f( left, top);
        glTexCoord2f( ani_left, ani_bottom );
        glVertex2f( left,  bottom);
        glTexCoord2f( ani_right, ani_bottom );
        glVertex2f(  right, bottom);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GlRenderer::renderTextureCur(float x, float y, float w, float h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    PGE_PointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    glBegin( GL_TRIANGLES );
        glTexCoord2f( ani_left, ani_top );
        glVertex2f( left, top);
        glTexCoord2f( ani_right, ani_top );
        glVertex2f(  right, top);
        glTexCoord2f( ani_right, ani_bottom );
        glVertex2f(  right, bottom);

        glTexCoord2f( ani_left, ani_top );
        glVertex2f( left, top);
        glTexCoord2f( ani_left, ani_bottom );
        glVertex2f( left,  bottom);
        glTexCoord2f( ani_right, ani_bottom );
        glVertex2f(  right, bottom);
    glEnd();
}

