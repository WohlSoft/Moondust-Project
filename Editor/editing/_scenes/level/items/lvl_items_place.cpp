/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../lvl_scene.h"
#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "item_playerpoint.h"



void LvlScene::placeBlock(LevelBlock &block, bool toGrid)
{
    obj_block &mergedSet = m_localConfigBlocks[block.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_block[1];
        mergedSet.image = m_dummyBlockImg;
    }

    QPoint newPos = QPoint(block.x, block.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(block.x, block.y), mergedSet.setup.grid);
        block.x = newPos.x();
        block.y = newPos.y();
    }

    ItemBlock *BlockImage = new ItemBlock(this);
    block.meta.userdata = BlockImage;
    BlockImage->setBlockData(block, &mergedSet, &animator);

    if(m_pastingMode) BlockImage->setSelected(true);
}

void LvlScene::placeBGO(LevelBGO &bgo, bool toGrid)
{
    obj_bgo &mergedSet = m_localConfigBGOs[bgo.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_bgo[1];
        mergedSet.image = m_dummyBgoImg;
    }

    QPoint newPos = QPoint(bgo.x, bgo.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(bgo.x, bgo.y), mergedSet.setup.grid,
                           QPoint(mergedSet.setup.offsetX, mergedSet.setup.offsetY));
        bgo.x = newPos.x();
        bgo.y = newPos.y();
    }

    ItemBGO *BGOItem = new ItemBGO(this);
    bgo.meta.userdata = BGOItem;
    BGOItem->setBGOData(bgo, &mergedSet, &animator);

    if(m_pastingMode) BGOItem->setSelected(true);
}

void LvlScene::placeNPC(LevelNPC &npc, bool toGrid)
{
    obj_npc &mergedSet = m_localConfigNPCs[npc.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_npc[1];
        mergedSet.image = m_dummyNpcImg;
    }

    QPoint newPos = QPoint(npc.x, npc.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(npc.x, npc.y), mergedSet.setup.grid,
                           QPoint(mergedSet.setup.grid_offset_x,
                                  mergedSet.setup.grid_offset_y));
        npc.x = newPos.x();
        npc.y = newPos.y();
    }

    ItemNPC *NPCItem = new ItemNPC(this);
    npc.meta.userdata = NPCItem;
    NPCItem->setNpcData(npc, &mergedSet, &animator);

    if(m_pastingMode) NPCItem->setSelected(true);
}

void LvlScene::placeEnvironmentZone(LevelPhysEnv &water, bool toGrid)
{
    QPoint newPos = QPoint(water.x, water.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(water.x, water.y), 16);
        water.x = newPos.x();
        water.y = newPos.y();
    }

    ItemPhysEnv *PhysEnvItem = new ItemPhysEnv(this);
    water.meta.userdata = PhysEnvItem;
    PhysEnvItem->setPhysEnvData(water);

    if(m_pastingMode) PhysEnvItem->setSelected(true);
}


void LvlScene::placePlayerPoint(PlayerPoint plr, bool init)
{
    ItemPlayerPoint * player = NULL;
    bool found = false;
    if(!init)
    {
        foreach(QGraphicsItem * plrt, this->items())
        {
            if(
                (plrt->data(ITEM_TYPE).toString() == "playerPoint") &&
                ((unsigned int)plrt->data(ITEM_ARRAY_ID).toInt()==plr.id)
               )
            {
                player = dynamic_cast<ItemPlayerPoint *>(plrt);
                found = true;
                break;
            }
        }
    }

    if(found)
    {
        player->m_data = plr;
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
    QPoint newPosI = QPoint(door.ix, door.iy);
    if(toGrid)
    {
        newPosI = applyGrid(QPoint(door.ix, door.iy), 16);
        door.ix = newPosI.x();
        door.iy = newPosI.y();
    }

    ItemDoor* doorItemEntrance = new ItemDoor(this);
    door.userdata_enter = doorItemEntrance;
    doorItemEntrance->setDoorData(door, ItemDoor::D_Entrance, init);
}



void LvlScene::placeDoorExit(LevelDoor &door, bool toGrid, bool init)
{
    QPoint newPosO = QPoint(door.ox, door.oy);
    if(toGrid)
    {
        newPosO = applyGrid(QPoint(door.ox, door.oy), 16);
        door.ox = newPosO.x();
        door.oy = newPosO.y();
    }
    ItemDoor * doorItemExit = new ItemDoor(this);
    door.userdata_exit = doorItemExit;
    doorItemExit->setDoorData(door, ItemDoor::D_Exit, init);
}



