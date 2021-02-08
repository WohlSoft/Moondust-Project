/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <fmt/fmt_qformat.h>

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
    switch(int(m_currentMenu))
    {
    case menu_playlevel_wait:
    case menu_playbattle_wait:
    case menu_playepisode_wait:
        if(m_filefind_finished)
        {
            switch(int(m_currentMenu))
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
        if(m_menu.isAccepted())
            m_menu.resetState();
        return;
    default:
        break;
    }

    if(!m_menu.isSelected()) return;

    if(m_menu.isAccepted())
    {
        m_menustates[m_currentMenu].first = m_menu.currentItemI();
        m_menustates[m_currentMenu].second = m_menu.offset();

        std::string value = m_menu.currentItem().item_key;
        switch(int(m_currentMenu))
        {
        case menu_main:
            if(value == "game1p")
            {
                m_numOfPlayers = 1;
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_playepisode_wait);
            }
            else if(value == "game2p")
            {
                m_numOfPlayers = 2;
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_playepisode_wait);
            }
            else if(value == "playlevel")
            {
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_playlevel_wait);
            }
            else if(value == "Options")
            {
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_options);
            }
            else if(value == "Exit")
            {
                m_exitCode = ANSWER_EXIT;
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
                m_menu.resetState();
            }
            break;
        case menu_playepisode:
        {
            if(value == "noworlds")
            {
                //do nothing!
                m_menu.resetState();
            }
            else
            {
                m_result_episode.worldfile = value;
                m_result_episode.character = 0;
                m_result_episode.savefile = "save1.savx";
                if(m_numOfPlayers > 1)
                    m_exitCode = ANSWER_PLAYEPISODE_2P;
                else
                    m_exitCode = ANSWER_PLAYEPISODE;
                m_fader.setFade(10, 1.0, 0.06);
                m_doExit = true;
                m_menu.resetState();
            }
        }
        break;
        case menu_playlevel:
            if(value == "nolevel")
            {
                //do nothing!
                m_menu.resetState();
            }
            else
            {
                m_result_level.levelfile = value;
                m_exitCode = ANSWER_PLAYLEVEL;
                m_fader.setFade(10, 1.0, 0.06);
                m_doExit = true;
                m_menu.resetState();
            }
            break;
        case menu_playlevel_wait:
        case menu_playbattle_wait:
        case menu_playepisode_wait:
        {
            m_menu.resetState();
        }
        break;
        case menu_options:
            if(value == "tests")
            {
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_tests);
            }
            else if(value == "testboxes")
            {
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_testboxes);
            }
            else if(value == "controls")
            {
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_controls);
            }
            else if(value == "videosetup")
            {
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_videosettings);
            }
            else
            {
                PGE_MsgBox msgBox(this, "Dummy", PGE_MsgBox::msg_warn);
                msgBox.exec();
                m_menu.resetState();
            }
            break;
        case menu_controls:
            if(value == "control_plr1")
            {
                m_menuChain.push_back(m_currentMenu);
                setMenu(menu_controls_plr1);
            }
            else if(value == "control_plr2")
            {
                m_menuChain.push_back(m_currentMenu);
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
                m_exitCode = ANSWER_CREDITS;
                m_doExit = true;
            }
            else if(value == "loading")
            {
                m_exitCode = ANSWER_LOADING;
                m_doExit = true;
            }
            else if(value == "gameover")
            {
                m_exitCode = ANSWER_GAMEOVER;
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
                m_menu.resetState();
            }
            else if(value == "menubox")
            {
                PGE_MenuBox menuBox = PGE_MenuBox(this, "Select something", PGE_MenuBox::msg_info, PGE_Point(-1, -1),
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
                menuBox.addMenuItems(items);
                menuBox.setRejectSnd(obj_sound_role::MenuPause);
                menuBox.setMaxMenuItems(5);
                menuBox.exec();

                if(menuBox.answer() != PGE_Menu::npos)
                {
                    PGE_MsgBox msg(this, "Your answer is:\n" + items[menuBox.answer()], PGE_BoxBase::msg_info_light, PGE_Point(-1, -1),
                                   ConfigManager::setup_message_box.box_padding,
                                   ConfigManager::setup_message_box.sprite);
                    msg.exec();
                }

                m_menu.resetState();
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
                m_menu.resetState();
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
                if(hor.answer() != PGE_Menu::npos)
                {
                    PGE_MsgBox msg(this, "Answer on so dumb question is:\n" + items[hor.answer()], PGE_BoxBase::msg_info_light, PGE_Point(-1, -1),
                                   ConfigManager::setup_message_box.box_padding,
                                   ConfigManager::setup_message_box.sprite);
                    msg.exec();
                }
                m_menu.resetState();
            }
            break;
        case menu_dummy_and_big:
            m_menu.resetState();
            break;
        default:
            break;

        }
    }
    else
    {
        if(m_currentMenu == menu_main)
        {
            m_menu.reset();
            m_menu.setCurrentItem(4);
        }
        else
        {
            if(m_currentMenu == menu_options)
            {
                g_AppSettings.apply();
                g_AppSettings.save();
                PGE_Audio::playSoundByRole(obj_sound_role::Bonus1up);
            }

            if(m_menu.isKeyGrabbing())
                m_menu.reset();
            else if(m_menuChain.size() > 0)
            {
                m_menu.reset();
                setMenu((CurrentMenu)m_menuChain.back());
                m_menuChain.pop_back();
            }
        }
    }
}


