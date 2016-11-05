
/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "logger.h"
#include "npc_animator.h"
#include <cmath>
#include <cstdlib>

AdvNpcAnimator::AdvNpcAnimator()
{
    m_isValid = false;
    m_ticks = 0;
    m_onceMode = false;
    m_animationFinished = false;
    m_isEnabled = false;
    m_animated = false;
    m_frameDelay = 0.0;
    m_frameStyle = 0;
    m_aniBiDirect = false;
    m_frameStep = 0;
    m_customAnimate = false;
    m_customAniAlg = 0;
    m_custom_frameFL = 0;
    m_custom_frameEL = 0;
    m_custom_frameFR = 0;
    m_custom_frameER = 0;
    m_frameSequance = false;
    m_framesCount = 0;
    m_frameHeight = 0.0;
    m_frameWidth = 0.0;
    m_spriteHeight = 0.0;
    m_aniDirectL = 0;
    m_aniDirectR = 0;
    m_curFrameL_real = 0;
    m_curFrameR_real = 0;
    m_curFrameL_timer = 0;
    m_curFrameR_timer = 0;
    m_frameFirstL = 0;
    m_frameLastL = -1;
    m_frameFirstR = 0;
    m_frameLastR = -1;
}

AdvNpcAnimator::AdvNpcAnimator(PGE_Texture &sprite, obj_npc &config)
{
    m_isValid = false;
    m_ticks = 0;
    construct(sprite, config);
}

void AdvNpcAnimator::construct(PGE_Texture &sprite, obj_npc &config)
{
    m_mainImage = sprite;
    m_setup = config;
    m_curFrameL_real = 0; //Real frame
    m_curFrameR_real = 0; //Real frame
    m_curFrameL_timer = 0; //Timer frame
    m_curFrameR_timer = 0; //Timer frame
    m_frameFirstL = 0; //from first frame
    m_frameLastL = -1; //to unlimited frameset
    m_frameFirstR = 0; //from first frame
    m_frameLastR = -1; //to unlimited frameset
    m_onceMode = false;
    m_animationFinished = false;
    m_isEnabled = false;
    m_animated = true;
    m_framesCount = static_cast<int>(m_setup.setup.frames);
    m_frameDelay = m_setup.setup.framespeed;
    m_frameStyle = m_setup.setup.framestyle;
    m_frameStep = 1;
    m_frameSequance = false;
    m_aniBiDirect = m_setup.setup.ani_bidir;
    m_customAniAlg = m_setup.setup.custom_ani_alg;
    m_customAnimate = m_setup.setup.custom_animate;
    m_custom_frameFL = m_setup.setup.custom_ani_fl;//first left
    m_custom_frameEL = m_setup.setup.custom_ani_el;//end left
    m_custom_frameFR = m_setup.setup.custom_ani_fr;//first right
    m_custom_frameER = m_setup.setup.custom_ani_er;//enf right
    //bool refreshFrames = updFrames;
    m_frameHeight = m_setup.setup.gfx_h; // height of one frame
    m_frameWidth = m_setup.setup.gfx_w; //width of target image
    m_spriteHeight = m_mainImage.h; // Height of complete image

    //Protectors
    if(m_frameHeight <= 0)
        m_frameHeight = 1;

    if(m_frameHeight > m_mainImage.h)
        m_frameHeight = m_mainImage.h;

    if(m_frameWidth <= 0)
        m_frameWidth = 1;

    if(m_frameWidth > m_mainImage.w)
        m_frameWidth = m_mainImage.w;

    //int dir=direction;

    //if(direction==0) //if direction=random
    //{
    //    dir=((0==qrand()%2)?-1:1); //set randomly 1 or -1
    //}

    if(m_setup.setup.ani_directed_direct)
    {
        m_aniDirectL = (true) ^ (m_setup.setup.ani_direct);
        m_aniDirectR = (false) ^ (m_setup.setup.ani_direct);
    }
    else
    {
        m_aniDirectL = m_setup.setup.ani_direct;
        m_aniDirectR = m_setup.setup.ani_direct;
    }

    if(m_customAnimate) // User defined spriteSet (example: boss)
    {
        //LEFT
        m_frameFirstL = m_custom_frameFL;

        switch(m_customAniAlg)
        {
        case ANI_CustomSequence:
            m_frameSequance = true;
            s_framesL = m_setup.setup.frames_left;
            m_frameFirstL = 0;
            m_frameLastL = s_framesL.size() - 1;
            break;

        case ANI_FrameJump:
            m_frameStep = m_custom_frameEL;
            m_frameLastL = -1;
            break;

        case ANI_DefaultSequence:
        default:
            m_frameLastL = m_custom_frameEL;
            break;
        }

        //RIGHT
        m_frameFirstR = m_custom_frameFR;

        switch(m_customAniAlg)
        {
        case ANI_CustomSequence:
            m_frameSequance = true;
            s_framesR = m_setup.setup.frames_right;
            m_frameFirstR = 0;
            m_frameLastR = s_framesR.size() - 1;
            break;

        case ANI_FrameJump:
            m_frameStep = m_custom_frameER;
            m_frameLastR = -1;
            break;

        case ANI_DefaultSequence:
        default:
            m_frameLastR = m_custom_frameER;
            break;
        }
    }
    else
    {
        //Standard animation
        switch(m_frameStyle)
        {
        case 2: //Left-Right-upper sprite
            m_framesCount = static_cast<int>(m_setup.setup.frames) * 4;
            //left
            m_frameFirstL = 0;
            m_frameLastL = (m_framesCount - (m_framesCount / 4) * 3) - 1;
            //Right
            m_frameFirstR = (m_framesCount - (m_framesCount / 4) * 3);
            m_frameLastR = (m_framesCount / 2) - 1;
            break;

        case 1: //Left-Right sprite
            m_framesCount = static_cast<int>(m_setup.setup.frames) * 2;
            //left
            m_frameFirstL = 0;
            m_frameLastL = (m_framesCount / 2) - 1;
            //Right
            m_frameFirstR = (m_framesCount / 2);
            m_frameLastR = m_framesCount - 1;
            break;

        case 0: //Single sprite
        default:
            m_frameFirstL = 0;
            m_frameLastL = m_framesCount - 1;
            m_frameFirstR = 0;
            m_frameLastR = m_framesCount - 1;
            break;
        }
    }

    createAnimationFrames();
    setFrameL(m_frameFirstL);
    setFrameR(m_frameFirstR);
    m_isValid = true;
}

