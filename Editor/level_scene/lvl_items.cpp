/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "lvlscene.h"
#include "../edit_level/leveledit.h"

#include "../file_formats/file_formats.h"

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"


QPoint LvlScene::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    int gridX, gridY;
    if((grid)&&(gridSize>0))
    { //ATTACH TO GRID
        gridX = ((int)source.x() - (int)source.x() % gridSize);
        gridY = ((int)source.y() - (int)source.y() % gridSize);

        if((int)source.x()<0)
        {
            if( (int)source.x() < gridOffset.x()+gridX - (int)(gridSize/2) )
                gridX -= gridSize;
        }
        else
        {
            if( (int)source.x() > gridOffset.x()+gridX + (int)(gridSize/2) )
                gridX += gridSize;
        }

        if((int)source.y()<0)
        {if( (int)source.y() < gridOffset.y()+gridY - (int)(gridSize/2) )
            gridY -= gridSize;
        }
        else {if( (int)source.y() > gridOffset.y()+gridY + (int)(gridSize/2) )
         gridY += gridSize;
        }

        return QPoint(gridOffset.x()+gridX, gridOffset.y()+gridY);

    }
    else
        return source;
}


QPixmap LvlScene::getNPCimg(unsigned long npcID)
{
    bool noimage=true, found=false;
    bool isUser=false, isUserTxt=false;
    int j;
    QPixmap tempI;
    int gfxH = 0;

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
                gfxH = uNPCs[index_npc[npcID].i].merged.gfx_h;
            else
                gfxH = pConfigs->main_npc[index_npc[npcID].gi].height;
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
        for(j=0;j<uNPCs.size();j++)
        {
            if(uNPCs[j].id == npcID)
            {
                if(uNPCs[j].withImg)
                {
                    isUser=true;
                    noimage=false;
                    tempI = uNPCs[j].image;
                }
                if(uNPCs[j].withTxt)
                {
                    isUserTxt = true;
                    gfxH = uNPCs[j].merged.gfx_h;
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
                    gfxH =  pConfigs->main_npc[j].gfx_h;
                break;
            }
        }
    }

    if((noimage)||(tempI.isNull()))
    {
        return uNpcImg;
    }

    return tempI.copy(0,0, tempI.width(), gfxH );
}



////////////////////////////////// Place new ////////////////////////////////

