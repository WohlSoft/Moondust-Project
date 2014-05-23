/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "item_resizer.h"

#include <QBrush>
#include <QLinearGradient>
#include <QDebug>

#include <math.h>
#include "../../common_features/grid.h"

/**
  *  This box can be re-sized and it can be moved. For moving, we capture
  *  the mouse move events and move the box location accordingly.
  *
  *  To resize the box, we place small indicator boxes on the four corners that give the user
  *  a visual cue to grab with the mouse. The user then drags the corner to stretch
  *  or shrink the box.  The corner grabbers are implemented with the CornerGrabber class.
  *  The CornerGrabber class captures the mouse when the mouse is over the corner's area,
  *  but the ItemResizer object (which owns the corners) captures and processes the mouse
  *  events on behalf of the CornerGrabbers (because the owner wants to be
  *  resized, not the CornerGrabbers themselves). This is accomplished by installed a scene event filter
  *  on the CornerGrabber objects:
          _corners[0]->installSceneEventFilter(this);
  *
  *
  *
*/

ItemResizer::ItemResizer(QSize size, QColor color, int grid):
        //_outterborderColor(Qt::black),
        _width( size.width() ),
        _height( size.height() ),
        _cornerDragStart(0,0),
        _drawingWidth(  _width ),
        _drawingHeight( _height ),
        _drawingOrigenX( 0 ),
        _drawingOrigenY( 0 )
{
    _dragStart = QPointF(0,0);
    type = 0;

    _resizerColor = color;

    _grid = grid;

    this->setRect(0,0,_width,_height);
    this->setZValue(50000);

    QBrush brush;
    brush.setStyle(Qt::DiagCrossPattern);
    brush.setColor( _resizerColor );
    this->setBrush(brush);

    _location = this->pos();

    QPen pEn;
    pEn.setWidth(3);
    pEn.setColor(_resizerColor);
    this->setPen(pEn);

    this->setAcceptHoverEvents(true);

    _corners[0] = new CornerGrabber(this,0);
    _corners[1] = new CornerGrabber(this,1);
    _corners[2] = new CornerGrabber(this,2);
    _corners[3] = new CornerGrabber(this,3);
    _corners[4] = new CornerGrabber(this,4);
    _corners[5] = new CornerGrabber(this,5);
    _corners[6] = new CornerGrabber(this,6);
    _corners[7] = new CornerGrabber(this,7);

    _minSize = QSizeF(64, 64); //set Default minimal size

    setCornerPositions();

}

/**
 *  To allow the user to grab the corners to re-size, we need to get a hover
 *  indication. But if the mouse pointer points to the left, then when the mouse
 *  tip is to the left but just outsize the box, we will not get the hover.
 *  So the solution is to tell the graphics scene the box is larger than
 *  what the painter actually paints in. This way when the user gets the mouse
 *  within a few pixels of what appears to be the edge of the box, we get
 *  the hover indication.

 *  So the cornerGrabBuffer is a few pixel wide buffer zone around the outside
 *  edge of the box.
 *
 */

void ItemResizer::adjustSize(int x, int y)
{
    _width += x;
    _height += y;

    QPoint sz = Grid::applyGrid(QPoint(_width, _height), _grid );
    _width = sz.x();
    _height = sz.y();

    _drawingWidth =  _width;
    _drawingHeight=  _height;

    this->setRect(0,0,_drawingWidth,_drawingHeight);

}

/**
  * This scene event filter has been registered with all four corner grabber items.
  * When called, a pointer to the sending item is provided along with a generic
  * event.  A dynamic_cast is used to determine if the event type is one of the events
  * we are interrested in.
  */

