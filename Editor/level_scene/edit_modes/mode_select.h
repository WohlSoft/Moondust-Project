#ifndef LVL_MODE_SELECT_H
#define LVL_MODE_SELECT_H

#include "../../common_features/edit_mode_base.h"

class LVL_ModeSelect : public EditMode
{
public:
    LVL_ModeSelect(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~LVL_ModeSelect();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);

private:
    QPoint sourcePos;
    int gridSize;
    int offsetX;
    int offsetY;//, gridX, gridY, i=0;
    void setItemSourceData(QGraphicsItem *it, QString ObjType);
};

#endif // LVL_MODE_SELECT_H
