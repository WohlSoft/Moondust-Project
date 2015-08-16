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
#include "../lvl_section.h"

#include <audio/pge_audio.h>


void LVL_Player::update(float ticks)
{
    if(isLocked) return;
    if(!_isInited) return;
    if(!camera) return;
    LVL_Section* section = sct();
    if(!section) return;

    event_queue.processEvents(ticks);

    if((isWarping) || (!isAlive))
    {
        animator.tickAnimation(ticks);
        updateCamera();
        return;
    }

    _onGround = !foot_contacts_map.isEmpty();
    on_slippery_surface = !foot_sl_contacts_map.isEmpty();
    bool climbableUp  = !climbable_map.isEmpty();
    bool climbableDown= climbableUp && !_onGround;
    climbing = (climbableUp && climbing && !_onGround && (posRect.center().y()>=(climbableHeight-physics_cur.velocity_climb_y_up)) );
    if(_onGround)
    {
        phys_setup.decelerate_x =
                (fabs(speedX())<=physics_cur.MaxSpeed_walk)?
                (on_slippery_surface?physics_cur.decelerate_stop/physics_cur.slippery_c : physics_cur.decelerate_stop):
                (on_slippery_surface?physics_cur.decelerate_run/physics_cur.slippery_c : physics_cur.decelerate_run);

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

    if(doKill)
    {
        doKill=false;
        isAlive = false;
        setPaused(true);
        LvlSceneP::s->checkPlayers();
        return;
    }

    if(climbing)
    {
        PGE_Phys_Object* climbableItem = *(climbable_map.begin());
        if(climbableItem)
        {
            _velocityX_add=climbableItem->speedX();
            _velocityY_add=climbableItem->speedY();
        } else
        {
            _velocityX_add=0.0f;
            _velocityY_add=0.0f;
        }

        if(gscale_Backup != 1)
        {
            setGravityScale(0);
            gscale_Backup = 1;
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

        if(last_environment != newEnv)
        {
            qDebug()<<"Enter to environment" << newEnv;
            environment = newEnv;
        }
    }

    refreshEnvironmentState();

    if(_onGround)
    {
        if(!floating_isworks)
        {
            floating_timer=floating_maxtime;
        }
    }

    //Running key
    if(keys.run)
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
                    physics_cur.MaxSpeed_walk) *(_onGround?physics_cur.ground_c_max:1.0f);
        phys_setup.min_vel_x = -fabs(_isRunning ?
                    physics_cur.MaxSpeed_run :
                    physics_cur.MaxSpeed_walk) *(_onGround?physics_cur.ground_c_max:1.0f);
    }


    if(keys.alt_run)
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
    else
    {
        if(attack_pressed) attack_pressed=false;
    }



    //if
    if(!keys.up && !keys.down && !keys.left && !keys.right)
    {
        if(wasEntered)
        {
            wasEntered = false;
            wasEnteredTimeout=0;
        }
    }

    //Reset state
    if(wasEntered)
    {
        wasEnteredTimeout-=ticks;
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
            if(posRect.center().y() >= climbableHeight)
                setSpeedY(-physics_cur.velocity_climb_y_up);
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

        if(on_slippery_surface) force=force/physics_cur.slippery_c;
        else if((_onGround)&&(physics_cur.ground_c!=1.0f)) force=force*physics_cur.ground_c;

        if(keys.left) _direction=-1;
        if(keys.right) _direction=1;

        if(!ducking || !_onGround)
        {
            //If left key is pressed
            if(keys.right && collided_right.isEmpty())
            {
                if(climbing)
                    setSpeedX(physics_cur.velocity_climb_x);
                else
                    applyAccel(force, 0);
            }
            //If right key is pressed
            if(keys.left && collided_left.isEmpty())
            {
                if(climbing)
                    setSpeedX(-physics_cur.velocity_climb_x);
                else
                    applyAccel(-force, 0);
            }
        }
    }

    if( keys.alt_jump )
    {
        //Temporary it is ability to fly up!
        if(!bumpDown && !bumpUp) {
            setSpeedY(-physics_cur.velocity_jump);
        }
    }

    if( keys.jump )
    {
        if(!JumpPressed)
        {
            if(environment!=LVL_PhysEnv::Env_Water)
                { if(climbing || _onGround || (environment==LVL_PhysEnv::Env_Quicksand))
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
            if(_onGround || climbing)
            {
                climbing=false;
                jumpTime=physics_cur.jump_time;
                jumpVelocity=physics_cur.velocity_jump;
                floating_timer = floating_maxtime;
                setSpeedY(-jumpVelocity-fabs(speedX()/physics_cur.velocity_jump_c));
            }
            else
            if((floating_allow)&&(floating_timer>0))
            {
                floating_isworks=true;

                //if true - do floating with sin, if false - do with cos.
                floating_start_type=(speedY()<0);

                setSpeedY(0);
                setGravityScale(0);
            }
        }
        else
        {
            if(jumpTime>0)
            {
                jumpTime -= ticks;
                setSpeedY(-jumpVelocity-fabs(speedX()/physics_cur.velocity_jump_c));
            }

            if(floating_isworks)
            {
                floating_timer -= ticks;
                if(floating_start_type)
                    setSpeedY( state_cur.floating_amplitude*(-cos(floating_timer/80.0)) );
                else
                    setSpeedY( state_cur.floating_amplitude*(cos(floating_timer/80.0)) );
                if(floating_timer<=0)
                {
                    floating_timer=0;
                    floating_isworks=false;
                    setGravityScale(climbing?0:physics_cur.gravity_scale);
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
    animator.tickAnimation(ticks);

    PGE_RectF sBox = section->sectionLimitBox();

    //Return player to start position on fall down
    if( posY() > sBox.bottom()+_height )
    {
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
        if((posX() < sBox.left()-_width-1 )||(posX() > sBox.right() + 1 ))
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
                _exiting_swimTimer-= ticks;
            } else keys.run=false;
        }
    }
    else
    if(section->isWarp())
    {
        if(posX() < sBox.left()-_width-1 )
            setPosX( sBox.right()+1 );
        else
        if(posX() > sBox.right() + 1 )
            setPosX( sBox.left()-_width-1 );
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
                    setSpeedX(0.0);
                }
            }

            if((posX() < sBox.left()-_width-1 ) || (posX() > sBox.right() + 1 ))
            {
                setLocked(true);
                _no_render=true;
                LvlSceneP::s->setExiting(1000, LvlExit::EXIT_OffScreen);
                return;
            }
        }
        else
        {
            //Prevent moving of player away from screen
            if( posX() < sBox.left())
            {
                setPosX(sBox.left());
                setSpeedX(0.0);
            }
            else
            if( posX()+_width > sBox.right())
            {
                setPosX(sBox.right()-_width);
                setSpeedX(0.0);
            }
        }
    }

    if(_stucked)
    {
        posRect.setX(posRect.x()-_direction*2);
        applyAccel(0, 0);
    }

    processWarpChecking();

    if(_doSafeSwitchCharacter) setCharacter(characterID, stateID);

    try {
        lua_onLoop();
    } catch (luabind::error& e) {
        LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
    }
    updateCamera();
}

void LVL_Player::updateCamera()
{
    camera->setCenterPos( round(posCenterX()), round(bottom())-state_cur.height/2 );
}