void TitleScene::setMenu(TitleScene::CurrentMenu targetMenu)
{
    if(targetMenu < menuFirst) return;
    if(targetMenu > menuLast) return;

    m_currentMenu = targetMenu;
    m_menu.clear();
    m_menu.setTextLenLimit(22);
    switch(int(targetMenu))
    {
    case menu_main:
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //% "1 Player Game"
        m_menu.addMenuItem("game1p", qtTrId("MAINMENU_1_PLAYER_GAME"));
        //% "2 Player Game"
        m_menu.addMenuItem("game2p", qtTrId("MAINMENU_2_PLAYER_GAME"));
        //% "Play level"
        m_menu.addMenuItem("playlevel", qtTrId("MAINMENU_PLAY_SINGLE_LEVEL"));
        //% "Options"
        m_menu.addMenuItem("Options", qtTrId("MAINMENU_OPTIONS"));
        //% "Exit"
        m_menu.addMenuItem("Exit", qtTrId("MAINMENU_EXIT"));
        break;
    case menu_options:
        m_menu.setPos(260, 284);
        m_menu.setItemsNumber(9);
        //% "Test of screens"
        m_menu.addMenuItem("tests", qtTrId("MAINMENU_OPTIONS_TEST_SCRS"));
        //% "Test of message boxes"
        m_menu.addMenuItem("testboxes", qtTrId("MAINMENU_OPTIONS_TEST_MSGS"));
        //% "Player controlling"
        m_menu.addMenuItem("controls", qtTrId("MAINMENU_OPTIONS_CONTROLLS"));
        //% "Video settings"
        m_menu.addMenuItem("videosetup", qtTrId("MAINMENU_OPTIONS_VIDEO"));
        //% "Music volume"
        m_menu.addIntMenuItem(&g_AppSettings.volume_music, 0, 128, "vlm_music", qtTrId("MAINMENU_OPTIONS_MUS_VOL"), false,
                              []()->void{ PGE_MusPlayer::setVolume(g_AppSettings.volume_music); });
        //% "Sound volume"
        m_menu.addIntMenuItem(&g_AppSettings.volume_sound, 0, 128, "vlm_sound", qtTrId("MAINMENU_OPTIONS_SND_VOL"), false);
#ifndef __ANDROID__
        //% "Full Screen mode"
        m_menu.addBoolMenuItem(&g_AppSettings.fullScreen, "full_screen", qtTrId("MAINMENU_OPTIONS_FULLSCR"),
                               []()->void{ PGE_Window::setFullScreen(g_AppSettings.fullScreen); });
#endif
        break;
    case menu_tests:
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //% "Credits"
        m_menu.addMenuItem("credits",     qtTrId("TESTSCR_CREDITS"));
        //% "Loading screen"
        m_menu.addMenuItem("loading",     qtTrId("TESTSCR_LOADING_SCREEN"));
        //% "Game over screen"
        m_menu.addMenuItem("gameover",    qtTrId("TESTSCR_GAMEOVER"));
        break;
    case menu_testboxes:
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //% "Message box"
        m_menu.addMenuItem("messagebox",  qtTrId("TEST_MSGBOX"));
        //% "Menu box"
        m_menu.addMenuItem("menubox",     qtTrId("TEST_MENUBOX"));
        //% "Text Input box"
        m_menu.addMenuItem("inputbox",    qtTrId("TEST_TEXTINPUTBOX"));
        //% "Question box"
        m_menu.addMenuItem("questionbox", qtTrId("TEST_QUESTION_BOX"));
        break;
    case menu_videosettings:
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //% "Show debug info"
        m_menu.addBoolMenuItem(&g_AppSettings.showDebugInfo, "dbg_flag", qtTrId("VIDEO_SHOW_DEBUG"));
        //% "Enable frame-skip"
        m_menu.addBoolMenuItem(&g_AppSettings.frameSkip, "frame_skip", qtTrId("VIDEO_ENABLE_FRSKIP"));
        //% "Enable V-Sync"
        m_menu.addBoolMenuItem(&g_AppSettings.vsync, "vsync", qtTrId("VIDEO_ENABLE_VSYNC"),
                               [this]()->void
        {
            PGE_Window::vsync = g_AppSettings.vsync;
            PGE_Window::toggleVSync(g_AppSettings.vsync);
            g_AppSettings.timeOfFrame = PGE_Window::frameDelay;
            m_menu.setEnabled("phys_step", !PGE_Window::vsync);
        },
        PGE_Window::vsyncIsSupported
                              );

        //% "Frame time (ms.)"
        m_menu.addIntMenuItem(&g_AppSettings.timeOfFrame, 2, 17, "phys_step", qtTrId("VIDEO_FRAME_TIME"), false,
                              [this]()->void
        {
            if(!PGE_Window::vsync)
            {
                PGE_Window::frameRate = 1000.0 / double(g_AppSettings.timeOfFrame);
                PGE_Window::frameDelay = g_AppSettings.timeOfFrame;
                g_AppSettings.frameRate = 1000.0 / double(g_AppSettings.timeOfFrame);
                this->updateTickValue();
            }
            else
                g_AppSettings.timeOfFrame = PGE_Window::frameDelay;
        }, !PGE_Window::vsync);
        break;
    case menu_controls:
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //% "Player 1 controls"
        m_menu.addMenuItem("control_plr1", qtTrId("CONTROLLS_SETUP_PLAYER1"));
        //% "Player 2 controls"
        m_menu.addMenuItem("control_plr2", qtTrId("CONTROLLS_SETUP_PLAYER2"));
        break;
    case menu_controls_plr1:
    case menu_controls_plr2:
    {

        KeyMap *mp_p;
        int *mct_p = nullptr;
        SDL_Joystick *jDev = nullptr;
        std::function<void()> ctrlSwitch;

        if(targetMenu == menu_controls_plr1)
        {
            ctrlSwitch = [this]()->void
            {
                setMenu(menu_controls_plr1);
            };
            mct_p = &g_AppSettings.player1_controller;
            if((*mct_p >= 0) && (*mct_p < static_cast<int>(g_AppSettings.player1_joysticks.size())))
            {
                if(*mct_p < static_cast<int>(g_AppSettings.joysticks.size()))
                    jDev = g_AppSettings.joysticks[size_t(*mct_p)];
                mp_p = &g_AppSettings.player1_joysticks[size_t(*mct_p)];
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
                    jDev = g_AppSettings.joysticks[size_t(*mct_p)];
                mp_p = &g_AppSettings.player2_joysticks[size_t(*mct_p)];
            }
            else
                mp_p = &g_AppSettings.player2_keyboard;
        }

        m_menu.setPos(300, 216);
        m_menu.setItemsNumber(11);
        std::vector<NamedIntItem> controllers;
        NamedIntItem controller;
        controller.value = -1;
        //% "Keyboard"
        controller.label =   qtTrId("PLAYER_CONTROLS_SETUP_KEYBOARD");
        controllers.push_back(controller);
        for(size_t i = 0; i < g_AppSettings.joysticks.size(); i++)
        {
            controller.value = int(i);
            //FIXME: missing in-string arguments support
            //% "Joystick: %1"
            controller.label = fmt::qformat(qtTrId("PLAYER_CONTROLS_SETUP_JOYSTICK"), SDL_JoystickName(g_AppSettings.joysticks[i]));
            controllers.push_back(controller);
        }
        //% "Input:"
        m_menu.addNamedIntMenuItem(mct_p, controllers, "ctrl_type", qtTrId("PLAYER_CONTROLS_SETUP_INPUT_TYPE"), true, ctrlSwitch);
        m_menu.setItemWidth(300);
        m_menu.setValueOffset(150);
        m_menu.addKeyGrabMenuItem(&mp_p->left, "key1",        "Left.........", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->right, "key2",       "Right........", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->up, "key3",          "Up...........", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->down, "key4",        "Down.........", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->jump, "key5",        "Jump.........", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->jump_alt, "key6",    "Alt-Jump....", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->run, "key7",         "Run..........", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->run_alt, "key8",     "Alt-Run.....", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->drop, "key9",        "Drop.........", jDev);
        m_menu.setValueOffset(210);
        m_menu.addKeyGrabMenuItem(&mp_p->start, "key10",      "Start........", jDev);
        m_menu.setValueOffset(210);
    }
    break;
    case menu_playepisode_wait:
    {
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //% "Please wait..."
        m_menu.addMenuItem("waitinginprocess", qtTrId("MSG_PLEASEWAIT"));
        m_filefind_finished = false;
        m_filefind_folders.clear();
#if !defined(__APPLE__)
        m_filefind_folders.push_back(ConfigManager::dirs.worldsProgram);
#endif
        if(!ConfigManager::dirs.worldsUser.empty())
            m_filefind_folders.push_back(ConfigManager::dirs.worldsUser);
#ifndef PGE_NO_THREADING
        m_filefind_thread = SDL_CreateThread(findEpisodes, "EpisodeFinderThread", nullptr);
#else
        findEpisodes(nullptr);
#endif
    }
    break;
    case menu_playepisode:
    {
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //Build list of episodes
        for(size_t i = 0; i < m_filefind_found_files.size(); i++)
        {
            std::pair<std::string, std::string > &item = m_filefind_found_files[i];
            bool enabled = true;
            if(i == 0) enabled = (item.first != "noworlds");
            m_menu.addMenuItem(item.first, item.second, []()->void{}, enabled);
        }
        m_menu.sort();
    }
    break;
    case menu_playlevel_wait:
    {
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //% "Please wait..."
        m_menu.addMenuItem("waitinginprocess", qtTrId("MSG_PLEASEWAIT"));
        m_filefind_finished = false;
        m_filefind_folders.clear();
#if !defined(__APPLE__)
        m_filefind_folders.push_back(ConfigManager::dirs.worldsProgram);
#endif
        if(!ConfigManager::dirs.worldsUser.empty())
            m_filefind_folders.push_back(ConfigManager::dirs.worldsUser);
#ifndef PGE_NO_THREADING
        m_filefind_thread = SDL_CreateThread(findLevels, "LevelFinderThread", nullptr);
#else
        findLevels(nullptr);
#endif
    }
    break;
    case menu_playlevel:
    {
        m_menu.setPos(300, 350);
        m_menu.setItemsNumber(5);
        //Build list of levels
        for(size_t i = 0; i < m_filefind_found_files.size(); i++)
        {
            std::pair<std::string, std::string > &item = m_filefind_found_files[i];
            bool enabled = true;
            if(i == 0) enabled = (item.first != "noworlds");
            m_menu.addMenuItem(item.first, item.second, []()->void{}, enabled);
        }
        m_menu.sort();
    }
    break;
    default:
        break;
    }

    PGE_Rect menuBox = m_menu.rect();
    m_menu.setPos(PGE_Window::Width / 2 - menuBox.width() / 2, menuBox.y());
    pLogDebug("Menuitem ID: %d, scrolling offset: %d", m_menustates[targetMenu].first, m_menustates[targetMenu].second);
    m_menu.setCurrentItem(m_menustates[targetMenu].first);
    m_menu.setOffset(m_menustates[targetMenu].second);
}




