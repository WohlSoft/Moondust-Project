#ifndef MODE_SELECT_H
#define MODE_SELECT_H

#include "../../common_features/edit_mode_base.h"

class ModeSelect : public EditMode
{
public:
    ModeSelect(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~ModeSelect();
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

#endif // MODE_SELECT_H
