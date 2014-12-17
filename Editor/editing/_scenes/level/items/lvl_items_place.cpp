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

void LvlScene::placeBlock(LevelBlock &block, bool toGrid)
{
    bool noimage=true;
    long item_i=0;
    long animator=0;

    obj_block mergedSet;

    getConfig_Block(block.id, item_i, animator, mergedSet, &noimage);

    //Set pointers
    QPoint newPos = QPoint(block.x, block.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(block.x, block.y), mergedSet.grid);
        block.x = newPos.x();
        block.y = newPos.y();
    }

    ItemBlock *BlockImage = new ItemBlock(this);
    BlockImage->setBlockData(block, &mergedSet, &animator);
    BlockImage->setLocked(lock_block);

    if(PasteFromBuffer) BlockImage->setSelected(true);
}


void LvlScene::placeBGO(LevelBGO &bgo, bool toGrid)
{
    long item_i = 0;
    bool noimage=true;
    long animator=0;
    obj_bgo mergedSet;

    getConfig_BGO(bgo.id, item_i, animator, mergedSet, &noimage);

    QPoint newPos = QPoint(bgo.x, bgo.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(bgo.x, bgo.y), mergedSet.grid,
                           QPoint(mergedSet.offsetX, mergedSet.offsetY));
        bgo.x = newPos.x();
        bgo.y = newPos.y();
    }

    ItemBGO *BGOItem = new ItemBGO(this);
    BGOItem->setBGOData(bgo, &mergedSet, &animator);
    BGOItem->setLocked(lock_bgo);

    if(PasteFromBuffer) BGOItem->setSelected(true);
}










void LvlScene::placeNPC(LevelNPC &npc, bool toGrid)
{

    long item_i = 0;
    bool noimage=true;
    long animator=0;
    obj_npc mergedSet;
    getConfig_NPC(npc.id, item_i, animator, mergedSet, &noimage);

    ItemNPC *NPCItem = new ItemNPC;
    NPCItem->setScenePoint(this);
    NPCItem->setNpcData(npc);
    NPCItem->localProps = mergedSet;
    NPCItem->setContextMenu(npcMenu);

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> Add to scene");
    #endif

    addItem(NPCItem);

    if(NPCItem->localProps.foreground)
        NPCItem->setZValue(Z_npcFore);
    else
    if(NPCItem->localProps.background)
        NPCItem->setZValue(Z_npcBack);
    else
        NPCItem->setZValue(Z_npcStd);

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> set Generator");
    #endif

    NPCItem->setGenerator(npc.generator, npc.generator_direct, npc.generator_type, true);

    if((mergedSet.container)&&(npc.special_data>0))
        NPCItem->setIncludedNPC(npc.special_data, true);

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> calculate grid");
    #endif
    QPoint newPos = QPoint(npc.x, npc.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(npc.x, npc.y), NPCItem->gridSize,
                           QPoint(NPCItem->localProps.grid_offset_x,
                                  NPCItem->localProps.grid_offset_y));
        npc.x = newPos.x();
        npc.y = newPos.y();
    }

    npc.is_star = mergedSet.is_star;

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> set position");
    #endif
    NPCItem->setPos( QPointF(newPos) );

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> set animation");
    #endif

    NPCItem->setAnimator(animator);
    NPCItem->changeDirection(npc.direct);

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> set flags");
    #endif

    NPCItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_npc));
    NPCItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_npc));

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> set props");
    #endif

    if(NPCItem->localProps.frames>1)
        NPCItem->setData(4, "animated");

    NPCItem->setData(ITEM_TYPE, "NPC"); // ObjType
    NPCItem->setData(ITEM_ID, QString::number(npc.id) );
    NPCItem->setData(ITEM_ARRAY_ID, QString::number(npc.array_id) );

    NPCItem->setData(ITEM_NPC_BLOCK_COLLISION,  QString::number((int)NPCItem->localProps.collision_with_blocks) );
    NPCItem->setData(ITEM_NPC_NO_NPC_COLLISION, QString::number((int)NPCItem->localProps.no_npc_collions) );

    NPCItem->setData(ITEM_WIDTH,  QString::number(NPCItem->localProps.width) ); //width
    NPCItem->setData(ITEM_HEIGHT, QString::number(NPCItem->localProps.height) ); //height

    if(PasteFromBuffer) NPCItem->setSelected(true);
    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> done");
    #endif
}








