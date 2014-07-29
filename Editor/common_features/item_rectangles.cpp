#include "item_rectangles.h"

QList<QGraphicsRectItem *> item_rectangles::rectArray;

void item_rectangles::clearArray()
{
    while(!rectArray.isEmpty())
    {
        QGraphicsRectItem * tmp = rectArray.first();
        rectArray.pop_front();
        if(tmp) delete tmp;
    }
}

void item_rectangles::drawMatrix(QGraphicsScene *scene, QRect bigRect, QSize smallRect)
{
    clearArray();

    long x = bigRect.x();
    long y = bigRect.y();
    long width = bigRect.width();
    long height = bigRect.height();
    int repWidth = width/smallRect.width();
    int repHeight = height/smallRect.height();
    QBrush brush = QBrush(Qt::darkYellow);

    for(int i = 0; i < repWidth; i++){
        for(int j = 0; j < repHeight; j++){
            long x1 = x + i * smallRect.width();
            long y1 = y + j * smallRect.height();
            rectArray.push_back(scene->addRect(0, 0, smallRect.width(), smallRect.height(), QPen(Qt::yellow, 2), brush));
            rectArray.last()->setPos(x1, y1);
            rectArray.last()->setOpacity(0.7);
            rectArray.last()->setZValue(7000);
        }
    }
}
