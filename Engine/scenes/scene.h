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

#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_opengl.h>

class Scene
{
public:
    enum TypeOfScene
    {
        _Unknown=0,
        Intro,
        Title,
        Level,
        World,
        Credits,
        GameOver
    };

    Scene();
    Scene(TypeOfScene _type);
    virtual ~Scene();
    virtual void update();
    virtual void render();
    virtual void renderMouse();
    virtual int exec(); //scene's loop
    TypeOfScene type();

    /**************Fader**************/
    bool isOpacityFadding();
    bool fadeRunned;
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
    TypeOfScene sceneType;
};

#endif // SCENE_H
