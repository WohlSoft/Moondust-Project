#include "window.h"
#include <iostream>
#include "../common_features/graphics_funcs.h"

int PGE_Window::Width=800;
int PGE_Window::Height=600;
SDL_Window *PGE_Window::window;

bool PGE_Window::IsInit=false;



bool PGE_Window::init(QString WindowTitle)
{
    // Initalizing SDL

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ){
        std::cout << "Unable to init SDL, error: " << SDL_GetError() << '\n';
        return false;
    }

    // Enabling double buffer, setting up colors...

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    // Creating window with QString title, with size 800x600 and placing to screen center

    window = SDL_CreateWindow(WindowTitle.toStdString().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              Width, Height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window); // Creating of the OpenGL Context

    SDL_SetWindowIcon(window,
                           GraphicsHelps::QImage_toSDLSurface(QImage(":/icon/cat_16.png")));


    SDL_GL_SetSwapInterval(1);
    Q_UNUSED(glcontext);

    if(window == NULL)
    {	// If failed to create window - exiting
        return false;
    }

    IsInit=true;
    return true;
}


bool PGE_Window::uninit()
{
    SDL_Quit();
    IsInit=false;
    return true;
}

bool PGE_Window::isReady()
{
    return IsInit;
}
