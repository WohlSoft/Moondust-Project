#ifndef RASTER_CONVAS_H
#define RASTER_CONVAS_H

#include <QGraphicsItem>
#include <QImage>
#include <QPixmap>

class rasterCanvas : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    rasterCanvas(QImage *img, QGraphicsItem *parent=0);
    ~rasterCanvas();
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

    QImage *image;
    QPoint lastPos;
    QColor foreColor;
    QColor backColor;
};

#endif // RASTER_CONVAS_H

