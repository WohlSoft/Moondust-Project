#ifndef MODE_PLACING_H
#define MODE_PLACING_H

#include "../../common_features/edit_mode_base.h"

class ModePlace : public EditMode
{
public:
    ModePlace(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~ModePlace();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);
};

#endif // MODE_PLACING_H
