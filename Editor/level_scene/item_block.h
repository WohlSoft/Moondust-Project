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
#include "../file_formats/lvl_filedata.h"

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
    //void setGroupPoint(QGraphicsItemGroup *theGrp);
    //void setNPCItemPoint(QGraphicsItem *includedNPCPnt);

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
    void setLayer(QString layer);
    void setBlockSize(QRect rect);

    void setIncludedNPC(int npcID, bool init=false);

    void arrayApply();
    void removeFromArray();

    QPoint fPos() const;
    void setFrame(int);
    LevelBlock blockData;
    int gridSize;

    //Locks
    bool isLocked;
    void setLocked(bool lock);

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    //virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

private slots:
    void nextFrame();

private:
    bool animated;
    int frameSpeed;

    QVector<QPixmap> frames; //Animation frames

    void createAnimationFrames();
    int CurrentFrame;

    QPixmap drawSizableBlock(int w, int h, QPixmap srcimg);
    QGraphicsItemGroup * grp;
    QGraphicsItem * includedNPC;

    bool sizable;
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
