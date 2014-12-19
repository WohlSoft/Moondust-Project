/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/grid.h>
#include <common_features/mainwinconnect.h>
#include <editing/edit_world/world_edit.h>

#include "../wld_scene.h"
#include "item_tile.h"
#include "item_scene.h"
#include "item_path.h"
#include "item_level.h"
#include "item_music.h"
#include "item_point.h"


QPoint WldScene::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    if((grid)&&(gridSize>0))
        return Grid::applyGrid(source, gridSize, gridOffset);
    else
        return source;
}

void WldScene::applyGroupGrid(QList<QGraphicsItem *> items, bool force)
{
    if(items.size()==0)
        return;

    QPoint sourcePos=QPoint(0,0);
    QPoint sourcePosMax=QPoint(0,0);
    int gridSize=0,gridSizeMax=0, offsetX=0, offsetY=0, offsetXMax=0, offsetYMax=0;//, gridX, gridY, i=0;
    QGraphicsItem * lead = NULL;
    //QGraphicsItemGroup *tmp = NULL;
    QString ObjType;

    foreach(QGraphicsItem * it, items)
    {
        if(!it) continue;
        offsetX=0;
        offsetY=0;
        ObjType = it->data(0).toString();
        if( ObjType == "TILE")
        {
            sourcePos = QPoint(  dynamic_cast<ItemTile *>(it)->tileData.x, dynamic_cast<ItemTile *>(it)->tileData.y);
            gridSize = dynamic_cast<ItemTile *>(it)->gridSize;
        }
        else
        if( ObjType == "SCENERY")
        {
            sourcePos = QPoint(  dynamic_cast<ItemScene *>(it)->sceneData.x, dynamic_cast<ItemScene *>(it)->sceneData.y);
            gridSize = dynamic_cast<ItemScene *>(it)->gridSize;
        }
        else
        if( ObjType == "PATH")
        {
            sourcePos = QPoint(  dynamic_cast<ItemPath *>(it)->pathData.x, dynamic_cast<ItemPath *>(it)->pathData.y);
            gridSize = dynamic_cast<ItemPath *>(it)->gridSize;
        }
        else
        if( ObjType == "LEVEL")
        {
            sourcePos = QPoint(  dynamic_cast<ItemLevel *>(it)->levelData.x, dynamic_cast<ItemLevel *>(it)->levelData.y);
            gridSize = dynamic_cast<ItemLevel *>(it)->gridSize;
        }
        else
        if( ObjType == "MUSICBOX")
        {
            sourcePos = QPoint(  dynamic_cast<ItemMusic *>(it)->musicData.x, dynamic_cast<ItemMusic *>(it)->musicData.y);
            gridSize = dynamic_cast<ItemMusic *>(it)->gridSize;
        }

        if(gridSize>gridSizeMax)
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
        if( sourcePosMax==lead->scenePos().toPoint() && !force )
            return;

        offset=lead->scenePos().toPoint();
        lead->setPos(QPointF(applyGrid(lead->scenePos().toPoint(), gridSizeMax, QPoint(offsetXMax,offsetYMax) ) ) );

        offset.setX( offset.x() - lead->scenePos().toPoint().x() );
        offset.setY( offset.y() - lead->scenePos().toPoint().y() );

        if(items.size()>1)
        {
            foreach(QGraphicsItem * it, items)
            {
                if(it!=lead)
                {
                    QPoint target;
                    target.setX( it->scenePos().toPoint().x()-offset.x() );
                    target.setY( it->scenePos().toPoint().y()-offset.y() );
                    it->setPos(target);
                }
                if(force) applyArrayForItem(it);
            }
        } else if(force) applyArrayForItem(lead);
    }
}

void WldScene::applyGridToEach(QList<QGraphicsItem *> items)
{
    if(items.size()==0)
        return;

    QPoint sourcePos=QPoint(0,0);
    int gridSize=0,offsetX=0, offsetY=0;//, gridX, gridY, i=0;
    QString ObjType;

    foreach(QGraphicsItem * it, items)
    {
        if(!it) continue;
        offsetX=0;
        offsetY=0;
        ObjType = it->data(0).toString();
        if( ObjType == "TILE")
        {
            sourcePos = QPoint(  dynamic_cast<ItemTile *>(it)->tileData.x, dynamic_cast<ItemTile *>(it)->tileData.y);
            gridSize = dynamic_cast<ItemTile *>(it)->gridSize;
        }
        else
        if( ObjType == "SCENERY")
        {
            sourcePos = QPoint(  dynamic_cast<ItemScene *>(it)->sceneData.x, dynamic_cast<ItemScene *>(it)->sceneData.y);
            gridSize = dynamic_cast<ItemScene *>(it)->gridSize;
        }
        else
        if( ObjType == "PATH")
        {
            sourcePos = QPoint(  dynamic_cast<ItemPath *>(it)->pathData.x, dynamic_cast<ItemPath *>(it)->pathData.y);
            gridSize = dynamic_cast<ItemPath *>(it)->gridSize;
        }
        else
        if( ObjType == "LEVEL")
        {
            sourcePos = QPoint(  dynamic_cast<ItemLevel *>(it)->levelData.x, dynamic_cast<ItemLevel *>(it)->levelData.y);
            gridSize = dynamic_cast<ItemLevel *>(it)->gridSize;
        }
        else
        if( ObjType == "MUSICBOX")
        {
            sourcePos = QPoint(  dynamic_cast<ItemMusic *>(it)->musicData.x, dynamic_cast<ItemMusic *>(it)->musicData.y);
            gridSize = dynamic_cast<ItemMusic *>(it)->gridSize;
        }

        it->setPos( QPointF(Grid::applyGrid(it->pos().toPoint(), gridSize, QPoint(offsetX, offsetY))) );
        if( sourcePos != it->scenePos() )
            applyArrayForItem(it);
    }
}






