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
    static void drawRound(QGraphicsScene *scene, QRect bigRect, QSize smallRect);
    static void drawLine(QGraphicsScene *scene, QLineF lineItem, QSize smallRect);

    static QLineF snapLine(QLineF mouseLine, QSizeF Box);
};

#endif // ITEM_RECTANGLES_H
