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

#include "lvlscene.h"
#include "../edit_level/leveledit.h"

#include "../file_formats/file_formats.h"

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"

#include "../common_features/grid.h"


QPoint LvlScene::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    if((grid)&&(gridSize>0))
        return Grid::applyGrid(source, gridSize, gridOffset);
    else
        return source;
}


QPixmap LvlScene::getNPCimg(unsigned long npcID, int Direction)
{
    bool noimage=true, found=false;
    bool isUser=false, isUserTxt=false;
    int j, q;
    QPixmap tempI;
    int gfxH = 0;
    obj_npc merged;

    //Check Index exists
    if(npcID < (unsigned int)index_npc.size())
    {
        j = index_npc[npcID].gi;
        if(j<pConfigs->main_npc.size())
        {
            if(pConfigs->main_npc[j].id == npcID)
                found=true;
        }
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_npc[npcID].type==1)
        {
            isUser=true;
            if(uNPCs[index_npc[npcID].i].withImg)
            {
                noimage=false;
                tempI = uNPCs[index_npc[npcID].i].image;
            }
            if(uNPCs[index_npc[npcID].i].withTxt)
            {
                gfxH = uNPCs[index_npc[npcID].i].merged.gfx_h;
                merged = uNPCs[index_npc[npcID].i].merged;
            }
            else
            {
                gfxH = pConfigs->main_npc[index_npc[npcID].gi].height;
                merged = pConfigs->main_npc[index_npc[npcID].gi];
            }
        }

        if(!noimage)
        {
            tempI = pConfigs->main_npc[(isUser) ? index_npc[npcID].gi : index_npc[npcID].i].image;
            noimage=false;
        }

    }
    else
    {
        //found neccesary element in arrays and select
        for(q=0;q<uNPCs.size();q++)
        {
            if(uNPCs[q].id == npcID)
            {
                if(uNPCs[q].withImg)
                {
                    isUser=true;
                    noimage=false;
                    tempI = uNPCs[q].image;
                }
                if(uNPCs[q].withTxt)
                {
                    isUserTxt = true;
                    gfxH = uNPCs[q].merged.gfx_h;
                    merged = uNPCs[q].merged;
                }
                break;
            }
        }

        for(j=0;j<pConfigs->main_npc.size();j++)
        {
            if(pConfigs->main_npc[j].id==npcID)
            {
                noimage=false;
                if(!isUser)
                    tempI = pConfigs->main_npc[j].image;
                if(!isUserTxt)
                {
                    gfxH =  pConfigs->main_npc[j].gfx_h;
                    merged = pConfigs->main_npc[j];
                }
                break;
            }
        }
    }

    if((noimage)||(tempI.isNull()))
    {
        return uNpcImg;
    }

    if(Direction<=0)
    {
        int frame=0;
        if(merged.custom_animate)
        {
            frame=merged.custom_ani_fl;
        }
        return tempI.copy(0,frame*gfxH, tempI.width(), gfxH );
    }
    else
    {
        int frame=0;
        int framesQ;
        if(merged.custom_animate)
        {
            frame=merged.custom_ani_fr;
        }
        else
        {
            switch(merged.framestyle)
            {
            case 2:
                framesQ = merged.frames * 4;
                frame = (int)(framesQ-(framesQ/4)*3);
                break;
            case 1:
                framesQ = merged.frames * 2;
                frame = (int)(framesQ / 2);
                break;
            case 0:
            default:
                break;
            }

        }
        return tempI.copy(0,frame*gfxH, tempI.width(), gfxH );
    }

}



////////////////////////////////// Place new ////////////////////////////////

