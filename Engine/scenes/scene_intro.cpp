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
    offscreen=false;
    mousePos.setX(-300);
    mousePos.setY(-300);
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

void IntroScene::renderMouse()
{
    int posX=mousePos.x();
    int posY=mousePos.y();
    glDisable(GL_TEXTURE_2D);
    glColor4f( 0.f, 1.f, 0.f, 1.0f);
    glBegin( GL_QUADS );
        glVertex2f( posX, posY);
        glVertex2f( posX+10, posY);
        glVertex2f( posX+10, posY+10);
        glVertex2f( posX, posY+10);
    glEnd();
}

int IntroScene::exec()
{
    int ret=0;
    //Level scene's Loop
    Uint32 start_render;
    bool running = true;
    float doUpdate_Render=0;
    bool doExit=false;

    menustates.clear();
    menuChain.clear();

    for(int i=menuFirst; i<menuLast;i++)
        menustates[(CurrentMenu)i] = menustate(0, 0);

    setMenu(menu_main);

    while(running)
    {
        //UPDATE Events
        start_render=SDL_GetTicks();
        render();

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
                        return ANSWER_EXIT;
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
                case SDL_MOUSEMOTION:
                    mousePos.setX(event.motion.x);
                    mousePos.setY(event.motion.y);
                    menu.setMouseHoverPos(mousePos.x(), mousePos.y());
                break;
                case SDL_MOUSEBUTTONDOWN:
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            menu.setMouseClickPos(event.button.x, event.button.y);
                        break;
                        case SDL_BUTTON_RIGHT:
                            menu.rejectItem();
                        break;
                        default:
                        break;
                    }
                break;
                case SDL_MOUSEWHEEL:
                    if(event.wheel.y>0)
                        menu.scrollUp();
                    else
                        menu.scrollDown();
                default: break;
            }
        }
        int mouseX=0;
        int mouseY=0;
        SDL_PumpEvents();
        SDL_GetMouseState(&mouseX, &mouseY);
        mousePos.setX(mouseX);
        mousePos.setY(mouseY);


        renderMouse();
        glFlush();
        SDL_GL_SwapWindow(PGE_Window::window);

        if( 100.0 / (float)PGE_Window::PhysStep >SDL_GetTicks()-start_render)
        {
            doUpdate_Render = 1000.0/100.0-(SDL_GetTicks()-start_render);
            SDL_Delay( doUpdate_Render );
        }

        if(!doExit)
        {
            if(menu.isSelected())
            {
                if(menu.isAccepted())
                {
                    menustates[_currentMenu].first = menu.currentItemI();
                    menustates[_currentMenu].second = menu.offset();

                    QString value = menu.currentItem().value;
                    switch(_currentMenu)
                    {
                        case menu_main:
                            if(value=="Options")
                            {
                                menuChain.push(_currentMenu);
                                setMenu(menu_options);
                            }
                            else
                            if(value=="Exit")
                            {
                                ret = ANSWER_EXIT;
                                doExit=true;
                            }
                            else
                            {
                                PGE_MsgBox msgBox(this, QString("Dummy"),
                                                  PGE_MsgBox::msg_warn);
                                msgBox.exec();
                                menu.resetState();
                            }
                        break;
                        case menu_options:
                            if(value=="tests")
                            {
                                menuChain.push(_currentMenu);
                                setMenu(menu_tests);
                            }
                            else
                            if(value=="dab")
                            {
                                menuChain.push(_currentMenu);
                                setMenu(menu_dummy_and_big);
                            }
                            else
                            {
                                PGE_MsgBox msgBox(this, QString("Dummy"),
                                                  PGE_MsgBox::msg_warn);
                                msgBox.exec();
                                menu.resetState();
                            }

                        break;
                        case menu_tests:
                            if(value=="credits")
                            {
                                ret = ANSWER_CREDITS;
                                doExit=true;
                            }
                            else
                            if(value=="title")
                            {
                                ret = ANSWER_TITLE;
                                doExit=true;
                            }
                            else
                            if(value=="gameover")
                            {
                                ret = ANSWER_GAMEOVER;
                                doExit=true;
                            }

                        break;
                        case menu_playlevel:

                        break;
                    default:
                        break;

                    }
                }
                else
                {
                    switch(_currentMenu)
                    {
                        case menu_main:
                            menu.reset();
                            menu.setCurrentItem(4);
                        break;
                    default:
                        if(menuChain.size()>0)
                        {
                            setMenu((CurrentMenu)menuChain.pop());
                            menu.reset();
                        }
                        break;
                    }
                }
            }
        }
    }
    menu.clear();
    return ret;
}

void IntroScene::setMenu(IntroScene::CurrentMenu _menu)
{
    if(_menu<menuFirst) return;
    if(_menu>menuLast) return;

    _currentMenu=_menu;
    menu.clear();
    switch(_menu)
    {
        case menu_main:
            menu.addMenuItem("game1p", "1 Player Game");
            menu.addMenuItem("game2p", "2 Player Game");
            menu.addMenuItem("gamebt", "Battle Game");
            menu.addMenuItem("Options", "Options");
            menu.addMenuItem("Exit", "Exit");
        break;
            case menu_options:
                menu.addMenuItem("tests", "Test of screens");
                menu.addMenuItem("dab", "Dummy and big menu");
            break;
                case menu_tests:
                    menu.addMenuItem("credits", "Credits");
                    menu.addMenuItem("title", "Title screen");
                    menu.addMenuItem("gameover", "Game over screen");
                break;
                case menu_dummy_and_big:
                    menu.addMenuItem("1", "Item 1");
                    menu.addMenuItem("2", "Item 2");
                    menu.addMenuItem("3", "Item 3");
                    menu.addMenuItem("4", "Кое-что по-русски");
                    menu.addMenuItem("5", "Ich bin glücklich!");
                    menu.addMenuItem("6", "¿Que se ocupas?");
                    menu.addMenuItem("7", "Minä rakastan sinua!");
                    menu.addMenuItem("8", "هذا هو اختبار صغير");
                    menu.addMenuItem("9", "這是一個小測試!");
                    menu.addMenuItem("10", "דאס איז אַ קליין פּרובירן");
                    menu.addMenuItem("11", "આ નાના કસોટી છે");
                    menu.addMenuItem("12", "यह एक छोटी सी परीक्षा है");
                break;
        case menu_playlevel:
            menu.addMenuItem("dummy", "Less menuitems");
            menu.addMenuItem("dummy1", "he-he 1");
        break;
    default:
        break;
    }
    menu.setCurrentItem(menustates[_menu].first);
    menu.setOffset(menustates[_menu].second);
}
