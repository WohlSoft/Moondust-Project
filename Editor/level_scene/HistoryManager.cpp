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
#include "item_water.h"
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

void LvlScene::addResizeSectionHistory(int sectionID, long oldLeft, long oldTop, long oldRight, long oldBottom,
                                       long newLeft, long newTop, long newRight, long newBottom)
{
    cleanupRedoElements();
    HistoryOperation resizeOperation;
    resizeOperation.type = HistoryOperation::LEVELHISTORY_RESIZESECTION;
    QList<QVariant> oldSizes;
    QList<QVariant> newSizes;
    QList<QVariant> package;
    oldSizes.push_back(QVariant((qlonglong)oldLeft));
    oldSizes.push_back(QVariant((qlonglong)oldTop));
    oldSizes.push_back(QVariant((qlonglong)oldRight));
    oldSizes.push_back(QVariant((qlonglong)oldBottom));
    newSizes.push_back(QVariant((qlonglong)newLeft));
    newSizes.push_back(QVariant((qlonglong)newTop));
    newSizes.push_back(QVariant((qlonglong)newRight));
    newSizes.push_back(QVariant((qlonglong)newBottom));
    package.push_back(sectionID);
    package.push_back(oldSizes);
    package.push_back(newSizes);
    resizeOperation.extraData = QVariant(package);
    operationList.push_back(resizeOperation);
    historyIndex++;

    historyChanged = true;
}

void LvlScene::addChangedLayerHistory(LevelData changedItems, QString newLayerName)
{
    cleanupRedoElements();

    HistoryOperation chLaOperation;
    chLaOperation.type = HistoryOperation::LEVELHISTORY_CHANGEDLAYER;
    chLaOperation.extraData = QVariant(newLayerName);
    chLaOperation.data = changedItems;
    operationList.push_back(chLaOperation);
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

        foreach (LevelWater water, deletedData.water)
        {
            //place them back
            LvlData->water.push_back(water);
            placeWater(water);
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
        findGraphicsItem(placeData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRemoveBGO, &LvlScene::historyRemoveNPC, &LvlScene::historyRemoveWater);

        break;
    }
    case HistoryOperation::LEVELHISTORY_MOVE:
    {
        //revert move
        LevelData movedSourceData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &LvlScene::historyUndoMoveBlocks, &LvlScene::historyUndoMoveBGO, &LvlScene::historyUndoMoveNPC, &LvlScene::historyUndoMoveWater);

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        if(lastOperation.subtype == SETTING_INVISIBLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsInvisibleBlock, 0, 0, 0, false, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_SLIPPERY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsSlipperyBlock, 0, 0, 0, false, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_FRIENDLY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsFriendlyNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_BOSS){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsBossNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_NOMOVEABLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsNoMoveableNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_MESSAGE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsMessageNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_DIRECTION){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsDirectionNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_CHANGENPC){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsChangeNPCBlocks, 0, 0, 0, false, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_WATERTYPE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &LvlScene::historyUndoSettingsTypeWater, true, true, true);
        }
        break;
    }
    case HistoryOperation::LEVELHISTORY_RESIZESECTION:
    {
        QList<QVariant> package = lastOperation.extraData.toList();
        int sectionID = package[0].toInt();
        QList<QVariant> oldSizes = package[1].toList();
        long tarLeft = (long)oldSizes[0].toLongLong();
        long tarTop = (long)oldSizes[1].toLongLong();
        long tarRight = (long)oldSizes[2].toLongLong();
        long tarBottom = (long)oldSizes[3].toLongLong();

        LvlData->sections[sectionID].size_left = tarLeft;
        LvlData->sections[sectionID].size_right = tarRight;
        LvlData->sections[sectionID].size_top = tarTop;
        LvlData->sections[sectionID].size_bottom = tarBottom;

        ChangeSectionBG(LvlData->sections[sectionID].background, sectionID);
        if(sectionID == LvlData->CurSection){
            drawSpace();
        }

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDLAYER:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoChangeLayerBlocks, &LvlScene::historyUndoChangeLayerBGO, &LvlScene::historyUndoChangeLayerNPC, &LvlScene::historyUndoChangeLayerWater);
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
        findGraphicsItem(deletedData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRedoMoveBGO, &LvlScene::historyRemoveNPC, &LvlScene::historyRemoveWater);

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

        foreach (LevelWater water, placedData.water)
        {
            //place them back
            LvlData->water.push_back(water);
            placeWater(water);
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
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &LvlScene::historyRedoMoveBlocks, &LvlScene::historyRedoMoveBGO, &LvlScene::historyRedoMoveNPC, &LvlScene::historyRedoMoveWater);
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        if(lastOperation.subtype == SETTING_INVISIBLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsInvisibleBlock, 0, 0, 0, false, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_SLIPPERY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsSlipperyBlock, 0, 0, 0, false, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_FRIENDLY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsFriendlyNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_BOSS){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsBossNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_NOMOVEABLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsNoMoveableNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_MESSAGE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsMessageNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_DIRECTION){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsDirectionNPC, 0, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_CHANGENPC){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsChangeNPCBlocks, 0, 0, 0, false, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_WATERTYPE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &LvlScene::historyRedoSettingsTypeWater, true, true, true);
        }
        break;
    }
    case HistoryOperation::LEVELHISTORY_RESIZESECTION:
    {
        QList<QVariant> package = lastOperation.extraData.toList();
        int sectionID = package[0].toInt();
        QList<QVariant> newSizes = package[2].toList();
        long tarLeft = (long)newSizes[0].toLongLong();
        long tarTop = (long)newSizes[1].toLongLong();
        long tarRight = (long)newSizes[2].toLongLong();
        long tarBottom = (long)newSizes[3].toLongLong();

        LvlData->sections[sectionID].size_left = tarLeft;
        LvlData->sections[sectionID].size_right = tarRight;
        LvlData->sections[sectionID].size_top = tarTop;
        LvlData->sections[sectionID].size_bottom = tarBottom;

        ChangeSectionBG(LvlData->sections[sectionID].background, sectionID);
        if(sectionID == LvlData->CurSection){
            drawSpace();
        }
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDLAYER:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoChangeLayerBlocks, &LvlScene::historyRedoChangeLayerBGO, &LvlScene::historyRedoChangeLayerNPC, &LvlScene::historyRedoChangeLayerWater);
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

