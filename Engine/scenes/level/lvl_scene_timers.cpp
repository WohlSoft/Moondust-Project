/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../scene_level.h"
#include "../../common_features/simple_animator.h"
#include "../../common_features/graphics_funcs.h"


/**************************Fader*******************************/
void LevelScene::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_opacity = target;
    fadeSpeed = speed;
    fader_timer_id = SDL_AddTimer(speed, &LevelScene::nextOpacity, this);
}

unsigned int LevelScene::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    LevelScene *self = reinterpret_cast<LevelScene *>(p);
    self->fadeStep();
    return 0;
}

void LevelScene::fadeStep()
{
    if(fader_opacity < target_opacity)
        fader_opacity+=fade_step;
    else
        fader_opacity-=fade_step;

    if(fader_opacity>=1.0 || fader_opacity<=0.0)
        SDL_RemoveTimer(fader_timer_id);
    else
        fader_timer_id = SDL_AddTimer(fadeSpeed, &LevelScene::nextOpacity, this);
}
/**************************Fader**end**************************/




/**************************LoadAnimation*******************************/
namespace lvl_scene_loader
{
    SimpleAnimator * loading_Ani = NULL;
    PGE_Texture loading_texture;
}

void LevelScene::drawLoader()
{
    using namespace lvl_scene_loader;

    if(!loading_Ani) return;

    QRectF loadAniG = QRectF(PGE_Window::Width/2 - loading_texture.w/2,
                           PGE_Window::Height/2 - (loading_texture.h/4)/2,
                           loading_texture.w,
                           loading_texture.h/4);

    glEnable(GL_TEXTURE_2D);
    glColor4f( 1.f, 1.f, 1.f, 1.f);

    AniPos x(0,1);
            x = loading_Ani->image();

    glBindTexture( GL_TEXTURE_2D, loading_texture.texture );

    glBegin( GL_QUADS );
        glTexCoord2f( 0, x.first );
        glVertex2f( loadAniG.left(), loadAniG.top());

        glTexCoord2f( 1, x.first );
        glVertex2f(  loadAniG.right(), loadAniG.top());

        glTexCoord2f( 1, x.second );
        glVertex2f(  loadAniG.right(),  loadAniG.bottom());

        glTexCoord2f( 0, x.second );
        glVertex2f( loadAniG.left(),  loadAniG.bottom());
        glEnd();
    glDisable(GL_TEXTURE_2D);
}


void LevelScene::setLoaderAnimation(int speed)
{
    using namespace lvl_scene_loader;
    loaderSpeed = speed;
    loading_texture = GraphicsHelps::loadTexture(loading_texture, ":/images/shell.png");

    loading_Ani = new SimpleAnimator(true,
                                     4,
                                     70,
                                     0, -1, false, false);
    loading_Ani->start();

    loader_timer_id = SDL_AddTimer(speed, &LevelScene::nextLoadAniFrame, this);
    IsLoaderWorks = true;
}

void LevelScene::stopLoaderAnimation()
{
    using namespace lvl_scene_loader;

    doLoaderStep = false;
    IsLoaderWorks = false;
    SDL_RemoveTimer(loader_timer_id);

    if(loading_Ani)
    {
        loading_Ani->stop();
        delete loading_Ani;
        loading_Ani = NULL;
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures( 1, &(loading_texture.texture) );
    }

}

unsigned int LevelScene::nextLoadAniFrame(unsigned int x, void *p)
{
    Q_UNUSED(x);
    LevelScene *self = reinterpret_cast<LevelScene *>(p);
    self->loaderTick();
    return 0;
}

void LevelScene::loaderTick()
{
    doLoaderStep = true;
}

void LevelScene::loaderStep()
{
    if(!IsLoaderWorks) return;
    if(!doLoaderStep) return;

    SDL_Event event; //  Events of SDL
    while ( SDL_PollEvent(&event) )
    {
        switch(event.type)
        {
            case SDL_QUIT:
            break;
        }
    }

    render();
    glFlush();
    SDL_GL_SwapWindow(PGE_Window::window);

    loader_timer_id = SDL_AddTimer(loaderSpeed, &LevelScene::nextLoadAniFrame, this);
    doLoaderStep = false;
}

QVector<LVL_Background *> *LevelScene::bgList()
{
    return &backgrounds;
}

LevelData *LevelScene::levelData()
{
    return &data;
}


/**************************LoadAnimation**end**************************/


