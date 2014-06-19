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
#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../common_features/logger.h"
#include "../common_features/mainwinconnect.h"
#include "../file_formats/file_formats.h"


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

    MainWinConnect::pMainWin->refreshHistoryButtons();
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

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addPlaceDoorHistory(int array_id, bool isEntrance, long x, long y)
{
    cleanupRedoElements();

    HistoryOperation plDoorOperation;
    plDoorOperation.type = HistoryOperation::LEVELHISTORY_PLACEDOOR;
    QList<QVariant> doorExtraData;
    doorExtraData.push_back(array_id);
    doorExtraData.push_back(isEntrance);
    doorExtraData.push_back((qlonglong)x);
    doorExtraData.push_back((qlonglong)y);
    plDoorOperation.extraData = QVariant(doorExtraData);

    operationList.push_back(plDoorOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
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

    MainWinConnect::pMainWin->refreshHistoryButtons();
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

    MainWinConnect::pMainWin->refreshHistoryButtons();
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

    MainWinConnect::pMainWin->refreshHistoryButtons();
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

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addResizeBlockHistory(LevelBlock bl, long oldLeft, long oldTop, long oldRight, long oldBottom, long newLeft, long newTop, long newRight, long newBottom)
{
    cleanupRedoElements();

    HistoryOperation resizeBlOperation;
    resizeBlOperation.type = HistoryOperation::LEVELHISTORY_RESIZEBLOCK;
    LevelData blData;
    blData.blocks.push_back(bl);
    resizeBlOperation.data = blData;
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
    package.push_back(oldSizes);
    package.push_back(newSizes);
    resizeBlOperation.extraData = QVariant(package);
    operationList.push_back(resizeBlOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addAddWarpHistory(int array_id, int listindex, int doorindex)
{
    cleanupRedoElements();

    HistoryOperation addWpOperation;
    addWpOperation.type = HistoryOperation::LEVELHISTORY_ADDWARP;
    QList<QVariant> package;
    package.push_back(array_id);
    package.push_back(listindex);
    package.push_back(doorindex);
    addWpOperation.extraData = QVariant(package);
    operationList.push_back(addWpOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRemoveWarpHistory(LevelDoors removedDoor)
{
    cleanupRedoElements();

    HistoryOperation rmWpOperation;
    rmWpOperation.type = HistoryOperation::LEVELHISTORY_REMOVEWARP;
    LevelData data;
    data.doors.push_back(removedDoor);
    rmWpOperation.data = data;
    operationList.push_back(rmWpOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeWarpSettingsHistory(int array_id, LvlScene::SettingSubType subtype, QVariant extraData)
{
    cleanupRedoElements();

    HistoryOperation chWpSettingsOperation;
    chWpSettingsOperation.type = HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSWARP;
    chWpSettingsOperation.subtype = subtype;
    QList<QVariant> package;
    package.push_back(array_id);
    package.push_back(extraData);
    chWpSettingsOperation.extraData = QVariant(package);
    operationList.push_back(chWpSettingsOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addAddEventHistory(int array_id, int listindex, QString name)
{
    cleanupRedoElements();

    HistoryOperation addEvOperation;
    addEvOperation.type = HistoryOperation::LEVELHISTORY_ADDEVENT;
    QList<QVariant> package;
    package.push_back(array_id);
    package.push_back(listindex);
    package.push_back(name);
    addEvOperation.extraData = QVariant(package);
    operationList.push_back(addEvOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
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
        bool hasToUpdateDoorData = false;

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

        foreach (LevelDoors door, deletedData.doors)
        {
            LevelDoors originalDoor;
            bool found = false;
            foreach(LevelDoors findDoor, LvlData->doors){
                if(door.array_id == findDoor.array_id){
                    originalDoor = findDoor;
                    found = true;
                    break;
                }
            }
            if(!found)
                break;

            if(door.isSetIn&&!door.isSetOut)
            {
                originalDoor.ix = door.ix;
                originalDoor.iy = door.iy;
                originalDoor.isSetIn = true;
                placeDoorEnter(originalDoor, false, false);
            }
            else
            if(!door.isSetIn&&door.isSetOut)
            {
                originalDoor.ox = door.ox;
                originalDoor.oy = door.oy;
                originalDoor.isSetOut = true;
                placeDoorExit(originalDoor, false, false);
            }
            hasToUpdateDoorData = true;
        }
        if(hasToUpdateDoorData)
            MainWinConnect::pMainWin->setDoorData(-2);


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
        findGraphicsItem(placeData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRemoveBGO, &LvlScene::historyRemoveNPC, &LvlScene::historyRemoveWater, 0, false, false, false, false, true);

        break;
    }
    case HistoryOperation::LEVELHISTORY_MOVE:
    {
        //revert move
        LevelData movedSourceData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &LvlScene::historyUndoMoveBlocks, &LvlScene::historyUndoMoveBGO, &LvlScene::historyUndoMoveNPC, &LvlScene::historyUndoMoveWater, &LvlScene::historyUndoMoveDoors);

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        if(lastOperation.subtype == SETTING_INVISIBLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsInvisibleBlock, 0, 0, 0, 0, false, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_SLIPPERY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsSlipperyBlock, 0, 0, 0, 0, false, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_FRIENDLY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsFriendlyNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_BOSS){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsBossNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_NOMOVEABLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsNoMoveableNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_MESSAGE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsMessageNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_DIRECTION){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsDirectionNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_CHANGENPC){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsChangeNPCBlocks, 0, &LvlScene::historyUndoSettingsChangeNPCNPC, 0, 0, false, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_WATERTYPE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &LvlScene::historyUndoSettingsTypeWater, 0, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_NOYOSHI){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, 0, &LvlScene::historyUndoSettingsNoYoshiDoors, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_ALLOWNPC){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, 0, &LvlScene::historyUndoSettingsAllowNPCDoors, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_LOCKED){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, 0, &LvlScene::historyUndoSettingsLockedDoors, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_GENACTIVATE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsActivateGeneratorNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_GENTYPE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsTypeGeneratorNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_GENDIR){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsDirectionGeneratorNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_GENTIME){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsTimeGeneratorNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_ATTACHLAYER){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsAttachLayerNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_DESTROYED){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsDestroyedEventBlocks, 0, 0, 0, 0, false, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_HITED){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsHitedEventBlocks, 0, 0, 0, 0, false, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_LAYER_EMP){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoSettingsLayerEmptyEventBlocks, 0, &LvlScene::historyUndoSettingsLayerEmptyEventNPC, 0, 0, false, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_ACTIVATE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsActivateEventNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_DEATH){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsDeathEventNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_TALK){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsTalkEventNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_SPECIAL_DATA){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyUndoSettingsSpecialDataNPC, 0, 0, true, true, false, true, true);
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
        findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoChangeLayerBlocks, &LvlScene::historyUndoChangeLayerBGO, &LvlScene::historyUndoChangeLayerNPC, &LvlScene::historyUndoChangeLayerWater, 0, false, false, false, false, true);
        break;
    }
    case HistoryOperation::LEVELHISTORY_RESIZEBLOCK:
    {
        LevelData resizedBlock = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(resizedBlock, &lastOperation, cbData, &LvlScene::historyUndoResizeBlock, 0, 0, 0, 0, false, true, true, true, true);
        break;
    }
    case HistoryOperation::LEVELHISTORY_PLACEDOOR:
    {
        CallbackData cbData;
        findGraphicsDoor(lastOperation.extraData.toList()[0].toInt(), &lastOperation, cbData, &LvlScene::historyRemoveDoors, lastOperation.extraData.toList()[1].toBool());

        break;
    }
    case HistoryOperation::LEVELHISTORY_ADDWARP:
    {
        WriteToLog(QtDebugMsg, "HistoryManager -> undo Door entry add");

        int arrayidDoor = lastOperation.extraData.toList()[0].toInt();
        int listindex = lastOperation.extraData.toList()[1].toInt();
        doorPointsSync((unsigned int)arrayidDoor,true);

        for(int i = 0; i < LvlData->doors.size(); i++)
        {
            if(LvlData->doors[i].array_id==(unsigned int)arrayidDoor)
            {
                LvlData->doors.remove(i);
                break;
            }
        }

        bool found = false;

        QComboBox* warplist = MainWinConnect::pMainWin->getWarpList();

        if((warplist->currentIndex()==listindex)&&(warplist->count()>2))
        {
            warplist->setCurrentIndex(warplist->currentIndex()-1);
        }


        WriteToLog(QtDebugMsg, "HistoryManager -> check index");

        if(listindex < warplist->count()){
            if(arrayidDoor == warplist->itemData(listindex).toInt()){
                found = true;
                warplist->removeItem(listindex);
            }
        }
        WriteToLog(QtDebugMsg, QString("HistoryManager -> found = %1").arg(found));


        if(!found)
        {
            found=false;
            for(int i = 0; i < warplist->count(); i++)
            {
                if(arrayidDoor == warplist->itemData(i).toInt())
                {
                    warplist->removeItem(i);
                    found=true;
                    break;
                }
            }
        }
        WriteToLog(QtDebugMsg, QString("HistoryManager -> found and removed = %1").arg(found));


        if(warplist->count()<=0) MainWinConnect::pMainWin->setWarpRemoveButtonEnabled(false);

        MainWinConnect::pMainWin->setDoorData(-2);

        //warplist->update();
        //warplist->repaint();
        break;
    }
    case HistoryOperation::LEVELHISTORY_REMOVEWARP:
    {
        LevelDoors removedDoor = lastOperation.data.doors[0];
        LvlData->doors.insert(removedDoor.index, removedDoor);

        QComboBox* warplist = MainWinConnect::pMainWin->getWarpList();
        warplist->insertItem(removedDoor.index, QString("%1: x%2y%3 <=> x%4y%5")
                             .arg(removedDoor.array_id).arg(removedDoor.ix).arg(removedDoor.iy).arg(removedDoor.ox).arg(removedDoor.oy),
                             removedDoor.array_id);
        if(warplist->count() > (int)removedDoor.index)
        {
            warplist->setCurrentIndex( removedDoor.index );
        }
        else
        {
            warplist->setCurrentIndex( warplist->count()-1 );
        }

        if(removedDoor.isSetOut){
            placeDoorExit(removedDoor);
        }
        if(removedDoor.isSetIn){
            placeDoorEnter(removedDoor);
        }


        MainWinConnect::pMainWin->setDoorData(-2);
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSWARP:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        int array_id = lastOperation.extraData.toList()[0].toInt();
        int index = -1;
        QVariant extraData = lastOperation.extraData.toList()[1];
        LevelDoors * doorp;
        bool found = false;

        for(int i = 0; i < LvlData->doors.size(); i++){
            if(LvlData->doors[i].array_id == (unsigned int)array_id){
                found = true;
                doorp = LvlData->doors.data();
                index = i;
                break;
            }
        }

        if(!found)
            break;


        if(subtype == SETTING_NOYOSHI){
            doorp[index].noyoshi = !extraData.toBool();
        }
        else
        if(subtype == SETTING_ALLOWNPC){
            doorp[index].allownpc = !extraData.toBool();
        }
        else
        if(subtype == SETTING_LOCKED){
            doorp[index].locked = !extraData.toBool();
        }
        else
        if(subtype == SETTING_WARPTYPE){
            doorp[index].type = extraData.toList()[0].toInt();
        }
        else
        if(subtype == SETTING_NEEDASTAR){
            doorp[index].stars = extraData.toList()[0].toInt();
        }
        else
        if(subtype == SETTING_ENTRDIR){
            doorp[index].idirect = extraData.toList()[0].toInt();
        }
        else
        if(subtype == SETTING_EXITDIR){
            doorp[index].odirect = extraData.toList()[0].toInt();
        }
        else
        if(subtype == SETTING_LEVELEXIT){
            doorp[index].lvl_o = !extraData.toList()[0].toBool();
            if(!doorp[index].lvl_o && !doorp[index].isSetOut && extraData.toList().size() >= 3){
                doorp[index].ox = extraData.toList()[1].toInt();
                doorp[index].oy = extraData.toList()[2].toInt();
                doorp[index].isSetOut = true;
                placeDoorExit(doorp[index]);
            }
        }
        else
        if(subtype == SETTING_LEVELENTR){
            doorp[index].lvl_i = !extraData.toList()[0].toBool();
            if(!doorp[index].lvl_i && !doorp[index].isSetIn && extraData.toList().size() >= 3){
                doorp[index].ix = extraData.toList()[1].toInt();
                doorp[index].iy = extraData.toList()[2].toInt();
                doorp[index].isSetIn = true;
                placeDoorEnter(doorp[index]);
            }
        }
        else
        if(subtype == SETTING_LEVELWARPTO){
            doorp[index].warpto = extraData.toList()[0].toInt();
        }

        MainWinConnect::pMainWin->setDoorData(-2);
        doorPointsSync(array_id);

        break;            
    }
    case HistoryOperation::LEVELHISTORY_ADDEVENT:
    {
        int array_id = lastOperation.extraData.toList()[0].toInt();
        MainWinConnect::pMainWin->setEventToolsLocked(true);

        for (int i = 0; i < LvlData->events.size(); i++) {
            if(LvlData->events[i].array_id == (unsigned int)array_id){
                for(int j = 0; j < MainWinConnect::pMainWin->getEventList()->count(); j++){
                    if(MainWinConnect::pMainWin->getEventList()->item(j)->data(3).toInt() == array_id){
                        delete MainWinConnect::pMainWin->getEventList()->item(j);
                    }
                }
                MainWinConnect::pMainWin->ModifyEvent(LvlData->events[i].name, "");
                LvlData->events.remove(i);
            }
        }

        MainWinConnect::pMainWin->EventListsSync();
        MainWinConnect::pMainWin->setEventToolsLocked(false);
        break;
    }
    default:
        break;
    }
    LvlData->modified = true;

    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Undone: %1").arg(getHistoryText(lastOperation)));
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
        findGraphicsItem(deletedData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRedoMoveBGO, &LvlScene::historyRemoveNPC, &LvlScene::historyRemoveWater, &LvlScene::historyRemoveDoors);

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
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &LvlScene::historyRedoMoveBlocks, &LvlScene::historyRedoMoveBGO, &LvlScene::historyRedoMoveNPC, &LvlScene::historyRedoMoveWater, &LvlScene::historyRedoMoveDoors);
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        if(lastOperation.subtype == SETTING_INVISIBLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsInvisibleBlock, 0, 0, 0, 0, false, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_SLIPPERY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsSlipperyBlock, 0, 0, 0, 0, false, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_FRIENDLY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsFriendlyNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_BOSS){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsBossNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_NOMOVEABLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsNoMoveableNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_MESSAGE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsMessageNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_DIRECTION){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsDirectionNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_CHANGENPC){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsChangeNPCBlocks, 0, &LvlScene::historyRedoSettingsChangeNPCNPC, 0, 0, false, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_WATERTYPE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &LvlScene::historyRedoSettingsTypeWater, 0, true, true, true, false, true);
        }
        else
        if(lastOperation.subtype == SETTING_NOYOSHI){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, 0, &LvlScene::historyRedoSettingsNoYoshiDoors, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_ALLOWNPC){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, 0, &LvlScene::historyRedoSettingsAllowNPCDoors, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_LOCKED){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, 0, &LvlScene::historyRedoSettingsLockedDoors, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_GENACTIVATE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsActivateGeneratorNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_GENTYPE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsTypeGeneratorNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_GENDIR){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsDirectionGeneratorNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_GENTIME){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsTimeGeneratorNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_ATTACHLAYER){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsAttachLayerNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_DESTROYED){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsDestroyedEventBlocks, 0, 0, 0, 0, false, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_HITED){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsHitedEventBlocks, 0, 0, 0, 0, false, true, true, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_LAYER_EMP){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoSettingsLayerEmptyEventBlocks, 0, &LvlScene::historyRedoSettingsLayerEmptyEventNPC, 0, 0, false, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_ACTIVATE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsActivateEventNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_DEATH){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsDeathEventNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_EV_TALK){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsTalkEventNPC, 0, 0, true, true, false, true, true);
        }
        else
        if(lastOperation.subtype == SETTING_SPECIAL_DATA){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, &LvlScene::historyRedoSettingsSpecialDataNPC, 0, 0, true, true, false, true, true);
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
        findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoChangeLayerBlocks, &LvlScene::historyRedoChangeLayerBGO, &LvlScene::historyRedoChangeLayerNPC, &LvlScene::historyRedoChangeLayerWater, 0, false, false, false, false, true);
        break;
    }
    case HistoryOperation::LEVELHISTORY_RESIZEBLOCK:
    {
        LevelData resizedBlock = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(resizedBlock, &lastOperation, cbData, &LvlScene::historyRedoResizeBlock, 0, 0, 0, 0, false, true, true, true, true);
        break;
    }
    case HistoryOperation::LEVELHISTORY_PLACEDOOR:
    {
        bool found = false;
        int array_id = lastOperation.extraData.toList()[0].toInt();
        LevelDoors door;

        foreach(LevelDoors findDoor, LvlData->doors){
            if(array_id == (int)findDoor.array_id){
                door = findDoor;
                found = true;
                break;
            }
        }

        if(!found)
            return;

        bool isEntrance = lastOperation.extraData.toList()[1].toBool();

        if(isEntrance){
            door.ix = (long)lastOperation.extraData.toList()[2].toLongLong();
            door.iy = (long)lastOperation.extraData.toList()[3].toLongLong();
            door.isSetIn = true;
            placeDoorEnter(door, false, false);
        }else{
            door.ox = (long)lastOperation.extraData.toList()[2].toLongLong();
            door.oy = (long)lastOperation.extraData.toList()[3].toLongLong();
            door.isSetOut = true;
            placeDoorExit(door, false, false);
        }

        MainWinConnect::pMainWin->setDoorData(-2);

        break;
    }
    case HistoryOperation::LEVELHISTORY_ADDWARP:
    {
        int arrayid = lastOperation.extraData.toList()[0].toInt();
        int listindex = lastOperation.extraData.toList()[1].toInt();
        int doorindex = lastOperation.extraData.toList()[2].toInt();

        LevelDoors newDoor = FileFormats::dummyLvlDoor();
        newDoor.array_id = arrayid;
        newDoor.index = doorindex;

        LvlData->doors.insert(doorindex, newDoor);
        QComboBox* warplist = MainWinConnect::pMainWin->getWarpList();
        warplist->addItem(QString("%1: x%2y%3 <=> x%4y%5")
                          .arg(newDoor.array_id).arg(newDoor.ix).arg(newDoor.iy).arg(newDoor.ox).arg(newDoor.oy),
                          newDoor.array_id);
        if(warplist->count() < listindex)
        {
            warplist->setCurrentIndex( listindex );
        }
        else
        {
            warplist->setCurrentIndex( warplist->count()-1 );
        }

        MainWinConnect::pMainWin->setWarpRemoveButtonEnabled(true);

        MainWinConnect::pMainWin->setDoorData(-2);
        break;
    }
    case HistoryOperation::LEVELHISTORY_REMOVEWARP:
    {
        LevelDoors removedDoor = lastOperation.data.doors[0];
        doorPointsSync( removedDoor.array_id, true);

        for(int i=0;i<LvlData->doors.size();i++)
        {
            if(LvlData->doors[i].array_id==removedDoor.array_id)
            {
                LvlData->doors.remove(i);
                break;
            }
        }

        QComboBox* warplist = MainWinConnect::pMainWin->getWarpList();
        for(int i = 0; i < warplist->count(); i++){
            if((unsigned int)warplist->itemData(i).toInt() == removedDoor.array_id){
                warplist->removeItem(i);
                break;
            }
        }

        if(warplist->count()<=0) MainWinConnect::pMainWin->setWarpRemoveButtonEnabled(false);

        MainWinConnect::pMainWin->setDoorData(-2);
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSWARP:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        int array_id = lastOperation.extraData.toList()[0].toInt();
        int index = -1;
        QVariant extraData = lastOperation.extraData.toList()[1];
        LevelDoors * doorp;
        bool found = false;

        for(int i = 0; i < LvlData->doors.size(); i++){
            if(LvlData->doors[i].array_id == (unsigned int)array_id){
                found = true;
                doorp = LvlData->doors.data();
                index = i;
                break;
            }
        }

        if(!found)
            break;


        if(subtype == SETTING_NOYOSHI){
            doorp[index].noyoshi = extraData.toBool();
        }
        else
        if(subtype == SETTING_ALLOWNPC){
            doorp[index].allownpc = extraData.toBool();
        }
        else
        if(subtype == SETTING_LOCKED){
            doorp[index].locked = extraData.toBool();
        }
        else
        if(subtype == SETTING_WARPTYPE){
            doorp[index].type = extraData.toList()[1].toInt();
        }
        else
        if(subtype == SETTING_NEEDASTAR){
            doorp[index].stars = extraData.toList()[1].toInt();
        }
        else
        if(subtype == SETTING_ENTRDIR){
            doorp[index].idirect = extraData.toList()[1].toInt();
        }
        else
        if(subtype == SETTING_EXITDIR){
            doorp[index].odirect = extraData.toList()[1].toInt();
        }
        else
        if(subtype == SETTING_LEVELEXIT){
            doorp[index].lvl_o = extraData.toList()[0].toBool();
            if(!(((!doorp[index].lvl_o) && (!doorp[index].lvl_i)) || (doorp[index].lvl_i)))
            {
                doorp[index].ox = extraData.toList()[1].toInt();
                doorp[index].oy = extraData.toList()[2].toInt();
            }
        }
        else
        if(subtype == SETTING_LEVELENTR){
            doorp[index].lvl_i = extraData.toList()[0].toBool();
            if(!(((!doorp[index].lvl_o) && (!doorp[index].lvl_i)) || ((doorp[index].lvl_o) && (!doorp[index].lvl_i))))
            {
                doorp[index].ix = extraData.toList()[1].toInt();
                doorp[index].iy = extraData.toList()[2].toInt();
            }
        }
        else
        if(subtype == SETTING_LEVELWARPTO){
            doorp[index].warpto = extraData.toList()[1].toInt();
        }

        MainWinConnect::pMainWin->setDoorData(-2);
        doorPointsSync(array_id);

        break;
    }
    case HistoryOperation::LEVELHISTORY_ADDEVENT:
    {
        int array_id = lastOperation.extraData.toList()[0].toInt();
        int listindex = lastOperation.extraData.toList()[1].toInt();
        QString name = lastOperation.extraData.toList()[2].toString();

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        QListWidgetItem * item;
        item = new QListWidgetItem;
        item->setText(name);
        item->setFlags(Qt::ItemIsEditable);
        item->setFlags(item->flags() | Qt::ItemIsEnabled);
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
        item->setData(3, array_id );
        QListWidget* evList = MainWinConnect::pMainWin->getEventList();
        LevelEvents NewEvent = FileFormats::dummyLvlEvent();
        NewEvent.name = item->text();
        NewEvent.array_id = array_id;
        LvlData->events.push_back(NewEvent);
        LvlData->modified = true;

        if(evList->count() > listindex){
            evList->insertItem(listindex, name);
        }else{
            evList->addItem(item);
        }

        MainWinConnect::pMainWin->EventListsSync();
        MainWinConnect::pMainWin->setEventToolsLocked(false);
        break;
    }
    default:
        break;

    }
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Redone: %1").arg(getHistoryText(lastOperation)));
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

void LvlScene::historyRedoMoveDoors(LvlScene::CallbackData cbData, LevelDoors data, bool isEntrance)
{
    if(isEntrance){
        long diffX = data.ix - cbData.x;
        long diffY = data.iy - cbData.y;

        cbData.item->setPos(QPointF(cbData.hist->x+diffX,cbData.hist->y+diffY));
        ((ItemDoor *)(cbData.item))->doorData.ix = (long)cbData.item->scenePos().x();
        ((ItemDoor *)(cbData.item))->doorData.iy = (long)cbData.item->scenePos().y();
        if((((ItemDoor *)(cbData.item))->doorData.lvl_i)||((ItemDoor *)(cbData.item))->doorData.lvl_o)
        {
            ((ItemDoor *)(cbData.item))->doorData.ox = (long)(cbData.item)->scenePos().x();
            ((ItemDoor *)(cbData.item))->doorData.oy = (long)(cbData.item)->scenePos().y();
        }
    }else{
        long diffX = data.ox - cbData.x;
        long diffY = data.oy - cbData.y;

        cbData.item->setPos(QPointF(cbData.hist->x+diffX,cbData.hist->y+diffY));
        ((ItemDoor *)(cbData.item))->doorData.ox = (long)cbData.item->scenePos().x();
        ((ItemDoor *)(cbData.item))->doorData.oy = (long)cbData.item->scenePos().y();
        if((((ItemDoor *)(cbData.item))->doorData.lvl_i)||((ItemDoor *)(cbData.item))->doorData.lvl_o)
        {
            ((ItemDoor *)(cbData.item))->doorData.ix = (long)(cbData.item)->scenePos().x();
            ((ItemDoor *)(cbData.item))->doorData.iy = (long)(cbData.item)->scenePos().y();
        }
    }
    ((ItemDoor *)(cbData.item))->arrayApply();
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

void LvlScene::historyUndoMoveDoors(LvlScene::CallbackData cbData, LevelDoors data, bool isEntrance)
{
    if(isEntrance){
        cbData.item->setPos(QPointF(data.ix, data.iy));
        ((ItemDoor *)(cbData.item))->doorData.ix = (long)cbData.item->scenePos().x();
        ((ItemDoor *)(cbData.item))->doorData.iy = (long)cbData.item->scenePos().y();
        if((((ItemDoor *)(cbData.item))->doorData.lvl_i)||((ItemDoor *)(cbData.item))->doorData.lvl_o)
        {
            ((ItemDoor *)(cbData.item))->doorData.ox = (long)(cbData.item)->scenePos().x();
            ((ItemDoor *)(cbData.item))->doorData.oy = (long)(cbData.item)->scenePos().y();
        }
    }else{
        cbData.item->setPos(QPointF(data.ox, data.oy));
        ((ItemDoor *)(cbData.item))->doorData.ox = (long)cbData.item->scenePos().x();
        ((ItemDoor *)(cbData.item))->doorData.oy = (long)cbData.item->scenePos().y();
        if((((ItemDoor *)(cbData.item))->doorData.lvl_i)||((ItemDoor *)(cbData.item))->doorData.lvl_o)
        {
            ((ItemDoor *)(cbData.item))->doorData.ix = (long)(cbData.item)->scenePos().x();
            ((ItemDoor *)(cbData.item))->doorData.iy = (long)(cbData.item)->scenePos().y();
        }
    }
    ((ItemDoor *)(cbData.item))->arrayApply();
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

void LvlScene::historyUndoSettingsInvisibleBlock(LvlScene::CallbackData cbData, LevelBlock data)
{
    ((ItemBlock*)cbData.item)->setInvisible(data.invisible);
}

void LvlScene::historyRedoSettingsInvisibleBlock(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->setInvisible(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsSlipperyBlock(LvlScene::CallbackData cbData, LevelBlock data)
{
    ((ItemBlock*)cbData.item)->setSlippery(data.slippery);
}

void LvlScene::historyRedoSettingsSlipperyBlock(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->setSlippery(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsFriendlyNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setFriendly(data.friendly);
}

void LvlScene::historyRedoSettingsFriendlyNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setFriendly(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsBossNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setLegacyBoss(data.legacyboss);
}

void LvlScene::historyRedoSettingsBossNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setLegacyBoss(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsNoMoveableNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setNoMovable(data.nomove);
}

void LvlScene::historyRedoSettingsNoMoveableNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setNoMovable(cbData.hist->extraData.toBool());
}

void LvlScene::historyUndoSettingsMessageNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setMsg(data.msg);
}

void LvlScene::historyRedoSettingsMessageNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setMsg(cbData.hist->extraData.toString());
}

void LvlScene::historyUndoSettingsDirectionNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->changeDirection(data.direct);
}

void LvlScene::historyRedoSettingsDirectionNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->changeDirection(cbData.hist->extraData.toInt());
}

void LvlScene::historyUndoSettingsChangeNPCBlocks(LvlScene::CallbackData cbData, LevelBlock data)
{
    ItemBlock* targetItem = (ItemBlock*)cbData.item;
    int targetNPC_id = data.npc_id;
    //targetItem->blockData.npc_id = (unsigned long)targetNPC_id;
    //targetItem->arrayApply();
    targetItem->setIncludedNPC((unsigned long)targetNPC_id);
}

void LvlScene::historyRedoSettingsChangeNPCBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ItemBlock* targetItem = (ItemBlock*)cbData.item;
    int targetNPC_id = cbData.hist->extraData.toInt();
    //targetItem->blockData.npc_id = (unsigned long)targetNPC_id;
    //targetItem->arrayApply();
    targetItem->setIncludedNPC((unsigned long)targetNPC_id);
}

void LvlScene::historyUndoSettingsChangeNPCNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setIncludedNPC(data.special_data);
}

