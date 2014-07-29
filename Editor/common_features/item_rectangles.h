#ifndef ITEM_RECTANGLES_H
#define ITEM_RECTANGLES_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>

class item_rectangles
{
public:
    static QList<QGraphicsRectItem *> rectArray;
    static void clearArray();
    static void drawMatrix(QGraphicsScene *scene, QRect bigRect, QSize smallRect);

};

#endif // ITEM_RECTANGLES_H
