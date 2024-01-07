/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <mainwindow.h>
#include <common_features/grid.h>
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>
#include <defines.h>
#include <qmath.h>

#include "../lvl_history_manager.h"

QPoint LvlScene::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    if((m_opts.grid_snap) && (gridSize > 0))
    {
        if(m_opts.grid_override)
            gridSize = m_opts.customGrid.width();
        return Grid::applyGrid(source, gridSize, gridOffset);
    }
    else
        return source;
}


void LvlScene::applyGroupGrid(QList<QGraphicsItem *> items, bool force)
{
    if(items.size() == 0)
        return;

    QPoint sourcePos = QPoint(0, 0);
    QPoint sourcePosMax = QPoint(0, 0);
    int gridSize = 0, gridSizeMax = 0, offsetX = 0, offsetY = 0, offsetXMax = 0, offsetYMax = 0; //, gridX, gridY, i=0;
    QGraphicsItem *lead = nullptr;
    //QGraphicsItemGroup *tmp = NULL;
    QString objType;

    for(QGraphicsItem *it : items)
    {
        if(!it) continue;
        offsetX = 0;
        offsetY = 0;
        objType = it->data(ITEM_TYPE).toString();
        if(objType == "NPC")
        {
            ItemNPC *item = dynamic_cast<ItemNPC *>(it);
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            gridSize = item->m_gridSize;
            offsetX =  item->m_localProps.setup.grid_offset_x;
            offsetY =  item->m_localProps.setup.grid_offset_y;
        }
        else if(objType == "Block")
        {
            ItemBlock *item = dynamic_cast<ItemBlock *>(it);
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            gridSize  = item->m_gridSize;
            offsetX =  item->m_gridOffsetX;
            offsetY =  item->m_gridOffsetY;
        }
        else if(objType == "BGO")
        {
            ItemBGO *item = dynamic_cast<ItemBGO *>(it);
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            gridSize = item->m_gridSize;
            offsetX =  item->m_gridOffsetX;
            offsetY =  item->m_gridOffsetY;
        }
        else if(objType == "Water")
        {
            ItemPhysEnv *item = dynamic_cast<ItemPhysEnv *>(it);
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            gridSize  = item->m_gridSize;
        }
        else if(objType == "Door_enter")
        {
            ItemDoor *item = dynamic_cast<ItemDoor *>(it);
            sourcePos = QPoint(item->m_data.ix, item->m_data.iy);
            gridSize  = item->m_gridSize;
        }
        else if(objType == "Door_exit")
        {
            ItemDoor *item = dynamic_cast<ItemDoor *>(it);
            sourcePos = QPoint(item->m_data.ox, item->m_data.oy);
            gridSize  = item->m_gridSize;
        }
        else if(objType == "playerPoint")
        {
            ItemPlayerPoint *item = dynamic_cast<ItemPlayerPoint *>(it);
            gridSize  = item->m_gridSize;
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            offsetX =  item->m_gridOffsetX;
            offsetY =  item->m_gridOffsetY;
        }

        if(gridSize > gridSizeMax)
        {
            offsetXMax = offsetX;
            offsetYMax = offsetY;
            gridSizeMax = gridSize;
            sourcePosMax = sourcePos;
            lead = it;
        }
    }

    QPoint offset;
    if(lead)
    {
        if(sourcePosMax == lead->scenePos().toPoint() && !force)
            return;

        offset = lead->scenePos().toPoint();
        lead->setPos(QPointF(applyGrid(lead->scenePos().toPoint(), gridSizeMax, QPoint(offsetXMax, offsetYMax))));

        offset.setX(offset.x() - lead->scenePos().toPoint().x());
        offset.setY(offset.y() - lead->scenePos().toPoint().y());

        if(items.size() > 1)
        {
            for(QGraphicsItem *it : items)
            {
                if(it != lead)
                {
                    QPoint target;
                    target.setX(it->scenePos().toPoint().x() - offset.x());
                    target.setY(it->scenePos().toPoint().y() - offset.y());
                    it->setPos(target);
                }
                if(force) applyArrayForItem(it);
            }
        }
        else if(force) applyArrayForItem(lead);
    }
}

