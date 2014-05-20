#ifndef CORNERGRABBER_H
#define CORNERGRABBER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QPointF>



class CornerGrabber : public QGraphicsItem
{

public:
    explicit CornerGrabber(QGraphicsItem *parent = 0,  int corner=0);

    int  getCorner(); ///< allows the owner to find out which coner this is
    void setMouseState(int); ///< allows the owner to record the current mouse state
    int  getMouseState(); ///< allows the owner to get the current mouse state

    qreal mouseDownX;
    qreal mouseDownY;

     enum {kMouseReleased=0, kMouseDown, kMouseMoving}; ///< define the mouse states



private:

    virtual QRectF boundingRect() const; ///< must be re-implemented in this class to provide the diminsions of the box to the QGraphicsView
    virtual void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget); ///< must be re-implemented here to pain the box on the paint-event
    virtual void hoverEnterEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover enter events
    virtual void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ); ///< must be re-implemented to handle mouse hover leave events

    // once the hover event handlers are implemented in this class,
    // the mouse events must allow be implemented because of
    // some linkage issue - apparrently there is some connection
    // between the hover events and mouseMove/Press/Release
    // events which triggers a vtable issue
    virtual void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mousePressEvent (QGraphicsSceneMouseEvent * event );
    virtual void mousePressEvent(QGraphicsSceneDragDropEvent *event);
    virtual void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );

    QColor _outterborderColor; ///< the hover event handlers will toggle this between red and black
    QPen _outterborderPen; ///< the pen is used to paint the red/black border

    qreal   _width;
    qreal   _height;

    int _corner;// 0,1,2,3  - starting at x=0,y=0 and moving clockwise around the box


    int _mouseButtonState;


};

#endif // CORNERGRABBER_H
