/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "matrix_animator.h"

MatrixAnimator::MatrixAnimator()
{
    framespeed=128;
    delay_wait=framespeed;
    direction=1;
    once=false;
    buildRect();
}

MatrixAnimator::MatrixAnimator(int _width, int _height)
{
    width=abs(_width);
    height=abs(_height);
    width_f = 1.0f/width;
    height_f = 1.0f/height;
    framespeed=128;
    delay_wait=framespeed;
    direction=1;
    once=false;
    buildRect();
}

MatrixAnimator::MatrixAnimator(const MatrixAnimator &a)
{
    width=a.width;
    height=a.height;
    width_f=a.width_f;
    height=a.height_f;
    framespeed=a.framespeed;
    delay_wait=a.delay_wait;
    curFrameI=a.curFrameI;
    sequence=a.sequence;
    direction=a.direction;
    once=a.once;
    backup_sequance=a.backup_sequance;
    s_bank_left = a.s_bank_left;
    s_bank_right = a.s_bank_right;
    buildRect();
}

MatrixAnimator::~MatrixAnimator()
{}

void MatrixAnimator::setFrameSequance(QList<MatrixAnimatorFrame> _sequence)
{
    curFrameI=0;
    sequence.clear();
    sequence = _sequence;
    buildRect();
}

void MatrixAnimator::setFrameSpeed(int speed)
{
    if(framespeed==speed) return;
    if(speed<=0)
    {
        framespeed=-1;//Stop animation if <=0
        return;
    }
    delay_wait = ((framespeed-speed)<1) ? delay_wait : delay_wait-(framespeed-speed);
    framespeed=abs(speed);
}

void MatrixAnimator::setSize(int _width, int _height)
{
    width=abs(_width);
    height=abs(_height);
    width_f = 1.0f/width;
    height_f = 1.0f/height;
}

void MatrixAnimator::tickAnimation(int frametime)
{
    if(framespeed<1) return; //Idling animation

    delay_wait-=abs(frametime);
        while(delay_wait<=0)
        {
            nextFrame();
            delay_wait+=framespeed;
        }
}

void MatrixAnimator::nextFrame()
{
    if(sequence.isEmpty())
    {
        buildRect();
        return;
    }
    curFrameI++;
    if(curFrameI>(sequence.size()-1))
    {
        curFrameI=0;
        if(once)
        {
            once=false;
            switchAnimation(backup_sequance, direction, framespeed);
        }
    }
    buildRect();
}

void MatrixAnimator::buildRect()
{
    if(sequence.isEmpty())
    {
        curRect = QRectF(0.0, 0.0, width_f, height_f);
    }
    else
    {
        curRect.setLeft(sequence[curFrameI].x);
        curRect.setTop(sequence[curFrameI].y);
        curRect.setRight(sequence[curFrameI].x+width_f);
        curRect.setBottom(sequence[curFrameI].y+height_f);

        curOffsets.setX(sequence[curFrameI].offset_x);
        curOffsets.setY(sequence[curFrameI].offset_y);
    }
}

QRectF MatrixAnimator::curFrame()
{
    return curRect;
}

QPointF MatrixAnimator::curOffset()
{
    return curOffsets;
}

MatrixAnimator::MatrixAninates MatrixAnimator::curAnimation()
{
    return current_sequance;
}

void MatrixAnimator::installAnimationSet(obj_player_calibration &calibration)
{
    s_bank_left.clear();
    s_bank_right.clear();

    for(int i=0; i<calibration.AniFrames.set.size(); i++)
    {
        MatrixAnimator::MatrixAninates seq = toEnum(calibration.AniFrames.set[i].name);
        for(int j=0; j<calibration.AniFrames.set[i].L.size();j++)
        {
            float x = calibration.AniFrames.set[i].L[j].x;
            float y = calibration.AniFrames.set[i].L[j].y;

            if(x>(calibration.framesX.size()-1)) continue;
            if(y>(calibration.framesX[x].size()-1)) continue;

            MatrixAnimatorFrame frame;
            frame.x = x/width;
            frame.y = y/width;
            frame.offset_x = calibration.framesX[x][y].offsetX;
            frame.offset_y = calibration.framesX[x][y].offsetY;
            s_bank_left[seq].push_back(frame);
        }
        for(int j=0; j<calibration.AniFrames.set[i].R.size();j++)
        {
            float x = calibration.AniFrames.set[i].R[j].x;
            float y = calibration.AniFrames.set[i].R[j].y;

            if(x>(calibration.framesX.size()-1)) continue;
            if(y>(calibration.framesX[x].size()-1)) continue;

            MatrixAnimatorFrame frame;
            frame.x = x/width;
            frame.y = y/width;
            frame.offset_x = calibration.framesX[x][y].offsetX;
            frame.offset_y = calibration.framesX[x][y].offsetY;
            s_bank_right[seq].push_back(frame);
        }
    }
}

