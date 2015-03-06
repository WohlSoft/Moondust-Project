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
#ifndef ITEM_MUSIC_H
#define ITEM_MUSIC_H

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
#include <QMenu>
#include <math.h>

#include <PGE_File_Formats/wld_filedata.h>

#include "../wld_scene.h"
#include "wld_base_item.h"

class ItemMusic : public QObject,
                  public QGraphicsItem,
                  public WldBaseItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    ItemMusic(QGraphicsItem *parent=0);
    ItemMusic(WldScene *parentScene, QGraphicsItem *parent=0);
    ~ItemMusic();
private:
    void construct();
public:

    void setMusicData(WorldMusic inD);
    void setScenePoint(WldScene *theScene);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    int getGridSize();
    QPoint sourcePos();

    WorldMusic musicData;
    QString musicTitle;

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


#endif // ITEM_MUSIC_H
