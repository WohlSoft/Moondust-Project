/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QBrush>
#include <QLinearGradient>
#include <QDebug>
#include <math.h>
#include <QMenu>

#include <common_features/grid.h>
#include <common_features/logger.h>

#include "item_resizer.h"

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
    m_width(size.width()),
    m_height(size.height()),
    m_cornerDragStart(0, 0),
    m_drawingWidth(m_width),
    m_drawingHeight(m_height),
    m_drawingOrigenX(0),
    m_drawingOrigenY(0)
{
    m_dragStart = QPointF(0, 0);
    m_resizerType = ItemResizer::Resizer_Section;

    m_resizerColor = color;

    m_grid = grid;
    m_no_grid = false;
    m_no_limit = false;

    this->setRect(0, 0, m_width, m_height);
    this->setZValue(50000);
    m_targetItem = nullptr;

    this->setFlag(QGraphicsItem::ItemIsMovable, true);

    QBrush brush;
    brush.setStyle(Qt::DiagCrossPattern);
    brush.setColor(m_resizerColor);
    this->setBrush(brush);

    m_location = this->pos();

    QPen pEn;
    pEn.setWidth(3);
    pEn.setColor(m_resizerColor);
    this->setPen(pEn);

    this->setAcceptHoverEvents(true);

    //Sides
    m_corners[4] = new CornerGrabber(this, 4, m_resizerColor);
    m_corners[5] = new CornerGrabber(this, 5, m_resizerColor);
    m_corners[6] = new CornerGrabber(this, 6, m_resizerColor);
    m_corners[7] = new CornerGrabber(this, 7, m_resizerColor);

    //corners
    m_corners[0] = new CornerGrabber(this, 0, m_resizerColor);
    m_corners[1] = new CornerGrabber(this, 1, m_resizerColor);
    m_corners[2] = new CornerGrabber(this, 2, m_resizerColor);
    m_corners[3] = new CornerGrabber(this, 3, m_resizerColor);

    m_minSize = QSizeF(64, 64); //set Default minimal size
    setCornerPositions();
}

void ItemResizer::setGridSize(int gridSize)
{
    m_grid = gridSize;
}

/**
  * This scene event filter has been registered with all four corner grabber items.
  * When called, a pointer to the sending item is provided along with a generic
  * event.  A dynamic_cast is used to determine if the event type is one of the events
  * we are interrested in.
  */


