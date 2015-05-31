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

#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include <QMap>
#include <QPair>
#include <QStack>
#include "scene.h"
#include <gui/pge_menu.h>
#include <common_features/pge_texture.h>
#include <common_features/simple_animator.h>
#include <common_features/point.h>
#include <controls/controller.h>

typedef QPair<int, int > menustate;

struct TitleScene_misc_img
{
    int x;
    int y;
    PGE_Texture t;
    SimpleAnimator a;
    int frmH;
};

struct PlayLevelResult
{
    QString levelfile;
};

struct PlayEpisodeResult
{
    QString worldfile;
    QString savefile;
    int character;
};

class TitleScene : public Scene
{
public:
    TitleScene();
    ~TitleScene();

    bool init();

    void update();
    void render();
    void renderMouse();
    int exec();
    void resetController();
    PGE_Menu menu;
    bool doExit;

    enum CurrentMenu
    {
        menu_main=0,
        menu_options,
        menu_playlevel,
        menu_playepisode,
        menu_playbattle,
        menu_opensave,
        menu_tests,
        menu_controls,
        menu_controls_plr1,
        menu_controls_plr2,
        menu_player,
        menu_volume,
        menu_dummy_and_big,//leave it!
        //For fetching
        menuFirst=menu_main,
        menuLast=menu_dummy_and_big
    };

    enum menuAnswer
    {
        ANSWER_EXIT=0,
        ANSWER_PLAYLEVEL,
        ANSWER_PLAYEPISODE,
        ANSWER_PLAYEPISODE_2P,
        ANSWER_PLAYBATTLE,
        ANSWER_CREDITS,
        ANSWER_LOADING,
        ANSWER_GAMEOVER
    };

    int numOfPlayers;
    PlayEpisodeResult result_episode; //play episode
    PlayLevelResult   result_level; //Play level/battle

    Controller *controller;

private:
    PGE_Point mousePos;
    CurrentMenu _currentMenu;
    void setMenu(CurrentMenu _menu);
    QMap<CurrentMenu, menustate> menustates;
    QStack<int > menuChain;

    PGE_Texture background;
    bool _bgIsLoaded;
    QVector<TitleScene_misc_img > imgs;

    PGE_Texture cursor;
    bool _cursorIsLoaded;
};

#endif // SCENE_TITLE_H
