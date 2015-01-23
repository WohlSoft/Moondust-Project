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

#include <graphics/gl_renderer.h>
#include <graphics/graphics.h>
#include <graphics/window.h>
#include <common_features/graphics_funcs.h>
#include <gui/pge_msgbox.h>

#include "scene_intro.h"

IntroScene::IntroScene()
{
    doExit=false;
}

void IntroScene::update()
{

}

void IntroScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();
//    glDisable(GL_TEXTURE_2D);
//    glColor4f( 0.f, 0.f, 0.f, 1.0f);
//    glBegin( GL_QUADS );
//        glVertex2f( 0, 0);
//        glVertex2f( PGE_Window::Width, 0);
//        glVertex2f( PGE_Window::Width, PGE_Window::Height);
//        glVertex2f( 0, PGE_Window::Height);
//    glEnd();

//    QRectF loadAniG = QRectF(PGE_Window::Width/2 - background.w/2,
//                           PGE_Window::Height/2 - background.h/2,
//                           background.w,
//                           background.h);

//    glEnable(GL_TEXTURE_2D);
//    glColor4f( 1.f, 1.f, 1.f, 1.f);

//    glBindTexture( GL_TEXTURE_2D, background.texture );

//    glBegin( GL_QUADS );
//        glTexCoord2f( 0, 0 );
//        glVertex2f( loadAniG.left(), loadAniG.top());

//        glTexCoord2f( 1, 0 );
//        glVertex2f(  loadAniG.right(), loadAniG.top());

//        glTexCoord2f( 1, 1 );
//        glVertex2f(  loadAniG.right(),  loadAniG.bottom());

//        glTexCoord2f( 0, 1 );
//        glVertex2f( loadAniG.left(),  loadAniG.bottom());
//        glEnd();
//    glDisable(GL_TEXTURE_2D);

//    for(int i=0;i<imgs.size();i++)
//    {
//        QRectF imgRect = QRectF(imgs[i].x,
//                               imgs[i].y,
//                               imgs[i].t.w,
//                               imgs[i].frmH);
//        glEnable(GL_TEXTURE_2D);
//        glColor4f( 1.f, 1.f, 1.f, 1.f);
//        glBindTexture( GL_TEXTURE_2D, imgs[i].t.texture );

//        AniPos x(0,1);
//               x = imgs[i].a.image();

//        glBegin( GL_QUADS );
//            glTexCoord2f( 0, x.first );
//            glVertex2f( imgRect.left(), imgRect.top());

//            glTexCoord2f( 1, x.first );
//            glVertex2f(  imgRect.right(), imgRect.top());

//            glTexCoord2f( 1, x.second );
//            glVertex2f(  imgRect.right(),  imgRect.bottom());

//            glTexCoord2f( 0, x.second );
//            glVertex2f( imgRect.left(),  imgRect.bottom());
//            glEnd();
//        glDisable(GL_TEXTURE_2D);
//    }
    menu.render();

    Scene::render();
}

int IntroScene::exec()
{
    int ret=0;
    //Level scene's Loop
    Uint32 start_render;
    bool running = true;
    float doUpdate_Render=0;
    bool doExit=false;

    menu.clear();
    menu.addMenuItem("1", "Menuitem 1");
    menu.addMenuItem("2", "Menuitem 2");
    menu.addMenuItem("3", "Menuitem 3");
    menu.addMenuItem("4", "Menuitem 4");
    menu.addMenuItem("5", "Menuitem 5");
    menu.addMenuItem("6", "Menuitem about crap");
    menu.addMenuItem("7", "Menuitem about money");
    menu.addMenuItem("8", "Menuitem about skunks");
    menu.addMenuItem("9", "he-he-he");
    menu.addMenuItem("9", "Is fantasy unlimited?");
    menu.addMenuItem("10", "YES!");

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
                    switch(event.key.keysym.sym)
                    {
                      case SDLK_t:
                          PGE_Window::SDL_ToggleFS(PGE_Window::window);
                      break;
                      case SDLK_F3:
                          PGE_Window::showDebugInfo=!PGE_Window::showDebugInfo;
                      break;
                      case SDLK_F12:
                          GlRenderer::makeShot();
                      break;
                      case SDLK_UP:
                        menu.selectUp();
                      break;
                      case SDLK_DOWN:
                        menu.selectDown();
                      break;
                      case SDLK_RETURN:
                        menu.acceptItem();
                      break;
                      case SDLK_ESCAPE:
                        menu.rejectItem();
                      break;
                      default:
                        break;
                    }
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
            if(menu.itemWasSelected())
            {
                if(menu.isAccepted())
                {
                    PGE_MsgBox msgBox(this, "Accepted menuitem: ["+menu.currentItem().title+"]",
                                      PGE_MsgBox::msg_warn);
                    msgBox.exec();
                    ret=1;
                }
                else
                {
                    PGE_MsgBox msgBox(this, "Menu rejected",
                                      PGE_MsgBox::msg_warn);
                    msgBox.exec();
                    ret=-1;
                }
                doExit=true;
            }
        }
    }
    menu.clear();
    return ret;
}
