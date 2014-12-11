/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <file_formats/lvl_filedata.h>

#include "lvl_scene.h"

class ItemWater : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    ItemWater(QGraphicsPolygonItem *parent=0);
    ~ItemWater();

    void setSize(QSize sz);
    void setRectSize(QRect rect);

    void setType(int tp);
    void setWaterData(LevelPhysEnv inD);

    void setContextMenu(QMenu &menu);
    void setScenePoint(LvlScene *theScene);

    void setLocked(bool lock);

    void drawWater();

    QRectF boundingRect() const;

    QMenu *ItemMenu;
//    QGraphicsScene * scene;
//    QGraphicsPolygonItem * image;

    void setLayer(QString layer);

    void arrayApply();
    void removeFromArray();

    LevelPhysEnv waterData;

    int gridSize;
    int gridOffsetX;
    int gridOffsetY;
    QSize waterSize;
    int penWidth;

    QPen _pen;

    //Locks
    bool isLocked;

protected:
    bool mouseLeft;
    bool mouseMid;
    bool mouseRight;
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

private:
    LvlScene * scene;
};

#endif // ITEM_WATER_H