void LvlScene::applyGridToEach(QList<QGraphicsItem *> items)
{
    if(items.size() == 0)
        return;

    QPoint sourcePos = QPoint(0, 0);
    int gridSize = 0, offsetX = 0, offsetY = 0; //, gridX, gridY, i=0;
    QString ObjType;

    for(QGraphicsItem *it : items)
    {
        if(!it) continue;
        offsetX = 0;
        offsetY = 0;
        ObjType = it->data(ITEM_TYPE).toString();
        if(ObjType == "NPC")
        {
            ItemNPC *item = dynamic_cast<ItemNPC *>(it);
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            gridSize = item->m_gridSize;
            offsetX =  item->m_localProps.setup.grid_offset_x;
            offsetY =  item->m_localProps.setup.grid_offset_y;
        }
        else if(ObjType == "Block")
        {
            ItemBlock *item = dynamic_cast<ItemBlock *>(it);
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            gridSize  = item->m_gridSize;
        }
        else if(ObjType == "BGO")
        {
            ItemBGO *item = dynamic_cast<ItemBGO *>(it);
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            gridSize = item->m_gridSize;
            offsetX =  item->m_gridOffsetX;
            offsetY =  item->m_gridOffsetY;
        }
        else if(ObjType == "Water")
        {
            ItemPhysEnv *item = dynamic_cast<ItemPhysEnv *>(it);
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            gridSize  = item->m_gridSize;
        }
        else if(ObjType == "Door_enter")
        {
            ItemDoor *item = dynamic_cast<ItemDoor *>(it);
            sourcePos = QPoint(item->m_data.ix, item->m_data.iy);
            gridSize  = item->m_gridSize;
        }
        else if(ObjType == "Door_exit")
        {
            ItemDoor *item = dynamic_cast<ItemDoor *>(it);
            sourcePos = QPoint(item->m_data.ox, item->m_data.oy);
            gridSize  = item->m_gridSize;
        }
        else if(ObjType == "playerPoint")
        {
            ItemPlayerPoint *item = dynamic_cast<ItemPlayerPoint *>(it);
            gridSize  = item->m_gridSize;
            sourcePos = QPoint(item->m_data.x, item->m_data.y);
            offsetX =  item->m_gridOffsetX;
            offsetY =  item->m_gridOffsetY;
        }

        it->setPos(QPointF(Grid::applyGrid(it->pos().toPoint(), gridSize, QPoint(offsetX, offsetY))));
        if(QPointF(sourcePos) != it->scenePos())
            applyArrayForItem(it);
    }
}




void LvlScene::flipGroup(QList<QGraphicsItem *> items, bool vertical, bool recordHistory, bool flipSection)
{
    if(items.size() == 0)
        return;

    //For history
    LevelData unflippedData;
    collectDataFromItems(unflippedData, items);
    LevelData flippedData;

    QRect zone = QRect(0, 0, 0, 0);
    QRect itemZone = QRect(0, 0, 0, 0);
    //Calculate common width/height of group

    zone.setX(qRound(items.first()->scenePos().x()));
    zone.setWidth(items.first()->data(ITEM_WIDTH).toInt() + 1);
    zone.setY(qRound(items.first()->scenePos().y()));
    zone.setHeight(items.first()->data(ITEM_HEIGHT).toInt() + 1);

    if(flipSection)
    {
        int s_top =      m_data->sections[m_data->CurSection].size_top;
        int s_left =     m_data->sections[m_data->CurSection].size_left;
        int s_right =    m_data->sections[m_data->CurSection].size_right;
        int s_bottom =   m_data->sections[m_data->CurSection].size_bottom;

        QPoint tmp(s_left, s_top);
        tmp = Grid::applyGrid(tmp, 32);
        zone.setX(tmp.x());
        zone.setY(tmp.y());
        zone.setWidth(abs(tmp.x() - s_right) + 1);
        zone.setHeight(abs(tmp.y() - s_bottom) + 1);
    }
    else
    {
        for(QGraphicsItem *item : items)
        {
            if(item->data(ITEM_IS_ITEM).isNull())
                continue;

            itemZone.setX(qRound(item->scenePos().x()));
            itemZone.setWidth(item->data(ITEM_WIDTH).toInt() + 1);
            itemZone.setY(qRound(item->scenePos().y()));
            itemZone.setHeight(item->data(ITEM_HEIGHT).toInt() + 1);

            if(itemZone.left() < zone.left()) zone.setLeft(itemZone.left());
            if(itemZone.top() < zone.top()) zone.setTop(itemZone.top());
            if(itemZone.right() > zone.right()) zone.setRight(itemZone.right());
            if(itemZone.bottom() > zone.bottom()) zone.setBottom(itemZone.bottom());
        }
    }

    //Apply flipping formula to each item
    for(QGraphicsItem *item : items)
    {
        if(vertical)
        {
            qreal h2;//Opposit height (between bottom side of item and bottom side of zone)
            int item_b = (item->scenePos().y() + item->data(ITEM_HEIGHT).toInt());

            if(item_b < zone.bottom())
                h2 = qFabs(item_b - zone.bottom());
            else
                h2 = -qFabs(zone.bottom() - item_b);

            applyRotationTable(item, RT_FlipV);

            item->setY(zone.top() + h2);
        }
        else
        {
            qreal w2;//Opposit width (between right side of item and right side of zone)
            int item_r = (item->scenePos().x() + item->data(ITEM_WIDTH).toInt());
            if(item_r < zone.right())
                w2 = qFabs(item_r - zone.right());
            else
                w2 = -qFabs(zone.right() - item_r);

            applyRotationTable(item, RT_FlipH);

            item->setX(zone.left() + w2);

            //Flip NPC direction
            if(item->data(ITEM_TYPE) == "NPC")
                dynamic_cast<ItemNPC * >(item)->changeDirection(
                    -dynamic_cast<ItemNPC * >(item)->m_data.direct
                );
            else //Flip Player point direction
                if(item->data(ITEM_TYPE) == "playerPoint")
                    dynamic_cast<ItemPlayerPoint * >(item)->changeDirection(
                        -dynamic_cast<ItemPlayerPoint * >(item)->m_data.direction
                    );
        }
        applyArrayForItem(item);
        if(recordHistory)
            collectDataFromItem(flippedData, item);
    }

    if(recordHistory)
        m_history->addFlip(flippedData, unflippedData);
}

