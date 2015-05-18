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

#include <graphics/gl_renderer.h>
#include <graphics/graphics.h>
#include <graphics/window.h>
#include <common_features/graphics_funcs.h>
#include <settings/global_settings.h>
#include <data_configs/config_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <gui/pge_msgbox.h>
#include <audio/pge_audio.h>
#include <audio/SdlMusPlayer.h>

#include <fontman/font_manager.h>
#include <controls/controller_joystick.h>

#include "scene_title.h"
#include <QtDebug>
#include <QDir>

#include <SDL2/SDL.h>
#undef main

TitleScene::TitleScene()
{
    doExit=false;
    mousePos.setX(-300);
    mousePos.setY(-300);
    _cursorIsLoaded=false;

    numOfPlayers=1;

    controller = AppSettings.openController(1);

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

    delete controller;
}

void TitleScene::update()
{

}

static int debug_joy_keyval=0;
static int debug_joy_keyid=0;
static int debug_joy_keytype=0;

void TitleScene::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Reset modelview matrix
    glLoadIdentity();

    if(_bgIsLoaded)
    {
        GlRenderer::renderTexture(&background, PGE_Window::Width/2 - background.w/2,
                                  PGE_Window::Height/2 - background.h/2);
    }

    for(int i=0;i<imgs.size();i++)
    {
        AniPos x(0,1); x = imgs[i].a.image();
        GlRenderer::renderTexture(&imgs[i].t,
                                  imgs[i].x,
                                  imgs[i].y,
                                  imgs[i].t.w,
                                  imgs[i].frmH, x.first, x.second);
    }

    if(AppSettings.showDebugInfo)
    {
        FontManager::printText(QString("Joystick key: val=%1, id=%2, type=%3")
                               .arg(debug_joy_keyval)
                               .arg(debug_joy_keyid)
                               .arg(debug_joy_keytype),10, 10);
//        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
//                               "abcdefghijklmnopqrstuvwxyz\n"
//                               "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ\n"
//                               "абвгдеёжзийклмнопрстуфхцчшщъыьэюя\n"
//                               "Ich bin glücklich!",10, 60, 2, 1.0, 1.0, 1.0, 1.0);
        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
                               "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ\n"
                               "{|}\\¡¢£€¥Š§š©ª«¬®¯°±²³Žµ¶·ž¹º»ŒŸ¿\n"
                               "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×\n"
                               "ØÙÚÛÜÝÞß÷ © ®\n\n"
                               "Ich bin glücklich!\n\n"
                               "Как хорошо, что всё работает!\n"
                               "Живіть всі дружно!", 10, 50, 0, 1.0, 1.0, 0, 1.0);
