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

#ifndef WINDOW_H
#define WINDOW_H

#undef main
#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>
#undef main

#include <QString>

class PGE_Window
{
public:
    static int Width;
    static int Height;
    static int MaxFPS;
    static bool vsync;
    static float TicksPerSecond;
    static int TimeOfFrame;
    static bool showDebugInfo;
    static bool showPhysicsDebug;
    static bool init(QString WindowTitle="Platformer Game Engine by Wohldtand");
    static void toggleVSync(bool vsync);
    static bool uninit();
    static bool isReady();
    static void setCursorVisibly(bool viz);
    static void clean();
    static void rePaint();

    static SDL_Window *window;
    static SDL_GLContext glcontext_background;
    static SDL_GLContext glcontext;
    static int setFullScreen(bool fs);
    static int SDL_ToggleFS(SDL_Window *win=NULL);

    static int processEvents(SDL_Event &event);

    static void checkSDLError(int line = -1);
private:
    static bool IsInit;
    static bool showCursor;
    static SDL_bool IsFullScreen(SDL_Window *win);
};

#endif // WINDOW_H
