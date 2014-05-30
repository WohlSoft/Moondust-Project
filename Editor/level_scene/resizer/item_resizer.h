/*
  * Copyright 2010, David W. Drell
  *
  * This is free sample code. Use as you wish, no warranties.
  */

#ifndef ItemResizer_H
#define ItemResizer_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>
#include "corner_grabber.h"

class ItemResizer : public QGraphicsRectItem
{
public:
    ItemResizer(QSize size, QColor color = Qt::green, int grid=32);
    qreal   _width;
    qreal   _height;

    int type; //Resizer Type: 0 - Section, 1 - selecting of position, 2 - item size
    QSizeF _minSize;
    QGraphicsItem * targetItem;

    private:

    virtual QRectF boundingRect() const; ///< must be re-implemented in this class to provide the diminsions of the box to the QGraphicsView
    //virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); ///< must be re-implemented here to pain the box on the paint-event
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover enter events
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover leave events

    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );///< allows the main object to be moved in the scene by capturing the mouse move events
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual bool sceneEventFilter (QGraphicsItem *watched, QEvent * event ) ;

    void setCornerPositions();
    //void adjustSize(int x, int y);


    //QColor _outterborderColor; ///< the hover event handlers will toggle this between red and black
    //QPen _outterborderPen; ///< the pen is used to paint the red/black border

    QPointF _location;

    int _grid;

    QPointF _dragStart;

    QColor  _resizerColor;

    QPointF _cornerDragStart;

    qreal   _drawingWidth;
    qreal   _drawingHeight;
    qreal   _drawingOrigenX;
    qreal   _drawingOrigenY;

    CornerGrabber*  _corners[8];// 0,1,2,3  - starting at x=0,y=0 and moving clockwise around the box


};

#endif // ItemResizer_H
