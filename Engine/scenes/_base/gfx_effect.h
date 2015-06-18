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

#ifndef LVL_EFFECT_H
#define LVL_EFFECT_H

#include <data_configs/obj_effect.h>
#include <common_features/simple_animator.h>
#include <common_features/rectf.h>
#include <PGE_File_Formats/file_formats.h>

struct Scene_Effect_Phys
{
    Scene_Effect_Phys();
    float mix_vel_x;
    float mix_vel_y;
    float max_vel_x;
    float max_vel_y;
    float decelerate_x;
    float decelerate_y;
};

class Scene_Effect
{
public:
    Scene_Effect();
    Scene_Effect(const Scene_Effect &e);
    ~Scene_Effect();

    void init();

    float posX();
    float posY();
    bool finished();

    int direction;
    int frameStyle;

    bool animated;
    long animator_ID;

    bool _limit_delay;
    float _delay;

    float m_velocityX;
    float m_velocityY;

    float gravity;
    Scene_Effect_Phys phys_setup;

    PGE_RectF posRect;

    bool _finished;
    obj_effect * setup;//Global config
    PGE_Texture    texture;
    SimpleAnimator animator;

    void update(float ticks);
    void iterateStep(float ticks);
    float timeStep;

    void render(double camX=0.0, double camY=0.0);
};

#endif // LVL_EFFECT_H



