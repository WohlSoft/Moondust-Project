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
#include <editing/edit_level/level_edit.h>
#include <file_formats/file_formats.h>

#include "../lvl_scene.h"
#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "item_playerpoint.h"

QPoint LvlScene::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    if((grid)&&(gridSize>0))
        return Grid::applyGrid(source, gridSize, gridOffset);
    else
        return source;
}


void LvlScene::applyGroupGrid(QList<QGraphicsItem *> items, bool force)
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
        ObjType = it->data(ITEM_TYPE).toString();
        if( ObjType == "NPC")
        {
            sourcePos = QPoint(  dynamic_cast<ItemNPC *>(it)->npcData.x, dynamic_cast<ItemNPC *>(it)->npcData.y);
            gridSize = dynamic_cast<ItemNPC *>(it)->gridSize;
            offsetX = dynamic_cast<ItemNPC *>(it)->localProps.grid_offset_x;
            offsetY = dynamic_cast<ItemNPC *>(it)->localProps.grid_offset_y;
        }
        else
        if( ObjType == "Block")
        {
            sourcePos = QPoint(  dynamic_cast<ItemBlock *>(it)->blockData.x, dynamic_cast<ItemBlock *>(it)->blockData.y);
            gridSize = dynamic_cast<ItemBlock *>(it)->gridSize;
            //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
        }
        else
        if( ObjType == "BGO")
        {
            sourcePos = QPoint(  dynamic_cast<ItemBGO *>(it)->bgoData.x, dynamic_cast<ItemBGO *>(it)->bgoData.y);
            gridSize = dynamic_cast<ItemBGO *>(it)->gridSize;
            offsetX = dynamic_cast<ItemBGO *>(it)->gridOffsetX;
            offsetY = dynamic_cast<ItemBGO *>(it)->gridOffsetY;
        }
        else
        if( ObjType == "Water")
        {
            sourcePos = QPoint(  dynamic_cast<ItemWater *>(it)->waterData.x, dynamic_cast<ItemWater *>(it)->waterData.y);
            gridSize = qRound(qreal(pConfigs->default_grid)/2);
        }
        else
        if( ObjType == "Door_enter")
        {
            sourcePos = QPoint(  dynamic_cast<ItemDoor *>(it)->doorData.ix, dynamic_cast<ItemDoor *>(it)->doorData.iy);
            gridSize = qRound(qreal(pConfigs->default_grid)/2);
        }
        else
        if( ObjType == "Door_exit"){
            sourcePos = QPoint(  dynamic_cast<ItemDoor *>(it)->doorData.ox, dynamic_cast<ItemDoor *>(it)->doorData.oy);
            gridSize = qRound(qreal(pConfigs->default_grid)/2);
        }
        else
        if( ObjType == "playerPoint" )
        {
            gridSize = 2 ;
            sourcePos = QPoint(dynamic_cast<ItemPlayerPoint *>(it)->pointData.x, dynamic_cast<ItemPlayerPoint *>(it)->pointData.y);
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

void LvlScene::applyGridToEach(QList<QGraphicsItem *> items)
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
        ObjType = it->data(ITEM_TYPE).toString();
        if( ObjType == "NPC")
        {
            sourcePos = QPoint(  dynamic_cast<ItemNPC *>(it)->npcData.x, dynamic_cast<ItemNPC *>(it)->npcData.y);
            gridSize = dynamic_cast<ItemNPC *>(it)->gridSize;
            offsetX = dynamic_cast<ItemNPC *>(it)->localProps.grid_offset_x;
            offsetY = dynamic_cast<ItemNPC *>(it)->localProps.grid_offset_y;
        }
        else
        if( ObjType == "Block")
        {
            sourcePos = QPoint(  dynamic_cast<ItemBlock *>(it)->blockData.x, dynamic_cast<ItemBlock *>(it)->blockData.y);
            gridSize = dynamic_cast<ItemBlock *>(it)->gridSize;
            //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
        }
        else
        if( ObjType == "BGO")
        {
            sourcePos = QPoint(  dynamic_cast<ItemBGO *>(it)->bgoData.x, dynamic_cast<ItemBGO *>(it)->bgoData.y);
            gridSize = dynamic_cast<ItemBGO *>(it)->gridSize;
            offsetX = dynamic_cast<ItemBGO *>(it)->gridOffsetX;
            offsetY = dynamic_cast<ItemBGO *>(it)->gridOffsetY;
        }
        else
        if( ObjType == "Water")
        {
            sourcePos = QPoint(  dynamic_cast<ItemWater *>(it)->waterData.x, dynamic_cast<ItemWater *>(it)->waterData.y);
            gridSize = qRound(qreal(pConfigs->default_grid)/2);
        }
        else
        if( ObjType == "Door_enter")
        {
            sourcePos = QPoint(  dynamic_cast<ItemDoor *>(it)->doorData.ix, dynamic_cast<ItemDoor *>(it)->doorData.iy);
            gridSize = qRound(qreal(pConfigs->default_grid)/2);
        }
        else
        if( ObjType == "Door_exit"){
            sourcePos = QPoint(  dynamic_cast<ItemDoor *>(it)->doorData.ox, dynamic_cast<ItemDoor *>(it)->doorData.oy);
            gridSize = qRound(qreal(pConfigs->default_grid)/2);
        }
        else
        if( ObjType == "playerPoint" )
        {
            gridSize = 2 ;
            sourcePos = QPoint(dynamic_cast<ItemPlayerPoint *>(it)->pointData.x, dynamic_cast<ItemPlayerPoint *>(it)->pointData.y);
        }

        it->setPos( QPointF(Grid::applyGrid(it->pos().toPoint(), gridSize, QPoint(offsetX, offsetY))) );
        if( sourcePos != it->scenePos() )
            applyArrayForItem(it);
    }
}




