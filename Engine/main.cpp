//#include "mainwindow.h"
#include <QCoreApplication>
#include <QElapsedTimer>

#include <SDL2/SDL.h> // SDL 2 Library

#undef main

#include <GL/gl.h> // OpenGL Library
#include <GL/glu.h> // GLU Library

#include <iostream>
using namespace std;

SDL_Window *window; // Creating window for SDL

const int screen_width = 800; // Width of Window
const int screen_height = 600; // Height of Window

//Viewport mode
enum ViewPortMode
{
    VIEWPORT_MODE_FULL,
    VIEWPORT_MODE_HALF_CENTER,
    VIEWPORT_MODE_HALF_TOP,
    VIEWPORT_MODE_QUAD,
    VIEWPORT_MODE_RADAR
};

//Viewport mode
int gViewportMode = VIEWPORT_MODE_FULL;

void drawQuads();

void init()
{

    // Initalizing SDL

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        cout << "Unable to init SDL, error: " << SDL_GetError() << endl;
        exit(1);
    }

    // Enabling double buffer, setting up colors...

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    // Creating window with QString title, with size 800x600 and placing to screen center

    window = SDL_CreateWindow(QString("Title - is a QString from Qt, working in the SDL with OpenGL!!!").toStdString().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              screen_width, screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window); // Creating of the OpenGL Context
    Q_UNUSED(glcontext);

    if(window == NULL){	// If failed to create window - exiting
        exit(1);
    }

    // Initializing OpenGL

    glViewport( 0.f, 0.f, screen_width, screen_height );

    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( 0.0, screen_width, screen_height, 0.0, 1.0, -1.0 );

    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    //Initialize clear color
    glClearColor( 0.f, 0.f, 0.f, 1.f );

    //Check for error
    //GLenum error = glGetError();
//    if( error != GL_NO_ERROR )
//    {
//        printf( "Error initializing OpenGL! %s\n", gluErrorString( error ) );
//        //return false;
//    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color
    //glClearDepth(1.0);
    //glDepthFunc(GL_LESS);
    //glEnable(GL_DEPTH_TEST); // Enabling depth test
    //glShadeModel(GL_SMOOTH);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluPerspective(45.0f, (float) screen_width / (float) screen_height, 0.1f, 100.0f); // Setting up 3D perspective
    //glMatrixMode(GL_MODELVIEW);

    //glViewport( 0.f, 0.f, screen_width, screen_height );

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Q_UNUSED(a);
    //MainWindow w;
    //w.show();

    //a.exec();

    Uint32 start;

    init(); // Initializing
    bool running = true;

    //float xrf = 0, yrf = 0, zrf = 0; // Rotating angles

    while(running)
    {
        start=SDL_GetTicks();
        SDL_Event event; //  Events of SDL

        while ( SDL_PollEvent(&event) )
        {
            switch(event.type){
                case SDL_QUIT:
                    running = false;
                break;

                case SDL_KEYDOWN: // If pressed key
                    switch(event.key.keysym.sym)
                    { // Check which
                    case SDLK_ESCAPE: // ESC
                            running = false; // End work of program
                        break;
                    case SDLK_LEFT:
                            gViewportMode++;
                            if( gViewportMode > VIEWPORT_MODE_RADAR )
                            {
                                gViewportMode = VIEWPORT_MODE_FULL;
                            }
                        break;
                    case SDLK_RIGHT:
                            gViewportMode--;
                            if( gViewportMode < VIEWPORT_MODE_FULL )
                            {
                                gViewportMode = VIEWPORT_MODE_RADAR;
                            }
                        break;

                        //                    case SDLK_UP:
                        //                            zrf -= 1.0;
                        //                        break;
                        //                    case SDLK_DOWN:
                        //                            zrf += 1.0;
                        //                        break;
                    }
                break;
            }
        }

        // While program is running, changing loop

        //xrf -= 1;
        //yrf -= 1;
        //zrf -= 1;

        drawQuads();
        //drawCube(xrf, yrf, zrf); // Draw cube with current rotating conrers

        // Updating screen

        glFlush();
        SDL_GL_SwapWindow(window);

        if(1000.0/1000>SDL_GetTicks()-start)
                                SDL_Delay(1000.0/1000-(SDL_GetTicks()-start));
    }

    SDL_Quit(); // Ending work of the SDL and exiting
    return 0;
    //return
}



//Cube draw (only test of the OpenGL works)

