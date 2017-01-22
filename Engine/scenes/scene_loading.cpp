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

#include <QtDebug>


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

    for(int i = 0; i < imgs.size(); i++)
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
                        ConfigManager::setup_LoadingScreen.updateDelay);
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

    D_pLogDebug("LoadingScene: Mouse pressed");
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

    for(int i = 0; i < imgs.size(); i++)
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

    for(int i = 0; i < imgs.size(); i++)
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

int LoadingScene::exec()
{
    m_doExit = false;
    LoopTiming times;
    times.start_common = SDL_GetTicks();
    bool frameSkip = g_AppSettings.frameSkip;
    PGE_Audio::playSoundByRole(obj_sound_role::Greeting);

    while(m_isRunning)
    {
        times.start_common = SDL_GetTicks();
        processEvents();
        update();
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
