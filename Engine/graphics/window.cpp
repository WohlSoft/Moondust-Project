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

#include "window.h"
#include <iostream>

#undef main
#include "../common_features/graphics_funcs.h"

#include <settings/global_settings.h>
#include "gl_renderer.h"

int PGE_Window::Width=800;
int PGE_Window::Height=600;

int PGE_Window::MaxFPS=250;
int PGE_Window::TicksPerSecond=65;

bool PGE_Window::showDebugInfo=false;
bool PGE_Window::showPhysicsDebug=false;


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
    // Enabling double buffer, setting up colors...
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);

    SDL_GL_SetSwapInterval(0);
    checkSDLError();

    GlRenderer::setViewportSize(Width, Height);

    window = SDL_CreateWindow(WindowTitle.toStdString().c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              Width, Height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    checkSDLError();

    SDL_SetWindowMinimumSize(window, Width, Height);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    if(window == NULL)
    {
        // If failed to create window - exiting
        QMessageBox::critical(NULL, "SDL Error",
            QString("Unable to create window!\n%1")
            .arg( SDL_GetError() ), QMessageBox::Ok);
        return false;
    }

#ifdef Q_OS_MACX
    QImage icon(":/icon/cat_256.png");
#else
    QImage icon(":/icon/cat_16.png");
#endif
    SDL_SetWindowIcon(window, GraphicsHelps::QImage_toSDLSurface(icon));

    glcontext = SDL_GL_CreateContext(window); // Creating of the OpenGL Context
    checkSDLError();

    SDL_ShowWindow(window);

    IsInit=true;
    return true;
}


bool PGE_Window::uninit()
{
    // Swith to WINDOWED mode
    if (SDL_SetWindowFullscreen(window, SDL_FALSE) < 0)
    {
        qDebug() << "Setting windowed failed : "<<SDL_GetError();
        return -1;
    }

    SDL_HideWindow(window);
    SDL_PumpEvents();
    GlRenderer::uninit();
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

void PGE_Window::clean()
{
    if(window==NULL) return;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();
    glFlush();
    rePaint();
}

void PGE_Window::rePaint()
{
    if(window==NULL) return;
    SDL_GL_SwapWindow(window);
}

int PGE_Window::setFullScreen(bool fs)
{
    if(window==NULL) return -1;
    if(fs != IsFullScreen(window))
    {
        if(fs)
        {
            // Swith to FULLSCREEN mode
            if (SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) < 0)
            {
                //Hide mouse cursor in full screen mdoe
                qDebug() <<"Setting fullscreen failed : "<<SDL_GetError();
                return -1;
            }
            SDL_ShowCursor(SDL_DISABLE);
            return 1;
        }
        else
        {
            //Show mouse cursor
            if(showCursor)
                SDL_ShowCursor(SDL_ENABLE);

            // Swith to WINDOWED mode
            if (SDL_SetWindowFullscreen(window, SDL_FALSE) < 0)
            {
                qDebug() <<"Setting windowed failed : "<<SDL_GetError();
                return -1;
            }
            return 0;
        }
    }
    return 0;
}



SDL_bool PGE_Window::IsFullScreen(SDL_Window *win)
{
   Uint32 flags = SDL_GetWindowFlags(win);

   if(flags & SDL_WINDOW_FULLSCREEN_DESKTOP) return SDL_TRUE; // return SDL_TRUE if fullscreen

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
            qDebug() <<"Setting windowed failed : "<<SDL_GetError();
            return -1;
        }
        return 0;
    }

    // Swith to FULLSCREEN mode
    if (SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP) < 0)
    {
        //Hide mouse cursor in full screen mdoe
        qDebug() <<"Setting fullscreen failed : "<<SDL_GetError();
        return -1;
    }
    SDL_ShowCursor(SDL_DISABLE);
    return 1;
}



int PGE_Window::processEvents(SDL_Event &event)
{
    switch(event.type)
    {
        case SDL_WINDOWEVENT:
            if(event.window.event==SDL_WINDOWEVENT_RESIZED)
                GlRenderer::resetViewport();
            return 1;
        break;
    case SDL_KEYDOWN:
          switch(event.key.keysym.sym)
          {
              case SDLK_f:
                 if((event.key.keysym.mod&(KMOD_LCTRL|KMOD_RCTRL))!=0)
                 {
                    AppSettings.fullScreen=(PGE_Window::SDL_ToggleFS(PGE_Window::window)==1);
                    return 2;
                 }
              break;
              case SDLK_F2:
                  PGE_Window::showPhysicsDebug=!PGE_Window::showPhysicsDebug;
                  return 2;
              break;
              case SDLK_F3:
                  PGE_Window::showDebugInfo=!PGE_Window::showDebugInfo;
                  return 2;
              break;
              case SDLK_F12:
                  GlRenderer::makeShot();
                  return 2;
              break;
          }
        break;
    }
    return 0;
}
