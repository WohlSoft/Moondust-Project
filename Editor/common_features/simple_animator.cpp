/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

SimpleAnimator::SimpleAnimator(QPixmap &sprite, bool enables, int framesq, int fspeed, int First, int Last, bool rev, bool bid)
{
    timer=NULL;
    mainImage = sprite;
    animated = enables;
    frameFirst = First;
    frameLast = Last;
    CurrentFrame = 0;

    bidirectional = bid;
    reverce = rev;

    speed=fspeed;
    framesQ = framesq;

    frameWidth = mainImage.width();
    frameHeight = mainImage.height();

    if(animated)
        frameSize = qRound(qreal(frameHeight/framesQ));
    else
        frameSize = frameHeight;
    framePos = QPoint(0,0);

    createAnimationFrames();

    setFrame(frameFirst);

    timer = new QTimer(this);
    connect(
                timer, SIGNAL(timeout()),
                this,
                SLOT( nextFrame() ) );
}

SimpleAnimator::~SimpleAnimator()
{
    delete timer;
}

//Returns images

QPixmap SimpleAnimator::image(int frame)
{
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

    timer->start(speed);
}

void SimpleAnimator::stop()
{
    if(!animated) return;
    timer->stop();
    setFrame(frameFirst);
}


