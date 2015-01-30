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
#include <data_configs/config_manager.h>
#include <file_formats.h>
#include <gui/pge_msgbox.h>

#include "scene_title.h"
#include <QtDebug>
#include <QDir>

TitleScene::TitleScene()
{
    doExit=false;
    mousePos.setX(-300);
    mousePos.setY(-300);
    _cursorIsLoaded=false;

    glClearColor(float(ConfigManager::setup_TitleScreen.backgroundColor.red())/255.0f,
                 float(ConfigManager::setup_TitleScreen.backgroundColor.green())/255.0f,
                 float(ConfigManager::setup_TitleScreen.backgroundColor.blue())/255.0f, 1.0f);
                // Set background color from file

    if(ConfigManager::setup_cursors.normal.isEmpty())
    {
        _cursorIsLoaded=false;
    }
    else
    {
        cursor = GraphicsHelps::loadTexture(cursor, ConfigManager::setup_cursors.normal);
        _cursorIsLoaded=true;
    }

    if(!ConfigManager::setup_TitleScreen.backgroundImg.isEmpty())
    {
        background = GraphicsHelps::loadTexture(background, ConfigManager::setup_TitleScreen.backgroundImg);
        _bgIsLoaded=true;
    }
    else
        _bgIsLoaded=false;

    imgs.clear();

    for(int i=0; i<ConfigManager::setup_TitleScreen.AdditionalImages.size(); i++)
    {
        if(ConfigManager::setup_TitleScreen.AdditionalImages[i].imgFile.isEmpty()) continue;

        TitleScene_misc_img img;
        img.t = GraphicsHelps::loadTexture(img.t, ConfigManager::setup_TitleScreen.AdditionalImages[i].imgFile);

        //Using of X-Y as offsets if aligning is enabled
        int x_offset=ConfigManager::setup_TitleScreen.AdditionalImages[i].x;
        int y_offset=ConfigManager::setup_TitleScreen.AdditionalImages[i].y;

        switch(ConfigManager::setup_TitleScreen.AdditionalImages[i].align_to)
        {
        case TitleScreenAdditionalImage::LEFT_ALIGN:
            img.y = (PGE_Window::Height/2)-(img.t.h/2) + y_offset;
            break;
        case TitleScreenAdditionalImage::TOP_ALIGN:
            img.x = (PGE_Window::Width/2)-(img.t.w/2) + x_offset;
            break;
        case TitleScreenAdditionalImage::RIGHT_ALIGN:
            img.x = PGE_Window::Width-img.t.w - x_offset;
            img.y = (PGE_Window::Height/2)-(img.t.h/2) + y_offset;
            break;
        case TitleScreenAdditionalImage::BOTTOM_ALIGN:
            img.x = (PGE_Window::Width/2)-(img.t.w/2) + x_offset;
            img.y = PGE_Window::Height-img.t.h - y_offset;
            break;
        case TitleScreenAdditionalImage::CENTER_ALIGN:
            img.x = (PGE_Window::Width/2)-(img.t.w/2) + x_offset;
            img.y = (PGE_Window::Height/2)-(img.t.h/2) + y_offset;
            break;
        default:
            img.x = ConfigManager::setup_TitleScreen.AdditionalImages[i].x;
            img.y = ConfigManager::setup_TitleScreen.AdditionalImages[i].y;
            break;
        }

        if(ConfigManager::setup_TitleScreen.AdditionalImages[i].center_x)
            img.x = (PGE_Window::Width/2)-(img.t.w/2) + x_offset;

        if(ConfigManager::setup_TitleScreen.AdditionalImages[i].center_y)
            img.y = (PGE_Window::Height/2)-(img.t.h/2) + y_offset;

        img.a.construct(ConfigManager::setup_TitleScreen.AdditionalImages[i].animated,
                        ConfigManager::setup_TitleScreen.AdditionalImages[i].frames,
                        ConfigManager::setup_TitleScreen.AdditionalImages[i].framespeed);

        img.frmH = (img.t.h / ConfigManager::setup_TitleScreen.AdditionalImages[i].frames);

        imgs.push_back(img);
    }

    for(int i=0;i<imgs.size();i++)
    {
        imgs[i].a.start();
    }
}