bool ItemResizer::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    static QPoint sXY;
    static QPoint sWH;


    CornerGrabber *corner = dynamic_cast<CornerGrabber *>(watched);
    if(corner == nullptr)
    {
        return false; // not expected to get here
    }

    QGraphicsSceneMouseEvent *mevent = dynamic_cast<QGraphicsSceneMouseEvent *>(event);
    if(mevent == nullptr)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }


    switch(event->type())
    {
    // if the mouse went down, record the x,y coords of the press, record it inside the corner object
    case QEvent::GraphicsSceneMousePress:
    {
        sXY = this->scenePos().toPoint();
        sWH = QPoint(sXY.x() + this->rect().width(), sXY.y() + this->rect().height());

        corner->setMouseState(CornerGrabber::kMouseDown);
        corner->mouseDownX = mevent->scenePos().x();
        corner->mouseDownY = mevent->scenePos().y();
    }
    break;

    case QEvent::GraphicsSceneMouseRelease:
    {
        corner->setMouseState(CornerGrabber::kMouseReleased);
        corner->ungrabMouse();
    }
    break;

    case QEvent::GraphicsSceneMouseMove:
    {
        if(corner->getMouseState() != CornerGrabber::kMouseReleased)
            corner->setMouseState(CornerGrabber::kMouseMoving);
    }
    break;

    default:
        // we dont care about the rest of the events
        return false;
        break;
    }

    if(corner->getMouseState() == CornerGrabber::kMouseMoving)
    {
#ifdef _DEBUG_
        qDebug() << "mouseMove state activated!";
#endif

        //Current XY (left-top corner)
        QPoint cXY = this->scenePos().toPoint();
        //Current HeightWidth point (right-bottom corner)
        QPoint cWH = QPoint(cXY.x() + this->rect().width(), cXY.y() + this->rect().height());

        QPoint oXY = cXY; //BackUP
        QPoint oWH = cWH;

#ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("Resizer -> StartPos -> %1 %2 size %3x%4").arg(cXY.x()).arg(cXY.y()).arg(cWH.x()).arg(cWH.y()));
        WriteToLog(QtDebugMsg, QString("Resizer -> mouse XY %1-%2 corner %3").arg(mevent->scenePos().x()).arg(mevent->scenePos().y()).arg(corner->getCorner()));
#endif

        bool DeltaSize = false; //Size was changed
        bool DeltaPos = false; //Position was changed

        bool P_onlyX = false; //Change only X position
        bool P_onlyY = false; //Change only Y position
        bool S_onlyX = false; //Change only X size
        bool S_onlyY = false; //Change only Y size

        bool validX = false;
        bool validY = false;
        const QSizeF &minSize = m_no_limit ? QSizeF(16.0, 16.0) : m_minSize;

        switch(corner->getCorner())
        {
        case 0:
            cXY = QPoint(mevent->scenePos().x(), mevent->scenePos().y());
            DeltaPos = true;
            break;
        case 1:
            cXY = QPoint(sXY.x(), mevent->scenePos().y());
            cWH = QPoint(mevent->scenePos().x(), sWH.y());
            DeltaPos = true;
            DeltaSize = true;
            P_onlyY = true;
            S_onlyX = true;
            break;
        case 2:
            cWH = QPoint(mevent->scenePos().x(), mevent->scenePos().y());
            DeltaSize = true;
            break;
        case 3:
            cXY = QPoint(mevent->scenePos().x(), sXY.y());
            cWH = QPoint(sWH.x(), mevent->scenePos().y());
            DeltaPos = true;
            DeltaSize = true;
            P_onlyX = true;
            S_onlyY = true;
            break;
        case 4:
            cXY = QPoint(sXY.x(), mevent->scenePos().y());
            DeltaPos = true;
            P_onlyY = true;
            break;
        case 5:
            cXY = QPoint(mevent->scenePos().x(), sXY.y());
            DeltaPos = true;
            P_onlyX = true;
            break;
        case 6:
            cWH = QPoint(sWH.x(), mevent->scenePos().y());
            DeltaSize = true;
            S_onlyY = true;
            break;
        case 7:
            cWH = QPoint(mevent->scenePos().x(), sWH.y());
            DeltaSize = true;
            P_onlyY = true;
            break;
        default:
            break;
        }

        if(DeltaPos)
            cXY = Grid::applyGrid(cXY, m_no_grid ? 1 : m_grid);
        else
            cXY = sXY;

        if(DeltaSize)
            cWH = Grid::applyGrid(cWH, m_no_grid ? 1 : m_grid);
        else
            cWH = sWH;

        if(P_onlyX) cXY.setY(oXY.y());
        if(P_onlyY) cXY.setX(oXY.x());
        if(S_onlyX) cWH.setY(cWH.y());
        if(S_onlyY) cWH.setX(oWH.x());


        if(cXY.x() < cWH.x())
            validX = true;
        if(cXY.y() < cWH.y())
            validY = true;

        if(validX)
        {
            qreal t_width = (qreal)abs(cXY.x() - cWH.x());
            if(t_width < minSize.width())
            {
                cXY.setX(oXY.x());
                cWH.setX(oWH.x());
                m_width = m_drawingWidth;
            }
            else
            {
                m_width = t_width;
            }

        }
        else
        {
            cXY.setX(oXY.x());
            cWH.setX(oWH.x());
            m_width = m_drawingWidth;
        }

        if(validY)
        {
            qreal t_height = (qreal)abs(cXY.y() - cWH.y());
            if(t_height < minSize.height())
            {
                cXY.setY(
                    ((m_resizerType >= ItemResizer::Resizer_Item) ||
                     ((corner->getCorner() != 0) &&
                      (corner->getCorner() != 1) &&
                      (corner->getCorner() != 4))) ?
                    oXY.y() :
                    (oWH.y() - minSize.height()));
                cWH.setY(oWH.y());
                m_drawingHeight = (qreal)abs(cXY.y() - cWH.y());
                m_height = m_drawingHeight;
            }
            else
            {
                m_height = t_height;
            }

        }
        else
        {
            cXY.setY(oXY.y());
            cWH.setY(oWH.y());
            m_height = m_drawingHeight;
        }

        m_drawingWidth  =  m_width;
        m_drawingHeight =  m_height;

#ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("Resizer -> TargetPos %1x%2 size %3x%4").arg(cXY.x()).arg(cXY.y()).arg(cWH.x()).arg(cWH.y()));
#endif

        this->setPos(QPointF(cXY));
        this->setRect(0, 0, m_width, m_height);

        setCornerPositions();
    }

    return true;// true => do not send event to watched - we are finished with this event
}



