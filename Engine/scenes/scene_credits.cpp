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

#include "scene_credits.h"
#include <graphics/gl_renderer.h>
#include <graphics/graphics.h>
#include <graphics/window.h>
#include <settings/global_settings.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>
#include <Utils/maths.h>

CreditsScene_misc_img::CreditsScene_misc_img()
{
    x = 0;
    y = 0;
    t.w = 0;
    frmH = 0;
}

CreditsScene_misc_img::~CreditsScene_misc_img()
{}

CreditsScene_misc_img::CreditsScene_misc_img(const CreditsScene_misc_img &im)
{
    x = im.x;
    y = im.y;
    t = im.t;
    a = im.a;
    frmH = im.frmH;
}


CreditsScene::CreditsScene() : Scene(Credits), luaEngine(this)
{
    _waitTimer = 30000;
}

CreditsScene::~CreditsScene()
{
    GlRenderer::clearScreen();
    GlRenderer::deleteTexture(background);

    for(auto &img : imgs)
        GlRenderer::deleteTexture(img.t);

    imgs.clear();
}

void CreditsScene::init()
{
    /*****************************Load built-in stuff*******************************/
    bgcolor.r = ConfigManager::setup_CreditsScreen.backgroundColor.Red();
    bgcolor.g = ConfigManager::setup_CreditsScreen.backgroundColor.Green();
    bgcolor.b = ConfigManager::setup_CreditsScreen.backgroundColor.Blue();

    if(!ConfigManager::setup_CreditsScreen.backgroundImg.empty())
        GlRenderer::loadTextureP(background, ConfigManager::setup_CreditsScreen.backgroundImg);
    else
        GlRenderer::loadTextureP(background, ":cat_splash.png");

    imgs.clear();

    for(size_t i = 0; i < ConfigManager::setup_CreditsScreen.AdditionalImages.size(); i++)
    {
        if(ConfigManager::setup_CreditsScreen.AdditionalImages[i].imgFile.empty())
            continue;

        CreditsScene_misc_img img;
        GlRenderer::loadTextureP(img.t, ConfigManager::setup_CreditsScreen.AdditionalImages[i].imgFile);
        img.x = ConfigManager::setup_CreditsScreen.AdditionalImages[i].x;
        img.y = ConfigManager::setup_CreditsScreen.AdditionalImages[i].y;
        img.a.construct(ConfigManager::setup_CreditsScreen.AdditionalImages[i].animated,
                        ConfigManager::setup_CreditsScreen.AdditionalImages[i].frames,
                        ConfigManager::setup_CreditsScreen.updateDelay);
        img.frmH = (img.t.h / ConfigManager::setup_CreditsScreen.AdditionalImages[i].frames);
        imgs.push_back(img);
    }

    /*****************************Load built-in stuff*end***************************/
    /*****************************Load LUA stuff*******************************/
    // onLoad() <- Gives ability to load/init custom stuff
    luaEngine.setLuaScriptPath(ConfigManager::PathScript());
    luaEngine.setCoreFile(":/script/maincore_credits.lua");
    luaEngine.setUserFile(ConfigManager::setup_CreditsScreen.luaFile);
    luaEngine.setErrorReporterFunc([](const std::string & errorMessage, const std::string  &stacktrace)
    {
        pLogWarning("Lua-Error: ");
        pLogWarning("Error Message: %s", errorMessage.c_str());
        pLogWarning("Stacktrace:\n%s", stacktrace.c_str());
        // Do not show error message box in credits
    });
    luaEngine.init();
    /*****************************Load LUA stuff*******************************/
}

void CreditsScene::setWaitTime(int time)
{
    if(time <= 0)
        _waitTimer = 30000;
    else
        _waitTimer = time;
}

void CreditsScene::exitFromScene()
{
    m_doExit = true;
    m_fader.setFade(10, 1.0, 0.01);
}

void CreditsScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(m_doExit) return;

    switch(sdl_key)
    {
    case SDLK_ESCAPE:
        exitFromScene();
        break;

    default:
        break;
    }
}

LuaEngine *CreditsScene::getLuaEngine()
{
    return &luaEngine;
}

void CreditsScene::update()
{
    if(m_doExit)
    {
        if(m_fader.isFull())
        {
            m_isRunning = false;
            return;
        }
    }

    /******************Update built-in faders and animators*********************/
    Scene::update();
    updateLua();

    for(auto &img : imgs)
        img.a.manualTick(uTickf);

    /******************Update built-in faders and animators*********************/

    /*****************************Update LUA stuff*******************************/

    //Note: loop function must accept int value which a time ticks loop step.
    // onLoop() <- Independent to time (for message boxes texts, catching events, etc.)
    // onLoop(int ticks) <- If need to sync with a loop time. (For animations, modeling scripts, etc.)

    /*****************************Update LUA stuff*******************************/

    if(!m_doExit)
    {
        if(_waitTimer > 0)
            _waitTimer -= uTickf;
        else
            exitFromScene();
    }
}

void CreditsScene::render()
{
    GlRenderer::clearScreen();
    GlRenderer::renderRect(0, 0, PGE_Window::Width, PGE_Window::Height, bgcolor.r, bgcolor.g, bgcolor.b, 1.0);
    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
    GlRenderer::renderTexture(&background, PGE_Window::Width / 2 - background.w / 2, PGE_Window::Height / 2 - background.h / 2);

    for(auto &img : imgs)
    {
        AniPos x(0, 1);
        x = img.a.image();
        GlRenderer::renderTexture(&img.t,
                                  img.x,
                                  img.y,
                                  img.t.w,
                                  img.frmH, x.first, x.second);
    }

    Scene::render();
}

int CreditsScene::exec()
{
    runVsyncValidator();

    m_doExit = false;
    LoopTiming times;
    times.start_common = SDL_GetTicks();
    bool frameSkip = g_AppSettings.frameSkip;
    GlRenderer::setClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background color

    while(m_isRunning)
    {
        times.start_common = SDL_GetTicks();

        while(times.doUpdate_physics < static_cast<double>(uTick))
        {
            processEvents();
            update();
            times.doUpdate_physics += uTickf;
            Maths::clearPrecision(times.doUpdate_physics);
        }
        times.doUpdate_physics -= static_cast<double>(uTick);
        Maths::clearPrecision(times.doUpdate_physics);

        times.stop_render = 0;
        times.start_render = 0;

        /**********************Process rendering of stuff****************************/
        if((PGE_Window::vsync) || (times.doUpdate_render <= 0.0))
        {
            times.start_render = SDL_GetTicks();
            /**********************Render everything***********************/
            render();
            GlRenderer::flush();
            GlRenderer::repaint();
            times.stop_render = SDL_GetTicks();
            times.doUpdate_render = frameSkip ? uTickf + (times.stop_render - times.start_render) : 0;
        }

        times.doUpdate_render -= uTickf;

        if(times.stop_render < times.start_render)
        {
            times.stop_render = 0;
            times.start_render = 0;
        }

        /****************************************************************************/

        if((!PGE_Window::vsync) && (uTick > times.passedCommonTime()))
            SDL_Delay(uTick - times.passedCommonTime());
    }

    return 0;
}
