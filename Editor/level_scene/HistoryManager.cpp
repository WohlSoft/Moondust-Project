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
#include "item_block.h"
#include "item_bgo.h"

void LvlScene::addRemoveHistory(LevelData removedItems)
{
    //add cleanup redo elements
    cleanupRedoElements();
    //add new element
    HistoryOperation rmOperation;
    rmOperation.type = HistoryOperation::LEVELHISTORY_REMOVE;
    rmOperation.data = removedItems;
    operationList.push_back(rmOperation);
    historyIndex++;

    historyChanged = true;
}

void LvlScene::addPlaceHistory(LevelData placedItems)
{
    //add cleanup redo elements
    cleanupRedoElements();
    //add new element
    HistoryOperation plOperation;
    plOperation.type = HistoryOperation::LEVELHISTORY_PLACE;
    plOperation.data = placedItems;
    operationList.push_back(plOperation);
    historyIndex++;

    historyChanged = true;
}

void LvlScene::addMoveHistory(LevelData sourceMovedItems, LevelData targetMovedItems)
{
    cleanupRedoElements();

    //calc new Pos:
    long baseX=0, baseY=0;

    //set first base
    QPoint base = calcTopLeftCorner(&targetMovedItems);
    baseX = (long)base.x();
    baseY = (long)base.y();

    HistoryOperation mvOperation;
    mvOperation.type = HistoryOperation::LEVELHISTORY_MOVE;
    mvOperation.data = sourceMovedItems;
    mvOperation.x = baseX;
    mvOperation.y = baseY;
    operationList.push_back(mvOperation);
    historyIndex++;

    historyChanged = true;
}

void LvlScene::historyBack()
{
    historyIndex--;
    HistoryOperation lastOperation = operationList[historyIndex];

    switch( lastOperation.type )
    {
    case HistoryOperation::LEVELHISTORY_REMOVE:
    {
        //revert remove
        LevelData deletedData = lastOperation.data;

        foreach (LevelBlock block, deletedData.blocks)
        {
            //place them back
            LvlData->blocks.push_back(block);
            placeBlock(block);

        }

        foreach (LevelBGO bgo, deletedData.bgo)
        {
            //place them back
            LvlData->bgo.push_back(bgo);
            placeBGO(bgo);

        }

        //refresh Animation control
        if(opts.animationEnabled) stopAnimation();
        if(opts.animationEnabled) startBlockAnimation();

        break;
    }
    case HistoryOperation::LEVELHISTORY_PLACE:
    {
        //revert place
        LevelData placeData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(placeData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRemoveBGO);

        break;
    }
    case HistoryOperation::LEVELHISTORY_MOVE:
    {
        //revert move
        LevelData movedSourceData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &LvlScene::historyUndoMoveBlocks, &LvlScene::historyUndoMoveBGO);

        break;
    }
    default:
        break;
    }

    LvlData->modified = true;

    historyChanged = true;
}

void LvlScene::historyForward()
{

    HistoryOperation lastOperation = operationList[historyIndex];

    switch( lastOperation.type )
    {
    case HistoryOperation::LEVELHISTORY_REMOVE:
    {
        //redo remove
        LevelData deletedData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(deletedData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRedoMoveBGO);

        break;
    }
    case HistoryOperation::LEVELHISTORY_PLACE:
    {
        //redo place
        LevelData placedData = lastOperation.data;

        foreach (LevelBlock block, placedData.blocks)
        {
            //place them back
            LvlData->blocks.push_back(block);
            placeBlock(block, true);
        }

        foreach (LevelBGO bgo, placedData.bgo)
        {
            //place them back
            LvlData->bgo.push_back(bgo);
            placeBGO(bgo, true);
        }

        //refresh Animation control
        if(opts.animationEnabled) stopAnimation();
        if(opts.animationEnabled) startBlockAnimation();

    }
    case HistoryOperation::LEVELHISTORY_MOVE:
    {

        //revert move
        LevelData movedSourceData = lastOperation.data;

        //calc new Pos:
        long baseX=0, baseY=0;

        //set first base
        QPoint base = calcTopLeftCorner(&movedSourceData);
        baseX = (long)base.x();
        baseY = (long)base.y();

        CallbackData cbData;
        cbData.x = baseX;
        cbData.y = baseY;
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &LvlScene::historyRedoMoveBlocks, &LvlScene::historyRedoMoveBGO);
        break;
    }
    default:
        break;
    }

    historyIndex++;

    historyChanged = true;
}

int LvlScene::getHistroyIndex()
{
    return historyIndex;
}

void LvlScene::cleanupRedoElements()
{
    if(canRedo())
    {
        int lastSize = operationList.size();
        for(int i = historyIndex; i < lastSize; i++)
        {
            operationList.pop_back();
        }
    }
}

bool LvlScene::canUndo()
{
    return historyIndex > 0;
}

bool LvlScene::canRedo()
{
    return historyIndex < operationList.size();
}

