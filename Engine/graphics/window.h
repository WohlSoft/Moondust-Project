/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>

#include <string>

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
extern double   frameRate;
//! Time of one frame rounded into integers
extern int      frameDelay;
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
 * @brief Change size of internal resolution
 * @param newWidth New width in pixels
 * @param newHeight New height in pixels
 */
void changeViewportResolution(unsigned int newWidth, unsigned int newHeight);

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
int SDL_ToggleFS(SDL_Window *win = nullptr);

int processEvents(SDL_Event &event);

bool isSdlError();
bool checkSDLError(const char *fn, int line, const char *func);
void printSDLWarn(std::string info);
void printSDLError(std::string info);

int  msgBoxInfo(std::string title, std::string text);
int  msgBoxWarning(std::string title, std::string text);
int  msgBoxCritical(std::string title, std::string text);
}//namespace PGE_Window

#endif // WINDOW_H
