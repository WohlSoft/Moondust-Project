#ifndef MODE_RESIZE_H
#define MODE_RESIZE_H

#include "../../common_features/edit_mode_base.h"

class ModeResize : public EditMode
{
public:
    ModeResize(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~ModeResize();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);
};

#endif // MODE_RESIZE_H
