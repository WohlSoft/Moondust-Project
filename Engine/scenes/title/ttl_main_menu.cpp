/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <DirManager/dirman.h>
#include <Utils/files.h>

#include <common_features/logger.h>
#include <common_features/tr.h>
#include <audio/pge_audio.h>
#include <audio/play_music.h>
#include <graphics/window.h>
#include <gui/pge_msgbox.h>
#include <gui/pge_menubox.h>
#include <gui/pge_textinputbox.h>
#include <settings/global_settings.h>
#include <data_configs/config_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <gui/pge_questionbox.h>

#include "../scene_title.h"

void TitleScene::processMenu()
{
    if(m_doExit)
        return;

    //Waiter in process
    switch(_currentMenu)
    {
    case menu_playlevel_wait:
    case menu_playbattle_wait:
    case menu_playepisode_wait:
        if(filefind_finished)
        {
            switch(_currentMenu)
            {
            case menu_playepisode_wait:
                setMenu(menu_playepisode);
                return;
            case menu_playlevel_wait:
                setMenu(menu_playlevel);
                return;
            case menu_playbattle_wait:
                setMenu(menu_playbattle);
                return;
            default:
                break;
            }
            return;
        }
        if(menu.isAccepted())
            menu.resetState();
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

        std::string value = menu.currentItem().item_key;
        switch(_currentMenu)
        {
        case menu_main:
            if(value == "game1p")
            {
                numOfPlayers = 1;
                menuChain.push_back(_currentMenu);
                setMenu(menu_playepisode_wait);
            }
            else if(value == "game2p")
            {
                numOfPlayers = 2;
                menuChain.push_back(_currentMenu);
                setMenu(menu_playepisode_wait);
            }
            else if(value == "playlevel")
            {
                menuChain.push_back(_currentMenu);
                setMenu(menu_playlevel_wait);
            }
            else if(value == "Options")
            {
                menuChain.push_back(_currentMenu);
                setMenu(menu_options);
            }
            else if(value == "Exit")
            {
                ret = ANSWER_EXIT;
                m_fader.setNull();
                m_doExit = true;
            }
            else
            {
                PGE_MsgBox msgBox(this, "Sorry, is not implemented yet...", PGE_MsgBox::msg_warn);
                m_fader.setRatio(0.5);
                PGE_Window::setCursorVisibly(true);
                msgBox.exec();
                PGE_Window::setCursorVisibly(false);
                m_fader.setNull();
                menu.resetState();
            }
            break;
        case menu_playepisode:
        {
            if(value == "noworlds")
            {
                //do nothing!
                menu.resetState();
            }
            else
            {
                result_episode.worldfile = value;
                result_episode.character = 0;
                result_episode.savefile = "save1.savx";
                if(numOfPlayers > 1)
                    ret = ANSWER_PLAYEPISODE_2P;
                else
                    ret = ANSWER_PLAYEPISODE;
                m_fader.setFade(10, 1.0, 0.06);
                m_doExit = true;
                menu.resetState();
            }
        }
        break;
        case menu_playlevel:
            if(value == "nolevel")
            {
                //do nothing!
                menu.resetState();
            }
            else
            {
                result_level.levelfile = value;
                ret = ANSWER_PLAYLEVEL;
                m_fader.setFade(10, 1.0, 0.06);
                m_doExit = true;
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
            if(value == "tests")
            {
                menuChain.push_back(_currentMenu);
                setMenu(menu_tests);
            }
            else if(value == "testboxes")
            {
                menuChain.push_back(_currentMenu);
                setMenu(menu_testboxes);
            }
            else if(value == "controls")
            {
                menuChain.push_back(_currentMenu);
                setMenu(menu_controls);
            }
            else if(value == "videosetup")
            {
                menuChain.push_back(_currentMenu);
                setMenu(menu_videosettings);
            }
            else
            {
                PGE_MsgBox msgBox(this, "Dummy", PGE_MsgBox::msg_warn);
                msgBox.exec();
                menu.resetState();
            }
            break;
        case menu_controls:
            if(value == "control_plr1")
            {
                menuChain.push_back(_currentMenu);
                setMenu(menu_controls_plr1);
            }
            else if(value == "control_plr2")
            {
                menuChain.push_back(_currentMenu);
                setMenu(menu_controls_plr2);
            }
            break;
        case menu_controls_plr1:

            break;
        case menu_controls_plr2:

            break;
        case menu_tests:
            if(value == "credits")
            {
                ret = ANSWER_CREDITS;
                m_doExit = true;
            }
            else if(value == "loading")
            {
                ret = ANSWER_LOADING;
                m_doExit = true;
            }
            else if(value == "gameover")
            {
                ret = ANSWER_GAMEOVER;
                m_doExit = true;
            }
            break;
        case menu_testboxes:
            if(value == "messagebox")
            {
                PGE_MsgBox msg(this, "This is a small message box without texture\nЭто маленкая коробочка-сообщение без текстуры", PGE_BoxBase::msg_info_light);
                msg.exec();

                PGE_MsgBox msg2(this, "This is a small message box with texture\nЭто маленкая коробочка-сообщение с текстурой", PGE_BoxBase::msg_info, PGE_Point(-1, -1),
                                ConfigManager::setup_message_box.box_padding,
                                ConfigManager::setup_message_box.sprite);
                msg2.exec();
                menu.resetState();
            }
            else if(value == "menubox")
            {
                PGE_MenuBox menubox(this, "Select something", PGE_MenuBox::msg_info, PGE_Point(-1, -1),
                                    ConfigManager::setup_menu_box.box_padding,
                                    ConfigManager::setup_message_box.sprite);
                std::vector<std::string> items =
                {
                    "Menuitem 1",
                    "Menuitem 2",
                    "Menuitem 3",
                    "Menuitem 4",
                    "Menuitem 5",
                    "Menuitem 6",
                    "Menuitem 7",
                    "Menuitem 8",
                    "Menuitem 9",
                    "Menuitem 10",
                    "Menuitem 11"
                };
                menubox.addMenuItems(items);
                menubox.setRejectSnd(obj_sound_role::MenuPause);
                menubox.setMaxMenuItems(5);
                menubox.exec();

                if(menubox.answer() >= 0)
                {
                    PGE_MsgBox msg(this, "Your answer is:\n" + items[menubox.answer()], PGE_BoxBase::msg_info_light, PGE_Point(-1, -1),
                                   ConfigManager::setup_message_box.box_padding,
                                   ConfigManager::setup_message_box.sprite);
                    msg.exec();
                }

                menu.resetState();
            }
            else if(value == "inputbox")
            {
                PGE_TextInputBox text(this, "Type a text", PGE_BoxBase::msg_info_light,
                                      PGE_Point(-1, -1),
                                      ConfigManager::setup_message_box.box_padding,
                                      ConfigManager::setup_message_box.sprite);
                text.exec();

                PGE_MsgBox msg(this, "Typed a text:\n" + text.inputText(), PGE_BoxBase::msg_info_light);
                msg.exec();
                menu.resetState();
            }
            else if(value == "questionbox")
            {
                PGE_QuestionBox hor(this, "AHHHH?", PGE_MenuBox::msg_info, PGE_Point(-1, -1),
                                    ConfigManager::setup_menu_box.box_padding,
                                    ConfigManager::setup_message_box.sprite);
                std::vector<std::string> items = {"One", "Two", "Three", "Four", "Five", "Six", "Seven"};
                hor.addMenuItems(items);
                hor.setRejectSnd(obj_sound_role::BlockSmashed);
                hor.exec();
                if(hor.answer() >= 0)
                {
                    PGE_MsgBox msg(this, "Answer on so dumb question is:\n" + items[hor.answer()], PGE_BoxBase::msg_info_light, PGE_Point(-1, -1),
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
            else if(menuChain.size() > 0)
            {
                menu.reset();
                setMenu((CurrentMenu)menuChain.back());
                menuChain.pop_back();
            }
            break;
        }
    }
}


void TitleScene::setMenu(TitleScene::CurrentMenu _menu)
{
    if(_menu < menuFirst) return;
    if(_menu > menuLast) return;

    _currentMenu = _menu;
    menu.clear();
    menu.setTextLenLimit(22);
    switch(_menu)
    {
    case menu_main:
        menu.setPos(300, 350);
        menu.setItemsNumber(5);
        //% "1 Player Game"
        menu.addMenuItem("game1p", qsTrId("MAINMENU_1_PLAYER_GAME"));
        //% "2 Player Game"
        menu.addMenuItem("game2p", qsTrId("MAINMENU_2_PLAYER_GAME"));
        //% "Play level"
        menu.addMenuItem("playlevel", qsTrId("MAINMENU_PLAY_SINGLE_LEVEL"));
        //% "Options"
        menu.addMenuItem("Options", qsTrId("MAINMENU_OPTIONS"));
        //% "Exit"
        menu.addMenuItem("Exit", qsTrId("MAINMENU_EXIT"));
        break;
    case menu_options:
        menu.setPos(260, 284);
        menu.setItemsNumber(9);
        //% "Test of screens"
        menu.addMenuItem("tests", qsTrId("MAINMENU_OPTIONS_TEST_SCRS"));
        //% "Test of message boxes"
        menu.addMenuItem("testboxes", qsTrId("MAINMENU_OPTIONS_TEST_MSGS"));
        //% "Player controlling"
        menu.addMenuItem("controls", qsTrId("MAINMENU_OPTIONS_CONTROLLS"));
        //% "Video settings"
        menu.addMenuItem("videosetup", qsTrId("MAINMENU_OPTIONS_VIDEO"));
        //% "Music volume"
        menu.addIntMenuItem(&g_AppSettings.volume_music, 0, 128, "vlm_music", qsTrId("MAINMENU_OPTIONS_MUS_VOL"), false,
                            []()->void{ PGE_MusPlayer::setVolume(g_AppSettings.volume_music); });
        //% "Sound volume"
        menu.addIntMenuItem(&g_AppSettings.volume_sound, 0, 128, "vlm_sound", qsTrId("MAINMENU_OPTIONS_SND_VOL"), false);
        //% "Full Screen mode"
        menu.addBoolMenuItem(&g_AppSettings.fullScreen, "full_screen", qsTrId("MAINMENU_OPTIONS_FULLSCR"),
                             []()->void{ PGE_Window::setFullScreen(g_AppSettings.fullScreen); }
                            );
        break;
    case menu_tests:
        menu.setPos(300, 350);
        menu.setItemsNumber(5);
        //% "Credits"
        menu.addMenuItem("credits",     qsTrId("TESTSCR_CREDITS"));
        //% "Loading screen"
        menu.addMenuItem("loading",     qsTrId("TESTSCR_LOADING_SCREEN"));
        //% "Game over screen"
        menu.addMenuItem("gameover",    qsTrId("TESTSCR_GAMEOVER"));
        break;
    case menu_testboxes:
        menu.setPos(300, 350);
        menu.setItemsNumber(5);
        //% "Message box"
        menu.addMenuItem("messagebox",  qsTrId("TEST_MSGBOX"));
        //% "Menu box"
        menu.addMenuItem("menubox",     qsTrId("TEST_MENUBOX"));
        //% "Text Input box"
        menu.addMenuItem("inputbox",    qsTrId("TEST_TEXTINPUTBOX"));
        //% "Question box"
        menu.addMenuItem("questionbox", qsTrId("TEST_QUESTION_BOX"));
        break;
    case menu_videosettings:
        menu.setPos(300, 350);
        menu.setItemsNumber(5);
        //% "Show debug info"
        menu.addBoolMenuItem(&g_AppSettings.showDebugInfo, "dbg_flag", qsTrId("VIDEO_SHOW_DEBUG"));
        //% "Enable frame-skip"
        menu.addBoolMenuItem(&g_AppSettings.frameSkip, "frame_skip", qsTrId("VIDEO_ENABLE_FRSKIP"));
        //% "Enable V-Sync"
        menu.addBoolMenuItem(&g_AppSettings.vsync, "vsync", qsTrId("VIDEO_ENABLE_VSYNC"),
                             [this]()->void
        {
            PGE_Window::vsync = g_AppSettings.vsync;
            PGE_Window::toggleVSync(g_AppSettings.vsync);
            g_AppSettings.timeOfFrame = PGE_Window::TimeOfFrame;
            menu.setEnabled("phys_step", !PGE_Window::vsync);
        },
        PGE_Window::vsyncIsSupported
                            );

        //% "Frame time (ms.)"
        menu.addIntMenuItem(&g_AppSettings.timeOfFrame, 2, 17, "phys_step", qsTrId("VIDEO_FRAME_TIME"), false,
                            [this]()->void
        {
            if(!PGE_Window::vsync)
            {
                PGE_Window::TicksPerSecond = 1000.0f / g_AppSettings.timeOfFrame;
                PGE_Window::TimeOfFrame = g_AppSettings.timeOfFrame;
                g_AppSettings.TicksPerSecond = 1000.0f / g_AppSettings.timeOfFrame;
                this->updateTickValue();
            }
            else
                g_AppSettings.timeOfFrame = PGE_Window::TimeOfFrame;
        },
        !PGE_Window::vsync
                           );
        break;
    case menu_controls:
        menu.setPos(300, 350);
        menu.setItemsNumber(5);
        //% "Player 1 controls"
        menu.addMenuItem("control_plr1", qsTrId("CONTROLLS_SETUP_PLAYER1"));
        //% "Player 2 controls"
        menu.addMenuItem("control_plr2", qsTrId("CONTROLLS_SETUP_PLAYER2"));
        break;
    case menu_controls_plr1:
    case menu_controls_plr2:
    {

        KeyMap *mp_p;
        int *mct_p = 0;
        SDL_Joystick *jdev = NULL;
        std::function<void()> ctrlSwitch;

        if(_menu == menu_controls_plr1)
        {
            ctrlSwitch = [this]()->void
            {
                setMenu(menu_controls_plr1);
            };
            mct_p = &g_AppSettings.player1_controller;
            if((*mct_p >= 0) && (*mct_p < static_cast<int>(g_AppSettings.player1_joysticks.size())))
            {
                if(*mct_p < static_cast<int>(g_AppSettings.joysticks.size()))
                    jdev        = g_AppSettings.joysticks[*mct_p];
                mp_p         = &g_AppSettings.player1_joysticks[*mct_p];
            }
            else
                mp_p = &g_AppSettings.player1_keyboard;
        }
        else
        {
            ctrlSwitch = [this]()->void
            {
                setMenu(menu_controls_plr2);
            };
            mct_p  = &g_AppSettings.player2_controller;
            if((*mct_p >= 0) && (*mct_p < static_cast<int>(g_AppSettings.player2_joysticks.size())))
            {
                if(*mct_p < static_cast<int>(g_AppSettings.joysticks.size()))
                    jdev        = g_AppSettings.joysticks[*mct_p];
                mp_p = &g_AppSettings.player2_joysticks[*mct_p];
            }
            else
                mp_p = &g_AppSettings.player2_keyboard;
        }

        menu.setPos(300, 216);
        menu.setItemsNumber(11);
        std::vector<NamedIntItem> ctrls;
        NamedIntItem controller;
        controller.value = -1;
        //% "Keyboard"
        controller.label =   qsTrId("PLAYER_CONTROLS_SETUP_KEYBOARD");
        ctrls.push_back(controller);
        for(size_t i = 0; i < g_AppSettings.joysticks.size(); i++)
        {
            controller.value = i;
            //FIXME: missing in-string arguments support
            //% "Joystick: %1"
            controller.label = qsTrId("PLAYER_CONTROLS_SETUP_JOYSTICK") + SDL_JoystickName(g_AppSettings.joysticks[i]);
            ctrls.push_back(controller);
        }
        //% "Input:"
        menu.addNamedIntMenuItem(mct_p, ctrls, "ctrl_type", qsTrId("PLAYER_CONTROLS_SETUP_INPUT_TYPE"), true, ctrlSwitch);
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
        menu.addMenuItem("waitinginprocess", qsTrId("MSG_PLEASEWAIT"));
        filefind_finished = false;
        filefind_folder = ConfigManager::dirs.worlds;
        filefind_thread = SDL_CreateThread(findEpisodes, "EpisodeFinderThread", NULL);
    }
    break;
    case menu_playepisode:
    {
        menu.setPos(300, 350);
        menu.setItemsNumber(5);
        //Build list of episodes
        for(size_t i = 0; i < filefind_found_files.size(); i++)
        {
            std::pair<std::string, std::string > &item = filefind_found_files[i];
            bool enabled = true;
            if(i == 0) enabled = (item.first != "noworlds");
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
        menu.addMenuItem("waitinginprocess", qsTrId("MSG_PLEASEWAIT"));
        filefind_finished = false;
        filefind_folder = ConfigManager::dirs.worlds;
        filefind_thread = SDL_CreateThread(findLevels, "LevelFinderThread", NULL);
    }
    break;
    case menu_playlevel:
    {
        menu.setPos(300, 350);
        menu.setItemsNumber(5);
        //Build list of levels
        for(size_t i = 0; i < filefind_found_files.size(); i++)
        {
            std::pair<std::string, std::string > &item = filefind_found_files[i];
            bool enabled = true;
            if(i == 0) enabled = (item.first != "noworlds");
            menu.addMenuItem(item.first, item.second, []()->void{}, enabled);
        }
        menu.sort();
    }
    break;
    default:
        break;
    }

    PGE_Rect menuBox = menu.rect();
    menu.setPos(PGE_Window::Width / 2 - menuBox.width() / 2, menuBox.y());
    pLogDebug("Menuitem ID: %d, scrolling offset: %d", menustates[_menu].first, menustates[_menu].second);
    menu.setCurrentItem(menustates[_menu].first);
    menu.setOffset(menustates[_menu].second);
}




int TitleScene::findEpisodes(void *)
{
    filefind_found_files.clear();
    DirMan worlddir(filefind_folder);
    std::vector<std::string> files;
    std::vector<std::string> folders;
    worlddir.getListOfFolders(folders);

    for(std::string &folder : folders)
    {
        std::string path = filefind_folder + folder;
        DirMan episodedir(path);
        std::vector<std::string> worlds;
        episodedir.getListOfFiles(worlds, {".wld", ".wldx"});
        for(std::string &world : worlds)
            files.push_back(filefind_folder + folder + "/" + world);
    }

    if(files.empty())
    {
        std::pair<std::string, std::string > file;
        file.first = "noworlds";
        //% "<episodes not found>"
        file.second = qsTrId("MSG_EPISODES_NOT_FOUND");
        filefind_found_files.push_back(file);
    }
    else
    {
        WorldData world;
        for(std::string &filename : files)
        {
            if(FileFormats::OpenWorldFileHeader(filename, world))
            {
                std::string title = world.EpisodeTitle;
                std::pair<std::string, std::string > file;
                file.first = filename;
                file.second = (title.empty() ? Files::basename(filename) : title);
                filefind_found_files.push_back(file);
            }
        }
    }
    filefind_finished = true;
    return 0;
}

int TitleScene::findLevels(void *)
{
    //Build list of casual levels
    DirMan leveldir(filefind_folder);

    std::vector<std::string> files;
    leveldir.getListOfFiles(files, {".lvl", ".lvlx"});

    filefind_found_files.clear();//Clean up old stuff

    if(files.empty())
    {
        std::pair<std::string, std::string > file;
        file.first = "noworlds";
        //% "<levels not found>"
        file.second = qsTrId("MSG_LEVELS_NOT_FOUND");
        filefind_found_files.push_back(file);
    }
    else
    {
        LevelData level;
        for(std::string &file : files)
        {
            if(FileFormats::OpenLevelFileHeader(filefind_folder + file, level))
            {
                std::string title = level.LevelName;
                std::pair<std::string, std::string > filex;
                filex.first = filefind_folder + file;
                filex.second = (title.empty() ? file : title);
                filefind_found_files.push_back(filex);
            }
        }
    }
    filefind_finished = true;

    return 0;
}
