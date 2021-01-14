/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../../../common_features/logger.h"


///
/// \brief cleanCollisionBuffer
/// Remove trash from collision buffer for crash protection
void LvlScene::prepareCollisionBuffer()
{
    for(int i = 0; i < collisionCheckBuffer.size(); i++)
    {
        bool removeIt = false;
        if(collisionCheckBuffer[i] == nullptr)
            removeIt = true;
        else if(collisionCheckBuffer[i]->data(ITEM_IS_ITEM).isNull())
            removeIt = true;
        else if(collisionCheckBuffer[i]->data(ITEM_TYPE).toString() == "playerPoint")
            removeIt = true;

        if(removeIt)
            collisionCheckBuffer.removeAt(i--);
    }
}

//Checking group collisions. Return true if was found even one passed collision in this group
bool LvlScene::checkGroupCollisions(PGE_ItemList *items)
{
    if(!items)
        return false;
    if(items->empty())
        return false;
    if(items->size() == 1)
    {
        LogDebug(QString("Collision check: single item"));
        return (itemCollidesWith(items->first(), nullptr) != nullptr);
    }

    //9 - width, 10 - height
    QRectF findZone = QRectF(items->first()->scenePos(),
                             QSizeF(items->first()->data(ITEM_WIDTH).toInt(),
                                    items->first()->data(ITEM_HEIGHT).toInt()));
    //get Zone
    for(auto *it : *items)
    {
        if(!it) continue;
        if(it->scenePos().x() - 10 < findZone.left())
            findZone.setLeft(it->scenePos().x());
        if(it->scenePos().y() - 10 < findZone.top())
            findZone.setTop(it->scenePos().y());

        if(it->scenePos().x() + it->data(ITEM_WIDTH).toInt() > findZone.right())
            findZone.setRight(it->scenePos().x() + it->data(ITEM_WIDTH).toInt());
        if(it->scenePos().y() + it->data(ITEM_HEIGHT).toInt() > findZone.bottom())
            findZone.setBottom(it->scenePos().y() + it->data(ITEM_HEIGHT).toInt());
    }

    findZone.setLeft(findZone.left() - 10);
    findZone.setRight(findZone.right() + 10);
    findZone.setTop(findZone.top() - 10);
    findZone.setBottom(findZone.bottom() + 10);

    PGE_ItemList CheckZone;
    queryItems(findZone, &CheckZone);
#ifdef _DEBUG_
    LogDebug(QString("Collision check: found items for check %1").arg(CheckZone.size()));
    LogDebug(QString("Collision rect: x%1 y%2 w%3 h%4").arg(findZone.x())
             .arg(findZone.y()).arg(findZone.width()).arg(findZone.height()));
#endif

    //Don't collide with items which in the group
    for(int i = 0; i < CheckZone.size(); i++)
    {
        for(int j = 0; j < (*items).size(); j++)
        {
            if(CheckZone[i] == (*items)[j])
            {
                CheckZone.removeOne(CheckZone[i]);
                i--;
                break;
            }
        }
    }

    for(auto *it : *items)
    {
        if(itemCollidesWith(it, &CheckZone) != nullptr)
            return true;
    }
    return false;

}

