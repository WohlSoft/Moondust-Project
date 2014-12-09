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

#include "lvl_scene.h"
#include "../../edit_level/level_edit.h"

#include "../../../file_formats/file_formats.h"

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "item_playerpoint.h"

#include "../../../common_features/grid.h"
#include "../../../common_features/mainwinconnect.h"


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
        ObjType = it->data(0).toString();
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


void LvlScene::applyArrayForItemGroup(QList<QGraphicsItem * >items)
{
    foreach(QGraphicsItem * it, items)
    {
        if(it) applyArrayForItem(it);
    }
}

void LvlScene::applyArrayForItem(QGraphicsItem * item)
{
    if(!item) return;

    QString ObjType = item->data(0).toString();
    if( ObjType == "NPC")
    {
        dynamic_cast<ItemNPC *>(item)->arrayApply();
    }
    else
    if( ObjType == "Block")
    {
        dynamic_cast<ItemBlock *>(item)->arrayApply();
    }
    else
    if( ObjType == "BGO")
    {
        dynamic_cast<ItemBGO *>(item)->arrayApply();
    }
    else
    if( ObjType == "Water")
    {
        dynamic_cast<ItemWater *>(item)->arrayApply();
    }
    else
    if(( ObjType == "Door_enter")||( ObjType == "Door_exit"))
    {
        dynamic_cast<ItemDoor *>(item)->arrayApply();
    }
    else
    if( ObjType == "playerPoint" )
    {
        dynamic_cast<ItemPlayerPoint *>(item)->arrayApply();
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
        ObjType = it->data(0).toString();
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



void LvlScene::returnItemBackGroup(QList<QGraphicsItem * >items)
{
    foreach(QGraphicsItem * it, items)
    {
        if(it) returnItemBack(it);
    }
}

void LvlScene::returnItemBack(QGraphicsItem * item)
{
    if(!item) return;

    QString ObjType = item->data(0).toString();
    if( ObjType == "NPC")
    {
        ItemNPC * it = dynamic_cast<ItemNPC *>(item);
        it->setPos(it->npcData.x, it->npcData.y);
    }
    else
    if( ObjType == "Block")
    {
        ItemBlock * it = dynamic_cast<ItemBlock *>(item);
        it->setPos(it->blockData.x, it->blockData.y);
    }
    else
    if( ObjType == "BGO")
    {
        ItemBGO * it = dynamic_cast<ItemBGO *>(item);
        it->setPos(it->bgoData.x, it->bgoData.y);
    }
    else
    if( ObjType == "Water")
    {
        ItemWater * it = dynamic_cast<ItemWater *>(item);
        it->setPos(it->waterData.x, it->waterData.y);
    }
    else
    if(ObjType == "Door_enter")
    {
        ItemDoor * it = dynamic_cast<ItemDoor *>(item);
        it->setPos(it->doorData.ix, it->doorData.iy);
    }
    else
    if(ObjType == "Door_exit")
    {
        ItemDoor * it = dynamic_cast<ItemDoor *>(item);
        it->setPos(it->doorData.ox, it->doorData.oy);
    }
    else
    if( ObjType == "playerPoint" )
    {
        ItemPlayerPoint * it = dynamic_cast<ItemPlayerPoint *>(item);
        it->setPos(it->pointData.x, it->pointData.y);
    }
}



void LvlScene::Debugger_updateItemList()
{
    QString itemList=
            tr("Player start points:\t\t%1\n"
               "Blocks:\t\t\t%2\n"
               "Background objects's:\t%3\n"
               "Non-playable characters's:\t%4\n"
               "Warp entries:\t\t%5\n"
               "Physical env. zones:\t%6\n");

    itemList = itemList.arg(LvlData->players.size())
            .arg(LvlData->blocks.size())
            .arg(LvlData->bgo.size())
            .arg(LvlData->npc.size())
            .arg(LvlData->doors.size())
            .arg(LvlData->physez.size());

    MainWinConnect::pMainWin->Debugger_UpdateItemList(itemList);
}





////////////////////////////////// Place new ////////////////////////////////

void LvlScene::placeBlock(LevelBlock &block, bool toGrid)
{
    bool noimage=true, found=false;
    int j, item_i=0;
    long animator=0;

    ItemBlock *BlockImage = new ItemBlock;

    //Check Index exists
    if(block.id < (unsigned int)index_blocks.size())
    {
        j = index_blocks[block.id].i;
        item_i = j;
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
        for(j=0;j<pConfigs->main_block.size();j++)
        {
            if(pConfigs->main_block[j].id==block.id)
            {
                noimage=false;
                item_i = j;
                break;
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_block.size())
        {
            j=0;
            item_i = j;
        }
    }

    BlockImage->setBlockData(block, pConfigs->main_block[item_i].sizable);
    BlockImage->gridSize = pConfigs->main_block[item_i].grid;
    //BlockImage->setMainPixmap(tImg);
    addItem(BlockImage);

    //Set pointers
    BlockImage->setScenePoint(this);


    BlockImage->setContextMenu(blockMenu);

    BlockImage->setAnimator(animator);

    if((!noimage) && (pConfigs->main_block[item_i].animated))
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
        BlockImage->setZValue( Z_blockSizable + ((double)block.y/(double)100000000000) + 1 - ((double)block.w * (double)0.0000000000000001) ); // applay sizable block Z
        //sbZ += 0.0000000001;
    }
    else
    {
        if(pConfigs->main_block[j].view==1)
            BlockImage->setZValue(Z_BlockFore); // applay lava block Z
        else
            BlockImage->setZValue(Z_Block); // applay standart block Z
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
    int j,item_i = 0;
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
        item_i = j;
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
                item_i = j;
                break;
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_bgo.size())
        {
            j=0;
            item_i = j;
        }
    }

    QPoint newPos = QPoint(bgo.x, bgo.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(bgo.x, bgo.y), pConfigs->main_bgo[item_i].grid,
                           QPoint(pConfigs->main_bgo[j].offsetX, pConfigs->main_bgo[item_i].offsetY));
        bgo.x = newPos.x();
        bgo.y = newPos.y();
    }

    BGOItem->setScenePoint(this);

    BGOItem->setBGOData(bgo);
    BGOItem->gridSize = pConfigs->main_bgo[item_i].grid;
    BGOItem->gridOffsetX = pConfigs->main_bgo[item_i].offsetX;
    BGOItem->gridOffsetY = pConfigs->main_bgo[item_i].offsetY;
    BGOItem->setAnimator(animator);
    //BGOItem->setMainPixmap(tImg);
    BGOItem->setContextMenu(bgoMenu);
    addItem(BGOItem);


    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("BGO Item-> grid config value %1").arg(pConfigs->main_bgo[item_i].grid));
        WriteToLog(QtDebugMsg, QString("BGO Item-> grid value %1").arg(BGOItem->gridSize));
        WriteToLog(QtDebugMsg, QString("BGO Item-> j value %1").arg(j));
    #endif

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 1 %1 %2").arg(bgo.x).arg(bgo.y));

    BGOItem->setPos( QPointF(newPos) );

    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 2 %1 %2").arg(newPos.x()).arg(newPos.y()));
    //WriteToLog(QtDebugMsg, QString("BGO Item-> new data pos 3 %1 %2").arg(BGOItem->pos().x()).arg(BGOItem->pos().y()));


    if((!noimage) && (pConfigs->main_bgo[item_i].animated))
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

    BGOItem->zMode = pConfigs->main_bgo[j].view;
    BGOItem->zOffset = pConfigs->main_bgo[j].zOffset;
    BGOItem->setZMode(bgo.z_mode, bgo.z_offset, true);

    if(PasteFromBuffer) BGOItem->setSelected(true);
}

