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

#include "lvl_scene.h"
#include "items/item_block.h"
#include "items/item_bgo.h"
#include "items/item_npc.h"
#include "items/item_water.h"
#include "items/item_door.h"

#ifdef _DEBUG_
#include "../../../common_features/logger.h"
#endif


///
/// \brief cleanCollisionBuffer
/// Remove trash from collision buffer for crash protection
void LvlScene::prepareCollisionBuffer()
{
    for(int i = 0; i < collisionCheckBuffer.size(); i++)
    {
        bool removeIt = false;
        auto &it = collisionCheckBuffer[i];

        if(it == nullptr)
            removeIt = true;
        else if(it->data(ITEM_IS_ITEM).isNull())
            removeIt = true;
        else if(it->data(ITEM_TYPE_INT).toInt() == ItemTypes::LVL_Player)
            removeIt = true;

        if(removeIt)
            collisionCheckBuffer.removeAt(i--);
    }
}

QGraphicsItem *LvlScene::itemCollidesWith(const QGraphicsItem *item, PGE_ItemList *itemgrp, PGE_ItemList *allCollisions)
{
    qreal leftA, leftB;
    qreal rightA, rightB;
    qreal topA, topB;
    qreal bottomA, bottomB;

    int objType1, objType2, shape1, shape2;
    bool sizable1, sizable2, collNoNpc1, collNoNpc2, collNoBlock1, collNoBlock2;
    unsigned long itemId1, itemId2;

    if(item == nullptr)
        return nullptr;

    objType1 = item->data(ITEM_TYPE_INT).toInt();

    if(objType1 == ItemTypes::META_YellowRect ||
       objType1 == ItemTypes::LVL_PhysEnv ||
       objType1 == ItemTypes::LVL_META_DoorEnter ||
       objType1 == ItemTypes::LVL_META_DoorExit)
        return nullptr;

    shape1 = item->data(ITEM_BLOCK_SHAPE).toInt();
    sizable1 = item->data(ITEM_BLOCK_IS_SIZABLE).toBool();
    itemId1 = (unsigned long)item->data(ITEM_ID).toULongLong();
    collNoBlock1 = !item->data(ITEM_NPC_BLOCK_COLLISION).toBool();
    collNoNpc1 = item->data(ITEM_NPC_NO_NPC_COLLISION).toBool();


    PGE_ItemList collisions;

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

    foreach(auto *it, collisions)
    {
        if(it == item)
            continue;
        if(it == nullptr)
            continue;
        if(!it->isVisible())
            continue;
        if(it->data(ITEM_TYPE_INT).isNull())
            continue;
        if(it->data(ITEM_IS_ITEM).isNull() || !it->data(ITEM_IS_ITEM).toBool())
            continue;

        objType2 = it->data(ITEM_TYPE_INT).toInt();
        shape2 = it->data(ITEM_BLOCK_SHAPE).toInt();
        sizable2 = it->data(ITEM_BLOCK_IS_SIZABLE).toBool();
        itemId2 = (unsigned long)it->data(ITEM_ID).toULongLong();
        collNoBlock2 = !it->data(ITEM_NPC_BLOCK_COLLISION).toBool();
        collNoNpc2 = it->data(ITEM_NPC_NO_NPC_COLLISION).toBool();

        // if(objType2 == ItemTypes::LVL_Player)
        //     continue; //

        if((objType2 != ItemTypes::LVL_Block) &&
           (objType2 != ItemTypes::LVL_BGO) &&
           (objType2 != ItemTypes::LVL_NPC))
           continue;

        if(objType1 == ItemTypes::LVL_NPC)
        {
            if(collNoNpc1)   // Disabled collisions with other NPCs
            {
                if(itemId1 != itemId2)
                    continue;
            }
            else
            {
                if(
                    (objType2 == ItemTypes::LVL_Block && collNoBlock1)  //BlockCollision
                    ||
                    (
                        (objType2 == ItemTypes::LVL_Block) //Don't collide NPC's with triangle blocks
                        &&
                        (
                            (shape2 == (int)BlockSetup::SHAPE_triangle_right_top) ||
                            (shape2 == (int)BlockSetup::SHAPE_triangle_left_bottom) ||
                            (shape2 == (int)BlockSetup::SHAPE_triangle_right_bottom) ||
                            (shape2 == (int)BlockSetup::SHAPE_triangle_left_top)
                        )
                    )
                    ||
                    (objType2 == ItemTypes::LVL_NPC && collNoNpc2) //NpcCollision
                    ||
                    ((objType2 != ItemTypes::LVL_NPC) && (objType2 != ItemTypes::LVL_Block))
                )
                    continue;
            }

        }
        else if(objType1 == ItemTypes::LVL_Block)
        {
            if(
                (objType2 == ItemTypes::LVL_NPC && collNoBlock2) //BlockCollision
                ||
                (objType2 != ItemTypes::LVL_NPC && objType2 != ItemTypes::LVL_Block) //Don't collide non-block and non-NPC items
                ||
                (
                    (objType2 == ItemTypes::LVL_NPC) //Don't collide NPC's with triangle blocks
                    &&
                    (
                        (shape1 == (int)BlockSetup::SHAPE_triangle_right_top) ||
                        (shape1 == (int)BlockSetup::SHAPE_triangle_left_top) ||
                        (shape1 == (int)BlockSetup::SHAPE_triangle_right_bottom) ||
                        (shape1 == (int)BlockSetup::SHAPE_triangle_left_bottom)
                    )
                )
            )
                continue;

        }
        else if(objType1 != objType2)
            continue;

        if(sizable1 || sizable2)
        {
            // Don't collide with sizable block
#ifdef _DEBUG_
            LogDebug(QString("sizable block"));
#endif
            continue;
        }

        if(objType1 == ItemTypes::LVL_BGO)
        {
            if(m_editMode != MODE_Fill && itemId1 != itemId2)
                continue;
        }

        leftA = item->scenePos().x();
        rightA = item->scenePos().x() + item->data(ITEM_WIDTH).toReal();
        topA = item->scenePos().y();
        bottomA = item->scenePos().y() + item->data(ITEM_HEIGHT).toReal();

        leftB = it->scenePos().x();
        rightB = it->scenePos().x() + it->data(ITEM_WIDTH).toReal();
        topB = it->scenePos().y();
        bottomB = it->scenePos().y() + it->data(ITEM_HEIGHT).toReal();

#ifdef _DEBUG_
        LogDebug(QString("Collision check -> Item1 L%1 | T%2 | R%3 |  B%4")
                 .arg(leftA).arg(topA).arg(rightA).arg(bottomA));
        LogDebug(QString("Collision check -> Item2 R%3 | B%4 | L%1 |  T%2")
                 .arg(leftB).arg(topB).arg(rightB).arg(bottomB));

        LogDebug(QString("Collision check -> Item1 W%1, H%2")
                 .arg(item->data(ITEM_WIDTH).toReal()).arg(item->data(ITEM_HEIGHT).toReal()));
        LogDebug(QString("Collision check -> Item2 W%1, H%2")
                 .arg(it->data(ITEM_WIDTH).toReal()).arg(it->data(ITEM_HEIGHT).toReal()));

        LogDebug(QString("Collision check -> B%1 <= T%2 -> %3")
                 .arg(bottomA).arg(topB).arg(bottomA <= topB));
        LogDebug(QString("Collision check -> T%1 >= B%2 -> %3")
                 .arg(topA).arg(bottomB).arg(topA >= bottomB));
        LogDebug(QString("Collision check -> R%1 <= L%2 -> %3")
                 .arg(rightA).arg(leftB).arg(rightA <= leftB));
        LogDebug(QString("Collision check -> L%1 >= R%2 -> %3")
                 .arg(leftA).arg(rightB).arg(leftA >= rightB));

        //Collision check -> Item1 L-199776 | T-200288 | R-199744 |  B-200256
        //Collision check -> Item2 R-199744 | B-200288 | L-199776 |  T-200320
#endif

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

QGraphicsItem *LvlScene::itemCollidesCursor(const QGraphicsItem *item)
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

        //skip locked items
        if(type == ItemTypes::LVL_Block)
        {
            if(m_lockBlock || dynamic_cast<ItemBlock *>(it)->m_locked)
                continue;
        }
        else if(type == ItemTypes::LVL_BGO)
        {
            if(m_lockBgo || dynamic_cast<ItemBGO *>(it)->m_locked)
                continue;
        }
        else if(type == ItemTypes::LVL_NPC)
        {
            if(m_lockNpc || dynamic_cast<ItemNPC *>(it)->m_locked)
                continue;
        }
        else if(type == ItemTypes::LVL_PhysEnv)
        {
            if(m_lockPhysenv || dynamic_cast<ItemPhysEnv *>(it)->m_locked)
                continue;
        }
        else if(type == ItemTypes::LVL_META_DoorEnter || type == ItemTypes::LVL_META_DoorExit)
        {
            if(m_lockDoor || dynamic_cast<ItemDoor *>(it)->m_locked)
                continue;
        }

        if(
            (type == ItemTypes::LVL_Block) ||
            (type == ItemTypes::LVL_BGO) ||
            (type == ItemTypes::LVL_NPC) ||
            (type == ItemTypes::LVL_META_DoorEnter) ||
            (type == ItemTypes::LVL_META_DoorExit) ||
            (type == ItemTypes::LVL_PhysEnv) ||
            (type == ItemTypes::LVL_Player)
        )
            return it;
    }
    return nullptr;
}
