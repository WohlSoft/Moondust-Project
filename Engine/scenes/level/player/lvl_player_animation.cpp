/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

