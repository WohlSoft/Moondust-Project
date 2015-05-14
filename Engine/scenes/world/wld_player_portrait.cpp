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

#include "wld_player_portrait.h"
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>

WorldScene_Portrait::WorldScene_Portrait()
{
    posX=0;
    posY=0;
    posX_render=0;
    posY_render=0;
    setup = ConfigManager::playable_characters[1];
    state_cur = ConfigManager::playable_characters[1].states[1];
    frameW = 0;
    frameH = 0;
}

WorldScene_Portrait::WorldScene_Portrait(int CharacterID, int stateID, int _posX, int _posY, QString ani, int framedelay, int dir)
{
    posX=_posX;
    posY=_posY;
    setup = ConfigManager::playable_characters[CharacterID];
    state_cur = ConfigManager::playable_characters[CharacterID].states[stateID];

    posX_render=posX-state_cur.width/2;
    posY_render=posY-state_cur.height;

    long tID = ConfigManager::getLvlPlayerTexture(CharacterID, stateID);
    if( tID >= 0 )
    {
        texture = ConfigManager::level_textures[tID];
        frameW = ConfigManager::level_textures[tID].w / setup.matrix_width;
        frameH = ConfigManager::level_textures[tID].h / setup.matrix_height;
    }
    else return;

    animator.setSize(setup.matrix_width, setup.matrix_height);
    animator.installAnimationSet(state_cur.sprite_setup);
    animator.switchAnimation(animator.toEnum(ani), dir, framedelay);
}

WorldScene_Portrait::WorldScene_Portrait(const WorldScene_Portrait &pt)
{
    this->setup = pt.setup;
    this->state_cur = pt.state_cur;
    this->animator = pt.animator;
    this->texture = pt.texture;
    this->frameW = pt.frameW;
    this->frameH = pt.frameH;
    this->posX = pt.posX;
    this->posY = pt.posY;
    this->posX_render = pt.posX_render;
    this->posY_render = pt.posY_render;
}

WorldScene_Portrait::~WorldScene_Portrait()
{}

void WorldScene_Portrait::render()
{
    QRectF tPos = animator.curFrame();
    QPointF Ofs = animator.curOffset();

    QRectF player = QRectF( posX_render - Ofs.x(),
                            posY_render-Ofs.y(),
                            frameW,
                            frameH
                            );
    GlRenderer::renderTexture(&texture,
                              player.x(),
                              player.y(),
                              player.width(),
                              player.height(),
                              tPos.top(),
                              tPos.bottom(),
                              tPos.left(),
                              tPos.right());
}

void WorldScene_Portrait::update(int ticks)
{
    animator.tickAnimation(ticks);
}

