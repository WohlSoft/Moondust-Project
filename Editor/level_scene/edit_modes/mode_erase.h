#ifndef LVL_MODE_ERASE_H
#define LVL_MODE_ERASE_H

#include "../../common_features/edit_mode_base.h"

class LVL_ModeErase : public EditMode
{
public:
    LVL_ModeErase(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~LVL_ModeErase();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);

};

#endif // LVL_MODE_ERASE_H
