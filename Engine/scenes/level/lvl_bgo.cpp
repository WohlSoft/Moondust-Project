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

#include "lvl_bgo.h"
#include "../../data_configs/config_manager.h"

#include <graphics/gl_renderer.h>

LVL_Bgo::LVL_Bgo() : PGE_Phys_Object()
{
    type = LVLBGO;
    data=FileFormats::dummyLvlBgo();
    animated=false;
    animator_ID=0;
}

LVL_Bgo::~LVL_Bgo()
{}

void LVL_Bgo::init()
{
    setSize(texture.w, texture.h);
    setPos(data.x, data.y);
    collide=COLLISION_NONE;
}

void LVL_Bgo::render(double camX, double camY)
{
    AniPos x(0,1);

    if(animated) //Get current animated frame
        x = ConfigManager::Animator_BGO[animator_ID].image();
    GlRenderer::renderTexture(&texture, posX()-camX, posY()-camY, width, height, x.first, x.second);
}
