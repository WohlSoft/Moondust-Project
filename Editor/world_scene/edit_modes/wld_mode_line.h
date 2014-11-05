#ifndef WLD_MODE_LINE_H
#define WLD_MODE_LINE_H

#include "../../common_features/edit_mode_base.h"

class WLD_ModeLine : public EditMode
{
public:
    WLD_ModeLine(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~WLD_ModeLine();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);

private:
    QPointF drawStartPos;

};

#endif // WLD_MODE_SQUARE_H
