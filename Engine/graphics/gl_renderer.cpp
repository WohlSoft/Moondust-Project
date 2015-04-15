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
#ifdef __APPLE__
    #include <OpenGL/glu.h>
#else
    #include <GL/glu.h>
#endif
#undef main

#include <QDir>
#include <QImage>
#include <QDateTime>
#include <QMessageBox>
#include <QGLWidget>
#include <QtDebug>

bool GlRenderer::_isReady=false;

bool GlRenderer::init()
{
    if(!PGE_Window::isReady())
        return false;

    // Initializing OpenGL
    glMatrixMode( GL_PROJECTION );
    //glPushMatrix();
    glLoadIdentity();

    glViewport( 0.f, 0.f, PGE_Window::Width, PGE_Window::Height );
    glOrtho( 0.0, PGE_Window::Width, PGE_Window::Height, 0.0, 1.0, -1.0 );

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

void GlRenderer::makeShot()
{
    if(!_isReady) return;

    // Make the BYTE array, factor of 3 because it's RBG.
    uchar* pixels = new uchar[ 3 * PGE_Window::Width * PGE_Window::Height];
    glReadPixels(0, 0, PGE_Window::Width, PGE_Window::Height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    QImage shotImg = QImage(PGE_Window::Width, PGE_Window::Height, QImage::Format_ARGB32);
    shotImg.fill(Qt::black);

    for(int i=0, x=0, y=PGE_Window::Height-1; i<3*PGE_Window::Width*PGE_Window::Height; i+=3, x++)
    {
        if(x==PGE_Window::Width) {x=0; y--;}
        shotImg.setPixel( QPoint(x,y), qRgb(pixels[i],pixels[i+1], pixels[i+2]) );
    }

    if(!QDir(ScreenshotPath).exists()) QDir().mkpath(ScreenshotPath);

    QDate date = QDate::currentDate();
    QTime time = QTime::currentTime();

    QString saveTo = QString("%1Scr_%2_%3_%4_%5_%6_%7_%8.png").arg(ScreenshotPath)
            .arg(date.year()).arg(date.month()).arg(date.day())
            .arg(time.hour()).arg(time.minute()).arg(time.second()).arg(time.msec());

    qDebug() << saveTo << shotImg.width() << shotImg.height() << pixels[0];

    shotImg.save(saveTo, "PNG");

    delete [] pixels;
}

bool GlRenderer::ready()
{
    return _isReady;
}
