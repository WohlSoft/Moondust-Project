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

SimpleAnimator::SimpleAnimator(QPixmap &sprite, bool enables, int framesq, int fspeed)
{
    timer=NULL;
    mainImage = sprite;
    animated=enables;

    speed=fspeed;
    framesQ = framesq;

    frameWidth = mainImage.width();
    frameHeight = mainImage.height();
    if(animated)
        frameSize = qRound(qreal(frameHeight)/qreal(framesQ));
    else
        frameSize = frameWidth;
    framePos = QPoint(0,0);

    setFrame(frameFirst);

    timer = new QTimer(this);
    connect(
                timer, SIGNAL(timeout()),
                this,
                SLOT( nextFrame() ) );
}

//Returns images

QPixmap SimpleAnimator::image()
{
    QMutexLocker locker(&mutex); //Glitch protection
    return mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
}

QPixmap SimpleAnimator::wholeImage()
{
    return mainImage;
}

//Animation process
void SimpleAnimator::nextFrame()
{
    mutex.lock();

    frameCurrent += frameSize;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
    framePos.setY( framePos.y() + frameSize );

    mutex.unlock();
}


void SimpleAnimator::setFrame(int y)
{
    mutex.lock();

    frameCurrent = frameSize * y;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
    framePos.setY( frameCurrent );

    mutex.unlock();
}

void SimpleAnimator::start()
{
    if(!animated) return;
    timer->start(speed);
}

void SimpleAnimator::stop()
{
    if(!animated) return;
    timer->stop();
    setFrame(frameFirst);
}


