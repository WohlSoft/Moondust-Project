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

const int width = 800; // Width of Window
const int height = 600; // Height of Window

void drawCube(float xrf, float yrf, float zrf);

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
                              width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window); // Creating of the OpenGL Context
    Q_UNUSED(glcontext);

    if(window == NULL){	// If failed to create window - exiting
        exit(1);
    }

    // Initializing OpenGL

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color
    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST); // Enabling depth test
    glShadeModel(GL_SMOOTH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float) width / (float) height, 0.1f, 100.0f); // Setting up 3D perspective
    glMatrixMode(GL_MODELVIEW); // Gonna into 3D mode

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

    float xrf = 0, yrf = 0, zrf = 0; // Rotating angles

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
                            yrf -= 1.0;
                        break;
                    case SDLK_RIGHT:
                            yrf += 1.0;
                        break;

                    case SDLK_UP:
                            zrf -= 1.0;
                        break;
                    case SDLK_DOWN:
                            zrf += 1.0;
                        break;
                    }
                break;
            }
        }

        // While program is running, changing loop

        xrf -= 1;
        yrf -= 1;
        zrf -= 1;

        drawCube(xrf, yrf, zrf); // Draw cube with current rotating conrers

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

void drawCube(float xrf, float yrf, float zrf){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -7.0f);	// Сдвинуть вглубь экрана

    glRotatef(xrf, 1.0f, 0.0f, 0.0f);	// Вращение куба по X, Y, Z
    glRotatef(yrf, 0.0f, 1.0f, 0.0f);	// Вращение куба по X, Y, Z
    glRotatef(zrf, 0.0f, 0.0f, 1.0f);	// Вращение куба по X, Y, Z

    glBegin(GL_QUADS);					// Рисуем куб

    glColor3f(0.0f, 1.0f, 0.0f);		// Синяя сторона (Верхняя)
    glVertex3f( 1.0f, 1.0f, -1.0f);		// Верхний правый угол квадрата
    glVertex3f(-1.0f, 1.0f, -1.0f);		// Верхний левый
    glVertex3f(-1.0f, 1.0f,  1.0f);		// Нижний левый
    glVertex3f( 1.0f, 1.0f,  1.0f);		// Нижний правый

    glColor3f(1.0f, 0.5f, 0.0f);		// Оранжевая сторона (Нижняя)
    glVertex3f( 1.0f, -1.0f,  1.0f);	// Верхний правый угол квадрата
    glVertex3f(-1.0f, -1.0f,  1.0f);	// Верхний левый
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Нижний левый
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Нижний правый

    glColor3f(1.0f, 0.0f, 0.0f);		// Красная сторона (Передняя)
    glVertex3f( 1.0f,  1.0f, 1.0f);		// Верхний правый угол квадрата
    glVertex3f(-1.0f,  1.0f, 1.0f);		// Верхний левый
    glVertex3f(-1.0f, -1.0f, 1.0f);		// Нижний левый
    glVertex3f( 1.0f, -1.0f, 1.0f);		// Нижний правый

    glColor3f(1.0f,1.0f,0.0f);			// Желтая сторона (Задняя)
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Верхний правый угол квадрата
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Верхний левый
    glVertex3f(-1.0f,  1.0f, -1.0f);	// Нижний левый
    glVertex3f( 1.0f,  1.0f, -1.0f);	// Нижний правый

    glColor3f(0.0f,0.0f,1.0f);			// Синяя сторона (Левая)
    glVertex3f(-1.0f,  1.0f,  1.0f);	// Верхний правый угол квадрата
    glVertex3f(-1.0f,  1.0f, -1.0f);	// Верхний левый
    glVertex3f(-1.0f, -1.0f, -1.0f);	// Нижний левый
    glVertex3f(-1.0f, -1.0f,  1.0f);	// Нижний правый

    glColor3f(1.0f,0.0f,1.0f);			// Фиолетовая сторона (Правая)
    glVertex3f( 1.0f,  1.0f, -1.0f);	// Верхний правый угол квадрата
    glVertex3f( 1.0f,  1.0f,  1.0f);	// Верхний левый
    glVertex3f( 1.0f, -1.0f,  1.0f);	// Нижний левый
    glVertex3f( 1.0f, -1.0f, -1.0f);	// Нижний правый

    glEnd();							// Закончили квадраты

}
