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


void LVL_Player::refreshEnvironmentState()
{

    if((last_environment!=environment)||(last_environment==-1))
    {
        physics_cur = physics[environment];
        last_environment=environment;

        if(physics_cur.zero_speed_y_on_enter)
        {
            if(speedY()>0)
                setSpeedY(0.0);
            else
                setSpeedY(speedY()*(physics_cur.slow_up_speed_y_coeff));
        }

        if(physics_cur.slow_speed_x_on_enter)
            setSpeedX( speedX()*(physics_cur.slow_speed_x_coeff) );

        if(JumpPressed) jumpVelocity=physics_cur.velocity_jump;

        phys_setup.max_vel_x = fabs(_isRunning ?
                    physics_cur.MaxSpeed_run :
                    physics_cur.MaxSpeed_walk) *(_onGround?physics_cur.ground_c_max:1.0f);
        phys_setup.min_vel_x = -fabs(_isRunning ?
                    physics_cur.MaxSpeed_run :
                    physics_cur.MaxSpeed_walk) *(_onGround?physics_cur.ground_c_max:1.0f);
        phys_setup.max_vel_y = fabs(physics_cur.MaxSpeed_down);
        phys_setup.min_vel_y = -fabs(physics_cur.MaxSpeed_up);
        phys_setup.decelerate_x = physics_cur.decelerate_air;
        phys_setup.gravityScale = physics_cur.gravity_scale;
        phys_setup.grd_dec_x    = physics_cur.walk_force;

        floating_isworks=false;//< Reset floating on re-entering into another physical envirinment
    }
}


void LVL_Player::setDuck(bool duck)
{
    if(!duck_allow) return;
    if(duck==ducking) return;
    float b=posRect.bottom();
    setSize(state_cur.width, duck? state_cur.duck_height : state_cur.height);
    setPos(posX(), b-_height);
    ducking=duck;
    if(!duck && !isWarping)
    {
        _heightDelta = state_cur.duck_height-state_cur.height;
        if(_heightDelta>0.0f) _heightDelta=0.0f;
        _collideUnduck();
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
    return _onGround;
}

int LVL_Player::direction()
{
    return _direction;
}


