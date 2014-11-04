#ifndef WLD_MODE_SETPOINT_H
#define WLD_MODE_SETPOINT_H

#include "../../common_features/edit_mode_base.h"

class WLD_ModeSetPoint : public EditMode
{
public:
    WLD_ModeSetPoint(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~WLD_ModeSetPoint();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);

};


#endif // WLD_MODE_SETPOINT_H
