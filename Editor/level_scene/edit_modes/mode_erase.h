#ifndef MODE_ERASE_H
#define MODE_ERASE_H

#include "../../common_features/edit_mode_base.h"

class ModeErase : public EditMode
{
public:
    ModeErase(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~ModeErase();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);

};

#endif // MODE_ERASE_H
