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

#include <common_features/pge_texture.h>

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_timer.h>

#include "scene.h"
class TitleScene : public Scene
{
public:
    TitleScene();
    ~TitleScene();
    void init();

    void setWaitTime(unsigned int time);
    void render();
    int exec();

    /**************Fader**************/
    float fader_opacity;
    float target_opacity;
    float fade_step;
    int fadeSpeed;
    void setFade(int speed, float target, float step);
    static unsigned int nextOpacity(unsigned int x, void *p);
    void fadeStep();
    SDL_TimerID fader_timer_id;
    /**************Fader**************/

private:
    unsigned int _waitTimer;

    PGE_Texture background;
};

#endif // SCENE_TITLE_H
