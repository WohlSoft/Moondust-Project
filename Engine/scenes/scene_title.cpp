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

#include "scene_title.h"
#include <graphics/gl_renderer.h>
#include <graphics/graphics.h>
#include <graphics/window.h>
#include <common_features/graphics_funcs.h>
#include <data_configs/config_manager.h>

TitleScene::TitleScene()
{
    _waitTimer=5000;

    /*********Fader*************/
    fader_opacity=1.0f;
    target_opacity=1.0f;
    fade_step=0.0f;
    fadeSpeed=25;
    /*********Fader*************/
}

TitleScene::~TitleScene()
{
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures( 1, &(background.texture) );
    SDL_RemoveTimer(fader_timer_id);
}

void TitleScene::init()
{
    if(!ConfigManager::LoadingScreen.backgroundImg.isEmpty())
        background = GraphicsHelps::loadTexture(background, ConfigManager::LoadingScreen.backgroundImg);
    else
        background = GraphicsHelps::loadTexture(background, ":/images/cat_splash.png");


}

void TitleScene::setWaitTime(unsigned int time)
{
    if(time==0)
        _waitTimer=5000;
    else
        _waitTimer=time;
}

void TitleScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    glDisable(GL_TEXTURE_2D);
    glColor4f( 0.f, 0.f, 0.f, 1.0f);
    glBegin( GL_QUADS );
        glVertex2f( 0, 0);
        glVertex2f( PGE_Window::Width, 0);
        glVertex2f( PGE_Window::Width, PGE_Window::Height);
        glVertex2f( 0, PGE_Window::Height);
    glEnd();

    QRectF loadAniG = QRectF(PGE_Window::Width/2 - background.w/2,
                           PGE_Window::Height/2 - background.h/2,
                           background.w,
                           background.h);

    glEnable(GL_TEXTURE_2D);
    glColor4f( 1.f, 1.f, 1.f, 1.f);

    glBindTexture( GL_TEXTURE_2D, background.texture );

    glBegin( GL_QUADS );
        glTexCoord2f( 0, 0 );
        glVertex2f( loadAniG.left(), loadAniG.top());

        glTexCoord2f( 1, 0 );
        glVertex2f(  loadAniG.right(), loadAniG.top());

        glTexCoord2f( 1, 1 );
        glVertex2f(  loadAniG.right(),  loadAniG.bottom());

        glTexCoord2f( 0, 1 );
        glVertex2f( loadAniG.left(),  loadAniG.bottom());
        glEnd();
    glDisable(GL_TEXTURE_2D);

    if(fader_opacity>0.0f)
    {
        glDisable(GL_TEXTURE_2D);
        glColor4f( 0.f, 0.f, 0.f, fader_opacity);
        glBegin( GL_QUADS );
            glVertex2f( 0, 0);
            glVertex2f( PGE_Window::Width, 0);
            glVertex2f( PGE_Window::Width, PGE_Window::Height);
            glVertex2f( 0, PGE_Window::Height);
        glEnd();
    }
}

int TitleScene::exec()
{
    //Level scene's Loop
    Uint32 start_render;
    bool running = true;
    float doUpdate_Render=0;
    bool doExit=false;

    Uint32 start_wait_timer=SDL_GetTicks();
    while(running)
    {

        //UPDATE Events
        start_render=SDL_GetTicks();
        render();
        glFlush();
        SDL_GL_SwapWindow(PGE_Window::window);

        if(doExit)
        {
            if(fader_opacity<=0.0f) setFade(25, 1.0f, 0.02f);
            if(fader_opacity>=1.0)
                running=false;
        }

        SDL_Event event; //  Events of SDL
        while ( SDL_PollEvent(&event) )
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        return -1;
                    }   // End work of program
                break;

                case SDL_KEYDOWN: // If pressed key
                  doExit=true;
                break;

                case SDL_KEYUP:
                break;
                default: break;
            }
        }

        if( 100.0 / (float)PGE_Window::PhysStep >SDL_GetTicks()-start_render)
        {
            doUpdate_Render = 1000.0/100.0-(SDL_GetTicks()-start_render);
            SDL_Delay( doUpdate_Render );
        }

        if(!doExit)
        {
            if( (SDL_GetTicks()-start_wait_timer) > _waitTimer)
                doExit=true;
        }

    }
    return 0;
}







/**************************Fader*******************************/
void TitleScene::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_opacity = target;
    fadeSpeed = speed;
    fader_timer_id = SDL_AddTimer(speed, &TitleScene::nextOpacity, this);
}

unsigned int TitleScene::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    TitleScene *self = reinterpret_cast<TitleScene *>(p);
    self->fadeStep();
    return 0;
}

void TitleScene::fadeStep()
{
    if(fader_opacity < target_opacity)
        fader_opacity+=fade_step;
    else
        fader_opacity-=fade_step;

    if(fader_opacity>=1.0 || fader_opacity<=0.0)
        SDL_RemoveTimer(fader_timer_id);
    else
        fader_timer_id = SDL_AddTimer(fadeSpeed, &TitleScene::nextOpacity, this);
}
/**************************Fader**end**************************/
