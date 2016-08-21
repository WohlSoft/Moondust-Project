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

#include "../lvl_player.h"
#include "../lvl_bgo.h"
#include "../lvl_block.h"
#include "../lvl_npc.h"
#include "../lvl_physenv.h"
#include "../lvl_warp.h"

#include "../../scene_level.h"
#include <audio/pge_audio.h>

static inline void processCharacterSwitchBlock(LVL_Player*player, LVL_Block*nearest)
{
    //Do transformation if needed
    if( nearest->setup->setup.plSwitch_Button && (player->characterID!=nearest->setup->setup.plSwitch_Button_id) )
    {
        int target_id = (nearest->setup->setup.plSwitch_Button_id-1);
        QVector<saveCharState>&states=player->_scene->getGameState()->game_state.characterStates;
        if(target_id >= states.size())
        {
            PlayerState x = player->_scene->getGameState()->getPlayerState(player->playerID);
            x.characterID    = target_id+1;
            x.stateID        = 1;
            x._chsetup.state = 1;
            player->_scene->getGameState()->setPlayerState(player->playerID, x);
        }
        saveCharState&st = states[target_id];
        player->setCharacterSafe(nearest->setup->setup.plSwitch_Button_id, st.state);
    }
}

void LVL_Player::refreshEnvironmentState()
{

    if((last_environment!=environment)||(last_environment==-1))
    {
        physics_cur = physics[environment];
        last_environment=environment;

        if(physics_cur.zero_speed_y_on_enter)
        {
            if( speedY() > 0.54 )
                setSpeedY( 0.54 );
            else
                setSpeedY( speedY()*(physics_cur.slow_up_speed_y_coeff) );
        }

        if(physics_cur.slow_speed_x_on_enter)
            setSpeedX( speedX()*(physics_cur.slow_speed_x_coeff) );

        if(JumpPressed) jumpVelocity=physics_cur.velocity_jump;

        phys_setup.max_vel_x = fabs(_isRunning ?
                    physics_cur.MaxSpeed_run :
                    physics_cur.MaxSpeed_walk) *(m_stand ? physics_cur.ground_c_max : 1.0f);
        phys_setup.min_vel_x = -fabs(_isRunning ?
                    physics_cur.MaxSpeed_run :
                    physics_cur.MaxSpeed_walk) *(m_stand ? physics_cur.ground_c_max : 1.0f);
        phys_setup.max_vel_y = fabs(physics_cur.MaxSpeed_down);
        phys_setup.min_vel_y = -fabs(physics_cur.MaxSpeed_up);
        phys_setup.decelerate_x = physics_cur.decelerate_air;
        phys_setup.gravityScale = physics_cur.gravity_scale;
        phys_setup.grd_dec_x    = physics_cur.walk_force;

        floating_isworks=false;//< Reset floating on re-entering into another physical envirinment
    }
}

