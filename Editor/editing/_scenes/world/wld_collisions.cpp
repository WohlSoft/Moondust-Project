/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <editing/edit_level/level_edit.h>

#include "wld_scene.h"
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"


///
/// \brief cleanCollisionBuffer
/// Remove trash from collision buffer for crash protection
void WldScene::prepareCollisionBuffer()
{
    for(int i = 0; i < collisionCheckBuffer.size(); i++)
    {
        bool removeIt = false;
        if(collisionCheckBuffer[i] == nullptr)
            removeIt = true;
        else if(collisionCheckBuffer[i]->data(ITEM_IS_ITEM).isNull())
            removeIt = true;

        if(removeIt)
            collisionCheckBuffer.removeAt(i--);
    }
}

QGraphicsItem *WldScene::itemCollidesWith(const QGraphicsItem *item, PGE_ItemList *itemgrp, PGE_ItemList *allCollisions)
{
    qreal leftA, leftB;
    qreal rightA, rightB;
    qreal topA, topB;
    qreal bottomA, bottomB;

    int objType1, objType2;
    unsigned long itemId1, itemId2;

    if(item == nullptr)
        return nullptr;

    objType1 = item->data(ITEM_TYPE_INT).toInt();
    itemId1 = (unsigned long)item->data(ITEM_ID).toULongLong();

    if(objType1 == ItemTypes::META_YellowRect)
        return nullptr;

    QList<QGraphicsItem *> collisions;

    if(itemgrp)
        collisions = *itemgrp;
    else
    {
        QRectF findZoneR(item->scenePos().x() - 10, item->scenePos().y() - 10,
                         item->data(ITEM_WIDTH).toReal() + 20, item->data(ITEM_HEIGHT).toReal() + 20);
        queryItems(findZoneR, &collisions);
    }

    if(collisions.isEmpty())
        return nullptr;

    //collidingItems(item, Qt::IntersectsItemBoundingRect);
    foreach(auto *it, collisions)
    {
        if(it == item)
            continue;
        if(it == nullptr)
            continue;

        if(!it->isVisible()) continue;

        if(it->data(ITEM_TYPE_INT).isNull())
            continue;
        if(it->data(ITEM_IS_ITEM).isNull() || !it->data(ITEM_IS_ITEM).toBool())
            continue;

        objType2 = it->data(ITEM_TYPE_INT).toInt();
        itemId2 = (unsigned long)it->data(ITEM_ID).toULongLong();

        if(objType1 != objType2)
            continue;

        if(objType1 == ItemTypes::WLD_Scenery && itemId1 != itemId2)
            continue;

        leftA = item->scenePos().x();
        rightA = item->scenePos().x() + item->data(ITEM_WIDTH).toReal();
        topA = item->scenePos().y();
        bottomA = item->scenePos().y() + item->data(ITEM_HEIGHT).toReal();

        leftB = it->scenePos().x();
        rightB = it->scenePos().x() + it->data(ITEM_WIDTH).toReal();
        topB = it->scenePos().y();
        bottomB = it->scenePos().y() + it->data(ITEM_HEIGHT).toReal();

        if(bottomA <= topB)
            continue;
        if(topA >= bottomB)
            continue;
        if(rightA <= leftB)
            continue;
        if(leftA >= rightB)
            continue;

        // Collision found!
        if(allCollisions)
            allCollisions->push_back(it); // Get all found items
        else
            return it; // First item only
    }

    return nullptr;
}

QGraphicsItem *WldScene::itemCollidesCursor(const QGraphicsItem *item)
{
    PGE_ItemList collisions;
    double x = item->scenePos().x();
    double y = item->scenePos().y();
    double w = item->data(ITEM_WIDTH).toReal();
    double h = item->data(ITEM_HEIGHT).toReal();

    if(w < 8.0)
    {
        x -= 4;
        w += 8;
    }

    if(h < 8.0)
    {
        y -= 4;
        h += 8;
    }

    QRectF findZoneR(x, y, w, h);
    queryItems(findZoneR, &collisions);

    foreach(auto *it, collisions)
    {
        if(it == item || !it->isVisible())
            continue;

        int type = it->data(ITEM_TYPE_INT).toInt();

        if(type == ItemTypes::WLD_Tile)
        {
            if(m_lockTerrain || dynamic_cast<ItemTile *>(it)->isLocked())
                continue;
        }
        else if(type == ItemTypes::WLD_Scenery)
        {
            if(m_lockScenery || dynamic_cast<ItemScene *>(it)->isLocked())
                continue;
        }
        else if(type == ItemTypes::WLD_Path)
        {
            if(m_lockPath || dynamic_cast<ItemPath *>(it)->isLocked())
                continue;
        }
        else if(type == ItemTypes::WLD_Level)
        {
            if(m_lockLevel || dynamic_cast<ItemLevel *>(it)->isLocked())
                continue;
        }
        else if(type == ItemTypes::WLD_MusicBox)
        {
            if(m_lockMusicBox || dynamic_cast<ItemMusic *>(it)->isLocked())
                continue;
        }

        if(
            (type == ItemTypes::WLD_Tile) || (type == ItemTypes::WLD_Scenery) ||
            (type == ItemTypes::WLD_Path) || (type == ItemTypes::WLD_Level) ||
            (type == ItemTypes::WLD_MusicBox)
        )
            return it;
    }

    return nullptr;
}
