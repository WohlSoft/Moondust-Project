/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

WorldScene_Portrait::WorldScene_Portrait():
    m_setup(ConfigManager::playable_characters[1]),
    m_state_cur(ConfigManager::playable_characters[1].states[1])
{}

WorldScene_Portrait::WorldScene_Portrait(
    unsigned long CharacterID,
    unsigned long stateID,
    int posX,
    int posY,
    QString ani,
    int framedelay,
    int dir)
{
    init(CharacterID, stateID, posX, posY, ani.toStdString(), framedelay, dir);
}

WorldScene_Portrait::~WorldScene_Portrait()
{}

void WorldScene_Portrait::init(unsigned long CharacterID,
                               unsigned long stateID,
                               int posX,
                               int posY,
                               std::string ani,
                               int framedelay,
                               int dir)
{
    m_posX = posX;
    m_posY = posY;
    m_setup = ConfigManager::playable_characters[CharacterID];
    m_state_cur = ConfigManager::playable_characters[CharacterID].states[stateID];
    m_posX_render = m_posX - m_state_cur.width / 2;
    m_posY_render = m_posY - m_state_cur.height;
    m_frameW = 0;
    m_frameH = 0;
    int tID = ConfigManager::getLvlPlayerTexture(CharacterID, stateID);
    if(tID >= 0)
    {
        m_texture   = ConfigManager::level_textures[tID];
        m_frameW    = ConfigManager::level_textures[tID].w / m_setup.matrix_width;
        m_frameH    = ConfigManager::level_textures[tID].h / m_setup.matrix_height;
    }
    m_animator.setSize(m_setup.matrix_width, m_setup.matrix_height);
    m_isValid = m_animator.installAnimationSet(m_state_cur.sprite_setup);
    m_animator.switchAnimation(m_animator.toEnum(ani), dir, framedelay);
}

void WorldScene_Portrait::render()
{
    PGE_RectF tPos = m_animator.curFrame();
    PGE_PointF Ofs = m_animator.curOffset();
    PGE_RectF player;
    player.setRect(m_posX_render - Ofs.x(),
                   m_posY_render - Ofs.y(),
                   m_frameW,
                   m_frameH
                  );
    GlRenderer::renderTexture(&m_texture,
                              static_cast<float>(player.x()),
                              static_cast<float>(player.y()),
                              static_cast<float>(player.width()),
                              static_cast<float>(player.height()),
                              static_cast<float>(tPos.top()),
                              static_cast<float>(tPos.bottom()),
                              static_cast<float>(tPos.left()),
                              static_cast<float>(tPos.right()));
}

void WorldScene_Portrait::update(double ticks)
{
    m_animator.tickAnimation(ticks);
}

bool WorldScene_Portrait::isValid() const
{
    return m_isValid;
}