//        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\nIch bin glücklich!", 10, 90, 1, 0, 1.0, 0, 1.0);
//        FontManager::printText("0123456789\n"
//                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n"
//                               "abcdefghijklmnopqrstuvwxyz\n"
//                               "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ\n"
//                               "абвгдеёжзийклмнопрстуфхцчшщъыьэюя\n"
//                               "Ich bin glücklich!", 10, 130, 2, 1.0, 1.0, 1.0, 1.0);
//        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\nIch bin glücklich!", 10, 250, 3, 1.0, 1.0, 1.0, 1.0);
//        FontManager::printText("0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ\nIch bin glücklich!", 10, 290, FontManager::DefaultTTF_Font, 1.0, 0.5, 1.0, 1.0);
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
        GlRenderer::renderTexture(&cursor, posX, posY);
    }
    else
    {
        GlRenderer::renderRect(posX, posY, 10,10, 0.f, 1.f, 0.f, 1.0f);
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

    //Hide mouse cursor
    PGE_Window::setCursorVisibly(false);

    while(running)
    {
        //UPDATE Events
        start_render=SDL_GetTicks();
        if(doExit)
        {
            if(fader_opacity<=0.0f) setFade(25, 1.0f, 0.02f);
            if(fader_opacity>=1.0)
                running=false;
        }

        controller->update();


        if(PGE_Window::showDebugInfo)
        {
            if(AppSettings.joysticks.size()>0)
            {
                JoystickController::bindJoystickKey(AppSettings.joysticks.first(),
                                                    debug_joy_keyval,
                                                    debug_joy_keyid,
                                                    debug_joy_keytype);
            }
        }


        SDL_Event event; //  Events of SDL
        SDL_PumpEvents();             //for mouse
        while( SDL_PollEvent(&event) )//Common
        {
            if(PGE_Window::processEvents(event)!=0) continue;
            switch(event.type)
            {
                case SDL_QUIT:
                    {
                        return ANSWER_EXIT;
                    }   // End work of program
                break;
            case SDL_KEYDOWN: // If pressed key
                    if(menu.isKeyGrabbing())
                    {
                        if(event.key.keysym.scancode!=SDL_SCANCODE_ESCAPE)
                            menu.storeKey(event.key.keysym.scancode);
                        else
                            menu.storeKey(PGE_KEYGRAB_REMOVE_KEY);
                    /**************Control men via controllers*************/
                    } else if(controller->keys.up) {
                        menu.selectUp();
                    } else if(controller->keys.down) {
                        menu.selectDown();
                    } else if(controller->keys.left) {
                        menu.selectLeft();
                    } else if(controller->keys.right) {
                        menu.selectRight();
                    } else if(controller->keys.jump) {
                        menu.acceptItem();
                    } else if(controller->keys.run) {
                        menu.rejectItem();
                    } else
                    if(!doExit)
                    switch(event.key.keysym.sym)
                    {
                      case SDLK_UP:
                        menu.selectUp();
                      break;
                      case SDLK_DOWN:
                        menu.selectDown();
                      break;
                      case SDLK_LEFT:
                        menu.selectLeft();
                      break;
                      case SDLK_RIGHT:
                        menu.selectRight();
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
                    mousePos = GlRenderer::MapToScr(event.motion.x, event.motion.y);
                if(!menu.isKeyGrabbing() && !doExit)
                    menu.setMouseHoverPos(mousePos.x(), mousePos.y());
                break;
                case SDL_MOUSEBUTTONDOWN:
                    if(menu.isKeyGrabbing())
                        menu.storeKey(PGE_KEYGRAB_CANCEL); //Calcel Keygrabbing
                    else
                    switch(event.button.button)
                    {
                        case SDL_BUTTON_LEFT:
                            mousePos = GlRenderer::MapToScr(event.button.x, event.button.y);
                            menu.setMouseClickPos(mousePos.x(), mousePos.y());
                        break;
                        case SDL_BUTTON_RIGHT:
                            menu.rejectItem();
                        break;
                        default:
                        break;
                    }
                break;
                case SDL_MOUSEWHEEL:
                    if(!menu.isKeyGrabbing() && !doExit)
                    {
                        if(event.wheel.y>0)
                            menu.scrollUp();
                        else
                            menu.scrollDown();
                    }
                default: break;
            }
        }
        int mouseX=0;
        int mouseY=0;
        SDL_GetMouseState(&mouseX, &mouseY);
        mousePos = GlRenderer::MapToScr(mouseX, mouseY);

        render();
        renderMouse();
        glFlush();
        SDL_GL_SwapWindow(PGE_Window::window);

        if( (100.0 / (float)PGE_Window::PhysStep) >(SDL_GetTicks()-start_render))
        {
            doUpdate_Render = (1000.0/100.0)-(SDL_GetTicks()-start_render);
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
                                numOfPlayers=1;
                                menuChain.push(_currentMenu);
                                setMenu(menu_playepisode);
                            }
                            else
                            if(value=="game2p")
                            {
                                numOfPlayers=2;
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
                                PGE_Window::setCursorVisibly(true);
                                msgBox.exec();
                                PGE_Window::setCursorVisibly(false);
                                fader_opacity=0.0;
                                menu.resetState();
                            }
                        break;
                        case menu_playepisode:
                            {
                                if(value=="noworlds")
                                {
                                    //do nothing!
                                    menu.resetState();
                                }
                                else
                                {
                                    result_episode.worldfile = value;
                                    result_episode.character = 0;
                                    result_episode.savefile = "save1.savx";
                                    if(numOfPlayers>1)
                                        ret = ANSWER_PLAYEPISODE_2P;
                                    else
                                        ret = ANSWER_PLAYEPISODE;
                                    setFade(21, 1.0f, 0.2f);
                                    fader_opacity=0.1f;
                                    doExit=true;
                                }
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
                                setFade(21, 1.0f, 0.2f);
                                fader_opacity=0.1f;
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
                            if(value=="controls")
                            {
                                menuChain.push(_currentMenu);
                                setMenu(menu_controls);
                            }
                            else
                            {
                                PGE_MsgBox msgBox(this, QString("Dummy"),
                                                  PGE_MsgBox::msg_warn);
                                msgBox.exec();
                                menu.resetState();
                            }
                        break;
                        case menu_controls:
                            if(value=="control_plr1")
                            {
                                menuChain.push(_currentMenu);
                                setMenu(menu_controls_plr1);
                            }
                            else
                            if(value=="control_plr2")
                            {
                                menuChain.push(_currentMenu);
                                setMenu(menu_controls_plr2);
                            }
                        break;
                        case menu_controls_plr1:

                        break;
                        case menu_controls_plr2:

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
                        case menu_options:
                        AppSettings.apply();
                        AppSettings.save();
                        PGE_Audio::playSoundByRole(obj_sound_role::Bonus1up);
                    default:
                        if(menu.isKeyGrabbing())
                            menu.reset();
                        else
                        if(menuChain.size()>0)
                        {
                            menu.reset();
                            setMenu((CurrentMenu)menuChain.pop());
                        }
                        break;
                    }
                }
            }
        }
    }
    menu.clear();

    //Show mouse cursor
    PGE_Window::setCursorVisibly(true);
    return ret;
}

