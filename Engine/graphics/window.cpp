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

bool PGE_Window::showDebugInfo=false;

SDL_Window *PGE_Window::window;
SDL_GLContext PGE_Window::glcontext;

bool PGE_Window::IsInit=false;
bool PGE_Window::showCursor=true;


#include <QMessageBox>
#include <QtDebug>

void PGE_Window::checkSDLError(int line)
{
    const char *error = SDL_GetError();
    if (*error != '\0')
    {
        qDebug() << QString("SDL Error: %1").arg(error)
        << ((line != -1)?
            QString(" + line: %i").arg(line) : "");
        SDL_ClearError();
    }
}


bool PGE_Window::init(QString WindowTitle)
{
    // Initalizing SDL

    if ( SDL_Init(SDL_INIT_EVERYTHING) < 0 )
    {
        QMessageBox::critical(NULL, "SDL Error",
            QString("Unable to init SDL!\n%1")
            .arg( SDL_GetError() ), QMessageBox::Ok);
            //std::cout << "Unable to init SDL, error: " << SDL_GetError() << '\n';
        return false;
    }

    // Enabling double buffer, setting up colors...
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 6);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);

    // Устанавливаем версию использованной OpenGL (2.1)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    window = SDL_CreateWindow(WindowTitle.toStdString().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              Width, Height,
                              SDL_WINDOW_SHOWN /*| SDL_WINDOW_RESIZABLE*/ | SDL_WINDOW_OPENGL);
    checkSDLError();

    if(window == NULL)
    {
        // If failed to create window - exiting
        QMessageBox::critical(NULL, "SDL Error",
            QString("Unable to create window!\n%1")
            .arg( SDL_GetError() ), QMessageBox::Ok);
        return false;
    }

    SDL_SetWindowIcon(window, GraphicsHelps::QImage_toSDLSurface(QImage(":/icon/cat_16.png")));


    glcontext = SDL_GL_CreateContext(window); // Creating of the OpenGL Context
    checkSDLError();

    SDL_GL_SetSwapInterval(1);
    checkSDLError();
    IsInit=true;
    return true;
}


bool PGE_Window::uninit()
{
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    IsInit=false;
    return true;
}

bool PGE_Window::isReady()
{
    return IsInit;
}

void PGE_Window::setCursorVisibly(bool viz)
{
    showCursor=viz;
    if(window!=NULL)
    {
        if(!IsFullScreen(window))
        {
            if(showCursor)
                SDL_ShowCursor(SDL_ENABLE);
            else
                SDL_ShowCursor(SDL_DISABLE);
        }
    }
}



SDL_bool PGE_Window::IsFullScreen(SDL_Window *win)
{
   Uint32 flags = SDL_GetWindowFlags(win);

   if(flags & SDL_WINDOW_FULLSCREEN) return SDL_TRUE; // return SDL_TRUE if fullscreen

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
        //Show mouse cursor
        if(showCursor)
            SDL_ShowCursor(SDL_ENABLE);

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
        //Hide mouse cursor in full screen mdoe
        std::cout<<"Setting fullscreen failed : "<<SDL_GetError()<<std::endl;
        return -1;
    }
    SDL_ShowCursor(SDL_DISABLE);
    return 1;
}
