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
#ifndef ItemTile_H
#define ItemTile_H

#include <PGE_File_Formats/wld_filedata.h>

#include "../wld_scene.h"
#include "wld_base_item.h"

class ItemTile : public WldBaseItem
{
    void construct();

    static inline QString tr(const char *sourceText, const char *disambiguation = nullptr, int n = -1)
    {
        return QCoreApplication::translate("ItemTile", sourceText, disambiguation, n);
    }

public:
    ItemTile(QGraphicsItem *parent = nullptr);
    ItemTile(WldScene *parentScene, QGraphicsItem *parent = nullptr);
    virtual ~ItemTile();

    void setTileData(WorldTerrainTile inD, obj_w_tile *mergedSet = nullptr, long *animator_id = nullptr);

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

    WorldTerrainTile m_data;
    obj_w_tile m_localProps;

    bool itemTypeIsLocked();

    void contextMenu(QGraphicsSceneMouseEvent *mouseEvent);
};

#endif // ItemTile_H