AdvNpcAnimator::~AdvNpcAnimator()
{}

AniPos AdvNpcAnimator::image(int dir, int frame)
{
    if(frames.isEmpty())
    {
        //If animator haven't frames, return red sqare
        AniPos tmp(0, 1);
        return tmp;
    }

    if((frame < 0) || (frame >= frames.size()))
    {
        if(dir < 0)
            return frames[m_curFrameL_real];
        else if(dir == 0)
            return frames[m_curFrameL_real];
        else
            return frames[m_curFrameR_real];
    }
    else return frames[frame];
}

AniPos AdvNpcAnimator::wholeImage()
{
    return AniPos(0, 1);
}

void AdvNpcAnimator::setSequenceL(QList<int> _frames)
{
    if(_frames.isEmpty()) return;

    m_customAniAlg = ANI_CustomSequence;
    m_frameSequance = true;
    s_framesL = _frames;
    m_frameFirstL = 0;
    m_frameLastL = _frames.size() - 1;
    m_onceMode = false;
    m_animationFinished = false;

    if(m_curFrameL_timer > s_framesL.size() - 1) m_curFrameL_timer = m_frameFirstL;

    setFrameL(m_frameSequance ? s_framesL[m_curFrameL_timer] : m_curFrameL_timer);
}

void AdvNpcAnimator::setSequenceR(QList<int> _frames)
{
    if(_frames.isEmpty()) return;

    m_customAniAlg = ANI_CustomSequence;
    m_frameSequance = true;
    s_framesR =  _frames;
    m_frameFirstR = 0;
    m_frameLastR = _frames.size() - 1;
    m_onceMode = false;
    m_animationFinished = false;

    if(m_curFrameR_timer > s_framesR.size() - 1) m_curFrameR_timer = m_frameFirstR;

    setFrameR(m_frameSequance ? s_framesR[m_curFrameR_timer] : m_curFrameR_timer);
}

