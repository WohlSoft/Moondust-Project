/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "scene.h"
#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <graphics/vsync_validator.h>
#include <Utils/maths.h>

#include <script/lua_event.h>
#include <script/bindings/core/events/luaevents_core_engine.h>

void Scene::construct()
{
    if(PGE_Window::m_currentScene == nullptr)
        PGE_Window::m_currentScene = this;//Register this scene as current global

    m_messages.m_scene = this;
    m_fader.setFull();
    m_fader.setFade(10, 0.0, 0.02); //!< Fade in scene when it was started
    m_isRunning = true;
    m_doExit = false;
    m_doShutDown = false;
    updateTickValue();
}

void Scene::updateTickValue()
{
    //uTickf = Maths::roundToDown(PGE_Window::frameDelay, 0.4875);//Experimentally
    uTickf = Maths::roundToDown(PGE_Window::frameDelay, 0.480769231);
    Maths::clearPrecision(uTickf);
    //static_cast<double>(PGE_Window::TimeOfFrame);//1000.0f/(float)PGE_Window::TicksPerSecond;
    uTick  = static_cast<uint32_t>(std::ceil(uTickf));

    constexpr double tickDelay = 1000.0 / 65;

    if(uTickf > tickDelay)
    {
        uTick = static_cast<Uint32>(PGE_Window::frameDelay);
        while(uTickf > tickDelay)
            uTickf /= 2.0;
        uTickf = Maths::roundToDown(uTickf, 0.480769231);
        Maths::clearPrecision(uTickf);
    }

    if(uTick == 0)
        uTick = 1;
    if(uTickf <= 0.0)
        uTickf = 1.0;
    SDL_assert(uTick < 2000u);//"uTick Must be less than two seconds!!!"
}

double Scene::frameDelay()
{
    return uTickf;
}

Scene::Scene()
{
    sceneType = _Unknown;
    construct();
}

Scene::Scene(TypeOfScene _type)
{
    sceneType = _type;
    construct();
}

Scene::~Scene()
{
    GlRenderer::clearScreen();
    //Reset scene pointer only if this scene was current
    if(PGE_Window::m_currentScene == this)
        PGE_Window::m_currentScene = nullptr;
}

void Scene::onKeyInput(int)
{}

void Scene::onKeyboardPressed(SDL_Scancode)
{}

void Scene::onKeyboardPressedSDL(SDL_Keycode, Uint16)
{}

void Scene::onKeyboardReleased(SDL_Scancode)
{}

void Scene::onKeyboardReleasedSDL(SDL_Keycode, Uint16)
{}

void Scene::onMouseMoved(SDL_MouseMotionEvent &)
{}

void Scene::onMousePressed(SDL_MouseButtonEvent &)
{}

void Scene::onMouseReleased(SDL_MouseButtonEvent &)
{}

void Scene::onMouseWheel(SDL_MouseWheelEvent &)
{}


void Scene::processEvents()
{
    SDL_Event event; //  Events of SDL

    while(SDL_PollEvent(&event))
    {
        if(PGE_Window::processEvents(event) != 0)
            continue;

        switch(event.type)
        {
        case SDL_QUIT:
        {
            m_doExit    = true;
            m_isRunning = false;
            m_doShutDown = true;
            break;
        }// End work of program

        case SDL_KEYDOWN: // If pressed key
            onKeyboardPressedSDL(event.key.keysym.sym, event.key.keysym.mod);
            onKeyboardPressed(event.key.keysym.scancode);
            break;

        case SDL_KEYUP: // If released key
            onKeyboardReleasedSDL(event.key.keysym.sym, event.key.keysym.mod);
            onKeyboardReleased(event.key.keysym.scancode);
            break;

        case SDL_MOUSEBUTTONDOWN:
            onMousePressed(event.button);
            break;

        case SDL_MOUSEBUTTONUP:
            onMouseReleased(event.button);
            break;

        case SDL_MOUSEWHEEL:
            onMouseWheel(event.wheel);
            break;

        case SDL_MOUSEMOTION:
            onMouseMoved(event.motion);
            break;
        }
    }
}

LuaEngine *Scene::getLuaEngine()
{
    return nullptr;
}

void Scene::update()
{
    m_fader.tickFader(uTickf);
}

void Scene::updateLua()
{
    LuaEngine *sceneLuaEngine = getLuaEngine();
    renderArrayClear();//Clean up last rendered stuff

    if(sceneLuaEngine)
    {
        if(sceneLuaEngine->isValid() && !sceneLuaEngine->shouldShutdown())
        {
            //sceneLuaEngine->runGarbageCollector();
            LuaEvent loopEvent = BindingCore_Events_Engine::createLoopEvent(sceneLuaEngine, uTickf);
            sceneLuaEngine->dispatchEvent(loopEvent);
        }
    }
}

void Scene::render()
{
    for(RenderFuncs &rf : luaRenders)
        (rf.render)(0.0, 0.0);

    if(!m_fader.isNull())
        GlRenderer::renderRect(0, 0,
                               PGE_Window::Width,
                               PGE_Window::Height,
                               0.f, 0.f,
                               0.f, static_cast<float>(m_fader.fadeRatio()));
}

void Scene::renderMouse()
{}

int Scene::exec()
{
    return 0;
}

void Scene::runVsyncValidator()
{
    if(!PGE_Window::vsync)
        return; // Do nothing, VSync is disabled

    auto vSyncProbe = VSyncValidator(this, PGE_Window::frameDelay);
    while(!vSyncProbe.isComplete())
    {
        GlRenderer::clearScreen();
        for(int i = 0; i < 10000; i++)
        {
            GlRenderer::renderRect(1.0f * float(i), 1.0f * float(i), 12.0f, 12.0f,
                                   0.0f, 0.0f, 0.0f, 1.0f * float(i)/100.0f);
        }
        GlRenderer::flush();
        GlRenderer::repaint();
        vSyncProbe.update();
    }
}

Scene::TypeOfScene Scene::type()
{
    return sceneType;
}

void Scene::renderArrayAddFunction(const RenderFuncs::Function &renderFunc, long double zIndex)
{
    luaRenders.push_back(RenderFuncs{zIndex, renderFunc});
}

static bool compareZIndeces(const Scene::RenderFuncs &i, const Scene::RenderFuncs &j)
{
    return (i.z_index < j.z_index);
}

void Scene::renderArrayPrepare()
{
    std::stable_sort(luaRenders.begin(), luaRenders.end(), compareZIndeces);
}

void Scene::renderArrayClear()
{
    luaRenders.clear();
}

bool Scene::isVizibleOnScreen(PGE_RectF &rect)
{
    PGE_RectF screen(0, 0, PGE_Window::Width, PGE_Window::Height);
    return screen.collideRect(rect);
}

bool Scene::isVizibleOnScreen(double x, double y, double w, double h)
{
    PGE_RectF screen(0, 0, PGE_Window::Width, PGE_Window::Height);
    return screen.collideRect(x, y, w, h);
}

bool Scene::isExiting()
{
    return m_doExit;
}

bool Scene::doShutDown()
{
    return m_doShutDown;
}

/**************************Fader*******************************/
bool Scene::isOpacityFadding()
{
    return m_fader.isFading();
}

void Scene::setFade(int speed, float target, float step)
{
    m_fader.setFade(speed, target, step);
}
/**************************Fader**end**************************/

std::string Scene::errorString()
{
    return _errorString;
}
