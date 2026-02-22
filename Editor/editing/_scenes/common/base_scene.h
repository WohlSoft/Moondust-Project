/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef BASE_SCENE_H
#define BASE_SCENE_H

#include <QSet>
#include <QMultiMap>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QSharedPointer>

#include <common_features/RTree.h>

class MainWindow;
class GraphicsWorkspace;
class EditMode;

class MoondustBaseScene : public QGraphicsScene
{
    Q_OBJECT

    static bool treeSearchCallback(QGraphicsItem *item, void *arg);
    static bool treeSearchCallbackMap(QGraphicsItem *item, void *arg);

public:
    explicit MoondustBaseScene(MainWindow *mw, GraphicsWorkspace *parentView, QObject *parent = nullptr);
    virtual ~MoondustBaseScene();

    typedef QList<QGraphicsItem *> PGE_ItemList;
    typedef QMultiMap<qreal, QGraphicsItem*> PGE_ItemsListSorted;
    typedef RTree<QGraphicsItem *, int64_t, 2, int64_t > IndexTree;
    typedef int64_t RPoint[2];

    /*!
     * \brief Defining indexes for data values of items
     */
    enum ItemDataField
    {
        //! Defining indexes for data values of items
        ITEM_TYPE                  = 0,  //String
        //! ID of item
        ITEM_ID                    = 1,  //int
        //! in-array UID
        ITEM_ARRAY_ID              = 2,  //int
        //! Is it's a block item which is sizable
        ITEM_BLOCK_IS_SIZABLE      = 3,  //bool
        //! Shape type of blocks
        ITEM_BLOCK_SHAPE           = 4,  //int
        //! Is it's NPC which should collide blocks
        ITEM_NPC_BLOCK_COLLISION   = 5,  //bool
        //! NPC that shoudn't collide other NPCs
        ITEM_NPC_NO_NPC_COLLISION  = 6,  //bool
        //! Width of element
        ITEM_WIDTH                 = 7,  //int
        //! Height of element
        ITEM_HEIGHT                = 8, //int
        //! Is it's a scene object, not a meta-data or misc. non-related to items object
        ITEM_IS_ITEM               = 24, //bool
        //! Is it's a cursor object
        ITEM_IS_CURSOR             = 25, //bool
        //! Last remembered position before commit
        ITEM_LAST_POS              = 26, //QPointF
        //! Last remembered size before commit
        ITEM_LAST_SIZE             = 27, //QSizeF
        //! Never seen in game or on exported images
        ITEM_IS_META =               28, //bool
        //! Item type as an integer enum value (see ItemTypes enumeration at defines.h)
        ITEM_TYPE_INT              = 29, //Int
    };

    enum EditModeID
    {
        MODE_Selecting = 0,
        MODE_HandScroll,
        MODE_Erasing,
        MODE_PlacingNew,
        MODE_DrawRect,
        MODE_DrawCircle,
        MODE_PasteFromClip,
        MODE_Resizing,
        MODE_SelectingOnly,
        MODE_Line,
        MODE_SetPoint,
        MODE_Fill
    };

    void switchEditMode(EditModeID EdtMode);

    void setEditFlagEraser(bool en);
    bool getEditFlagEraser() const;

    void setEditFlagPasteMode(bool en);
    bool getEditFlagPasteMode() const;

    void setEditFlagBusyMode(bool en);
    bool getEditFlagBusyMode() const;

    void setEditFlagNoMoveItems(bool en);
    bool getEditFlagNoMoveItems() const;

    EditModeID editMode() const;

    virtual bool allowEditModeSwitch() const;

    MainWindow *mw();
    GraphicsWorkspace *curViewPort();

    QSet<QGraphicsItem*> &allItems();

    void queryItems(const QRectF &zone, PGE_ItemList *resultList);
    void queryItems(double x, double y, PGE_ItemList *resultList);

    /**
     * @brief Query items and keep them zorted by Z-value
     * @param zone Rectangular zone to search
     * @param resultList Multi-Map that contains returned items
     */
    void queryItemsSorted(const QRectF &zone, PGE_ItemsListSorted *resultList);

    bool checkGroupCollisions(const PGE_ItemList &items);
    virtual QGraphicsItem *itemCollidesWith(const QGraphicsItem *item, PGE_ItemList *itemgrp = nullptr, PGE_ItemList *allCollisions = nullptr) = 0;
    virtual QGraphicsItem *itemCollidesCursor(const QGraphicsItem *item) = 0;

    void registerElement(QGraphicsItem *item);
    void updateElement(QGraphicsItem *item);
    void unregisterElement(QGraphicsItem *item);

protected:
    //! Main window pointer
    MainWindow          *m_mw = nullptr;
    //! Pointer to parent graphics view
    GraphicsWorkspace   *m_viewPort = nullptr;
    //! Index tree
    IndexTree tree;
    //! List of all items
    QSet<QGraphicsItem*> m_itemsAll;
    //! Current editing mode
    EditModeID m_editMode = MODE_Selecting;
    //! Store of available edit modes
    QHash<EditModeID, QSharedPointer<EditMode>> m_editModes;
    //! Instance of the current edit mode
    EditMode *m_editModeObj = nullptr;

    // --- Scene related setup ---
    bool m_eraserIsEnabled = false;
    bool m_pastingMode = false;
    //! Placing/drawing on map, disable selecting and dragging items
    bool m_busyMode = false;
    bool m_disableMoveItems = false;
};

#endif
