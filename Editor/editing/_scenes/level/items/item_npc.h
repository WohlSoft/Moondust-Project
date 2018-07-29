/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef ITEM_NPC_H
#define ITEM_NPC_H

#include <PGE_File_Formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemNPC : public LvlBaseItem
{
    Q_OBJECT
    void construct();
public:
    ItemNPC(LvlScene *parentScene, QGraphicsItem *parent=0);
    ItemNPC(bool noScene=false, QGraphicsItem *parent=0);
    ~ItemNPC();

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
    QPoint sourcePos();

    LevelNPC    m_data;
    obj_npc     m_localProps;

    int m_imgOffsetX;
    int m_imgOffsetY;
    int m_offset_x;
    int m_offset_y;

    void refreshOffsets();

    void setAnimator(long aniID);

    bool itemTypeIsLocked();
    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

    void setMetaSignsVisibility(bool visible);

private:
    bool    m_extAnimator;
    long    m_animatorID;
    QRectF  m_imageSize;

    bool    m_DisableScene;

    QGraphicsItemGroup *    m_grp;
    QGraphicsItem *         m_includedNPC;
    QGraphicsPixmapItem *   m_generatorArrow;

    QRectF  m_offseted;
    bool    m_animated;
    int     m_direction;
};

#endif // ITEM_NPC_H
