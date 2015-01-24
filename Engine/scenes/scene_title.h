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

#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include <QVector>
#include <common_features/pge_texture.h>
#include <common_features/simple_animator.h>

#include "scene.h"

struct TitleScene_misc_img
{
    int x;
    int y;
    PGE_Texture t;
    SimpleAnimator a;
    int frmH;
};

class TitleScene : public Scene
{
public:
    TitleScene();
    ~TitleScene();
    void init();

    void setWaitTime(unsigned int time);
    void render();
    int exec();

private:
    unsigned int _waitTimer;

    PGE_Texture background;
    QVector<TitleScene_misc_img > imgs;
};

#endif // SCENE_TITLE_H
