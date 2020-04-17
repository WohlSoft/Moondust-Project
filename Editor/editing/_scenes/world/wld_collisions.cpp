/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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


//Checking group collisions. Return true if was found even one passed collision in this group
bool WldScene::checkGroupCollisions(QList<QGraphicsItem *> *items)
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

    QList<QGraphicsItem *> CheckZone;
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

QGraphicsItem *WldScene::itemCollidesWith(QGraphicsItem *item, QList<QGraphicsItem *> *itemgrp)
{
    qreal leftA, leftB;
    qreal rightA, rightB;
    qreal topA, topB;
    qreal bottomA, bottomB;

    if(item == nullptr)
        return nullptr;

    if(item->data(ITEM_TYPE).toString() == "YellowRectangle")
        return nullptr;

    QList<QGraphicsItem *> collisions;

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

    //collidingItems(item, Qt::IntersectsItemBoundingRect);
    for(auto *it : collisions)
    {
        if(it == item)
            continue;
        if(it == nullptr)
            continue;
        if(!it->isVisible()) continue;

        if(it->data(ITEM_TYPE).isNull())
            continue;
        if(it->data(ITEM_IS_ITEM).isNull())
            continue;
        //      if(it->data(ITEM_TYPE).toString()=="Space")
        //          continue;
        //            if(it->data(ITEM_TYPE).toString()=="Square")
        //                continue;
        if(item->data(ITEM_TYPE).toString() != it->data(ITEM_TYPE).toString())
            continue;

        if(item->data(ITEM_TYPE).toString() == "SCENERY" &&
           item->data(ITEM_ID).toInt() != it->data(ITEM_ID).toInt())
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

        return it; // Collision found!
    }

    return nullptr;
}

QGraphicsItem *WldScene::itemCollidesCursor(QGraphicsItem *item)
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
        if(type == "TILE")
        {
            if(m_lockTerrain || dynamic_cast<ItemTile *>(it)->isLocked())
                continue;
        }
        else if(type == "SCENERY")
        {
            if(m_lockScenery || dynamic_cast<ItemScene *>(it)->isLocked())
                continue;
        }
        else if(type == "PATH")
        {
            if(m_lockPath || dynamic_cast<ItemPath *>(it)->isLocked())
                continue;
        }
        else if(type == "LEVEL")
        {
            if(m_lockLevel || dynamic_cast<ItemLevel *>(it)->isLocked())
                continue;
        }
        else if(type == "MUSICBOX")
        {
            if(m_lockMusicBox || dynamic_cast<ItemMusic *>(it)->isLocked())
                continue;
        }

        if(
            (type == "TILE") ||
            (type == "SCENERY") ||
            (type == "PATH") ||
            (type == "LEVEL") ||
            (type == "MUSICBOX")
        )
            return it;
    }
    return nullptr;
}

namespace WorldScene_space
{
    bool _TreeSearchCallback(QGraphicsItem *item, void *arg)
    {
        WldScene::PGE_ItemList *list = static_cast<WldScene::PGE_ItemList * >(arg);
        if(list)
        {
            if(item)(*list).push_back(item);
        }
        return true;
    }
}

void WldScene::queryItems(QRectF &zone, PGE_ItemList *resultList)
{
    RPoint lt = {zone.left(), zone.top()};
    RPoint rb = {zone.right() + 1, zone.bottom() + 1};
    tree.Search(lt, rb, WorldScene_space::_TreeSearchCallback, (void *)resultList);
}

void WldScene::queryItems(double x, double y, PGE_ItemList *resultList)
{
    QRectF zone(x, y, 1, 1);
    queryItems(zone, resultList);
}

void WldScene::registerElement(QGraphicsItem *item)
{
    QPointF pt = item->scenePos();
    QSizeF pz(item->data(ITEM_WIDTH).toInt(), item->data(ITEM_HEIGHT).toInt());
    RPoint lt = {pt.x(), pt.y()};
    RPoint rb = {pt.x() + pz.width(), pt.y() + pz.height()};
    if(pz.width() <= 0)
        rb[0] = pt.x() + 1;
    if(pz.height() <= 0)
        rb[1] = pt.y() + 1;
    tree.Insert(lt, rb, item);
    item->setData(ITEM_LAST_POS, pt);
    item->setData(ITEM_LAST_SIZE, pz);
}

void WldScene::unregisterElement(QGraphicsItem *item)
{
    if(!item->data(ITEM_LAST_POS).isValid()) return;
    if(item->data(ITEM_LAST_SIZE).isNull()) return;

    QPointF pt = item->data(ITEM_LAST_POS).toPointF();
    QSizeF pz = item->data(ITEM_LAST_POS).toSizeF();
    RPoint lt = {pt.x(), pt.y()};
    RPoint rb = {pt.x() + pz.width(), pt.y() + pz.height()};
    if(pz.width() <= 0)
        rb[0] = pt.x() + 1;
    if(pz.height() <= 0)
        rb[1] = pt.y() + 1;
    tree.Remove(lt, rb, item);
}
