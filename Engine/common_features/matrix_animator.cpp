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

#ifdef __APPLE__
#include <tgmath.h>
#endif

MatrixAnimator::MatrixAnimator()
{
    framespeed=128;
    delay_wait=framespeed;
    direction=1;
    current_sequance=Idle;
    backup_sequance=Idle;
    curFrameI=0;
    once=false;
    once_fixed_speed=false;
    once_locked=false;
    once_play_again=false;
    once_play_again_skip_last_frames=0;
    once_play_again_direction=0;
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
    once_fixed_speed=false;
    once_locked=false;
    once_play_again=false;
    once_play_again_skip_last_frames=0;
    once_play_again_direction=0;
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
    once_fixed_speed=a.once_fixed_speed;
    once_locked=a.once_locked;
    once_play_again=a.once_play_again;
    once_play_again_skip_last_frames=a.once_play_again_skip_last_frames;
    once_play_again_direction=a.once_play_again_direction;
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
    if(once && once_fixed_speed ) return;
    if(framespeed==speed) return;
    if(speed<=0)
    {
        framespeed=-1;//Stop animation if <=0
        return;
    }
    delay_wait = ((framespeed-speed)<1) ? delay_wait : delay_wait-(framespeed-speed);
    framespeed=abs(speed);
}

void MatrixAnimator::setDirection(int _direction, bool force)
{
    if(force || (direction!=_direction))
        direction=_direction;
    if(direction<0)
    {//left
        if(!s_bank_left.contains(current_sequance)) return;
        sequence = s_bank_left[current_sequance];
    }
    else
    {//right
        if(!s_bank_right.contains(current_sequance)) return;
        sequence = s_bank_right[current_sequance];
    }
}

void MatrixAnimator::setSize(int _width, int _height)
{
    width=abs(_width);
    height=abs(_height);
    width_f = 1.0f/width;
    height_f = 1.0f/height;
}

PGE_SizeF MatrixAnimator::size()
{
    return PGE_SizeF(width, height);
}

PGE_SizeF MatrixAnimator::sizeOfFrame()
{
    return PGE_SizeF(width_f, height_f);
}

