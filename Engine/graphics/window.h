/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>

#include <QString>

#define SDLCHECKERROR() PGE_Window::checkSDLError(__FILE__, __LINE__, __FUNCTION__)

// Currently processing scene
class Scene;

namespace PGE_Window
{

//! Currently processing scene
extern Scene*   m_currentScene;
//! Width of world screen surface in pixels
extern int      Width;
//! Height of world screen surface in pixels
extern int      Height;
//! Is vertical synchronization is enabled
extern bool     vsync;
//! Is vertical synchronization is supported by this video driver
extern bool     vsyncIsSupported;
//! Framerate
extern double   TicksPerSecond;
//! Time of one frame rounded into integers
extern int      TimeOfFrame;
//! Enable printing of debug information
extern bool     showDebugInfo;
//! Enable rendering of physical engine debug shapes
extern bool     showPhysicsDebug;
//! Descriptor of the game window
extern SDL_Window       *window;
//! Descriptor of the OpenGL context
extern SDL_GLContext    glcontext;

/* Functions */
/**
 * @brief Create window with title and specified renderer type
 * @param WindowTitle Initial title of the window
 * @param renderType Type of renderer
 * @return
 */
bool init(std::string WindowTitle = "Platformer Game Engine by Wohldtand", int renderType = 0);

/**
 * @brief Change title of the window
 * @param title Title of window to change
 */
void setWindowTitle(std::string title);

/**
 * @brief Toggle Vertical synchronization mode
 * @param vsync State of vertical synchronization mode
 */
void toggleVSync(bool vsync);

/**
 * @brief Unload graphical subsystem and destroy window
 * @return true if everything successfully completed
 */
bool uninit();
bool isReady();
void setCursorVisibly(bool viz);
void clean();

int setFullScreen(bool fs);
int SDL_ToggleFS(SDL_Window *win = NULL);

int processEvents(SDL_Event &event);

bool isSdlError();
bool checkSDLError(const char *fn, int line, const char *func);
void printSDLWarn(QString info);
void printSDLError(QString info);

int  msgBoxInfo(QString title, QString text);
int  msgBoxWarning(QString title, QString text);
int  msgBoxCritical(QString title, QString text);
}//namespace PGE_Window

#endif // WINDOW_H
