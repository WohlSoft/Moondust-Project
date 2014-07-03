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

SimpleAnimator::SimpleAnimator(QPixmap &sprite, bool enables, int framesq, int fspeed, int First, int Last)
{
    timer=NULL;
    mainImage = sprite;
    animated=enables;
    frameFirst=First;
    frameLast=Last;
    CurrentFrame = 0;

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
    CurrentFrame++;
    if(CurrentFrame>=frames.size())
        CurrentFrame=frameFirst;

    //mutex.lock();
    /*
    frameCurrent += frameSize;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
    framePos.setY( framePos.y() + frameSize );

    currentFrame = mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
    // mutex.unlock();
    */
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
    //mutex.lock();
    if(y>=frames.size()) y=frameFirst;
    CurrentFrame = y;
    /*
    frameCurrent = frameSize * y;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
    framePos.setY( frameCurrent );

    currentFrame = mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
    //mutex.unlock();
    */
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


