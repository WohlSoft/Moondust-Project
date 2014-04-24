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


class ItemBlock : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    ItemBlock(QGraphicsPixmapItem *parent=0);
    ~ItemBlock();

    void setMainPixmap(const QPixmap &pixmap);
    void setContextMenu(QMenu &menu);

    QRectF boundingRect() const;

    QPixmap mainImage;
    QMenu *ItemMenu;
//    QGraphicsScene * scene;
//    QGraphicsPixmapItem * image;

    //////Animation////////
    void setAnimation(QPixmap &sprite, int frames);
    void draw();
    QPoint fPos() const;
    void setFrame(int);

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );

private slots:
    void nextFrame();

private:
    bool animated;
    QPixmap farameSet;
    int frameCurrent;
    QTimer * timer;
    QPoint framePos;
    int framesQ;
    int frameSize; // size of one frame
    int frameWidth; // sprite width
    int frameHeight; //sprite height
    QPixmap currentImage;
};

#endif // ITEM_BLOCK_H