void MatrixAnimator::playOnce(MatrixAninates aniName, int _direction, int speed)
{
    if(once)
    {
        if(current_sequance==aniName)
            return;
    }

    if(_direction<0)
    {//left
        if(!s_bank_left.contains(aniName)) return;
        sequence = s_bank_left[aniName];
    }
    else
    {//right
        if(!s_bank_right.contains(aniName)) return;
        sequence = s_bank_right[aniName];
    }
    once=true;
    framespeed_once = speed;
    direction = _direction;
    curFrameI = 0;
    backup_sequance = current_sequance;
    current_sequance = aniName;
    buildRect();
}

void MatrixAnimator::switchAnimation(MatrixAninates aniName, int _direction, int speed)
{
    if(once)
    {
        if(backup_sequance==aniName)
        {
            setFrameSpeed(speed);
            return;
        }
    }
    else
    if((current_sequance==aniName)&&(direction==_direction))
    {
        setFrameSpeed(speed);
        return;
    }

    if(_direction<0)
    {//left
        if(!s_bank_left.contains(aniName)) return;
        sequence = s_bank_left[aniName];
    }
    else
    {//right
        if(!s_bank_right.contains(aniName)) return;
        sequence = s_bank_right[aniName];
    }
    setFrameSpeed(speed);
    direction = _direction;
    curFrameI = 0;
    current_sequance = aniName;
    once=false;
    buildRect();
}


void MatrixAnimator::buildEnums()
{
    StrToEnum["Idle"]=              MatrixAninates::Idle;
    StrToEnum["Run"]=               MatrixAninates::Run;
    StrToEnum["JumpFloat"]=         MatrixAninates::JumpFloat;
    StrToEnum["JumpFall"]=          MatrixAninates::JumpFall;
    StrToEnum["SpinJump"]=          MatrixAninates::SpinJump;
    StrToEnum["Sliding"]=           MatrixAninates::Sliding;
    StrToEnum["Climbing"]=          MatrixAninates::Climbing;
    StrToEnum["Fire"]=              MatrixAninates::Fire;
    StrToEnum["SitDown"]=           MatrixAninates::SitDown;
    StrToEnum["Dig"]=               MatrixAninates::Dig;
    StrToEnum["GrabIdle"]=          MatrixAninates::GrabIdle;
    StrToEnum["GrabRun"]=           MatrixAninates::GrabRun;
    StrToEnum["GrabJump"]=          MatrixAninates::GrabJump;
    StrToEnum["GrabSitDown"]=       MatrixAninates::GrabSitDown;
    StrToEnum["RacoonRun"]=         MatrixAninates::RacoonRun;
    StrToEnum["RacoonFloat"]=       MatrixAninates::RacoonFloat;
    StrToEnum["RacoonFly"]=         MatrixAninates::RacoonFly;
    StrToEnum["RacoonTail"]=        MatrixAninates::RacoonTail;
    StrToEnum["Swim"]=              MatrixAninates::Swim;
    StrToEnum["SwimUp"]=            MatrixAninates::SwimUp;
    StrToEnum["OnYoshi"]=           MatrixAninates::OnYoshi;
    StrToEnum["OnYoshiSit"]=        MatrixAninates::OnYoshiSit;
    StrToEnum["PipeUpDown"]=        MatrixAninates::PipeUpDown;
    StrToEnum["PipeUpDownRear"]=    MatrixAninates::PipeUpDownRear;
    StrToEnum["SlopeSlide"]=        MatrixAninates::SlopeSlide;
    StrToEnum["TanookiStatue"]=     MatrixAninates::TanookiStatue;
    StrToEnum["SwordAttak"]=        MatrixAninates::SwordAttak;
    StrToEnum["JumpSwordUp"]=       MatrixAninates::JumpSwordUp;
    StrToEnum["JumpSwordDown"]=     MatrixAninates::JumpSwordDown;
    StrToEnum["DownSwordAttak"]=    MatrixAninates::DownSwordAttak;
    StrToEnum["Hurted"]=            MatrixAninates::Hurted;
}

MatrixAnimator::MatrixAninates MatrixAnimator::toEnum(QString aniName)
{
    if(StrToEnum.isEmpty())
        buildEnums();
    if(StrToEnum.contains(aniName))
        return StrToEnum[aniName];
    else
        return MatrixAninates::Nothing;
}
