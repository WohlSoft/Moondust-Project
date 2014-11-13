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

#include "pge_msgbox.h"

#include "../fontman/font_manager.h"
#include "../graphics/window.h"

#include "../graphics/gl_renderer.h"

#include "../scenes/scene_level.h"

#include <QRect>


PGE_MsgBox::PGE_MsgBox()
    : PGE_BoxBase(0)
{
    message = "Message box works fine!";
    type = msg_info;
    switch(type)
    {
        case msg_info: bg_color = QColor(qRgb(0,0,0)); break;
        case msg_warn: bg_color = QColor(qRgb(255,201,14)); break;
        case msg_error: bg_color = QColor(qRgb(125,0,0)); break;
        case msg_fatal: bg_color = QColor(qRgb(255,0,0)); break;
        default:  bg_color = QColor(qRgb(0,0,0)); break;
    }
    setBoxSize(200, 125, 20);
    buildBox();
}

PGE_MsgBox::PGE_MsgBox(Scene *_parentScene, QString msg, msgType _type, QSizeF boxSize, float _padding)
    : PGE_BoxBase(_parentScene)
{
    message = msg;
    type = _type;
    switch(type)
    {
        case msg_info: bg_color = QColor(qRgb(0,0,0)); break;
        case msg_warn: bg_color = QColor(qRgb(255,201,14)); break;
        case msg_error: bg_color = QColor(qRgb(125,0,0)); break;
        case msg_fatal: bg_color = QColor(qRgb(255,0,0)); break;
        default:  bg_color = QColor(qRgb(0,0,0)); break;
    }
    setBoxSize(boxSize.width()/2, boxSize.height()/2, _padding);
    buildBox();
}

void PGE_MsgBox::buildBox()
{

    textTexture = FontManager::TextToTexture(message,
                                             QRect(0,0, width*2, height*2),
                                             Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap);
}

PGE_MsgBox::~PGE_MsgBox()
{
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &textTexture );
}

void PGE_MsgBox::setBoxSize(float _Width, float _Height, float _padding)
{
    width = _Width;
    height = _Height;
    padding = _padding;
}

