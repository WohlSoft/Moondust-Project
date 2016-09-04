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

#include "../../scene_level.h"
#include "../lvl_player.h"
#include "../lvl_section.h"

#include <audio/pge_audio.h>
#include <settings/debugger.h>

void LVL_Player::lua_updateKey(bool& target_key, ControllableObject::KeyType ktype, bool &state)
{
    if(target_key!=state)
    {
        if(state)
            lua_onKeyPressed(ktype);
        else
            lua_onKeyReleased(ktype);
        target_key=state;
    }
}

void LVL_Player::lua_processKeyEvents()
{
    try
    {
        lua_updateKey(keys_prev.left, KEY_LEFT, keys.left);
        lua_updateKey(keys_prev.right, KEY_RIGHT, keys.right);
        lua_updateKey(keys_prev.up, KEY_UP, keys.up);
        lua_updateKey(keys_prev.down, KEY_DOWN, keys.down);
        lua_updateKey(keys_prev.run, KEY_RUN, keys.run);
        lua_updateKey(keys_prev.jump, KEY_JUMP, keys.jump);
        lua_updateKey(keys_prev.alt_run, KEY_ALT_RUN, keys.alt_run);
        lua_updateKey(keys_prev.alt_jump, KEY_ALT_JUMP, keys.alt_jump);
        lua_updateKey(keys_prev.drop, KEY_DROP, keys.drop);
        lua_updateKey(keys_prev.start, KEY_DROP, keys.start);
    } catch (luabind::error& e) {
        _scene->getLuaEngine()->postLateShutdownError(e);
    }
}

