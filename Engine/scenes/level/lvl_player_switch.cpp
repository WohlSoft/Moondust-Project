/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_player_switch.h"
#include <data_configs/config_manager.h>
#include "../scene_level.h"

CharacterSwitchState::CharacterSwitchState() :
    CharacterID(0), animator_p(NULL)
{}

CharacterBlockState::CharacterBlockState() :
    CharacterID(0), animator_p(NULL)
{}

CharacterSwitcherEngine::CharacterSwitcherEngine() :
    m_scene(NULL)
{}

CharacterSwitcherEngine::CharacterSwitcherEngine(LevelScene *scene):
    m_scene(scene)
{}

void CharacterSwitcherEngine::setScene(LevelScene *scene)
{
    m_scene = scene;
}

void CharacterSwitcherEngine::buildSwitch(obj_block &blk)
{
    if(blk.setup.plSwitch_Button)
    {
        CharacterSwitchState swst;
        swst.CharacterID  = blk.setup.plSwitch_Button_id;
        swst.sequence_on  = blk.setup.plSwitch_frames_true;
        swst.sequence_off = blk.setup.plSwitch_frames_false;

        if(blk.animator_ID < int32_t(ConfigManager::Animator_Blocks.size()))
            swst.animator_p = &ConfigManager::Animator_Blocks[blk.animator_ID];

        m_switches.push_back(swst);
    }
}

void CharacterSwitcherEngine::buildBrick(obj_block &blk)
{
    if(blk.setup.plFilter_Block)
    {
        CharacterBlockState swst;
        swst.CharacterID  = blk.setup.plFilter_Block_id;
        swst.sequence_on  = blk.setup.plFilter_frames_true;
        swst.sequence_off = blk.setup.plFilter_frames_false;

        if(blk.animator_ID < int32_t(ConfigManager::Animator_Blocks.size()))
            swst.animator_p = &ConfigManager::Animator_Blocks[blk.animator_ID];

        m_blocks.push_back(swst);
    }
}

void CharacterSwitcherEngine::refreshState()
{
    if(m_scene)
    {
        for(size_t i = 0; i < m_switches.size(); i++)
        {
            CharacterSwitchState &sw = m_switches[i];
            unsigned long needed = static_cast<unsigned long>(sw.CharacterID);
            bool found = false;

            for(int j = 1; j <= m_scene->m_numberOfPlayers; j++)
            {
                if(needed == m_scene->m_gameState->getPlayerState(j).characterID)
                {
                    found = true;
                    break;
                }
            }

            if(found)
            {
                if(sw.animator_p)
                    sw.animator_p->setFrameSequance(sw.sequence_on);
            }
            else
            {
                if(sw.animator_p)
                    sw.animator_p->setFrameSequance(sw.sequence_off);
            }
        }

        for(size_t i = 0; i < m_blocks.size(); i++)
        {
            CharacterBlockState &blk = m_blocks[i];
            unsigned long needed = static_cast<unsigned long>(blk.CharacterID);
            bool found = false;

            for(int j = 1; j <= m_scene->m_numberOfPlayers; j++)
            {
                if(needed == m_scene->m_gameState->getPlayerState(j).characterID)
                {
                    found = true;
                    break;
                }
            }

            if(found)
            {
                if(blk.animator_p)
                    blk.animator_p->setFrameSequance(blk.sequence_on);
            }
            else
            {
                if(blk.animator_p)
                    blk.animator_p->setFrameSequance(blk.sequence_off);
            }
        }
    }
}
