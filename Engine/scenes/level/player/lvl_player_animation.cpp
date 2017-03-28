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
        if(m_climbing)
        {
            m_animator.unlock();
            if((speedY()<0.0)||(speedX()!=0.0))
                m_animator.switchAnimation(MatrixAnimator::Climbing, m_direction, 128);
            else
                m_animator.switchAnimation(MatrixAnimator::Climbing, m_direction, -1);
        }
        else
        if(m_ducking)
        {
            m_animator.switchAnimation(MatrixAnimator::SitDown, m_direction, 128);
        }
        else
        if(!m_stand)
        {
            if(environment==LVL_PhysEnv::Env_Water)
            {
                if(speedY()>=0)
                    m_animator.switchAnimation(MatrixAnimator::Swim, m_direction, 128);
                else
                    m_animator.switchAnimation(MatrixAnimator::SwimUp, m_direction, 128);
            }
            else if(environment==LVL_PhysEnv::Env_Quicksand)
            {
                if(speedY()<0)
                    m_animator.switchAnimation(MatrixAnimator::JumpFloat, m_direction, 64);
                else if(speedY()>0)
                    m_animator.switchAnimation(MatrixAnimator::Idle, m_direction, 64);
            }
            else
            {
                if(speedY()<0)
                    m_animator.switchAnimation(MatrixAnimator::JumpFloat, m_direction, 64);
                else if(speedY()>0)
                    m_animator.switchAnimation(MatrixAnimator::JumpFall, m_direction, 64);
            }
        }
        else
        {
            bool busy=false;
            if((speedX()<-1)&&(m_direction>0))
                if(keys.right)
                {
                    if(SDL_GetTicks()-slideTicks>100)
                    {
                        PGE_Audio::playSoundByRole(obj_sound_role::PlayerSlide);
                        slideTicks=SDL_GetTicks();
                    }
                    m_animator.switchAnimation(MatrixAnimator::Sliding, m_direction, 64);
                    busy=true;
                }

            if(!busy)
            {
                if((speedX()>1)&&(m_direction<0))
                    if(keys.left)
                    {
                        if(SDL_GetTicks()-slideTicks>100)
                        {
                            PGE_Audio::playSoundByRole(obj_sound_role::PlayerSlide);
                            slideTicks=SDL_GetTicks();
                        }
                        m_animator.switchAnimation(MatrixAnimator::Sliding, m_direction, 64);
                        busy=true;
                    }
            }

            if(!busy)
            {
                float velX = speedX();
                if( ((!m_onSlippery)&&(velX>0.0))||((m_onSlippery)&&(m_accelX>0.0)) )
                    m_animator.switchAnimation(MatrixAnimator::Run, m_direction,
                                               (100-((velX*12)<85 ? velX*12 : 85)) );
                else if( ((!m_onSlippery)&& (velX<0.0))||((m_onSlippery)&&(m_accelX<0.0)) )
                    m_animator.switchAnimation(MatrixAnimator::Run, m_direction,
                                             (100-((-velX*12)<85 ? -velX*12 : 85)) );
                else
                    m_animator.switchAnimation(MatrixAnimator::Idle, m_direction, 64);
            }
        }
    /**********************************Animation switcher**********************************/
}