QGraphicsItem *LvlScene::itemCollidesWith(QGraphicsItem *item, PGE_ItemList *itemgrp)
{
    qreal leftA, leftB;
    qreal rightA, rightB;
    qreal topA, topB;
    qreal bottomA, bottomB;

    if(item == nullptr)
        return nullptr;

    if(item->data(ITEM_TYPE).toString() == "YellowRectangle")
        return nullptr;
    if(item->data(ITEM_TYPE).toString() == "Water")
        return nullptr;
    if(item->data(ITEM_TYPE).toString() == "Door_exit")
        return nullptr;
    if(item->data(ITEM_TYPE).toString() == "Door_enter")
        return nullptr;

    PGE_ItemList collisions;

    if(itemgrp && !itemgrp->isEmpty())
        collisions = *itemgrp;
    else
    {
        QRectF findZoneR(item->scenePos().x() - 10, item->scenePos().y() - 10,
                         item->data(ITEM_WIDTH).toReal() + 20, item->data(ITEM_HEIGHT).toReal() + 20);
        queryItems(findZoneR, &collisions);
    }

    if(collisions.isEmpty())
        return nullptr;

    for(auto *it : collisions)
    {
        if(it == item)
            continue;
        if(it == nullptr)
            continue;
        if(!it->isVisible())
            continue;
        if(it->data(ITEM_TYPE).isNull())
            continue;
        if(it->data(ITEM_IS_ITEM).isNull())
            continue;
        if(it->data(ITEM_TYPE).toString() == "PlayerPoint")
            continue;

        if(
            (it->data(ITEM_TYPE).toString() != "Block") &&
            (it->data(ITEM_TYPE).toString() != "BGO") &&
            (it->data(ITEM_TYPE).toString() != "NPC")
        ) continue;

        if(item->data(ITEM_TYPE).toString() == "NPC")
        {
            if(item->data(ITEM_NPC_NO_NPC_COLLISION).toBool())   // Disabled collisions with other NPCs
            {
                if(item->data(ITEM_ID).toInt() != it->data(ITEM_ID).toInt()) continue;
            }
            else
            {
                if(
                    (
                        (it->data(ITEM_TYPE).toString() == "Block")
                        && (!item->data(ITEM_NPC_BLOCK_COLLISION).toBool()) //BlockCollision
                    )
                    ||
                    (
                        (it->data(ITEM_TYPE).toString() == "Block") //Don't collide NPC's with triangle blocks
                        &&
                        (
                            (it->data(ITEM_BLOCK_SHAPE).toInt() == (int)BlockSetup::SHAPE_triangle_right_top) ||
                            (it->data(ITEM_BLOCK_SHAPE).toInt() == (int)BlockSetup::SHAPE_triangle_left_bottom) ||
                            (it->data(ITEM_BLOCK_SHAPE).toInt() == (int)BlockSetup::SHAPE_triangle_right_bottom) ||
                            (it->data(ITEM_BLOCK_SHAPE).toInt() == (int)BlockSetup::SHAPE_triangle_left_top)
                        )
                    )
                    ||
                    (
                        (it->data(ITEM_TYPE).toString() == "NPC")
                        && (it->data(ITEM_NPC_NO_NPC_COLLISION).toBool()) //NpcCollision
                    )
                    ||
                    ((it->data(ITEM_TYPE).toString() != "NPC")
                     && (it->data(ITEM_TYPE).toString() != "Block"))
                )
                    continue;
            }

        }
        else if(item->data(ITEM_TYPE).toString() == "Block")
        {
            if(
                (
                    (it->data(ITEM_TYPE).toString() == "NPC")
                    && (!it->data(ITEM_NPC_BLOCK_COLLISION).toBool()) //BlockCollision
                ) ||
                (
                    (it->data(ITEM_TYPE).toString() != "NPC") //Don't collide non-block and non-NPC items
                    && (it->data(ITEM_TYPE).toString() != "Block")
                ) ||
                (
                    (it->data(ITEM_TYPE).toString() == "NPC") //Don't collide NPC's with triangle blocks
                    &&
                    (
                        (item->data(ITEM_BLOCK_SHAPE).toInt() == (int)BlockSetup::SHAPE_triangle_right_top) ||
                        (item->data(ITEM_BLOCK_SHAPE).toInt() == (int)BlockSetup::SHAPE_triangle_left_top) ||
                        (item->data(ITEM_BLOCK_SHAPE).toInt() == (int)BlockSetup::SHAPE_triangle_right_bottom) ||
                        (item->data(ITEM_BLOCK_SHAPE).toInt() == (int)BlockSetup::SHAPE_triangle_left_bottom)
                    )
                )
            )
                continue;

        }
        else if(item->data(ITEM_TYPE).toString() != it->data(ITEM_TYPE).toString()) continue;

        if(item->data(ITEM_BLOCK_IS_SIZABLE).toString() == "sizable")
        {
            // Don't collide with sizable block
#ifdef _DEBUG_
            LogDebug(QString("sizable block"));
#endif
            continue;
        }

        if(it->data(ITEM_BLOCK_IS_SIZABLE).toString() == "sizable") continue; // Don't collide with sizable block

        if(item->data(ITEM_TYPE).toString() == "BGO")
            if((m_editMode != MODE_Fill) && (item->data(ITEM_ID).toInt() != it->data(ITEM_ID).toInt())) continue;

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

        return it; // Collision found!
    }
    return nullptr;
}