TitleScene::~TitleScene()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Black background color
    //Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    if(_cursorIsLoaded)
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures(1, &cursor.texture);
    }

    glDisable(GL_TEXTURE_2D);
    glDeleteTextures( 1, &(background.texture) );

    for(int i=0;i<imgs.size();i++)
    {
        imgs[i].a.stop();
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures( 1, &(imgs[i].t.texture) );
    }
    imgs.clear();
}

void TitleScene::update()
{

}

void TitleScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    if(_bgIsLoaded)
    {
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
    }

    for(int i=0;i<imgs.size();i++)
    {
        QRectF imgRect = QRectF(imgs[i].x,
                               imgs[i].y,
                               imgs[i].t.w,
                               imgs[i].frmH);
        glEnable(GL_TEXTURE_2D);
        glColor4f( 1.f, 1.f, 1.f, 1.f);
        glBindTexture( GL_TEXTURE_2D, imgs[i].t.texture );

        AniPos x(0,1);
               x = imgs[i].a.image();

        glBegin( GL_QUADS );
            glTexCoord2f( 0, x.first );
            glVertex2f( imgRect.left(), imgRect.top());

            glTexCoord2f( 1, x.first );
            glVertex2f(  imgRect.right(), imgRect.top());

            glTexCoord2f( 1, x.second );
            glVertex2f(  imgRect.right(),  imgRect.bottom());

            glTexCoord2f( 0, x.second );
            glVertex2f( imgRect.left(),  imgRect.bottom());
            glEnd();
        glDisable(GL_TEXTURE_2D);
    }


    menu.render();

    Scene::render();
}

void TitleScene::renderMouse()
{
    int posX=mousePos.x();
    int posY=mousePos.y();

    if(_cursorIsLoaded)
    {
        glEnable(GL_TEXTURE_2D);
        glColor4f( 1.f, 1.f, 1.f, 1.f);
        glBindTexture(GL_TEXTURE_2D, cursor.texture);
        glBegin( GL_QUADS );
            glTexCoord2f( 0, 0 );
            glVertex2f( posX, posY);
            glTexCoord2f( 1, 0 );
            glVertex2f( posX+cursor.w, posY);
            glTexCoord2f( 1, 1 );
            glVertex2f( posX+cursor.w, posY+cursor.h);
            glTexCoord2f( 0, 1 );
            glVertex2f( posX, posY+cursor.h);
        glEnd();
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
        glColor4f( 0.f, 1.f, 0.f, 1.0f);
        glBegin( GL_QUADS );
            glVertex2f( posX, posY);
            glVertex2f( posX+10, posY);
            glVertex2f( posX+10, posY+10);
            glVertex2f( posX, posY+10);
        glEnd();
    }
}