void TitleScene::setMenu(TitleScene::CurrentMenu _menu)
{
    if(_menu<menuFirst) return;
    if(_menu>menuLast) return;

    _currentMenu=_menu;
    menu.clear();
    menu.setTextLenLimit(22);
    switch(_menu)
    {
        case menu_main:
            menu.setPos(300, 350);
            menu.setItemsNumber(5);
            menu.addMenuItem("game1p", "1 Player Game");
            menu.addMenuItem("game2p", "2 Player Game");
            menu.addMenuItem("gamebt", "Battle Game");
            menu.addMenuItem("Options", "Options");
            menu.addMenuItem("Exit", "Exit");
        break;
            case menu_options:
                menu.setPos(260,284);
                menu.setItemsNumber(8);
                menu.addMenuItem("tests", "Test of screens");
                menu.addMenuItem("controls", "Player controlling");
                menu.addIntMenuItem(&AppSettings.volume_music, 0, 128, "vlm_music", "Music volume", false,
                                    []()->void{ PGE_MusPlayer::MUS_changeVolume(AppSettings.volume_music); });
                menu.addBoolMenuItem(&AppSettings.fullScreen, "full_screen", "Full Screen mode",
                                     []()->void{ PGE_Window::setFullScreen(AppSettings.fullScreen); }
                                     );
                menu.addBoolMenuItem(&AppSettings.showDebugInfo, "dbg_flag", "Show debug info");
                menu.addBoolMenuItem(&AppSettings.enableDummyNpc, "dummy_npcs", "Enable dummy NPC's");
                menu.addIntMenuItem(&AppSettings.MaxFPS, 65, 1000, "max_fps", "Max FPS");
                menu.addIntMenuItem(&AppSettings.PhysStep, 65, 80, "phys_step", "Physics step");
            break;
                case menu_tests:
                    menu.setPos(300, 350);
                    menu.setItemsNumber(5);
                    menu.addMenuItem("credits", "Credits");
                    menu.addMenuItem("loading", "Loading screen");
                    menu.addMenuItem("gameover", "Game over screen");
                break;
                    case menu_controls:
                        menu.setPos(300, 350);
                        menu.setItemsNumber(5);
                        menu.addMenuItem("control_plr1", "Player 1 controls");
                        menu.addMenuItem("control_plr2", "Player 2 controls");
                    break;
                        case menu_controls_plr1:
                        case menu_controls_plr2:
                        {

                        KeyMap *mp_p;
                        int *mct_p;
                        std::function<void()> ctrlSwitch;

                        if(_menu==menu_controls_plr1)
                        {
                            ctrlSwitch = [this]()->void{
                                setMenu(menu_controls_plr1);
                                };
                            mct_p = &AppSettings.player1_controller;
                            if((*mct_p>=0)&&(*mct_p<AppSettings.player1_joysticks.size()))
                                mp_p = &AppSettings.player1_joysticks[*mct_p];
                            else
                                mp_p = &AppSettings.player1_keyboard;
                        }
                        else{
                            ctrlSwitch = [this]()->void{
                                setMenu(menu_controls_plr2);
                                };
                            mct_p  = &AppSettings.player2_controller;
                            if((*mct_p>=0)&&(*mct_p<AppSettings.player2_joysticks.size()))
                                mp_p = &AppSettings.player2_joysticks[*mct_p];
                            else
                                mp_p = &AppSettings.player2_keyboard;
                        }

                            menu.setPos(300, 216);
                            menu.setItemsNumber(11);
                            QList<IntAssocItem> ctrls;
                            IntAssocItem controller;
                            controller.value=-1;
                            controller.label="Keyboard";
                            ctrls.push_back(controller);
                            for(int i=0;i<AppSettings.joysticks.size();i++)
                            {
                                controller.value=i;
                                controller.label=QString("Joystick: %1").arg(SDL_JoystickName(AppSettings.joysticks[i]));
                                ctrls.push_back(controller);
                            }
                            menu.addNamedIntMenuItem(mct_p, ctrls, "ctrl_type", "Input:", true, ctrlSwitch);
                            menu.setItemWidth(300);
                            menu.setValueOffset(150);
                            menu.addKeyGrabMenuItem(&mp_p->left, "key1",        "Left.........");
                            menu.addKeyGrabMenuItem(&mp_p->right, "key2",       "Right........");
                            menu.addKeyGrabMenuItem(&mp_p->up, "key3",          "Up...........");
                            menu.addKeyGrabMenuItem(&mp_p->down, "key4",        "Down.........");
                            menu.addKeyGrabMenuItem(&mp_p->jump, "key5",        "Jump.........");
                            menu.addKeyGrabMenuItem(&mp_p->jump_alt, "key6",    "Alt-Jump....");
                            menu.addKeyGrabMenuItem(&mp_p->run, "key7",         "Run..........");
                            menu.addKeyGrabMenuItem(&mp_p->run_alt, "key8",     "Alt-Run.....");
                            menu.addKeyGrabMenuItem(&mp_p->drop, "key9",        "Drop.........");
                            menu.addKeyGrabMenuItem(&mp_p->start, "key10",      "Start........");
                        }
                        break;
        case menu_playepisode:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
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
                        WorldData world = FileFormats::OpenWorldFileHeader(file);
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
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
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
                        LevelData level = FileFormats::OpenLevelFileHeader(ConfigManager::dirs.worlds+file);
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
    QRect menuBox = menu.rect();
    menu.setPos(PGE_Window::Width/2-menuBox.width()/2, menuBox.y());
    qDebug()<<"Menuitem ID: "<<menustates[_menu].first << ", scrolling offset: "<<menustates[_menu].second;
    menu.setCurrentItem(menustates[_menu].first);
    menu.setOffset(menustates[_menu].second);
}
