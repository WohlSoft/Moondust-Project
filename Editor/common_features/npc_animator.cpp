/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

static  QPixmap *tempImage = nullptr;
static  int AdvNpcAnimator_count = 0;

inline void constructTempImage()
{
    if(!tempImage)
    {
        tempImage = new QPixmap(QSize(32, 32));
        tempImage->fill(QColor(Qt::red));
    }

    AdvNpcAnimator_count++;
}

inline void destroyTempImage()
{
    AdvNpcAnimator_count--;

    if((AdvNpcAnimator_count <= 0) && (tempImage))
    {
        delete tempImage;
        tempImage = nullptr;
    }
}

AdvNpcAnimator::AdvNpcAnimator():
    m_timer(nullptr)
{
    constructTempImage();
}

AdvNpcAnimator::AdvNpcAnimator(QPixmap &sprite, obj_npc &config):
    m_timer(nullptr)
{
    constructTempImage();
    buildAnimator(sprite, config);
}

AdvNpcAnimator::~AdvNpcAnimator()
{
    if(m_timer)
        delete m_timer;

    destroyTempImage();
}

void AdvNpcAnimator::buildAnimator(QPixmap &sprite, obj_npc &config)
{
    if(m_timer)
    {
        m_timer->stop();
        delete m_timer;
    }

    //frames.clear();
    m_mainImage = &sprite;
    m_setup = config;
    m_animated = false;
    m_aniBiDirect = false;
    m_curDirect = -1;
    m_frameStep = 1;
    m_frameHeight = 1;
    m_frameCurrentRealL = 0; //Real frame
    m_frameCurrentRealR = 0; //Real frame
    m_frameCurrentTimerL = 0; //Timer frame
    m_frameCurrentTimerR = 0; //Timer frame
    m_frameFirstL =  0; //from first frame
    m_frameLastL  = -1; //to unlimited frameset
    m_frameFirstR =  0; //from first frame
    m_frameLastR  = -1; //to unlimited frameset
    m_animated = true;
    m_frameSpeed = static_cast<int>(m_setup.setup.framespeed);
    m_frameStyle = (int)m_setup.setup.framestyle;
    m_frameStep  = 1;
    m_frameSequance = false;
    m_aniBiDirect  = m_setup.setup.ani_bidir;
    m_customAnimate = m_setup.setup.custom_animate;
    //bool refreshFrames = updFrames;
    m_frameHeight = (int)m_setup.setup.gfx_h; // height of one frame
    m_frameWidth  = (int)m_setup.setup.gfx_w; //width of target image
    m_spriteHeight = m_mainImage->height(); // Height of target image
    m_framesCountOneSide = static_cast<int>(m_setup.setup.frames);
    //int framesMod = spriteHeight % frameHeight;
    //int roundedSpriteHeight = (spriteHeight-framesMod) / frameHeight;
    int framesZoneHeight = 0;
    m_framesCountTotal = 0;

    //Protectors
    if(m_frameHeight <= 0) m_frameHeight = 1;
    if(m_frameHeight > m_spriteHeight) m_frameHeight = m_spriteHeight;
    if(m_frameWidth <= 0) m_frameWidth = 1;

    if(m_frameWidth > m_mainImage->width())
        m_frameWidth = m_mainImage->width();

    int frameBottomSpace = m_spriteHeight % m_frameHeight;
    framesZoneHeight = m_spriteHeight + (frameBottomSpace == 0 ? 0 : m_frameHeight - frameBottomSpace);
    m_framesCountTotal = framesZoneHeight / m_frameHeight;

    //while(framesZoneHeight < spriteHeight)
    //{
    //    framesCountTotal++;
    //    framesZoneHeight += frameHeight;
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
        m_frameSequance = true;
        m_framesListL = m_setup.setup.frames_left;
        m_framesListR = m_setup.setup.frames_right;
        m_frameFirstL = 0;
        m_frameFirstR = 0;
        m_frameLastL = m_framesListL.size() - 1;
        m_frameLastR = m_framesListR.size() - 1;
    }
    else
    {
        //Standard animation
        switch(m_frameStyle)
        {
        case 2: //Left-Right-upper sprite
            m_framesCountOneSide = (int)m_setup.setup.frames * 4;
            //left
            m_frameFirstL = 0;
            m_frameLastL = (int)(m_framesCountOneSide - (m_framesCountOneSide / 4) * 3) - 1;
            //Right
            m_frameFirstR = (int)(m_framesCountOneSide - (m_framesCountOneSide / 4) * 3);
            m_frameLastR = (int)(m_framesCountOneSide / 2) - 1;
            break;

        case 1: //Left-Right sprite
            m_framesCountOneSide = (int)m_setup.setup.frames * 2;
            //left
            m_frameFirstL = 0;
            m_frameLastL = (int)(m_framesCountOneSide / 2) - 1;
            //Right
            m_frameFirstR = (int)(m_framesCountOneSide / 2);
            m_frameLastR = m_framesCountOneSide - 1;
            break;

        case 0: //Single sprite
        default:
            m_frameFirstL = 0;
            m_frameLastL = m_framesCountOneSide - 1;
            m_frameFirstR = 0;
            m_frameLastR = m_framesCountOneSide - 1;
            break;
        }
    }

#ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("NPC-%1, framestyle is %2").arg(setup.id).arg(setup.framestyle));
#endif
    //curDirect  = dir;
    //setOffset(imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );
    m_timer = new QTimer(this);
    m_timer->connect(
        m_timer, SIGNAL(timeout()),
        this,
        SLOT(nextFrame()));
    m_timer->setTimerType(Qt::PreciseTimer);
    //createAnimationFrames();
    setFrameL(getFrameNumL(m_frameFirstL));
    setFrameR(getFrameNumR(m_frameFirstR));
}

QPixmap AdvNpcAnimator::image(int dir, int frame)
{
    if((frame < 0) || (frame >= m_framesCountTotal))
    {
        if(dir <= 0)
            return m_mainImage->copy(0, m_frameCurrentRealL * frame, m_frameWidth, m_frameHeight); //frames[CurrentFrameL];
        else
            return m_mainImage->copy(0, m_frameCurrentRealR * frame, m_frameWidth, m_frameHeight); //frames[CurrentFrameR];
    }
    else
        return m_mainImage->copy(0, m_frameHeight * frame, m_frameWidth, m_frameHeight);
}

QPixmap &AdvNpcAnimator::wholeImage()
{
    return *m_mainImage;
}

QRect &AdvNpcAnimator::frameRect(int dir)
{
    if(dir <= 0)
        return m_frameRectL;
    else
        return m_frameRectR;
}

void AdvNpcAnimator::setFrameL(int y)
{
    //if(frames.isEmpty()) return;

    //Out of range protection
    if(y < m_frameFirstL) y = (m_frameLastL < 0) ? m_framesCountTotal - 1 : m_frameLastL;
    if(y >= m_framesCountTotal) y = (m_frameFirstL < m_framesCountTotal) ? m_frameFirstL : 0;
    m_frameCurrentRealL = y;
    m_frameRectL.setRect(0, m_frameHeight * m_frameCurrentRealL, m_frameWidth, m_frameHeight);
}

void AdvNpcAnimator::setFrameR(int y)
{
    //if(frames.isEmpty()) return;

    //Out of range protection
    if(y < m_frameFirstR) y = (m_frameLastR < 0) ? m_framesCountTotal - 1 : m_frameLastR;
    if(y >= m_framesCountTotal) y = (m_frameFirstR < m_framesCountTotal) ? m_frameFirstR : 0;
    m_frameCurrentRealR = y;
    m_frameRectR.setRect(0, m_frameHeight * m_frameCurrentRealR, m_frameWidth, m_frameHeight);
}

