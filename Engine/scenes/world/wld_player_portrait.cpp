/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "wld_player_portrait.h"
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>

WorldScene_Portrait::WorldScene_Portrait():
    m_setup(ConfigManager::playable_characters[1]),
    m_state_cur(ConfigManager::playable_characters[1].states[1])
{}

WorldScene_Portrait::WorldScene_Portrait(unsigned long CharacterID,
    unsigned long stateID,
    int posX,
    int posY,
    std::string ani,
    int framedelay,
    int dir)
{
    init(CharacterID, stateID, posX, posY, std::move(ani), framedelay, dir);
}

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
    if(m_isValid)
        m_animator.switchAnimation(m_animator.toEnum(ani), dir, framedelay);
}

void WorldScene_Portrait::render()
{
    if(!m_isValid)
        return;

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
