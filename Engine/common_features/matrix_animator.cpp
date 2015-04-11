#include "matrix_animator.h"

MatrixAnimator::MatrixAnimator()
{
    framespeed=128;
    delay_wait=framespeed;
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
    check_frame:
    delay_wait-=abs(frametime);
        if(delay_wait<=0)
        {
            nextFrame();
            delay_wait = framespeed+delay_wait;
            goto check_frame;
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
        curFrameI=0;

    buildRect();
}

void MatrixAnimator::buildRect()
{
    if(sequence.isEmpty())
    {
        curRect = QRectF(0.0, 0.0, 1.0, 1.0);
    }
    else
    {
        curRect.setLeft(sequence[curFrameI].x+sequence[curFrameI].offset_x);
        curRect.setTop(sequence[curFrameI].y+sequence[curFrameI].offset_y);
        curRect.setRight(sequence[curFrameI].x+sequence[curFrameI].offset_x+width_f);
        curRect.setBottom(sequence[curFrameI].x+sequence[curFrameI].offset_x+height_f);
    }
}

QRectF MatrixAnimator::curFrame()
{
    if(sequence.isEmpty())
    {
        return QRectF(0.0, 0.0, 1.0, 1.0);
    }
    return curRect;
}

