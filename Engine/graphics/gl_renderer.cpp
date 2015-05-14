/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

bool GlRenderer::init()
{
    if(!PGE_Window::isReady())
        return false;

    // Initializing OpenGL
    glMatrixMode( GL_PROJECTION );
    //glPushMatrix();
    glLoadIdentity();

    glViewport( 0.f, 0.f, PGE_Window::Width, PGE_Window::Height );
    //glOrtho( 0.0, PGE_Window::Width, PGE_Window::Heightб 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    //glPushMatrix();
    //glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    glEnable( GL_TEXTURE_2D ); // Need this to display a texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glEnable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glDepthFunc(GL_NEVER);// Ignore depth values (Z) to cause drawing bottom to top


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

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    ScreenshotPath = AppPathManager::userAppDir()+"/screenshots/";
    _isReady=true;

    resetViewport();

    return true;
}

bool GlRenderer::uninit()
{
    return false;
}

QPointF GlRenderer::mapToOpengl(QPoint s)
{
    qreal nx  =  s.x() - qreal(PGE_Window::Width)  /  2;
    qreal ny  =  s.y() - qreal(PGE_Window::Height)  /  2;
    return QPointF(nx, ny);
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
    uchar* pixels = new uchar[4*w*h];
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
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

QPointF GlRenderer::MapToGl(QPoint point)
{
    return MapToGl(point.x(), point.y());
}

QPointF GlRenderer::MapToGl(int x, int y)
{
    double nx1 = (float)x/(viewport_w_half)-1.f;
    double ny1 = (viewport_h-(float)y)/viewport_h_half-1.f;
    return QPointF(nx1, ny1);
}

QPoint GlRenderer::MapToScr(QPoint point)
{
    return MapToScr(point.x(), point.y());
}

QPoint GlRenderer::MapToScr(int x, int y)
{
    return QPoint(((float(x))/viewport_scale_x)-offset_x, ((float(y))/viewport_scale_y)-offset_y);
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



void GlRenderer::renderRect(int x, int y, int w, int h, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    QPointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    glDisable(GL_TEXTURE_2D);
    glColor4f( red, green, blue, alpha);
    glBegin( GL_QUADS );
        glVertex2f( left, top);
        glVertex2f(  right, top);
        glVertex2f(  right, bottom);
        glVertex2f( left,  bottom);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GlRenderer::renderRectBR(int _left, int _top, int _right, int _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    QPointF point;
        point = MapToGl(_left, _top);
    float left = point.x();
    float top = point.y();
        point = MapToGl(_right, _bottom);
    float right = point.x();
    float bottom = point.y();

    glDisable(GL_TEXTURE_2D);
    glColor4f( red, green, blue, alpha);
    glBegin( GL_QUADS );
        glVertex2f( left, top);
        glVertex2f(  right, top);
        glVertex2f(  right, bottom);
        glVertex2f( left,  bottom);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void GlRenderer::renderTexture(PGE_Texture *texture, int x, int y)
{
    QPointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+texture->w, y+texture->h);
    float right = point.x();
    float bottom = point.y();

    glEnable(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP);

    glColor4f( color_level_red, color_level_green, color_level_blue, color_level_alpha);
    glBindTexture( GL_TEXTURE_2D, texture->texture );

    glBegin( GL_QUADS );
        glTexCoord2f( 0, 0 );
        glVertex2f( left, top);

        glTexCoord2f( 1, 0 );
        glVertex2f(  right, top);

        glTexCoord2f( 1, 1 );
        glVertex2f(  right, bottom);

        glTexCoord2f( 0, 1 );
        glVertex2f( left,  bottom);
        glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GlRenderer::renderTexture(PGE_Texture *texture, int x, int y, int w, int h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    QPointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    glEnable(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP);

    glColor4f( color_level_red, color_level_green, color_level_blue, color_level_alpha);
    glBindTexture( GL_TEXTURE_2D, texture->texture );

    glBegin( GL_QUADS );
        glTexCoord2f( ani_left, ani_top );
        glVertex2f( left, top);

        glTexCoord2f( ani_right, ani_top );
        glVertex2f(  right, top);

        glTexCoord2f( ani_right, ani_bottom );
        glVertex2f(  right, bottom);

        glTexCoord2f( ani_left, ani_bottom );
        glVertex2f( left,  bottom);
        glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GlRenderer::renderTextureCur(int x, int y, int w, int h, float ani_top, float ani_bottom, float ani_left, float ani_right)
{
    QPointF point;
        point = MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    glBegin( GL_QUADS );
        glTexCoord2f( ani_left, ani_top );
        glVertex2f( left, top);

        glTexCoord2f( ani_right, ani_top );
        glVertex2f(  right, top);

        glTexCoord2f( ani_right, ani_bottom );
        glVertex2f(  right, bottom);

        glTexCoord2f( ani_left, ani_bottom );
        glVertex2f( left,  bottom);
    glEnd();
}