void AdvNpcAnimator::start()
{
    if(!m_animated) return;
    if((m_frameLastL > 0) && ((m_frameLastL - m_frameFirstL) <= 0)) return; //Don't start singleFrame animation
    if((m_frameLastR > 0) && ((m_frameLastR - m_frameFirstR) <= 0)) return; //Don't start singleFrame animation
    m_frameCurrentTimerL = m_frameFirstL;
    m_frameCurrentTimerR = m_frameFirstR;
    m_timer->start(m_frameSpeed);
}

void AdvNpcAnimator::stop()
{
    if(!m_animated) return;

    m_timer->stop();
    setFrameL(getFrameNumL(m_frameFirstL));
    setFrameR(getFrameNumR(m_frameFirstR));
}

void AdvNpcAnimator::nextFrame()
{
    //Left
    if(!m_aniDirectL)
    {
        //frameCurrent += frameSize * frameStep;
        m_frameCurrentTimerL += m_frameStep;

        if(((m_frameCurrentTimerL >= m_framesCountTotal - (m_frameStep - 1)) && (m_frameLastL <= -1)) ||
           ((m_frameCurrentTimerL > m_frameLastL) && (m_frameLastL >= 0)))
        {
            if(!m_aniBiDirect)
                m_frameCurrentTimerL = m_frameFirstL;
            else
            {
                m_frameCurrentTimerL -= m_frameStep * 2;
                m_aniDirectL = !m_aniDirectL;
            }
        }
    }
    else
    {
        //frameCurrent -= frameSize * frameStep;
        m_frameCurrentTimerL -= m_frameStep;

        if(m_frameCurrentTimerL < m_frameFirstL)
        {
            if(!m_aniBiDirect)
                m_frameCurrentTimerL = ((m_frameLastL == -1) ? m_framesCountTotal - 1 : m_frameLastL);
            else
            {
                m_frameCurrentTimerL += m_frameStep * 2;
                m_aniDirectL = !m_aniDirectL;
            }
        }
    }

    setFrameL(getFrameNumL(m_frameCurrentTimerL));

    //Right
    if(!m_aniDirectR)
    {
        //frameCurrent += frameSize * frameStep;
        m_frameCurrentTimerR += m_frameStep;

        if(((m_frameCurrentTimerR >= m_framesCountTotal - (m_frameStep - 1)) && (m_frameLastR <= -1)) ||
           ((m_frameCurrentTimerR > m_frameLastR) && (m_frameLastR >= 0)))
        {
            if(!m_aniBiDirect)
                m_frameCurrentTimerR = m_frameFirstR;
            else
            {
                m_frameCurrentTimerR -= m_frameStep * 2;
                m_aniDirectR = !m_aniDirectR;
            }
        }
    }
    else
    {
        //frameCurrent -= frameSize * frameStep;
        m_frameCurrentTimerR -= m_frameStep;

        if(m_frameCurrentTimerR < m_frameFirstR)
        {
            if(!m_aniBiDirect)
                m_frameCurrentTimerR = ((m_frameLastR == -1) ? m_framesCountTotal - 1 : m_frameLastR);
            else
            {
                m_frameCurrentTimerR += m_frameStep * 2;
                m_aniDirectR = !m_aniDirectR;
            }
        }
    }

    setFrameR(getFrameNumR(m_frameCurrentTimerR));
    emit onFrame();
}

int AdvNpcAnimator::getFrameNumL(int num)
{
    if(m_frameSequance)
    {
        if((num < 0) || (num >= m_framesListL.size()))
            num = 0;
        return m_framesListL[num];
    }
    else
        return num;
}

int AdvNpcAnimator::getFrameNumR(int num)
{
    if(m_frameSequance)
    {
        if((num < 0) || (num >= m_framesListR.size()))
            num = 0;
        return m_framesListR[num];
    }
    else
        return num;
}
