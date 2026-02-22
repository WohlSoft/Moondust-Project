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

#include <QPainter>

#include <common_features/logger.h>
#include <common_features/edit_mode_base.h>

#include "base_scene.h"


MoondustBaseScene::MoondustBaseScene(MainWindow *mw, GraphicsWorkspace *parentView, QObject *parent) :
    QGraphicsScene(parent)
    , m_mw(mw)
    , m_viewPort(parentView)
{
    setItemIndexMethod(QGraphicsScene::BspTreeIndex);
}

MoondustBaseScene::~MoondustBaseScene()
{
    m_editModes.clear();
}

void MoondustBaseScene::switchEditMode(EditModeID EdtMode)
{
    m_eraserIsEnabled = false;
    m_pastingMode = false;
    m_busyMode = false;
    m_disableMoveItems = false;

    auto newMode = m_editModes.find(EdtMode);

    if(newMode == m_editModes.end())
        newMode = m_editModes.find(MODE_Selecting);

    Q_ASSERT(newMode != m_editModes.end());

    m_editModeObj = newMode.value().data();
    m_editModeObj->set(EdtMode);
    m_editMode = EdtMode;
}

void MoondustBaseScene::setEditFlagEraser(bool en)
{
    m_eraserIsEnabled = en;
}

bool MoondustBaseScene::getEditFlagEraser() const
{
    return m_eraserIsEnabled;
}

void MoondustBaseScene::setEditFlagPasteMode(bool en)
{
    m_pastingMode = en;
}

bool MoondustBaseScene::getEditFlagPasteMode() const
{
    return m_pastingMode;
}

void MoondustBaseScene::setEditFlagBusyMode(bool en)
{
    m_busyMode = en;
}

bool MoondustBaseScene::getEditFlagBusyMode() const
{
    return m_busyMode;
}

void MoondustBaseScene::setEditFlagNoMoveItems(bool en)
{
    m_disableMoveItems = en;
}

