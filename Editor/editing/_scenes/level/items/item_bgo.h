/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef ITEM_BGO_H
#define ITEM_BGO_H

#include <PGE_File_Formats/lvl_filedata.h>

#include "../lvl_scene.h"
#include "lvl_base_item.h"

class ItemBGO : public LvlBaseItem
{
    Q_OBJECT
    void construct();
public:
    ItemBGO(QGraphicsItem *parent = nullptr);
    ItemBGO(LvlScene *parentScene, QGraphicsItem *parent = nullptr);
    ~ItemBGO();

    void setBGOData(LevelBGO inD, obj_bgo *mergedSet = nullptr, long *animator_id = nullptr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //////Animation////////
    void setAnimator(long aniID);

    void setLayer(QString layer);
    void setZMode(int mode, qreal offset, bool init = false);

    void transformTo(long target_id);

    void arrayApply();
    void removeFromArray();

    void    returnBack();
    QPoint  sourcePos();

    LevelBGO    m_data;
    obj_bgo     m_localProps;

    bool        m_zValueOverride = false;
    qreal       m_zValue = 0.0;
    int         m_zLayer = 0;
    qreal       m_zOffset = 0.0;

    QString getLayerName();
    bool itemTypeIsLocked();
    void contextMenu(QGraphicsSceneMouseEvent *event);

};

#endif // ITEM_BGO_H