void LVL_Player::processContacts()
{
    //Hurt if contacted surface is dangerous, etc.

    /* *********************** Check all collided sides ***************************** */
    bool doHurt = false;
    int  hurtDamage=0;
    bool doKill = false;
    deathReason killReason = DEAD_killed;
    QVector<LVL_Block*> blocks_to_bounce_bottom;
    QVector<LVL_Npc*> npcs_to_stomp;

    for(ObjectCollidersIt it=l_contactAny.begin(); it!=l_contactAny.end(); it++)
    {
        PGE_Phys_Object* cEL = it.value();
        switch(cEL->type)
        {
        case LVLWarp:
            {
                contactedWarp = static_cast<LVL_Warp*>(cEL);
                if(contactedWarp)
                    contactedWithWarp=true;
                break;
            }
        case PGE_Phys_Object::LVLBGO:
            {
                LVL_Bgo *bgo= static_cast<LVL_Bgo*>(cEL);
                if(bgo)
                {
                    if(bgo->setup->setup.climbing)
                    {
                        bool hasClimbables = climbable_map.isEmpty();
                        climbable_map[intptr_t(cEL)] = cEL;
                        if(hasClimbables)
                            climbableHeight = cEL->m_momentum.top();
                        else if(cEL->top() < climbableHeight)
                            climbableHeight = cEL->m_momentum.top();
                    }
                }
                break;
            }
        case PGE_Phys_Object::LVLPhysEnv:
            {
                LVL_PhysEnv *env= static_cast<LVL_PhysEnv*>(cEL);
                if(env)
                {
                    environments_map[intptr_t(env)] = env->env_type;
                }
                break;
            }
        case PGE_Phys_Object::LVLNPC:
            {
                LVL_Npc *npc= static_cast<LVL_Npc*>(cEL);
                if(npc)
                {
                    if(!npc->enablePlayerCollision) break;
                    if(npc->data.friendly) break;
                    if(npc->isGenerator) break;
                    if(npc->setup->setup.climbable)
                    {
                        bool set=climbable_map.isEmpty();
                        climbable_map[intptr_t(cEL)]=cEL;
                        if(set)
                            climbableHeight=cEL->m_momentum.top();
                        else if(cEL->m_momentum.top()<climbableHeight)
                            climbableHeight=cEL->m_momentum.top();
                    }
                    if( (!npc->data.friendly) && (npc->setup->setup.takable) )
                    {
                        collided_talkable_npc = nullptr;
                        npc->doHarm(LVL_Npc::DAMAGE_TAKEN);
                        kill_npc(npc, LVL_Player::NPC_Taked_Coin);
                    }

                    if(bumpUp || bumpDown)
                        break;

                    if(!npc->isActivated)
                        break;

                    //If character fell to the head of the NPC
                    if( ((m_blocked[m_filterID]&Block_TOP)==0) &&
                         (m_momentum.bottom() > npc->m_momentum.top()) &&
                         (m_momentum.bottomOld()<= npc->m_momentum.topOld()) &&
                          npc->setup->setup.kill_on_jump )
                    {
                        npc->doHarm(LVL_Npc::DAMAGE_STOMPED);
                        bump(true);
                        floating_timer = floating_maxtime;
                        if(floating_isworks)
                        {
                            floating_isworks=false;
                            setGravityScale(climbing ? 0 : physics_cur.gravity_scale);
                        }
                        kill_npc(npc, NPC_Stomped);
                    } else {
                        if(npc->setup->setup.hurt_player)
                        {
                            doHurt = true;
                            hurtDamage = 1;
                        }
                    }
                }
                break;
            }
        }
    }

    for(ObjectCollidersIt it=l_contactB.begin(); it!=l_contactB.end(); it++)
    {
        PGE_Phys_Object* cEL = it.value();
        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBlock:
            {
                LVL_Block *blk= static_cast<LVL_Block*>(cEL);
                if(!blk) continue;
                if(blk->setup->setup.bounce)
                    blocks_to_bounce_bottom.push_back(blk);
                if((blk->m_danger[m_filterID]&Block_TOP) != 0)
                    doHurt = true;
            }
            break;
        case PGE_Phys_Object::LVLNPC:
            {
                LVL_Npc *npc= static_cast<LVL_Npc*>(cEL);
                if(!npc) continue;
                if( ((npc->m_blocked[m_filterID]&Block_TOP) != 0) &&
                     (npc->setup->setup.kill_on_jump) )
                    npcs_to_stomp.push_back(npc);
            }
            break;
        default:;
        }
    }

    for(ObjectCollidersIt it=l_contactT.begin(); it!=l_contactT.end(); it++)
    {
        PGE_Phys_Object* cEL = it.value();
        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBlock:
            {
                LVL_Block *blk= static_cast<LVL_Block*>(cEL);
                if(!blk) continue;
                if((blk->m_danger[m_filterID]&Block_BOTTOM) != 0)
                    doHurt = true;
            }
            break;
        default:;
        }
    }

    for(ObjectCollidersIt it=l_contactL.begin(); it!=l_contactL.end(); it++)
    {
        PGE_Phys_Object* cEL = it.value();
        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBlock:
            {
                LVL_Block *blk= static_cast<LVL_Block*>(cEL);
                if(!blk) continue;
                if((blk->m_danger[m_filterID]&Block_RIGHT) != 0)
                    doHurt = true;
            }
            break;
        default:;
        }
    }

    for(ObjectCollidersIt it=l_contactR.begin(); it!=l_contactR.end(); it++)
    {
        PGE_Phys_Object* cEL = it.value();
        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBlock:
            {
                LVL_Block *blk= static_cast<LVL_Block*>(cEL);
                if(!blk) continue;
                if((blk->m_danger[m_filterID]&Block_LEFT) != 0)
                    doHurt = true;
            }
            break;
        default:;
        }
    }

    if(doHurt)
        harm(hurtDamage);
    if(doKill)
        kill(killReason);

    if(!blocks_to_bounce_bottom.isEmpty())
    {
        LVL_Block* candidate = nullptr;
        for(/*unsigned*/ int bump=0; bump < blocks_to_bounce_bottom.size(); bump++)
        {
            LVL_Block* x = blocks_to_bounce_bottom[bump];
            if(!candidate)
            {
                candidate = x;
                continue;
            }
            if(candidate == x)
                continue;
            if(!candidate)
                candidate = x;
            if( x->m_momentum.betweenH(m_momentum.centerX()) )
            {
                candidate = x;
            }
        }

        processCharacterSwitchBlock(this, candidate);//Do transformation if needed
        long npcid = candidate->data.npc_id;
        candidate->hit(LVL_Block::down);
        if(  candidate->setup->setup.hitable || (npcid !=0) ||
            (candidate->destroyed) || candidate->setup->setup.bounce )
            bump(true, physics_cur.velocity_jump_bounce, physics_cur.jump_time_bounce);

    }

    //Stomp all detected NPC's
    while(!npcs_to_stomp.isEmpty())
    {
        LVL_Npc* npc = npcs_to_stomp.last();
        npcs_to_stomp.pop_back();
        /*
        //Avoid workarround "don't hurt while flying up"
        if(bottom() >= npc->top())
            setPosY( npc->top() - height()-1 );
        */
        npc->doHarm(LVL_Npc::DAMAGE_STOMPED);
        bump(true);

        //Reset floating state
        floating_timer = floating_maxtime;
        if(floating_isworks)
        {
            floating_isworks=false;
            setGravityScale(climbing ? 0 : physics_cur.gravity_scale);
        }
        kill_npc(npc, NPC_Stomped);
    }
}

