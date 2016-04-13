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

#include "simple_animator.h"
#include "logger.h"

SimpleAnimator::SimpleAnimator(QObject *parent)
    :QObject(parent), TimedAnimation()
{
    QPixmap dummy;
    setSettings(dummy, false, 1, 64, 0, -1, false, false);
}

SimpleAnimator::SimpleAnimator(const SimpleAnimator &a, QObject *parent):
    QObject(parent), TimedAnimation(a)
{
    mainImage = a.mainImage;
    animated = a.animated;
    frameFirst = a.frameFirst;
    frameLast = a.frameLast;
    frame_sequance_enabled=a.frame_sequance_enabled;
    frame_sequance=a.frame_sequance;
    frame_sequance_cur=a.frame_sequance_cur;
    CurrentFrame = a.CurrentFrame;
    frameCurrent = a.frameCurrent;
    bidirectional = a.bidirectional;
    reverce = a.reverce;
    speed = a.speed;
    framesQ = a.framesQ;
    frameWidth = a.frameWidth;
    frameHeight = a.frameHeight;
    frameSize = a.frameSize;
    frames = a.frames;

    //Inherets!
    frame_delay = a.frameCurrent;
    ticks_left  = a.ticks_left;
//    connect(
//                &timer, SIGNAL(timeout()),
//                this,
//                SLOT( nextFrame() ) );
//    if(a.timer.isActive())
//        start();
}

SimpleAnimator &SimpleAnimator::operator=(const SimpleAnimator &a)
{
    mainImage = a.mainImage;
    animated = a.animated;
    frameFirst = a.frameFirst;
    frameLast = a.frameLast;
    CurrentFrame = a.CurrentFrame;
    frame_sequance_enabled = a.frame_sequance_enabled;
    frame_sequance = a.frame_sequance;
    frame_sequance_cur = a.frame_sequance_cur;
    frameCurrent = a.frameCurrent;
    bidirectional = a.bidirectional;
    reverce = a.reverce;
    speed = a.speed;
    framesQ = a.framesQ;
    frameWidth = a.frameWidth;
    frameHeight = a.frameHeight;
    frameSize = a.frameSize;
    frames = a.frames;

    //Inherets!
    frame_delay = a.frameCurrent;
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
    setSettings(sprite, enables, framesq, fspeed, First, Last, rev, bid);
}

SimpleAnimator::~SimpleAnimator()
{}

void SimpleAnimator::setSettings(QPixmap &sprite, bool enables, int framesq, int fspeed, int First, int Last, bool rev, bool bid)
{
    mainImage = sprite;
    animated = enables;
    frameFirst = First;
    frameLast = Last;
    CurrentFrame = 0;

    frame_sequance_enabled=false;
    frame_sequance_cur=0;

    bidirectional = bid;
    reverce = rev;

    speed = fspeed;
    frame_delay = fspeed;
    framesQ = framesq;

    if(mainImage.isNull())
    {
        animated=false;
        LogWarning("SimpleAnimator can't work with null images");
        return;
    }

    frameWidth = mainImage.width();
    frameHeight = mainImage.height();

    // Frame must not be less than 1 pixel
    if(framesQ>frameHeight) framesQ=frameHeight;

    if(animated)
        frameSize = qRound(qreal(frameHeight/framesQ));
    else
        frameSize = frameHeight;
    framePos = QPoint(0,0);

    createAnimationFrames();

    resetFrame();

//    connect(
//                &timer, SIGNAL(timeout()),
//                this,
//                SLOT( nextFrame() ) );
}

//FIXME!! Implement support of frame-sequences in the frameNext!!!
void SimpleAnimator::setFrameSequance(QList<int> sequance)
{
    frame_sequance=sequance;
    frame_sequance_enabled=true;
    frame_sequance_cur=0;
    //animationFinished=false;
    if(!frame_sequance.isEmpty())
    {
        CurrentFrame = frame_sequance[frame_sequance_cur];
        //pos1 = CurrentFrame/framesQ;
        //pos2 = CurrentFrame/framesQ + 1.0/framesQ;
        //manual_ticks = speed;
    }
}


//Returns images

QPixmap SimpleAnimator::image(int frame)
{
    if(frames.isEmpty())
    {   //If animator haven't frames, return red sqare
        QPixmap tmp = QPixmap(QSize(32,32));
        tmp.fill(QColor(Qt::red));
        return tmp;
    }

    // QMutexLocker locker(&mutex); //Glitch protection
    if((frame<0)||(frame>=frames.size()))
        return frames[CurrentFrame];//mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
    else
        return frames[frame];//mainImage.copy(QRect(framePos.x(), frameSize*frame, frameWidth, frameSize ));
}

QPixmap SimpleAnimator::wholeImage()
{
    return mainImage;
}


//Animation process
void SimpleAnimator::nextFrame()
{
    if(reverce)
    { // Reverce animation
        CurrentFrame--;
        if(CurrentFrame<frameFirst)
        {
            if(bidirectional)
            {
                reverce=!reverce; // change direction on first frame
                CurrentFrame+=2;
            }
            else
            {
                 // Return to last frame;
                if(frameLast<0)
                    CurrentFrame=frames.size()-1;
                else
                    CurrentFrame=frameLast;
            }
        }
    }
    else
    { // Direct animation
        CurrentFrame++;
        if(((CurrentFrame>=frames.size())&&(frameLast<0))||
           ((CurrentFrame>frameLast)&&(frameLast>=0)))
        {
            if(bidirectional)
            {
                reverce=!reverce; // change direction on last frame
                CurrentFrame-=2;
            }
            else
            {
                CurrentFrame=frameFirst; // Return to first frame;
            }
        }
    }
}

void SimpleAnimator::createAnimationFrames()
{
    for(int i=0;i<framesQ;i++)
    {
        frames.push_back( mainImage.copy(QRect(framePos.x(), frameSize *i, frameWidth, frameSize )) );
    }
}


void SimpleAnimator::setFrame(int y)
{
    if(y>=frames.size()) y= frameFirst;
    if(y<frameFirst) y = (frameLast<0)? frames.size()-1 : frameLast;
    CurrentFrame = y;
}

void SimpleAnimator::start()
{
    if(!animated) return;
    if((frameLast>0)&&((frameLast-frameFirst)<=1)) return; //Don't start singleFrame animation

    timer.setTimerType(Qt::PreciseTimer);
    timer.start(speed);
}

void SimpleAnimator::stop()
{
    if(!animated) return;
    timer.stop();
    resetFrame();
}

void SimpleAnimator::resetFrame()
{
    setFrame(frameFirst);
    ticks_left = 0.0f;
}


