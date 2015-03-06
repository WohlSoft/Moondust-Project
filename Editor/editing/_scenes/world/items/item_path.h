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

#ifndef ItemPath_H
#define ItemPath_H

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
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

#include <PGE_File_Formats/wld_filedata.h>

#include "../wld_scene.h"
#include "wld_base_item.h"

class ItemPath : public QObject,
                 public QGraphicsItem,
                 public WldBaseItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    ItemPath(QGraphicsItem *parent=0);
    ItemPath(WldScene *parentScene, QGraphicsItem *parent=0);
    ~ItemPath();
private:
    void construct();
public:

    void setPathData(WorldPaths inD, obj_w_path *mergedSet=0, long *animator_id=0);
    void setScenePoint(WldScene *theScene);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //////Animation////////
    void setAnimator(long aniID);

    //void setLayer(QString layer);

    void transformTo(long target_id);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    int getGridSize();
    QPoint sourcePos();

    WorldPaths pathData;
    obj_w_path localProps;

    int gridSize;
    int gridOffsetX;
    int gridOffsetY;

    //Locks
    bool isLocked;
    void setLocked(bool lock);

protected:
    bool mouseLeft;
    bool mouseMid;
    bool mouseRight;
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:
    long animatorID;
    QRectF imageSize;

    bool animated;
    WldScene * scene;

};

#endif // ItemPath_H
