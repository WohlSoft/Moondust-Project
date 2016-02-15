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
#include <QMenu>
#include <math.h>

#include <PGE_File_Formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemNPC : public LvlBaseItem
{
public:
    ItemNPC(LvlScene *parentScene, QGraphicsItem *parent=0);
    ItemNPC(bool noScene=false, QGraphicsItem *parent=0);
    ~ItemNPC();
private:
    void construct();

public:
    void setMainPixmap(const QPixmap &pixmap);
    void setNpcData(LevelNPC inD, obj_npc *mergedSet=0, long *animator_id=0);
    void setContextMenu(QMenu &menu);
    void setScenePoint(LvlScene *theScene=NULL);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *);

    AdvNpcAnimator* _internal_animator;

    void setFriendly(bool fri);
    void setNoMovable(bool stat);
    void setLegacyBoss(bool boss);
    void setMsg(QString message);
    void setIncludedNPC(int npcID, bool init=false);
    void setGenerator(bool enable, int direction=1, int type=1, bool init=false);
    void setLayer(QString layer);

    void changeDirection(int dir);

    void transformTo(long target_id);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint gridOffset();
    int getGridSize();
    QPoint sourcePos();

    void setFrame(int);
    LevelNPC npcData;
    obj_npc localProps;

    int imgOffsetX;
    int imgOffsetY;
    bool no_npc_collions;
    int _offset_x;
    int _offset_y;

    void refreshOffsets();

    void setAnimator(long aniID);

    bool itemTypeIsLocked();
    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

private:

    bool extAnimator;
    long animatorID;
    QRectF imageSize;

    bool DisableScene;

    QGraphicsItemGroup * grp;
    QGraphicsItem * includedNPC;
    QGraphicsPixmapItem * generatorArrow;

    QRectF offseted;

    bool animated;
    int direction;

    int curDirect;

};

#endif // ITEM_NPC_H
