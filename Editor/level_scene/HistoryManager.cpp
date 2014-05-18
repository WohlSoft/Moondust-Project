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
#include "item_npc.h"
#include "../common_features/logger.h"


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

void LvlScene::addChangeSettingsHistory(LevelData modifiedItems, LvlScene::SettingSubType subType, QVariant extraData)
{
    cleanupRedoElements();

    HistoryOperation modOperation;
    modOperation.type = HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS;
    modOperation.data = modifiedItems;
    modOperation.subtype = subType;
    modOperation.extraData = extraData;
    operationList.push_back(modOperation);
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

        foreach (LevelNPC npc, deletedData.npc)
        {
            //place them back
            LvlData->npc.push_back(npc);
            placeNPC(npc);

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
        findGraphicsItem(placeData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRemoveBGO, &LvlScene::historyRemoveNPC);

        break;
    }
    case HistoryOperation::LEVELHISTORY_MOVE:
    {
        //revert move
        LevelData movedSourceData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &LvlScene::historyUndoMoveBlocks, &LvlScene::historyUndoMoveBGO, &LvlScene::historyUndoMoveNPC);

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        if(lastOperation.subtype == SETTING_INVISIBLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsInvisibleBlock, 0, 0, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_SLIPPERY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsSlipperyBlock, 0, 0, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_FRIENDLY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsFriendlyNPC, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_BOSS){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsBossNPC, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_NOMOVEABLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsNoMoveableNPC, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_MESSAGE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsMessageNPC, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_DIRECTION){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsDirectionNPC, true, true);
        }
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
        findGraphicsItem(deletedData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRedoMoveBGO, &LvlScene::historyRemoveNPC);

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
            placeBlock(block);
        }

        foreach (LevelBGO bgo, placedData.bgo)
        {
            //place them back

            //WriteToLog(QtDebugMsg, QString("History-> placed items pos %1 %2").arg(bgo.x).arg(bgo.y));

            LvlData->bgo.push_back(bgo);
            //WriteToLog(QtDebugMsg, QString("History-> added into the array items pos %1 %2").arg(bgo.x).arg(bgo.y));
            placeBGO(bgo);
            //WriteToLog(QtDebugMsg, QString("History-> placed on map pos %1 %2").arg(bgo.x).arg(bgo.y));
        }

        foreach (LevelNPC npc, placedData.npc)
        {
            //place them back
            LvlData->npc.push_back(npc);
            placeNPC(npc);
        }

        //refresh Animation control
        if(opts.animationEnabled) stopAnimation();
        if(opts.animationEnabled) startBlockAnimation();

        break;
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
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &LvlScene::historyRedoMoveBlocks, &LvlScene::historyRedoMoveBGO, &LvlScene::historyRedoMoveNPC);
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        if(lastOperation.subtype == SETTING_INVISIBLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsInvisibleBlock, 0, 0, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_SLIPPERY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsSlipperyBlock, 0, 0, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_FRIENDLY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsFriendlyNPC, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_BOSS){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsBossNPC, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_NOMOVEABLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsNoMoveableNPC, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_MESSAGE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsMessageNPC, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_DIRECTION){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsDirectionNPC, true, true);
        }
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

void LvlScene::historyRedoMoveNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->x+diffX,cbData.hist->y+diffY));
    ((ItemNPC *)(cbData.item))->npcData.x = (long)cbData.item->scenePos().x();
    ((ItemNPC *)(cbData.item))->npcData.y = (long)cbData.item->scenePos().y();
    ((ItemNPC *)(cbData.item))->arrayApply();
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

void LvlScene::historyUndoMoveNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemNPC *)(cbData.item))->npcData.x = (long)cbData.item->scenePos().x();
    ((ItemNPC *)(cbData.item))->npcData.y = (long)cbData.item->scenePos().y();
    ((ItemNPC *)(cbData.item))->arrayApply();
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

void LvlScene::historyRemoveNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC *)cbData.item)->removeFromArray();
    removeItem(cbData.item);
}