void LvlScene::flipGroup(QList<QGraphicsItem *> items, bool vertical)
{
    if(items.size()<1)
        return;

    QRect zone = QRect(0,0,0,0);
    QRect itemZone = QRect(0,0,0,0);
    //Calculate common width/height of group

    zone.setX(qRound(items.first()->scenePos().x()));
    zone.setWidth(items.first()->data(ITEM_WIDTH).toInt()+1);
    zone.setY(qRound(items.first()->scenePos().y()));
    zone.setHeight(items.first()->data(ITEM_HEIGHT).toInt()+1);

    foreach(QGraphicsItem * item, items)
    {
        QString t = item->data(ITEM_TYPE).toString();
        if((t!="Block") && (t!="BGO")&& (t!="NPC") && (t!="Water")
           && (t!="playerPoint") && (t !="Door_enter")&& (t !="Door_exit"))
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

            item->setY( zone.top()+h2 );

        }
        else
        {
            qreal w2;//Opposit width (between right side of item and right side of zone)
            w2 = qFabs( (item->scenePos().x() + item->data(ITEM_WIDTH).toInt() ) - zone.right() );

            item->setX( zone.left()+w2 );

            //Flip NPC direction
            if(item->data(ITEM_TYPE)=="NPC")
                dynamic_cast<ItemNPC * >(item)->changeDirection(
                        -dynamic_cast<ItemNPC * >(item)->npcData.direct
                        );
            else //Flip Player point direction
            if(item->data(ITEM_TYPE)=="playerPoint")
                dynamic_cast<ItemPlayerPoint * >(item)->changeDirection(
                        -dynamic_cast<ItemPlayerPoint * >(item)->pointData.direction
                        );
        }
        applyArrayForItem(item);
    }

}

void LvlScene::rotateGroup(QList<QGraphicsItem *> items, bool byClockwise)
{
    if(items.size()==0)
        return;

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
        QString t = item->data(ITEM_TYPE).toString();
        if((t!="Block") && (t!="BGO")&& (t!="NPC") && (t!="Water")
           && (t!="playerPoint") && (t !="Door_enter")&& (t !="Door_exit"))
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
        }
        else
        {
            targetRect.setX( zone.left() + dist_t );
            targetRect.setY( zone.top() + dist_r );
        }

        item->setPos(targetRect.x(), targetRect.y());

        if(item->data(ITEM_BLOCK_IS_SIZABLE).toString()=="sizable")
        {
            //Rotate width and height
            targetRect.setWidth(itemZone.height()-1);
            targetRect.setHeight(itemZone.width()-1);
            if(item->data(ITEM_TYPE)=="Block")
                dynamic_cast<ItemBlock *>(item)->setBlockSize(targetRect);
            else
            if(item->data(ITEM_TYPE)=="Water")
                dynamic_cast<ItemWater *>(item)->setRectSize(targetRect);
        }

        applyArrayForItem(item);
    }
}