// for supporting moving the box across the scene
void ItemResizer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button() == Qt::RightButton)
    {
        QMenu contextMenu;
        QAction *crop_top =    contextMenu.addAction(QObject::tr("Cut top here"));
        QAction *crop_bottom = contextMenu.addAction(QObject::tr("Cut bottom here"));
        QAction *crop_left =   contextMenu.addAction(QObject::tr("Cut left here"));
        QAction *crop_right =  contextMenu.addAction(QObject::tr("Cut right here"));
        contextMenu.addSeparator();

        QAction *no_grid =     contextMenu.addAction(QObject::tr("Don't snap to grid"));
        no_grid->setCheckable(true);
        no_grid->setChecked(m_no_grid);

        QAction *no_limit =     contextMenu.addAction(QObject::tr("Disable minimal size limit"));
        no_limit->setCheckable(true);
        no_limit->setChecked(m_no_limit);

        QAction *answer = contextMenu.exec(event->screenPos());
        if(answer == nullptr)
        {
            event->setAccepted(true);
            return;
        }

        if(answer == no_grid)
        {
            m_no_grid = no_grid->isChecked();
            event->setAccepted(true);
            return;
        }
        else if(answer == no_limit)
        {
            m_no_limit = no_limit->isChecked();
            event->setAccepted(true);
            return;
        }

        QPointF target = event->scenePos();
        target = QPointF(Grid::applyGrid(target.toPoint(), m_no_grid ? 1 : m_grid));

        QRectF rect;
        rect.setTop(this->scenePos().y());
        rect.setLeft(this->scenePos().x());
        rect.setWidth(m_width);
        rect.setHeight(m_height);

        const QSizeF &minSize = m_no_limit ? QSizeF(16.0, 16.0) : m_minSize;

        if(answer == crop_top)
        {
            rect.setTop(target.y());

            if(rect.height() < minSize.height())
                rect.setTop(rect.top() - (minSize.height() - rect.height()));
        }
        else if(answer == crop_bottom)
        {
            rect.setBottom(target.y());

            if(rect.height() < minSize.height())
                rect.setBottom(rect.bottom() + (minSize.height() - rect.height()));
        }
        else if(answer == crop_left)
        {
            rect.setLeft(target.x());

            if(rect.width() < minSize.width())
                rect.setLeft(rect.left() - (minSize.width() - rect.width()));
        }
        else if(answer == crop_right)
        {
            rect.setRight(target.x());

            if(rect.width() < minSize.width())
                rect.setRight(rect.right() + (minSize.width() - rect.width()));
        }

        m_width = rect.width();
        m_height = rect.height();

        m_drawingWidth  =  m_width;
        m_drawingHeight =  m_height;

        this->setPos(rect.topLeft());
        this->setRect(0, 0, m_width, m_height);
        setCornerPositions();
    }

    event->setAccepted(true);
}

// for supporting moving the box across the scene
void ItemResizer::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    event->setAccepted(true);
    m_dragStart = event->pos();
    //qDebug() << QString("Drag X:%1 Y:%2").arg(_dragStart.x()).arg(_dragStart.y());

    QPointF newPos = event->scenePos() ;
    m_location = (newPos - m_dragStart);

    m____lastLocation = m_location;
    m____offset = this->scenePos() - event->scenePos();
    //qDebug() << QString("Move -1 loc X:%1 Y:%2").arg(_location.x()).arg(_location.y());
}


