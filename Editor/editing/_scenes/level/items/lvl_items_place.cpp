/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <PGE_File_Formats/file_formats.h>

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

    QPoint newPos = QPoint(block.x, block.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(block.x, block.y), mergedSet.grid);
        block.x = newPos.x();
        block.y = newPos.y();
    }

    ItemBlock *BlockImage = new ItemBlock(this);
    BlockImage->setBlockData(block, &mergedSet, &animator);

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

    if(PasteFromBuffer) BGOItem->setSelected(true);
}








void LvlScene::placeNPC(LevelNPC &npc, bool toGrid)
{
    long item_i = 0;
    bool noimage=true;
    long animator=0;
    obj_npc mergedSet;

    getConfig_NPC(npc.id, item_i, animator, mergedSet, &noimage);

    QPoint newPos = QPoint(npc.x, npc.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(npc.x, npc.y), mergedSet.grid,
                           QPoint(mergedSet.grid_offset_x,
                                  mergedSet.grid_offset_y));
        npc.x = newPos.x();
        npc.y = newPos.y();
    }

    ItemNPC *NPCItem = new ItemNPC(this);
    NPCItem->setNpcData(npc, &mergedSet, &animator);

    if(PasteFromBuffer) NPCItem->setSelected(true);
}







void LvlScene::placeWater(LevelPhysEnv &water, bool toGrid)
{
    QPoint newPos = QPoint(water.x, water.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(water.x, water.y), 16);
        water.x = newPos.x();
        water.y = newPos.y();
    }

    ItemWater *WATERItem = new ItemWater(this);
    WATERItem->setWaterData(water);

    if(PasteFromBuffer) WATERItem->setSelected(true);
}










void LvlScene::placePlayerPoint(PlayerPoint plr, bool init)
{
    ItemPlayerPoint * player = NULL;
    bool found=false;
    if(!init)
    {
        foreach(QGraphicsItem * plrt, this->items())
        {
            if(
                 (plrt->data(ITEM_TYPE).toString()=="playerPoint")&&
                ((unsigned int)plrt->data(ITEM_ARRAY_ID).toInt()==plr.id)
               )
            {
                player = dynamic_cast<ItemPlayerPoint *>(plrt);
                found=true;
                break;
            }
        }
    }

    if(found)
    {
        player->pointData = plr;
        player->setPos(plr.x, plr.y);
        player->arrayApply();
    }
    else
    {
        if((plr.h!=0)||(plr.w!=0)||(plr.x!=0)||(plr.y!=0))
        {
            player = new ItemPlayerPoint(this);
            player->setPointData(plr, init);
        }
    }

}










void LvlScene::placeDoor(LevelDoor &door, bool toGrid)
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










void LvlScene::placeDoorEnter(LevelDoor &door, bool toGrid, bool init)
{
    ItemDoor * doorItemEntrance;

    QPoint newPosI = QPoint(door.ix, door.iy);
    if(toGrid)
    {
        newPosI = applyGrid(QPoint(door.ix, door.iy), 16);
        door.ix = newPosI.x();
        door.iy = newPosI.y();
    }

    doorItemEntrance = new ItemDoor(this);
    doorItemEntrance->setDoorData(door, ItemDoor::D_Entrance, init);
}










void LvlScene::placeDoorExit(LevelDoor &door, bool toGrid, bool init)
{

    ItemDoor * doorItemExit;
    QPoint newPosO = QPoint(door.ox, door.oy);
    if(toGrid)
    {
        newPosO = applyGrid(QPoint(door.ox, door.oy), 16);
        door.ox = newPosO.x();
        door.oy = newPosO.y();
    }

    doorItemExit = new ItemDoor(this);
    doorItemExit->setDoorData(door, ItemDoor::D_Exit, init);
}