bool MoondustBaseScene::getEditFlagNoMoveItems() const
{
    return m_disableMoveItems;
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

// static inline void _q_adjustRect(QRectF *rect)
// {
//     Q_ASSERT(rect);

//     if (!rect->width())
//         rect->adjust(qreal(-0.00001), 0, qreal(0.00001), 0);

//     if (!rect->height())
//         rect->adjust(0, qreal(-0.00001), 0, qreal(0.00001));
// }

// static inline QRectF adjustedItemEffectiveBoundingRect(const QGraphicsItem *item)
// {
//     Q_ASSERT(item);
//     QRectF boundingRect(item->sceneBoundingRect());
//     _q_adjustRect(&boundingRect);
//     return boundingRect;
// }

// static inline bool itemIsUntransformableF(const QGraphicsItem *p)
// {
//     return p->flags() & QGraphicsItem::ItemIgnoresTransformations;
// }

#if 0
void MoondustBaseScene::drawBetterDrawItems(QPainter *painter, const QTransform * const viewTransform, QRegion *exposedRegion, QWidget *widget)
{
    QRectF exposedSceneRect;

    Q_ASSERT(exposedRegion);

    if(!exposedRegion)
        return; // Nothing to draw!

    if(exposedRegion)
    {
        exposedSceneRect = exposedRegion->boundingRect().adjusted(-1, -1, 1, 1);

        if(viewTransform)
            exposedSceneRect = viewTransform->inverted().mapRect(exposedSceneRect);
    }

    PGE_ItemsListSorted tli;
    queryItemsSorted(exposedSceneRect, &tli);

    // bool allItems = false;
    // QList<QGraphicsItem *> tli = items(exposedSceneRect, Qt::IntersectsItemShape, Qt::DescendingOrder, *viewTransform);

    for(auto it = tli.begin(); it != tli.end(); ++it)
    {
        QGraphicsItem *item = it.value();
        if(!item->isVisible())
            continue;

        painter->save();
        painter->setTransform(item->sceneTransform(), true);
        item->paint(painter, nullptr, widget);
        painter->restore();

        // QList<QGraphicsItem *> children = item->childItems();
        // for(auto jt = children.begin(); jt != children.end(); ++jt)
        // {
        //     QGraphicsItem *child = *jt;
        //     child->paint(painter, nullptr, widget);
        // }
    }
}
#endif

#if 0
static inline qreal combineOpacityFromParent(const QGraphicsItem *p, qreal parentOpacity)
{
    if(p->parentItem() && !(p->flags() & QGraphicsItem::ItemIgnoresParentOpacity) && !(p->parentItem()->flags() & QGraphicsItem::ItemDoesntPropagateOpacityToChildren))
        return parentOpacity * p->opacity();

    return p->opacity();
}

static inline bool childrenCombineOpacity(const QGraphicsItem *p)
{
    if(!p->childItems().size())
        return true;

    if(p->flags() & QGraphicsItem::ItemDoesntPropagateOpacityToChildren)
        return false;

    for(int i = 0; i < p->childItems().size(); ++i)
    {
        if (p->childItems().at(i)->flags() & QGraphicsItem::ItemIgnoresParentOpacity)
            return false;
    }

    return true;
}



void MoondustBaseScene::drawSubtreeRecursive(QGraphicsItem *item, QPainter *painter, const QTransform * const viewTransform, QRegion *exposedRegion, QWidget *widget, qreal parentOpacity, const QTransform * const effectTransform)
{
    drawItems();

    Q_ASSERT(item);

    if(!item->isVisible())
        return;

    const bool itemHasContents = !(item->flags() & QGraphicsItem::ItemHasNoContents);
    const bool itemHasChildren = !item->childItems().isEmpty();

    if(!itemHasContents && !itemHasChildren)
        return; // Item has neither contents nor children!(?)

    const qreal opacity = combineOpacityFromParent(item, parentOpacity);
    const bool itemIsFullyTransparent = opacity < qreal(0.001);
    if(itemIsFullyTransparent && (!itemHasChildren || childrenCombineOpacity(item)))
        return;

    QTransform sceneTransform(Qt::Uninitialized);
    QTransform transform(Qt::Uninitialized);
    QTransform *transformPtr = nullptr;
    bool translateOnlyTransform = false;

#define ENSURE_TRANSFORM_PTR \
    if(!transformPtr) \
    { \
        Q_ASSERT(!itemIsUntransformable); \
        if (viewTransform) \
        { \
            transform = item->sceneTransform(); \
            transform *= *viewTransform; \
            transformPtr = &transform; \
        } \
        else \
        { \
            sceneTransform = item->sceneTransform(); \
            transformPtr = &sceneTransform; \
        } \
    }


    // Update the item's scene transform if the item is transformable;
    // otherwise calculate the full transform,
    bool wasDirtyParentSceneTransform = false;
    const bool itemIsUntransformable = itemIsUntransformableF(item);

    if(itemIsUntransformable)
    {
        transform = item->deviceTransform(viewTransform ? *viewTransform : QTransform());
        transformPtr = &transform;
    }
    // else if(item->d_ptr->dirtySceneTransform)
    // {
    //     item->d_ptr->updateSceneTransformFromParent();
    //     Q_ASSERT(!item->d_ptr->dirtySceneTransform);
    //     wasDirtyParentSceneTransform = true;
    // }

    const bool itemClipsChildrenToShape = (item->flags() & QGraphicsItem::ItemClipsChildrenToShape || item->flags() & QGraphicsItem::ItemContainsChildrenInShape);
    bool drawItem = itemHasContents && !itemIsFullyTransparent;
    if(drawItem || minimumRenderSize() > 0.0)
    {
        const QRectF brect = adjustedItemEffectiveBoundingRect(item);

        ENSURE_TRANSFORM_PTR

        QRectF preciseViewBoundingRect = translateOnlyTransform ? brect.translated(transformPtr->dx(), transformPtr->dy())
                                                                : transformPtr->mapRect(brect);

        bool itemIsTooSmallToRender = false;
        if(minimumRenderSize() > 0.0 && (preciseViewBoundingRect.width() < minimumRenderSize() || preciseViewBoundingRect.height() < minimumRenderSize()))
        {
            itemIsTooSmallToRender = true;
            drawItem = false;
        }

        bool itemIsOutsideVisibleRect = false;

        if(drawItem)
        {
            QRect viewBoundingRect = preciseViewBoundingRect.toAlignedRect();
            viewBoundingRect.adjust(-int(rectAdjust), -int(rectAdjust), rectAdjust, rectAdjust);

            if(widget)
                item->d_ptr->paintedViewBoundingRects.insert(widget, viewBoundingRect);

            drawItem = exposedRegion ? exposedRegion->intersects(viewBoundingRect)
                                     : !viewBoundingRect.normalized().isEmpty();
            itemIsOutsideVisibleRect = !drawItem;
        }

        if (itemIsTooSmallToRender || itemIsOutsideVisibleRect)
        {
            // We cannot simply use !drawItem here. If we did it is possible
            // to enter the outter if statement with drawItem == false and minimumRenderSize > 0
            // and finally end up inside this inner if, even though none of the above two
            // conditions are met. In that case we should not return from this function
            // but call draw() instead.
            if(!itemHasChildren)
                return;

            if(itemClipsChildrenToShape)
            {
                // if (wasDirtyParentSceneTransform)
                //     item->d_ptr->invalidateChildrenSceneTransform();
                return;
            }
        }
    } // else we know for sure this item has children we must process.

    if (itemHasChildren && itemClipsChildrenToShape)
        ENSURE_TRANSFORM_PTR;


    draw(item, painter, viewTransform, transformPtr, exposedRegion, widget, opacity,
         effectTransform, wasDirtyParentSceneTransform, drawItem);

}
#endif // if 0