void LvlScene::placeWater(LevelPhysEnv &water, bool toGrid)
{
    ItemWater *WATERItem = new ItemWater();

    QPoint newPos = QPoint(water.x, water.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(water.x, water.y), 16);
        water.x = newPos.x();
        water.y = newPos.y();
    }

    WATERItem->setScenePoint(this);
    WATERItem->setContextMenu(waterMenu);
    WATERItem->setWaterData(water);
    WATERItem->drawWater();

    this->addItem( WATERItem );
    WATERItem->setPos(QPointF(newPos));

    WATERItem->gridSize = pConfigs->default_grid/2;

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("WaterDraw -> Scene x=%1").arg(WATERItem->pos().x()));
    WriteToLog(QtDebugMsg, QString("WaterDraw -> Scene y=%1").arg(WATERItem->pos().y()));
    #endif

    WATERItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_water));
    WATERItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_water));

    WATERItem->setZValue(Z_sys_PhysEnv);

    WATERItem->setData(ITEM_TYPE, "Water"); // ObjType
    WATERItem->setData(ITEM_ID, QString::number(0) );
    WATERItem->setData(ITEM_ARRAY_ID, QString::number(water.array_id) );
    if(PasteFromBuffer) WATERItem->setSelected(true);
}










void LvlScene::placePlayerPoint(PlayerPoint plr, bool init)
{
    ItemPlayerPoint *	player = NULL;
    bool found=false;
    if(!init)
    {
        foreach(QGraphicsItem * plrt, this->items())
        {
            if( (plrt->data(ITEM_TYPE).toString()=="playerPoint")&&((unsigned int)plrt->data(ITEM_ARRAY_ID).toInt()==plr.id) )
            {
                player = dynamic_cast<ItemPlayerPoint *>(plrt);
                found=true;
                break;
            }
        }
    }

    if(found)
    {
        player->setPos(plr.x, plr.y);
        player->pointData = plr;
        player->arrayApply();
    }
    else
    {
        if((plr.h!=0)||(plr.w!=0)||(plr.x!=0)||(plr.y!=0))
        {
            player = new ItemPlayerPoint();
            player->setScenePoint(this);
            this->addItem(player);
            player->setPointData(plr, init);
        }
    }

}










void LvlScene::placeDoor(LevelDoors &door, bool toGrid)
{
    if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)) )
    {
        placeDoorEnter(door, toGrid, true);
    }

    if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)) )
    {
        placeDoorExit(door, toGrid, true);
    }
}










void LvlScene::placeDoorEnter(LevelDoors &door, bool toGrid, bool init)
{
    ItemDoor * doorItemEntrance;
    QPoint newPosI = QPoint(door.ix, door.iy);

    doorItemEntrance = new ItemDoor;
    doorItemEntrance->setScenePoint(this);
    doorItemEntrance->setContextMenu(DoorMenu);
    if(toGrid)
    {
        newPosI = applyGrid(QPoint(door.ix, door.iy), 16);
        door.ix = newPosI.x();
        door.iy = newPosI.y();
    }
    addItem(doorItemEntrance);
    doorItemEntrance->setDoorData(door, ItemDoor::D_Entrance, init);
}










void LvlScene::placeDoorExit(LevelDoors &door, bool toGrid, bool init)
{

    ItemDoor * doorItemExit;
    QPoint newPosO = QPoint(door.ox, door.oy);

    doorItemExit = new ItemDoor;
    doorItemExit->setScenePoint(this);
    doorItemExit->setContextMenu(DoorMenu);
    if(toGrid)
    {
        newPosO = applyGrid(QPoint(door.ox, door.oy), 16);
        door.ox = newPosO.x();
        door.oy = newPosO.y();
    }
    addItem(doorItemExit);
    doorItemExit->setDoorData(door, ItemDoor::D_Exit, init);
}




