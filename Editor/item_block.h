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

#ifndef ITEM_BLOCK_H
#define ITEM_BLOCK_H

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
#include "lvlscene.h"
#include "lvl_filedata.h"

class ItemBlock : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    ItemBlock(QGraphicsPixmapItem *parent=0);
    ~ItemBlock();

    void setMainPixmap(const QPixmap &pixmap);
    void setBlockData(LevelBlock inD, bool is_sz);
    void setContextMenu(QMenu &menu);
    void setScenePoint(LvlScene *theScene);

    QRectF boundingRect() const;

    QPixmap mainImage;
    QMenu *ItemMenu;
//    QGraphicsScene * scene;
//    QGraphicsPixmapItem * image;

    //////Animation////////
    void setAnimation(int frames, int framespeed, int algorithm);
    void AnimationStart();
    void AnimationStop();
    void draw();
    void setSlippery(bool slip);
    void setInvisible(bool inv);

    void arrayApply();
    void removeFromArray();

    QPoint fPos() const;
    void setFrame(int);
    LevelBlock blockData;

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    //virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event);

private slots:
    void nextFrame();

private:
    bool animated;
    int frameSpeed;

    QPixmap drawSizebleBlock(int w, int h, QPixmap srcimg);
    bool sizeble;
    LvlScene * scene;
    int frameCurrent;
    QTimer * timer;
    QPoint framePos;
    int framesQ;
    int frameSize; // size of one frame
    int frameWidth; // sprite width
    int frameHeight; //sprite height
    QPixmap currentImage;

    //Animation alhorithm
    int frameFirst;
    int frameLast;

};

#endif // ITEM_BLOCK_H