void LvlScene::placeBlock(LevelBlock &block, bool toGrid)
{
    bool noimage=true, found=false;
    bool isUser=false;
    int j;

    //QGraphicsItem *npc = NULL;
    //QGraphicsItemGroup *includedNPC;
    ItemBlock *BlockImage = new ItemBlock;

    noimage=true;
    isUser=false;


    //Check Index exists
    if(block.id < (unsigned int)index_blocks.size())
    {
        j = index_blocks[block.id].i;

        if(j<pConfigs->main_block.size())
        {
        if(pConfigs->main_block[j].id == block.id)
            found=true;
        //WriteToLog(QtDebugMsg, QString("ItemPlacer -> Index: %1[i=%2], value: %3").arg(block.id).arg(j).arg(pConfigs->main_block[j].id));
        }
    }


    //if Index found
    if(found)
    {   //get neccesary element directly
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

        for(j=0;j<pConfigs->main_block.size();j++)
        {
            if(pConfigs->main_block[j].id==block.id)
            {
                noimage=false;
                if(!isUser)
                    tImg = pConfigs->main_block[j].image; break;
            }
        }

        //WriteToLog(QtDebugMsg, QString("ItemPlacer -> Found by Fetch %1").arg(j));
    }

    if((noimage)||(tImg.isNull()))
    {
        //if(block.id==89) WriteToLog(QtDebugMsg, QString("Block 89 is %1, %2").arg(noimage).arg(tImg.isNull()));
        tImg = uBlockImg;
    }

    BlockImage->setBlockData(block, pConfigs->main_block[j].sizable);
    BlockImage->setMainPixmap(tImg);
    addItem(BlockImage);

    BlockImage->setContextMenu(blockMenu);

    if((!noimage) && (pConfigs->main_block[j].animated))
    {
        BlockImage->setAnimation(pConfigs->main_block[j].frames, pConfigs->main_block[j].framespeed, pConfigs->main_block[j].algorithm);
        BlockImage->setData(4, "animated");
    }

    //includedNPC = new QGraphicsItemGroup(BlockImage);

    //Set pointers
    BlockImage->setScenePoint(this);
    //BlockImage->setGroupPoint(includedNPC);
    //BlockImage->setNPCItemPoint(npc);

    if(block.invisible)
        BlockImage->setOpacity(qreal(0.5));

    QPoint newPos = QPoint(block.x, block.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(block.x, block.y), 32);
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
    bool isUser=false;

    noimage=true;
    isUser=false;

    //Check Index exists
    if(bgo.id < (unsigned int)index_bgo.size())
    {
        j = index_bgo[bgo.id].i;

        if(j<pConfigs->main_bgo.size())
        {
        if(pConfigs->main_bgo[j].id == bgo.id)
            found=true;
        }
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_bgo[bgo.id].type==1)
        {
            isUser=true;
            noimage=false;
            tImg = uBGOs[index_bgo[bgo.id].i].image;
        }
        else
        {
            tImg = pConfigs->main_bgo[index_bgo[bgo.id].i].image;
            noimage=false;
        }
    }
    else
    {
        //fetching arrays
        for(j=0;j<uBGOs.size();j++)
        {
            if(uBGOs[j].id==bgo.id)
            {
                isUser=true;
                noimage=false;
                tImg = uBGOs[j].image;
                break;
            }
        }

        for(j=0;j<pConfigs->main_bgo.size();j++)
        {
            if(pConfigs->main_bgo[j].id==bgo.id)
            {
                noimage=false;
                if(!isUser)
                tImg = pConfigs->main_bgo[j].image; break;
            }
        }
    }

    if((noimage)||(tImg.isNull()))
    {
        tImg=uBgoImg;
    }

    BGOItem->setBGOData(bgo);
        BGOItem->gridSize = pConfigs->main_bgo[j].grid;
        BGOItem->gridOffsetX = pConfigs->main_bgo[j].offsetX;
        BGOItem->gridOffsetY = pConfigs->main_bgo[j].offsetY;
    BGOItem->setMainPixmap(tImg);
    BGOItem->setContextMenu(bgoMenu);
    addItem(BGOItem);

    //WriteToLog(QtDebugMsg, QString("BGO Item-> source data %1 %2").arg(bgo.x).arg(bgo.y));

    QPoint newPos = QPoint(bgo.x, bgo.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(bgo.x, bgo.y), pConfigs->main_bgo[j].grid,
                           QPoint(pConfigs->main_bgo[j].offsetX, pConfigs->main_bgo[j].offsetY));
        bgo.x = newPos.x();
        bgo.y = newPos.y();
    }

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 1 %1 %2").arg(bgo.x).arg(bgo.y));

    BGOItem->setPos( QPointF(newPos) );

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 2 %1 %2").arg(newPos.x()).arg(newPos.y()));
    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 3 %1 %2").arg(BGOItem->pos().x()).arg(BGOItem->pos().y()));


    if((!noimage) && (pConfigs->main_bgo[j].animated))
    {
        //tImg=tImg.copy(0, 0, tImg.width(), (int)round(tImg.height()/pConfigs->main_bgo[j].frames));
        BGOItem->setAnimation(pConfigs->main_bgo[j].frames, pConfigs->main_bgo[j].framespeed);
        BGOItem->setData(4, "animated");
    }

    BGOItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_bgo));
    BGOItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_bgo));

    BGOItem->setData(0, "BGO");
    BGOItem->setData(1, QString::number(bgo.id) );
    BGOItem->setData(2, QString::number(bgo.array_id) );

    BGOItem->setData(9, QString::number(tImg.width()) ); //width
    BGOItem->setData(10, QString::number(tImg.height()) ); //height

    if(pConfigs->main_bgo[j].view!=0)
        BGOItem->setZValue(bgoZf + pConfigs->main_bgo[j].zOffset);
        //bgoback->addToGroup(box);
    else
        BGOItem->setZValue(bgoZb + pConfigs->main_bgo[j].zOffset);
        //bgofore->addToGroup(box);

    BGOItem->setScenePoint(this);

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
    }


    NPCItem->setScenePoint(this);

        //WriteToLog(QtDebugMsg, "NPC place -> set Data");
    NPCItem->setNpcData(npc);

        //WriteToLog(QtDebugMsg, "NPC place -> set Props");
    NPCItem->localProps = mergedSet;

        //WriteToLog(QtDebugMsg, "NPC place -> set Pixmap");
    NPCItem->setMainPixmap(tImg);

        //WriteToLog(QtDebugMsg, "NPC place -> set ContextMenu");
    NPCItem->setContextMenu(npcMenu);

        WriteToLog(QtDebugMsg, "NPC place -> Add to scene");

    addItem(NPCItem);

    if(NPCItem->localProps.foreground)
        NPCItem->setZValue(npcZf);
    else
    if(NPCItem->localProps.background)
        NPCItem->setZValue(npcZb);
    else
        NPCItem->setZValue(npcZs);

        WriteToLog(QtDebugMsg, "NPC place -> set Generator");
    NPCItem->setGenerator(npc.generator, npc.generator_direct, npc.generator_type, true);

    if((mergedSet.container)&&(npc.special_data>0))
        NPCItem->setIncludedNPC(npc.special_data, true);

        WriteToLog(QtDebugMsg, "NPC place -> calculate grid");
    QPoint newPos = QPoint(npc.x, npc.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(npc.x, npc.y), NPCItem->gridSize,
                           QPoint(NPCItem->localProps.grid_offset_x,
                                  NPCItem->localProps.grid_offset_y));
        npc.x = newPos.x();
        npc.y = newPos.y();
    }


        WriteToLog(QtDebugMsg, "NPC place -> set position");
    NPCItem->setPos( QPointF(newPos) );

        WriteToLog(QtDebugMsg, "NPC place -> set animation");
    NPCItem->setAnimation(NPCItem->localProps.frames,
                          NPCItem->localProps.framespeed,
                          NPCItem->localProps.framestyle,
                          npc.direct,
                          NPCItem->localProps.custom_animate,
                          NPCItem->localProps.custom_ani_fl,
                          NPCItem->localProps.custom_ani_el,
                          NPCItem->localProps.custom_ani_fr,
                          NPCItem->localProps.custom_ani_er);

        WriteToLog(QtDebugMsg, "NPC place -> set flags");
    NPCItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_npc));
    NPCItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_npc));

    //npcfore->addToGroup(box);

        WriteToLog(QtDebugMsg, "NPC place -> set props");
    if(NPCItem->localProps.frames>1)
        NPCItem->setData(4, "animated");

    NPCItem->setData(0, "NPC"); // ObjType
    NPCItem->setData(1, QString::number(npc.id) );
    NPCItem->setData(2, QString::number(npc.array_id) );

    NPCItem->setData(9, QString::number(NPCItem->localProps.width) ); //width
    NPCItem->setData(10, QString::number(NPCItem->localProps.height) ); //height

    if(PasteFromBuffer) NPCItem->setSelected(true);
    WriteToLog(QtDebugMsg, "NPC place -> done");
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

    WriteToLog(QtDebugMsg, QString("WaterDraw -> Scene x=%1").arg(WATERItem->pos().x()));
    WriteToLog(QtDebugMsg, QString("WaterDraw -> Scene y=%1").arg(WATERItem->pos().y()));

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
            //player->setFlag(QGraphicsItem::ItemIsMovable, true);
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

    ItemDoor * doorItemEntrance;
    QPoint newPosI = QPoint(door.ix, door.iy);

    if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)) )
    {
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
        doorItemEntrance->setDoorData(door, ItemDoor::D_Entrance, true);
    }


    ItemDoor * doorItemExit;
    QPoint newPosO = QPoint(door.ox, door.oy);
    if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)) )
    {
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
        doorItemExit->setDoorData(door, ItemDoor::D_Exit, true);
    }


}
