/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../lvl_player.h"
#include "../lvl_bgo.h"
#include "../lvl_block.h"
#include "../lvl_npc.h"
#include "../lvl_physenv.h"
#include "../lvl_warp.h"

#include "../../scene_level.h"
#include <audio/pge_audio.h>
#include <cassert>

static inline void processCharacterSwitchBlock(LVL_Player *player, LVL_Block *nearest)
{
    if(!nearest || !player)
        return;
    //Do transformation if needed
    if(nearest->setup->setup.plSwitch_Button && (player->characterID != nearest->setup->setup.plSwitch_Button_id))
    {
        unsigned long target_id = (nearest->setup->setup.plSwitch_Button_id - 1);
        std::vector<saveCharState> &states = player->m_scene->getGameState()->game_state.characterStates;
        if(target_id >= static_cast<unsigned long>(states.size()))
        {
            PlayerState x = player->m_scene->getGameState()->getPlayerState(player->playerID);
            x.characterID    = uint32_t(target_id + 1);
            x.stateID        = 1;
            x._chsetup.state = 1;
            player->m_scene->getGameState()->setPlayerState(player->playerID, x);
        }
        saveCharState &st = states[static_cast<size_t>(target_id)];
        player->setCharacterSafe(nearest->setup->setup.plSwitch_Button_id, st.state);
    }
}

void LVL_Player::refreshEnvironmentState()
{
    if((last_environment != environment) || (last_environment == -1))
    {
        physics_cur = physics[static_cast<unsigned long>(environment)];
        last_environment = environment;

        if(physics_cur.zero_speed_y_on_enter)
        {
            if(speedY() > 0.54)
                setSpeedY(0.54);
            else
                setSpeedY(speedY() * (physics_cur.slow_up_speed_y_coeff));
        }

        if(physics_cur.slow_speed_x_on_enter)
            setSpeedX(speedX() * (physics_cur.slow_speed_x_coeff));

        if(m_jumpPressed) m_jumpVelocity = physics_cur.velocity_jump;

        phys_setup.max_vel_x = fabs(m_isRunning ?
                                    physics_cur.MaxSpeed_run :
                                    physics_cur.MaxSpeed_walk) * (m_stand ? physics_cur.ground_c_max : 1.0f);
        phys_setup.min_vel_x = -fabs(m_isRunning ?
                                     physics_cur.MaxSpeed_run :
                                     physics_cur.MaxSpeed_walk) * (m_stand ? physics_cur.ground_c_max : 1.0f);
        phys_setup.max_vel_y = fabs(physics_cur.MaxSpeed_down);
        phys_setup.min_vel_y = -fabs(physics_cur.MaxSpeed_up);
        phys_setup.decelerate_x = physics_cur.decelerate_air;
        phys_setup.gravityScale = physics_cur.gravity_scale;
        phys_setup.grd_dec_x    = physics_cur.walk_force;
        m_floatingIsWorks = false; //< Reset floating on re-entering into another physical envirinment
    }
}