void drawQuads()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Reset modelview matrix
    glLoadIdentity();

    //Move to center of the screen
    glTranslatef( screen_width / 2.f, screen_height / 2.f, 0.f );

    //Full View
    if( gViewportMode == VIEWPORT_MODE_FULL )
    {
        //Fill the screen
        glViewport( 0.f, 0.f, screen_width, screen_height );

        //Red quad
        glBegin( GL_QUADS );
            glColor3f( 1.f, 0.f, 0.f );
            glVertex2f( -screen_width / 2.f, -screen_height / 2.f );
            glVertex2f(  screen_width / 2.f, -screen_height / 2.f );
            glVertex2f(  screen_width / 2.f,  screen_height / 2.f );
            glVertex2f( -screen_width / 2.f,  screen_height / 2.f );
        glEnd();
    }
    //View port at center of screen
    else if( gViewportMode == VIEWPORT_MODE_HALF_CENTER )
    {
        //Center viewport
        glViewport( screen_width / 4.f, screen_height / 4.f, screen_width / 2.f, screen_height / 2.f );

        //Green quad
        glBegin( GL_QUADS );
            glColor3f( 0.f, 1.f, 0.f );
            glVertex2f( -screen_width / 2.f, -screen_height / 2.f );
            glVertex2f(  screen_width / 2.f, -screen_height / 2.f );
            glVertex2f(  screen_width / 2.f,  screen_height / 2.f );
            glVertex2f( -screen_width / 2.f,  screen_height / 2.f );
        glEnd();
    }
    //Viewport centered at the top
    else if( gViewportMode == VIEWPORT_MODE_HALF_TOP )
    {
        //Viewport at top
        glViewport( screen_width / 4.f, screen_height / 2.f, screen_width / 2.f, screen_height / 2.f );

        //Blue quad
        glBegin( GL_QUADS );
            glColor3f( 0.f, 0.f, 1.f );
            glVertex2f( -screen_width / 2.f, -screen_height / 2.f );
            glVertex2f(  screen_width / 2.f, -screen_height / 2.f );
            glVertex2f(  screen_width / 2.f,  screen_height / 2.f );
            glVertex2f( -screen_width / 2.f,  screen_height / 2.f );
        glEnd();
    }
    //Four viewports
    else if( gViewportMode == VIEWPORT_MODE_QUAD )
    {
        //Bottom left red quad
        glViewport( 0.f, 0.f, screen_width / 2.f, screen_height / 2.f );
        glBegin( GL_QUADS );
            glColor3f( 1.f, 0.f, 0.f );
            glVertex2f( -screen_width / 4.f, -screen_height / 4.f );
            glVertex2f(  screen_width / 4.f, -screen_height / 4.f );
            glVertex2f(  screen_width / 4.f,  screen_height / 4.f );
            glVertex2f( -screen_width / 4.f,  screen_height / 4.f );
        glEnd();

        //Bottom right green quad
        glViewport( screen_width / 2.f, 0.f, screen_width / 2.f, screen_height / 2.f );
        glBegin( GL_QUADS );
            glColor3f( 0.f, 1.f, 0.f );
            glVertex2f( -screen_width / 4.f, -screen_height / 4.f );
            glVertex2f(  screen_width / 4.f, -screen_height / 4.f );
            glVertex2f(  screen_width / 4.f,  screen_height / 4.f );
            glVertex2f( -screen_width / 4.f,  screen_height / 4.f );
        glEnd();

        //Top left blue quad
        glViewport( 0.f, screen_height / 2.f, screen_width / 2.f, screen_height / 2.f );
        glBegin( GL_QUADS );
            glColor3f( 0.f, 0.f, 1.f );
            glVertex2f( -screen_width / 4.f, -screen_height / 4.f );
            glVertex2f(  screen_width / 4.f, -screen_height / 4.f );
            glVertex2f(  screen_width / 4.f,  screen_height / 4.f );
            glVertex2f( -screen_width / 4.f,  screen_height / 4.f );
        glEnd();

        //Top right yellow quad
        glViewport( screen_width / 2.f, screen_height / 2.f, screen_width / 2.f, screen_height / 2.f );
        glBegin( GL_QUADS );
            glColor3f( 1.f, 1.f, 0.f );
            glVertex2f( -screen_width / 4.f, -screen_height / 4.f );
            glVertex2f(  screen_width / 4.f, -screen_height / 4.f );
            glVertex2f(  screen_width / 4.f,  screen_height / 4.f );
            glVertex2f( -screen_width / 4.f,  screen_height / 4.f );
        glEnd();
    }
    //Viewport with radar subview port
    else if( gViewportMode == VIEWPORT_MODE_RADAR )
    {
        //Full size quad
        glViewport( 0.f, 0.f, screen_width, screen_height );
        glBegin( GL_QUADS );
            glColor3f( 1.f, 1.f, 1.f );
            glVertex2f( -screen_width / 8.f, -screen_height / 8.f );
            glVertex2f(  screen_width / 8.f, -screen_height / 8.f );
            glVertex2f(  screen_width / 8.f,  screen_height / 8.f );
            glVertex2f( -screen_width / 8.f,  screen_height / 8.f );
            glColor3f( 0.f, 0.f, 0.f );
            glVertex2f( -screen_width / 16.f, -screen_height / 16.f );
            glVertex2f(  screen_width / 16.f, -screen_height / 16.f );
            glVertex2f(  screen_width / 16.f,  screen_height / 16.f );
            glVertex2f( -screen_width / 16.f,  screen_height / 16.f );
        glEnd();

        //Radar quad
        glViewport( screen_width / 2.f, screen_height / 2.f, screen_width / 2.f, screen_height / 2.f );
        glBegin( GL_QUADS );
            glColor3f( 1.f, 1.f, 1.f );
            glVertex2f( -screen_width / 8.f, -screen_height / 8.f );
            glVertex2f(  screen_width / 8.f, -screen_height / 8.f );
            glVertex2f(  screen_width / 8.f,  screen_height / 8.f );
            glVertex2f( -screen_width / 8.f,  screen_height / 8.f );
            glColor3f( 0.f, 0.f, 0.f );
            glVertex2f( -screen_width / 16.f, -screen_height / 16.f );
            glVertex2f(  screen_width / 16.f, -screen_height / 16.f );
            glVertex2f(  screen_width / 16.f,  screen_height / 16.f );
            glVertex2f( -screen_width / 16.f,  screen_height / 16.f );
        glEnd();
    }
}
