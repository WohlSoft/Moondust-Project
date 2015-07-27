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

#include "scene_credits.h"
#include <graphics/gl_renderer.h>
#include <graphics/graphics.h>
#include <graphics/window.h>
#include <settings/global_settings.h>
#include <common_features/graphics_funcs.h>
#include <data_configs/config_manager.h>
#include <audio/pge_audio.h>

#include <QtDebug>


CreditsScene_misc_img::CreditsScene_misc_img()
{
    x=0;y=0;t.w=0;frmH=0;
}

CreditsScene_misc_img::~CreditsScene_misc_img()
{}

CreditsScene_misc_img::CreditsScene_misc_img(const CreditsScene_misc_img &im)
{
    x=im.x;
    y=im.y;
    t=im.t;
    a=im.a;
    frmH=im.frmH;
}


CreditsScene::CreditsScene() : Scene(Credits), luaEngine(this)
{
    _waitTimer=30000;
}

CreditsScene::~CreditsScene()
{
    glDisable(GL_TEXTURE_2D);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color
    //Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    GlRenderer::deleteTexture( background );

    for(int i=0;i<imgs.size();i++)
    {
        GlRenderer::deleteTexture(imgs[i].t);
    }
    imgs.clear();
}

void CreditsScene::init()
{
    /*****************************Load built-in stuff*******************************/
    glClearColor(float(ConfigManager::setup_CreditsScreen.backgroundColor.red())/255.0f,
                 float(ConfigManager::setup_CreditsScreen.backgroundColor.green())/255.0f,
                 float(ConfigManager::setup_CreditsScreen.backgroundColor.blue())/255.0f, 1.0f);
                // Set background color from file

    if(!ConfigManager::setup_CreditsScreen.backgroundImg.isEmpty())
        GlRenderer::loadTextureP(background, ConfigManager::setup_CreditsScreen.backgroundImg);
    else
        GlRenderer::loadTextureP(background, ":/images/cat_splash.png");
    imgs.clear();
    for(int i=0; i<ConfigManager::setup_CreditsScreen.AdditionalImages.size(); i++)
    {
        if(ConfigManager::setup_CreditsScreen.AdditionalImages[i].imgFile.isEmpty()) continue;

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
    luaEngine.setErrorReporterFunc([this](const QString& errorMessage, const QString& stacktrace){
        qWarning() << "Lua-Error: ";
        qWarning() << "Error Message: " << errorMessage;
        qWarning() << "Stacktrace: \n" << stacktrace;
        // Do not show error message box in credits
    });
    luaEngine.init();


    /*****************************Load LUA stuff*******************************/
}

void CreditsScene::setWaitTime(int time)
{
    if(time<=0)
        _waitTimer=30000;
    else
        _waitTimer=time;
}

void CreditsScene::exitFromScene()
{
    doExit=true;
    fader.setFade(10, 1.0f, 0.01f);
}

void CreditsScene::onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16)
{
    if(doExit) return;

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
    if(doExit)
    {
        if(fader.isFull())
        {
            running=false;
            return;
        }
    }

    /******************Update built-in faders and animators*********************/
    Scene::update();
    updateLua();
    for(int i=0;i<imgs.size(); i++)
        imgs[i].a.manualTick(uTickf);
    /******************Update built-in faders and animators*********************/

    /*****************************Update LUA stuff*******************************/

    //Note: loop function must accept int value which a time ticks loop step.
    // onLoop() <- Independent to time (for message boxes texts, catching events, etc.)
    // onLoop(int ticks) <- If need to sync with a loop time. (For animations, modeling scripts, etc.)


    /*****************************Update LUA stuff*******************************/

    if(!doExit)
    {
        if(_waitTimer>0)
            _waitTimer-=uTickf;
        else
            exitFromScene();
    }
}

void CreditsScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    GlRenderer::renderTexture(&background, PGE_Window::Width/2 - background.w/2, PGE_Window::Height/2 - background.h/2);

    for(int i=0;i<imgs.size();i++)
    {
        AniPos x(0,1); x = imgs[i].a.image();
        GlRenderer::renderTexture(&imgs[i].t,
                                  imgs[i].x,
                                  imgs[i].y,
                                  imgs[i].t.w,
                                  imgs[i].frmH, x.first, x.second);
    }

    Scene::render();
}

int CreditsScene::exec()
{
    //Level scene's Loop
    Uint32 start_render;
    doExit=false;

    while(running)
    {
        start_render=SDL_GetTicks();

        processEvents();
        update();
        render();
        glFlush();
        PGE_Window::rePaint();

        if( floor(uTickf) > (float)(SDL_GetTicks()-start_render))
        {
            wait( uTickf-(float)(SDL_GetTicks()-start_render) );
        }
    }
    return 0;
}

