/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef ITEM_WATER_H
#define ITEM_WATER_H

#include <QGraphicsItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QString>
#include <QPoint>
#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QTimer>
#include <QMenu>
#include <math.h>

#include <PGE_File_Formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemWater : public LvlBaseItem
{
public:
    ItemWater(QGraphicsItem *parent=0);
    ItemWater(LvlScene *parentScene, QGraphicsItem *parent=0);
    ~ItemWater();
private:
    void construct();

public:

    void setSize(QSize sz);
    void setRectSize(QRect rect);

    void setType(int tp);
    void setWaterData(LevelPhysEnv inD);

    void drawWater();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    void setLayer(QString layer);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint gridOffset();
    int getGridSize();
    QPoint sourcePos();

    void updateColor();

    LevelPhysEnv waterData;

    int gridSize;
    int gridOffsetX;
    int gridOffsetY;
    QSize waterSize;
    int penWidth;

    QPen _pen;
    QColor _color;

    bool itemTypeIsLocked();
    void contextMenu( QGraphicsSceneMouseEvent * mouseEvent );
};

#endif // ITEM_WATER_H
