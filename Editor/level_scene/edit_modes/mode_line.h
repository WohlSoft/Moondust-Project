#ifndef LVL_MODE_LINE_H
#define LVL_MODE_LINE_H

#include "../../common_features/edit_mode_base.h"

class LVL_ModeLine : public EditMode
{
public:
    LVL_ModeLine(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~LVL_ModeLine();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);

private:
    QPointF drawStartPos;

};

#endif // LVL_MODE_SQUARE_H
