#ifndef LVL_MODE_HAND_H
#define LVL_MODE_HAND_H

#include "../../common_features/edit_mode_base.h"

class LVL_ModeHand : public EditMode
{
public:
    LVL_ModeHand(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~LVL_ModeHand();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);
};

#endif // LVL_MODE_HAND_H
