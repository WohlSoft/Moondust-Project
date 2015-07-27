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

#include "../lvl_player.h"
#include "../lvl_scene_ptr.h"

#include <audio/pge_audio.h>
#include <audio/SdlMusPlayer.h>



void LVL_Player::attack(LVL_Player::AttackDirection _dir)
{
    PGE_RectF attackZone;

    switch(_dir)
    {
        case Attack_Up:
            attackZone.setRect(posCenterX()-5, top()-17, 10, 5);
        break;
        case Attack_Down:
            attackZone.setRect(posCenterX()-5, bottom(), 10, 5);
        break;
        case Attack_Forward:
            if(_direction>=0)
                attackZone.setRect(right(), bottom()-32, 10, 10);
            else
                attackZone.setRect(left()-10, bottom()-32, 10, 10);
        break;
    }


    QVector<PGE_Phys_Object*> bodies;
    LvlSceneP::s->queryItems(attackZone, &bodies);
    int contacts = 0;

    QList<LVL_Block *> target_blocks;
    QList<LVL_Npc*> target_npcs;
    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object*visibleBody=*it;
        contacts++;
        if(visibleBody==this) continue;
        if(visibleBody==NULL)
            continue;
        if(!visibleBody->isVisible()) //Don't whip elements from hidden layers!
            continue;
        switch(visibleBody->type)
        {
            case PGE_Phys_Object::LVLBlock:
                target_blocks.push_back(static_cast<LVL_Block*>(visibleBody));
                break;
            case PGE_Phys_Object::LVLNPC:
                target_npcs.push_back(static_cast<LVL_Npc*>(visibleBody));
                break;
            case PGE_Phys_Object::LVLPlayer:
                default:break;
        }
    }

    foreach(LVL_Block *x, target_blocks)
    {
        if(!x) continue;
        if(x->destroyed) continue;
        if(x->sizable && _dir==Attack_Forward)
            continue;
        x->hit();
        if(!x->destroyed)
        {
            LvlSceneP::s->launchStaticEffectC(69, x->posCenterX(), x->posCenterY(), 1, 0, 0, 0, 0);
            PGE_Audio::playSoundByRole(obj_sound_role::WeaponExplosion);
        }
        x->destroyed=true;
    }
    foreach(LVL_Npc *x, target_npcs)
    {
        if(!x) continue;
        if(x->isPaused()) continue; //Don't attak NPC with paused physics!
        if(x->isKilled()) continue;
        if(x->isGenerator) continue;
        x->harm();
        LvlSceneP::s->launchStaticEffectC(75, attackZone.center().x(), attackZone.center().y(), 1, 0, 0, 0, 0);
        kill_npc(x, NPC_Kicked);
    }
}


void LVL_Player::kill_npc(LVL_Npc *target, LVL_Player::kill_npc_reasons reason)
{
    if(!target) return;
    if(!target->isKilled()) return;

    switch(reason)
    {
        case NPC_Unknown:
            break;
        case NPC_Stomped:
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerStomp); break;
        case NPC_Kicked:
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerKick); break;
        case NPC_Taked_Coin:
            PGE_Audio::playSoundByRole(obj_sound_role::BonusCoin); break;
        case NPC_Taked_Powerup:
            break;
    }

    if(target->setup->exit_is)
    {
        long snd=target->setup->exit_snd;
        if(snd<=0)
        {
            switch(target->setup->exit_code)
            {
                case  1: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit01); break;
                case  2: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit02); break;
                case  3: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit03); break;
                case  4: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit04); break;
                case  5: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit05); break;
                case  6: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit06); break;
                case  7: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit07); break;
                case  8: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit08); break;
                case  9: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit09); break;
                case 10: snd=ConfigManager::getSoundByRole(obj_sound_role::LevelExit10); break;
                default: break;
            }
        }
        if(snd>0)
        {
            PGE_MusPlayer::MUS_stopMusic();
            PGE_Audio::playSound(snd);
        }
        /***********************Reset and unplug controllers************************/
        LvlSceneP::s->player1Controller->resetControls();
        LvlSceneP::s->player1Controller->sendControls();
        LvlSceneP::s->player1Controller->removeFromControl(this);
        LvlSceneP::s->player2Controller->resetControls();
        LvlSceneP::s->player2Controller->sendControls();
        LvlSceneP::s->player2Controller->removeFromControl(this);
        /***********************Reset and unplug controllers*end********************/
        if(target->setup->exit_walk_direction<0)
            keys.left=true;
        else
        if(target->setup->exit_walk_direction>0)
            keys.right=true;
        isExiting=true;
        LvlSceneP::s->setExiting(target->setup->exit_delay, target->setup->exit_code);
    }
}

void LVL_Player::harm(int _damage)
{
    doHarm=true;
    doHarm_damage=_damage;
}


void LVL_Player::kill(deathReason reason)
{
    doKill=true;
    kill_reason=reason;
}


