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

#include "window.h"
#include <iostream>

#undef main
#include "../common_features/graphics_funcs.h"


int PGE_Window::Width=800;
int PGE_Window::Height=600;

int PGE_Window::MaxFPS=250;
int PGE_Window::PhysStep=75;

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

    //int StencilSize;
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, StencilSize);
    SDL_GL_SetSwapInterval(1);


    // Creating window with QString title, with size 800x600 and placing to screen center

    window = SDL_CreateWindow(WindowTitle.toStdString().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              Width, Height,
                              SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/ | SDL_WINDOW_OPENGL);

    SDL_GLContext glcontext = SDL_GL_CreateContext(window); // Creating of the OpenGL Context

    SDL_SetWindowIcon(window,
                           GraphicsHelps::QImage_toSDLSurface(QImage(":/icon/cat_16.png")));

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



SDL_bool PGE_Window::IsFullScreen(SDL_Window *win)
{
   Uint32 flags = SDL_GetWindowFlags(win);

    if (flags & SDL_WINDOW_FULLSCREEN) return SDL_TRUE; // return SDL_TRUE if fullscreen

   return SDL_FALSE; // Return SDL_FALSE if windowed
}

/// Toggles On/Off FullScreen
/// @returns -1 on error, 1 on Set fullscreen successful, 0 on Set Windowed successful

int PGE_Window::SDL_ToggleFS(SDL_Window *win)
{
    if(!win)
        win=window;

    if (IsFullScreen(win))
    {
        // Swith to WINDOWED mode
        if (SDL_SetWindowFullscreen(win, SDL_FALSE) < 0)
      {
         std::cout<<"Setting windowed failed : "<<SDL_GetError()<<std::endl;
         return -1;
      }

        return 0;
    }

    // Swith to FULLSCREEN mode
    if (SDL_SetWindowFullscreen(win, SDL_TRUE) < 0)
   {
      std::cout<<"Setting fullscreen failed : "<<SDL_GetError()<<std::endl;
      return -1;
   }

   return 1;
}
