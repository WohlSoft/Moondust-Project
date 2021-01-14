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
        m_npcActive.erase(corpse);
        m_layers.removeRegItem(corpse->data.layer, corpse);
        corpse->unregisterFromTree();
        m_itemsNpc.erase(corpse);
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

void LevelScene::collectGarbageBlocks()
{
    std::vector<LVL_Block *> stillVizible;//Avoid camera crash (which uses a cached render list)
    while(!m_blocksToDelete.empty())
    {
        LVL_Block *corpse = m_blocksToDelete.back();
        m_blocksToDelete.pop_back();
        if(corpse->isInRenderList())
        {
            stillVizible.push_back(corpse);//Avoid camera crash (which uses a cached render list)
            continue;
        }
        m_layers.removeRegItem(corpse->data.layer, corpse);
        corpse->unregisterFromTree();
        m_itemsBlocks.erase(corpse);
        delete corpse;
    }
    m_blocksToDelete.insert(m_blocksToDelete.end(), stillVizible.begin(), stillVizible.end());
}