void LvlScene::rotateGroup(QList<QGraphicsItem *> items, bool byClockwise, bool recordHistory, bool rotateSection)
{
    if(items.size() == 0)
        return;

    //For history
    LevelData unrotatedData;
    collectDataFromItems(unrotatedData, items);
    LevelData rotatedData;

    //Calculate common width/height of group
    QRect zone = QRect(0, 0, 0, 0);
    QRect itemZone = QRect(0, 0, 0, 0);
    QRect targetRect = QRect(0, 0, 0, 0);
    //Calculate common width/height of group

    zone.setX(qRound(items.first()->scenePos().x()));
    zone.setWidth(items.first()->data(ITEM_WIDTH).toInt() + 1);
    zone.setY(qRound(items.first()->scenePos().y()));
    zone.setHeight(items.first()->data(ITEM_HEIGHT).toInt() + 1);


    if(rotateSection)
    {
        int s_top = m_data->sections[m_data->CurSection].size_top;
        int s_left = m_data->sections[m_data->CurSection].size_left;
        int s_right = m_data->sections[m_data->CurSection].size_right;
        int s_bottom = m_data->sections[m_data->CurSection].size_bottom;

        QPoint tmp(s_left, s_top);
        tmp = Grid::applyGrid(tmp, 32);
        zone.setX(s_left);
        zone.setY(tmp.y());
        zone.setWidth(abs(tmp.x() - s_right) + 1);
        zone.setHeight(abs(tmp.y() - s_bottom) + 1);
    }
    else
    {
        for(QGraphicsItem *item : items)
        {
            if(item->data(ITEM_IS_ITEM).isNull())
                continue;

            itemZone.setX(qRound(item->scenePos().x()));
            itemZone.setWidth(item->data(ITEM_WIDTH).toInt() + 1);
            itemZone.setY(qRound(item->scenePos().y()));
            itemZone.setHeight(item->data(ITEM_HEIGHT).toInt() + 1);

            if(itemZone.left() < zone.left()) zone.setLeft(itemZone.left());
            if(itemZone.top() < zone.top()) zone.setTop(itemZone.top());
            if(itemZone.right() > zone.right()) zone.setRight(itemZone.right());
            if(itemZone.bottom() > zone.bottom()) zone.setBottom(itemZone.bottom());
        }
    }

    //Apply rotate formula to each item
    for(QGraphicsItem *item : items)
    {
        itemZone.setX(qRound(item->scenePos().x()));
        itemZone.setWidth(item->data(ITEM_WIDTH).toInt() + 1);
        itemZone.setY(qRound(item->scenePos().y()));
        itemZone.setHeight(item->data(ITEM_HEIGHT).toInt() + 1);

        //Distacnces between sides
        qreal dist_t = 0;
        if(zone.top() <= itemZone.top())
            dist_t = qFabs(zone.top() - itemZone.top());
        else
            dist_t = -qFabs(itemZone.top() - zone.top());

        qreal dist_l = 0;
        if(zone.left() <= itemZone.left())
            dist_l = qFabs(zone.left() - itemZone.left());
        else
            dist_l = -qFabs(itemZone.left() - zone.left());

        qreal dist_r = 0;
        if(itemZone.right() <= zone.right())
            dist_r = qFabs(itemZone.right() - zone.right());
        else
            dist_r = -qFabs(zone.right() - itemZone.right());

        qreal dist_b = 0;
        if(itemZone.bottom() <= zone.bottom())
            dist_b = qFabs(itemZone.bottom() - zone.bottom());
        else
            dist_b = -qFabs(zone.bottom() - itemZone.bottom());

        //If item located in one of quouters of zone rectangle
        if(byClockwise)
        {
            if(item->data(ITEM_BLOCK_IS_SIZABLE).toString() != "sizable")
                applyRotationTable(item, RT_RotateRight);
            targetRect.setX(zone.left() + dist_b);
            targetRect.setY(zone.top() + dist_l);
        }
        else
        {
            if(item->data(ITEM_BLOCK_IS_SIZABLE).toString() != "sizable")
                applyRotationTable(item, RT_RotateLeft);
            targetRect.setX(zone.left() + dist_t);
            targetRect.setY(zone.top() + dist_r);
        }

        item->setPos(targetRect.x(), targetRect.y());

        if(item->data(ITEM_BLOCK_IS_SIZABLE).toString() == "sizable")
        {
            //Rotate width and height
            targetRect.setWidth(itemZone.height() - 1);
            targetRect.setHeight(itemZone.width() - 1);
            if(item->data(ITEM_TYPE) == "Block")
                dynamic_cast<ItemBlock *>(item)->setBlockSize(targetRect);
            else if(item->data(ITEM_TYPE) == "Water")
                dynamic_cast<ItemPhysEnv *>(item)->setRectSize(targetRect);
        }

        applyArrayForItem(item);
        if(recordHistory)
            collectDataFromItem(rotatedData, item);
    }

    if(rotateSection)
    {
        int s_top = m_data->sections[m_data->CurSection].size_top;
        int s_left = m_data->sections[m_data->CurSection].size_left;
        int s_right = m_data->sections[m_data->CurSection].size_right;
        int s_bottom = m_data->sections[m_data->CurSection].size_bottom;

        QPoint tmp(s_left, s_top);
        tmp = Grid::applyGrid(tmp, 32);
        s_left = tmp.x();
        s_top = tmp.y();
        int ns_right = s_left + abs(s_top - s_bottom);
        int ns_bottom = s_top + abs(s_left - s_right);
        s_bottom = ns_bottom;
        s_right = ns_right;

        m_data->sections[m_data->CurSection].size_top = s_top;
        m_data->sections[m_data->CurSection].size_left = s_left;
        m_data->sections[m_data->CurSection].size_right = s_right;
        m_data->sections[m_data->CurSection].size_bottom = s_bottom;

        ChangeSectionBG(m_data->sections[m_data->CurSection].background);
        drawSpace();
    }

    if(recordHistory)
        m_history->addRotate(rotatedData, unrotatedData);
}

