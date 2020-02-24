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

#include "../scene_level.h"
#include "../../common_features/simple_animator.h"
#include "../../common_features/graphics_funcs.h"

#include <fontman/font_manager.h>
#include <networking/intproc.h>
#include <graphics/gl_renderer.h>

/**************************LoadAnimation*******************************/
namespace lvl_scene_loader
{
    static SimpleAnimator *loading_Ani = nullptr;
    static PGE_Texture loading_texture;
}

void LevelScene::drawLoader()
{
    using namespace lvl_scene_loader;

    if(!loading_Ani)
        return;

    GlRenderer::clearScreen();
    GlRenderer::renderRect(0, 0, PGE_Window::Width, PGE_Window::Height, 0.f, 0.f, 0.f, 1.0f);
    PGE_RectF loadAniG;
    loadAniG.setRect(PGE_Window::Width / 2 - loading_texture.w / 2,
                     PGE_Window::Height / 2 - (loading_texture.h / 4) / 2,
                     loading_texture.w,
                     loading_texture.h / 4);
    GlRenderer::resetViewport();
    AniPos x(0, 1);
    x = loading_Ani->image();
    GlRenderer::renderTexture(&loading_texture,
                              static_cast<float>(loadAniG.left()),
                              static_cast<float>(loadAniG.top()),
                              static_cast<float>(loadAniG.width()),
                              static_cast<float>(loadAniG.height()),
                              static_cast<float>(x.first),
                              static_cast<float>(x.second));

    if(IntProc::isEnabled())
        FontManager::printText(IntProc::getState(), 10, 10);
}


void LevelScene::setLoaderAnimation(int speed)
{
    using namespace lvl_scene_loader;
    m_loaderSpeed = speed;

    if(IntProc::isEnabled())
    {
        if(!loading_texture.inited)
            GlRenderer::loadTextureP(loading_texture, ":coin.png");
    }
    else
    {
        if(!loading_texture.inited)
            GlRenderer::loadTextureP(loading_texture, ":shell.png");
    }

    loading_Ani = new SimpleAnimator(true,
                                     4,
                                     128,
                                     0, -1, false, false);
    loading_Ani->start();
    m_loader_timer_id = SDL_AddTimer(static_cast<Uint32>(speed), &LevelScene::nextLoadAniFrame, this);
    m_loaderIsWorks = true;
}

void LevelScene::stopLoaderAnimation()
{
    using namespace lvl_scene_loader;
    m_loaderDoStep = false;
    m_loaderIsWorks = false;
    SDL_RemoveTimer(m_loader_timer_id);
    render();

    if(loading_Ani)
    {
        loading_Ani->stop();
        delete loading_Ani;
        loading_Ani = NULL;
    }
}

void LevelScene::destroyLoaderTexture()
{
    using namespace lvl_scene_loader;
    GlRenderer::deleteTexture(loading_texture);
}

unsigned int LevelScene::nextLoadAniFrame(unsigned int x, void *p)
{
    (void)(x);
    LevelScene *self = reinterpret_cast<LevelScene *>(p);
    self->loaderTick();
    return 0;
}

void LevelScene::loaderTick()
{
    m_loaderDoStep = true;
}

void LevelScene::loaderStep()
{
    if(!m_loaderIsWorks)
        return;

    if(!m_loaderDoStep)
        return;

    SDL_Event event; //  Events of SDL

    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);

        switch(event.type)
        {
        case SDL_QUIT:
            //Give able to quit from game even loading process is not finished
            m_isLevelContinues = false;
            m_doExit = true;
            break;
        }
    }

    drawLoader();
    GlRenderer::flush();
    GlRenderer::repaint();
    m_loader_timer_id = SDL_AddTimer(static_cast<Uint32>(m_loaderSpeed),
                                   &LevelScene::nextLoadAniFrame, this);
    m_loaderDoStep = false;
}

LevelData *LevelScene::levelData()
{
    return &m_data;
}

/**************************LoadAnimation**end**************************/
