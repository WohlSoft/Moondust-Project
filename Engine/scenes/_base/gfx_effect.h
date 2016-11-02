/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
    double min_vel_x;
    double min_vel_y;
    double max_vel_x;
    double max_vel_y;
    double decelerate_x;
    double decelerate_y;
};

class Scene_Effect
{
    public:
        Scene_Effect();
        Scene_Effect(const Scene_Effect &e);
        ~Scene_Effect();

        void init();

        double posX();
        double posY();

        bool finished();

        double m_startupDelay;

        long double m_zIndex;

        int m_direction;
        int m_frameStyle;

        bool m_limitLifeTime;
        double m_lifeTime;

        double m_velocityX;
        double m_velocityY;

        double m_gravity;
        Scene_Effect_Phys m_phys_setup;

        PGE_RectF m_posRect;

        bool m_finished;
        obj_effect     *m_setup;//Global config
        PGE_Texture     m_texture;
        bool            m_limitLoops;
        SimpleAnimator  m_animator;

        void update(double ticks);
        void iterateStep(double ticks);
        static const double timeStep;

        void render(double camX = 0.0, double camY = 0.0);
};

#endif // LVL_EFFECT_H


