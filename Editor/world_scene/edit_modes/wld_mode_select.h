#ifndef WLD_MODE_SELECT_H
#define WLD_MODE_SELECT_H

#include "../../common_features/edit_mode_base.h"

class WLD_ModeSelect : public EditMode
{
public:
    WLD_ModeSelect(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~WLD_ModeSelect();
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

#endif // WLD_MODE_SELECT_H
