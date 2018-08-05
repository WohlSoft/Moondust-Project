/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
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
    ItemResizer(QSize size, QColor color = Qt::green, int grid = 32);

    qreal   m_width = 32.0;
    qreal   m_height = 32.0;

    enum ResizerType
    {
        Resizer_Section = 0,
        Resizer_InEventSection = 1,
        Resizer_Item = 2,
        Resizer_PhyzEnvZone = 3,
        Resizer_Capturer = 4,
    };

    //!Resizer Type: 0 - Section, 1 - selecting of position, 2 - item size
    int m_resizerType = Resizer_Section;

    QSizeF m_minSize;
    QGraphicsItem *m_targetItem = nullptr;

    void setGridSize(int gridSize);

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

    QPointF m_location;

    int m_grid = 1;

    bool m_no_grid = false;
    bool m_no_limit = false;

    QPointF m_dragStart;

    QColor  m_resizerColor;

    QPointF m_cornerDragStart;

    qreal   m_drawingWidth = 0.0;
    qreal   m_drawingHeight = 0.0;
    qreal   m_drawingOrigenX = 0.0;
    qreal   m_drawingOrigenY = 0.0;

    QPointF m____lastLocation;
    QPointF m____lastCurLocation;
    QPointF m____offset;

    //! 0,1,2,3  - starting at x=0,y=0 and moving clockwise around the box
    CornerGrabber *m_corners[8];
};

#endif // ItemResizer_H