void LVL_Player::update(double tickTime)
{
    if(isLocked) return;
    if(!_isInited) return;
    if(!camera) return;
    LVL_Section* section = sct();
    if(!section) return;

    event_queue.processEvents(tickTime);

    if((isWarping) || (!isAlive))
    {
        animator.tickAnimation(tickTime);
        updateCamera();
        return;
    }

    if(invincible)
    {
        invincible_delay-=tickTime;
        if(invincible_delay<0.0f)
        {
            invincible=false;
            blink_screen=false;
        }
    }

    //_onGround = !foot_contacts_map.isEmpty();
    //on_slippery_surface = !foot_sl_contacts_map.isEmpty();
    bool climbableUp  = !climbable_map.isEmpty();
    bool climbableDown= climbableUp && !m_stand;
    climbing = (climbableUp && climbing && !m_stand && (m_momentum.centerY() >= (climbableHeight - physics_cur.velocity_climb_y_up)) );
    if(m_stand)
    {
        phys_setup.decelerate_x =
                (fabs(speedX())<=physics_cur.MaxSpeed_walk)?
                (m_onSlippery ? physics_cur.decelerate_stop/physics_cur.slippery_c : physics_cur.decelerate_stop):
                (m_onSlippery ? physics_cur.decelerate_run/physics_cur.slippery_c : physics_cur.decelerate_run);

        if(physics_cur.strict_max_speed_on_ground)
        {
            if((speedX()>0)&&(speedX()>phys_setup.max_vel_x))
                setSpeedX(phys_setup.max_vel_x);
            else
            if((speedX()<0)&&(speedX()<phys_setup.min_vel_x))
                setSpeedX(phys_setup.min_vel_x);
        }
    }
    else
        phys_setup.decelerate_x = physics_cur.decelerate_air;

    if(climbing)
    {
        PGE_Phys_Object* climbableItem = *(climbable_map.begin());
        if(climbableItem)
        {
            m_momentum.velX = climbableItem->speedX();
            m_momentum.velY = climbableItem->speedY();
            //LEGACY_m_velocityX_add=climbableItem->speedX();
            //LEGACY_m_velocityY_add=climbableItem->speedY();
        } /*else
        {
            //LEGACY_m_velocityX_add=0.0f;
            //LEGACY_m_velocityY_add=0.0f;
        }*/
        if(gscale_Backup != 1.0)
        {
            setGravityScale(0);
            gscale_Backup = 1.0;
        }
    }
    else
    {
        if(gscale_Backup != 0.f)
        {
            setGravityScale(physics_cur.gravity_scale);
            gscale_Backup = 0.f;
        }
    }

    if(climbing)
    {
        setSpeed(0,0);
    }

    if(environments_map.isEmpty())
    {
        if(last_environment!=section->getPhysicalEnvironment() )
        {
            environment = section->getPhysicalEnvironment();
        }
    }
    else
    {
        int newEnv = section->getPhysicalEnvironment();

        foreach(int x, environments_map)
        {
            newEnv = x;
        }

        if( (newEnv!=LVL_PhysEnv::Env_SameAsAround) && (last_environment != newEnv) )
        {
            qDebug()<<"Enter to environment" << newEnv;
            environment = newEnv;
        }
    }

    refreshEnvironmentState();

    if(m_stand)
    {
        if(!floating_isworks)
        {
            floating_timer=floating_maxtime;
        }
    }

    //Processing lua key events
    lua_processKeyEvents();

    //Running key
    if(keys.run || keys.alt_run)
    {
        if(!_isRunning)
        {
            phys_setup.max_vel_x = physics_cur.MaxSpeed_run;
            phys_setup.min_vel_x = -physics_cur.MaxSpeed_run;
            _isRunning=true;
        }
    }
    else
    {
        if(_isRunning)
        {
            phys_setup.max_vel_x = physics_cur.MaxSpeed_walk;
            phys_setup.min_vel_x = -physics_cur.MaxSpeed_walk;
            _isRunning=false;
        }
    }
    if((physics_cur.ground_c_max!=1.0f))
    {
        phys_setup.max_vel_x = fabs(_isRunning ?
                    physics_cur.MaxSpeed_run :
                    physics_cur.MaxSpeed_walk) * (m_stand ? physics_cur.ground_c_max : 1.0f);
        phys_setup.min_vel_x = -fabs(_isRunning ?
                    physics_cur.MaxSpeed_run :
                    physics_cur.MaxSpeed_walk) * (m_stand ? physics_cur.ground_c_max : 1.0f);
    }


    if(keys.alt_run)
    {
        if(PGE_Debugger::cheat_chucknorris)
        {
            if(attack_enabled && !attack_pressed && !climbing)
            {
                attack_pressed=true;

                if(keys.up)
                    attack(Attack_Up);
                else
                if(keys.down)
                    attack(Attack_Down);
                else
                {
                    attack(Attack_Forward);
                    PGE_Audio::playSoundByRole(obj_sound_role::PlayerTail);
                    animator.playOnce(MatrixAnimator::RacoonTail, _direction, 75, true, true, 1);
                }
            }
        }
    }
    else
    {
        if(attack_pressed) attack_pressed=false;
    }

//  if(!keys.up && !keys.down && !keys.left && !keys.right)
//  {
//      if(wasEntered)
//      {
//          wasEntered = false;
//          wasEnteredTimeout=0;
//      }
//  }

    //Reset state
    if(wasEntered)
    {
        wasEnteredTimeout-=tickTime;
        if(wasEnteredTimeout<0)
        {
            wasEnteredTimeout=0;
            wasEntered=false;
        }
    }

    if(keys.up)
    {
        if(climbableUp&&(jumpTime<=0))
        {
            setDuck(false);
            climbing=true;
            floating_isworks=false;//!< Reset floating on climbing start
        }

        if(climbing)
        {
            if(m_momentum.centerY() >= climbableHeight)
                setSpeedY(-physics_cur.velocity_climb_y_up);
        } else {
            if(collided_talkable_npc) collided_talkable_npc->talkWith();
        }
    }

    if(keys.down)
    {
        if( climbableDown && (jumpTime<=0) )
        {
            setDuck(false);
            climbing=true;
            floating_isworks=false;//!< Reset floating on climbing start
        }
        else
        {
            if((duck_allow & !ducking)&&( (animator.curAnimation()!=MatrixAnimator::RacoonTail) ) )
            {
                setDuck(true);
            }
        }

        if(climbing)
        {
            setSpeedY(physics_cur.velocity_climb_y_down);
        }
    }
    else
    {
        if(ducking)
            setDuck(false);
    }

    if( (!keys.left) || (!keys.right) )
    {
        bool turning=(((speedX()>0)&&(_direction<0))||((speedX()<0)&&(_direction>0)));

        float force = turning?
                    physics_cur.decelerate_turn :
                    (fabs(speedX())>physics_cur.MaxSpeed_walk)?physics_cur.run_force : physics_cur.walk_force;

        if(m_onSlippery)
            force = force/physics_cur.slippery_c;
        else if( (m_stand)&&(physics_cur.ground_c!=1.0f) )
            force=force*physics_cur.ground_c;

        if(keys.left) _direction=-1;
        if(keys.right) _direction=1;

        if(!ducking || !m_stand)
        {
            //If left key is pressed
            if(keys.right && LEGACY_collided_right.isEmpty())
            {
                if(climbing)
                    setSpeedX(physics_cur.velocity_climb_x);
                else
                    applyAccel(force, 0.0);
            }
            //If right key is pressed
            if(keys.left && LEGACY_collided_left.isEmpty())
            {
                if(climbing)
                    setSpeedX(-physics_cur.velocity_climb_x);
                else
                    applyAccel(-force, 0.0);
            }

            if( (keys.left || keys.right) && turning && m_stand )
            {
                SpawnEffectDef effect;
                effect.id = 74;
                effect.startX = (float)posCenterX();
                effect.startY = (float)bottom()+2.0f;
                effect.gravity = 0.f;
                effect.start_delay = 0.0f;
                effect.velocityY = 0.f;
                effect.delay = 0;
                effect.animationLoops = 1;
                effect.framespeed = 64;
                _scene->launchEffect(effect, true);
            }
        }
    }

    m_moveLeft =  keys.left;
    m_moveRight = keys.right;

    if( keys.alt_jump && PGE_Debugger::cheat_superman )
    {
        //Temporary it is ability to fly up!
        if(!bumpDown && !bumpUp) {
            setSpeedY(-physics_cur.velocity_jump);
        }
    }

    if( keys.jump || keys.alt_jump )
    {
        if(!JumpPressed)
        {
            if(environment!=LVL_PhysEnv::Env_Water)
                { if(climbing || m_stand || (environment==LVL_PhysEnv::Env_Quicksand))
                    PGE_Audio::playSoundByRole(obj_sound_role::PlayerJump); }
            else
                PGE_Audio::playSoundByRole(obj_sound_role::PlayerWaterSwim);
        }

        if((environment==LVL_PhysEnv::Env_Water)||(environment==LVL_PhysEnv::Env_Quicksand))
        {
            if(!JumpPressed)
            {
                if(environment==LVL_PhysEnv::Env_Water)
                {
                    if(!ducking) animator.playOnce(MatrixAnimator::SwimUp, _direction, 75);
                }
                else
                if(environment==LVL_PhysEnv::Env_Quicksand)
                {
                    if(!ducking) animator.playOnce(MatrixAnimator::JumpFloat, _direction, 64);
                }

                JumpPressed=true;
                jumpTime = physics_cur.jump_time;
                jumpVelocity=physics_cur.velocity_jump;
                floating_timer = floating_maxtime;
                setSpeedY(speedY()-jumpVelocity);
            }
        }
        else
        if(!JumpPressed)
        {
            JumpPressed=true;
            if(m_stand || climbing)
            {
                climbing=false;
                jumpTime=physics_cur.jump_time;
                jumpVelocity=physics_cur.velocity_jump;
                floating_timer = floating_maxtime;
                //LEGACY_m_velocityY_add = 0;//Remove Y speed-add when player jumping
                setSpeedY(-jumpVelocity-fabs(speedX()/physics_cur.velocity_jump_c));
            }
            else
            if((floating_allow)&&(floating_timer > 0.0f))
            {
                floating_isworks=true;

                //if true - do floating with sin, if false - do with cos.
                floating_start_type=(speedY() < 0.0);

                setSpeedY(0.0);
                setGravityScale(0.0);
            }
        }
        else
        {
            if(jumpTime>0)
            {
                jumpTime -= tickTime;
                setSpeedY(-jumpVelocity-fabs(speedX()/physics_cur.velocity_jump_c));
            }

            if(floating_isworks)
            {
                floating_timer -= tickTime;
                if(floating_start_type)
                    setSpeedY( state_cur.floating_amplitude*(-cos(floating_timer/80.0f)) );
                else
                    setSpeedY( state_cur.floating_amplitude*(cos(floating_timer/80.0f)) );
                if(floating_timer<=0)
                {
                    floating_timer=0;
                    floating_isworks=false;
                    setGravityScale( climbing ? 0.0 : physics_cur.gravity_scale );
                }
            }
        }
    }
    else
    {
        jumpTime=0;
        if(JumpPressed)
        {
            JumpPressed=false;
            if(floating_allow)
            {
                if(floating_isworks)
                {
                    floating_timer=0;
                    floating_isworks=false;
                    setGravityScale(climbing?0:physics_cur.gravity_scale);
                }
            }
        }
    }

    refreshAnimation();
    animator.tickAnimation(tickTime);

    PGE_RectF sBox = section->sectionLimitBox();
    float asVelX=0.0f;

    if((section->isAutoscroll)&&(camera->isAutoscroll))
    {
        sBox = camera->limitBox;
        asVelX = camera->_autoscrollVelocityX;
    }

    //Return player to start position on fall down
    if(section->isWrapV())
    {
        if(posY()<sBox.top()-m_height_registered-1 )
            setPosY(sBox.bottom()-1);
        else
        if(posY()>sBox.bottom() + 1 )
            setPosY(sBox.top()-m_height_registered+1);
    } else if( posY() > sBox.bottom()+m_height_registered ) {
        kill(DEAD_fall);
    }

    if(bumpDown)
    {
        bumpDown=false;
        jumpTime=0;
        setSpeedY(bumpVelocity);
    }
    else
    if(bumpUp)
    {
        bumpUp=false;
        if(keys.jump)
        {
            jumpTime=bumpJumpTime;
            jumpVelocity=bumpJumpVelocity;
        }
        setSpeedY( (keys.jump ?
                        (-fabs(bumpJumpVelocity)-fabs(speedX()/physics_cur.velocity_jump_c)):
                         -fabs(bumpJumpVelocity)) );
    }


    //Connection of section opposite sides
    if(isExiting) // Allow walk offscreen if exiting
    {
        if((posX() < sBox.left()-m_width_registered-1 )||(posX() > sBox.right() + 1 ))
        {
            setGravityScale(0.0);//Prevent falling [we anyway exited from this level, isn't it?]
            setSpeedY(0.0);
        }
        if(keys.left||keys.right)
        {
            if((environment==LVL_PhysEnv::Env_Water)||(environment==LVL_PhysEnv::Env_Quicksand))
            {
                keys.run=true;
                if(_exiting_swimTimer<0 && !keys.jump)
                    keys.jump=true;
                else
                if(_exiting_swimTimer<0 && keys.jump)
                {
                    keys.jump=false; _exiting_swimTimer=(environment==LVL_PhysEnv::Env_Quicksand)? 1 : 500;
                }
                _exiting_swimTimer-= tickTime;
            } else keys.run=false;
        }
    }
    else
    if(section->isWrapH())
    {
        if(posX() < sBox.left()-m_width_registered-1 )
            setPosX( sBox.right()+1 );
        else
        if(posX() > sBox.right() + 1 )
            setPosX( sBox.left()-m_width_registered-1 );
    }
    else
    {

        if(section->ExitOffscreen())
        {
            if(section->RightOnly())
            {
                if( posX() < sBox.left())
                {
                    setPosX( sBox.left() );
                    if(asVelX==0.0) setSpeedX(0.0);
                    if((asVelX<0)&&(speedX()>0)) setSpeedX(0.0);
                    if((asVelX>0)&&(speedX()<0)) setSpeedX(0.0);
                }
            }

            if((posX() < sBox.left()-m_width_registered-1 ) || (posX() > sBox.right() + 1 ))
            {
                setLocked(true);
                _no_render=true;
                _scene->setExiting(1000, LvlExit::EXIT_OffScreen);
                return;
            }
        }
        else
        {
            //Prevent moving of player away from screen
            if( posX() < sBox.left())
            {
                setPosX(sBox.left());
                if(asVelX==0.0) setSpeedX(0.0);
                if((asVelX<0)&&(speedX()>0)) setSpeedX(0.0);
                if((asVelX>0)&&(speedX()<0)) setSpeedX(0.0);
            }
            else
            if( posX()+m_width_registered > sBox.right())
            {
                setPosX(sBox.right()-m_width_registered);
                if(asVelX==0.0) setSpeedX(0.0);
                if((asVelX<0)&&(speedX()>0)) setSpeedX(0.0);
                if((asVelX>0)&&(speedX()<0)) setSpeedX(0.0);
            }
        }
    }

    /*
    if(m_crushed && m_crushedOld)
    {
        m_momentum.x -= _direction*4;
        applyAccel(0.0, 0.0);
    }*/

    processWarpChecking();

    if(_doSafeSwitchCharacter) setCharacter(characterID, stateID);

    try {
        lua_onLoop(tickTime);
    } catch (luabind::error& e) {
        _scene->getLuaEngine()->postLateShutdownError(e);
    }
    _syncPosition();
}

void LVL_Player::updateCamera()
{
    camera->setCenterPos( round(posCenterX()), round(bottom())-state_cur.height/2 );
}

