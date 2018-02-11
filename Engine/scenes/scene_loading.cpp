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

#include "scene_loading.h"
#include <graphics/gl_renderer.h>
#include <graphics/graphics.h>
#include <graphics/window.h>
#include <settings/global_settings.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>

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

    for(size_t i = 0; i < imgs.size(); i++)
        GlRenderer::deleteTexture(imgs[i].t);

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

    for(size_t i = 0; i < imgs.size(); i++)
        imgs[i].a.manualTick(uTickf);

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

    for(size_t i = 0; i < imgs.size(); i++)
    {
        AniPos x(0, 1);
        x = imgs[i].a.image();
        GlRenderer::renderTexture(&imgs[i].t,
                                  imgs[i].x,
                                  imgs[i].y,
                                  imgs[i].t.w,
                                  imgs[i].frmH,
                                  static_cast<float>(x.first),
                                  static_cast<float>(x.second));
    }

    Scene::render();
}

void loadingSceneLoopStep(void *scene)
{
    LoadingScene* s = reinterpret_cast<LoadingScene*>(scene);
    s->times.start_common = SDL_GetTicks();
    s->processEvents();
    s->update();
    s->times.stop_render = 0;
    s->times.start_render = 0;

    /**********************Process rendering of stuff****************************/
    if((PGE_Window::vsync) || (s->times.doUpdate_render <= 0.0))
    {
        s->times.start_render = SDL_GetTicks();
        /**********************Render everything***********************/
        s->render();
        GlRenderer::flush();
        GlRenderer::repaint();
        s->times.stop_render = SDL_GetTicks();
        s->times.doUpdate_render = s->m_gfx_frameSkip ? s->uTickf + (s->times.stop_render - s->times.start_render) : 0;
    }

    s->times.doUpdate_render -= s->uTickf;

    if(s->times.stop_render < s->times.start_render)
    {
        s->times.stop_render = 0;
        s->times.start_render = 0;
    }

    /****************************************************************************/
    #ifndef __EMSCRIPTEN__
    if((!PGE_Window::vsync) && (s->uTick > s->times.passedCommonTime()))
        SDL_Delay(s->uTick - s->times.passedCommonTime());
    #else
    if(!s->m_isRunning)
    {
        emscripten_cancel_main_loop();
        pLogDebug("Exit from loop triggered");
    }
    #endif
}

int LoadingScene::exec()
{
    m_doExit = false;
    times.init();
    times.start_common = SDL_GetTicks();
    m_gfx_frameSkip = g_AppSettings.frameSkip;
    PGE_Audio::playSoundByRole(obj_sound_role::Greeting);

    #ifndef __EMSCRIPTEN__
    while(m_isRunning)
        loadingSceneLoopStep(this);
    #else
    pLogDebug("Main loop started");
    emscripten_set_main_loop_arg(loadingSceneLoopStep, this, -1, 1);
    pLogDebug("Main loop finished");
    #endif

    return 0;
}
