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

#ifndef SIMPLE_ANIMATOR_H
#define SIMPLE_ANIMATOR_H

#include <QObject>
#include <QTimer>
#include <QPixmap>

class SimpleAnimator : public QObject
{
    Q_OBJECT
public:
    SimpleAnimator(QPixmap &sprite, bool enables=false, int framesq=1, int fspeed=64);
    QPixmap image();
    QPixmap wholeImage();

    void setFrame(int y);

    void start();
    void stop();

private slots:
    void nextFrame();

private:
    QPixmap mainImage; //Whole image

    bool animated;

    int speed;

    int frameCurrent;
    QTimer * timer;
    QPoint framePos;
    int framesQ;
    int frameSize; // size of one frame
    int frameWidth; // sprite width
    int frameHeight; //sprite height

    //Animation alhorithm
    int frameFirst;
    int frameLast;

};

#endif // SIMPLE_ANIMATOR_H
