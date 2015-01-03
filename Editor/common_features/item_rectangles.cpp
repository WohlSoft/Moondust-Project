/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <qmath.h>

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
            rectArray.push_back(scene->addRect(1, 1, smallRect.width()-2, smallRect.height()-2,
                                               QPen(Qt::yellow, 1),
                                               brush));
            rectArray.last()->setPos(x1, y1);
            rectArray.last()->setData(0, "YellowRectangle");
            rectArray.last()->setOpacity(0.7);
            rectArray.last()->setZValue(10000);
        }
    }
}

void item_rectangles::drawLine(QGraphicsScene *scene, QLineF lineItem, QSize smallRect)
{
    clearArray();

    int h_dir=0;
    int v_dir=0;

    int angle = qRound(lineItem.angle());

    //Calculate direction
    switch(angle)
    {
        case 0:
            h_dir=1;
            break;
        case 90:
            v_dir=-1;
            break;
        case 180:
            h_dir=-1;
            break;
        case 270:
            v_dir=1;
            break;
        default:
            if((angle>0)&&(angle<90))
            {
                h_dir=1;
                v_dir=-1;
            }
            else if((angle>90)&&(angle<180))
            {
                h_dir=-1;
                v_dir=-1;
            }
            else if((angle>180)&&(angle<270))
            {
                h_dir=-1;
                v_dir=1;
            }
            else if((angle>270)&&(angle<360))
            {
                h_dir=1;
                v_dir=1;
            }
        break;
    }

    long x = lineItem.p1().x();
    long y = lineItem.p1().y();

    long tW = fabs(qRound(lineItem.dx())); //targetWidth
    long tH = fabs(qRound(lineItem.dy())); //targetHeight

        QBrush brush = QBrush(Qt::darkYellow);

    for(int k=0,l=0, i=0, j=0; (i<=tW)&&(j<=tH); i+=smallRect.width()*fabs(h_dir), j+=smallRect.height()*fabs(v_dir),k++,l++ )
    {
        long x1 = x + k * smallRect.width()*h_dir;
        long y1 = y + l * smallRect.height()*v_dir;
        rectArray.push_back(scene->addRect(1, 1, smallRect.width()-2,
                                           smallRect.height()-2,
                                           QPen(Qt::yellow, 2), brush));
        rectArray.last()->setPos(x1, y1);
        rectArray.last()->setData(0, "YellowRectangle");
        rectArray.last()->setOpacity(0.7);
        rectArray.last()->setZValue(10000);
    }
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
