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

#include "simple_animator.h"
#include <Utils/maths.h>
#include "logger.h"

SimpleAnimator::SimpleAnimator()
{
    construct(false, 1, 64, 0, -1, false, false);
}

SimpleAnimator::SimpleAnimator(const SimpleAnimator &animator):
    m_frameDelay(animator.m_frameDelay),
    m_pos1(animator.m_pos1),
    m_pos2(animator.m_pos2),
    m_nextFrameTime(animator.m_nextFrameTime),
    m_onceMode(animator.m_onceMode),
    m_onceMode_loops(animator.m_onceMode_loops),
    m_animationFinished(animator.m_animationFinished),
    m_frameSequanceEnabled(animator.m_frameSequanceEnabled),
    m_frameSequance(animator.m_frameSequance),
    m_frameSequanceCur(animator.m_frameSequanceCur),
    m_currentFrame(animator.m_currentFrame),
    m_animated(animator.m_animated),
    m_bidirectional(animator.m_bidirectional),
    m_reverce(animator.m_reverce),
    m_isEnabled(animator.m_isEnabled),
    m_timerId(0),
    m_framesCount(animator.m_framesCount),
    m_frameFirst(animator.m_frameFirst),
    m_frameLast(animator.m_frameLast)
{}

SimpleAnimator::SimpleAnimator(bool enables, int framesq, int fspeed, int First, int Last, bool rev, bool bid)
{
    construct(enables, framesq, fspeed, First, Last, rev, bid);
}

void SimpleAnimator::construct(bool enables, int framesq, int fspeed, int First, int Last, bool rev, bool bid)
{
    m_animated = enables;
    m_frameFirst = First;
    m_frameLast = Last;
    m_currentFrame = 0;
    m_frameSequanceEnabled = false;
    m_frameSequanceCur = 0;
    m_nextFrameTime = fspeed;
    m_onceMode = false;
    m_animationFinished = false;
    m_onceMode_loops = 1;
    m_isEnabled = false;
    m_pos1 = 0.0;
    m_pos2 = 1.0;
    m_bidirectional = bid;
    m_reverce = rev;
    m_frameDelay = fspeed;
    m_framesCount = framesq;
    m_timerId = 0;

    if(m_framesCount <= 0.0)
        m_framesCount = 1.0;

    setFrame(m_frameFirst);
}

void SimpleAnimator::setFrameSequance(std::vector<int> sequance)
{
    m_frameSequance = sequance;
    m_frameSequanceEnabled = true;
    m_frameSequanceCur = 0;
    m_animationFinished = false;

    if(!m_frameSequance.empty())
    {
        m_currentFrame = m_frameSequance[m_frameSequanceCur];
        m_pos1 = m_currentFrame / m_framesCount;
        m_pos2 = m_currentFrame / m_framesCount + 1.0 / m_framesCount;
        m_nextFrameTime = m_frameDelay;
    }
}

SimpleAnimator::~SimpleAnimator()
{
    this->stop();
}

bool SimpleAnimator::operator!=(const SimpleAnimator &animator) const
{
    return !(*this == animator);
}

bool SimpleAnimator::operator==(const SimpleAnimator &animator) const
{
    if(!Maths::equals(animator.m_frameDelay, m_frameDelay))
        return false;

    if(animator.m_animated != m_animated)
        return false;

    if(!Maths::equals(animator.m_framesCount, m_framesCount))
        return false;

    if(animator.m_frameFirst != m_frameFirst)
        return false;

    if(animator.m_frameLast != m_frameLast)
        return false;

    if(animator.m_reverce != m_reverce)
        return false;

    if(animator.m_bidirectional != m_bidirectional)
        return false;

    if(!Maths::equals(animator.m_nextFrameTime, m_nextFrameTime))
        return false;

    if(animator.m_animationFinished != m_animationFinished)
        return false;

    if(animator.m_onceMode != m_onceMode)
        return false;

    return true;
}

//Returns images
AniPos SimpleAnimator::image(double frame)
{
    if((frame < 0) || (frame >= m_framesCount))
        return AniPos(m_pos1, m_pos2);
    else
        return AniPos(frame / m_framesCount, frame / m_framesCount + 1.0 / m_framesCount);
}

