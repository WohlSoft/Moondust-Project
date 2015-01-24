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

#ifndef SCENE_INTRO_H
#define SCENE_INTRO_H

#include <QMap>
#include <QPair>
#include <QStack>
#include "scene.h"
#include "intro/pge_menu.h"
#include <common_features/pge_texture.h>

typedef QPair<int, int > menustate;

class IntroScene : public Scene
{
public:
    IntroScene();
    ~IntroScene();
    void update();
    void render();
    void renderMouse();
    int exec();
    PGE_Menu menu;
    bool doExit;

    enum CurrentMenu
    {
        menu_main=0,
        menu_options,
        menu_playlevel,
        menu_tests,
        menu_dummy_and_big,
        //For fetching
        menuFirst=menu_main,
        menuLast=menu_dummy_and_big
    };

    enum menuAnswer
    {
        ANSWER_EXIT=0,
        ANSWER_CREDITS,
        ANSWER_TITLE,
        ANSWER_GAMEOVER
    };

private:
    QPoint mousePos;
    CurrentMenu _currentMenu;
    void setMenu(CurrentMenu _menu);
    QMap<CurrentMenu, menustate> menustates;
    QStack<int > menuChain;

    PGE_Texture cursor;
    bool _cursorIsLoaded;
};

#endif // SCENE_INTRO_H
