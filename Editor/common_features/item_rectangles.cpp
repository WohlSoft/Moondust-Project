#include "item_rectangles.h"
#include <qmath.h>

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

void item_rectangles::drawLine(QGraphicsScene *scene, QLine lineItem, QSize smallRect)
{

    //TEMPLATE!!!!!
//    clearArray();

//    //long x = bigRect.x();
//    //long y = bigRect.y();
//    //long width = lineItem.dx();
//    //long height = lineItem.dy();
//    //int repWidth = width/smallRect.width();
//    //int repHeight = height/smallRect.height();
//    QBrush brush = QBrush(Qt::darkYellow);

//    //long x1 = x + i * smallRect.width();
//    //long y1 = y + j * smallRect.height();
//    rectArray.push_back(scene->addRect(0, 0, smallRect.width(),
//                                       smallRect.height(),
//                                       QPen(Qt::yellow, 2), brush));
//    rectArray.last()->setPos(x1, y1);
//    rectArray.last()->setOpacity(0.7);
//    rectArray.last()->setZValue(7000);
}


QLineF item_rectangles::snapLine(QLineF mouseLine, QSizeF Box)
{
    qreal a = mouseLine.angle();
    qreal relA = QLineF(0,0, Box.width(), -Box.height()).angle();
    qreal tarA = 0;

    bool isDiagonal=false;
    bool isVertical=false;

    if(a == 0)
    {
        tarA = a;
        goto skipAngleCalculate;
    }
    else if(a == 90)
    {
        tarA = a;
        isVertical=true;
        goto skipAngleCalculate;
    }
    else if(a == 180)
    {
        tarA = a;
        goto skipAngleCalculate;
    }
    else if(a == 270)
    {
        tarA = a;
        isVertical=true;
        goto skipAngleCalculate;
    }

    //Calculating target angle
    if(a <= 90){
        if(a < relA/2){
            tarA = 0;
        }else if(a <= 90-(90-relA)/2 ){
            tarA = relA;
            isDiagonal=true;
        }else{
            tarA = 90;
            isVertical=true;
        }
    }else if(a <= 180){
        if(a <= 90+(90-relA)/2 ){
            tarA = 90;
            isVertical=true;
        }else if(a <= 180-relA/2 ){
            tarA = 180-relA;
            isDiagonal=true;
        }else{
            tarA = 180;
        }
    }else if(a <= 270){
        if(a < 180+(relA/2)){
            tarA = 180;
        }else if(a <= 270-(90-relA)/2){
            tarA = 180+relA;
            isDiagonal=true;
        }else{
            tarA = 270;
            isVertical=true;
        }
    }else{
        if(a <= 270+(90-relA)/2 ){
            tarA = 270;
            isVertical=true;
        }else if(a < 360-relA/2){
            tarA = 360-relA;
            isDiagonal=true;
        }else{
            tarA = 360;
        }
    }


    skipAngleCalculate:

    qreal diagonalSize = qSqrt( qPow(Box.height(),2) + qPow(Box.width(),2) ); //get box diagonal size
    qreal Lenght = mouseLine.length() -
            ((isDiagonal)? qRound(mouseLine.length())%qRound(diagonalSize)   //Diagonal
            : (isVertical ? qRound(mouseLine.length())%qRound(Box.height()): //Vertical
              qRound(mouseLine.length())%qRound(Box.width()) ) );            //Horizontal

    return QLineF::fromPolar(Lenght,tarA).translated(mouseLine.p1());;
}