QGraphicsItem *LvlScene::itemCollidesCursor(QGraphicsItem *item)
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

    for(auto *it : collisions)
    {
        if(it == item || !it->isVisible())
            continue;

        QString type = it->data(ITEM_TYPE).toString();

        //skip locked items
        if(type == "Block")
        {
            if(m_lockBlock || dynamic_cast<ItemBlock *>(it)->m_locked)
                continue;
        }
        else if(type == "BGO")
        {
            if(m_lockBgo || dynamic_cast<ItemBGO *>(it)->m_locked)
                continue;
        }
        else if(type == "NPC")
        {
            if(m_lockNpc || dynamic_cast<ItemNPC *>(it)->m_locked)
                continue;
        }
        else if(type == "Water")
        {
            if(m_lockPhysenv || dynamic_cast<ItemPhysEnv *>(it)->m_locked)
                continue;
        }
        else if(type == "Door_enter" || type == "Door_exit")
        {
            if(m_lockDoor || dynamic_cast<ItemDoor *>(it)->m_locked)
                continue;
        }

        if(
            (type == "Block") ||
            (type == "BGO") ||
            (type == "NPC") ||
            (type == "Door_exit") ||
            (type == "Door_enter") ||
            (type == "Water") ||
            (type == "player1") ||
            (type == "player2")
        )
            return it;
    }
    return nullptr;
}

namespace LevelScene_space
{
    bool _TreeSearchCallback(QGraphicsItem *item, void *arg)
    {
        LvlScene::PGE_ItemList *list = static_cast<LvlScene::PGE_ItemList * >(arg);
        if(list)
        {
            if(item)
                (*list).push_back(item);
        }
        return true;
    }
}

void LvlScene::queryItems(QRectF &zone, PGE_ItemList *resultList)
{
    RPoint lt = {(int64_t)zone.left(), (int64_t)zone.top()};
    RPoint rb = {(int64_t)zone.right() + 1, (int64_t)zone.bottom() + 1};
    tree.Search(lt, rb, LevelScene_space::_TreeSearchCallback, (void *)resultList);
}

void LvlScene::queryItems(double x, double y, PGE_ItemList *resultList)
{
    QRectF zone(x, y, 1, 1);
    queryItems(zone, resultList);
}


void LvlScene::registerElement(QGraphicsItem *item)
{
    QPoint pt = item->scenePos().toPoint();
    QSize pz(item->data(ITEM_WIDTH).toInt(), item->data(ITEM_HEIGHT).toInt());
    RPoint lt = {(int64_t)pt.x(), (int64_t)pt.y()};
    RPoint rb = {(int64_t)pt.x() + (int64_t)pz.width(), (int64_t)pt.y() + (int64_t)pz.height()};
    if(pz.width() <= 0)
    {
        rb[0] = pt.x() + 1;
        pz.setWidth(1);
    }
    if(pz.height() <= 0)
    {
        rb[1] = pt.y() + 1;
        pz.setHeight(1);
    }
    tree.Insert(lt, rb, item);
    item->setData(ITEM_LAST_POS, pt);
    item->setData(ITEM_LAST_SIZE, pz);
}

void LvlScene::unregisterElement(QGraphicsItem *item)
{
    if(!item->data(ITEM_LAST_POS).isValid())
        return;
    if(item->data(ITEM_LAST_SIZE).isNull())
        return;
    QPoint pt = item->data(ITEM_LAST_POS).toPoint();
    QSize  pz = item->data(ITEM_LAST_SIZE).toSize();
    RPoint lt = {(int64_t)pt.x(), (int64_t)pt.y()};
    RPoint rb = {(int64_t)pt.x() + (int64_t)pz.width(), (int64_t)pt.y() + (int64_t)pz.height()};
    if(pz.width() <= 0)
        rb[0] = pt.x() + 1;
    if(pz.height() <= 0)
        rb[1] = pt.y() + 1;
    tree.Remove(lt, rb, item);
}
