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

#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_events.h>
#include <common_features/fader.h>
#include <common_features/rectf.h>
#include <script/lua_engine.h>
#include <scenes/_base/gfx_effect.h>
#include <scenes/_base/msgbox_queue.h>
#include <data_configs/spawn_effect_def.h>

#include <queue>
#include <vector>
#include <functional>

struct LoopTiming
{
    inline LoopTiming()
    {
        init();
    }

    inline void init()
    {
        start_render    = 0;
        stop_render     = 0;
        doUpdate_render = 0.0;
        doUpdate_physics = 0.0;
        start_physics   = 0;
        stop_physics    = 0;
        start_events    = 0;
        stop_events     = 0;
        start_common    = 0;
    }

    inline Uint32 passedCommonTime()
    {
        return SDL_GetTicks() - start_common;
    }

    Uint32 start_render;
    Uint32 stop_render;
    double doUpdate_render;
    double doUpdate_physics;

    Uint32 start_physics;
    Uint32 stop_physics;

    Uint32 start_events;
    Uint32 stop_events;

    Uint32 start_common;
};

class Scene
{
    void construct();
public:
    void updateTickValue();

    double frameDelay();

    enum TypeOfScene
    {
        _Unknown = 0,
        ConfigSelect,
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
    virtual LuaEngine *getLuaEngine();

    virtual void update();
    virtual void updateLua();
    virtual void render();
    virtual void renderMouse();
    virtual int exec(); //scene's loop
    void runVsyncValidator();
    TypeOfScene type();

    struct RenderFuncs
    {
        typedef std::function<void(double, double)> Function;
        long double z_index;
        Function    render;
    };

protected:
    std::vector<RenderFuncs> luaRenders;

public:
    void renderArrayAddFunction(const RenderFuncs::Function &renderFunc, long double zIndex = 400.0L);
    void renderArrayPrepare();
    void renderArrayClear();

    virtual bool isVizibleOnScreen(PGE_RectF &rect);
    virtual bool isVizibleOnScreen(double x, double y, double w, double h);

    bool isExiting();
    bool doShutDown();
    /**************Fader**************/
    bool isOpacityFadding();
    void setFade(int speed, float target, float step);
    PGE_Fader m_fader;
    /**************Fader**************/

    /*  Effects engine   */
    typedef std::vector<Scene_Effect>    SceneEffectsArray;
    SceneEffectsArray  WorkingEffects;
    ///
    /// \brief launchStaticEffect
    /// Starts static effect by ID at position X,Y relative to left-top corner of effect picture
    /// \param effectID ID of effect from lvl_effects.ini
    /// \param startX X position relative to left side of effect picture
    /// \param startY Y position relative to top side of effect picture
    /// \param animationLoops Number of loops before effect will be finished. 0 - unlimited while time is not exited
    /// \param delay max time of effect working. 0 - unlimited while loops are not exited or while effect still vizible of screen.
    /// \param velocityX Horizontal motion speed (pixels per 1/65 second [independent to framerate])
    /// \param velocityY Vertical motion speed (pixels per 1/65 second [independent to framerate])
    /// \param gravity Y-gravitation will cause falling of effect picture
    /// \param phys Additional physical settings
    ///
    void  launchEffect(unsigned long effectID, double startX, double startY, int animationLoops, int delay, double velocityX, double velocityY, double gravity, int direction = 0, Scene_Effect_Phys phys = Scene_Effect_Phys());

    ///
    /// \brief launchStaticEffectC
    /// Starts static effect by ID at position X,Y relative to center of effect picture
    /// \param effectID ID of effect from lvl_effects.ini
    /// \param startX X position relative to center of effect picture
    /// \param startY Y position relative to center of effect picture
    /// \param animationLoops Number of loops before effect will be finished. 0 - unlimited while time is not exited
    /// \param delay max time of effect working. 0 - unlimited while loops are not exited or while effect still vizible of screen.
    /// \param velocityX Horizontal motion speed (pixels per 1/65 second [independent to framerate])
    /// \param velocityY Vertical motion speed (pixels per 1/65 second [independent to framerate])
    /// \param gravity Y-gravitation will cause falling of effect picture
    /// \param phys Additional physical settings
    ///
    void launchStaticEffectC(unsigned long effectID, double startX, double startY, int animationLoops, int delay, double velocityX, double velocityY, double gravity, int direction = 0, Scene_Effect_Phys phys = Scene_Effect_Phys());

    void launchEffect(const SpawnEffectDef &effect_def, bool centered = false);

    void processEffects(double ticks);
    /*  Effects engine   */

    std::string errorString();

    //! Queue of message boxes to show them after all code of one frame will be updated/processed
    MessageBoxQueue m_messages;

protected:
    bool        m_isRunning;
    bool        m_doShutDown;
    bool        m_doExit;
    uint32_t    uTick;
    double      uTickf;
    LoopTiming  times;

    std::string _errorString;
private:
    TypeOfScene sceneType;
};

#endif // SCENE_H
