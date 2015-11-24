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

#include "../scene_level.h"

#include <audio/pge_audio.h>
#include <audio/SdlMusPlayer.h>

void LevelScene::collectGarbageNPCs()
{
    while(!dead_npcs.isEmpty())
    {
        LVL_Npc *corpse = dead_npcs.last();
        dead_npcs.pop_back();
        #if (QT_VERSION >= 0x050400)
        active_npcs.removeAll(corpse);
        npcs.removeAll(corpse);
        layers.removeRegItem(corpse->data.layer, corpse);
        #else
        //He-he, it's a great workaround for a Qt less than 5.4 which has QVector without removeAll() function
        while(1)
        {
            const QVector<LVL_Npc *>::const_iterator ce = active_npcs.cend(), cit = std::find(active_npcs.cbegin(), ce, corpse);
            if (cit == ce)
                break;
            const QVector<LVL_Npc *>::iterator e = active_npcs.end(), it = std::remove(active_npcs.begin() + (cit - active_npcs.cbegin()), e, corpse);
            active_npcs.erase(it, e);
            break;
        }

        while(1)
        {
            const QVector<LVL_Npc *>::const_iterator ce = npcs.cend(), cit = std::find(npcs.cbegin(), ce, corpse);
            if (cit == ce)
                break;
            const QVector<LVL_Npc *>::iterator e = npcs.end(), it = std::remove(npcs.begin() + (cit - npcs.cbegin()), e, corpse);
            npcs.erase(it, e);
            break;
        }
        #endif
        luaEngine.destoryLuaNpc(corpse);
    }
}

void LevelScene::collectGarbagePlayers()
{
    while(!dead_players.isEmpty())
    {
        LVL_Player *corpse = dead_players.last();
        dead_players.pop_back();
        LVL_Player::deathReason reason = corpse->kill_reason;

        #if (QT_VERSION >= 0x050400)
        players.removeAll(corpse);
        #else
        //He-he, it's a great workaround for a Qt less than 5.4 which has QVector without removeAll() function
        while(1)
        {
            const QVector<LVL_Player *>::const_iterator ce = players.cend(), cit = std::find(players.cbegin(), ce, corpse);
            if (cit == ce)
                break;
            const QVector<LVL_Player *>::iterator e = players.end(), it = std::remove(players.begin() + (cit - players.cbegin()), e, corpse);
            players.erase(it, e);
            break;
        }
        #endif
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
        if(reason==LVL_Player::deathReason::DEAD_burn)
            PGE_Audio::playSoundByRole(obj_sound_role::NpcLavaBurn);
    }

    if(players.isEmpty())
    {
        PGE_MusPlayer::MUS_stopMusic();
        setExiting(4000, LvlExit::EXIT_PlayerDeath);
    }
}


