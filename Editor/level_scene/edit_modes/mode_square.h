#ifndef MODE_SQUARE_H
#define MODE_SQUARE_H

#include "../../common_features/edit_mode_base.h"

class ModeSquare : public EditMode
{
public:
    ModeSquare(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~ModeSquare();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);

private:
    QPointF drawStartPos;

};

#endif // MODE_SQUARE_H