void WldScene::flipGroup(QList<QGraphicsItem *> items, bool vertical, bool recordHistory)
{
    if(items.size()<1)
        return;

    //For history
    //WorldData rotatedData;
    Q_UNUSED(recordHistory);

    QRect zone = QRect(0,0,0,0);
    QRect itemZone = QRect(0,0,0,0);
    //Calculate common width/height of group

    zone.setX(qRound(items.first()->scenePos().x()));
    zone.setWidth(items.first()->data(ITEM_WIDTH).toInt()+1);
    zone.setY(qRound(items.first()->scenePos().y()));
    zone.setHeight(items.first()->data(ITEM_HEIGHT).toInt()+1);

    foreach(QGraphicsItem * item, items)
    {
        if( item->data(ITEM_IS_ITEM).isNull() )
            continue;

        itemZone.setX(qRound(item->scenePos().x()));
        itemZone.setWidth(item->data(ITEM_WIDTH).toInt()+1);
        itemZone.setY(qRound(item->scenePos().y()));
        itemZone.setHeight(item->data(ITEM_HEIGHT).toInt()+1);

        if(itemZone.left()<zone.left()) zone.setLeft(itemZone.left());
        if(itemZone.top()<zone.top()) zone.setTop(itemZone.top());
        if(itemZone.right()>zone.right()) zone.setRight(itemZone.right());
        if(itemZone.bottom()>zone.bottom()) zone.setBottom(itemZone.bottom());
    }

    //Apply flipping formula to each item
    foreach(QGraphicsItem * item, items)
    {
        if(vertical)
        {
            qreal h2;//Opposit height (between bottom side of item and bottom side of zone)
            h2 = qFabs( (item->scenePos().y() + item->data(ITEM_HEIGHT).toInt()) - zone.bottom());

            applyRotationTable(item, RT_FlipV);

            item->setY( zone.top()+h2 );
        }
        else
        {
            qreal w2;//Opposit width (between right side of item and right side of zone)
            w2 = qFabs( (item->scenePos().x() + item->data(ITEM_WIDTH).toInt() ) - zone.right() );

            applyRotationTable(item, RT_FlipH);

            item->setX( zone.left()+w2 );
        }
        applyArrayForItem(item);
        //if(recordHistory)
            //collectDataFromItem(rotatedData, item);
    }

    //if(recordHistory){
    //    addFlipHistory(rotatedData, vertical);
    //}
}

