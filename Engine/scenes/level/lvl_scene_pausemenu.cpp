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

#include "../scene_level.h"

#include <audio/pge_audio.h>
#include <common_features/tr.h>

void LevelScene::initPauseMenu1()
{
    m_pauseMenu_opened=false;
    m_pauseMenuID=1;
    m_pauseMenu.setParentScene(this);


    m_pauseMenu.construct(
                //% "Pause"
                qtTrId("LVL_MENU_PAUSE_TTL"),
                PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                ConfigManager::setup_menu_box.box_padding,
                ConfigManager::setup_menu_box.sprite);
    m_pauseMenu.clearMenu();

    std::vector<std::string> items;
    //% "Continue"
    items.push_back(qtTrId("LVL_MENU_PAUSE_CONTINUE"));
    //% "Save and continue"
    items.push_back(qtTrId("LVL_MENU_PAUSE_CONTINUESAVE"));
    //% "Save and quit"
    items.push_back(qtTrId("LVL_MENU_PAUSE_EXITSAVE"));
    //% "Exit without saving"
    items.push_back(qtTrId("LVL_MENU_PAUSE_EXITNOSAVE"));
    m_pauseMenu.addMenuItems(items);
    m_pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    m_pauseMenu.setMaxMenuItems(4);
    m_isPauseMenu=false;
    m_isTimeStopped=false;
}

void LevelScene::initPauseMenu2()
{
    m_pauseMenu_opened=false;
    m_pauseMenuID=2;
    m_pauseMenu.setParentScene(this);
    m_pauseMenu.construct(
                //% "Pause"
                qtTrId("LVL_MENU_PAUSE_TTL"),
                PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                ConfigManager::setup_menu_box.box_padding,
                ConfigManager::setup_menu_box.sprite);
    m_pauseMenu.clearMenu();

    std::vector<std::string> items;
    //% "Continue"
    items.push_back(qtTrId("LVL_MENU_PAUSE_CONTINUE"));
    //% "Quit"
    items.push_back(qtTrId("LVL_MENU_PAUSE_EXIT"));

    m_pauseMenu.addMenuItems(items);
    m_pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    m_pauseMenu.setMaxMenuItems(4);
    m_isPauseMenu=false;
    m_isTimeStopped=false;
}

void LevelScene::initPauseMenu3()
{
    m_pauseMenu_opened=false;
    m_pauseMenuID=3;
    m_pauseMenu.setParentScene(this);
    m_pauseMenu.construct(
                //% "Pause"
                qtTrId("LVL_MENU_PAUSE_TTL"),
                PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                ConfigManager::setup_menu_box.box_padding,
                ConfigManager::setup_menu_box.sprite);
    m_pauseMenu.clearMenu();

    std::vector<std::string> items;
    //% "Continue"
    items.push_back(qtTrId("LVL_MENU_PAUSE_CONTINUE"));
    //% "Replay level"
    items.push_back(qtTrId("LVL_MENU_PAUSE_REPLAY"));
    //% "Quit"
    items.push_back(qtTrId("LVL_MENU_PAUSE_EXIT"));

    m_pauseMenu.addMenuItems(items);
    m_pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    m_pauseMenu.setMaxMenuItems(3);
    m_isPauseMenu=false;
    m_isTimeStopped=false;
}

void LevelScene::processPauseMenu()
{
    if(!m_pauseMenu_opened)
    {
        m_pauseMenu.restart();
        m_pauseMenu_opened=true;
        PGE_Audio::playSoundByRole(obj_sound_role::MenuPause);
    }
    else
    {
        m_pauseMenu.update(uTickf);
        if(!m_pauseMenu.isRunning())
        {
            switch(m_pauseMenuID)
            {
            case 1:
                switch(m_pauseMenu.answer())
                {
                case PAUSE_Continue:
                    //do nothing!!
                break;
                case PAUSE_SaveCont:
                    //Save game state!
                    m_gameState->save();
                break;
                case PAUSE_SaveQuit:
                    //Save game state! and exit from episode
                    m_gameState->save();
                    setExiting(0, LvlExit::EXIT_MenuExit);
                    break;
                case PAUSE_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, LvlExit::EXIT_MenuExit);
                break;
                default: break;
                }
            break;
            case 2:
                switch(m_pauseMenu.answer())
                {
                case PAUSE_2_Continue:
                    //do nothing!!
                break;
                case PAUSE_2_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, LvlExit::EXIT_MenuExit);
                break;
                default: break;
                }
                break;
            case 3:
                switch(m_pauseMenu.answer())
                {
                case PAUSE_3_Continue:
                    //do nothing!!
                break;
                case PAUSE_3_Replay:
                    //Replay same level again
                    setExiting(0, LvlExit::EXIT_ReplayRequest);
                break;
                case PAUSE_3_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, LvlExit::EXIT_MenuExit);
                break;
                default: break;
                }
                break;
            }
            m_pauseMenu_opened=false;
            m_isPauseMenu=false;
        }
    }
}


