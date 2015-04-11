#ifndef MATRIXANIMATOR_H
#define MATRIXANIMATOR_H

#include <QList>
#include <QRectF>

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
    QList<MatrixAnimatorFrame > sequence;
};

#endif // MATRIXANIMATOR_H
