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


