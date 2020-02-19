/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "simple_animator.h"
#include "logger.h"

SimpleAnimator::SimpleAnimator(QObject *parent)
    :QObject(parent), TimedAnimation()
{
    frame_delay = 0;
    ticks_left  = 0.0f;
    m_currentFrame = 0;
    QPixmap dummy(2, 2);
    dummy.fill(Qt::white);
    setSettings(dummy, false, 1, 64, 0, -1, false, false);
}

SimpleAnimator::SimpleAnimator(const SimpleAnimator &a, QObject *parent):
    QObject(parent), TimedAnimation(a)
{
    m_texture = a.m_texture;
    m_animated = a.m_animated;
    m_frameFirst = a.m_frameFirst;
    m_frameLast = a.m_frameLast;
    m_frame_sequance_enabled=a.m_frame_sequance_enabled;
    m_frame_sequance = a.m_frame_sequance;
    m_frame_sequance_cur = a.m_frame_sequance_cur;
    m_frame_rect = a.m_frame_rect;
    m_currentFrame = a.m_currentFrame;
    m_bidirectional = a.m_bidirectional;
    m_reverce = a.m_reverce;
    m_frameDelay = a.m_frameDelay;
    m_framesCount = a.m_framesCount;
    m_frameWidth = a.m_frameWidth;
    m_frameHeight = a.m_frameHeight;
    m_frameHeight = a.m_frameHeight;
    //Inherets!
    frame_delay = a.frame_delay;
    ticks_left  = a.ticks_left;
}

SimpleAnimator &SimpleAnimator::operator=(const SimpleAnimator &a)
{
    m_texture = a.m_texture;
    m_animated = a.m_animated;
    m_frameFirst = a.m_frameFirst;
    m_frameLast = a.m_frameLast;
    m_currentFrame = a.m_currentFrame;
    m_frame_sequance_enabled = a.m_frame_sequance_enabled;
    m_frame_sequance = a.m_frame_sequance;
    m_frame_sequance_cur = a.m_frame_sequance_cur;
    m_frame_rect = a.m_frame_rect;
    m_bidirectional = a.m_bidirectional;
    m_reverce = a.m_reverce;
    m_frameDelay = a.m_frameDelay;
    m_framesCount = a.m_framesCount;
    m_frameWidth = a.m_frameWidth;
    m_frameHeight = a.m_frameHeight;
    m_frameHeight = a.m_frameHeight;
    //frames = a.frames;

    //Inherets!
    frame_delay = a.frame_delay;
    ticks_left  = a.ticks_left;

//    connect(
//                &timer, SIGNAL(timeout()),
//                this,
//                SLOT( nextFrame() ) );
//    if(a.timer.isActive())
//        start();
    return *this;
}

SimpleAnimator::SimpleAnimator(QPixmap &sprite, bool enables, int framesq, int fspeed, int First, int Last, bool rev, bool bid, QObject *parent)
    : QObject(parent), TimedAnimation()
{
    m_currentFrame = 0;
    setSettings(sprite, enables, framesq, fspeed, First, Last, rev, bid);
}

SimpleAnimator::~SimpleAnimator()
{}

void SimpleAnimator::setSettings(QPixmap &sprite, bool enables, int framesq, int fspeed, int First, int Last, bool rev, bool bid)
{
    m_texture = &sprite;
    m_animated = enables;
    m_frameFirst = First;
    m_frameLast = Last;
    m_currentFrame = 0;
    ticks_left = 0.0f;

    m_frame_sequance_enabled=false;
    m_frame_sequance_cur=0;

    m_bidirectional = bid;
    m_reverce = rev;

    m_frameDelay = fspeed;
    frame_delay = fspeed;
    m_framesCount = framesq;

    if(m_texture->isNull())
    {
        m_animated=false;
        LogWarning("SimpleAnimator can't work with null images");
        return;
    }

    m_frameWidth = m_texture->width();
    m_spriteHeight = m_texture->height();

    // Frame must not be less than 1 pixel
    if( m_framesCount > m_spriteHeight)
        m_framesCount = m_frameHeight;

    if(m_framesCount <= 0) //Avoid SIGFPE if so small image while so much frames
        m_framesCount = 1;

    if(m_animated)
        m_frameHeight = qRound(qreal(m_spriteHeight/m_framesCount));
    else
        m_frameHeight = m_spriteHeight;
    m_framePos = QPoint(0,0);

    //createAnimationFrames();

    resetFrame();

//    connect(
//                &timer, SIGNAL(timeout()),
//                this,
//                SLOT( nextFrame() ) );
}

//FIXME!! Implement support of frame-sequences in the frameNext!!!
void SimpleAnimator::setFrameSequance(QList<int> sequance)
{
    if(sequance.isEmpty())
        return;
    m_frame_sequance=sequance;
    m_frame_sequance_enabled=true;
    m_frame_sequance_cur = 0;
    m_currentFrame = m_frame_sequance[m_frame_sequance_cur];
    m_frame_rect.setRect(0, m_frameHeight*m_currentFrame, m_frameWidth, m_frameHeight);
    if( m_frame_sequance.size() <=1 )
        m_animated=false;
}


//Returns images

