/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef ItemScene_H
#define ItemScene_H

#include <PGE_File_Formats/wld_filedata.h>

#include "../wld_scene.h"
#include "wld_base_item.h"

class ItemScene : public WldBaseItem
{
    Q_OBJECT
    void construct();
public:
    ItemScene(QGraphicsItem *parent = nullptr);
    ItemScene(WldScene *parentScene, QGraphicsItem *parent = nullptr);
    virtual ~ItemScene();

    void setSceneData(WorldScenery inD, obj_w_scenery *mergedSet = nullptr, long *animator_id = nullptr);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    //////Animation////////
    void setAnimator(long aniID);

    //void setLayer(QString layer);

    void transformTo(long target_id);

    void arrayApply();
    void removeFromArray();

    void returnBack();
    QPoint sourcePos();

    WorldScenery  m_data;
    obj_w_scenery m_localProps;

    bool itemTypeIsLocked();
    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);
};

#endif // ItemScene_H