int TitleScene::exec()
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
                            if(value=="game1p")
                            {
                                menuChain.push(_currentMenu);
                                setMenu(menu_playepisode);
                            }
                            else
                            if(value=="gamebt")
                            {
                                menuChain.push(_currentMenu);
                                setMenu(menu_playlevel);
                            }
                            else
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
                                PGE_MsgBox msgBox(this, QString("Sorry, is not implemented yet..."),
                                                  PGE_MsgBox::msg_warn);
                                fader_opacity=0.5;
                                msgBox.exec();
                                fader_opacity=0.0;
                                menu.resetState();
                            }
                        break;
                        case menu_playepisode:
                            {
                                menu.resetState();
                            }
                        break;
                        case menu_playlevel:
                            if(value=="nolevel")
                            {
                                //do nothing!
                                menu.resetState();
                            }
                            else
                            {
                                result_level.levelfile = value;
                                ret = ANSWER_PLAYLEVEL;
                                setFade(25, 1.0f, 0.09f);
                                doExit=true;
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
                            if(value=="loading")
                            {
                                ret = ANSWER_LOADING;
                                doExit=true;
                            }
                            else
                            if(value=="gameover")
                            {
                                ret = ANSWER_GAMEOVER;
                                doExit=true;
                            }

                        break;
                        case menu_dummy_and_big:
                            menu.resetState();
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

void TitleScene::setMenu(TitleScene::CurrentMenu _menu)
{
    if(_menu<menuFirst) return;
    if(_menu>menuLast) return;

    _currentMenu=_menu;
    menu.clear();
    switch(_menu)
    {
        case menu_main:
            menu.setPos(260,380);
            menu.setSize(300, 30);
            menu.setItemsNumber(5);
            menu.addMenuItem("game1p", "1 Player Game");
            menu.addMenuItem("game2p", "2 Player Game");
            menu.addMenuItem("gamebt", "Battle Game");
            menu.addMenuItem("Options", "Options");
            menu.addMenuItem("Exit", "Exit");
        break;
            case menu_options:
                menu.setPos(260,380);
                menu.setSize(300, 30);
                menu.setItemsNumber(5);
                menu.addMenuItem("tests", "Test of screens");
                menu.addMenuItem("dab", "Dummy and big menu");
            break;
                case menu_tests:
                    menu.setPos(260,380);
                    menu.setSize(300, 30);
                    menu.setItemsNumber(5);
                    menu.addMenuItem("credits", "Credits");
                    menu.addMenuItem("loading", "Loading screen");
                    menu.addMenuItem("gameover", "Game over screen");
                break;
                case menu_dummy_and_big:
                    menu.setPos(260,300);
                    menu.setSize(300, 30);
                    menu.setItemsNumber(7);
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
        case menu_playepisode:
            {
                //Build list of episodes
                QDir worlddir(ConfigManager::dirs.worlds);
                QStringList filter;
                filter << "*.wld" << "*.wldx";
                QStringList files;
                QStringList folders = worlddir.entryList(QDir::Dirs);

                foreach(QString folder, folders)
                {
                    QString path = ConfigManager::dirs.worlds+folder;
                    QDir episodedir(path);
                    QStringList worlds = episodedir.entryList(filter);
                    foreach(QString world, worlds)
                    {
                        files << ConfigManager::dirs.worlds+folder+"/"+world;
                    }
                }

                if(files.isEmpty())
                    menu.addMenuItem("noworlds", "<episodes not found>");
                else
                {
                    foreach(QString file, files)
                    {
                        WorldData world = FileFormats::OpenWorldFile(file);
                        if(world.ReadFileValid)
                        {
                            QString title = world.EpisodeTitle;
                            menu.addMenuItem(file, (title.isEmpty()?QFileInfo(file).fileName():title));
                        }
                    }
                    menu.sort();
                }
            }
        break;
        case menu_playlevel:
            {
                //Build list of casual levels
                QDir leveldir(ConfigManager::dirs.worlds);
                QStringList filter;
                filter<<"*.lvl" << "*.lvlx";
                QStringList files = leveldir.entryList(filter);

                if(files.isEmpty())
                    menu.addMenuItem("nolevel", "<levels not found>");
                else
                {
                    foreach(QString file, files)
                    {
                        LevelData level = FileFormats::OpenLevelFile(ConfigManager::dirs.worlds+file);
                        if(level.ReadFileValid)
                        {
                            QString title = level.LevelName;
                            menu.addMenuItem(ConfigManager::dirs.worlds+file, (title.isEmpty()?file:title));
                        }
                    }
                    menu.sort();
                }
            }
        break;
    default:
        break;
    }
    menu.setCurrentItem(menustates[_menu].first);
    menu.setOffset(menustates[_menu].second);
}
