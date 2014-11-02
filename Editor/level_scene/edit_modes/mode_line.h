#ifndef MODE_LINE_H
#define MODE_LINE_H

#include "../../common_features/edit_mode_base.h"

class ModeLine : public EditMode
{
public:
    ModeLine(QGraphicsScene * parentScene = 0, QObject *parent = 0);
    ~ModeLine();
    void set();
    void mousePress(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseMove(QGraphicsSceneMouseEvent *mouseEvent);
    void mouseRelease(QGraphicsSceneMouseEvent *mouseEvent);

    void keyPress(QKeyEvent *keyEvent);
    void keyRelease(QKeyEvent *keyEvent);

private:
    QPointF drawStartPos;

};

#endif // MODE_SQUARE_H
