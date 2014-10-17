#include "gl_renderer.h"
#include "window.h"

#undef main
#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#undef main

bool GlRenderer::init()
{
    if(!PGE_Window::isReady())
        return false;

    // Initializing OpenGL
    glViewport( 0.f, 0.f, PGE_Window::Width, PGE_Window::Height );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    glOrtho( 0.0, PGE_Window::Width, PGE_Window::Height, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    glEnable( GL_TEXTURE_2D ); // Need this to display a texture
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glEnable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glDisable( GL_LIGHTING );

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);




    //Check for error
    GLenum error = glGetError();
    if( error != GL_NO_ERROR )
    {
       //printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
       return false;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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