void LVL_Player::preCollision()
{
    contactedWarp = nullptr;
    contactedWithWarp = false;

    collided_talkable_npc = nullptr;

    climbable_map.clear();
    environments_map.clear();

    collided_top.clear();
    collided_left.clear();
    collided_right.clear();
    collided_bottom.clear();
    collided_center.clear();
}

void LVL_Player::postCollision()
{

}

void LVL_Player::collisionHitBlockTop(std::vector<PGE_Phys_Object *> &blocksHit)
{
    PGE_Phys_Object*candidate = nullptr;
    for(unsigned int bump=0; bump<blocksHit.size(); bump++)
    {
        PGE_Phys_Object* x = blocksHit[bump];
        if(!candidate)
        {
            candidate = x;
            continue;
        }
        if(candidate == x)
            continue;
        if(!candidate)
            candidate = x;
        if( x->m_momentum.betweenH(m_momentum.centerX()) )
        {
            candidate = x;
        }
    }

    if(candidate)
    {
        if(candidate->type == LVLBlock)
        {
            LVL_Block* nearest = static_cast<LVL_Block*>(candidate);
            processCharacterSwitchBlock(this, nearest);//Do transformation if needed
            long npcid=nearest->data.npc_id;
            nearest->hit(LVL_Block::up, this, 1);
            if( nearest->setup->setup.hitable ||
                (npcid !=0 ) ||
                (nearest->destroyed) ||
                (nearest->setup->setup.bounce) )
            {
                bump(false,
                     speedY(),
                     physics_cur.jump_time_bounce);
            }
        } else {
            PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);
        }
    }
}

void LVL_Player::setDuck(bool duck)
{
    if(!duck_allow) return;
    if(duck==ducking) return;
    float b = m_momentum.bottom();
    setSize(state_cur.width, duck? state_cur.duck_height : state_cur.height);
    setPos(posX(), b-m_height_registered);
    ducking=duck;
    if(!duck && !isWarping)
    {
        _heightDelta = state_cur.duck_height-state_cur.height;
        if(_heightDelta>0.0f) _heightDelta=0.0f;
        //_collideUnduck();
    }
}

void LVL_Player::bump(bool _up, double bounceSpeed, int timeToJump)
{
    if(_up)
    {
        bumpUp=true;
        bumpJumpTime = (timeToJump>0) ? timeToJump: physics_cur.jump_time_bounce ;
        bumpJumpVelocity = (bounceSpeed>0) ? bounceSpeed : physics_cur.velocity_jump_bounce;
    }
    else
    {
        bumpDown=true;
        bumpVelocity = fabs(bounceSpeed)/4.0;
    }
}

bool LVL_Player::isRunning()
{
    return _isRunning;
}

bool LVL_Player::onGround()
{
    return m_stand;
}

int LVL_Player::direction()
{
    return _direction;
}


