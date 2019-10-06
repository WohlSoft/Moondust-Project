/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "scene_gameover.h"

#include <common_features/tr.h>
#include "../gui/pge_questionbox.h"
#include <data_configs/config_manager.h>
#include <gui/pge_menubox.h>
#include <settings/global_settings.h>
#include <Utils/maths.h>

GameOverScene::GameOverScene(): Scene(GameOver)
{
    player1Controller = g_AppSettings.openController(1);
}

GameOverScene::~GameOverScene()
{
    delete player1Controller;
}

void GameOverScene::update()
{
    Scene::update();
}

void GameOverScene::render()
{
    GlRenderer::clearScreen();
    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
}

int GameOverScene::exec()
{
    runVsyncValidator();

    PGE_QuestionBox continueOrQuit(this,
                                   //% "Game Over!"
                                   qtTrId("GAMEOVER_TITLE"),
                                   PGE_MenuBox::msg_info, PGE_Point(-1,-1),
                                   ConfigManager::setup_menu_box.box_padding,
                                   ConfigManager::setup_message_box.sprite);
    std::vector<std::string> items;
    //% "Continue"
    items.push_back(qtTrId("GAMEOVER_CONTINUE"));
    //% "Quit"
    items.push_back(qtTrId("GAMEOVER_EXIT"));
    continueOrQuit.addMenuItems(items);
    continueOrQuit.setRejectSnd(obj_sound_role::BlockSmashed);
    continueOrQuit.exec();

    if (continueOrQuit.answer() != PGE_Menu::npos)
        if (continueOrQuit.answer() == 0)
            return GameOverSceneResult::CONTINUE;

    return GameOverSceneResult::QUIT;
}

void GameOverScene::processEvents()
{
    Scene::processEvents();
}
