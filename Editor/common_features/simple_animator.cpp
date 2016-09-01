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
    frame_delay = 0;
    ticks_left  = 0.0f;
    CurrentFrame = 0;
    QPixmap dummy(2, 2);
    dummy.fill(Qt::white);
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
    frame_sequance = a.frame_sequance;
    frame_sequance_cur = a.frame_sequance_cur;
    frame_rect = a.frame_rect;
    CurrentFrame = a.CurrentFrame;
    bidirectional = a.bidirectional;
    reverce = a.reverce;
    frameDelay = a.frameDelay;
    framesCount = a.framesCount;
    frameWidth = a.frameWidth;
    frameHeight = a.frameHeight;
    frameHeight = a.frameHeight;
    //Inherets!
    frame_delay = a.frame_delay;
    ticks_left  = a.ticks_left;
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
    frame_rect = a.frame_rect;
    bidirectional = a.bidirectional;
    reverce = a.reverce;
    frameDelay = a.frameDelay;
    framesCount = a.framesCount;
    frameWidth = a.frameWidth;
    frameHeight = a.frameHeight;
    frameHeight = a.frameHeight;
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
    CurrentFrame = 0;
    setSettings(sprite, enables, framesq, fspeed, First, Last, rev, bid);
}

SimpleAnimator::~SimpleAnimator()
{}

void SimpleAnimator::setSettings(QPixmap &sprite, bool enables, int framesq, int fspeed, int First, int Last, bool rev, bool bid)
{
    mainImage = &sprite;
    animated = enables;
    frameFirst = First;
    frameLast = Last;
    CurrentFrame = 0;
    ticks_left = 0.0f;

    frame_sequance_enabled=false;
    frame_sequance_cur=0;

    bidirectional = bid;
    reverce = rev;

    frameDelay = fspeed;
    frame_delay = fspeed;
    framesCount = framesq;

    if(mainImage->isNull())
    {
        animated=false;
        LogWarning("SimpleAnimator can't work with null images");
        return;
    }

    frameWidth = mainImage->width();
    spriteHeight = mainImage->height();

    // Frame must not be less than 1 pixel
    if( framesCount > spriteHeight)
        framesCount = frameHeight;

    if(animated)
        frameHeight = qRound(qreal(spriteHeight/framesCount));
    else
        frameHeight = spriteHeight;
    framePos = QPoint(0,0);

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
    frame_sequance=sequance;
    frame_sequance_enabled=true;
    frame_sequance_cur = 0;
    CurrentFrame = frame_sequance[frame_sequance_cur];
    frame_rect.setRect(0, frameHeight*CurrentFrame, frameWidth, frameHeight);
    if( frame_sequance.size() <=1 )
        animated=false;
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
    if( frame<0 || frame >= framesCount)
        frame = CurrentFrame;
//    if((frame<0)||(frame>=frames.size()))
//        return frames[CurrentFrame];//mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
//    else
//        return frames[frame];//mainImage.copy(QRect(framePos.x(), frameSize*frame, frameWidth, frameSize ));
    return mainImage->copy(QRect(0, frame * frameHeight, frameWidth, frameHeight ));
}

QRect &SimpleAnimator::frameRect()
{
    return frame_rect;
}

QRectF SimpleAnimator::frameRectF()
{
    return QRectF(frame_rect);
}

QPixmap &SimpleAnimator::wholeImage()
{
    return *mainImage;
}


//Animation process
void SimpleAnimator::nextFrame()
{
    if(frame_sequance_enabled)
    {
        if(reverce)
        {
            frame_sequance_cur--;
            if(frame_sequance_cur < 0)
            {
                if(bidirectional)
                {
                    reverce = !reverce; // change direction on first frame
                    frame_sequance_cur+=2;
                    if(frame_sequance_cur >= frame_sequance.size())
                        frame_sequance_cur = frame_sequance.size()-1;
                }
                else
                {
                    frame_sequance_cur = frame_sequance.size()-1;
                }
            }
        }
        else
        {
            frame_sequance_cur++;
            if(frame_sequance_cur < 0)
                frame_sequance_cur = 0;
            if(frame_sequance_cur >= frame_sequance.size())
            {
                if(bidirectional)
                {
                    reverce=!reverce; // change direction on last frame
                    frame_sequance_cur -= 2;
                    if(frame_sequance_cur < 0)
                        frame_sequance_cur = 0;
                }
                else
                {
                    frame_sequance_cur = 0; // Return to first frame;
                }
            }
        }
        if( (!frame_sequance.isEmpty()) && (frame_sequance_cur<frame_sequance.size()) )
            CurrentFrame = frame_sequance[frame_sequance_cur];
        if(CurrentFrame<0 || CurrentFrame >= framesCount)
            CurrentFrame = frameFirst;
    }
    else
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

                    if( ((CurrentFrame>=framesCount)&&(frameLast<0)) ||
                        ((CurrentFrame>frameLast)&&(frameLast>=0)) )
                    {
                        // Return to last frame;
                        if(frameLast<0)
                            CurrentFrame=framesCount-1;
                        else
                            CurrentFrame=frameLast;
                    }
                }
                else
                {
                    // Return to last frame;
                    if(frameLast<0)
                        CurrentFrame=framesCount-1;
                    else
                        CurrentFrame=frameLast;
                }
            }
        }
        else
        { // Direct animation
            CurrentFrame++;
            if(((CurrentFrame>=framesCount)&&(frameLast<0))||
                    ((CurrentFrame>frameLast)&&(frameLast>=0)))
            {
                if(bidirectional)
                {
                    reverce=!reverce; // change direction on last frame
                    CurrentFrame-=2;
                    if( CurrentFrame < 0 )
                        CurrentFrame = 0;
                }
                else
                {
                    CurrentFrame=frameFirst; // Return to first frame;
                }
            }
        }
    }
    frame_rect.setRect(0, frameHeight*CurrentFrame, frameWidth, frameHeight);
}

//void SimpleAnimator::createAnimationFrames()
//{
//    for(int i=0; (i < framesCount); i++)
//    {
//        frames.push_back( mainImage.copy(QRect(framePos.x(), frameHeight*i, frameWidth, frameHeight )) );
//    }
//}


void SimpleAnimator::setFrame(int y)
{
    if(frame_sequance_enabled)
    {
        if(y<0) y = 0;
        if(!frame_sequance.isEmpty())
        {
            if(y >= frame_sequance.size()) y = 0;
            frame_sequance_cur = y;
            y = frame_sequance[y];
        }
    }
    //if(y>=frames.size()) y= frameFirst;
    if(y>=framesCount) y = frameFirst;
    //if(y<frameFirst) y = (frameLast<0)? frames.size()-1 : frameLast;
    if(y<frameFirst) y = (frameLast<0)? framesCount-1 : frameLast;
    CurrentFrame = y;
    frame_rect.setRect(0, frameHeight*y, frameWidth, frameHeight);
}

void SimpleAnimator::start()
{
    if(!animated) return;
    if((frameLast>0)&&((frameLast-frameFirst)<=1)) return; //Don't start singleFrame animation
}

void SimpleAnimator::stop()
{
    if(!animated) return;
    resetFrame();
}

void SimpleAnimator::resetFrame()
{
    setFrame(frameFirst);
    ticks_left = 0.0f;
}


