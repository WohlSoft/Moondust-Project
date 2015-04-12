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
#include <QRectF>

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
    MatrixAnimator();
    MatrixAnimator(int _width, int _height);
    MatrixAnimator(const MatrixAnimator &a);
    ~MatrixAnimator();
    void setFrameSequance(QList<MatrixAnimatorFrame > _sequence);
    void setFrameSpeed(int speed);
    void setSize(int _width, int _height);
    void tickAnimation(int frametime);
    QRectF curFrame();

    void installAnimationSet(obj_player_calibration &calibration);
    void playOnce(QString aniName, int direction, int speed=-1);
    void switchAnimation(QString aniName, int direction, int speed=-1);

private:
    void nextFrame();
    void buildRect();

    float width; //!< width of frame matrix
    float height; //!< height of frame matrix
    float width_f; //!< width of one frame;
    float height_f; //!< height of one frame;
    int delay_wait; //!< Delay between next frame will be switched
    int framespeed; //!< delay between frames
    int curFrameI; //!< index of current frame
    QRectF curRect;
    typedef QList<MatrixAnimatorFrame > AniSequence;

    int direction;
    bool once;
    QString backup_sequance;
    AniSequence sequence;//!< Current frame sequance
    QHash<QString, AniSequence > s_bank_left;  //!< Animation sequances bank for left  frames
    QHash<QString, AniSequence > s_bank_right; //!< Animation sequances bank for right frames
};

#endif // MATRIXANIMATOR_H