void PGE_MsgBox::exec()
{
    Uint32 start_render;
    Uint32 start_fade;

    Uint32 AntiFreeze_delay=3000; //Protecting from freezing if fading timer wasn't start

    SDL_Event event; //  Events of SDL

    while ( SDL_PollEvent(&event) )
    {
        if(parentScene!=NULL)
        {
            if(parentScene->type()==Scene::Level)
                dynamic_cast<LevelScene *>(parentScene)->keyboard1.update(event);
        }
    }

    if(parentScene!=NULL)
    {
        if(parentScene->type()==Scene::Level)
            dynamic_cast<LevelScene *>(parentScene)->keyboard1.sendControls();
    }

    setFade(20, 1.0f, 0.09f);

    start_fade = SDL_GetTicks();
    while(fader_opacity<1.0f)
    {
        if( start_fade+AntiFreeze_delay
                < SDL_GetTicks()-start_fade)
            fader_opacity = 1.0f;

        start_render=SDL_GetTicks();

        PGE_BoxBase::exec();

        glDisable(GL_TEXTURE_2D);
        glColor4f( bg_color.red()/255.0f, bg_color.green()/255.0f, bg_color.blue()/255.0f, 1.0);
        glBegin( GL_QUADS );
            glVertex2f( PGE_Window::Width/2 - width*fader_opacity - padding, PGE_Window::Height/2 - height*fader_opacity - padding);
            glVertex2f( PGE_Window::Width/2 + width*fader_opacity + padding, PGE_Window::Height/2 - height*fader_opacity - padding);
            glVertex2f( PGE_Window::Width/2 + width*fader_opacity + padding, PGE_Window::Height/2 + height*fader_opacity + padding);
            glVertex2f( PGE_Window::Width/2 - width*fader_opacity - padding, PGE_Window::Height/2 + height*fader_opacity + padding);
        glEnd();

        glFlush();
        SDL_GL_SwapWindow(PGE_Window::window);

        while ( SDL_PollEvent(&event) )
        {
            if(parentScene!=NULL)
            {
                if(parentScene->type()==Scene::Level)
                    dynamic_cast<LevelScene *>(parentScene)->keyboard1.update(event);
            }
        }

        if(parentScene!=NULL)
        {
            if(parentScene->type()==Scene::Level)
                dynamic_cast<LevelScene *>(parentScene)->keyboard1.sendControls();
        }

        if(1000.0 / (float)PGE_Window::MaxFPS >SDL_GetTicks() - start_render)
                //SDL_Delay(1000.0/1000-(SDL_GetTicks()-start));
                SDL_Delay(1000.0 / (float)PGE_Window::MaxFPS - (SDL_GetTicks()-start_render) );
    }


    bool running=true;
    while(running)
    {

        start_render=SDL_GetTicks();

        PGE_BoxBase::exec();

        glDisable(GL_TEXTURE_2D);
        glColor4f( bg_color.red()/255.0f, bg_color.green()/255.0f, bg_color.blue()/255.0f, 1.0);
        glBegin( GL_QUADS );
            glVertex2f( PGE_Window::Width/2 - width - padding,
                        PGE_Window::Height/2 - height - padding);
            glVertex2f( PGE_Window::Width/2 + width + padding,
                        PGE_Window::Height/2 - height - padding);
            glVertex2f( PGE_Window::Width/2 + width + padding,
                        PGE_Window::Height/2 + height + padding);
            glVertex2f( PGE_Window::Width/2 - width - padding,
                        PGE_Window::Height/2 + height + padding);
        glEnd();

        FontManager::SDL_string_render2D(PGE_Window::Width/2-width,
                                         PGE_Window::Height/2-height,
                                         &textTexture);

        glFlush();
        SDL_GL_SwapWindow(PGE_Window::window);

        while ( SDL_PollEvent(&event) )
        {
            if(parentScene!=NULL)
            {
                if(parentScene->type()==Scene::Level)
                    dynamic_cast<LevelScene *>(parentScene)->keyboard1.update(event);
            }

            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        running=false;
                    }
                break;

                case SDL_KEYDOWN: // If pressed key
                  switch(event.key.keysym.sym)
                  { // Check which
                    case SDLK_ESCAPE: // ESC
                    case SDLK_RETURN:// Enter
                    case SDLK_KP_ENTER:
                        {
                            running=false;
                        }
                    break;
                    case SDLK_t:
                        PGE_Window::SDL_ToggleFS(PGE_Window::window);
                    break;
                    case SDLK_F12:
                        GlRenderer::makeShot();
                    break;
                    default:
                      break;
                  }
                break;
                default:
                  break;
            }
        }
        if(parentScene!=NULL)
        {
            if(parentScene->type()==Scene::Level)
                dynamic_cast<LevelScene *>(parentScene)->keyboard1.sendControls();
        }

        if(1000.0 / 75.0 > SDL_GetTicks() - start_render)
                //SDL_Delay(1000.0/1000-(SDL_GetTicks()-start));
                SDL_Delay(1000.0 / 75.0 - (SDL_GetTicks()-start_render) );
    }





    setFade(20, 0.0f, 0.09f);

    start_fade = SDL_GetTicks();
    while(fader_opacity>0.0f)
    {
        if( start_fade+AntiFreeze_delay
                < SDL_GetTicks()-start_fade)
            fader_opacity = 0.0f;

        start_render=SDL_GetTicks();

        PGE_BoxBase::exec();

        glDisable(GL_TEXTURE_2D);
        glColor4f( bg_color.red()/255.0f, bg_color.green()/255.0f, bg_color.blue()/255.0f, 1.0);
        glBegin( GL_QUADS );
            glVertex2f( PGE_Window::Width/2 - width*fader_opacity - padding,
                        PGE_Window::Height/2 - height*fader_opacity - padding);
            glVertex2f( PGE_Window::Width/2 + width*fader_opacity + padding,
                        PGE_Window::Height/2 - height*fader_opacity - padding);
            glVertex2f( PGE_Window::Width/2 + width*fader_opacity + padding,
                        PGE_Window::Height/2 + height*fader_opacity + padding);
            glVertex2f( PGE_Window::Width/2 - width*fader_opacity - padding,
                        PGE_Window::Height/2 + height*fader_opacity + padding);
        glEnd();

        glFlush();
        SDL_GL_SwapWindow(PGE_Window::window);

        while ( SDL_PollEvent(&event) )
        {
            if(parentScene!=NULL)
            {
                if(parentScene->type()==Scene::Level)
                    dynamic_cast<LevelScene *>(parentScene)->keyboard1.update(event);
            }
        }
        if(parentScene!=NULL)
        {
            if(parentScene->type()==Scene::Level)
                dynamic_cast<LevelScene *>(parentScene)->keyboard1.sendControls();
        }

        if(1000.0 / (float)PGE_Window::MaxFPS >SDL_GetTicks() - start_render)
                //SDL_Delay(1000.0/1000-(SDL_GetTicks()-start));
                SDL_Delay(1000.0 / (float)PGE_Window::MaxFPS - (SDL_GetTicks()-start_render) );
    }

}