bool ItemResizer::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
    CornerGrabber * corner = dynamic_cast<CornerGrabber *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }


    switch (event->type() )
    {
            // if the mouse went down, record the x,y coords of the press, record it inside the corner object
        case QEvent::GraphicsSceneMousePress:
            {
                corner->setMouseState(CornerGrabber::kMouseDown);
                corner->mouseDownX = mevent->pos().x();
                corner->mouseDownY = mevent->pos().y();
            }
            break;

        case QEvent::GraphicsSceneMouseRelease:
            {
                corner->setMouseState(CornerGrabber::kMouseReleased);
            }
            break;

        case QEvent::GraphicsSceneMouseMove:
            {
                corner->setMouseState(CornerGrabber::kMouseMoving );
            }
            break;

        default:
            // we dont care about the rest of the events
            return false;
            break;
    }


    if ( corner->getMouseState() == CornerGrabber::kMouseMoving )
    {

        qreal x = mevent->pos().x(), y = mevent->pos().y();
         prepareGeometryChange();

        // depending on which corner has been grabbed, we want to move the position
        // of the item as it grows/shrinks accordingly. so we need to eitehr add
        // or subtract the offsets based on which corner this is.

        int onlyX=1;
        int onlyY=1;

        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( corner->getCorner() )
        {
        case 5:
            onlyX=0;
        case 0:
            {
                XaxisSign = +1;
                YaxisSign = +1;
            }
            break;
        case 4:
            onlyY=0;
        case 1:
            {
                XaxisSign = -1;
                YaxisSign = +1;
            }
            break;
        case 7:
            onlyX=0;
        case 2:
            {
                XaxisSign = -1;
                YaxisSign = -1;
            }
            break;
        case 6:
            onlyY=0;
        case 3:
            {
                XaxisSign = +1;
                YaxisSign = -1;
            }
            break;

        }

        // if the mouse is being dragged, calculate a new size and also re-position
        // the box to give the appearance of dragging the corner out/in to resize the box

        int xMoved = (corner->mouseDownX - x)*onlyY;
        int yMoved = (corner->mouseDownY - y)*onlyX;

        int newWidth = _width + ( XaxisSign * xMoved);
        if ( newWidth < _minSize.width() ) newWidth  = _minSize.width();

        int newHeight = _height + (YaxisSign * yMoved) ;
        if ( newHeight < _minSize.height() ) newHeight = _minSize.height()-((type==0)?8:0);

        int deltaWidth  =   newWidth - _width ;
        int deltaHeight =   newHeight - _height ;

        adjustSize(  deltaWidth ,   deltaHeight);

        deltaWidth *= (-1);
        deltaHeight *= (-1);

        if ( corner->getCorner() == 0 )
        {
            //left-top
            int newXpos = this->pos().x() + deltaWidth;
            int newYpos = this->pos().y() + deltaHeight; //(allow height-8 if <min)

            this->setPos( QPointF( Grid::applyGrid(QPoint(newXpos, newYpos), _grid )));

            this->setRect(0, 0, _width, _height);
        }
        else   if ( corner->getCorner() == 1 )
        {
            //right-top
            int newYpos = this->pos().y() + deltaHeight; //(allow height-8 if <min)
            //this->setPos(this->pos().x(), newYpos);
            this->setPos( QPointF( Grid::applyGrid(QPoint(this->pos().x(), newYpos), _grid )));
            this->setRect(0, 0, _width, _height);
        }
        else   if ( corner->getCorner() == 3 )
        { //left-bottom
            int newXpos = this->pos().x() + deltaWidth;
            //this->setPos(newXpos, this->pos().y());

            this->setPos( QPointF( Grid::applyGrid(QPoint(newXpos, this->pos().y()), _grid )));

            this->setRect(0, 0, _width, _height);
        }
        else   if ( corner->getCorner() == 4 )
        {   //top
            int newYpos = this->pos().y() + deltaHeight; //(allow height-8 if <min)
            //this->setPos(this->pos().x(), newYpos);
            this->setPos( QPointF( Grid::applyGrid(QPoint(this->pos().x(), newYpos), _grid )));
            this->setRect(0, 0, _width, _height);
        }
        if ( corner->getCorner() == 5 )
        { //Left
            int newXpos = this->pos().x() + deltaWidth;
            int newYpos = this->pos().y() + deltaHeight;
            //this->setPos(newXpos, newYpos);
            this->setPos( QPointF( Grid::applyGrid(QPoint(newXpos, newYpos), _grid )));
            this->setRect(0, 0, _width, _height);
        }
        else   if ( corner->getCorner() == 6 )
        { //Bottom
            int newXpos = this->pos().x() + deltaWidth;
            //this->setPos(newXpos, this->pos().y());
            this->setPos( QPointF( Grid::applyGrid(QPoint(newXpos, this->pos().y()), _grid )));
            this->setRect(0, 0, _width, _height);
        }

        setCornerPositions();
    }

    return true;// true => do not send event to watched - we are finished with this event
}



// for supporting moving the box across the scene
void ItemResizer::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
}


// for supporting moving the box across the scene
void ItemResizer::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    _dragStart = event->pos();
    //qDebug() << QString("Drag X:%1 Y:%2").arg(_dragStart.x()).arg(_dragStart.y());

    QPointF newPos = event->scenePos() ;
    _location = (newPos-_dragStart);
    //qDebug() << QString("Move -1 loc X:%1 Y:%2").arg(_location.x()).arg(_location.y());
}


// for supporting moving the box across the scene
void ItemResizer::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    event->accept();
    /*
    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);

    this->setPos( QPointF( Grid::applyGrid(_location.toPoint(), _grid ) ) );
    */
}

// remove the corner grabbers

void ItemResizer::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
    //_outterborderColor = Qt::black;
}


// create the corner grabbers

void ItemResizer::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    //_outterborderColor = Qt::red;

    _corners[0]->installSceneEventFilter(this);
    _corners[1]->installSceneEventFilter(this);
    _corners[2]->installSceneEventFilter(this);
    _corners[3]->installSceneEventFilter(this);
    _corners[4]->installSceneEventFilter(this);
    _corners[5]->installSceneEventFilter(this);
    _corners[6]->installSceneEventFilter(this);
    _corners[7]->installSceneEventFilter(this);

    setCornerPositions();
}

void ItemResizer::setCornerPositions()
{
    _corners[0]->setPos(_drawingOrigenX-5, _drawingOrigenY-5);
    _corners[1]->setPos(_drawingWidth-5,  _drawingOrigenY-5);
    _corners[2]->setPos(_drawingWidth-5 , _drawingHeight-5);
    _corners[3]->setPos(_drawingOrigenX-5, _drawingHeight-5);

    _corners[4]->setPos( (_drawingOrigenX-5) + qreal((double)_width/2), _drawingOrigenY-5); //top
    _corners[5]->setPos( (_drawingOrigenX-5),  (_drawingOrigenY-5)+qreal((double)_height/2) ); //left
    _corners[6]->setPos( (_drawingOrigenX-5) + qreal(_width/2), (_drawingHeight-5));  //bottom
    _corners[7]->setPos( (_drawingWidth-5), (_drawingHeight-5)-qreal((double)_height/2)); //right
}

QRectF ItemResizer::boundingRect() const
{
    return QRectF(0,0,_width,_height);
}


void ItemResizer::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void ItemResizer::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}
