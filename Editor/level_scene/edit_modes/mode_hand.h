#ifndef MODE_HAND_H
#define MODE_HAND_H

#include "../../common_features/edit_mode_base.h"

class ModeHand : public EditMode
{
public:
    ModeHand(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~ModeHand();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);
};

#endif // MODE_HAND_H
