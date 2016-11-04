/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
    double x;
    double y;
    double offset_x;
    double offset_y;
};

class MatrixAnimator
{
    public:

        enum MatrixAnimates
        {
            Nothing = -1,
            Idle = 0,
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
            RacoonFlyDown,
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
        MatrixAnimator(int width, int height);
        MatrixAnimator(const MatrixAnimator &a);
        ~MatrixAnimator();
        void setFrameSequance(QList<MatrixAnimatorFrame > _sequence);
        void setFrameSpeed(int speed);
        void setDirection(int _direction, bool force = false);
        void setSize(int _width, int _height);
        PGE_SizeF size();
        PGE_SizeF sizeOfFrame();
        void tickAnimation(double frametime);
        PGE_RectF curFrame();
        PGE_PointF curOffset();
        MatrixAnimates curAnimation();
        int curFramespeed();

        void installAnimationSet(obj_player_calibration &calibration);
        void playOnce(MatrixAnimates aniName, int _direction, int speed = -1, bool fixed_speed = false, bool locked = false, int skipLastFrames = 0);
        void unlock();
        void switchAnimation(MatrixAnimates aniName, int _direction, int speed = -1);

        MatrixAnimates toEnum(QString aniName);
    private:
        void nextFrame();
        void buildRect();

        //! width of frame matrix
        double m_width;
        //! height of frame matrix
        double m_height;
        //! width of one frame; (from 0 to 1)
        double m_width_f;
        //! height of one frame; (from 0 to 1)
        double m_height_f;
        //! Delay between next frame will be switched
        double m_nextFrameDelay;
        //! delay between frames
        int m_frameDelay;
        //! delay between frames for "once" mode
        int m_frameDelay_once;
        //! index of current frame
        int m_currentFrameIndex;

        //! Current frame rectangle
        PGE_RectF   m_currentFrameRect;
        //! Current frame offsets
        PGE_PointF  m_currentFrameOffsets;

        //! Face direction (<1 left, >1 - right)
        int  m_direction;
        //! Once mode (animation sequence will be played once and stopped)
        bool m_once;
        //! Once mode Lock current frame delay
        bool m_once_fixed_speed;
        //! Once mode: delay can't be toggled until animation finished
        bool m_once_locked;
        //! Once mode: requested a repeat of animation
        bool m_once_play_again;
        //! Once mode: skip last frames count on repeating
        int  m_once_play_again_skip_last_frames;
        //! Once mode: repeat animation with direction
        int  m_once_play_again_direction;
        //! Currently processing animation sequence
        MatrixAnimates m_current_sequance;
        //! Remembered animation sequence which will be restored on finishing once mode animation
        MatrixAnimates m_backup_sequance;

        //! Animation sequence list type
        typedef QList<MatrixAnimatorFrame > AniSequence;
        //! Custom frame sequance
        AniSequence  m_sequence;
        //! Pointer to current sequence
        AniSequence *m_sequenceP;
        //! Animation sequances bank for left  frames
        QHash<MatrixAnimates, AniSequence > s_bank_left;
        //! Animation sequances bank for right frames
        QHash<MatrixAnimates, AniSequence > s_bank_right;

        void buildEnums();
        QHash<QString, MatrixAnimates > StrToEnum;
};

#endif // MATRIXANIMATOR_H
