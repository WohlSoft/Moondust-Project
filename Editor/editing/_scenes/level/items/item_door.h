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

#ifndef ITEM_DOOR_H
#define ITEM_DOOR_H

#include <QGraphicsItem>
#include <QGraphicsRectItem>
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

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemDoor : public QObject,
                 public QGraphicsRectItem,
                 public LvlBaseItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    ItemDoor(QGraphicsRectItem *parent=0);
    ItemDoor(LvlScene *parentScene, QGraphicsRectItem *parent=0);
    ~ItemDoor();
private:
    void construct();
public:

    void setDoorData(LevelDoors inD, int doorDir, bool init=false);
    void setScenePoint(LvlScene *theScene);

    void setLocked(bool lock);

    int direction;
    enum doorDirect{
        D_Entrance=0,
        D_Exit
    };

    QRectF boundingRect() const;

    void setLayer(QString layer);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint gridOffset();
    int getGridSize();
    QPoint sourcePos();

    LevelDoors doorData;

    int gridSize;
    int gridOffsetX;
    int gridOffsetY;
    QSize itemSize;

    //Locks
    bool isLocked;

protected:
    bool mouseLeft;
    bool mouseMid;
    bool mouseRight;
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event);

private:
    LvlScene * scene;

    QGraphicsItemGroup * grp;
    QGraphicsPixmapItem * doorLabel;
    //QGraphicsTextItem * doorLabel_shadow;

};

#endif // ITEM_DOOR_H