void LvlScene::historyRedoSettingsChangeNPCNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->setIncludedNPC(cbData.hist->extraData.toInt());
}

void LvlScene::historyUndoSettingsTypeWater(LvlScene::CallbackData cbData, LevelWater data)
{
    ((ItemWater*)cbData.item)->setType(data.quicksand ? 1 : 0);
}

void LvlScene::historyRedoSettingsTypeWater(LvlScene::CallbackData cbData, LevelWater /*data*/)
{
    ((ItemWater*)cbData.item)->setType(cbData.hist->extraData.toBool() ? 0 : 1);
}

void LvlScene::historyUndoSettingsNoYoshiDoors(LvlScene::CallbackData cbData, LevelDoors /*data*/, bool /*isEntrance*/)
{
    ((ItemDoor*)cbData.item)->doorData.noyoshi = !cbData.hist->extraData.toBool();
    ((ItemDoor*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsNoYoshiDoors(LvlScene::CallbackData cbData, LevelDoors /*data*/, bool /*isEntrance*/)
{
    ((ItemDoor*)cbData.item)->doorData.noyoshi = cbData.hist->extraData.toBool();
    ((ItemDoor*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsAllowNPCDoors(LvlScene::CallbackData cbData, LevelDoors /*data*/, bool /*isEntrance*/)
{
    ((ItemDoor*)cbData.item)->doorData.allownpc = !cbData.hist->extraData.toBool();
    ((ItemDoor*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsAllowNPCDoors(LvlScene::CallbackData cbData, LevelDoors /*data*/, bool /*isEntrance*/)
{
    ((ItemDoor*)cbData.item)->doorData.allownpc = cbData.hist->extraData.toBool();
    ((ItemDoor*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsLockedDoors(LvlScene::CallbackData cbData, LevelDoors /*data*/, bool /*isEntrance*/)
{
    ((ItemDoor*)cbData.item)->doorData.locked = !cbData.hist->extraData.toBool();
    ((ItemDoor*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsLockedDoors(LvlScene::CallbackData cbData, LevelDoors /*data*/, bool /*isEntrance*/)
{
    ((ItemDoor*)cbData.item)->doorData.locked = cbData.hist->extraData.toBool();
    ((ItemDoor*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsActivateGeneratorNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setGenerator(data.generator, data.generator_direct, data.generator_type);
}

void LvlScene::historyRedoSettingsActivateGeneratorNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setGenerator(cbData.hist->extraData.toBool(), data.generator_direct, data.generator_type);
}

void LvlScene::historyUndoSettingsTypeGeneratorNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setGenerator(data.generator, data.generator_direct, data.generator_type);
}

void LvlScene::historyRedoSettingsTypeGeneratorNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setGenerator(data.generator, data.generator_direct, cbData.hist->extraData.toInt());
}

void LvlScene::historyUndoSettingsDirectionGeneratorNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setGenerator(data.generator, data.generator_direct, data.generator_type);
}

void LvlScene::historyRedoSettingsDirectionGeneratorNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->setGenerator(data.generator, cbData.hist->extraData.toInt(), data.generator_type);
}

void LvlScene::historyUndoSettingsTimeGeneratorNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->npcData.generator_period = data.generator_period;
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsTimeGeneratorNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->npcData.generator_period = cbData.hist->extraData.toInt();
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsAttachLayerNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->npcData.attach_layer = data.attach_layer;
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsAttachLayerNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->npcData.attach_layer = cbData.hist->extraData.toString();
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsDestroyedEventBlocks(LvlScene::CallbackData cbData, LevelBlock data)
{
    ((ItemBlock*)cbData.item)->blockData.event_destroy = data.event_destroy;
    ((ItemBlock*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsDestroyedEventBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->blockData.event_destroy = cbData.hist->extraData.toString();
    ((ItemBlock*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsHitedEventBlocks(LvlScene::CallbackData cbData, LevelBlock data)
{
    ((ItemBlock*)cbData.item)->blockData.event_hit = data.event_hit;
    ((ItemBlock*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsHitedEventBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->blockData.event_hit = cbData.hist->extraData.toString();
    ((ItemBlock*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsLayerEmptyEventBlocks(LvlScene::CallbackData cbData, LevelBlock data)
{
    ((ItemBlock*)cbData.item)->blockData.event_no_more = data.event_no_more;
    ((ItemBlock*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsLayerEmptyEventBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->blockData.event_no_more = cbData.hist->extraData.toString();
    ((ItemBlock*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsLayerEmptyEventNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->npcData.event_nomore = data.event_nomore;
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsLayerEmptyEventNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->npcData.event_nomore = cbData.hist->extraData.toString();
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsActivateEventNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->npcData.event_activate = data.event_activate;
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsActivateEventNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->npcData.event_activate = cbData.hist->extraData.toString();
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsDeathEventNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->npcData.event_die = data.event_die;
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsDeathEventNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->npcData.event_die = cbData.hist->extraData.toString();
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsTalkEventNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->npcData.event_talk = data.event_talk;
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsTalkEventNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->npcData.event_talk = cbData.hist->extraData.toString();
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyUndoSettingsSpecialDataNPC(LvlScene::CallbackData cbData, LevelNPC data)
{
    ((ItemNPC*)cbData.item)->npcData.special_data = data.special_data;
    ((ItemNPC*)cbData.item)->arrayApply();
}

void LvlScene::historyRedoSettingsSpecialDataNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC*)cbData.item)->npcData.special_data = cbData.hist->extraData.toInt();
    ((ItemNPC*)cbData.item)->arrayApply();
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

void LvlScene::historyUndoResizeBlock(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    QList<QVariant> package = cbData.hist->extraData.toList();
    QList<QVariant> oldSizes = package[0].toList();
    long tarLeft = (long)oldSizes[0].toLongLong();
    long tarTop = (long)oldSizes[1].toLongLong();
    long tarRight = (long)oldSizes[2].toLongLong();
    long tarBottom = (long)oldSizes[3].toLongLong();
    ((ItemBlock *)cbData.item)->setBlockSize(QRect(tarLeft, tarTop, tarRight-tarLeft, tarBottom-tarTop));
}

void LvlScene::historyRedoResizeBlock(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    QList<QVariant> package = cbData.hist->extraData.toList();
    QList<QVariant> oldSizes = package[1].toList();
    long tarLeft = (long)oldSizes[0].toLongLong();
    long tarTop = (long)oldSizes[1].toLongLong();
    long tarRight = (long)oldSizes[2].toLongLong();
    long tarBottom = (long)oldSizes[3].toLongLong();
    ((ItemBlock *)cbData.item)->setBlockSize(QRect(tarLeft, tarTop, tarRight-tarLeft, tarBottom-tarTop));
}

void LvlScene::historyRemoveDoors(LvlScene::CallbackData cbData, LevelDoors /*door*/, bool /*isEntrance*/)
{
    ((ItemDoor *)(cbData.item))->removeFromArray();
    if((cbData.item)) delete (cbData.item);
    MainWinConnect::pMainWin->setDoorData(-2);
}

void LvlScene::findGraphicsItem(LevelData toFind,
                                HistoryOperation * operation,
                                CallbackData customData,
                                callBackLevelBlock clbBlock,
                                callBackLevelBGO clbBgo,
                                callBackLevelNPC clbNpc,
                                callBackLevelWater clbWater,
                                callBackLevelDoors clbDoor,
                                bool ignoreBlock,
                                bool ignoreBGO,
                                bool ignoreNPC,
                                bool ignoreWater,
                                bool ignoreDoors)
{
    QMap<int, LevelDoors> sortedEntranceDoors;
    QMap<int, LevelDoors> sortedExitDoors;
    if(!ignoreDoors){
        foreach (LevelDoors door, toFind.doors) {
            if(door.isSetIn&&!door.isSetOut){
                sortedEntranceDoors[door.array_id] = door;
            }
        }
        foreach (LevelDoors door, toFind.doors) {
            if(!door.isSetIn&&door.isSetOut){
                sortedExitDoors[door.array_id] = door;
            }
        }
    }
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
    QMap<int, QGraphicsItem*> sortedGraphDoorEntrance;
    QMap<int, QGraphicsItem*> sortedGraphDoorExit;
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
        else
        if(unsortedItem->data(0).toString()=="Door_enter")
        {
            if(!ignoreDoors){
                sortedGraphDoorEntrance[unsortedItem->data(2).toInt()] = unsortedItem;
            }

        }
        else
        if(unsortedItem->data(0).toString()=="Door_exit")
        {
            if(!ignoreDoors){
                sortedGraphDoorExit[unsortedItem->data(2).toInt()] = unsortedItem;
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

    if(!ignoreDoors)
    {
        foreach (QGraphicsItem* item, sortedGraphDoorEntrance)
        {
            if(sortedEntranceDoors.size()!=0)
            {
                QMap<int, LevelDoors>::iterator beginItem = sortedEntranceDoors.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedEntranceDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedEntranceDoors.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbDoor)(cbData,(*beginItem),true);
                    sortedEntranceDoors.erase(beginItem);
                }
            }
        }
        foreach (QGraphicsItem* item, sortedGraphDoorExit)
        {
            if(sortedExitDoors.size()!=0)
            {
                QMap<int, LevelDoors>::iterator beginItem = sortedExitDoors.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedExitDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedExitDoors.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbDoor)(cbData,(*beginItem),false);
                    sortedExitDoors.erase(beginItem);
                }
            }
        }
        MainWinConnect::pMainWin->setDoorData(-2); //update Door data
    }

}

void LvlScene::findGraphicsDoor(int array_id, LvlScene::HistoryOperation *operation, LvlScene::CallbackData customData, LvlScene::callBackLevelDoors clbDoors, bool isEntrance)
{
    CallbackData cbData = customData;
    cbData.hist = operation;
    foreach (QGraphicsItem* i, items())
    {
        if(i->data(2).toInt() == array_id){
            if(isEntrance && i->data(0).toString()=="Door_enter"){
                //(this->*clbDoors)(CallbackData, LevelDoors, bool)
                //find DoorData
                foreach(LevelDoors findDoor, LvlData->doors){
                    if(array_id == (int)findDoor.array_id){
                        cbData.item = i;
                        (this->*clbDoors)(cbData, findDoor, true);
                        break;
                    }
                }
            }else if(!isEntrance && i->data(0).toString()=="Door_exit"){
                foreach(LevelDoors findDoor, LvlData->doors){
                    if(array_id == (int)findDoor.array_id){
                        cbData.item = i;
                        (this->*clbDoors)(cbData, findDoor, true);
                        break;
                    }
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
    }else if(!data->water.isEmpty()){
        baseX = (int)data->water[0].x;
        baseY = (int)data->water[0].y;
    }else if(!data->doors.isEmpty()){
        if(data->doors[0].isSetIn&&!data->doors[0].isSetOut){
            baseX = data->doors[0].ix;
            baseY = data->doors[0].iy;
        }
        else
        if(!data->doors[0].isSetIn&&data->doors[0].isSetOut){
            baseX = data->doors[0].ox;
            baseY = data->doors[0].oy;
        }
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
    foreach (LevelWater water, data->water) {
        if((int)water.x<baseX){
            baseX = (int)water.x;
        }
        if((int)water.y<baseY){
            baseY = (int)water.y;
        }
    }
    foreach (LevelDoors door, data->doors) {
        if(door.isSetIn&&!door.isSetOut){
            if((int)door.ix<baseX){
                baseX = (int)door.ix;
            }
            if((int)door.iy<baseY){
                baseY = (int)door.iy;
            }
        }
        else
        if(!door.isSetIn&&door.isSetOut){
            if((int)door.ox<baseX){
                baseX = (int)door.ox;
            }
            if((int)door.oy<baseY){
                baseY = (int)door.oy;
            }
        }
    }


    return QPoint(baseX, baseY);
}

QString LvlScene::getHistoryText(LvlScene::HistoryOperation operation)
{
    switch (operation.type) {
    case HistoryOperation::LEVELHISTORY_REMOVE: return tr("Remove");
    case HistoryOperation::LEVELHISTORY_PLACE: return tr("Place");
    case HistoryOperation::LEVELHISTORY_MOVE: return tr("Move");
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS: return tr("Changed Setting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    case HistoryOperation::LEVELHISTORY_CHANGEDLAYER: return tr("Change Layer");
    case HistoryOperation::LEVELHISTORY_RESIZEBLOCK: return tr("Resize Block");
    case HistoryOperation::LEVELHISTORY_PLACEDOOR: return tr("Place Door");
    case HistoryOperation::LEVELHISTORY_ADDWARP: return tr("Add Warp");
    case HistoryOperation::LEVELHISTORY_REMOVEWARP: return tr("Remove Warp");
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSWARP: return tr("Changed Warpsetting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    default:
        return tr("Unknown");
    }
}

QString LvlScene::getHistorySettingText(LvlScene::SettingSubType subType)
{
    switch (subType) {
    case SETTING_SLIPPERY: return tr("Slippery");
    case SETTING_FRIENDLY: return tr("Friendly");
    case SETTING_BOSS: return tr("Boss");
    case SETTING_NOMOVEABLE: return tr("Not Moveable");
    case SETTING_MESSAGE: return tr("Message");
    case SETTING_DIRECTION: return tr("Direction");
    case SETTING_CHANGENPC: return tr("Included NPC");
    case SETTING_WATERTYPE: return tr("Water Type");
    case SETTING_NOYOSHI: return tr("No Yoshi");
    case SETTING_ALLOWNPC: return tr("Allow NPC");
    case SETTING_LOCKED: return tr("Locked");
    case SETTING_WARPTYPE: return tr("Warp Type");
    case SETTING_NEEDASTAR: return tr("Need Stars");
    case SETTING_ENTRDIR: return tr("Entrance Direction");
    case SETTING_EXITDIR: return tr("Exit Direction");
    case SETTING_LEVELEXIT: return tr("Set Level Exit");
    case SETTING_LEVELENTR: return tr("Set Level Entrance");
    case SETTING_LEVELWARPTO: return tr("Level Warp To");
    case SETTING_GENACTIVATE: return tr("Activate Generator");
    case SETTING_GENTYPE: return tr("Generator Type");
    case SETTING_GENDIR: return tr("Generator Direction");
    case SETTING_GENTIME: return tr("Generator Time");
    case SETTING_ATTACHLAYER: return tr("Attach Layer");
    case SETTING_EV_DESTROYED: return tr("Event Block Destroyed");
    case SETTING_EV_HITED: return tr("Event Block Hited");
    case SETTING_EV_LAYER_EMP: return tr("Event Layer Empty");
    case SETTING_EV_ACTIVATE: return tr("Event NPC Activate");
    case SETTING_EV_DEATH: return tr("Event NPC Die");
    case SETTING_EV_TALK: return tr("Event NPC Talk");
    case SETTING_SPECIAL_DATA: return tr("NPC Special Data");
    default:
        return tr("Unknown");
    }
}