void AdvNpcAnimator::setSequence(QList<int> _frames)
{
    if(_frames.isEmpty()) return;

    m_customAniAlg = ANI_CustomSequence;
    m_frameSequance = true;

    switch(m_frameStyle)
    {
    case 2:
    case 1:
        s_framesL = _frames;
        m_frameFirstL = 0;
        m_frameLastL = _frames.size() - 1;
        s_framesR.clear();

        for(int i = 0; i < _frames.size(); i++)
            s_framesR.push_back(_frames[i] + static_cast<int>(m_setup.setup.frames));

        m_frameFirstR = 0;
        m_frameLastR = _frames.size() - 1;
        break;

    case 0:
    default:
        s_framesL = _frames;
        m_frameFirstL = 0;
        m_frameLastL = _frames.size() - 1;
        s_framesR =  _frames;
        m_frameFirstR = 0;
        m_frameLastR = _frames.size() - 1;
        break;
    }

    m_onceMode = false;
    m_animationFinished = false;

    if(m_curFrameL_timer > s_framesL.size() - 1)
        m_curFrameL_timer = m_frameFirstL;

    if(m_curFrameR_timer > s_framesR.size() - 1)
        m_curFrameR_timer = m_frameFirstR;

    setFrameL(m_frameSequance ? s_framesL[m_curFrameL_timer] : m_curFrameL_timer);
    setFrameR(m_frameSequance ? s_framesR[m_curFrameR_timer] : m_curFrameR_timer);
}

void AdvNpcAnimator::setFrameL(int y)
{
    if(frames.isEmpty())
        return;

    //Out of range protection
    if(y < m_frameFirstL)
        y = (m_frameLastL < 0) ? frames.size() - 1 : m_frameLastL;

    if(y >= frames.size())
        y = (m_frameFirstL < frames.size()) ? m_frameFirstL : 0;

    m_curFrameL_real = y;
}

void AdvNpcAnimator::setFrameR(int y)
{
    if(frames.isEmpty())
        return;

    //Out of range protection
    if(y < m_frameFirstR)
        y = (m_frameLastR < 0) ? frames.size() - 1 : m_frameLastR;

    if(y >= frames.size())
        y = (m_frameFirstR < frames.size()) ? m_frameFirstR : 0;

    m_curFrameR_real = y;
}

int AdvNpcAnimator::frameSpeed()
{
    return static_cast<int>(m_frameDelay);
}

void AdvNpcAnimator::setFrameSpeed(int ms)
{
    if(ms <= 0) return;

    m_frameDelay = ms;
}

void AdvNpcAnimator::setBidirectional(bool bid)
{
    m_aniBiDirect = bid;
}

void AdvNpcAnimator::setDirectedSequence(bool dd)
{
    m_setup.setup.ani_directed_direct = dd;

    if(dd)
    {
        m_aniDirectL = (true) ^ (m_setup.setup.ani_direct);
        m_aniDirectR = (false) ^ (m_setup.setup.ani_direct);
    }
    else
    {
        m_aniDirectL = m_setup.setup.ani_direct;
        m_aniDirectR = m_setup.setup.ani_direct;
    }
}

void AdvNpcAnimator::setOnceMode(bool en)
{
    m_onceMode = en;

    if(en/*&&_animationFinished*/)
    {
        m_animationFinished = false;
        m_curFrameL_timer = m_frameFirstL;
        m_curFrameR_timer = m_frameFirstR;
        setFrameL(m_frameSequance ? s_framesL[m_curFrameL_timer] : m_curFrameL_timer);
        setFrameR(m_frameSequance ? s_framesR[m_curFrameR_timer] : m_curFrameR_timer);
    }
    else
        m_animationFinished = false;
}

bool AdvNpcAnimator::animationFinished()
{
    return this->m_animationFinished;
}

PGE_SizeF AdvNpcAnimator::sizeOfFrame()
{
    return PGE_SizeF(1.0, m_frameHeight / m_spriteHeight);
}