// for supporting moving the box across the scene
void ItemResizer::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
        m_dragStart = event->pos();

        QPointF newPos = event->scenePos() ;
        m_location = (newPos - m_dragStart);

        QPoint gridOffset;
        gridOffset = QPoint(0, 0);

        const QSizeF &minSize = m_no_limit ? QSizeF(16.0, 16.0) : m_minSize;

        if(this->rect().height() <= minSize.height())
        {
            gridOffset.setY(-(qRound(minSize.height()) % (m_no_grid ? 1 : m_grid)));
        }
        m____lastLocation = QPointF(
                                Grid::applyGrid((m____lastLocation - (event->lastScenePos() - event->scenePos())).toPoint(),
                                                m_no_grid ? 1 : m_grid, gridOffset)
                            );
        if(m____lastLocation != this->scenePos())
        {
            this->setPos(m____lastLocation);
        }
        m____lastLocation = event->scenePos() + m____offset;
    }
    event->accept();
    /*
    QPointF newPos = event->pos() ;
    _location += (newPos - _dragStart);

    this->setPos( QPointF( Grid::applyGrid(_location.toPoint(), _grid ) ) );
    */
}

// remove the corner grabbers

void ItemResizer::hoverLeaveEvent(QGraphicsSceneHoverEvent *)
{
    //_outterborderColor = Qt::black;
}


// create the corner grabbers

void ItemResizer::hoverEnterEvent(QGraphicsSceneHoverEvent *)
{
    //_outterborderColor = Qt::red;

    m_corners[0]->installSceneEventFilter(this);
    m_corners[1]->installSceneEventFilter(this);
    m_corners[2]->installSceneEventFilter(this);
    m_corners[3]->installSceneEventFilter(this);
    m_corners[4]->installSceneEventFilter(this);
    m_corners[5]->installSceneEventFilter(this);
    m_corners[6]->installSceneEventFilter(this);
    m_corners[7]->installSceneEventFilter(this);

    setCornerPositions();
}

void ItemResizer::setCornerPositions()
{
    m_corners[0]->setPos(m_drawingOrigenX - 5, m_drawingOrigenY - 5);
    m_corners[1]->setPos(m_drawingWidth - 5,  m_drawingOrigenY - 5);
    m_corners[2]->setPos(m_drawingWidth - 5 , m_drawingHeight - 5);
    m_corners[3]->setPos(m_drawingOrigenX - 5, m_drawingHeight - 5);

    //_corners[4]->setPos( (_drawingOrigenX-5) + qreal((double)_width/2), _drawingOrigenY-5); //top
    m_corners[4]->setPos((m_drawingOrigenX + 2), m_drawingOrigenY - 2); //top
    m_corners[4]->setSize(m_width - 2, 4);

    //_corners[5]->setPos( (_drawingOrigenX-5),  (_drawingOrigenY-5)+qreal((double)_height/2) ); //left
    m_corners[5]->setPos((m_drawingOrigenX - 2), m_drawingOrigenY + 2); //left
    m_corners[5]->setSize(4, m_height - 2);

    //_corners[6]->setPos( (_drawingOrigenX-5) + qreal(_width/2), (_drawingHeight-5));  //bottom
    m_corners[6]->setPos(m_drawingOrigenX - 2, (m_drawingHeight - 2)); //bottom
    m_corners[6]->setSize(m_width - 2, 4);

    //_corners[7]->setPos( (_drawingWidth-5), (_drawingHeight-5)-qreal((double)_height/2)); //right
    m_corners[7]->setPos((m_drawingWidth - 2), (m_drawingOrigenY + 2)); //right
    m_corners[7]->setSize(4, m_height - 2);
}

QRectF ItemResizer::boundingRect() const
{
    return QRectF(-10, -10, m_width + 20, m_height + 20);
}

void ItemResizer::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void ItemResizer::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}