void LvlScene::placeBlock(LevelBlock &block, bool toGrid)
{
    bool noimage=true, found=false;
    int j;
    long animator=0;

    ItemBlock *BlockImage = new ItemBlock;

    //Check Index exists
    if(block.id < (unsigned int)index_blocks.size())
    {
        j = index_blocks[block.id].i;
        animator = index_blocks[block.id].ai;

        if(j<pConfigs->main_block.size())
        {
            if(pConfigs->main_block[j].id == block.id)
            {
                found=true;noimage=false;
            }
        //WriteToLog(QtDebugMsg, QString("ItemPlacer -> Index: %1[i=%2], value: %3").arg(block.id).arg(j).arg(pConfigs->main_block[j].id));
        }
    }


    //if Index found
    if(!found)
    {
        /*//get neccesary element directly
        if(index_blocks[block.id].type==1)
        {
            isUser=true;
            noimage=false;
            tImg = uBlocks[index_blocks[block.id].i].image;
        }
        else
        {
            tImg = pConfigs->main_block[index_blocks[block.id].i].image;
            noimage=false;
        }
       // WriteToLog(QtDebugMsg, QString("ItemPlacer -> Found by Index %1").arg(block.id));
    }
    else
    {
        //found neccesary element in arrays and select
        for(j=0;j<uBlocks.size();j++)
        {
            if(uBlocks[j].id == block.id)
            {
                isUser=true;
                noimage=false;
                tImg = uBlocks[j].image;
                break;
            }
        }
    */
        for(j=0;j<pConfigs->main_block.size();j++)
        {
            if(pConfigs->main_block[j].id==block.id)
            {
                noimage=false;
                //if(!isUser)
            }
        }

        //WriteToLog(QtDebugMsg, QString("ItemPlacer -> Found by Fetch %1").arg(j));
    }

    if(noimage)
    {
        if(j >= pConfigs->main_block.size())
        {
            j=0;
        }
    }

    BlockImage->setBlockData(block, pConfigs->main_block[j].sizable);
    BlockImage->gridSize = pConfigs->main_block[j].grid;
    //BlockImage->setMainPixmap(tImg);
    addItem(BlockImage);

    //Set pointers
    BlockImage->setScenePoint(this);


    BlockImage->setContextMenu(blockMenu);

    BlockImage->setAnimator(animator);

    if((!noimage) && (pConfigs->main_block[j].animated))
    {
        //BlockImage->setAnimation(pConfigs->main_block[j].frames, pConfigs->main_block[j].framespeed, pConfigs->main_block[j].algorithm);
        BlockImage->setData(4, "animated");
    }

    //includedNPC = new QGraphicsItemGroup(BlockImage);
    //BlockImage->setGroupPoint(includedNPC);
    //BlockImage->setNPCItemPoint(npc);

    if(block.invisible)
        BlockImage->setOpacity(qreal(0.5));

    QPoint newPos = QPoint(block.x, block.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(block.x, block.y), BlockImage->gridSize);
        block.x = newPos.x();
        BlockImage->blockData.x = newPos.x();
        block.y = newPos.y();
        BlockImage->blockData.y = newPos.y();
    }

    BlockImage->setPos(QPointF(newPos));

    //////////////////////////////Included NPC////////////////////////////////////////
    if(block.npc_id != 0)
    {
        BlockImage->setIncludedNPC(block.npc_id, true);
    }
    //////////////////////////////////////////////////////////////////////////////////

    if(pConfigs->main_block[j].sizable)
    {
        BlockImage->setMainPixmap();
        BlockImage->setZValue( blockZs + ((double)block.y/(double)100000000000) + 1 - ((double)block.w * (double)0.0000000000000001) ); // applay sizable block Z
        //sbZ += 0.0000000001;
    }
    else
    {
        if(pConfigs->main_block[j].view==1)
            BlockImage->setZValue(blockZl); // applay lava block Z
        else
            BlockImage->setZValue(blockZ); // applay standart block Z
    }

    BlockImage->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_block));
    BlockImage->setFlag(QGraphicsItem::ItemIsMovable, (!lock_block));

    BlockImage->setData(0, "Block");
    BlockImage->setData(1, QString::number(block.id) );
    BlockImage->setData(2, QString::number(block.array_id) );


    if(pConfigs->main_block[j].sizable)
    {
        BlockImage->setData(3, "sizable" );
    }
    else
        BlockImage->setData(3, "standart" );

    BlockImage->setData(9, QString::number(block.w) ); //width
    BlockImage->setData(10, QString::number(block.h) ); //height
    if(PasteFromBuffer) BlockImage->setSelected(true);
}


