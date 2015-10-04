
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "raster_convas.h"

rasterCanvas::rasterCanvas(QImage *img, QGraphicsItem *parent) :
    QGraphicsItem(parent),
    image(img),
    foreColor(0,0,0,255),
    backColor(255, 255, 255, 58)
{}

rasterCanvas::~rasterCanvas(){}

QRectF rasterCanvas::boundingRect() const
{
    if(image)
    { return QRectF(image->rect()); } else return QRectF(0,0,0,0);
}

void rasterCanvas::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(image->rect(), QPixmap::fromImage(*image), image->rect());
}

void rasterCanvas::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(image)
    {
        if(((mouseEvent->buttons()&Qt::LeftButton)!=0)||((mouseEvent->buttons()&Qt::RightButton)!=0))
        {
            lastPos=mouseEvent->pos().toPoint();
            image->setPixel(mouseEvent->pos().toPoint(),
                ((mouseEvent->buttons()&Qt::LeftButton)!=0)?foreColor.value():backColor.value());
        }
        update();
    }
}

void rasterCanvas::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(image)
    {
        if(((mouseEvent->buttons()&Qt::LeftButton)!=0)||((mouseEvent->buttons()&Qt::RightButton)!=0))
        {
            if(lastPos!=mouseEvent->pos().toPoint())
            {
                QBrush br( ((mouseEvent->buttons()&Qt::LeftButton)!=0)?foreColor:backColor );
                QPainter painter(image);
                painter.setBrush(br);
                painter.setPen(QPen(br, 25.0));
                painter.drawLine(lastPos, mouseEvent->pos().toPoint());
                lastPos=mouseEvent->pos().toPoint();
            }
            update();
        }
    }
}

void rasterCanvas::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{}
