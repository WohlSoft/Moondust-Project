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
    if(blk.plSwitch_Button)
    {
        CharacterSwitchState swst;
        swst.CharacterID=blk.plSwitch_Button_id;
        swst.sequence_on=blk.plSwitch_frames_true;
        swst.sequence_off=blk.plSwitch_frames_false;
        if(blk.animator_ID<ConfigManager::Animator_Blocks.size())
        {
            swst.animator_p = &ConfigManager::Animator_Blocks[blk.animator_ID];
        }
        m_switches.push_back(swst);
    }
}

void CharacterSwitcherEngine::buildBrick(obj_block &blk)
{
    if(blk.plFilter_Block)
    {
        CharacterBlockState swst;
        swst.CharacterID=blk.plFilter_Block_id;
        swst.sequence_on=blk.plFilter_frames_true;
        swst.sequence_off=blk.plFilter_frames_false;
        if(blk.animator_ID<ConfigManager::Animator_Blocks.size())
        {
            swst.animator_p = &ConfigManager::Animator_Blocks[blk.animator_ID];
        }
        m_blocks.push_back(swst);
    }
}

void CharacterSwitcherEngine::refreshState()
{
    if(m_scene)
    {
        for(int i=0; i<m_switches.size();i++)
        {
            CharacterSwitchState &sw=m_switches[i];
            int needed=sw.CharacterID;
            bool found=false;
            for(int j=1; j<=m_scene->numberOfPlayers; j++)
            {
                if(needed==m_scene->gameState->getPlayerState(j).characterID) { found=true; break; }
            }
            if(found)
            {
                if(sw.animator_p)
                {
                    sw.animator_p->setFrameSequance(sw.sequence_on);
                }
            } else {
                if(sw.animator_p)
                {
                    sw.animator_p->setFrameSequance(sw.sequence_off);
                }
            }
        }

        for(int i=0; i<m_blocks.size();i++)
        {
            CharacterBlockState &blk=m_blocks[i];
            int needed=blk.CharacterID;
            bool found=false;
            for(int j=1; j<=m_scene->numberOfPlayers; j++)
            {
                if(needed==m_scene->gameState->getPlayerState(j).characterID) { found=true; break; }
            }
            if(found)
            {
                if(blk.animator_p)
                {
                    blk.animator_p->setFrameSequance(blk.sequence_on);
                }
            } else {
                if(blk.animator_p)
                {
                    blk.animator_p->setFrameSequance(blk.sequence_off);
                }
            }
        }
    }
}