//Animation process
void SimpleAnimator::nextFrame()
{
    if(m_animationFinished) return;

    if(m_frameSequanceEnabled)
    {
        m_frameSequanceCur++;

        if(m_frameSequanceCur < 0)
            m_frameSequanceCur = 0;

        if(m_frameSequanceCur >= int32_t(m_frameSequance.size()))
        {
            m_frameSequanceCur = 0;

            if(m_onceMode)
            {
                m_onceMode_loops--;

                if(m_onceMode_loops <= 0)
                    m_animationFinished = true;
            }
        }

        if(!m_frameSequance.empty())
            m_currentFrame = m_frameSequance[m_frameSequanceCur];

        goto makeFrame;
    }

    if(m_reverce)
    {
        // Reverce animation
        m_currentFrame--;

        if(m_currentFrame < m_frameFirst)
        {
            if(m_bidirectional)
            {
                m_reverce = !m_reverce; // change direction on first frame
                m_currentFrame += 2;

                if(m_onceMode)
                {
                    m_onceMode_loops--;

                    if(m_onceMode_loops <= 0)
                        m_animationFinished = true;
                }
            }
            else
            {
                // Return to last frame;
                if(m_frameLast < 0)
                    m_currentFrame = static_cast<int>(m_framesCount) - 1;
                else
                    m_currentFrame = m_frameLast;

                if(m_onceMode)
                {
                    m_onceMode_loops--;

                    if(m_onceMode_loops <= 0)
                        m_animationFinished = true;
                }
            }
        }
    }
    else
    {
        // Direct animation
        m_currentFrame++;

        if(((m_currentFrame >= m_framesCount) && (m_frameLast < 0)) ||
           ((m_currentFrame > m_frameLast) && (m_frameLast >= 0)))
        {
            if(m_bidirectional)
            {
                m_reverce = !m_reverce; // change direction on last frame
                m_currentFrame -= 2;
            }
            else
            {
                m_currentFrame = m_frameFirst; // Return to first frame;

                if(m_onceMode)
                {
                    m_onceMode_loops--;

                    if(m_onceMode_loops <= 0)
                        m_animationFinished = true;
                }
            }
        }
    }

makeFrame:
    m_pos1 = m_currentFrame / m_framesCount;
    m_pos2 = m_currentFrame / m_framesCount + 1.0 / m_framesCount;

    if(m_isEnabled)
        m_timerId = SDL_AddTimer(static_cast<Uint32>(m_frameDelay), &SimpleAnimator::TickAnimation, this);
}


void SimpleAnimator::setFrame(int y)
{
    if(m_frameSequanceEnabled)
    {
        if((y >= 0) && (y < int32_t(m_frameSequance.size())))
        {
            m_frameSequanceCur = y;
            m_currentFrame = m_frameSequance[m_frameSequanceCur];
        }
    }
    else
    {
        if(y >= m_framesCount)
            y = m_frameFirst;

        if(y < m_frameFirst)
            y = (m_frameLast < 0) ? static_cast<int32_t>(m_framesCount) - 1 : m_frameLast;

        m_currentFrame = y;
    }

    m_pos1 = m_currentFrame / m_framesCount;
    m_pos2 = m_currentFrame / m_framesCount + 1.0 / m_framesCount;
}

void SimpleAnimator::setFrames(int first, int last)
{
    if((m_frameFirst == first) && (m_frameLast == last)) return;

    m_frameFirst = first;
    m_frameLast = last;
    setFrame(m_frameFirst);
}

void SimpleAnimator::start()
{
    if(!m_animated)
        return;

    if(m_isEnabled)
        return;

    if((m_frameLast > 0) && ((m_frameLast - m_frameFirst) <= 1))
        return; //Don't start singleFrame animation

    m_isEnabled = true;
    m_timerId = SDL_AddTimer(static_cast<Uint32>(m_frameDelay), &SimpleAnimator::TickAnimation, this);
}

void SimpleAnimator::stop()
{
    if(!m_animated) return;

    if(!m_isEnabled) return;

    m_isEnabled = false;
    SDL_RemoveTimer(m_timerId);
    setFrame(m_frameFirst);
}

unsigned int SimpleAnimator::TickAnimation(unsigned int x, void *p)
{
    (void)(x);
    SimpleAnimator *self = reinterpret_cast<SimpleAnimator *>(p);
    self->nextFrame();
    return 0;
}

void SimpleAnimator::setOnceMode(bool once, int loops)
{
    m_onceMode = once;
    m_onceMode_loops = loops;
    m_animationFinished = false;

    if(once)
        setFrame(m_frameFirst);
}

//Ability to tick animation manually!
void SimpleAnimator::manualTick(double ticks)
{
    if(m_frameDelay < 1.0)
        return; //Idling animation

    m_nextFrameTime -= std::fabs(ticks);

    while(m_nextFrameTime <= 0.0)
    {
        nextFrame();
        m_nextFrameTime += m_frameDelay;
    }
}

bool SimpleAnimator::isFinished()
{
    return m_animationFinished;
}
