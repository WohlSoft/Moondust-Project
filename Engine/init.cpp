#include "graphics.h"

#include <iostream>

void initSDL()
{
    // Initalizing SDL

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        std::cout << "Unable to init SDL, error: " << SDL_GetError() << '\n';
        exit(1);
    }

    // Enabling double buffer, setting up colors...

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    // Creating window with QString title, with size 800x600 and placing to screen center

    window = SDL_CreateWindow(QString("Title - is a QString from Qt, working in the SDL with OpenGL!!!").toStdString().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screen_width, screen_height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window); // Creating of the OpenGL Context
    SDL_GL_SetSwapInterval(1);
    Q_UNUSED(glcontext);

    if(window == NULL){	// If failed to create window - exiting
        exit(1);
    }
}





void initOpenGL()
{
    // Initializing OpenGL
    glViewport( 0.f, 0.f, screen_width, screen_height );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    glOrtho( 0.0, screen_width, screen_height, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glPushMatrix();
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    glEnable( GL_TEXTURE_2D ); // Need this to display a texture

    glEnable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    //Check for error
    //GLenum error = glGetError();
//    if( error != GL_NO_ERROR )
//    {
//        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
//        //return false;
//    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color


    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}