QPixmap SimpleAnimator::image(int frame)
{
//    if(frames.isEmpty())
//    {   //If animator haven't frames, return red sqare
//        QPixmap tmp = QPixmap(QSize(32,32));
//        tmp.fill(QColor(Qt::red));
//        return tmp;
//    }
    if( frame<0 || frame >= m_framesCount)
        frame = m_currentFrame;
//    if((frame<0)||(frame>=frames.size()))
//        return frames[CurrentFrame];//mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
//    else
//        return frames[frame];//mainImage.copy(QRect(framePos.x(), frameSize*frame, frameWidth, frameSize ));
    return m_texture->copy(QRect(0, frame * m_frameHeight, m_frameWidth, m_frameHeight ));
}

QRect &SimpleAnimator::frameRect()
{
    return m_frame_rect;
}

QRectF SimpleAnimator::frameRectF()
{
    return QRectF(m_frame_rect);
}

QPixmap &SimpleAnimator::wholeImage()
{
    return *m_texture;
}


//Animation process
void SimpleAnimator::nextFrame()
{
    if(m_frame_sequance_enabled)
    {
        if(m_reverce)
        {
            m_frame_sequance_cur--;
            if(m_frame_sequance_cur < 0)
            {
                if(m_bidirectional)
                {
                    m_reverce = !m_reverce; // change direction on first frame
                    m_frame_sequance_cur+=2;
                    if(m_frame_sequance_cur >= m_frame_sequance.size())
                        m_frame_sequance_cur = m_frame_sequance.size()-1;
                }
                else
                {
                    m_frame_sequance_cur = m_frame_sequance.size()-1;
                }
            }
        }
        else
        {
            m_frame_sequance_cur++;
            if(m_frame_sequance_cur < 0)
                m_frame_sequance_cur = 0;
            if(m_frame_sequance_cur >= m_frame_sequance.size())
            {
                if(m_bidirectional)
                {
                    m_reverce=!m_reverce; // change direction on last frame
                    m_frame_sequance_cur -= 2;
                    if(m_frame_sequance_cur < 0)
                        m_frame_sequance_cur = 0;
                }
                else
                {
                    m_frame_sequance_cur = 0; // Return to first frame;
                }
            }
        }
        if( (!m_frame_sequance.isEmpty()) && (m_frame_sequance_cur<m_frame_sequance.size()) )
            m_currentFrame = m_frame_sequance[m_frame_sequance_cur];
        if(m_currentFrame<0 || m_currentFrame >= m_framesCount)
            m_currentFrame = m_frameFirst;
    }
    else
    {
        if(m_reverce)
        { // Reverce animation
            m_currentFrame--;
            if(m_currentFrame<m_frameFirst)
            {
                if(m_bidirectional)
                {
                    m_reverce=!m_reverce; // change direction on first frame
                    m_currentFrame+=2;

                    if( ((m_currentFrame>=m_framesCount)&&(m_frameLast<0)) ||
                        ((m_currentFrame>m_frameLast)&&(m_frameLast>=0)) )
                    {
                        // Return to last frame;
                        if(m_frameLast<0)
                            m_currentFrame=m_framesCount-1;
                        else
                            m_currentFrame=m_frameLast;
                    }
                }
                else
                {
                    // Return to last frame;
                    if(m_frameLast<0)
                        m_currentFrame=m_framesCount-1;
                    else
                        m_currentFrame=m_frameLast;
                }
            }
        }
        else
        { // Direct animation
            m_currentFrame++;
            if(((m_currentFrame>=m_framesCount)&&(m_frameLast<0))||
                    ((m_currentFrame>m_frameLast)&&(m_frameLast>=0)))
            {
                if(m_bidirectional)
                {
                    m_reverce=!m_reverce; // change direction on last frame
                    m_currentFrame-=2;
                    if( m_currentFrame < 0 )
                        m_currentFrame = 0;
                }
                else
                {
                    m_currentFrame=m_frameFirst; // Return to first frame;
                }
            }
        }
    }
    m_frame_rect.setRect(0, m_frameHeight*m_currentFrame, m_frameWidth, m_frameHeight);
}

//void SimpleAnimator::createAnimationFrames()
//{
//    for(int i=0; (i < framesCount); i++)
//    {
//        frames.push_back( mainImage.copy(QRect(framePos.x(), frameHeight*i, frameWidth, frameHeight )) );
//    }
//}


void SimpleAnimator::setFrame(int fnum)
{
    if(m_frame_sequance_enabled)
    {
        if(fnum<0)
            fnum = 0;
        if(!m_frame_sequance.isEmpty())
        {
            if(fnum >= m_frame_sequance.size())
                fnum = 0;
            m_frame_sequance_cur = fnum;
            fnum = m_frame_sequance[fnum];
        }
    }
    //if(y>=frames.size()) y= frameFirst;
    if(fnum>=m_framesCount)
        fnum = m_frameFirst;
    //if(y<frameFirst) y = (frameLast<0)? frames.size()-1 : frameLast;
    if(fnum<m_frameFirst)
        fnum = (m_frameLast<0)? m_framesCount-1 : m_frameLast;
    m_currentFrame = fnum;
    m_frame_rect.setRect(0, m_frameHeight*fnum, m_frameWidth, m_frameHeight);
}

int SimpleAnimator::frame()
{
    return m_currentFrame;
}

void SimpleAnimator::start()
{
    if(!m_animated) return;
    if((m_frameLast>0)&&((m_frameLast-m_frameFirst)<=1)) return; //Don't start singleFrame animation
}

void SimpleAnimator::stop()
{
    if(!m_animated) return;
    resetFrame();
}

void SimpleAnimator::resetFrame()
{
    setFrame(m_frameFirst);
    ticks_left = 0.0f;
}


