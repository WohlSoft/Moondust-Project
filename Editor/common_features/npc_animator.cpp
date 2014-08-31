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

#include "npc_animator.h"

#include "logger.h"

AdvNpcAnimator::AdvNpcAnimator(QPixmap &sprite, obj_npc &config)
{

    mainImage = sprite;
    setup = config;

    timer=NULL;

    animated = true;
    framesQ = setup.frames;
    frameSpeed = setup.framespeed;
    frameStyle = setup.framestyle;
    direction = -1;
    frameStep = 1;

    frameSequance = false;

    aniBiDirect = setup.ani_bidir;
    customAniAlg = setup.custom_ani_alg;

    customAnimate = setup.custom_animate;

    custom_frameFL = setup.custom_ani_fl;//first left
    custom_frameEL = setup.custom_ani_el;//end left
    custom_frameFR = setup.custom_ani_fr;//first right
    custom_frameER = setup.custom_ani_er;//enf right

    //bool refreshFrames = updFrames;

    frameSize = setup.gfx_h; // height of one frame
    frameWidth = setup.gfx_w; //width of target image

    frameHeight = mainImage.height(); // Height of target image

    //Protectors
    if(frameSize<=0) frameSize=1;
    if(frameSize>mainImage.height()) frameSize = mainImage.height();

    if(frameWidth<=0) frameWidth=1;
    if(frameWidth>mainImage.width()) frameWidth = mainImage.width();

    int dir=direction;

    if(direction==0) //if direction=random
    {
        dir=((0==qrand()%2)?-1:1); //set randomly 1 or -1
    }

    if(setup.ani_directed_direct)
        aniDirect = (dir==-1) ^ (setup.ani_direct);
    else
        aniDirect = setup.ani_direct;

    if(customAnimate) // User defined spriteSet (example: boss)
    {
        switch(dir)
        {
        case -1: //left
            frameFirst = custom_frameFL;
            switch(customAniAlg)
            {
            case 2:
                frameSequance = true;
                frames_list = setup.frames_left;
                frameFirst = 0;
                frameLast = frames_list.size()-1;
                break;
            case 1:
                frameStep = custom_frameEL;
                frameLast = -1; break;
            case 0:
            default:
                frameLast = custom_frameEL; break;
            }
            break;
        case 1: //Right
            frameFirst = custom_frameFR;
            switch(customAniAlg)
            {
            case 2:
                frameSequance = true;
                frames_list = setup.frames_right;
                frameFirst = 0;
                frameLast = frames_list.size()-1; break;
            case 1:
                frameStep = custom_frameER;
                frameLast = -1; break;
            case 0:
            default:
                frameLast = custom_frameER; break;
            }
            break;
        default: break;
        }
    }
    else
    {
        switch(frameStyle)
        {
        case 2: //Left-Right-upper sprite
            framesQ = setup.frames*4;
            switch(dir)
            {
            case -1: //left
                frameFirst = 0;
                frameLast = (int)(framesQ-(framesQ/4)*3)-1;
                break;
            case 1: //Right
                frameFirst = (int)(framesQ-(framesQ/4)*3);
                frameLast = (int)(framesQ/2)-1;
                break;
            default: break;
            }
            break;

        case 1: //Left-Right sprite
            framesQ=setup.frames*2;
            switch(dir)
            {
            case -1: //left
                frameFirst = 0;
                frameLast = (int)(framesQ / 2)-1;
                break;
            case 1: //Right
                frameFirst = (int)(framesQ / 2);
                frameLast = framesQ-1;
                break;
            default: break;
            }

            break;

        case 0: //Single sprite
        default:
            frameFirst = 0;
            frameLast = framesQ-1;
            break;
        }
    }

    curDirect  = dir;
    //setOffset(imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );


    if(timer) delete timer;
    timer = new QTimer(this);
    connect(
                timer, SIGNAL(timeout()),
                this,
                SLOT( nextFrame() ) );

    createAnimationFrames();
    setFrame(frameFirst);
}

AdvNpcAnimator::~AdvNpcAnimator()
{
    delete timer;
}

QPixmap AdvNpcAnimator::image(int dir, int frame)
{
    return frames[CurrentFrame];
}

QPixmap AdvNpcAnimator::wholeImage()
{
    return mainImage;
}

void AdvNpcAnimator::setFrame(int y)
{
    if(frames.isEmpty()) return;
    //frameCurrent = frameSize * y;
    CurrentFrame = y;
    //Out of range protection
    if( CurrentFrame >= frames.size()) CurrentFrame = (frameFirst<frames.size()) ? frameFirst : 0;
    if( CurrentFrame < frameFirst) CurrentFrame = (frameLast<0)? frames.size()-1 : frameLast;
}

void AdvNpcAnimator::setframeL(int y)
{

}

void AdvNpcAnimator::setframeR(int y)
{

}

void AdvNpcAnimator::start()
{
    if(!animated) return;
    if((frameLast>0)&&((frameLast-frameFirst)<=0)) return; //Don't start singleFrame animation

    frameCurrent = frameFirst;
    timer->start(frameSpeed);
}

void AdvNpcAnimator::stop()
{
    if(!animated) return;
    timer->stop();
    setFrame(frameFirst);
}

void AdvNpcAnimator::nextFrame()
{
    if(!aniDirect)
    {
        //frameCurrent += frameSize * frameStep;
        frameCurrent += frameStep;

        if ( ((frameCurrent >= frames.size()-(frameStep-1) )&&(frameLast<=-1)) ||
             ((frameCurrent > frameLast )&&(frameLast>=0)) )
            {
                if(!aniBiDirect)
                {
                     frameCurrent = frameFirst;
                    //frameCurrent = frameFirst * frameSize;
                    //framePos.setY( frameFirst * frameSize );
                }
                else
                {
                    frameCurrent -= frameStep*2;
                    aniDirect=!aniDirect;
                    //framePos.setY( framePos.y() - frameSize*frameStep );
                }
            }
    }
    else
    {
        //frameCurrent -= frameSize * frameStep;
        frameCurrent -= frameStep;

        if ( frameCurrent < frameFirst )
            {
                if(!aniBiDirect)
                {
                    frameCurrent = ((frameLast==-1)? frames.size()-1 : frameLast);
                    //frameCurrent = ( ((frameLast==-1)? frameHeight : frameLast*frameSize)-frameSize);
                    //framePos.setY( ((frameLast==-1) ? frameHeight : frameLast*frameSize)-frameSize );
                }
                else
                {
                    frameCurrent+=frameStep*2;
                    aniDirect=!aniDirect;
                    //frameCurrent += frameSize*frameStep*2;
                    //framePos.setY( framePos.y() + frameSize*frameStep );
                }
            }
    }
    setFrame( frameSequance ? frames_list[frameCurrent] : frameCurrent);
}

void AdvNpcAnimator::createAnimationFrames()
{
    frames.clear();
    for(int i=0; (frameSize*i < frameHeight); i++)
    {
        frames.push_back( mainImage.copy(QRect(0, frameSize*i, frameWidth, frameSize )) );
    }
}