void LVL_Player::processContacts()
{
    //Hurt if contacted surface is dangerous, etc.
    /* *********************** Check all collided sides ***************************** */
    bool doHurt = false;
    int  hurtDamage = 0;
    bool doKill = false;
    deathReason killReason = DEAD_killed;
    std::vector<LVL_Block *> blocks_to_bounce_bottom;
    std::vector<LVL_Npc *> npcs_to_stomp;

    for(ObjectCollidersIt it = l_contactAny.begin(); it != l_contactAny.end(); it++)
    {
        PGE_Phys_Object *cEL = *it;

        switch(cEL->type)
        {
        case LVLWarp:
        {
            m_contactedWarp = static_cast<LVL_Warp *>(cEL);
            SDL_assert(m_contactedWarp);
            m_contactedWithWarp = true;
            break;
        }

        case PGE_Phys_Object::LVLBGO:
        {
            LVL_Bgo *bgo = static_cast<LVL_Bgo *>(cEL);
            SDL_assert(bgo);

            if(bgo->setup->setup.climbing)
            {
                bool hasClimbables = climbable_map.empty();
                climbable_map[intptr_t(cEL)] = cEL;

                if(hasClimbables)
                    m_climbableHeight = cEL->m_momentum.top();
                else if(cEL->top() < m_climbableHeight)
                    m_climbableHeight = cEL->m_momentum.top();
            }
            break;
        }

        case PGE_Phys_Object::LVLPhysEnv:
        {
            LVL_PhysEnv *env = static_cast<LVL_PhysEnv *>(cEL);
            SDL_assert(env);
            environments_map[intptr_t(env)] = env->env_type;
            break;
        }

        case PGE_Phys_Object::LVLBlock:
        {
            LVL_Block *blk = static_cast<LVL_Block *>(cEL);
            SDL_assert(blk);

            if(blk->m_isHidden)
                break;

            if(blk->setup->setup.lava)
            {
                doKill = true;
                killReason = DEAD_burn;
            }

            break;
        }

        case PGE_Phys_Object::LVLNPC:
        {
            LVL_Npc *npc = static_cast<LVL_Npc *>(cEL);
            SDL_assert(npc);

            if(npc->killed) break; //Don't deal with a corpses!

            if(!npc->data.msg.empty())
                collided_talkable_npc = npc;

            if(!npc->enablePlayerCollision) break;

            if(npc->data.friendly) break;

            if(npc->m_isGenerator) break;

            if(npc->setup->setup.climbable)
            {
                bool set = climbable_map.empty();
                climbable_map[intptr_t(cEL)] = cEL;

                if(set)
                    m_climbableHeight = cEL->m_momentum.top();
                else if(cEL->m_momentum.top() < m_climbableHeight)
                    m_climbableHeight = cEL->m_momentum.top();
            }

            if((!npc->data.friendly) && (npc->setup->setup.takable))
            {
                collided_talkable_npc = nullptr;
                npc->doHarm(LVL_Npc::DAMAGE_TAKEN, this);
                kill_npc(npc, LVL_Player::NPC_Taked_Coin);
                break;
            }

            if(m_bumpUp || m_bumpDown)
                break;

            if(!npc->isActivated)
                break;

            //If character fell to the head of the NPC
            if(((npc->m_blocked[m_filterID]&Block_TOP) == 0) &&
               (m_momentum.bottom() >= npc->m_momentum.top()) &&
               (m_momentum.bottomOld() <= npc->m_momentum.topOld()) &&
               npc->setup->setup.kill_on_jump)
            {
                npcs_to_stomp.push_back(npc);
                //npc->doHarm(LVL_Npc::DAMAGE_STOMPED);
                //bump(true);
                //kill_npc(npc, NPC_Stomped);
            }
            else
            {
                if(npc->setup->setup.hurt_player)
                {
                    if(((npc->m_blocked[m_filterID]&Block_TOP) == 0) ||
                       (((npc->m_blocked[m_filterID]&Block_TOP) != 0) &&
                        (m_momentum.top() > m_momentum.topOld()) && //Avoid post-resize shit
                        (m_momentum.bottom() > npc->m_momentum.top()) &&
                        (m_momentum.bottomOld() > npc->m_momentum.topOld())))
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

    for(ObjectCollidersIt it = l_contactB.begin(); it != l_contactB.end(); it++)
    {
        PGE_Phys_Object *cEL = *it;

        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBlock:
        {
            LVL_Block *blk = static_cast<LVL_Block *>(cEL);
            SDL_assert(blk);// continue;
            m_onSlippery |= blk->m_slippery_surface;

            if(blk->setup->setup.bounce)
                blocks_to_bounce_bottom.push_back(blk);

            if((blk->m_danger[m_filterID]&Block_TOP) != 0)
            {
                doHurt = true;
                hurtDamage = 1;
            }

            if(m_onSlopeFloorTopAlign && !m_slopeFloor.has && m_stand &&
               (blk->m_momentum.velY <= m_momentum.velY))
            {
                switch(blk->m_shape)
                {
                case PGE_physBody::SL_LeftBottom:
                {
                    if((m_momentum.left() <= blk->m_momentum.left()) &&
                       (m_momentum.left() + m_momentum.velX > m_momentum.left()))
                    {
                        double k = blk->m_momentum.h / blk->m_momentum.w;
                        double newx = m_momentum.x + m_momentum.velX;
                        double newy = blk->m_momentum.y + ((newx - blk->m_momentum.x) * k) - m_momentum.h;
                        m_momentum.velY = fabs(m_momentum.y - newy);
                        m_slopeFloor.has = true;
                        m_slopeFloor.shape = blk->m_shape;
                        m_slopeFloor.rect  = blk->m_momentum.rect();
                        m_onSlopeYAdd = 0.0;
                    }

                    break;
                }

                case PGE_physBody::SL_RightBottom:
                {
                    if((m_momentum.right() >= blk->m_momentum.right()) &&
                       (m_momentum.right() + m_momentum.velX < m_momentum.right()))
                    {
                        double k = blk->m_momentum.h / blk->m_momentum.w;
                        double newx = m_momentum.x + m_momentum.velX;
                        double newy = blk->m_momentum.y + ((blk->m_momentum.right() - newx - m_momentum.w) * k) - m_momentum.h;
                        m_momentum.velY = fabs(m_momentum.y - newy);
                        m_slopeFloor.has = true;
                        m_slopeFloor.shape = blk->m_shape;
                        m_slopeFloor.rect  = blk->m_momentum.rect();
                        m_onSlopeYAdd = 0.0;
                    }

                    break;
                }

                default:
                    ;
                }
            }
        }
        break;

        case PGE_Phys_Object::LVLNPC:
        {
            LVL_Npc *npc = static_cast<LVL_Npc *>(cEL);
            SDL_assert(npc);// continue;
            m_onSlippery |= npc->m_slippery_surface;
            //                if( ((npc->m_blocked[m_filterID]&Block_TOP) == 0) &&
            //                     (npc->setup->setup.kill_on_jump) )
            //                    npcs_to_stomp.push_back(npc);
        }
        break;

        default:
            ;
        }
    }

    for(ObjectCollidersIt it = l_contactT.begin(); it != l_contactT.end(); it++)
    {
        PGE_Phys_Object *cEL = *it;

        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBlock:
        {
            LVL_Block *blk = static_cast<LVL_Block *>(cEL);
            //if(!blk) continue;
            SDL_assert(blk);

            if((blk->m_danger[m_filterID]&Block_BOTTOM) != 0)
            {
                doHurt = true;
                hurtDamage = 1;
            }
        }
        break;

        default:
            ;
        }
    }

    for(ObjectCollidersIt it = l_contactL.begin(); it != l_contactL.end(); it++)
    {
        PGE_Phys_Object *cEL = *it;

        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBlock:
        {
            LVL_Block *blk = static_cast<LVL_Block *>(cEL);
            //if(!blk) continue;
            SDL_assert(blk);

            if((blk->m_danger[m_filterID]&Block_RIGHT) != 0)
            {
                doHurt = true;
                hurtDamage = 1;
            }
        }
        break;

        default:
            ;
        }
    }

    for(ObjectCollidersIt it = l_contactR.begin(); it != l_contactR.end(); it++)
    {
        PGE_Phys_Object *cEL = *it;

        switch(cEL->type)
        {
        case PGE_Phys_Object::LVLBlock:
        {
            LVL_Block *blk = static_cast<LVL_Block *>(cEL);
            //if(!blk) continue;
            SDL_assert(blk);

            if((blk->m_danger[m_filterID]&Block_LEFT) != 0)
            {
                doHurt = true;
                hurtDamage = 1;
            }
        }
        break;

        default:
            ;
        }
    }

    if(doKill)
        kill(killReason);
    else if(doHurt)
        harm(hurtDamage);

    if(!blocks_to_bounce_bottom.empty())
    {
        LVL_Block *candidate = nullptr;

        for(size_t bumpI = 0; bumpI < blocks_to_bounce_bottom.size(); bumpI++)
        {
            LVL_Block *x = blocks_to_bounce_bottom[bumpI];
            SDL_assert(x);

            if((x->m_blocked[m_filterID]&Block_TOP) == 0)
                continue;

            if(!candidate)
            {
                candidate = x;
                continue;
            }

            if(candidate == x)
                continue;

            if(x->m_momentum.betweenH(m_momentum.centerX()))
                candidate = x;
        }

        SDL_assert_release(candidate);//Must not be null!
        if(candidate)
        {
            processCharacterSwitchBlock(this, candidate);//Do transformation if needed
            long npcid = candidate->data.npc_id;
            candidate->hit(LVL_Block::down);

            if(candidate->setup->setup.bumpable || (npcid != 0) ||
               (candidate->m_destroyed) || candidate->setup->setup.bounce)
            {
                if(m_momentum.bottom() >= candidate->m_momentum.top())
                    m_momentum.setYatBottom(candidate->m_momentum.top() - std::fabs(physics_cur.velocity_jump_bounce));

                bump(true, physics_cur.velocity_jump_bounce, physics_cur.jump_time_bounce);
            }
        }
    }

    //Stomp all detected NPC's
    while(!npcs_to_stomp.empty())
    {
        LVL_Npc *npc = npcs_to_stomp.back();
        npcs_to_stomp.pop_back();
        //Avoid workarround "don't hurt while flying up"
        if(m_momentum.bottom() >= npc->m_momentum.top() + m_contactPadding)
            m_momentum.setYatBottom(npc->m_momentum.top() + m_contactPadding - 0.2);
        setSpeedY(npc->speedY());
        npc->doHarm(LVL_Npc::DAMAGE_STOMPED, this);
        bump(true);
        //Reset floating state
        m_floatingTimer = m_floatingMaxtime;

        if(m_floatingIsWorks)
        {
            m_floatingIsWorks = false;
            setGravityScale(m_climbing ? 0 : physics_cur.gravity_scale);
        }
        kill_npc(npc, NPC_Stomped);
    }
}

void LVL_Player::preCollision()
{
    m_bumpDown = false;
    m_bumpUp = false;
    m_contactedWarp = nullptr;
    m_contactedWithWarp = false;
    collided_talkable_npc = nullptr;
    climbable_map.clear();
    environments_map.clear();
}

void LVL_Player::postCollision()
{
    if(m_crushedHard)
    {
        //harm(1);
        kill(DEAD_killed);
    }

    if(m_crushed && m_crushedOld)
    {
        if(m_stand)
        {
            m_momentum.velXsrc = 0.0;
            m_momentum.velX = -m_direction * 8;
            applyAccel(0.0, 0.0);
        }
    }

    updateCamera();
}

void LVL_Player::collisionHitBlockTop(std::vector<PGE_Phys_Object *> &blocksHit)
{
    PGE_Phys_Object *candidate = nullptr;

    for(unsigned int bumpI = 0; bumpI < blocksHit.size(); bumpI++)
    {
        PGE_Phys_Object *x = blocksHit[bumpI];

        if((x->m_blocked[m_filterID]&Block_BOTTOM) == 0)
            continue;

        if(!candidate)
        {
            candidate = x;
            continue;
        }

        if(candidate == x)
            continue;

        if(x->m_momentum.betweenH(m_momentum.centerX()))
            candidate = x;
    }

    if(candidate)
    {
        if(candidate->type == LVLBlock)
        {
            LVL_Block *nearest = static_cast<LVL_Block *>(candidate);
            SDL_assert_release(nearest);
            processCharacterSwitchBlock(this, nearest);//Do transformation if needed
            long npcid = nearest->data.npc_id;
            nearest->hit(LVL_Block::up, this, 1);

            if(nearest->setup->setup.bumpable ||
               (npcid != 0) ||
               (nearest->m_destroyed) ||
               (nearest->setup->setup.bounce))
            {
                bump(false,
                     fabs(m_momentum.oldx - m_momentum.x),
                     physics_cur.jump_time_bounce);
            }
        }
        else
            PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);

        m_jumpTime = 0;
    }
}

bool LVL_Player::preCollisionCheck(PGE_Phys_Object *body)
{
    SDL_assert(body);
    if(body->type == LVLBlock)
    {
        LVL_Block *blk = static_cast<LVL_Block *>(body);
        SDL_assert_release(blk);//if(!blk) return false;
        if(blk->m_destroyed)
            return true;
        if(blk->setup->setup.plFilter_Block &&
           (characterID == blk->setup->setup.plFilter_Block_id))
            return true;
    }
    return false;
}

void LVL_Player::setDuck(bool duck)
{
    if(!m_duckAllow)
        return;
    if(duck == m_ducking)
        return;
    double b = m_momentum.bottom();
    setSize(state_cur.width, duck ? state_cur.duck_height : state_cur.height);
    setPos(posX(), b - m_momentum.h);
    m_ducking = duck;

    if(!duck && !m_isWarping)
    {
        _heightDelta = state_cur.duck_height - state_cur.height;
        if(_heightDelta > 0.0)
            _heightDelta = 0.0;
        //_collideUnduck();
    }
}

void LVL_Player::bump(bool _up, double bounceSpeed, int timeToJump)
{
    if(_up)
    {
        m_bumpUp = true;
        m_bumpJumpTime = (timeToJump > 0) ? timeToJump : physics_cur.jump_time_bounce ;
        m_bumpJumpVelocity = (bounceSpeed > 0) ? bounceSpeed : physics_cur.velocity_jump_bounce;

        if(keys.jump)
        {
            m_jumpTime = m_bumpJumpTime;
            m_jumpVelocity = m_bumpJumpVelocity;
        }

        setSpeedY((keys.jump ?
                   (-fabs(m_bumpJumpVelocity) - fabs(speedX() / physics_cur.velocity_jump_c)) :
                   -fabs(m_bumpJumpVelocity)));
    }
    else
    {
        m_bumpDown = true;
        m_bumpVelocity = fabs(bounceSpeed);//4.0;
        setSpeedY(m_bumpVelocity);
        m_jumpTime = 0;
    }
}

bool LVL_Player::isRunning()
{
    return m_isRunning;
}

bool LVL_Player::onGround()
{
    return m_stand;
}

int LVL_Player::direction()
{
    return m_direction;
}