void LvlScene::applyRotationTable(QGraphicsItem *item, LvlScene::rotateActions act)
{
    if(!item) return;
    if(item->data(ITEM_IS_ITEM).isNull()) return;

    QString itemType = item->data(ITEM_TYPE).toString();
    long itemID = item->data(ITEM_ID).toInt();

    if(itemType == "Block")
    {
        if(local_rotation_table_blocks.contains(itemID))
        {
            long target = 0;
            switch(act)
            {
            case RT_RotateLeft:
                target = local_rotation_table_blocks[itemID].rotate_left;
                break;
            case RT_RotateRight:
                target = local_rotation_table_blocks[itemID].rotate_right;
                break;
            case RT_FlipH:
                target = local_rotation_table_blocks[itemID].flip_h;
                break;
            case RT_FlipV:
                target = local_rotation_table_blocks[itemID].flip_v;
                break;
            }
            if(target > 0)
                dynamic_cast<ItemBlock *>(item)->transformTo(target);
        }
    }
    else if(itemType == "BGO")
    {
        if(local_rotation_table_bgo.contains(itemID))
        {
            long target = 0;
            switch(act)
            {
            case RT_RotateLeft:
                target = local_rotation_table_bgo[itemID].rotate_left;
                break;
            case RT_RotateRight:
                target = local_rotation_table_bgo[itemID].rotate_right;
                break;
            case RT_FlipH:
                target = local_rotation_table_bgo[itemID].flip_h;
                break;
            case RT_FlipV:
                target = local_rotation_table_bgo[itemID].flip_v;
                break;
            }
            if(target > 0)
                dynamic_cast<ItemBGO *>(item)->transformTo(target);
        }
    }
    else if(itemType == "NPC")
    {
        if(local_rotation_table_npc.contains(itemID))
        {
            long target = 0;
            switch(act)
            {
            case RT_RotateLeft:
                target = local_rotation_table_npc[itemID].rotate_left;
                break;
            case RT_RotateRight:
                target = local_rotation_table_npc[itemID].rotate_right;
                break;
            case RT_FlipH:
                target = local_rotation_table_npc[itemID].flip_h;
                break;
            case RT_FlipV:
                target = local_rotation_table_npc[itemID].flip_v;
                break;
            }
            if(target > 0)
                dynamic_cast<ItemBGO *>(item)->transformTo(target);
        }
    }
}