int TitleScene::findEpisodes(void *)
{
    m_filefind_found_files.clear();
    bool has_files_added = false;

    for(const auto &dir : m_filefind_folders)
    {
        DirMan worldDir(dir);
        std::vector<std::string> files;
        std::vector<std::string> folders;
        worldDir.getListOfFolders(folders);

        for(std::string &folder : folders)
        {
            std::string path = dir + folder;
            DirMan episodeDir(path);
            std::vector<std::string> worlds;
            episodeDir.getListOfFiles(worlds, {".wld", ".wldx"});
            for(std::string &world : worlds)
                files.push_back(dir + folder + "/" + world);
        }

        if(!files.empty())
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
                    m_filefind_found_files.push_back(file);
                    has_files_added = true;
                }
                else
                {
                    pLogWarning("Failed to parse world map header %s while listing available levels with error [%s] at line %d with line data [%s]",
                                filename.c_str(),
                                world.meta.ERROR_info.c_str(),
                                world.meta.ERROR_linenum,
                                world.meta.ERROR_linedata.c_str());
                }
            }
        }
    }

    if(!has_files_added)
    {
        std::pair<std::string, std::string > file;
        file.first = "noworlds";
        //% "<episodes not found>"
        file.second = qtTrId("MSG_EPISODES_NOT_FOUND");
        m_filefind_found_files.push_back(file);
    }

    m_filefind_finished = true;
    return 0;
}

