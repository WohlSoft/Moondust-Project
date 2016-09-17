/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>

#include <settings/global_settings.h>
#include <gui/pge_msgbox.h>
#include "gl_renderer.h"

#include <QMessageBox>
#include <QtDebug>

int PGE_Window::Width=800;
int PGE_Window::Height=600;

int PGE_Window::MaxFPS=250;
float PGE_Window::TicksPerSecond=1000.0f/15.0f;
int PGE_Window::TimeOfFrame=15;
bool PGE_Window::vsync=true;
bool PGE_Window::vsyncIsSupported=true;

bool PGE_Window::showDebugInfo=false;
bool PGE_Window::showPhysicsDebug=false;

SDL_Window *PGE_Window::window;
SDL_GLContext PGE_Window::glcontext_background;
SDL_GLContext PGE_Window::glcontext;

bool PGE_Window::IsInit=false;
bool PGE_Window::showCursor=true;

bool PGE_Window::isSdlError()
{
    const char *error = SDL_GetError();
    return (*error != '\0');
}

bool PGE_Window::checkSDLError(const char* fn, int line, const char* func)
{
    const char *error = SDL_GetError();
    if (*error != '\0')
    {
        PGE_MsgBox::warn( QString("SDL Error: %1\nFile: %2\nFunction: %3\nLine: %4")
                          .arg(error)
                          .arg(fn)
                          .arg(func)
                          .arg(line));
        SDL_ClearError();
        return true;
    }
    return false;
}

void PGE_Window::printSDLWarn(QString info)
{
    PGE_MsgBox::warn( QString("%1\nSDL Error: %2")
                              .arg(info)
                              .arg( SDL_GetError() )
                      );
}

void PGE_Window::printSDLError(QString info)
{
    PGE_MsgBox::error( QString("%1\nSDL Error: %2")
                              .arg(info)
                              .arg( SDL_GetError() )
                      );
}

bool PGE_Window::init(QString WindowTitle, int renderType)
{

    #if 0 //For testing! Change 0 to 1 and unommend one of GL Renderers to debug one specific renderer!
    GlRenderer::setup_SW_SDL();
    //GlRenderer::setup_OpenGL21();
    //GlRenderer::setup_OpenGL31();
    #else
    //Detect renderer
    GlRenderer::RenderEngineType rtype = GlRenderer::setRenderer( (GlRenderer::RenderEngineType)renderType );
    if( rtype == GlRenderer::RENDER_INVALID )
    {
                //% "Unable to find OpenGL support!\nSoftware renderer will be started.\n"
        printSDLWarn( qtTrId("RENDERER_NO_OPENGL_ERROR") );
        SDL_ClearError();
        rtype = GlRenderer::RENDER_SW_SDL;
    }

    switch(rtype)
    {
        case GlRenderer::RENDER_OPENGL_3_1: GlRenderer::setup_OpenGL31(); break;
        case GlRenderer::RENDER_OPENGL_2_1: GlRenderer::setup_OpenGL21(); break;
        case GlRenderer::RENDER_SW_SDL:     GlRenderer::setup_SW_SDL(); break;
        default:
                        //% "Renderer is not selected!"
            printSDLError( qtTrId("NO_RENDERER_ERROR") );
            return false;
    }
    #endif

    GlRenderer::setViewportSize(Width, Height);
    window = SDL_CreateWindow( WindowTitle.toStdString().c_str(),
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               Width, Height,
                               SDL_WINDOW_RESIZABLE|
                               SDL_WINDOW_HIDDEN|
                               GlRenderer::SDL_InitFlags() );
    if( window == NULL )
    {
                    //% "Unable to create window!"
        printSDLError( qtTrId("WINDOW_CREATE_ERROR") );
        SDL_ClearError();
        return false;
    }

    if( isSdlError() )
    {
                    //% "Unable to create window!"
        printSDLError( qtTrId("WINDOW_CREATE_ERROR") );
        SDL_ClearError();
        return false;
    }

    SDL_SetWindowMinimumSize(window, Width, Height);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    GraphicsHelps::initFreeImage();

#ifdef _WIN32
    FIBITMAP* img[2];
    img[0] = GraphicsHelps::loadImageRC(":/icon/cat_16.png");
    img[1] = GraphicsHelps::loadImageRC(":/icon/cat_32.png");
    if(!GraphicsHelps::setWindowIcon(window, img[0], 16))
    {
                        //% "Unable to setup window icon!"
        printSDLWarn( qtTrId("WINDOW_ICON_INIT_ERROR") );
        SDL_ClearError();
    }
    if(!GraphicsHelps::setWindowIcon(window, img[1], 32))
    {
                //% "Unable to setup window icon!"
        printSDLWarn( qtTrId("WINDOW_ICON_INIT_ERROR") );
        SDL_ClearError();
    }
    GraphicsHelps::closeImage(img[0]);
    GraphicsHelps::closeImage(img[1]);
#else//IF _WIN32
    FIBITMAP* img;
    #ifdef Q_OS_MACX
    img = GraphicsHelps::loadImageRC(":/icon/cat_256.png");
    #else
    img = GraphicsHelps::loadImageRC(":/icon/cat_16.png");
    #endif
    if(img)
    {
        SDL_Surface *sicon = GraphicsHelps::fi2sdl(img);
        SDL_SetWindowIcon(window, sicon);
        GraphicsHelps::closeImage(img);
        SDL_FreeSurface(sicon);
        if( isSdlError() )
        {
                       //% "Unable to setup window icon!"
            printSDLWarn( qtTrId("WINDOW_ICON_INIT_ERROR") );
            SDL_ClearError();
        }
    }
#endif//IF _WIN32 #else
    IsInit=true;

    //Init OpenGL (to work with textures, OpenGL should be load)
    LogDebug("Init OpenGL settings...");
    if( !GlRenderer::init() )
    {
                   //% "Unable to initialize renderer context!"
        printSDLError( qtTrId("RENDERER_CONTEXT_INIT_ERROR") );
        SDL_ClearError();
        IsInit=false;
        return false;
    }

    LogDebug( "Toggle vsync..." );
    vsyncIsSupported = (SDL_GL_SetSwapInterval(1) == 0);
    toggleVSync(vsync);
    LogDebug( QString("V-Sync supported: %1").arg(vsyncIsSupported) );

    return true;
}

