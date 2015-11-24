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

#include "../scene_level.h"

#include <audio/pge_audio.h>
#include <QTranslator>

void LevelScene::initPauseMenu1()
{
    _pauseMenu_opened=false;
    _pauseMenuID=1;
    _pauseMenu.setParentScene(this);
    _pauseMenu.construct(QTranslator::tr("Pause"), PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                         ConfigManager::setup_menu_box.box_padding,
                         ConfigManager::setup_menu_box.sprite);
    _pauseMenu.clearMenu();
    QStringList items;
    items<<QTranslator::tr("Continue");
    items<<QTranslator::tr("Save and continue");
    items<<QTranslator::tr("Save and quit");
    items<<QTranslator::tr("Exit without saving");
    _pauseMenu.addMenuItems(items);
    _pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    _pauseMenu.setMaxMenuItems(4);
    isPauseMenu=false;
    isTimeStopped=false;
}

void LevelScene::initPauseMenu2()
{
    _pauseMenu_opened=false;
    _pauseMenuID=2;
    _pauseMenu.setParentScene(this);
    _pauseMenu.construct(QTranslator::tr("Pause"), PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                         ConfigManager::setup_menu_box.box_padding,
                         ConfigManager::setup_menu_box.sprite);
    _pauseMenu.clearMenu();
    QStringList items;
    items<<QTranslator::tr("Continue");
    items<<QTranslator::tr("Quit");
    _pauseMenu.addMenuItems(items);
    _pauseMenu.setRejectSnd(obj_sound_role::MenuPause);
    _pauseMenu.setMaxMenuItems(4);
    isPauseMenu=false;
    isTimeStopped=false;
}

void LevelScene::processPauseMenu()
{
    if(!_pauseMenu_opened)
    {
        _pauseMenu.restart();
        _pauseMenu_opened=true;
        PGE_Audio::playSoundByRole(obj_sound_role::MenuPause);
    }
    else
    {
        _pauseMenu.update(uTickf);
        if(!_pauseMenu.isRunning())
        {
            if(_pauseMenuID==1)
            {
                switch(_pauseMenu.answer())
                {
                case PAUSE_Continue:
                    //do nothing!!
                break;
                case PAUSE_SaveCont:
                    //Save game state!
                    gameState->save();
                break;
                case PAUSE_SaveQuit:
                    //Save game state! and exit from episode
                    gameState->save();
                    setExiting(0, LvlExit::EXIT_MenuExit);
                    break;
                case PAUSE_Exit:
                    //Save game state! and exit from episode
                    setExiting(0, LvlExit::EXIT_MenuExit);
                break;
                default: break;
                }
            } else {
                switch(_pauseMenu.answer())
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
            }
            _pauseMenu_opened=false;
            isPauseMenu=false;
        }
    }
}


