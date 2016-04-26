/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QDir>

#include <common_features/logger.h>
#include <audio/pge_audio.h>
#include <audio/SdlMusPlayer.h>
#include <graphics/window.h>
#include <gui/pge_msgbox.h>
#include <gui/pge_menubox.h>
#include <gui/pge_textinputbox.h>
#include <settings/global_settings.h>
#include <data_configs/config_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <gui/pge_questionbox.h>

#include "../scene_title.h"

#include <QApplication>

void TitleScene::processMenu()
{
    if(doExit) return;

    //Waiter in process
    switch(_currentMenu)
    {
        case menu_playlevel_wait:
        case menu_playbattle_wait:
        case menu_playepisode_wait:
            if(filefind_finished) {
                switch(_currentMenu)
                {
                    case menu_playepisode_wait: setMenu(menu_playepisode); return;
                    case menu_playlevel_wait: setMenu(menu_playlevel); return;
                    case menu_playbattle_wait: setMenu(menu_playbattle); return;
                    default: break;
                }
                return;
            }
            if(menu.isAccepted()) { menu.resetState(); }
            return;
            break;
        default:
            break;
    }

    if(!menu.isSelected()) return;

    if(menu.isAccepted())
    {
        menustates[_currentMenu].first = menu.currentItemI();
        menustates[_currentMenu].second = menu.offset();

        QString value = menu.currentItem().item_key;
        switch(_currentMenu)
        {
            case menu_main:
                if(value=="game1p")
                {
                    numOfPlayers=1;
                    menuChain.push(_currentMenu);
                    setMenu(menu_playepisode_wait);
                }
                else
                if(value=="game2p")
                {
                    numOfPlayers=2;
                    menuChain.push(_currentMenu);
                    setMenu(menu_playepisode_wait);
                }
                else
                if(value=="playlevel")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_playlevel_wait);
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
                    fader.setNull();
                    doExit=true;
                }
                else
                {
                    PGE_MsgBox msgBox(this, QString("Sorry, is not implemented yet..."),
                                      PGE_MsgBox::msg_warn);
                    fader.setRatio(0.5);
                    PGE_Window::setCursorVisibly(true);
                    msgBox.exec();
                    PGE_Window::setCursorVisibly(false);
                    fader.setNull();
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
                        fader.setFade(10, 1.0f, 0.06f);
                        doExit=true;
                        menu.resetState();
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
                    fader.setFade(10, 1.0f, 0.06f);
                    doExit=true;
                    menu.resetState();
                }
            break;
            case menu_playlevel_wait:
            case menu_playbattle_wait:
            case menu_playepisode_wait:
                {
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
                if(value=="testboxes")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_testboxes);
                }
                else
                if(value=="controls")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_controls);
                }
                else
                if(value=="videosetup")
                {
                    menuChain.push(_currentMenu);
                    setMenu(menu_videosettings);
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
            case menu_testboxes:
                if(value=="messagebox")
                {
                    PGE_MsgBox msg(this, "This is a small message box without texture\nЭто маленкая коробочка-сообщение без текстуры", PGE_BoxBase::msg_info_light);
                    msg.exec();

                    PGE_MsgBox msg2(this, "This is a small message box with texture\nЭто маленкая коробочка-сообщение с текстурой", PGE_BoxBase::msg_info, PGE_Point(-1,-1),
                                    ConfigManager::setup_message_box.box_padding,
                                    ConfigManager::setup_message_box.sprite);
                    msg2.exec();
                    menu.resetState();
                }
                else
                if(value=="menubox")
                {
                    PGE_MenuBox menubox(this, "Select something", PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                                         ConfigManager::setup_menu_box.box_padding,
                                         ConfigManager::setup_message_box.sprite);
                    QStringList items;
                    items<<"Menuitem 1";
                    items<<"Menuitem 2";
                    items<<"Menuitem 3";
                    items<<"Menuitem 4";
                    items<<"Menuitem 5";
                    items<<"Menuitem 6";
                    items<<"Menuitem 7";
                    items<<"Menuitem 8";
                    items<<"Menuitem 9";
                    items<<"Menuitem 10";
                    items<<"Menuitem 11";
                    menubox.addMenuItems(items);
                    menubox.setRejectSnd(obj_sound_role::MenuPause);
                    menubox.setMaxMenuItems(5);
                    menubox.exec();

                    if(menubox.answer()>=0)
                    {
                        PGE_MsgBox msg(this, "Your answer is:\n"+items[menubox.answer()], PGE_BoxBase::msg_info_light, PGE_Point(-1,-1),
                                ConfigManager::setup_message_box.box_padding,
                                ConfigManager::setup_message_box.sprite);
                        msg.exec();
                    }

                    menu.resetState();
                }
                else
                if(value=="inputbox")
                {
                    PGE_TextInputBox text(this, "Type a text", PGE_BoxBase::msg_info_light,
                                         PGE_Point(-1,-1),
                                          ConfigManager::setup_message_box.box_padding,
                                          ConfigManager::setup_message_box.sprite);
                    text.exec();

                    PGE_MsgBox msg(this, "Typed a text:\n"+text.inputText(), PGE_BoxBase::msg_info_light);
                    msg.exec();
                    menu.resetState();
                }
                else if (value=="questionbox")
                {
                    PGE_QuestionBox hor(this, "AHHHH?", PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                                        ConfigManager::setup_menu_box.box_padding,
                                        ConfigManager::setup_message_box.sprite);
                    QStringList items;
                    items<<"One";
                    items<<"Two";
                    items<<"Three";
                    items<<"Four";
                    items<<"Five";
                    items<<"Six";
                    items<<"Seven";
                    hor.addMenuItems(items);
                    hor.setRejectSnd(obj_sound_role::BlockSmashed);
                    hor.exec();
                    if(hor.answer()>=0)
                    {
                        PGE_MsgBox msg(this, "Answer on so dumb question is:\n"+items[hor.answer()], PGE_BoxBase::msg_info_light, PGE_Point(-1,-1),
                                ConfigManager::setup_message_box.box_padding,
                                ConfigManager::setup_message_box.sprite);
                        msg.exec();
                    }
                    menu.resetState();
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
            g_AppSettings.apply();
            g_AppSettings.save();
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
            //% "1 Player Game"
            menu.addMenuItem("game1p", qtTrId("MAINMENU_1_PLAYER_GAME"));
            //% "2 Player Game"
            menu.addMenuItem("game2p", qtTrId("MAINMENU_2_PLAYER_GAME"));
            //% "Play level"
            menu.addMenuItem("playlevel", qtTrId("MAINMENU_PLAY_SINGLE_LEVEL"));
            //% "Options"
            menu.addMenuItem("Options", qtTrId("MAINMENU_OPTIONS"));
            //% "Exit"
            menu.addMenuItem("Exit", qtTrId("MAINMENU_EXIT"));
        break;
            case menu_options:
                menu.setPos(260,284);
                menu.setItemsNumber(9);
                //% "Test of screens"
                menu.addMenuItem("tests", qtTrId("MAINMENU_OPTIONS_TEST_SCRS"));
                //% "Test of message boxes"
                menu.addMenuItem("testboxes", qtTrId("MAINMENU_OPTIONS_TEST_MSGS"));
                //% "Player controlling"
                menu.addMenuItem("controls", qtTrId("MAINMENU_OPTIONS_CONTROLLS"));
                //% "Video settings"
                menu.addMenuItem("videosetup", qtTrId("MAINMENU_OPTIONS_VIDEO"));
                                                                                      //% "Music volume"
                menu.addIntMenuItem(&g_AppSettings.volume_music, 0, 128, "vlm_music", qtTrId("MAINMENU_OPTIONS_MUS_VOL"), false,
                                    []()->void{ PGE_MusPlayer::MUS_changeVolume(g_AppSettings.volume_music); });
                                                                                      //% "Sound volume"
                menu.addIntMenuItem(&g_AppSettings.volume_sound, 0, 128, "vlm_sound", qtTrId("MAINMENU_OPTIONS_SND_VOL"), false);
                                                                                      //% "Full Screen mode"
                menu.addBoolMenuItem(&g_AppSettings.fullScreen, "full_screen", qtTrId("MAINMENU_OPTIONS_FULLSCR"),
                                     []()->void{ PGE_Window::setFullScreen(g_AppSettings.fullScreen); }
                                     );
            break;
                case menu_tests:
                    menu.setPos(300, 350);
                    menu.setItemsNumber(5);
                                                    //% "Credits"
                    menu.addMenuItem("credits",     qtTrId("TESTSCR_CREDITS"));
                                                    //% "Loading screen"
                    menu.addMenuItem("loading",     qtTrId("TESTSCR_LOADING_SCREEN"));
                                                    //% "Game over screen"
                    menu.addMenuItem("gameover",    qtTrId("TESTSCR_GAMEOVER"));
                break;
                case menu_testboxes:
                    menu.setPos(300, 350);
                    menu.setItemsNumber(5);
                                                    //% "Message box"
                    menu.addMenuItem("messagebox",  qtTrId("TEST_MSGBOX"));
                                                    //% "Menu box"
                    menu.addMenuItem("menubox",     qtTrId("TEST_MENUBOX"));
                                                    //% "Text Input box"
                    menu.addMenuItem("inputbox",    qtTrId("TEST_TEXTINPUTBOX"));
                                                    //% "Question box"
                    menu.addMenuItem("questionbox", qtTrId("TEST_QUESTION_BOX"));
                break;
                    case menu_videosettings:
                        menu.setPos(300, 350);
                        menu.setItemsNumber(5);
                                                                                       //% "Show debug info"
                        menu.addBoolMenuItem(&g_AppSettings.showDebugInfo, "dbg_flag", qtTrId("VIDEO_SHOW_DEBUG"));
                                                                                     //% "Enable frame-skip"
                        menu.addBoolMenuItem(&g_AppSettings.frameSkip, "frame_skip", qtTrId("VIDEO_ENABLE_FRSKIP"));
                                                                            //% "Enable V-Sync"
                        menu.addBoolMenuItem(&g_AppSettings.vsync, "vsync", qtTrId("VIDEO_ENABLE_VSYNC"),
                                                 [this]()->void{
                                                     PGE_Window::vsync=g_AppSettings.vsync;
                                                     PGE_Window::toggleVSync(g_AppSettings.vsync);
                                                     g_AppSettings.timeOfFrame=PGE_Window::TimeOfFrame;
                                                     menu.setEnabled("phys_step", !PGE_Window::vsync);
                                                 },
                                                 PGE_Window::vsyncIsSupported
                                             );

                                                                                            //% "Frame time (ms.)"
                        menu.addIntMenuItem(&g_AppSettings.timeOfFrame, 2, 16, "phys_step", qtTrId("VIDEO_FRAME_TIME"), false,
                                                [this]()->void{
                                                    if(!PGE_Window::vsync)
                                                    {
                                                        PGE_Window::TicksPerSecond=1000.0f/g_AppSettings.timeOfFrame;
                                                        PGE_Window::TimeOfFrame=g_AppSettings.timeOfFrame;
                                                        g_AppSettings.TicksPerSecond=1000.0f/g_AppSettings.timeOfFrame;
                                                        this->updateTickValue();
                                                    } else {
                                                        g_AppSettings.timeOfFrame=PGE_Window::TimeOfFrame;
                                                    }
                                                },
                                                !PGE_Window::vsync
                                            );
                    break;
                    case menu_controls:
                        menu.setPos(300, 350);
                        menu.setItemsNumber(5);
                                                         //% "Player 1 controls"
                        menu.addMenuItem("control_plr1", qtTrId("CONTROLLS_SETUP_PLAYER1"));
                                                         //% "Player 2 controls"
                        menu.addMenuItem("control_plr2", qtTrId("CONTROLLS_SETUP_PLAYER2"));
                    break;
                        case menu_controls_plr1:
                        case menu_controls_plr2:
                        {

                        KeyMap *mp_p;
                        int *mct_p=0;
                        SDL_Joystick* jdev=NULL;
                        std::function<void()> ctrlSwitch;

                        if(_menu==menu_controls_plr1)
                        {
                            ctrlSwitch = [this]()->void{
                                setMenu(menu_controls_plr1);
                                };
                            mct_p = &g_AppSettings.player1_controller;
                            if((*mct_p>=0)&&(*mct_p<g_AppSettings.player1_joysticks.size()))
                            {
                                if(*mct_p<g_AppSettings.joysticks.size())
                                    jdev        = g_AppSettings.joysticks[*mct_p];
                                mp_p         = &g_AppSettings.player1_joysticks[*mct_p];
                            }
                            else
                                mp_p = &g_AppSettings.player1_keyboard;
                        }
                        else{
                            ctrlSwitch = [this]()->void{
                                setMenu(menu_controls_plr2);
                                };
                            mct_p  = &g_AppSettings.player2_controller;
                            if((*mct_p>=0)&&(*mct_p<g_AppSettings.player2_joysticks.size()))
                            {
                                if(*mct_p<g_AppSettings.joysticks.size())
                                    jdev        = g_AppSettings.joysticks[*mct_p];
                                mp_p = &g_AppSettings.player2_joysticks[*mct_p];
                            }
                            else
                                mp_p = &g_AppSettings.player2_keyboard;
                        }

                            menu.setPos(300, 216);
                            menu.setItemsNumber(11);
                            QList<NamedIntItem> ctrls;
                            NamedIntItem controller;
                            controller.value=-1;
                                                //% "Keyboard"
                            controller.label=   qtTrId("PLAYER_CONTROLS_SETUP_KEYBOARD");
                            ctrls.push_back(controller);
                            for(int i=0;i<g_AppSettings.joysticks.size();i++)
                            {
                                controller.value=i;
                                                    //% "Joystick: %1"
                                controller.label = qtTrId("PLAYER_CONTROLS_SETUP_JOYSTICK").arg( SDL_JoystickName(g_AppSettings.joysticks[i]) );
                                ctrls.push_back(controller);
                            }
                                                                                //% "Input:"
                            menu.addNamedIntMenuItem(mct_p, ctrls, "ctrl_type", qtTrId("PLAYER_CONTROLS_SETUP_INPUT_TYPE"), true, ctrlSwitch);
                            menu.setItemWidth(300);
                            menu.setValueOffset(150);
                            menu.addKeyGrabMenuItem(&mp_p->left, "key1",        "Left.........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->right, "key2",       "Right........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->up, "key3",          "Up...........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->down, "key4",        "Down.........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->jump, "key5",        "Jump.........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->jump_alt, "key6",    "Alt-Jump....", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->run, "key7",         "Run..........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->run_alt, "key8",     "Alt-Run.....", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->drop, "key9",        "Drop.........", jdev);
                            menu.setValueOffset(210);
                            menu.addKeyGrabMenuItem(&mp_p->start, "key10",      "Start........", jdev);
                            menu.setValueOffset(210);
                        }
                        break;
        case menu_playepisode_wait:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
                                                     //% "Please wait..."
                menu.addMenuItem("waitinginprocess", qtTrId("MSG_PLEASEWAIT"));
                filefind_finished=false;
                filefind_folder=ConfigManager::dirs.worlds;
                filefind_thread=SDL_CreateThread( findEpisodes, "EpisodeFinderThread", NULL);
            }
            break;
        case menu_playepisode:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
                //Build list of episodes
                for(int i=0;i<filefind_found_files.size();i++)
                {
                    QPair<QString, QString >&item = filefind_found_files[i];
                    bool enabled=true;
                    if(i==0) enabled = (item.first != "noworlds");
                    menu.addMenuItem(item.first, item.second, []()->void{}, enabled);
                }
                menu.sort();
            }
        break;
        case menu_playlevel_wait:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
                                                     //% "Please wait..."
                menu.addMenuItem("waitinginprocess", qtTrId("MSG_PLEASEWAIT"));
                filefind_finished=false;
                filefind_folder=ConfigManager::dirs.worlds;
                filefind_thread=SDL_CreateThread( findLevels, "LevelFinderThread", NULL);
            }
        break;
        case menu_playlevel:
            {
                menu.setPos(300, 350);
                menu.setItemsNumber(5);
                //Build list of levels
                for(int i=0;i<filefind_found_files.size();i++)
                {
                    QPair<QString, QString >&item = filefind_found_files[i];
                    bool enabled=true;
                    if(i==0) enabled = (item.first != "noworlds");
                    menu.addMenuItem(item.first, item.second, []()->void{}, enabled);
                }
                menu.sort();
            }
        break;
    default:
        break;
    }
    PGE_Rect menuBox = menu.rect();
    menu.setPos(PGE_Window::Width/2-menuBox.width()/2, menuBox.y());
    LogDebug( QString("Menuitem ID: ") + menustates[_menu].first + ", scrolling offset: " + menustates[_menu].second );
    menu.setCurrentItem(menustates[_menu].first);
    menu.setOffset(menustates[_menu].second);
}




