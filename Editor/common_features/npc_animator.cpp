/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

AdvNpcAnimator::AdvNpcAnimator(QPixmap &sprite, obj_npc &config)
{

    mainImage = sprite;
    setup = config;

    animated = false;
    aniBiDirect=false;
    curDirect = -1;
    frameStep = 1;
    frameSize=1;

    CurrentFrameL=0; //Real frame
    CurrentFrameR=0; //Real frame
    frameCurrentL=0; //Timer frame
    frameCurrentR=0; //Timer frame

    frameFirstL=0; //from first frame
    frameLastL=-1; //to unlimited frameset
    frameFirstR=0; //from first frame
    frameLastR=-1; //to unlimited frameset

    timer=NULL;

    animated = true;
    framesQ = setup.frames;
    frameSpeed = setup.framespeed;
    frameStyle = setup.framestyle;
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

    //    int dir=direction;

    //    if(direction==0) //if direction=random
    //    {
    //        dir=((0==qrand()%2)?-1:1); //set randomly 1 or -1
    //    }

    if(setup.ani_directed_direct)
    {
        aniDirectL = (true) ^ (setup.ani_direct);
        aniDirectR = (false) ^ (setup.ani_direct);
    }
    else
    {
        aniDirectL = setup.ani_direct;
        aniDirectR = setup.ani_direct;
    }

    if(customAnimate) // User defined spriteSet (example: boss)
    {

        //LEFT
        frameFirstL = custom_frameFL;
        switch(customAniAlg)
        {
        case 2:
            frameSequance = true;
            frames_listL = setup.frames_left;
            frameFirstL = 0;
            frameLastL = frames_listL.size()-1;
            break;
        case 1:
            frameStep = custom_frameEL;
            frameLastL = -1; break;
        case 0:
        default:
            frameLastL = custom_frameEL; break;
        }

        //RIGHT
        frameFirstR = custom_frameFR;
        switch(customAniAlg)
        {
        case 2:
            frameSequance = true;
            frames_listR = setup.frames_right;
            frameFirstR = 0;
            frameLastR = frames_listR.size()-1; break;
        case 1:
            frameStep = custom_frameER;
            frameLastR = -1; break;
        case 0:
        default:
            frameLastR = custom_frameER; break;
        }
    }
    else
    {   //Standard animation
        switch(frameStyle)
        {
        case 2: //Left-Right-upper sprite
            framesQ = setup.frames*4;

             //left
                frameFirstL = 0;
                frameLastL = (int)(framesQ-(framesQ/4)*3)-1;
             //Right
                frameFirstR = (int)(framesQ-(framesQ/4)*3);
                frameLastR = (int)(framesQ/2)-1;

            break;
        case 1: //Left-Right sprite
            framesQ=setup.frames*2;
             //left
                frameFirstL = 0;
                frameLastL = (int)(framesQ / 2)-1;
             //Right
                frameFirstR = (int)(framesQ / 2);
                frameLastR = framesQ-1;

            break;
        case 0: //Single sprite
        default:
            frameFirstL = 0;
            frameLastL = framesQ-1;

            frameFirstR = 0;
            frameLastR = framesQ-1;
            break;
        }
    }

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("NPC-%1, framestyle is %2").arg(setup.id).arg(setup.framestyle));
    #endif

    //curDirect  = dir;
    //setOffset(imgOffsetX+(-((double)localProps.gfx_offset_x)*curDirect), imgOffsetY );


    if(timer) delete timer;
    timer = new QTimer(this);
    connect(
                timer, SIGNAL(timeout()),
                this,
                SLOT( nextFrame() ) );

    timer->setTimerType(Qt::PreciseTimer);

    createAnimationFrames();
    setFrameL(frameFirstL);
    setFrameR(frameFirstR);
}

AdvNpcAnimator::~AdvNpcAnimator()
{
    delete timer;
}

QPixmap AdvNpcAnimator::image(int dir, int frame)
{
    if(frames.isEmpty())
    {   //If animator haven't frames, return red sqare
        QPixmap tmp = QPixmap(QSize(32,32));
        tmp.fill(QColor(Qt::red));
        return tmp;
    }

    if((frame<0)||(frame>=frames.size()))
    {
        if(dir<0)
            return frames[CurrentFrameL];
        else if(dir==0)
            return frames[CurrentFrameL];
        else
            return frames[CurrentFrameR];
    }
    else return frames[frame];
}

