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

#ifndef MATRIXANIMATOR_H
#define MATRIXANIMATOR_H

#include <QList>
#include <QHash>

#include "pointf.h"
#include "rectf.h"
#include "sizef.h"
#include "size.h"

#include "player_calibration.h"

struct MatrixAnimatorFrame
{
    float x;
    float y;
    float offset_x;
    float offset_y;
};

class MatrixAnimator
{
public:

    enum MatrixAnimates
    {
        Nothing=-1,
        Idle=0,
        Run,
        JumpFloat,
        JumpFall,
        SpinJump,
        Sliding,
        Climbing,
        Fire,
        SitDown,
        Dig,
        GrabIdle,
        GrabRun,
        GrabJump,
        GrabSitDown,
        RacoonRun,
        RacoonFloat,
        RacoonFly,
        RacoonTail,
        Swim,
        SwimUp,
        OnYoshi,
        OnYoshiSit,
        PipeUpDown,
        PipeUpDownRear,
        SlopeSlide,
        TanookiStatue,
        SwordAttak,
        JumpSwordUp,
        JumpSwordDown,
        DownSwordAttak,
        Hurted,
    };

    MatrixAnimator();
    MatrixAnimator(int _width, int _height);
    MatrixAnimator(const MatrixAnimator &a);
    ~MatrixAnimator();
    void setFrameSequance(QList<MatrixAnimatorFrame > _sequence);
    void setFrameSpeed(int speed);
    void setSize(int _width, int _height);
    PGE_SizeF size();
    PGE_SizeF sizeOfFrame();
    void tickAnimation(float frametime);
    PGE_RectF curFrame();
    PGE_PointF curOffset();
    MatrixAnimates curAnimation();
    int curFramespeed();

    void installAnimationSet(obj_player_calibration &calibration);
    void playOnce(MatrixAnimates aniName, int _direction, int speed=-1, bool fixed_speed=false, bool locked=false, int skipLastFrames=0);
    void unlock();
    void switchAnimation(MatrixAnimates aniName, int _direction, int speed=-1);

    MatrixAnimates toEnum(QString aniName);
private:
    void nextFrame();
    void buildRect();

    float width; //!< width of frame matrix
    float height; //!< height of frame matrix
    float width_f; //!< width of one frame; (from 0 to 1)
    float height_f; //!< height of one frame; (from 0 to 1)
    float delay_wait; //!< Delay between next frame will be switched
    int framespeed; //!< delay between frames
    int framespeed_once; //!< delay between frames for "once" mode
    int curFrameI; //!< index of current frame
    PGE_RectF curRect;
    PGE_PointF curOffsets;
    typedef QList<MatrixAnimatorFrame > AniSequence;

    int direction;
    bool once;
    bool once_fixed_speed;
    bool once_locked;
    bool once_play_again;
    int  once_play_again_skip_last_frames;
    MatrixAnimates backup_sequance;
    MatrixAnimates current_sequance;
    AniSequence sequence;//!< Current frame sequance
    QHash<MatrixAnimates, AniSequence > s_bank_left;  //!< Animation sequances bank for left  frames
    QHash<MatrixAnimates, AniSequence > s_bank_right; //!< Animation sequances bank for right frames

    void buildEnums();
    QHash<QString, MatrixAnimates > StrToEnum;
};

#endif // MATRIXANIMATOR_H
