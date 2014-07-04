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

class ItemBlock : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    ItemBlock(QGraphicsItem *parent=0);
    ~ItemBlock();

    void setMainPixmap(/*const QPixmap &pixmap*/);
    void setBlockData(LevelBlock inD, bool is_sz);
    void setContextMenu(QMenu &menu);

    void setScenePoint(LvlScene *theScene);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //////Animation////////
    void setAnimator(long aniID);

    QMenu *ItemMenu;

    void setSlippery(bool slip);
    void setInvisible(bool inv);
    void setLayer(QString layer);
    void setBlockSize(QRect rect);

    void setIncludedNPC(int npcID, bool init=false);

    void arrayApply();
    void removeFromArray();

    LevelBlock blockData;
    int gridSize;

    //Locks
    bool isLocked;
    void setLocked(bool lock);

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent);

private:

    long animatorID;
    QRectF imageSize;

    bool animated;


    QGraphicsItemGroup * grp;
    QGraphicsItem * includedNPC;
    QPixmap currentImage;
    bool sizable;
    LvlScene * scene;
    QPixmap drawSizableBlock(int w, int h, QPixmap srcimg);
};

#endif // ITEM_BLOCK_H
