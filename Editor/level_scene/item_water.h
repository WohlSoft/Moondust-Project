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
#include <math.h>
#include <QMenu>

#include "lvlscene.h"
#include "../file_formats/lvl_filedata.h"

class ItemWater : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    ItemWater(QGraphicsPolygonItem *parent=0);
    ~ItemWater();

    void setSize(QSize sz);
    void setType(int tp);
    void setWaterData(LevelWater inD);

    void setContextMenu(QMenu &menu);
    void setScenePoint(LvlScene *theScene);

    QRectF boundingRect() const;

    QMenu *ItemMenu;
//    QGraphicsScene * scene;
//    QGraphicsPolygonItem * image;

    void setLayer(QString layer);

    void arrayApply();
    void removeFromArray();

    LevelWater waterData;

    int gridSize;
    int gridOffsetX;
    int gridOffsetY;
    QSize waterSize;

    //Locks
    bool isLocked;
    void setLocked(bool lock);

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    //virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event);

private:
    LvlScene * scene;

    void drawWater();
};

#endif // ITEM_WATER_H