QPixmap AdvNpcAnimator::wholeImage()
{
    return mainImage;
}

void AdvNpcAnimator::setFrameL(int y)
{
    if(frames.isEmpty()) return;

    //Out of range protection
    if( y < frameFirstL) y = (frameLastL<0)? frames.size()-1 : frameLastL;
    if( y >= frames.size()) y = (frameFirstL<frames.size()) ? frameFirstL : 0;
    CurrentFrameL = y;
}

void AdvNpcAnimator::setFrameR(int y)
{
    if(frames.isEmpty()) return;

    //Out of range protection
    if( y < frameFirstR) y = (frameLastR<0)? frames.size()-1 : frameLastR;
    if( y >= frames.size()) y = (frameFirstR<frames.size()) ? frameFirstR : 0;

    CurrentFrameR = y;
}

void AdvNpcAnimator::start()
{
    if(!animated) return;
    if((frameLastL>0)&&((frameLastL-frameFirstL)<=0)) return; //Don't start singleFrame animation
    if((frameLastR>0)&&((frameLastR-frameFirstR)<=0)) return; //Don't start singleFrame animation

    frameCurrentL = frameFirstL;
    frameCurrentR = frameFirstR;
    timer->start(frameSpeed);
}

void AdvNpcAnimator::stop()
{
    if(!animated) return;
    timer->stop();
    setFrameL(frameFirstL);
    setFrameR(frameFirstR);
}

void AdvNpcAnimator::nextFrame()
{
    //Left
    if(!aniDirectL)
    {
        //frameCurrent += frameSize * frameStep;
        frameCurrentL += frameStep;

        if ( ((frameCurrentL >= frames.size()-(frameStep-1) )&&(frameLastL<=-1)) ||
             ((frameCurrentL > frameLastL )&&(frameLastL>=0)) )
            {
                if(!aniBiDirect)
                {
                     frameCurrentL = frameFirstL;
                }
                else
                {
                    frameCurrentL -= frameStep*2;
                    aniDirectL=!aniDirectL;
                }
            }
    }
    else
    {
        //frameCurrent -= frameSize * frameStep;
        frameCurrentL -= frameStep;

        if ( frameCurrentL < frameFirstL )
            {
                if(!aniBiDirect)
                {
                    frameCurrentL = ((frameLastL==-1)? frames.size()-1 : frameLastL);
                }
                else
                {
                    frameCurrentL+=frameStep*2;
                    aniDirectL=!aniDirectL;
                }
            }
    }
    setFrameL( frameSequance ? frames_listL[frameCurrentL] : frameCurrentL);

    //Right
    if(!aniDirectR)
    {
        //frameCurrent += frameSize * frameStep;
        frameCurrentR += frameStep;

        if ( ((frameCurrentR >= frames.size()-(frameStep-1) )&&(frameLastR<=-1)) ||
             ((frameCurrentR > frameLastR )&&(frameLastR>=0)) )
            {
                if(!aniBiDirect)
                {
                     frameCurrentR = frameFirstR;
                }
                else
                {
                    frameCurrentR -= frameStep*2;
                    aniDirectR=!aniDirectR;
                }
            }
    }
    else
    {
        //frameCurrent -= frameSize * frameStep;
        frameCurrentR -= frameStep;

        if ( frameCurrentR < frameFirstR )
            {
                if(!aniBiDirect)
                {
                    frameCurrentR = ((frameLastR==-1)? frames.size()-1 : frameLastR);
                }
                else
                {
                    frameCurrentR+=frameStep*2;
                    aniDirectR=!aniDirectR;
                }
            }
    }
    setFrameR( frameSequance ? frames_listR[frameCurrentR] : frameCurrentR);
}

void AdvNpcAnimator::createAnimationFrames()
{
    frames.clear();
    for(int i=0; (frameSize*i < frameHeight); i++)
    {
        frames.push_back( mainImage.copy(QRect(0, frameSize*i, frameWidth, frameSize )) );
    }
}
