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
#ifndef ITEM_DOOR_H
#define ITEM_DOOR_H

#include <PGE_File_Formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemDoor : public LvlBaseItem
{
    Q_OBJECT
    void construct();
public:
    ItemDoor(QGraphicsItem *parent = nullptr);
    ItemDoor(LvlScene *parentScene, QGraphicsItem *parent = nullptr);
    ~ItemDoor();

    void setDoorData(LevelDoor inD, int doorDir, bool init = false);
    void setScenePoint(LvlScene *theScene);

    int m_pointSide;
    enum doorDirect
    {
        D_Entrance = 0,
        D_Exit
    };

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    void setLayer(QString layer);

    void refreshArrows();

    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint sourcePos();

    LevelDoor   m_data;
    QSize       m_itemSize;

    QString getLayerName();
    bool itemTypeIsLocked();
    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

private:
    QBrush m_brush;
    QPen   m_pen;
    QGraphicsItemGroup     *m_grp = nullptr;
    QGraphicsPixmapItem    *m_doorLabel = nullptr;
    QGraphicsPixmapItem    *m_arrowEnter = nullptr;
    QGraphicsPixmapItem    *m_arrowExit = nullptr;
};

#endif // ITEM_DOOR_H