int TitleScene::findEpisodes( void* )
{
    filefind_found_files.clear();
    QDir worlddir(filefind_folder);
    QStringList filter;
    filter << "*.wld" << "*.wldx";
    QStringList files;
    QStringList folders = worlddir.entryList(QDir::Dirs);

    foreach(QString folder, folders)
    {
        QString path = filefind_folder+folder;
        QDir episodedir(path);
        QStringList worlds = episodedir.entryList(filter);
        foreach(QString world, worlds)
        {
            files << filefind_folder+folder+"/"+world;
        }
    }

    if(files.isEmpty())
    {
        QPair<QString,QString > file;
        file.first = "noworlds";
                      //% "<episodes not found>"
        file.second = qtTrId("MSG_EPISODES_NOT_FOUND");
        filefind_found_files.push_back(file);
    }
    else
    {
        WorldData world;
        foreach(QString filename, files)
        {
            if( FileFormats::OpenWorldFileHeader( filename, world ) )
            {
                QString title = world.EpisodeTitle;
                QPair<QString,QString > file;
                file.first = filename;
                file.second = (title.isEmpty()?QFileInfo(filename).fileName():title);
                filefind_found_files.push_back(file);
            }
        }
    }
    filefind_finished=true;
    return 0;
}

int TitleScene::findLevels( void* )
{
    //Build list of casual levels
    QDir leveldir(filefind_folder);
    QStringList filter;
    filter<<"*.lvl" << "*.lvlx";
    QStringList files = leveldir.entryList(filter);

    filefind_found_files.clear();//Clean up old stuff

    if(files.isEmpty())
    {
        QPair<QString,QString > file;
        file.first = "noworlds";
                      //% "<levels not found>"
        file.second = qtTrId("MSG_LEVELS_NOT_FOUND");
        filefind_found_files.push_back(file);
    }
    else
    {
        LevelData level;
        foreach(QString file, files)
        {
            if( FileFormats::OpenLevelFileHeader(filefind_folder+file, level) )
            {
                QString title = level.LevelName;
                QPair<QString,QString > filex;
                filex.first = filefind_folder+file;
                filex.second = (title.isEmpty()?file:title);
                filefind_found_files.push_back(filex);
            }
        }
    }
    filefind_finished=true;

    return 0;
}

