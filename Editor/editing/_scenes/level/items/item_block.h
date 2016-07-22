/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef ITEM_BLOCK_H
#define ITEM_BLOCK_H

#include <PGE_File_Formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemBlock : public LvlBaseItem
{
    Q_OBJECT
    void construct();
public:
    ItemBlock(LvlScene *parentScene, QGraphicsItem *parent=0);
    ItemBlock(QGraphicsItem *parent=0);
    ~ItemBlock();

    void setMainPixmap(/*const QPixmap &pixmap*/);
    void setBlockData(LevelBlock inD, obj_block *mergedSet=0, long *animator=0);
    void setScenePoint(LvlScene *theScene);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //////Animation////////
    void setAnimator(long aniID);

    //Block specific
    void setSlippery(bool slip);
    void setInvisible(bool inv);
    void setBlockSize(QRect rect);
    void setIncludedNPC(int npcID, bool init=false);

    void transformTo(long target_id);

    //Common
    void setLayer(QString layer);
    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint sourcePos();

    LevelBlock  m_data;
    obj_block   m_localProps;

    bool itemTypeIsLocked();
    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);

    bool isSizable();

private:
    QGraphicsItemGroup * m_grp;
    QGraphicsItem *      m_includedNPC;
    QPixmap              m_currentImage;
    bool                 m_sizable;
    void drawSizableBlock(int w, int h, QPixmap srcimg);
};

#endif // ITEM_BLOCK_H