void LvlScene::historyRedoMoveWater(LvlScene::CallbackData cbData, LevelWater data)
{
    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->x+diffX,cbData.hist->y+diffY));
    ((ItemWater *)(cbData.item))->waterData.x = (long)cbData.item->scenePos().x();
    ((ItemWater *)(cbData.item))->waterData.y = (long)cbData.item->scenePos().y();
    ((ItemWater *)(cbData.item))->arrayApply();
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

void LvlScene::historyUndoMoveWater(LvlScene::CallbackData cbData, LevelWater data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemWater *)(cbData.item))->waterData.x = (long)cbData.item->scenePos().x();
    ((ItemWater *)(cbData.item))->waterData.y = (long)cbData.item->scenePos().y();
    ((ItemWater *)(cbData.item))->arrayApply();
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

void LvlScene::historyRemoveWater(LvlScene::CallbackData cbData, LevelWater /*data*/)
{
    ((ItemWater *)cbData.item)->removeFromArray();
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

void LvlScene::historyUndoSettingsChangeNPCBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ItemBlock* targetItem = (ItemBlock*)cbData.item;
    int targetNPC_id = cbData.hist->extraData.toList()[0].toInt();
    targetItem->blockData.npc_id = (unsigned long)targetNPC_id;
    targetItem->arrayApply();
    targetItem->setIncludedNPC((unsigned long)targetNPC_id);
}

void LvlScene::historyRedoSettingsChangeNPCBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ItemBlock* targetItem = (ItemBlock*)cbData.item;
    int targetNPC_id = cbData.hist->extraData.toList()[1].toInt();
    targetItem->blockData.npc_id = (unsigned long)targetNPC_id;
    targetItem->arrayApply();
    targetItem->setIncludedNPC((unsigned long)targetNPC_id);
}

void LvlScene::historyUndoSettingsTypeWater(LvlScene::CallbackData cbData, LevelWater /*data*/)
{
    ((ItemWater*)cbData.item)->setType(cbData.hist->extraData.toBool() ? 1 : 0);
}

void LvlScene::historyRedoSettingsTypeWater(LvlScene::CallbackData cbData, LevelWater /*data*/)
{
    ((ItemWater*)cbData.item)->setType(cbData.hist->extraData.toBool() ? 0 : 1);
}