void PGE_Window::toggleVSync(bool vsync)
{
   if(vsync)
   {
       int display_count = 0, display_index = 0;
       SDL_DisplayMode mode = { SDL_PIXELFORMAT_UNKNOWN, 0, 0, 0, 0 };
       if ((display_count = SDL_GetNumVideoDisplays()) < 1) {
           LogWarning(QString("SDL_GetNumVideoDisplays returned: %1").arg(display_count));
           return;
       }
       if (SDL_GetCurrentDisplayMode(display_index, &mode) != 0) {
           LogWarning(QString("SDL_GetDisplayMode failed: %1").arg(SDL_GetError()));
           return;
       }
       //SDL_Log("SDL_GetDisplayMode(0, 0, &mode):\t\t%i bpp\t%i x %i",
       //SDL_BITSPERPIXEL(mode.format), mode.w, mode.h);
       //const char *error = SDL_GetError();
       if( SDL_GL_SetSwapInterval(1) == 0 )
       {
           //Vertical syncronization is supported
           vsyncIsSupported = true;

           TimeOfFrame = ceil(1000.f/float(mode.refresh_rate));
           TicksPerSecond=1000.0f/TimeOfFrame;

           g_AppSettings.timeOfFrame=TimeOfFrame;
           g_AppSettings.TicksPerSecond=TicksPerSecond;
           SDL_ClearError();
       } else {
           //Vertical syncronization is NOT supported
           vsyncIsSupported = false;

           TimeOfFrame=g_AppSettings.timeOfFrame;
           TicksPerSecond=g_AppSettings.TicksPerSecond;

           //Disable vertical syncronization because unsupported
           g_AppSettings.vsync=false;
           PGE_Window::vsync=false;
           SDL_GL_SetSwapInterval(0);
           SDL_ClearError();
       }
   } else {
       SDL_GL_SetSwapInterval(0);
   }
}


bool PGE_Window::uninit()
{
    // Swith to WINDOWED mode
    if (SDL_SetWindowFullscreen(window, SDL_FALSE) < 0)
    {
        LogWarning(QString("Setting windowed failed: ") + SDL_GetError ());
        return false;
    }
    SDL_HideWindow( window );
    GlRenderer::uninit();
    GraphicsHelps::closeFreeImage();
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
    GlRenderer::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GlRenderer::clearScreen();
    GlRenderer::flush();
    GlRenderer::repaint();
}

int PGE_Window::setFullScreen(bool fs)
{
    if(window==NULL) return -1;
    if(fs != IsFullScreen(window))
    {
        if(fs)
        {
            // Swith to FULLSCREEN mode
            if( SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) < 0 )
            {
                //Hide mouse cursor in full screen mdoe
                LogWarning(QString ("Setting fullscreen failed: ") + SDL_GetError() );
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
            if( SDL_SetWindowFullscreen(window, SDL_FALSE) < 0 )
            {
                LogWarning(QString ("Setting windowed failed: ") + SDL_GetError() );
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
   return ( flags & SDL_WINDOW_FULLSCREEN_DESKTOP ) ? SDL_TRUE : SDL_FALSE;
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
            LogWarning(QString ("Setting windowed failed: ") + SDL_GetError() );
            return -1;
        }
        return 0;
    }

    // Swith to FULLSCREEN mode
    if (SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP) < 0)
    {
        //Hide mouse cursor in full screen mdoe
        LogWarning(QString ("Setting fullscreen failed: ") + SDL_GetError() );
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
        {
            if(event.window.event == SDL_WINDOWEVENT_RESIZED)
                GlRenderer::resetViewport();
            return 1;
        }
    case SDL_KEYDOWN:
        switch(event.key.keysym.sym)
        {
            case SDLK_f:
            if((event.key.keysym.mod&(KMOD_LCTRL|KMOD_RCTRL))!=0)
            {
                g_AppSettings.fullScreen=(PGE_Window::SDL_ToggleFS(PGE_Window::window)==1);
                return 2;
            }
            break;
            #ifdef PANIC_KEY //Panic! (If you wanna have able to quickly close game
                             //        from employer - add "DEFINES+=PANIC_KEY" into qmake args
                             //        and then you can press NumPad + to instantly close game)
            case SDLK_KP_PLUS:
            {
                SDL_DestroyWindow(window);
                SDL_CloseAudio();
                exit(EXIT_FAILURE);
                return 2;
            }
            #endif
            case SDLK_F2:
            {
                PGE_Window::showPhysicsDebug = !PGE_Window::showPhysicsDebug;
                return 2;
            }
            case SDLK_F3:
            {
                PGE_Window::showDebugInfo = !PGE_Window::showDebugInfo;
                return 2;
            }
            case SDLK_F11:
            {
                GlRenderer::toggleRecorder();
                return 2;
            }
            case SDLK_F12:
            {
                GlRenderer::makeShot();
                return 2;
            }
        }
        break;
    }
    return 0;
}