void LvlScene::historyUndoSettingsInvisibleBlock(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->setInvisible(!cbData.hist->extraData.toBool());
}

void LvlScene::historyRedoSettingsInvisibleBlock(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->setInvisible(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsSlipperyBlock(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->setSlippery(!cbData.hist->extraData.toBool());
}

void LvlScene::historyRedoSettingsSlipperyBlock(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->setSlippery(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsFriendlyNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setFriendly(!cbData.hist->extraData.toBool());
}

void LvlScene::historyRedoSettingsFriendlyNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setFriendly(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsBossNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setLegacyBoss(!cbData.hist->extraData.toBool());
}

void LvlScene::historyRedoSettingsBossNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setLegacyBoss(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsNoMoveableNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setNoMovable(!cbData.hist->extraData.toBool());
}

void LvlScene::historyRedoSettingsNoMoveableNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setNoMovable(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsMessageNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setMsg(cbData.hist->extraData.toList()[0].toString());
}

void LvlScene::historyRedoSettingsMessageNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setMsg(cbData.hist->extraData.toList()[1].toString());
}

void LvlScene::historyUndoSettingsDirectionNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->changeDirection(cbData.hist->extraData.toList()[0].toInt());
}

void LvlScene::historyRedoSettingsDirectionNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->changeDirection(cbData.hist->extraData.toList()[1].toInt());
}

void LvlScene::findGraphicsItem(LevelData toFind,
                                HistoryOperation * operation,
                                CallbackData customData,
                                callBackLevelBlock clbBlock,
                                callBackLevelBGO clbBgo,
                                callBackLevelNPC clbNpc,
                                bool ignoreBlock,
                                bool ignoreBGO,
                                bool ignoreNPC)
{
    QMap<int, LevelBlock> sortedBlock;
    if(!ignoreBlock){
        foreach (LevelBlock block, toFind.blocks)
        {
            sortedBlock[block.array_id] = block;
        }
    }
    QMap<int, LevelBGO> sortedBGO;
    if(!ignoreBGO){
        foreach (LevelBGO bgo, toFind.bgo)
        {
            sortedBGO[bgo.array_id] = bgo;
        }
    }
    QMap<int, LevelNPC> sortedNPC;
    if(!ignoreNPC){
        foreach (LevelNPC npc, toFind.npc) 
		{
            sortedNPC[npc.array_id] = npc;
        }
    }
    //bool blocksFinished = false;
    //bool bgosFinished = false;
    CallbackData cbData = customData;
    cbData.hist = operation;
    QMap<int, QGraphicsItem*> sortedGraphBlocks;
    QMap<int, QGraphicsItem*> sortedGraphBGO;
    QMap<int, QGraphicsItem*> sortedGraphNPC;
    foreach (QGraphicsItem* unsortedItem, items())
    {
        if(unsortedItem->data(0).toString()=="Block")
        {
            if(!ignoreBlock){
                sortedGraphBlocks[unsortedItem->data(2).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(0).toString()=="BGO")
        {
            if(!ignoreBGO){
                sortedGraphBGO[unsortedItem->data(2).toInt()] = unsortedItem;
            }
        }
        if(unsortedItem->data(0).toString()=="NPC")
        {
            if(!ignoreNPC){
                sortedGraphNPC[unsortedItem->data(2).toInt()] = unsortedItem;
            }
        }
    }

    if(!ignoreBlock){
        foreach (QGraphicsItem* item, sortedGraphBlocks)
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
                break;
            }
        }
    }

    if(!ignoreBGO)
    {
        foreach (QGraphicsItem* item, sortedGraphBGO)
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
                break;
            }
        }
    }

    if(!ignoreNPC)
	{
        foreach (QGraphicsItem* item, sortedGraphNPC) 
		{
            if(sortedNPC.size()!=0)
            {
                QMap<int, LevelNPC>::iterator beginItem = sortedNPC.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedNPC.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedNPC.begin();
				
                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbNpc)(cbData,(*beginItem));
                    sortedNPC.erase(beginItem);
                }
            }
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
