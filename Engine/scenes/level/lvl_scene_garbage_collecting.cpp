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
    while(!dead_npcs.empty())
    {
        LVL_Npc *corpse = dead_npcs.back();
        dead_npcs.pop_back();
        if(corpse->isInRenderList())
        {
            stillVizible.push_back(corpse);//Avoid camera crash (which uses a cached render list)
            continue;
        }

        active_npcs.erase(std::remove(active_npcs.begin(), active_npcs.end(), corpse), active_npcs.end());
        npcs.erase(std::remove(npcs.begin(), npcs.end(), corpse), npcs.end());
        layers.removeRegItem(corpse->data.layer, corpse);
        luaEngine.destoryLuaNpc(corpse);
    }
    dead_npcs.insert(dead_npcs.end(), stillVizible.begin(), stillVizible.end());
}

void LevelScene::collectGarbagePlayers()
{
    std::vector<LVL_Player *> stillVizible;//Avoid camera crash (which uses a cached render list)
    while(!dead_players.empty())
    {
        LVL_Player *corpse = dead_players.back();
        dead_players.pop_back();
        if(corpse->isInRenderList())
        {
            stillVizible.push_back(corpse);//Avoid camera crash (which uses a cached render list)
            continue;
        }
        LVL_Player::deathReason reason = corpse->kill_reason;
        players.erase(std::remove(players.begin(), players.end(), corpse), players.end());
        luaEngine.destoryLuaPlayer(corpse);

        switch(reason)
        {
        case LVL_Player::deathReason::DEAD_burn:
        case LVL_Player::deathReason::DEAD_fall:
        case LVL_Player::deathReason::DEAD_killed:
            if(players.size() > 0)
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

    dead_players.insert(dead_players.end(), stillVizible.begin(), stillVizible.end());
    if(players.empty())
    {
        PGE_MusPlayer::stop();
        setExiting(4000, LvlExit::EXIT_PlayerDeath);
    }
}
