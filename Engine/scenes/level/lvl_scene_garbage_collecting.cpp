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

#include "../scene_level.h"

#include <audio/pge_audio.h>
#include <audio/play_music.h>
#include <algorithm>

void LevelScene::collectGarbageNPCs()
{
    std::vector<LVL_Npc *> stillVizible;//Avoid camera crash (which uses a cached render list)
    while(!m_npcDead.empty())
    {
        LVL_Npc *corpse = m_npcDead.back();
        m_npcDead.pop_back();
        if(corpse->isInRenderList())
        {
            stillVizible.push_back(corpse);//Avoid camera crash (which uses a cached render list)
            continue;
        }

        m_npcActive.erase(std::remove(m_npcActive.begin(), m_npcActive.end(), corpse), m_npcActive.end());
        m_itemsNpc.erase(std::remove(m_itemsNpc.begin(), m_itemsNpc.end(), corpse), m_itemsNpc.end());
        m_layers.removeRegItem(corpse->data.layer, corpse);
        m_luaEngine.destoryLuaNpc(corpse);
    }
    m_npcDead.insert(m_npcDead.end(), stillVizible.begin(), stillVizible.end());
}

void LevelScene::collectGarbagePlayers()
{
    std::vector<LVL_Player *> stillVizible;//Avoid camera crash (which uses a cached render list)
    while(!m_playersDead.empty())
    {
        LVL_Player *corpse = m_playersDead.back();
        m_playersDead.pop_back();
        if(corpse->isInRenderList())
        {
            stillVizible.push_back(corpse);//Avoid camera crash (which uses a cached render list)
            continue;
        }
        LVL_Player::deathReason reason = corpse->m_killReason;
        m_itemsPlayers.erase(std::remove(m_itemsPlayers.begin(), m_itemsPlayers.end(), corpse), m_itemsPlayers.end());
        m_luaEngine.destoryLuaPlayer(corpse);

        switch(reason)
        {
        case LVL_Player::deathReason::DEAD_burn:
        case LVL_Player::deathReason::DEAD_fall:
        case LVL_Player::deathReason::DEAD_killed:
            if(m_itemsPlayers.size() > 0)
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerDied);
            else
            {
                Mix_HaltChannel(-1);
                PGE_Audio::playSoundByRole(obj_sound_role::LevelFailed);
            }
            break;
        }
        if(reason == LVL_Player::deathReason::DEAD_burn)
            PGE_Audio::playSoundByRole(obj_sound_role::NpcLavaBurn);
    }

    m_playersDead.insert(m_playersDead.end(), stillVizible.begin(), stillVizible.end());
    if(m_itemsPlayers.empty())
    {
        PGE_MusPlayer::stop();
        setExiting(4000, LvlExit::EXIT_PlayerDeath);
    }
}
