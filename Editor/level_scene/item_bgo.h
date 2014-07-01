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

#ifndef ITEM_BGO_H
#define ITEM_BGO_H

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

class ItemBGO : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    ItemBGO(QGraphicsItem *parent=0);
    ~ItemBGO();

    void setMainPixmap(const QPixmap &pixmap);
    void setBGOData(LevelBGO inD);
    void setContextMenu(QMenu &menu);
    void setScenePoint(LvlScene *theScene);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

//    QPixmap mainImage;
    QMenu *ItemMenu;
//    QGraphicsScene * scene;
//    QGraphicsPixmapItem * image;

    //////Animation////////
    void setAnimation(int frames, int framespeed);
    void setAnimator(long aniID);
    void AnimationStart();
    void AnimationStop();
    void draw();

    void setLayer(QString layer);

    void arrayApply();
    void removeFromArray();

    QPoint fPos() const;
    void setFrame(int);
    LevelBGO bgoData;

    int gridSize;
    int gridOffsetX;
    int gridOffsetY;

    //Locks
    bool isLocked;
    void setLocked(bool lock);

//public slots:
    //void refresh();

protected:
    virtual void contextMenuEvent( QGraphicsSceneContextMenuEvent * event );
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent );
    //virtual void mouseReleaseEvent( QGraphicsSceneMouseEvent * event);

private slots:
    //void nextFrame();

private:
    long animatorID;
    QRectF imageSize;

    bool animated;
//    int frameSpeed;
    LvlScene * scene;
//    int frameCurrent;
//    QTimer * timer;
//    QPoint framePos;
//    int framesQ;
//    int frameSize; // size of one frame
//    int frameWidth; // sprite width
//    int frameHeight; //sprite height
//    QPixmap currentImage;

//    //Animation alhorithm
//    int frameFirst;
//    int frameLast;

};

#endif // ITEM_BGO_H
