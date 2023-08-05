/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef LVLBASEITEM_H
#define LVLBASEITEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPoint>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <math.h>

class LvlScene;

class LvlBaseItem:
    public QObject,
    public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

private:
    void construct();
public:
    LvlBaseItem(QGraphicsItem *parent = nullptr);
    LvlBaseItem(LvlScene *parentScene, QGraphicsItem *parent = nullptr);
    virtual ~LvlBaseItem();

    virtual void setScenePoint(LvlScene *theScene);

    ///
    /// \brief arrayApply();
    /// Registring current state of item into data array
    ///
    virtual void arrayApply();

    ///
    /// \brief returnBack();
    /// Returning item into last saved position
    ///
    virtual void returnBack();

    ///
    /// \brief removeFromArray();
    /// Removing item from array
    ///
    virtual void removeFromArray();

    ///
    /// \brief setLayer();
    /// Changes parent layer
    ///
    /// \param layer
    /// Layer name
    ///
    virtual void setLayer(QString layer);

    ///
    /// \brief getGridSize
    /// \return grid size of item
    ///
    virtual int getGridSize();

    ///
    /// \brief gridOffset
    /// \return offset between grid point and aligned point
    ///
    virtual QPoint gridOffset();

    ///
    /// \brief sourcePos
    /// \return registred position of item
    ///
    virtual QPoint sourcePos();

    //!
    //! \brief Implementation of context menu for current item type
    //! \param mouseEvent
    //!
    virtual void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

    virtual QPainterPath shape() const;

    int m_gridSize;
    int m_gridOffsetX;
    int m_gridOffsetY;

    //Locks
    bool m_locked;
    void setLocked(bool lock);
    virtual QString getLayerName();
    virtual bool itemTypeIsLocked();

    /**
     * @brief Show / Hide meta-signs of this element
     * @param visible Visibility state: true is visible, false is invisible
     */
    virtual void setMetaSignsVisibility(bool visible);

protected:
    long        m_animatorID;
    QRectF      m_imageSize;
    bool        m_animated;
    LvlScene   *m_scene;

    bool m_mouseLeft;
    bool m_mouseMid;
    bool m_mouseRight;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
};

#endif // LVLBASEITEM_H
