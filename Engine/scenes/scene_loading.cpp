/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "scene_loading.h"
#include <graphics/gl_renderer.h>
#include <graphics/graphics.h>
#include <graphics/window.h>
#include <gui/pge_msgbox.h>
#include <settings/global_settings.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>
#include <Utils/maths.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

LoadingScene_misc_img::LoadingScene_misc_img()
{
    x = 0;
    y = 0;
    t.w = 0;
    frmH = 0;
}

LoadingScene_misc_img::LoadingScene_misc_img(const LoadingScene_misc_img &im)
{
    x = im.x;
    y = im.y;
    t = im.t;
    a = im.a;
    frmH = im.frmH;
}


LoadingScene::LoadingScene() : Scene(Loading)
{
    _waitTimer = 5000;
}

LoadingScene::~LoadingScene()
{
    //Black background color
    GlRenderer::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Clear screen
    GlRenderer::clearScreen();
    GlRenderer::deleteTexture(background);

    for(auto &img : imgs)
        GlRenderer::deleteTexture(img.t);

    imgs.clear();
}

void LoadingScene::init()
{
    bgcolor.r = ConfigManager::setup_LoadingScreen.backgroundColor.Red();
    bgcolor.g = ConfigManager::setup_LoadingScreen.backgroundColor.Green();
    bgcolor.b = ConfigManager::setup_LoadingScreen.backgroundColor.Blue();

    if(!ConfigManager::setup_LoadingScreen.backgroundImg.empty())
        GlRenderer::loadTextureP(background, ConfigManager::setup_LoadingScreen.backgroundImg);
    else
        GlRenderer::loadTextureP(background, std::string(":cat_splash.png"));

    imgs.clear();

    for(LoadingScreenAdditionalImage &simg : ConfigManager::setup_LoadingScreen.AdditionalImages)
    {
        if(simg.imgFile.empty())
            continue;

        LoadingScene_misc_img img;
        GlRenderer::loadTextureP(img.t, simg.imgFile);
        img.x = simg.x;
        img.y = simg.y;
        img.a.construct(simg.animated,
                        simg.frames,
                        int(simg.frameDelay));
        img.frmH = (img.t.h / simg.frames);
        imgs.push_back(img);
    }
}

void LoadingScene::setWaitTime(int time)
{
    if(time <= 0)
        _waitTimer = 5000;
    else
        _waitTimer = time;
}

void LoadingScene::exitFromScene()
{
    m_doExit = true;
    m_fader.setFade(10, 1.0, 0.01);
}

void LoadingScene::onKeyboardPressedSDL(SDL_Keycode code, Uint16)
{
    if(m_doExit) return;

    if((code == SDLK_LCTRL) || (code == SDLK_RCTRL)) return;

    if(code == SDLK_f)
        return;

    if(code == SDLK_NUMLOCKCLEAR)
        return;

    if(code == SDLK_CAPSLOCK)
        return;

    if(code == SDLK_SCROLLLOCK)
        return;

    D_pLogDebug("LoadingScene: key pressed %d", SDL_GetKeyName(code));
    exitFromScene();
}

void LoadingScene::onMousePressed(SDL_MouseButtonEvent &)
{
    if(m_doExit) return;

    D_pLogDebugNA("LoadingScene: Mouse pressed");
    exitFromScene();
}

void LoadingScene::update()
{
    if(m_doExit)
    {
        if(m_fader.isFull())
        {
            m_isRunning = false;
            return;
        }
    }

    Scene::update();

    for(auto &img : imgs)
        img.a.manualTick(uTickf);

    if(!m_doExit)
    {
        if(_waitTimer > 0)
            _waitTimer -= uTickf;
        else
            exitFromScene();
    }
}

void LoadingScene::render()
{
    GlRenderer::clearScreen();
    GlRenderer::renderRect(0, 0, PGE_Window::Width, PGE_Window::Height, bgcolor.r, bgcolor.g, bgcolor.b, 1.0);
    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
    GlRenderer::renderTexture(&background, PGE_Window::Width / 2 - background.w / 2, PGE_Window::Height / 2 - background.h / 2);

    for (auto &img : imgs)
    {
        AniPos x(0, 1);
        x = img.a.image();
        GlRenderer::renderTexture(&img.t,
                                  img.x,
                                  img.y,
                                  img.t.w,
                                  img.frmH,
                                  static_cast<float>(x.first),
                                  static_cast<float>(x.second));
    }

    Scene::render();
}

int LoadingScene::exec()
{
    runVsyncValidator();

    m_doExit = false;
    times.init();
    times.start_common = SDL_GetTicks();
    m_gfx_frameSkip = g_AppSettings.frameSkip;
    PGE_Audio::playSoundByRole(obj_sound_role::Greeting);

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
            times.doUpdate_render = m_gfx_frameSkip ? uTickf + (times.stop_render - times.start_render) : 0;
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
