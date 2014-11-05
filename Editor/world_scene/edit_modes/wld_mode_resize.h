#ifndef WLD_MODE_RESIZE_H
#define WLD_MODE_RESIZE_H

#include "../../common_features/edit_mode_base.h"

class WLD_ModeResize : public EditMode
{
public:
    WLD_ModeResize(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~WLD_ModeResize();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);
};

#endif // WLD_MODE_RESIZE_H
