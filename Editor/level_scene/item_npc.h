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

#ifndef ITEM_NPC_H
#define ITEM_NPC_H

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

class ItemNPC : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    ItemNPC(bool noScene=false, QGraphicsPixmapItem *parent=0);
    ~ItemNPC();

    void setMainPixmap(const QPixmap &pixmap);
    void setNpcData(LevelNPC inD);
    void setContextMenu(QMenu &menu);
    void setScenePoint(LvlScene *theScene=NULL);

    QRectF boundingRect() const;

    QPixmap mainImage;

    QMenu *ItemMenu;
//    QGraphicsScene * scene;
//    QGraphicsPixmapItem * image;

    //////Animation////////
    void setAnimation(int frames, int framespeed, int framestyle, int direct,
               bool customAnimate=false, int frFL=0, int frEL=-1, int frFR=0, int frER=-1, bool edit=false);
    void AnimationStart();
    void AnimationStop();
    void draw();
    void setFriendly(bool fri);
    void setNoMovable(bool stat);
    void setLegacyBoss(bool boss);
    void setMsg(QString message);
    void setIncludedNPC(int npcID, bool init=false);
    void setGenerator(bool enable, int direction=1, int type=1, bool init=false);
    void setLayer(QString layer);

    void changeDirection(int dir);

    void arrayApply();
    void removeFromArray();

    QPoint fPos() const;
    void setFrame(int);
    LevelNPC npcData;
    obj_npc localProps;

    int imgOffsetX;
    int imgOffsetY;
    int gridSize;
    bool no_npc_collions;

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

    bool DisableScene;

    QGraphicsItemGroup * grp;
    QGraphicsItem * includedNPC;
    QGraphicsPixmapItem * generatorArrow;


    QVector<QPixmap> frames; //Whole image
    void createAnimationFrames();
    int CurrentFrame;


    bool animated;
    int frameSpeed;
    int frameStyle;
    int direction;

    bool aniDirect;
    bool aniBiDirect;

    int curDirect;
    int frameStep;

    bool customAnimate;
    int customAniAlg; //custom animation algorythm 0 - forward, 1 - frameJump
    int custom_frameFL;//first left
    int custom_frameEL;//end left / jump step
    int custom_frameFR;//first right
    int custom_frameER;//enf right / jump step
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

#endif // ITEM_NPC_H
