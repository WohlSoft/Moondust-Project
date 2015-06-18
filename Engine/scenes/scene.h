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

#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_events.h>
#include <common_features/fader.h>
#include <common_features/rectf.h>
#include <script/lua_engine.h>

#include <functional>
#include <QList>

class Scene
{
    void construct();
public:
    void updateTickValue();

    enum TypeOfScene
    {
        _Unknown=0,
        Loading,
        Title,
        Level,
        World,
        Credits,
        GameOver
    };

    Scene();
    Scene(TypeOfScene _type);
    virtual ~Scene();
    virtual void onKeyInput(int key);             //!< Triggering when pressed game specific key
    virtual void onKeyboardPressed(SDL_Scancode scancode); //!< Triggering when pressed any key on keyboard
    virtual void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier); //!< Triggering when pressed any key on keyboard
    virtual void onKeyboardReleased(SDL_Scancode scancode); //!< Triggering when pressed any key on keyboard
    virtual void onKeyboardReleasedSDL(SDL_Keycode sdl_key, Uint16 modifier); //!< Triggering when pressed any key on keyboard
    virtual void onMouseMoved(SDL_MouseMotionEvent &mmevent);
    virtual void onMousePressed(SDL_MouseButtonEvent &mbevent);
    virtual void onMouseReleased(SDL_MouseButtonEvent &mbevent);
    virtual void onMouseWheel(SDL_MouseWheelEvent &wheelevent);
    virtual void processEvents();
    virtual LuaEngine* getLuaEngine();

    virtual void update();
    virtual void updateLua();
    virtual void render();
    virtual void renderMouse();
    virtual int exec(); //scene's loop
    TypeOfScene type();

    void addRenderFunction(const std::function<void()>& renderFunc);
    void clearRenderFunctions();

    virtual bool isVizibleOnScreen(PGE_RectF &rect);
    virtual bool isVizibleOnScreen(double x, double y, double w, double h);

    bool isExiting();
    bool doShutDown();
    /**************Fader**************/
    bool isOpacityFadding();
    void setFade(int speed, float target, float step);
    PGE_Fader fader;
    /**************Fader**************/

protected:
    bool        running;
    bool        _doShutDown;
    bool        doExit;
    int         uTick;
    float       uTickf;

    /************waiting timer************/
    void wait(float ms);
    /************waiting timer************/
private:
    TypeOfScene sceneType;
    float __waiting_step;

    QVector<std::function<void()> > renderFunctions;
};

#endif // SCENE_H