void LvlScene::historyRedoMoveBlocks(CallbackData cbData, LevelBlock data)
{

    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->x+diffX,cbData.hist->y+diffY));
    ((ItemBlock *)(cbData.item))->blockData.x = (long)cbData.item->scenePos().x();
    ((ItemBlock *)(cbData.item))->blockData.y = (long)cbData.item->scenePos().y();
    ((ItemBlock *)(cbData.item))->arrayApply();
}

void LvlScene::historyRedoMoveBGO(CallbackData cbData, LevelBGO data)
{
    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->x+diffX,cbData.hist->y+diffY));
    ((ItemBGO *)(cbData.item))->bgoData.x = (long)cbData.item->scenePos().x();
    ((ItemBGO *)(cbData.item))->bgoData.y = (long)cbData.item->scenePos().y();
    ((ItemBGO *)(cbData.item))->arrayApply();
}

void LvlScene::historyUndoMoveBlocks(LvlScene::CallbackData cbData, LevelBlock data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemBlock *)(cbData.item))->blockData.x = (long)cbData.item->scenePos().x();
    ((ItemBlock *)(cbData.item))->blockData.y = (long)cbData.item->scenePos().y();
    ((ItemBlock *)(cbData.item))->arrayApply();
}

void LvlScene::historyUndoMoveBGO(LvlScene::CallbackData cbData, LevelBGO data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemBGO *)(cbData.item))->bgoData.x = (long)cbData.item->scenePos().x();
    ((ItemBGO *)(cbData.item))->bgoData.y = (long)cbData.item->scenePos().y();
    ((ItemBGO *)(cbData.item))->arrayApply();
}

void LvlScene::historyRemoveBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->removeFromArray();
    removeItem(cbData.item);
}

void LvlScene::historyRemoveBGO(LvlScene::CallbackData cbData, LevelBGO /*data*/)
{
    ((ItemBGO *)cbData.item)->removeFromArray();
    removeItem(cbData.item);
}

void LvlScene::findGraphicsItem(LevelData toFind,
                                HistoryOperation * operation,
                                CallbackData customData,
                                callBackLevelBlock clbBlock,
                                callBackLevelBGO clbBgo)
{
    QMap<int, LevelBlock> sortedBlock;
    foreach (LevelBlock block, toFind.blocks)
    {
        sortedBlock[block.array_id] = block;
    }

    QMap<int, LevelBGO> sortedBGO;
    foreach (LevelBGO bgo, toFind.bgo)
    {
        sortedBGO[bgo.array_id] = bgo;
    }
    bool blocksFinished = false;
    bool bgosFinished = false;
    CallbackData cbData = customData;
    cbData.hist = operation;
    foreach (QGraphicsItem* item, items()){
        if(item->data(0).toString()=="Block")
        {
            if(sortedBlock.size()!=0)
            {
                QMap<int, LevelBlock>::iterator beginItem = sortedBlock.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedBlock.erase(beginItem);
                }
                //but still test if the next blocks, is the block we search!
                beginItem = sortedBlock.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbBlock)(cbData,(*beginItem));
                    sortedBlock.erase(beginItem);
                }
            }
            else
            {
                blocksFinished = true;
            }
        }
        else
        if(item->data(0).toString()=="BGO")
        {
            if(sortedBGO.size()!=0)
            {
                QMap<int, LevelBGO>::iterator beginItem = sortedBGO.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedBGO.erase(beginItem);
                }
                //but still test if the next blocks, is the block we search!
                beginItem = sortedBGO.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbBgo)(cbData,(*beginItem));
                    sortedBGO.erase(beginItem);
                }
            }
            else
            {
                bgosFinished = true;
            }
        }
        if(blocksFinished&&bgosFinished)
        {
            break;
        }
    }


}

QPoint LvlScene::calcTopLeftCorner(LevelData *data)
{
    //calc new Pos:
    int baseX=0, baseY=0;

    //set first base
    if(!data->blocks.isEmpty()){
        baseX = (int)data->blocks[0].x;
        baseY = (int)data->blocks[0].y;
    }else if(!data->bgo.isEmpty()){
        baseX = (int)data->bgo[0].x;
        baseY = (int)data->bgo[0].y;
    }else if(!data->npc.isEmpty()){
        baseX = (int)data->npc[0].x;
        baseY = (int)data->npc[0].y;
    }

    foreach (LevelBlock block, data->blocks) {
        if((int)block.x<baseX){
            baseX = (int)block.x;
        }
        if((int)block.y<baseY){
            baseY = (int)block.y;
        }
    }
    foreach (LevelBGO bgo, data->bgo){
        if((int)bgo.x<baseX){
            baseX = (int)bgo.x;
        }
        if((int)bgo.y<baseY){
            baseY = (int)bgo.y;
        }
    }
    foreach (LevelNPC npc, data->npc){
        if((int)npc.x<baseX){
            baseX = (int)npc.x;
        }
        if((int)npc.y<baseY){
            baseY = (int)npc.y;
        }
    }

    return QPoint(baseX, baseY);
}
