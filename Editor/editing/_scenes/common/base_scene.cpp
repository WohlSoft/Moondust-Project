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

#include <common_features/logger.h>

#include "base_scene.h"


MoondustBaseScene::MoondustBaseScene(MainWindow *mw, GraphicsWorkspace *parentView, QObject *parent) :
    QGraphicsScene(parent)
    , m_mw(mw)
    , m_viewPort(parentView)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);
}

MainWindow *MoondustBaseScene::mw()
{
    return m_mw;
}

GraphicsWorkspace *MoondustBaseScene::curViewPort()
{
    return m_viewPort;
}

QSet<QGraphicsItem *> &MoondustBaseScene::allItems()
{
    return m_itemsAll;
}


bool MoondustBaseScene::treeSearchCallback(QGraphicsItem *item, void *arg)
{
    PGE_ItemList *list = static_cast<PGE_ItemList * >(arg);

    if(list)
    {
        if(item)
            (*list).push_back(item);
    }

    return true;
}

bool MoondustBaseScene::treeSearchCallbackMap(QGraphicsItem *item, void *arg)
{
    PGE_ItemsListSorted *list = static_cast<PGE_ItemsListSorted * >(arg);

    if(list)
    {
        if(item)
            list->insert(item->zValue(), item);
    }

    return true;
}

bool MoondustBaseScene::allowEditModeSwitch() const
{
    return true;
}

MoondustBaseScene::EditModeID MoondustBaseScene::editMode() const
{
    return m_editMode;
}

void MoondustBaseScene::queryItems(const QRectF &zone, PGE_ItemList *resultList)
{
    RPoint lt = {static_cast<int64_t>(zone.left()), static_cast<int64_t>(zone.top())};
    RPoint rb = {static_cast<int64_t>(zone.right()) + 1, static_cast<int64_t>(zone.bottom()) + 1};
    tree.Search(lt, rb, &treeSearchCallback, reinterpret_cast<void *>(resultList));
}

void MoondustBaseScene::queryItems(double x, double y, PGE_ItemList *resultList)
{
    QRectF zone(x, y, 1, 1);
    queryItems(zone, resultList);
}

void MoondustBaseScene::queryItemsSorted(const QRectF &zone, PGE_ItemsListSorted *resultList)
{
    RPoint lt = {static_cast<int64_t>(zone.left()), static_cast<int64_t>(zone.top())};
    RPoint rb = {static_cast<int64_t>(zone.right()) + 1, static_cast<int64_t>(zone.bottom()) + 1};
    tree.Search(lt, rb, &treeSearchCallbackMap, reinterpret_cast<void *>(resultList));
}

bool MoondustBaseScene::checkGroupCollisions(const PGE_ItemList &items)
{
    if(items.empty())
        return false;

    if(items.size() == 1)
    {
        LogDebug(QString("Collision check: single item"));
        return (itemCollidesWith(items.first()) != nullptr);
    }

    QGraphicsItem *firstItem = items.first();

    //9 - width, 10 - height
    QRectF findZone = QRectF(firstItem->scenePos(),
                             QSizeF(firstItem->data(ITEM_WIDTH).toInt(),
                                    firstItem->data(ITEM_HEIGHT).toInt()));

    //get Zone
    foreach(const auto *it, items)
    {
        if(!it)
            continue;

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

    PGE_ItemList checkZone;
    queryItems(findZone, &checkZone);

#ifdef _DEBUG_
    LogDebug(QString("Collision check: found items for check %1").arg(checkZone.size()));
    LogDebug(QString("Collision rect: x%1 y%2 w%3 h%4").arg(findZone.x())
             .arg(findZone.y()).arg(findZone.width()).arg(findZone.height()));
#endif

    //Don't collide with items which in the group
    for(auto it = checkZone.begin(); it != checkZone.end(); )
    {
        for(auto jt = items.begin(); jt != items.end(); ++jt)
        {
            if(*it == *jt)
            {
                it = checkZone.erase(it);
                break;
            }
        }

        if(it != checkZone.end())
            ++it;
    }

    foreach(const auto *it, items)
    {
        if(itemCollidesWith(it, &checkZone) != nullptr)
            return true;
    }

    return false;
}

void MoondustBaseScene::registerElement(QGraphicsItem *item)
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
    m_itemsAll.insert(item);
}

void MoondustBaseScene::updateElement(QGraphicsItem *item)
{
    unregisterElement(item);
    registerElement(item);
}

void MoondustBaseScene::unregisterElement(QGraphicsItem *item)
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
    m_itemsAll.remove(item);
}