void LvlScene::placeNPC(LevelNPC &npc, bool toGrid)
{

    int j,item_i = 0;
    bool noimage=true, found=false;
    bool isUser=false;
    bool isUserTxt=false;
    obj_npc mergedSet;

    ItemNPC *NPCItem = new ItemNPC;

    long animator=0;

    //Check Index exists
    if(npc.id < (unsigned int)index_npc.size())
    {
        j = index_npc[npc.id].gi;
        item_i = j;
        animator  = index_npc[npc.id].ai;

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
                item_i = j;
                if(!isUser)
                    tImg = pConfigs->main_npc[j].image;
                if(!isUserTxt)
                    mergedSet = pConfigs->main_npc[item_i];
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
            item_i = j;
            mergedSet = pConfigs->main_npc[item_i];
        }
    }


    NPCItem->setScenePoint(this);

        //WriteToLog(QtDebugMsg, "NPC place -> set Data");
    NPCItem->setNpcData(npc);

        //WriteToLog(QtDebugMsg, "NPC place -> set Props");
    NPCItem->localProps = mergedSet;
    NPCItem->setData(8, QString::number((int)mergedSet.no_npc_collions));

        //WriteToLog(QtDebugMsg, "NPC place -> set Pixmap");
    //NPCItem->setMainPixmap(tImg);

        //WriteToLog(QtDebugMsg, "NPC place -> set ContextMenu");
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
//    NPCItem->setAnimation(NPCItem->localProps.frames,
//                          NPCItem->localProps.framespeed,
//                          NPCItem->localProps.framestyle,
//                          npc.direct,
//                          NPCItem->localProps.custom_animate,
//                          NPCItem->localProps.custom_ani_fl,
//                          NPCItem->localProps.custom_ani_el,
//                          NPCItem->localProps.custom_ani_fr,
//                          NPCItem->localProps.custom_ani_er);

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

    NPCItem->setData(7, QString::number((int)NPCItem->localProps.collision_with_blocks) );
    NPCItem->setData(8, QString::number((int)NPCItem->localProps.no_npc_collions) );

    NPCItem->setData(9, QString::number(NPCItem->localProps.width) ); //width
    NPCItem->setData(10, QString::number(NPCItem->localProps.height) ); //height

    if(PasteFromBuffer) NPCItem->setSelected(true);
    #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "NPC place -> done");
    #endif
}

void LvlScene::placeWater(LevelPhysEnv &water, bool toGrid)
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

    WATERItem->setZValue(Z_sys_PhysEnv);

    WATERItem->setData(0, "Water"); // ObjType
    WATERItem->setData(1, QString::number(0) );
    WATERItem->setData(2, QString::number(water.array_id) );
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
            if( (plrt->data(0).toString()=="playerPoint")&&((unsigned int)plrt->data(2).toInt()==plr.id) )
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
