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
#include "../lvl_physenv.h"

#include <audio/pge_audio.h>

namespace LVL_Player_refreshAnimation
{
    Uint32 slideTicks=0;
}
void LVL_Player::refreshAnimation()
{
    using namespace LVL_Player_refreshAnimation;
    /**********************************Animation switcher**********************************/
        if(climbing)
        {
            animator.unlock();
            if((speedY()<0.0)||(speedX()!=0.0))
                animator.switchAnimation(MatrixAnimator::Climbing, _direction, 128);
            else
                animator.switchAnimation(MatrixAnimator::Climbing, _direction, -1);
        }
        else
        if(ducking)
        {
            animator.switchAnimation(MatrixAnimator::SitDown, _direction, 128);
        }
        else
        if(!_onGround)
        {
            if(environment==LVL_PhysEnv::Env_Water)
            {
                if(speedY()>=0)
                    animator.switchAnimation(MatrixAnimator::Swim, _direction, 128);
                else
                    animator.switchAnimation(MatrixAnimator::SwimUp, _direction, 128);
            }
            else if(environment==LVL_PhysEnv::Env_Quicksand)
            {
                if(speedY()<0)
                    animator.switchAnimation(MatrixAnimator::JumpFloat, _direction, 64);
                else if(speedY()>0)
                    animator.switchAnimation(MatrixAnimator::Idle, _direction, 64);
            }
            else
            {
                if(speedY()<0)
                    animator.switchAnimation(MatrixAnimator::JumpFloat, _direction, 64);
                else if(speedY()>0)
                    animator.switchAnimation(MatrixAnimator::JumpFall, _direction, 64);
            }
        }
        else
        {
            bool busy=false;
            if((speedX()<-1)&&(_direction>0))
                if(keys.right)
                {
                    if(SDL_GetTicks()-slideTicks>100)
                    {
                        PGE_Audio::playSoundByRole(obj_sound_role::PlayerSlide);
                        slideTicks=SDL_GetTicks();
                    }
                    animator.switchAnimation(MatrixAnimator::Sliding, _direction, 64);
                    busy=true;
                }

            if(!busy)
            {
                if((speedX()>1)&&(_direction<0))
                    if(keys.left)
                    {
                        if(SDL_GetTicks()-slideTicks>100)
                        {
                            PGE_Audio::playSoundByRole(obj_sound_role::PlayerSlide);
                            slideTicks=SDL_GetTicks();
                        }
                        animator.switchAnimation(MatrixAnimator::Sliding, _direction, 64);
                        busy=true;
                    }
            }

            if(!busy)
            {
                float velX = speedX();
                if( ((!on_slippery_surface)&&(velX>0.0))||((on_slippery_surface)&&(_accelX>0.0)) )
                    animator.switchAnimation(MatrixAnimator::Run, _direction,
                                               (100-((velX*12)<85 ? velX*12 : 85)) );
                else if( ((!on_slippery_surface)&& (velX<0.0))||((on_slippery_surface)&&(_accelX<0.0)) )
                    animator.switchAnimation(MatrixAnimator::Run, _direction,
                                             (100-((-velX*12)<85 ? -velX*12 : 85)) );
                else
                    animator.switchAnimation(MatrixAnimator::Idle, _direction, 64);
            }
        }
    /**********************************Animation switcher**********************************/
}