void WldScene::rotateGroup(QList<QGraphicsItem *> items, bool byClockwise, bool recordHistory)
{
    if(items.size()==0)
        return;

    //For history
    //WorldData rotatedData;
    Q_UNUSED(recordHistory);

    //Calculate common width/height of group
    QRect zone = QRect(0,0,0,0);
    QRect itemZone = QRect(0,0,0,0);
    QRect targetRect = QRect(0,0,0,0);
    //Calculate common width/height of group

    zone.setX(qRound(items.first()->scenePos().x()));
    zone.setWidth(items.first()->data(ITEM_WIDTH).toInt()+1);
    zone.setY(qRound(items.first()->scenePos().y()));
    zone.setHeight(items.first()->data(ITEM_HEIGHT).toInt()+1);

    foreach(QGraphicsItem * item, items)
    {
        if( item->data(ITEM_IS_ITEM).isNull() )
            continue;

        itemZone.setX(qRound(item->scenePos().x()));
        itemZone.setWidth(item->data(ITEM_WIDTH).toInt()+1);
        itemZone.setY(qRound(item->scenePos().y()));
        itemZone.setHeight(item->data(ITEM_HEIGHT).toInt()+1);

        if(itemZone.left()<zone.left()) zone.setLeft(itemZone.left());
        if(itemZone.top()<zone.top()) zone.setTop(itemZone.top());
        if(itemZone.right()>zone.right()) zone.setRight(itemZone.right());
        if(itemZone.bottom()>zone.bottom()) zone.setBottom(itemZone.bottom());
    }

    //Apply rotate formula to each item
    foreach(QGraphicsItem * item, items)
    {
        itemZone.setX(qRound(item->scenePos().x()));
        itemZone.setWidth(item->data(ITEM_WIDTH).toInt()+1);
        itemZone.setY(qRound(item->scenePos().y()));
        itemZone.setHeight(item->data(ITEM_HEIGHT).toInt()+1);

        //Distacnces between sides
        qreal dist_t = qFabs(zone.top()-itemZone.top());
        qreal dist_l = qFabs(zone.left()-itemZone.left());
        qreal dist_r = qFabs(itemZone.right()-zone.right());
        qreal dist_b = qFabs(itemZone.bottom()-zone.bottom());

        //If item located in one of quouters of zone rectangle

        if(byClockwise)
        {
            targetRect.setX( zone.left() + dist_b );
            targetRect.setY( zone.top() + dist_l );

            applyRotationTable(item, RT_RotateRight);
        }
        else
        {
            targetRect.setX( zone.left() + dist_t );
            targetRect.setY( zone.top() + dist_r );

            applyRotationTable(item, RT_RotateLeft);
        }

        item->setPos(targetRect.x(), targetRect.y());
        applyArrayForItem(item);
        //if(recordHistory)
        //    collectDataFromItem(rotatedData, item);
    }

    //if(recordHistory){
    //    addRotateHistory(rotatedData, byClockwise);
    //}
}


void WldScene::applyRotationTable(QGraphicsItem *item, WldScene::rotateActions act)
{
    if(!item) return;
    if(item->data(ITEM_IS_ITEM).isNull()) return;

    QString itemType = item->data(ITEM_TYPE).toString();
    long itemID = item->data(ITEM_ID).toInt();

    if(itemType=="TILE")
    {
        if(local_rotation_table_tiles.contains(itemID))
        {
            long target=0;
            switch(act)
            {
                case RT_RotateLeft:
                    target = local_rotation_table_tiles[itemID].rotate_left;
                    break;
                case RT_RotateRight:
                    target = local_rotation_table_tiles[itemID].rotate_right;
                    break;
                case RT_FlipH:
                    target = local_rotation_table_tiles[itemID].flip_h;
                    break;
                case RT_FlipV:
                    target = local_rotation_table_tiles[itemID].flip_v;
                    break;
            }
            if(target>0)
                dynamic_cast<ItemTile *>(item)->transformTo(target);
        }
    }
    else
    if(itemType=="SCENERY")
    {
        if(local_rotation_table_sceneries.contains(itemID))
        {
            long target=0;
            switch(act)
            {
                case RT_RotateLeft:
                    target = local_rotation_table_sceneries[itemID].rotate_left;
                    break;
                case RT_RotateRight:
                    target = local_rotation_table_sceneries[itemID].rotate_right;
                    break;
                case RT_FlipH:
                    target = local_rotation_table_sceneries[itemID].flip_h;
                    break;
                case RT_FlipV:
                    target = local_rotation_table_sceneries[itemID].flip_v;
                    break;
            }
            if(target>0)
                dynamic_cast<ItemScene *>(item)->transformTo(target);
        }
    }
    else
    if(itemType=="PATH")
    {
        if(local_rotation_table_paths.contains(itemID))
        {
            long target=0;
            switch(act)
            {
                case RT_RotateLeft:
                    target = local_rotation_table_paths[itemID].rotate_left;
                    break;
                case RT_RotateRight:
                    target = local_rotation_table_paths[itemID].rotate_right;
                    break;
                case RT_FlipH:
                    target = local_rotation_table_paths[itemID].flip_h;
                    break;
                case RT_FlipV:
                    target = local_rotation_table_paths[itemID].flip_v;
                    break;
            }
            if(target>0)
                dynamic_cast<ItemPath *>(item)->transformTo(target);
        }
    }
    else
    if(itemType=="LEVEL")
    {
        if(local_rotation_table_levels.contains(itemID))
        {
            long target=0;
            switch(act)
            {
                case RT_RotateLeft:
                    target = local_rotation_table_levels[itemID].rotate_left;
                    break;
                case RT_RotateRight:
                    target = local_rotation_table_levels[itemID].rotate_right;
                    break;
                case RT_FlipH:
                    target = local_rotation_table_levels[itemID].flip_h;
                    break;
                case RT_FlipV:
                    target = local_rotation_table_levels[itemID].flip_v;
                    break;
            }
            if(target>0)
                dynamic_cast<ItemLevel *>(item)->transformTo(target);
        }
    }

}