int TitleScene::findLevels(void *)
{
    m_filefind_found_files.clear();//Clean up old stuff
    bool has_files_added = false;

    //Build list of casual levels
    for(const auto &dir : m_filefind_folders)
    {
        DirMan levelDir(dir);

        std::vector<std::string> files;
        levelDir.getListOfFiles(files, {".lvl", ".lvlx"});

        if(!files.empty())
        {
            LevelData level;
            for(std::string &file : files)
            {
                if(FileFormats::OpenLevelFileHeader(dir + file, level))
                {
                    std::string title = level.LevelName;
                    std::pair<std::string, std::string > fileX;
                    fileX.first = dir + file;
                    fileX.second = (title.empty() ? file : title);
                    m_filefind_found_files.push_back(fileX);
                    has_files_added = true;
                }
                else
                {
                    pLogWarning("Failed to parse level header %s%s while listing available levels with error [%s] at line %d with line data [%s]",
                                dir.c_str(),
                                file.c_str(),
                                level.meta.ERROR_info.c_str(),
                                level.meta.ERROR_linenum,
                                level.meta.ERROR_linedata.c_str());
                }
            }
        }
    }

    if(!has_files_added)
    {
        std::pair<std::string, std::string > file;
        file.first = "noworlds";
        //% "<levels not found>"
        file.second = qtTrId("MSG_LEVELS_NOT_FOUND");
        m_filefind_found_files.push_back(file);
    }

    m_filefind_finished = true;

    return 0;
}
