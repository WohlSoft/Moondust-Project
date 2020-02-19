/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef WLD_BASE_ITEM_H
#define WLD_BASE_ITEM_H

#include <QString>
#include <QPoint>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <math.h>

class WldScene;
class WldBaseItem: public QObject,
                   public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

    /*!
     * \brief Shared constructor function
     */
    void construct();
public:
    WldBaseItem(QGraphicsItem *parent=0);
    WldBaseItem(WldScene *parentScene, QGraphicsItem *parent=0);

    /*!
     * \brief Changes pointer to parent scene of this item
     * \param theScene pointer to World map scene
     */
    virtual void setScenePoint(WldScene *theScene);

    /*!
     * \brief Destructor
     */
    virtual ~WldBaseItem();

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
    /// \brief Changes value of grid
    /// \param grid size
    ///
    virtual void setGridSize(int grid);

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

    /*!
     * \brief Is item locked
     * \return state of item lock
     */
    bool isLocked();

    /*!
     * \brief Lock or unlock this item (locked elements are can't be dragged/selected/deleted)
     * \param lock state of lock
     */
    void setLocked(bool lock);

    /*!
     * \brief Spawn context menu
     * \param mouseEvent Pointer to mouse event properties class
     */
    virtual void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

    /*!
     * \brief Detects global lock of items by type
     * \return state of type-based global lock of items
     */
    virtual bool itemTypeIsLocked();

protected:
    //! Is item locked
    bool m_locked;

    //! Size of grid cell
    int m_gridSize;

    //! offset X at grid edge
    int m_gridOffsetX;
    //! offset Y at grid edge
    int m_gridOffsetY;

    //! Offset X of image render
    int m_imgOffsetX;
    //! Offset Y of image render
    int m_imgOffsetY;

    //! Index of animator
    long    m_animatorID;
    //! Size of picture to render
    QRectF  m_imageSize;

    //! Left mouse key is busy
    bool m_mouseLeftPressed;
    //! Middle mouse key is busy
    bool m_mouseMidPressed;
    //! Right mouse key is busy
    bool m_mouseRightPressed;
    /*!
     * \brief Mouse press event
     * \param mouseEvent Mouse evernt preferences
     */
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    /*!
     * \brief Mouse release event
     * \param mouseEvent Mouse evernt preferences
     */
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);
    //! Pointer to parent scene
    WldScene * m_scene;
};

#endif // WLD_BASE_ITEM_H
