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


QLineF item_rectangles::snapLine(QLineF mouseLine, QSizeF Box)
{
    qreal a = mouseLine.angle();
    qreal relA = QLineF(0,0, Box.width(), -Box.height()).angle();
    qreal tarA = 0;

    if(a == 0 ||
            a == 90 ||
            a == 180 ||
            a == 270)
        return QLineF::fromPolar(mouseLine.length(),a).translated(mouseLine.p1());

    if(a <= 90){
        if(a < relA/2){
            tarA = 0;
        }else if(a < 90-relA/2){
            tarA = relA;
        }else{
            tarA = 90;
        }
    }else if(a <= 180){
        if(a <= 90+relA/2 ){
            tarA = 90;
        }else if(a < 180-relA/2 ){
            tarA = 180-relA;
        }else{
            tarA = 180;
        }
    }else if(a <= 270){
        if(a < 180+(relA/2)){
            tarA = 180;
        }else if(a >= 180+relA/2){
            tarA = 180+relA;
        }else{
            tarA = 270;
        }
    }else{
        if(a <= 270+(relA/2) ){
            tarA = 270;
        }else if(a < 360-relA/2){
            tarA = 360-relA;
        }else{
            tarA = 360;
        }
    }

    return QLineF::fromPolar(mouseLine.length(),tarA).translated(mouseLine.p1());;
}