void MatrixAnimator::tickAnimation(float frametime)
{
    if((!once) && (framespeed<1)) return; //Idling animation

    delay_wait-=fabs(frametime);
        while(delay_wait<=0.0f)
        {
            nextFrame();
            delay_wait+= once?framespeed_once : framespeed;
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
    if(curFrameI>(sequence.size()-1-once_play_again_skip_last_frames))
    {
        curFrameI=0;
        if(once)
        {
            if(once_play_again)
            {
                once_play_again=false;
                once_play_again_skip_last_frames=0;
                setDirection(once_play_again_direction);
            }
            else
            {
                once=false;
                once_play_again_skip_last_frames=0;
                switchAnimation(backup_sequance, direction, framespeed);
            }
        }
    }
    buildRect();
}

void MatrixAnimator::buildRect()
{
    if(sequence.isEmpty())
    {
        curRect.setRect(0.0, 0.0, width_f, height_f);
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

PGE_RectF MatrixAnimator::curFrame()
{
    return curRect;
}

PGE_PointF MatrixAnimator::curOffset()
{
    return curOffsets;
}

MatrixAnimator::MatrixAnimates MatrixAnimator::curAnimation()
{
    return current_sequance;
}

int MatrixAnimator::curFramespeed()
{
    return framespeed;
}

void MatrixAnimator::installAnimationSet(obj_player_calibration &calibration)
{
    s_bank_left.clear();
    s_bank_right.clear();

    for(int i=0; i<calibration.AniFrames.set.size(); i++)
    {
        MatrixAnimator::MatrixAnimates seq = toEnum(calibration.AniFrames.set[i].name);
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

    if(!s_bank_left.contains(current_sequance))
        current_sequance=Idle;
    if(!s_bank_right.contains(current_sequance))
        current_sequance=Idle;

    /*Update sequence settings*/
    if(direction<0)
    {//left
        sequence = s_bank_left[current_sequance];
    }
    else
    {//right
        sequence = s_bank_right[current_sequance];
    }

    if(curFrameI>(sequence.size()-1))
    {
        curFrameI=0;
        if(once)
        {
            once=false;
            once_fixed_speed=false;
            once_locked=false;
            once_play_again_skip_last_frames=false;
            switchAnimation(backup_sequance, direction, framespeed);
        }
    }
    buildRect();
}

void MatrixAnimator::playOnce(MatrixAnimates aniName, int _direction, int speed, bool fixed_speed, bool locked, int skipLastFrames)
{
    if(once)
    {
        if(once_locked || (current_sequance==aniName))
        {
            if(current_sequance==aniName)
            {
                once_play_again=true;
                once_play_again_direction=_direction;
                once_play_again_skip_last_frames=(skipLastFrames>=0) ? skipLastFrames : 0;
            }
            return;
        }
    }

    once_fixed_speed = fixed_speed;
    once_locked=locked;
    once_play_again=false;
    once_play_again_skip_last_frames=0;
    once=true;
    framespeed_once = (speed>0) ? speed : 0;
    curFrameI = 0;
    backup_sequance = current_sequance;
    current_sequance = aniName;
    setDirection(_direction, true);
    buildRect();
}

void MatrixAnimator::unlock()
{
    once_locked=false;
}

void MatrixAnimator::switchAnimation(MatrixAnimates aniName, int _direction, int speed)
{
    if(once)
    {
        if(once_locked || (backup_sequance==aniName))
        {
            if(!once_fixed_speed)
                framespeed_once = (speed>0) ? speed : 1;
            setFrameSpeed(speed);
            backup_sequance=aniName;
            return;
        } else backup_sequance=aniName;

        if(direction!=_direction)
        {
            setDirection(_direction);
            if(curFrameI>(sequence.size()-1))
                curFrameI=0;
        }
    }
    else
    if((current_sequance==aniName)&&(direction==_direction))
    {
        setFrameSpeed(speed);
        return;
    }

    current_sequance = aniName;
    setFrameSpeed(speed);
    setDirection(_direction);
    if((current_sequance!=aniName) || (curFrameI>(sequence.size()-1)))
        curFrameI = 0;
    once=false;
    buildRect();
}


void MatrixAnimator::buildEnums()
{
    StrToEnum["Idle"]=              MatrixAnimates::Idle;
    StrToEnum["Run"]=               MatrixAnimates::Run;
    StrToEnum["JumpFloat"]=         MatrixAnimates::JumpFloat;
    StrToEnum["JumpFall"]=          MatrixAnimates::JumpFall;
    StrToEnum["SpinJump"]=          MatrixAnimates::SpinJump;
    StrToEnum["Sliding"]=           MatrixAnimates::Sliding;
    StrToEnum["Climbing"]=          MatrixAnimates::Climbing;
    StrToEnum["Fire"]=              MatrixAnimates::Fire;
    StrToEnum["SitDown"]=           MatrixAnimates::SitDown;
    StrToEnum["Dig"]=               MatrixAnimates::Dig;
    StrToEnum["GrabIdle"]=          MatrixAnimates::GrabIdle;
    StrToEnum["GrabRun"]=           MatrixAnimates::GrabRun;
    StrToEnum["GrabJump"]=          MatrixAnimates::GrabJump;
    StrToEnum["GrabSitDown"]=       MatrixAnimates::GrabSitDown;
    StrToEnum["RacoonRun"]=         MatrixAnimates::RacoonRun;
    StrToEnum["RacoonFloat"]=       MatrixAnimates::RacoonFloat;
    StrToEnum["RacoonFly"]=         MatrixAnimates::RacoonFly;
    StrToEnum["RacoonFlyDown"]=     MatrixAnimates::RacoonFlyDown;
    StrToEnum["RacoonTail"]=        MatrixAnimates::RacoonTail;
    StrToEnum["Swim"]=              MatrixAnimates::Swim;
    StrToEnum["SwimUp"]=            MatrixAnimates::SwimUp;
    StrToEnum["OnYoshi"]=           MatrixAnimates::OnYoshi;
    StrToEnum["OnYoshiSit"]=        MatrixAnimates::OnYoshiSit;
    StrToEnum["PipeUpDown"]=        MatrixAnimates::PipeUpDown;
    StrToEnum["PipeUpDownRear"]=    MatrixAnimates::PipeUpDownRear;
    StrToEnum["SlopeSlide"]=        MatrixAnimates::SlopeSlide;
    StrToEnum["TanookiStatue"]=     MatrixAnimates::TanookiStatue;
    StrToEnum["SwordAttak"]=        MatrixAnimates::SwordAttak;
    StrToEnum["JumpSwordUp"]=       MatrixAnimates::JumpSwordUp;
    StrToEnum["JumpSwordDown"]=     MatrixAnimates::JumpSwordDown;
    StrToEnum["DownSwordAttak"]=    MatrixAnimates::DownSwordAttak;
    StrToEnum["Hurted"]=            MatrixAnimates::Hurted;
}

MatrixAnimator::MatrixAnimates MatrixAnimator::toEnum(QString aniName)
{
    if(StrToEnum.isEmpty())
        buildEnums();
    if(StrToEnum.contains(aniName))
        return StrToEnum[aniName];
    else
        return MatrixAnimates::Nothing;
}
