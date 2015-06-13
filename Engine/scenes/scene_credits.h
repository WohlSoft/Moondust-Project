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

#ifndef SCENE_CREDITS_H
#define SCENE_CREDITS_H

#include <QVector>
#include <common_features/pge_texture.h>
#include <common_features/simple_animator.h>

#include "scene.h"

#include <script/lua_credits_engine.h>

class CreditsScene_misc_img
{
public:
    CreditsScene_misc_img();
    ~CreditsScene_misc_img();
    CreditsScene_misc_img(const CreditsScene_misc_img &im);

    int x;
    int y;
    PGE_Texture t;
    SimpleAnimator a;
    int frmH;
};

class CreditsScene : public Scene
{
public:
    CreditsScene();
    ~CreditsScene();
    void init();

    void setWaitTime(int time);
    void exitFromScene();

    void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier);
    LuaEngine* getLuaEngine();
    void update();
    void render();
    int exec();

private:
    float _waitTimer;

    PGE_Texture background;
    QVector<CreditsScene_misc_img > imgs;

    LuaCreditsEngine luaEngine;
};

#endif // SCENE_CREDITS_H