void AdvNpcAnimator::start()
{
    if(!m_animated) return;

    if((m_frameLastL > 0) && ((m_frameLastL - m_frameFirstL) <= 0))
        return; //Don't start singleFrame animation

    if((m_frameLastR > 0) && ((m_frameLastR - m_frameFirstR) <= 0))
        return; //Don't start singleFrame animation

    m_curFrameL_timer = m_frameFirstL;
    m_curFrameR_timer = m_frameFirstR;
    m_ticks = m_frameDelay;
    m_isEnabled = true;
}

void AdvNpcAnimator::stop()
{
    if(!m_animated) return;

    m_isEnabled = false;
    setFrameL(m_frameFirstL);
    setFrameR(m_frameFirstR);
}

void AdvNpcAnimator::manualTick(double ticks)
{
    if(!m_isEnabled) return;

    if(m_frameDelay < 1)
        return; //Idling animation

    m_ticks -= std::fabs(ticks);

    while(m_ticks <= 0.0)
    {
        nextFrame();
        m_ticks += m_frameDelay;
    }
}

void AdvNpcAnimator::nextFrame()
{
    if(!m_isEnabled) return;

    if(m_onceMode && m_animationFinished) return;

    //Left
    if(!m_aniDirectL)
    {
        m_curFrameL_timer += m_frameStep;

        if(((m_curFrameL_timer >= frames.size() - (m_frameStep - 1)) && (m_frameLastL <= -1)) ||
           ((m_curFrameL_timer > m_frameLastL) && (m_frameLastL >= 0)))
        {
            if(!m_aniBiDirect)
                m_curFrameL_timer = m_frameFirstL;
            else
            {
                if(m_onceMode)
                {
                    m_curFrameL_timer -= m_frameStep;
                    m_animationFinished = true;
                }
                else
                {
                    m_curFrameL_timer -= m_frameStep * 2;
                    m_aniDirectL = !m_aniDirectL;
                }
            }
        }
    }
    else
    {
        m_curFrameL_timer -= m_frameStep;

        if(m_curFrameL_timer < m_frameFirstL)
        {
            if(m_onceMode)
            {
                m_curFrameL_timer += m_frameStep;
                m_animationFinished = true;
            }
            else
            {
                if(!m_aniBiDirect)
                    m_curFrameL_timer = ((m_frameLastL == -1) ? frames.size() - 1 : m_frameLastL);
                else
                {
                    m_curFrameL_timer += m_frameStep * 2;
                    m_aniDirectL = !m_aniDirectL;
                }
            }
        }
    }

    setFrameL(m_frameSequance ? s_framesL[m_curFrameL_timer] : m_curFrameL_timer);

    //Right
    if(!m_aniDirectR)
    {
        m_curFrameR_timer += m_frameStep;

        if(((m_curFrameR_timer >= frames.size() - (m_frameStep - 1)) && (m_frameLastR <= -1)) ||
           ((m_curFrameR_timer > m_frameLastR) && (m_frameLastR >= 0)))
        {
            if(!m_aniBiDirect)
                m_curFrameR_timer = m_frameFirstR;
            else
            {
                if(m_onceMode)
                {
                    m_curFrameR_timer -= m_frameStep;
                    m_animationFinished = true;
                }
                else
                {
                    m_curFrameR_timer -= m_frameStep * 2;
                    m_aniDirectR = !m_aniDirectR;
                }
            }
        }
    }
    else
    {
        m_curFrameR_timer -= m_frameStep;

        if(m_curFrameR_timer < m_frameFirstR)
        {
            if(m_onceMode)
            {
                m_curFrameR_timer += m_frameStep;
                m_animationFinished = true;
            }
            else
            {
                if(!m_aniBiDirect)
                    m_curFrameR_timer = ((m_frameLastR == -1) ? frames.size() - 1 : m_frameLastR);
                else
                {
                    m_curFrameR_timer += m_frameStep * 2;
                    m_aniDirectR = !m_aniDirectR;
                }
            }
        }
    }

    setFrameR(m_frameSequance ? s_framesR[m_curFrameR_timer] : m_curFrameR_timer);
}

void AdvNpcAnimator::createAnimationFrames()
{
    frames.clear();

    for(int i = 0; (m_frameHeight * i < m_spriteHeight); i++)
        frames.push_back(AniPos((m_frameHeight * i) / m_spriteHeight, (m_frameHeight * i + m_frameHeight) / m_spriteHeight));
}