void LvlScene::placeBGO(LevelBGO &bgo, bool toGrid)
{
    int j;
    bool noimage=true, found=false;

    ItemBGO *BGOItem = new ItemBGO;
    //bool isUser=false;

    noimage=true;
    //isUser=false;
    long animator=0;

    //Check Index exists
    if(bgo.id < (unsigned int)index_bgo.size())
    {
        j = index_bgo[bgo.id].i;
        animator = index_bgo[bgo.id].ai;
        if(j<pConfigs->main_bgo.size())
        {
        if(pConfigs->main_bgo[j].id == bgo.id)
            found=true;
        }
    }

    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_bgo.size();j++)
        {
            if(pConfigs->main_bgo[j].id==bgo.id)
            {
                noimage=false;
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_bgo.size())
        {
            j=0;
        }
    }

    QPoint newPos = QPoint(bgo.x, bgo.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(bgo.x, bgo.y), pConfigs->main_bgo[j].grid,
                           QPoint(pConfigs->main_bgo[j].offsetX, pConfigs->main_bgo[j].offsetY));
        bgo.x = newPos.x();
        bgo.y = newPos.y();
    }

    BGOItem->setScenePoint(this);

    BGOItem->setBGOData(bgo);
    BGOItem->gridSize = pConfigs->main_bgo[j].grid;
    BGOItem->gridOffsetX = pConfigs->main_bgo[j].offsetX;
    BGOItem->gridOffsetY = pConfigs->main_bgo[j].offsetY;
    BGOItem->setAnimator(animator);
    //BGOItem->setMainPixmap(tImg);
    BGOItem->setContextMenu(bgoMenu);
    addItem(BGOItem);


    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("BGO Item-> grid config value %1").arg(pConfigs->main_bgo[j].grid));
        WriteToLog(QtDebugMsg, QString("BGO Item-> grid value %1").arg(BGOItem->gridSize));
        WriteToLog(QtDebugMsg, QString("BGO Item-> j value %1").arg(j));
    #endif

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 1 %1 %2").arg(bgo.x).arg(bgo.y));

    BGOItem->setPos( QPointF(newPos) );

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 2 %1 %2").arg(newPos.x()).arg(newPos.y()));
    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 3 %1 %2").arg(BGOItem->pos().x()).arg(BGOItem->pos().y()));


    if((!noimage) && (pConfigs->main_bgo[j].animated))
    {
        //tImg=tImg.copy(0, 0, tImg.width(), (int)round(tImg.height()/pConfigs->main_bgo[j].frames));
        //BGOItem->setAnimation(pConfigs->main_bgo[j].frames, pConfigs->main_bgo[j].framespeed);
        BGOItem->setData(4, "animated");
    }

    BGOItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_bgo));
    BGOItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_bgo));

    BGOItem->setData(0, "BGO");
    BGOItem->setData(1, QString::number(bgo.id) );
    BGOItem->setData(2, QString::number(bgo.array_id) );


    if(pConfigs->main_bgo[j].view!=0)
        BGOItem->setZValue(bgoZf + pConfigs->main_bgo[j].zOffset);
    else
        BGOItem->setZValue(bgoZb + pConfigs->main_bgo[j].zOffset);
    if(PasteFromBuffer) BGOItem->setSelected(true);
}