void LvlScene::historyUndoChangeLayerBlocks(LvlScene::CallbackData cbData, LevelBlock data)
{
    ItemBlock* targetItem = (ItemBlock*)cbData.item;
    QString oldLayer = data.layer;
    targetItem->blockData.layer = oldLayer;
    foreach(LevelLayers lr, LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::historyUndoChangeLayerBGO(LvlScene::CallbackData cbData, LevelBGO data)
{
    ItemBGO* targetItem = (ItemBGO*)cbData.item;
    QString oldLayer = data.layer;
    targetItem->bgoData.layer = oldLayer;
    foreach(LevelLayers lr, LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::historyUndoChangeLayerNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ItemNPC* targetItem = (ItemNPC*)cbData.item;
    QString oldLayer = data.layer;
    targetItem->npcData.layer = oldLayer;
    foreach(LevelLayers lr, LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::historyUndoChangeLayerWater(LvlScene::CallbackData cbData, LevelWater data)
{
    ItemWater* targetItem = (ItemWater*)cbData.item;
    QString oldLayer = data.layer;
    targetItem->waterData.layer = oldLayer;
    foreach(LevelLayers lr, LvlData->layers)
    {
        if(lr.name == oldLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::historyRedoChangeLayerBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ItemBlock* targetItem = (ItemBlock*)cbData.item;
    QString newLayer = cbData.hist->extraData.toString();
    targetItem->blockData.layer = newLayer;
    foreach(LevelLayers lr, LvlData->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::historyRedoChangeLayerBGO(LvlScene::CallbackData cbData, LevelBGO /*data*/)
{
    ItemBGO* targetItem = (ItemBGO*)cbData.item;
    QString newLayer = cbData.hist->extraData.toString();
    targetItem->bgoData.layer = newLayer;
    foreach(LevelLayers lr, LvlData->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::historyRedoChangeLayerNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ItemNPC* targetItem = (ItemNPC*)cbData.item;
    QString newLayer = cbData.hist->extraData.toString();
    targetItem->npcData.layer = newLayer;
    foreach(LevelLayers lr, LvlData->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::historyRedoChangeLayerWater(LvlScene::CallbackData cbData, LevelWater /*data*/)
{
    ItemWater* targetItem = (ItemWater*)cbData.item;
    QString newLayer = cbData.hist->extraData.toString();
    targetItem->waterData.layer = newLayer;
    foreach(LevelLayers lr, LvlData->layers)
    {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::findGraphicsItem(LevelData toFind,
                                HistoryOperation * operation,
                                CallbackData customData,
                                callBackLevelBlock clbBlock,
                                callBackLevelBGO clbBgo,
                                callBackLevelNPC clbNpc,
                                callBackLevelWater clbWater,
                                bool ignoreBlock,
                                bool ignoreBGO,
                                bool ignoreNPC,
                                bool ignoreWater)
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
    QMap<int, LevelWater> sortedWater;
    if(!ignoreWater)
    {
        foreach (LevelWater water, toFind.water) {
            sortedWater[water.array_id] = water;
        }
    }

    //bool blocksFinished = false;
    //bool bgosFinished = false;
    CallbackData cbData = customData;
    cbData.hist = operation;
    QMap<int, QGraphicsItem*> sortedGraphBlocks;
    QMap<int, QGraphicsItem*> sortedGraphBGO;
    QMap<int, QGraphicsItem*> sortedGraphNPC;
    QMap<int, QGraphicsItem*> sortedGraphWater;
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
        else
        if(unsortedItem->data(0).toString()=="NPC")
        {
            if(!ignoreNPC){
                sortedGraphNPC[unsortedItem->data(2).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(0).toString()=="Water")
        {
            if(!ignoreWater){
                sortedGraphWater[unsortedItem->data(2).toInt()] = unsortedItem;
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

    if(!ignoreWater)
    {
        foreach (QGraphicsItem* item, sortedGraphWater)
        {
            if(sortedWater.size()!=0)
            {
                QMap<int, LevelWater>::iterator beginItem = sortedWater.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedWater.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedWater.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbWater)(cbData,(*beginItem));
                    sortedWater.erase(beginItem);
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