void LvlScene::placeNPC(LevelNPC &npc, bool toGrid)
{

    int j;
    bool noimage=true, found=false;
    bool isUser=false;
    bool isUserTxt=false;
    obj_npc mergedSet;

    ItemNPC *NPCItem = new ItemNPC;

    //Check Index exists
    if(npc.id < (unsigned int)index_npc.size())
    {
        j = index_npc[npc.id].gi;

        if(j<pConfigs->main_npc.size())
        {
        if(pConfigs->main_npc[j].id == npc.id)
            found=true;
        }
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_npc[npc.id].type==1)
        {
            isUser=true;
            if(uNPCs[index_npc[npc.id].i].withImg)
            {
                noimage=false;
                tImg = uNPCs[index_npc[npc.id].i].image;
            }

            if(uNPCs[index_npc[npc.id].i].withTxt)
            {
                isUserTxt=true;
                mergedSet = uNPCs[index_npc[npc.id].i].merged;
            }
            else
                mergedSet = pConfigs->main_npc[index_npc[npc.id].gi];
        }

        if(!noimage)
        {
            tImg = pConfigs->main_npc[ index_npc[npc.id].gi].image;
            noimage=false;
        }
    }
    else
    {
        //fetching arrays
        for(j=0;j<uNPCs.size();j++)
        {
            if(uNPCs[j].id==npc.id)
            {
                if(uNPCs[j].withImg)
                {
                    noimage=false;
                    isUser=true;
                    tImg = uNPCs[j].image;
                }

                if(uNPCs[j].withTxt)
                {
                    isUserTxt=true;
                    mergedSet = uNPCs[j].merged;
                }
                break;
            }
        }

        for(j=0;j<pConfigs->main_npc.size();j++)
        {
            if(pConfigs->main_npc[j].id==npc.id)
            {
                noimage=false;
                if(!isUser)
                    tImg = pConfigs->main_npc[j].image;
                if(!isUserTxt)
                    mergedSet = pConfigs->main_npc[j];
                break;
            }
        }
    }

    if((noimage)||(tImg.isNull()))
    {
        tImg=uNpcImg;
        if(j >= pConfigs->main_npc.size())
        {
            j=0;
            mergedSet = pConfigs->main_npc[j];
        }
    }


    NPCItem->setScenePoint(this);

        //WriteToLog(QtDebugMsg, "NPC place -> set Data");
    NPCItem->setNpcData(npc);

        //WriteToLog(QtDebugMsg, "NPC place -> set Props");
    NPCItem->localProps = mergedSet;
    NPCItem->setData(8, QString::number((int)mergedSet.no_npc_collions));

        //WriteToLog(QtDebugMsg, "NPC place -> set Pixmap");
    NPCItem->setMainPixmap(tImg);

        //WriteToLog(QtDebugMsg, "NPC place -> set ContextMenu");
    NPCItem->setContextMenu(npcMenu);

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> Add to scene");
    #endif

    addItem(NPCItem);

    if(NPCItem->localProps.foreground)
        NPCItem->setZValue(npcZf);
    else
    if(NPCItem->localProps.background)
        NPCItem->setZValue(npcZb);
    else
        NPCItem->setZValue(npcZs);

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

    NPCItem->setAnimation(NPCItem->localProps.frames,
                          NPCItem->localProps.framespeed,
                          NPCItem->localProps.framestyle,
                          npc.direct,
                          NPCItem->localProps.custom_animate,
                          NPCItem->localProps.custom_ani_fl,
                          NPCItem->localProps.custom_ani_el,
                          NPCItem->localProps.custom_ani_fr,
                          NPCItem->localProps.custom_ani_er);

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> set flags");
    #endif

    NPCItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_npc));
    NPCItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_npc));

    //npcfore->addToGroup(box);

    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> set props");
    #endif
    if(NPCItem->localProps.frames>1)
        NPCItem->setData(4, "animated");

    NPCItem->setData(0, "NPC"); // ObjType
    NPCItem->setData(1, QString::number(npc.id) );
    NPCItem->setData(2, QString::number(npc.array_id) );

    NPCItem->setData(9, QString::number(NPCItem->localProps.width) ); //width
    NPCItem->setData(10, QString::number(NPCItem->localProps.height) ); //height

    if(PasteFromBuffer) NPCItem->setSelected(true);
    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> done");
    #endif
}

void LvlScene::placeWater(LevelWater &water, bool toGrid)
{
    ItemWater *WATERItem = new ItemWater();
    //QGraphicsPolygonItem *	WATERItem;
    //if(!progress.wasCanceled())
    //    progress.setLabelText("Applayng water "+QString::number(i)+"/"+QString::number(FileData.water.size()));

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

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("WaterDraw -> Scene x=%1").arg(WATERItem->pos().x()));
    WriteToLog(QtDebugMsg, QString("WaterDraw -> Scene y=%1").arg(WATERItem->pos().y()));
    #endif

    WATERItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_water));
    WATERItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_water));

    WATERItem->setZValue(waterZ);

    WATERItem->setData(0, "Water"); // ObjType
    WATERItem->setData(1, QString::number(0) );
    WATERItem->setData(2, QString::number(water.array_id) );
}

void LvlScene::placePlayerPoint(PlayerPoint plr, bool init)
{
    QGraphicsItem *	player = NULL;
    bool found=false;
    if(!init)
    {
        foreach(QGraphicsItem * plrt, this->items())
        {
            if( plrt->data(0).toString()=="player"+QString::number(plr.id) )
            {
                player = plrt;
                found=true;
                break;
            }
        }
    }

    if(found)
    {
        player->setPos(plr.x, plr.y);
        for(int i=0; i<LvlData->players.size(); i++)
        {
         if(LvlData->players[i].id == plr.id)
         { LvlData->players[i] = plr; break; }
        }
    }
    else
    {
        if((plr.h!=0)||(plr.w!=0)||(plr.x!=0)||(plr.y!=0))
        {
            player = addPixmap(QPixmap(":/player"+QString::number(plr.id)+".png"));
            player->setPos(plr.x, plr.y);
            player->setZValue(playerZ);
            player->setData(0, "player"+QString::number(plr.id) );
            player->setData(2, QString::number(plr.id));
            player->setFlag(QGraphicsItem::ItemIsSelectable, true);
            player->setFlag(QGraphicsItem::ItemIsMovable, true);
            if(!init)
            {
                for(int i=0; i<LvlData->players.size(); i++)
                {
                 if(LvlData->players[i].id == plr.id)
                 { LvlData->players[i] = plr; break; }
                }
            }
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
