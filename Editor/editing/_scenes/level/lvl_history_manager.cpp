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

#include <common_features/logger.h>
#include <common_features/mainwinconnect.h>
#include <file_formats/file_formats.h>
#include <audio/music_player.h>
#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_sctc_props.h>

#include "lvl_scene.h"
#include "items/item_block.h"
#include "items/item_bgo.h"
#include "items/item_npc.h"
#include "items/item_water.h"
#include "items/item_door.h"
#include "items/item_playerpoint.h"

#include <editing/_components/history/historyelementmodification.h>
#include <editing/_components/history/historyelementmainsetting.h>
#include <editing/_components/history/historyelementitemsetting.h>
#include <editing/_components/history/historyelementresizesection.h>
#include <editing/_components/history/historyelementlayerchanged.h>
#include <editing/_components/history/historyelementresizeblock.h>

void LvlScene::addRemoveHistory(LevelData removedItems)
{
    //add cleanup redo elements
    cleanupRedoElements();
    //add new element
    HistoryOperation rmOperation;
    HistoryElementModification* modf = new HistoryElementModification(removedItems, LevelData());
    modf->setCustomHistoryName(tr("Remove"));
    modf->setScene(this);
    rmOperation.newElement = QSharedPointer<IHistoryElement>(modf);

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
    HistoryElementModification* modf = new HistoryElementModification(LevelData(), placedItems);
    modf->setCustomHistoryName(tr("Place"));
    modf->setScene(this);
    plOperation.newElement = QSharedPointer<IHistoryElement>(modf);

    operationList.push_back(plOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addOverwriteHistory(LevelData removedItems, LevelData placedItems)
{
    //add cleanup redo elements
    cleanupRedoElements();
    //add new element
    HistoryOperation overwriteOperation;
    HistoryElementModification* modf = new HistoryElementModification(removedItems, placedItems);
    modf->setCustomHistoryName(tr("Place & Overwrite"));
    modf->setScene(this);
    overwriteOperation.newElement = QSharedPointer<IHistoryElement>(modf);

    operationList.push_back(overwriteOperation);
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
    //add cleanup redo elements
    cleanupRedoElements();
    //add new element
    HistoryOperation moveOperation;
    HistoryElementModification* modf = new HistoryElementModification(sourceMovedItems, targetMovedItems);
    modf->setCustomHistoryName(tr("Move"));
    modf->setScene(this);
    moveOperation.newElement = QSharedPointer<IHistoryElement>(modf);

    operationList.push_back(moveOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeSettingsHistory(LevelData modifiedItems, HistorySettings::LevelSettingSubType subType, QVariant extraData)
{
    cleanupRedoElements();

    HistoryOperation modOperation;
    HistoryElementItemSetting* modf = new HistoryElementItemSetting(modifiedItems, subType, extraData);
    modf->setScene(this);
    modOperation.newElement = QSharedPointer<IHistoryElement>(modf);

    operationList.push_back(modOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addResizeSectionHistory(int sectionID, long oldLeft, long oldTop, long oldRight, long oldBottom,
                                       long newLeft, long newTop, long newRight, long newBottom)
{
    cleanupRedoElements();
    HistoryOperation resizeOperation;
    HistoryElementResizeSection *modf = new HistoryElementResizeSection(sectionID,
                                                                        QRect(QPoint(oldLeft, oldTop), QPoint(oldRight, oldBottom)),
                                                                        QRect(QPoint(newLeft, newTop), QPoint(newRight, newBottom)));
    modf->setScene(this);
    resizeOperation.newElement = QSharedPointer<IHistoryElement>(modf);

    operationList.push_back(resizeOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangedLayerHistory(LevelData changedItems, QString newLayerName)
{
    cleanupRedoElements();

    HistoryOperation chLaOperation;
    HistoryElementLayerChanged *modf = new HistoryElementLayerChanged(changedItems, newLayerName);
    modf->setScene(this);
    chLaOperation.newElement = QSharedPointer<IHistoryElement>(modf);
    operationList.push_back(chLaOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addResizeBlockHistory(LevelBlock bl, long oldLeft, long oldTop, long oldRight, long oldBottom, long newLeft, long newTop, long newRight, long newBottom)
{
    cleanupRedoElements();

    HistoryOperation resizeBlOperation;
    resizeBlOperation.type = HistoryOperation::LEVELHISTORY_RESIZEBLOCK;
    HistoryElementResizeBlock *modf = new HistoryElementResizeBlock(bl,
                                                                        QRect(QPoint(oldLeft, oldTop), QPoint(oldRight, oldBottom)),
                                                                        QRect(QPoint(newLeft, newTop), QPoint(newRight, newBottom)));
    modf->setScene(this);
    resizeBlOperation.newElement = QSharedPointer<IHistoryElement>(modf);
    operationList.push_back(resizeBlOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addResizeWaterHistory(LevelPhysEnv wt, long oldLeft, long oldTop, long oldRight, long oldBottom, long newLeft, long newTop, long newRight, long newBottom)
{
    cleanupRedoElements();

    HistoryOperation resizeWtOperation;
    resizeWtOperation.type = HistoryOperation::LEVELHISTORY_RESIZEWATER;
    LevelData wtData;
    wtData.physez.push_back(wt);
    resizeWtOperation.data = wtData;
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
    resizeWtOperation.extraData = QVariant(package);
    operationList.push_back(resizeWtOperation);
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

void LvlScene::addAddEventHistory(int array_id, QString name)
{
    cleanupRedoElements();

    HistoryOperation addEvOperation;
    addEvOperation.type = HistoryOperation::LEVELHISTORY_ADDEVENT;
    QList<QVariant> package;
    package.push_back(array_id);
    package.push_back(name);
    addEvOperation.extraData = QVariant(package);
    operationList.push_back(addEvOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRemoveEventHistory(LevelEvents ev)
{
    cleanupRedoElements();

    HistoryOperation rmEvOperation;
    rmEvOperation.type = HistoryOperation::LEVELHISTORY_REMOVEEVENT;
    rmEvOperation.data.events.push_back(ev);
    operationList.push_back(rmEvOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addDuplicateEventHistory(LevelEvents newDuplicate)
{
    cleanupRedoElements();

    HistoryOperation dupEvOperation;
    dupEvOperation.type = HistoryOperation::LEVELHISTORY_DULPICATEEVENT;
    dupEvOperation.data.events.push_back(newDuplicate);
    operationList.push_back(dupEvOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeEventSettingsHistory(int array_id, LvlScene::SettingSubType subtype, QVariant extraData)
{
    cleanupRedoElements();

    HistoryOperation chEvSettingsOperation;
    chEvSettingsOperation.type = HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSEVENT;
    chEvSettingsOperation.subtype = subtype;
    QList<QVariant> package;
    package.push_back(array_id);
    package.push_back(extraData);
    chEvSettingsOperation.extraData = QVariant(package);
    operationList.push_back(chEvSettingsOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangedNewLayerHistory(LevelData changedItems, LevelLayers newLayer)
{
    cleanupRedoElements();

    HistoryOperation chNewLaOperation;
    chNewLaOperation.type = HistoryOperation::LEVELHISTORY_CHANGEDNEWLAYER;
    chNewLaOperation.extraData = QVariant(newLayer.name);
    changedItems.layers.push_back(newLayer);
    chNewLaOperation.data = changedItems;
    operationList.push_back(chNewLaOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addAddLayerHistory(int array_id, QString name)
{
    cleanupRedoElements();

    HistoryOperation addNewLaOperation;
    addNewLaOperation.type = HistoryOperation::LEVELHISTORY_ADDLAYER;
    QList<QVariant> layerData;
    layerData.push_back(array_id);
    layerData.push_back(name);
    addNewLaOperation.extraData = QVariant(layerData);
    operationList.push_back(addNewLaOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRemoveLayerHistory(LevelData modData)
{
    cleanupRedoElements();

    HistoryOperation rmLaOperation;
    rmLaOperation.type = HistoryOperation::LEVELHISTORY_REMOVELAYER;
    rmLaOperation.data = modData;
    operationList.push_back(rmLaOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRenameEventHistory(int array_id, QString oldName, QString newName)
{
    cleanupRedoElements();

    HistoryOperation renameEvOperation;
    renameEvOperation.type = HistoryOperation::LEVELHISTORY_RENAMEEVENT;
    QList<QVariant> renameData;
    renameData.push_back(array_id);
    renameData.push_back(oldName);
    renameData.push_back(newName);
    renameEvOperation.extraData = QVariant(renameData);
    operationList.push_back(renameEvOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRenameLayerHistory(int array_id, QString oldName, QString newName)
{
    cleanupRedoElements();

    HistoryOperation renameLaOperation;
    renameLaOperation.type = HistoryOperation::LEVELHISTORY_RENAMELAYER;
    QList<QVariant> renameData;
    renameData.push_back(array_id);
    renameData.push_back(oldName);
    renameData.push_back(newName);
    renameLaOperation.extraData = QVariant(renameData);
    operationList.push_back(renameLaOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRemoveLayerAndSaveItemsHistory(LevelData modData)
{
    cleanupRedoElements();

    HistoryOperation rmLaAndSaveItemsOperation;
    rmLaAndSaveItemsOperation.type = HistoryOperation::LEVELHISTORY_REMOVELAYERANDSAVE;
    rmLaAndSaveItemsOperation.data = modData;
    rmLaAndSaveItemsOperation.extraData = QVariant(QString("Default"));
    operationList.push_back(rmLaAndSaveItemsOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addMergeLayer(LevelData mergedData, QString newLayerName)
{
    cleanupRedoElements();

    HistoryOperation mergeLaOperation;
    mergeLaOperation.type = HistoryOperation::LEVELHISTORY_MERGELAYER;
    mergeLaOperation.data = mergedData;
    mergeLaOperation.extraData = QVariant(newLayerName);
    operationList.push_back(mergeLaOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeSectionSettingsHistory(int sectionID, LvlScene::SettingSubType subtype, QVariant extraData)
{
    cleanupRedoElements();

    HistoryOperation chSecSettingsOperation;
    chSecSettingsOperation.type = HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSSECTION;
    chSecSettingsOperation.subtype = subtype;
    QList<QVariant> package;
    package.push_back(sectionID);
    package.push_back(extraData);
    chSecSettingsOperation.extraData = QVariant(package);
    operationList.push_back(chSecSettingsOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeLevelSettingsHistory(HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    cleanupRedoElements();

    HistoryOperation chLevelSettingsOperation;
    HistoryElementMainSetting* modf = new HistoryElementMainSetting(subtype, extraData);
    modf->setScene(this);
    chLevelSettingsOperation.newElement = QSharedPointer<IHistoryElement>(modf);
    operationList.push_back(chLevelSettingsOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addPlacePlayerPointHistory(PlayerPoint plr, QVariant oldPos)
{
    cleanupRedoElements();

    HistoryOperation plPlrPointOperation;
    plPlrPointOperation.type = HistoryOperation::LEVELHISTORY_REPLACEPLAYERPOINT;
    plPlrPointOperation.data.players.push_back(plr);
    if(!((long)oldPos.toList()[3].toLongLong() == 0 && (long)oldPos.toList()[4].toLongLong() == 0))
        plPlrPointOperation.extraData = oldPos;
    operationList.push_back(plPlrPointOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRotateHistory(LevelData rotatedItems, bool byClockwise)
{
    cleanupRedoElements();

    HistoryOperation rotateOperation;
    rotateOperation.type = HistoryOperation::LEVELHISTORY_ROTATE;
    rotateOperation.data = rotatedItems;
    rotateOperation.extraData = byClockwise;
    operationList.push_back(rotateOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addFlipHistory(LevelData flippedItems, bool vertical)
{
    cleanupRedoElements();

    HistoryOperation rotateOperation;
    rotateOperation.type = HistoryOperation::LEVELHISTORY_FLIP;
    rotateOperation.data = flippedItems;
    rotateOperation.extraData = vertical;
    operationList.push_back(rotateOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::historyBack()
{
    historyIndex--;
    HistoryOperation lastOperation = operationList[historyIndex];

    if(!lastOperation.newElement.isNull()){
        lastOperation.newElement->undo();
        LvlData->modified = true;

        Debugger_updateItemList();
        MainWinConnect::pMainWin->refreshHistoryButtons();
        MainWinConnect::pMainWin->showStatusMsg(tr("Undone: %1").arg(lastOperation.newElement->getHistoryName()));
        return;
    }

    switch( lastOperation.type )
    {
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

        QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();

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


        if(warplist->count()<=0) MainWinConnect::pMainWin->dock_LvlWarpProps->setWarpRemoveButtonEnabled(false);

        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

        //warplist->update();
        //warplist->repaint();
        break;
    }
    case HistoryOperation::LEVELHISTORY_REMOVEWARP:
    {
        LevelDoors removedDoor = lastOperation.data.doors[0];
        LvlData->doors.insert(removedDoor.index, removedDoor);

        QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
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


        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
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
            doorp[index].novehicles = !extraData.toBool();
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

        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
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
                    if(MainWinConnect::pMainWin->getEventList()->item(j)->data(ITEM_BLOCK_IS_SIZABLE).toInt() == array_id){
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
    case HistoryOperation::LEVELHISTORY_REMOVEEVENT:
    {
        LevelEvents rmEvents = lastOperation.data.events[0];

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        QListWidgetItem * item;
        item = new QListWidgetItem;
        item->setText(rmEvents.name);
        item->setFlags(Qt::ItemIsEditable);
        item->setFlags(item->flags() | Qt::ItemIsEnabled);
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
        item->setData(3, QString::number(rmEvents.array_id) );
        QListWidget* evList = MainWinConnect::pMainWin->getEventList();
        LevelEvents NewEvent = rmEvents;


        LvlData->events.push_back(NewEvent);
        evList->addItem(item);

        LvlData->modified = true;

        MainWinConnect::pMainWin->EventListsSync();
        MainWinConnect::pMainWin->setEventToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_DULPICATEEVENT:
    {
        LevelEvents dupEvents = lastOperation.data.events[0];

        for (int i = 0; i < LvlData->events.size(); i++) {
            if(LvlData->events[i].array_id == (unsigned int)dupEvents.array_id){
                for(int j = 0; j < MainWinConnect::pMainWin->getEventList()->count(); j++){
                    if((unsigned int)MainWinConnect::pMainWin->getEventList()->item(j)->data(ITEM_BLOCK_IS_SIZABLE).toInt() == dupEvents.array_id){
                        delete MainWinConnect::pMainWin->getEventList()->item(j);
                    }
                }
                MainWinConnect::pMainWin->ModifyEvent(LvlData->events[i].name, "");
                LvlData->events.remove(i);
            }
        }

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSEVENT:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        int array_id = lastOperation.extraData.toList()[0].toInt();
        int index = -1;
        QVariant extraData = lastOperation.extraData.toList()[1];
        LevelEvents * eventp;
        bool found = false;

        for(int i = 0; i < LvlData->events.size(); i++){
            if(LvlData->events[i].array_id == (unsigned int)array_id){
                found = true;
                eventp = LvlData->events.data();
                index = i;
                break;
            }
        }

        if(!found)
            break;

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        if(subtype == SETTING_EV_AUTOSTART){
            eventp[index].autostart = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_SMOKE){
            eventp[index].nosmoke = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_LSHOWADD){
            QString layer = extraData.toString();
            if(!eventp[index].layers_show.isEmpty()){
                for(int i = 0; i < eventp[index].layers_show.size(); i++){
                    if(eventp[index].layers_show[i] == layer){
                        eventp[index].layers_show.removeAt(i);
                    }
                }
            }
        }
        else
        if(subtype == SETTING_EV_LHIDEADD){
            QString layer = extraData.toString();
            if(!eventp[index].layers_hide.isEmpty()){
                for(int i = 0; i < eventp[index].layers_hide.size(); i++){
                    if(eventp[index].layers_hide[i] == layer){
                        eventp[index].layers_hide.removeAt(i);
                    }
                }
            }
        }
        else
        if(subtype == SETTING_EV_LTOGADD){
            QString layer = extraData.toString();
            if(!eventp[index].layers_toggle.isEmpty()){
                for(int i = 0; i < eventp[index].layers_toggle.size(); i++){
                    if(eventp[index].layers_toggle[i] == layer){
                        eventp[index].layers_toggle.removeAt(i);
                    }
                }
            }
        }
        else
        if(subtype == SETTING_EV_LSHOWDEL){
            QString layer = extraData.toString();
            eventp[index].layers_show.push_back(layer);
        }
        else
        if(subtype == SETTING_EV_LHIDEDEL){
            QString layer = extraData.toString();
            eventp[index].layers_hide.push_back(layer);
        }
        else
        if(subtype == SETTING_EV_LTOGDEL){
            QString layer = extraData.toString();
            eventp[index].layers_toggle.push_back(layer);
        }
        else
        if(subtype == SETTING_EV_MOVELAYER){
            QString layer = extraData.toList()[0].toString();
            eventp[index].movelayer = layer;
        }
        else
        if(subtype == SETTING_EV_SPEEDLAYERX){
            eventp[index].layer_speed_x = extraData.toList()[0].toDouble();
        }
        else
        if(subtype == SETTING_EV_SPEEDLAYERY){
            eventp[index].layer_speed_y = extraData.toList()[0].toDouble();
        }
        else
        if(subtype == SETTING_EV_AUTOSCRSEC){
            eventp[index].scroll_section = (long)extraData.toList()[0].toLongLong();
        }
        else
        if(subtype == SETTING_EV_AUTOSCRX){
            eventp[index].move_camera_x = extraData.toList()[0].toDouble();
        }
        else
        if(subtype == SETTING_EV_AUTOSCRY){
            eventp[index].move_camera_y = extraData.toList()[0].toDouble();
        }
        else
        if(subtype == SETTING_EV_SECSIZE){
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].position_top = (long)extraData.toList()[1].toLongLong();
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].position_right = (long)extraData.toList()[2].toLongLong();
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].position_bottom = (long)extraData.toList()[3].toLongLong();
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].position_left = (long)extraData.toList()[4].toLongLong();
        }
        else
        if(subtype == SETTING_EV_SECMUS){
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].music_id = (long)extraData.toList()[1].toLongLong();
        }
        else
        if(subtype == SETTING_EV_SECBG){
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].background_id = (long)extraData.toList()[1].toLongLong();
        }
        else
        if(subtype == SETTING_EV_MSG){
            eventp[index].msg = extraData.toList()[0].toString();
        }
        else
        if(subtype == SETTING_EV_SOUND){
            eventp[index].sound_id = (long)extraData.toList()[0].toLongLong();
        }
        else
        if(subtype == SETTING_EV_ENDGAME){
            eventp[index].end_game = (long)extraData.toList()[0].toLongLong();
        }
        else
        if(subtype == SETTING_EV_KUP){
            eventp[index].ctrl_up = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KDOWN){
            eventp[index].ctrl_down = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KLEFT){
            eventp[index].ctrl_left = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KRIGHT){
            eventp[index].ctrl_right = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KRUN){
            eventp[index].ctrl_run = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KALTRUN){
            eventp[index].ctrl_altrun = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KJUMP){
            eventp[index].ctrl_jump = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KALTJUMP){
            eventp[index].ctrl_altjump = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KDROP){
            eventp[index].ctrl_drop = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KSTART){
            eventp[index].ctrl_start = !extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_TRIACTIVATE){
            eventp[index].trigger = extraData.toList()[0].toString();
        }
        else
        if(subtype == SETTING_EV_TRIDELAY){
            eventp[index].trigger_timer = (long)extraData.toList()[0].toLongLong();
        }


        MainWinConnect::pMainWin->setEventData(-2);
        MainWinConnect::pMainWin->setEventToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDNEWLAYER:
    {
        LevelData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyUndoChangeLayerBlocks, &LvlScene::historyUndoChangeLayerBGO, &LvlScene::historyUndoChangeLayerNPC, &LvlScene::historyUndoChangeLayerWater, &LvlScene::historyUndoChangeLayerDoor, 0, false, false, false, false, false, true);
        for(int i = 0; i < LvlData->layers.size(); i++){
            if(LvlData->layers[i].array_id == lastOperation.data.layers[0].array_id){
                LvlData->layers.removeAt(i);
            }
        }
        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_ADDLAYER:
    {
        for(int i = 0; i < LvlData->layers.size(); i++){
            if(LvlData->layers[i].array_id == (unsigned int)lastOperation.extraData.toList()[0].toInt()){
                LvlData->layers.removeAt(i);
            }
        }
        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_REMOVELAYER:
    {
        LvlData->layers.push_back(lastOperation.data.layers[0]);

        foreach(LevelBlock b, lastOperation.data.blocks){
            LvlData->blocks.push_back(b);
            placeBlock(b);
        }
        foreach (LevelBGO b, lastOperation.data.bgo){
            LvlData->bgo.push_back(b);
            placeBGO(b);
        }
        foreach (LevelNPC n, lastOperation.data.npc){
            LvlData->npc.push_back(n);
            placeNPC(n);
        }
        foreach (LevelPhysEnv w, lastOperation.data.physez) {
            LvlData->physez.push_back(w);
            placeWater(w);
        }

        //merge doors
        foreach (LevelDoors d, lastOperation.data.doors) {
            for(int i = 0; i < LvlData->doors.size(); i++){
                if(d.array_id == LvlData->doors[i].array_id){
                    if(d.isSetIn&&!d.isSetOut){
                        if(!LvlData->doors[i].isSetIn){
                            LvlData->doors[i].isSetIn = true;
                            LvlData->doors[i].ix = d.ix;
                            LvlData->doors[i].iy = d.iy;
                            placeDoorEnter(LvlData->doors[i]);
                        }
                    }
                    else
                    if(!d.isSetIn&&d.isSetOut){
                        if(!LvlData->doors[i].isSetOut){
                            LvlData->doors[i].isSetOut = true;
                            LvlData->doors[i].ox = d.ox;
                            LvlData->doors[i].oy = d.oy;
                            placeDoorExit(LvlData->doors[i]);
                        }
                    }
                }
            }
        }

        CallbackData cbData;
        findGraphicsItem(lastOperation.data, &lastOperation, cbData, &LvlScene::historyUpdateVisibleBlocks, &LvlScene::historyUpdateVisibleBGO, &LvlScene::historyUpdateVisibleNPC, &LvlScene::historyUpdateVisibleWater, &LvlScene::historyUpdateVisibleDoor, 0, false, false, false, false, false, true);

        //just in case
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_RENAMEEVENT:
    {
        int array_id = lastOperation.extraData.toList()[0].toInt();
        QString oldName = lastOperation.extraData.toList()[1].toString();
        QString newName = lastOperation.extraData.toList()[2].toString();

        for(int i = 0; i < LvlData->events.size(); i++){
            if(LvlData->events[i].array_id == (unsigned int)array_id){
                LvlData->events[i].name = oldName;
            }
        }

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        MainWinConnect::pMainWin->ModifyEvent(newName, oldName);
        MainWinConnect::pMainWin->setEventsBox();
        MainWinConnect::pMainWin->setEventToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_RENAMELAYER:
    {
        int array_id = lastOperation.extraData.toList()[0].toInt();
        QString oldName = lastOperation.extraData.toList()[1].toString();
        QString newName = lastOperation.extraData.toList()[2].toString();

        for(int i = 0; i < LvlData->layers.size(); i++){
            if(LvlData->layers[i].array_id == (unsigned int)array_id){
                LvlData->layers[i].name = oldName;
            }
        }

        //just in case
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->ModifyLayer(newName, oldName);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_REMOVELAYERANDSAVE:
    {
        LvlData->layers.push_back(lastOperation.data.layers[0]);
        LevelData mvData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(mvData, &lastOperation, cbData, &LvlScene::historyUndoChangeLayerBlocks, &LvlScene::historyUndoChangeLayerBGO, &LvlScene::historyUndoChangeLayerNPC, &LvlScene::historyUndoChangeLayerWater, &LvlScene::historyUndoChangeLayerDoor, 0, false, false, false, false, false, true);

        //just in case
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);

        break;
    }
    case HistoryOperation::LEVELHISTORY_MERGELAYER:
    {
        LvlData->layers.push_back(lastOperation.data.layers[0]);
        LevelData mvData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(mvData, &lastOperation, cbData, &LvlScene::historyUndoChangeLayerBlocks, &LvlScene::historyUndoChangeLayerBGO, &LvlScene::historyUndoChangeLayerNPC, &LvlScene::historyUndoChangeLayerWater, &LvlScene::historyUndoChangeLayerDoor, 0, false, false, false, false, false, true);

        //just in case
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSSECTION:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        int sectionID = lastOperation.extraData.toList()[0].toInt();
        QVariant extraData = lastOperation.extraData.toList()[1];

        if(subtype == SETTING_SECISWARP){
            LvlData->sections[sectionID].IsWarp = !extraData.toBool();
        }
        else
        if(subtype == SETTING_SECOFFSCREENEXIT){
            LvlData->sections[sectionID].OffScreenEn = !extraData.toBool();
        }
        else
        if(subtype == SETTING_SECNOBACK){
            LvlData->sections[sectionID].noback = !extraData.toBool();
        }
        else
        if(subtype == SETTING_SECUNDERWATER){
            LvlData->sections[sectionID].underwater = !extraData.toBool();
        }
        else
        if(subtype == SETTING_SECBACKGROUNDIMG){
            ChangeSectionBG(extraData.toList()[0].toInt(), sectionID);
        }
        else
        if(subtype == SETTING_SECMUSIC){
            LvlData->sections[sectionID].music_id = extraData.toList()[0].toInt();
        }
        else
        if(subtype == SETTING_SECCUSTOMMUSIC){
            LvlData->sections[sectionID].music_file = extraData.toList()[0].toString();
        }

        MainWinConnect::pMainWin->dock_LvlSectionProps->setLevelSectionData();
        LvlMusPlay::updateMusic();
        MainWinConnect::pMainWin->setMusic(LvlMusPlay::musicButtonChecked);
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSLEVEL:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        QVariant extraData = lastOperation.extraData;

        if(subtype == SETTING_LEVELNAME){
            LvlData->LevelName = extraData.toList()[0].toString();

            _edit->setWindowTitle( LvlData->LevelName.isEmpty() ?
                _edit->userFriendlyCurrentFile() : LvlData->LevelName );
            MainWinConnect::pMainWin->updateWindowMenu();
        }
        break;
    }
    case HistoryOperation::LEVELHISTORY_REPLACEPLAYERPOINT:
    {
        //revert place
        LevelData placeData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(placeData, &lastOperation, cbData, 0, 0, 0, 0, 0, &LvlScene::historyRemovePlayerPoint, true, true, true, true, true);

        break;
    }
    case HistoryOperation::LEVELHISTORY_RESIZEWATER:
    {
        LevelData resizedWater = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(resizedWater, &lastOperation, cbData, 0, 0, 0, &LvlScene::historyUndoResizeWater, 0, 0, true, true, true, false, true, true);
        break;
    }
    case HistoryOperation::LEVELHISTORY_ROTATE:
    {
        LevelData rotatedData = lastOperation.data;
        rotateGroup(findGraphicsItems(rotatedData, static_cast<ItemTypes::itemTypesMultiSelectable>(
                                      ItemTypes::LVL_S_Block |
                                      ItemTypes::LVL_S_BGO |
                                      ItemTypes::LVL_S_NPC |
                                      ItemTypes::LVL_S_Door |
                                      ItemTypes::LVL_S_PhysEnv |
                                      ItemTypes::LVL_S_Player)), !lastOperation.extraData.toBool(), false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_FLIP:
    {
        LevelData flippedData = lastOperation.data;
        flipGroup(findGraphicsItems(flippedData, static_cast<ItemTypes::itemTypesMultiSelectable>(
                                        ItemTypes::LVL_S_Block |
                                        ItemTypes::LVL_S_BGO |
                                        ItemTypes::LVL_S_NPC |
                                        ItemTypes::LVL_S_Door |
                                        ItemTypes::LVL_S_PhysEnv |
                                        ItemTypes::LVL_S_Player)), !lastOperation.extraData.toBool(), false);
        break;
    }
    default:
        break;
    }
    LvlData->modified = true;

    Debugger_updateItemList();
    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Undone: %1").arg(getHistoryText(lastOperation)));
}

void LvlScene::historyForward()
{

    HistoryOperation lastOperation = operationList[historyIndex];
    if(!lastOperation.newElement.isNull()){
        lastOperation.newElement->redo();
        historyIndex++;

        LvlData->modified = true;
        Debugger_updateItemList();
        MainWinConnect::pMainWin->refreshHistoryButtons();
        MainWinConnect::pMainWin->showStatusMsg(tr("Redone: %1").arg(lastOperation.newElement->getHistoryName()));
        return;
    }

    switch( lastOperation.type )
    {
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

        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

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
        QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
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

        MainWinConnect::pMainWin->dock_LvlWarpProps->setWarpRemoveButtonEnabled(true);

        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
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

        QComboBox* warplist = MainWinConnect::pMainWin->dock_LvlWarpProps->getWarpList();
        for(int i = 0; i < warplist->count(); i++){
            if((unsigned int)warplist->itemData(i).toInt() == removedDoor.array_id){
                warplist->removeItem(i);
                break;
            }
        }

        if(warplist->count()<=0) MainWinConnect::pMainWin->dock_LvlWarpProps->setWarpRemoveButtonEnabled(false);

        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
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
            doorp[index].novehicles = extraData.toBool();
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

        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
        doorPointsSync(array_id);

        break;
    }
    case HistoryOperation::LEVELHISTORY_ADDEVENT:
    {
        int array_id = lastOperation.extraData.toList()[0].toInt();
        QString name = lastOperation.extraData.toList()[1].toString();

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        QListWidgetItem * item;
        item = new QListWidgetItem;
        item->setText(name);
        item->setFlags(Qt::ItemIsEditable);
        item->setFlags(item->flags() | Qt::ItemIsEnabled);
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
        item->setData(3,  QString::number(array_id) );
        QListWidget* evList = MainWinConnect::pMainWin->getEventList();
        LevelEvents NewEvent = FileFormats::dummyLvlEvent();
        NewEvent.name = item->text();
        NewEvent.array_id = array_id;


        LvlData->events.push_back(NewEvent);
        evList->addItem(item);

        LvlData->modified = true;

        MainWinConnect::pMainWin->EventListsSync();
        MainWinConnect::pMainWin->setEventToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_REMOVEEVENT:
    {
        LevelEvents rmEvents = lastOperation.data.events[0];

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        for (int i = 0; i < LvlData->events.size(); i++) {
            if(LvlData->events[i].array_id == (unsigned int)rmEvents.array_id){
                for(int j = 0; j < MainWinConnect::pMainWin->getEventList()->count(); j++){
                    if(MainWinConnect::pMainWin->getEventList()->item(j)->data(ITEM_BLOCK_IS_SIZABLE).toInt() == (int)rmEvents.array_id){
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
    case HistoryOperation::LEVELHISTORY_DULPICATEEVENT:
    {
        LevelEvents dupEvents = lastOperation.data.events[0];

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        QListWidgetItem * item;
        item = new QListWidgetItem;
        item->setText(dupEvents.name);
        item->setFlags(Qt::ItemIsEditable);
        item->setFlags(item->flags() | Qt::ItemIsEnabled);
        item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsSelectable);
        item->setData(3, QString::number(dupEvents.array_id) );
        QListWidget* evList = MainWinConnect::pMainWin->getEventList();
        LevelEvents NewEvent = dupEvents;

        LvlData->events.push_back(NewEvent);
        evList->addItem(item);

        LvlData->modified = true;

        MainWinConnect::pMainWin->EventListsSync();
        MainWinConnect::pMainWin->setEventToolsLocked(false);

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSEVENT:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        int array_id = lastOperation.extraData.toList()[0].toInt();
        int index = -1;
        QVariant extraData = lastOperation.extraData.toList()[1];
        LevelEvents * eventp;
        bool found = false;

        for(int i = 0; i < LvlData->events.size(); i++){
            if(LvlData->events[i].array_id == (unsigned int)array_id){
                found = true;
                eventp = LvlData->events.data();
                index = i;
                break;
            }
        }

        if(!found)
            break;

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        if(subtype == SETTING_EV_AUTOSTART){
            eventp[index].autostart = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_SMOKE){
            eventp[index].nosmoke = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_LSHOWADD){
            QString layer = extraData.toString();
            eventp[index].layers_show.push_back(layer);
        }
        else
        if(subtype == SETTING_EV_LHIDEADD){
            QString layer = extraData.toString();
            eventp[index].layers_hide.push_back(layer);
        }
        else
        if(subtype == SETTING_EV_LTOGADD){
            QString layer = extraData.toString();
            eventp[index].layers_toggle.push_back(layer);
        }
        else
        if(subtype == SETTING_EV_LSHOWDEL){
            QString layer = extraData.toString();
            if(!eventp[index].layers_show.isEmpty()){
                for(int i = 0; i < eventp[index].layers_show.size(); i++){
                    if(eventp[index].layers_show[i] == layer){
                        eventp[index].layers_show.removeAt(i);
                    }
                }
            }
        }
        else
        if(subtype == SETTING_EV_LHIDEDEL){
            QString layer = extraData.toString();
            if(!eventp[index].layers_hide.isEmpty()){
                for(int i = 0; i < eventp[index].layers_hide.size(); i++){
                    if(eventp[index].layers_hide[i] == layer){
                        eventp[index].layers_hide.removeAt(i);
                    }
                }
            }
        }
        else
        if(subtype == SETTING_EV_LTOGDEL){
            QString layer = extraData.toString();
            if(!eventp[index].layers_toggle.isEmpty()){
                for(int i = 0; i < eventp[index].layers_toggle.size(); i++){
                    if(eventp[index].layers_toggle[i] == layer){
                        eventp[index].layers_toggle.removeAt(i);
                    }
                }
            }
        }
        else
        if(subtype == SETTING_EV_MOVELAYER){
            QString layer = extraData.toList()[1].toString();
            eventp[index].movelayer = layer;
        }
        else
        if(subtype == SETTING_EV_SPEEDLAYERX){
            eventp[index].layer_speed_x = extraData.toList()[1].toDouble();
        }
        else
        if(subtype == SETTING_EV_SPEEDLAYERY){
            eventp[index].layer_speed_y = extraData.toList()[1].toDouble();
        }
        else
        if(subtype == SETTING_EV_AUTOSCRSEC){
            eventp[index].scroll_section = (long)extraData.toList()[1].toLongLong();
        }
        else
        if(subtype == SETTING_EV_AUTOSCRX){
            eventp[index].move_camera_x = extraData.toList()[1].toDouble();
        }
        else
        if(subtype == SETTING_EV_AUTOSCRY){
            eventp[index].move_camera_y = extraData.toList()[1].toDouble();
        }
        else
        if(subtype == SETTING_EV_SECSIZE){
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].position_top = (long)extraData.toList()[5].toLongLong();
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].position_right = (long)extraData.toList()[6].toLongLong();
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].position_bottom = (long)extraData.toList()[7].toLongLong();
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].position_left = (long)extraData.toList()[8].toLongLong();
        }
        else
        if(subtype == SETTING_EV_SECMUS){
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].music_id = (long)extraData.toList()[2].toLongLong();
        }
        else
        if(subtype == SETTING_EV_SECBG){
            eventp[index].sets[(int)extraData.toList()[0].toLongLong()].background_id = (long)extraData.toList()[2].toLongLong();
        }
        else
        if(subtype == SETTING_EV_MSG){
            eventp[index].msg = extraData.toList()[1].toString();
        }
        else
        if(subtype == SETTING_EV_SOUND){
            eventp[index].sound_id = (long)extraData.toList()[1].toLongLong();
        }
        else
        if(subtype == SETTING_EV_ENDGAME){
            eventp[index].end_game = (long)extraData.toList()[1].toLongLong();
        }
        else
        if(subtype == SETTING_EV_KUP){
            eventp[index].ctrl_up = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KDOWN){
            eventp[index].ctrl_down = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KLEFT){
            eventp[index].ctrl_left = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KRIGHT){
            eventp[index].ctrl_right = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KRUN){
            eventp[index].ctrl_run = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KALTRUN){
            eventp[index].ctrl_altrun = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KJUMP){
            eventp[index].ctrl_jump = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KALTJUMP){
            eventp[index].ctrl_altjump = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KDROP){
            eventp[index].ctrl_drop = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_KSTART){
            eventp[index].ctrl_start = extraData.toBool();
        }
        else
        if(subtype == SETTING_EV_TRIACTIVATE){
            eventp[index].trigger = extraData.toList()[1].toString();
        }
        else
        if(subtype == SETTING_EV_TRIDELAY){
            eventp[index].trigger_timer = (long)extraData.toList()[1].toLongLong();
        }


        MainWinConnect::pMainWin->setEventData(-2);
        MainWinConnect::pMainWin->setEventToolsLocked(false);

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDNEWLAYER:
    {
        LevelData modifiedSourceData = lastOperation.data;

        LvlData->layers.push_back(modifiedSourceData.layers[0]);

        CallbackData cbData;
        findGraphicsItem(modifiedSourceData, &lastOperation, cbData, &LvlScene::historyRedoChangeLayerBlocks, &LvlScene::historyRedoChangeLayerBGO, &LvlScene::historyRedoChangeLayerNPC, &LvlScene::historyRedoChangeLayerWater, &LvlScene::historyRedoChangeLayerDoor, 0, false, false, false, false, false, true);

        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_ADDLAYER:
    {
        LevelLayers l;
        l.array_id = lastOperation.extraData.toList()[0].toInt();
        l.name = lastOperation.extraData.toList()[1].toString();
        l.hidden = false;
        LvlData->layers.push_back(l);
        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_REMOVELAYER:
    {
        LevelData deletedData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(deletedData, &lastOperation, cbData, &LvlScene::historyRemoveBlocks, &LvlScene::historyRemoveBGO, &LvlScene::historyRemoveNPC, &LvlScene::historyRemoveWater, &LvlScene::historyRemoveDoors, &LvlScene::historyRemovePlayerPoint);

        for(int i = 0; i < LvlData->layers.size(); i++){
            if(LvlData->layers[i].array_id == lastOperation.data.layers[0].array_id){
                LvlData->layers.removeAt(i);
            }
        }

        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_RENAMEEVENT:
    {
        int array_id = lastOperation.extraData.toList()[0].toInt();
        QString oldName = lastOperation.extraData.toList()[1].toString();
        QString newName = lastOperation.extraData.toList()[2].toString();

        for(int i = 0; i < LvlData->events.size(); i++){
            if(LvlData->events[i].array_id == (unsigned int)array_id){
                LvlData->events[i].name = newName;
            }
        }

        MainWinConnect::pMainWin->setEventToolsLocked(true);
        MainWinConnect::pMainWin->ModifyEvent(oldName, newName);
        MainWinConnect::pMainWin->setEventsBox();
        MainWinConnect::pMainWin->setEventToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_RENAMELAYER:
    {
        int array_id = lastOperation.extraData.toList()[0].toInt();
        QString oldName = lastOperation.extraData.toList()[1].toString();
        QString newName = lastOperation.extraData.toList()[2].toString();

        for(int i = 0; i < LvlData->layers.size(); i++){
            if(LvlData->layers[i].array_id == (unsigned int)array_id){
                LvlData->layers[i].name = newName;
            }
        }

        //just in case
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->ModifyLayer(oldName, newName);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_REMOVELAYERANDSAVE:
    {
        LevelData mvData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(mvData, &lastOperation, cbData, &LvlScene::historyRedoChangeLayerBlocks, &LvlScene::historyRedoChangeLayerBGO, &LvlScene::historyRedoChangeLayerNPC, &LvlScene::historyRedoChangeLayerWater, &LvlScene::historyRedoChangeLayerDoor, 0, false, false, false, false, false, true);

        for(int i = 0; i < LvlData->layers.size(); i++){
            if(LvlData->layers[i].array_id == lastOperation.data.layers[0].array_id){
                LvlData->layers.removeAt(i);
            }
        }
        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_MERGELAYER:
    {
        LevelData mergeData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(mergeData, &lastOperation, cbData, &LvlScene::historyRedoChangeLayerBlocks, &LvlScene::historyRedoChangeLayerBGO, &LvlScene::historyRedoChangeLayerNPC, &LvlScene::historyRedoChangeLayerWater, &LvlScene::historyRedoChangeLayerDoor, 0, false, false, false, false, false, true);

        for(int i = 0; i < LvlData->layers.size(); i++){
            if(LvlData->layers[i].array_id == lastOperation.data.layers[0].array_id){
                LvlData->layers.removeAt(i);
            }
        }
        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSSECTION:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        int sectionID = lastOperation.extraData.toList()[0].toInt();
        QVariant extraData = lastOperation.extraData.toList()[1];

        if(subtype == SETTING_SECISWARP){
            LvlData->sections[sectionID].IsWarp = extraData.toBool();
        }
        else
        if(subtype == SETTING_SECOFFSCREENEXIT){
            LvlData->sections[sectionID].OffScreenEn = extraData.toBool();
        }
        else
        if(subtype == SETTING_SECNOBACK){
            LvlData->sections[sectionID].noback = extraData.toBool();
        }
        else
        if(subtype == SETTING_SECUNDERWATER){
            LvlData->sections[sectionID].underwater = extraData.toBool();
        }
        else
        if(subtype == SETTING_SECBACKGROUNDIMG){
            ChangeSectionBG(extraData.toList()[1].toInt(), sectionID);
        }
        else
        if(subtype == SETTING_SECMUSIC){
            LvlData->sections[sectionID].music_id = extraData.toList()[1].toInt();
        }
        else
        if(subtype == SETTING_SECCUSTOMMUSIC){
            LvlData->sections[sectionID].music_file = extraData.toList()[1].toString();
        }
        MainWinConnect::pMainWin->dock_LvlSectionProps->setLevelSectionData();
        LvlMusPlay::updateMusic();
        MainWinConnect::pMainWin->setMusic(LvlMusPlay::musicButtonChecked);

        break;
    }
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSLEVEL:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        QVariant extraData = lastOperation.extraData;

        if(subtype == SETTING_LEVELNAME){
            LvlData->LevelName = extraData.toList()[1].toString();
            _edit->setWindowTitle( LvlData->LevelName.isEmpty() ?
                _edit->userFriendlyCurrentFile() : LvlData->LevelName );
            MainWinConnect::pMainWin->updateWindowMenu();
        }

        break;
    }
    case HistoryOperation::LEVELHISTORY_REPLACEPLAYERPOINT:
    {
        LevelData placedData = lastOperation.data;
        //revert place
        foreach(PlayerPoint plr, placedData.players)
        {
            //for(int i = 0; i < LvlData->players.size(); i++){
            //    if(LvlData->players[i].id == plr.id){
            //        LvlData->players[i] = plr;
            //    }
            //}
            placePlayerPoint(plr);
        }

        break;
    }
    case HistoryOperation::LEVELHISTORY_RESIZEWATER:
    {
        LevelData resizedWater = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(resizedWater, &lastOperation, cbData, 0, 0, 0, &LvlScene::historyRedoResizeWater, 0, 0, true, true, true, false, true, true);
        break;
    }
    case HistoryOperation::LEVELHISTORY_ROTATE:
    {
        LevelData rotatedData = lastOperation.data;
        rotateGroup(findGraphicsItems(rotatedData, static_cast<ItemTypes::itemTypesMultiSelectable>(
                                      ItemTypes::LVL_S_Block |
                                      ItemTypes::LVL_S_BGO |
                                      ItemTypes::LVL_S_NPC |
                                      ItemTypes::LVL_S_Door |
                                      ItemTypes::LVL_S_PhysEnv |
                                      ItemTypes::LVL_S_Player)), lastOperation.extraData.toBool(), false);
        break;
    }
    case HistoryOperation::LEVELHISTORY_FLIP:
    {
        LevelData flippedData = lastOperation.data;
        flipGroup(findGraphicsItems(flippedData, static_cast<ItemTypes::itemTypesMultiSelectable>(
                                        ItemTypes::LVL_S_Block |
                                        ItemTypes::LVL_S_BGO |
                                        ItemTypes::LVL_S_NPC |
                                        ItemTypes::LVL_S_Door |
                                        ItemTypes::LVL_S_PhysEnv |
                                        ItemTypes::LVL_S_Player)), lastOperation.extraData.toBool(), false);
        break;
    }
    default:
        break;

    }
    historyIndex++;

    Debugger_updateItemList();
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

void LvlScene::historyRemoveBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    ((ItemBlock*)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void LvlScene::historyRemoveBGO(LvlScene::CallbackData cbData, LevelBGO /*data*/)
{
    ((ItemBGO *)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void LvlScene::historyRemoveNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    ((ItemNPC *)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void LvlScene::historyRemoveWater(LvlScene::CallbackData cbData, LevelPhysEnv /*data*/)
{
    ((ItemWater *)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void LvlScene::historyRemovePlayerPoint(LvlScene::CallbackData cbData, PlayerPoint /*data*/)
{
    dynamic_cast<ItemPlayerPoint *>(cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete (cbData.item);

    bool wasPlaced = false;
    PlayerPoint oPoint;
    if(!cbData.hist->extraData.isNull())
    {
        if(cbData.hist->extraData.type() == QVariant::List)
        {
            QList<QVariant> mData = cbData.hist->extraData.toList();
            if(mData.size() == 5)
            {
                oPoint.id = (unsigned int)mData[0].toInt();
                oPoint.x = (long)mData[1].toLongLong();
                oPoint.y = (long)mData[2].toLongLong();
                oPoint.w = (long)mData[3].toLongLong();
                oPoint.h = (long)mData[4].toLongLong();
                if(oPoint.id>0) wasPlaced = true;
            }
        }
    }

    if(wasPlaced) placePlayerPoint(oPoint);
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

void LvlScene::historyUndoChangeLayerWater(LvlScene::CallbackData cbData, LevelPhysEnv data)
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

void LvlScene::historyUndoChangeLayerDoor(LvlScene::CallbackData cbData, LevelDoors data, bool /*isEntrance*/)
{
    ItemDoor* targetItem = (ItemDoor*)cbData.item;
    QString oldLayer = data.layer;
    targetItem->doorData.layer = oldLayer;
    foreach (LevelLayers lr, LvlData->layers) {
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

void LvlScene::historyRedoChangeLayerWater(LvlScene::CallbackData cbData, LevelPhysEnv /*data*/)
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

void LvlScene::historyRedoChangeLayerDoor(LvlScene::CallbackData cbData, LevelDoors /*data*/, bool /*isEntrance*/)
{
    ItemDoor* targetItem = (ItemDoor*)cbData.item;
    QString newLayer = cbData.hist->extraData.toString();
    targetItem->doorData.layer = newLayer;
    foreach (LevelLayers lr, LvlData->layers) {
        if(lr.name == newLayer)
        {
            targetItem->setVisible(!lr.hidden);
        }
    }
    targetItem->arrayApply();
}

void LvlScene::historyUndoResizeWater(LvlScene::CallbackData cbData, LevelPhysEnv /*data*/)
{
    QList<QVariant> package = cbData.hist->extraData.toList();
    QList<QVariant> oldSizes = package[0].toList();
    long tarLeft = (long)oldSizes[0].toLongLong();
    long tarTop = (long)oldSizes[1].toLongLong();
    long tarRight = (long)oldSizes[2].toLongLong();
    long tarBottom = (long)oldSizes[3].toLongLong();
    ((ItemWater *)cbData.item)->setRectSize(QRect(tarLeft, tarTop, tarRight-tarLeft, tarBottom-tarTop));
}

void LvlScene::historyRedoResizeWater(LvlScene::CallbackData cbData, LevelPhysEnv /*data*/)
{
    QList<QVariant> package = cbData.hist->extraData.toList();
    QList<QVariant> oldSizes = package[1].toList();
    long tarLeft = (long)oldSizes[0].toLongLong();
    long tarTop = (long)oldSizes[1].toLongLong();
    long tarRight = (long)oldSizes[2].toLongLong();
    long tarBottom = (long)oldSizes[3].toLongLong();
    ((ItemWater *)cbData.item)->setRectSize(QRect(tarLeft, tarTop, tarRight-tarLeft, tarBottom-tarTop));
}

void LvlScene::historyRemoveDoors(LvlScene::CallbackData cbData, LevelDoors /*door*/, bool /*isEntrance*/)
{
    ((ItemDoor *)(cbData.item))->removeFromArray();
    if((cbData.item)) delete (cbData.item);
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
}

void LvlScene::historyUpdateVisibleBlocks(LvlScene::CallbackData cbData, LevelBlock /*data*/)
{
    cbData.item->setVisible(!cbData.hist->data.layers[0].hidden);
}

void LvlScene::historyUpdateVisibleBGO(LvlScene::CallbackData cbData, LevelBGO /*data*/)
{
    cbData.item->setVisible(!cbData.hist->data.layers[0].hidden);
}

void LvlScene::historyUpdateVisibleNPC(LvlScene::CallbackData cbData, LevelNPC /*data*/)
{
    cbData.item->setVisible(!cbData.hist->data.layers[0].hidden);
}

void LvlScene::historyUpdateVisibleWater(LvlScene::CallbackData cbData, LevelPhysEnv /*data*/)
{
    cbData.item->setVisible(!cbData.hist->data.layers[0].hidden);
}

void LvlScene::historyUpdateVisibleDoor(LvlScene::CallbackData cbData, LevelDoors /*data*/, bool /*isEntrance*/)
{
    cbData.item->setVisible(!cbData.hist->data.layers[0].hidden);
}

void LvlScene::findGraphicsItem(LevelData toFind,
                                HistoryOperation * operation,
                                CallbackData customData,
                                callBackLevelBlock clbBlock,
                                callBackLevelBGO clbBgo,
                                callBackLevelNPC clbNpc,
                                callBackLevelWater clbWater,
                                callBackLevelDoors clbDoor,
                                callBackLevelPlayerPoints clbPlayer,
                                bool ignoreBlock,
                                bool ignoreBGO,
                                bool ignoreNPC,
                                bool ignoreWater,
                                bool ignoreDoors,
                                bool ignorePlayer)
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
    QMap<int, LevelPhysEnv> sortedWater;
    if(!ignoreWater)
    {
        foreach (LevelPhysEnv water, toFind.physez) {
            sortedWater[water.array_id] = water;
        }
    }
    QMap<int, PlayerPoint> sortedPlayers;
    if(!ignorePlayer){
        foreach (PlayerPoint player, toFind.players) {
            sortedPlayers[player.id] = player;
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
    QMap<int, QGraphicsItem*> sortedGraphPlayers;
    foreach (QGraphicsItem* unsortedItem, items())
    {
        if(unsortedItem->data(ITEM_TYPE).toString()=="Block")
        {
            if(!ignoreBlock){
                sortedGraphBlocks[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="BGO")
        {
            if(!ignoreBGO){
                sortedGraphBGO[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="NPC")
        {
            if(!ignoreNPC){
                sortedGraphNPC[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Water")
        {
            if(!ignoreWater){
                sortedGraphWater[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Door_enter")
        {
            if(!ignoreDoors){
                sortedGraphDoorEntrance[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }

        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Door_exit")
        {
            if(!ignoreDoors){
                sortedGraphDoorExit[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="playerPoint")
        {
            if(!ignorePlayer){
                sortedGraphPlayers[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
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
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedBlock.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedBlock.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
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
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedBGO.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedBGO.begin();
                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
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
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedNPC.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedNPC.begin();
				
                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
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
                QMap<int, LevelPhysEnv>::iterator beginItem = sortedWater.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedWater.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedWater.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
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
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedEntranceDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedEntranceDoors.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
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
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedExitDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedExitDoors.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbDoor)(cbData,(*beginItem),false);
                    sortedExitDoors.erase(beginItem);
                }
            }
        }
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2); //update Door data
    }

    if(!ignorePlayer)
    {
        foreach (QGraphicsItem* item, sortedGraphPlayers)
        {
            if(sortedPlayers.size()!=0)
            {
                QMap<int, PlayerPoint>::iterator beginItem = sortedPlayers.begin();
                unsigned int currentArrayId = (*beginItem).id;

                //but still test if the next blocks, is the block we search!
                beginItem = sortedPlayers.begin();

                currentArrayId = (*beginItem).id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbPlayer)(cbData,(*beginItem));
                    sortedPlayers.erase(beginItem);
                }
            }
        }
    }

}

QList<QGraphicsItem *> LvlScene::findGraphicsItems(LevelData& toFind, ItemTypes::itemTypesMultiSelectable findingFilter)
{
    QMap<int, LevelDoors> sortedEntranceDoors;
    QMap<int, LevelDoors> sortedExitDoors;
    if(findingFilter & ItemTypes::LVL_S_Door){
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
    if(findingFilter & ItemTypes::LVL_S_Block){
        foreach (LevelBlock block, toFind.blocks)
        {
            sortedBlock[block.array_id] = block;
        }
    }
    QMap<int, LevelBGO> sortedBGO;
    if(findingFilter & ItemTypes::LVL_S_BGO){
        foreach (LevelBGO bgo, toFind.bgo)
        {
            sortedBGO[bgo.array_id] = bgo;
        }
    }
    QMap<int, LevelNPC> sortedNPC;
    if(findingFilter & ItemTypes::LVL_S_NPC){
        foreach (LevelNPC npc, toFind.npc)
        {
            sortedNPC[npc.array_id] = npc;
        }
    }
    QMap<int, LevelPhysEnv> sortedWater;
    if(findingFilter & ItemTypes::LVL_S_PhysEnv)
    {
        foreach (LevelPhysEnv water, toFind.physez) {
            sortedWater[water.array_id] = water;
        }
    }
    QMap<int, PlayerPoint> sortedPlayers;
    if(findingFilter & ItemTypes::LVL_S_Player){
        foreach (PlayerPoint player, toFind.players) {
            sortedPlayers[player.id] = player;
        }
    }

    QMap<int, QGraphicsItem*> sortedGraphBlocks;
    QMap<int, QGraphicsItem*> sortedGraphBGO;
    QMap<int, QGraphicsItem*> sortedGraphNPC;
    QMap<int, QGraphicsItem*> sortedGraphWater;
    QMap<int, QGraphicsItem*> sortedGraphDoorEntrance;
    QMap<int, QGraphicsItem*> sortedGraphDoorExit;
    QMap<int, QGraphicsItem*> sortedGraphPlayers;
    foreach (QGraphicsItem* unsortedItem, items())
    {
        if(unsortedItem->data(ITEM_TYPE).toString()=="Block")
        {
            if(findingFilter & ItemTypes::LVL_S_Block){
                sortedGraphBlocks[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="BGO")
        {
            if(findingFilter & ItemTypes::LVL_S_BGO){
                sortedGraphBGO[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="NPC")
        {
            if(findingFilter & ItemTypes::LVL_S_NPC){
                sortedGraphNPC[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Water")
        {
            if(findingFilter & ItemTypes::LVL_S_PhysEnv){
                sortedGraphWater[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Door_enter")
        {
            if(findingFilter & ItemTypes::LVL_S_Door){
                sortedGraphDoorEntrance[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }

        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Door_exit")
        {
            if(findingFilter & ItemTypes::LVL_S_Door){
                sortedGraphDoorExit[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="playerPoint")
        {
            if(findingFilter & ItemTypes::LVL_S_Player){
                sortedGraphPlayers[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
    }

    QList<QGraphicsItem*> returnItems;

    if(findingFilter & ItemTypes::LVL_S_Block){
        foreach (QGraphicsItem* item, sortedGraphBlocks)
        {

            if(sortedBlock.size()!=0)
            {
                QMap<int, LevelBlock>::iterator beginItem = sortedBlock.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedBlock.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedBlock.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    returnItems << item;
                }
            }
            else
            {
                break;
            }
        }
    }

    if(findingFilter & ItemTypes::LVL_S_BGO)
    {
        foreach (QGraphicsItem* item, sortedGraphBGO)
        {
            if(sortedBGO.size()!=0)
            {
                QMap<int, LevelBGO>::iterator beginItem = sortedBGO.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedBGO.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedBGO.begin();
                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    returnItems << item;
                }
            }
            else
            {
                break;
            }
        }
    }

    if(findingFilter & ItemTypes::LVL_S_NPC)
    {
        foreach (QGraphicsItem* item, sortedGraphNPC)
        {
            if(sortedNPC.size()!=0)
            {
                QMap<int, LevelNPC>::iterator beginItem = sortedNPC.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedNPC.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedNPC.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    returnItems << item;
                }
            }
        }
    }

    if(findingFilter & ItemTypes::LVL_S_PhysEnv)
    {
        foreach (QGraphicsItem* item, sortedGraphWater)
        {
            if(sortedWater.size()!=0)
            {
                QMap<int, LevelPhysEnv>::iterator beginItem = sortedWater.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedWater.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedWater.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    returnItems << item;
                }
            }
        }
    }

    if(findingFilter & ItemTypes::LVL_S_Door)
    {
        foreach (QGraphicsItem* item, sortedGraphDoorEntrance)
        {
            if(sortedEntranceDoors.size()!=0)
            {
                QMap<int, LevelDoors>::iterator beginItem = sortedEntranceDoors.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedEntranceDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedEntranceDoors.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    returnItems << item;
                }
            }
        }
        foreach (QGraphicsItem* item, sortedGraphDoorExit)
        {
            if(sortedExitDoors.size()!=0)
            {
                QMap<int, LevelDoors>::iterator beginItem = sortedExitDoors.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedExitDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedExitDoors.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    returnItems << item;
                }
            }
        }
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2); //update Door data
    }

    if(findingFilter & ItemTypes::LVL_S_Player)
    {
        foreach (QGraphicsItem* item, sortedGraphPlayers)
        {
            if(sortedPlayers.size()!=0)
            {
                QMap<int, PlayerPoint>::iterator beginItem = sortedPlayers.begin();
                unsigned int currentArrayId = (*beginItem).id;

                //but still test if the next blocks, is the block we search!
                beginItem = sortedPlayers.begin();

                currentArrayId = (*beginItem).id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    returnItems << item;
                }
            }
        }
    }
    return returnItems;
}

void LvlScene::findGraphicsDoor(int array_id, LvlScene::HistoryOperation *operation, LvlScene::CallbackData customData, LvlScene::callBackLevelDoors clbDoors, bool isEntrance)
{
    CallbackData cbData = customData;
    cbData.hist = operation;
    foreach (QGraphicsItem* i, items())
    {
        if(i->data(ITEM_ARRAY_ID).toInt() == array_id){
            if(isEntrance && i->data(ITEM_TYPE).toString()=="Door_enter"){
                //(this->*clbDoors)(CallbackData, LevelDoors, bool)
                //find DoorData
                foreach(LevelDoors findDoor, LvlData->doors){
                    if(array_id == (int)findDoor.array_id){
                        cbData.item = i;
                        (this->*clbDoors)(cbData, findDoor, true);
                        break;
                    }
                }
            }else if(!isEntrance && i->data(ITEM_TYPE).toString()=="Door_exit"){
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

QString LvlScene::getHistoryText(LvlScene::HistoryOperation operation)
{
    switch (operation.type) {
    case HistoryOperation::LEVELHISTORY_REMOVE: return tr("Remove");
    case HistoryOperation::LEVELHISTORY_PLACE: return tr("Place");
    case HistoryOperation::LEVELHISTORY_OVERWRITE: return tr("Place & Overwrite");
    case HistoryOperation::LEVELHISTORY_MOVE: return tr("Move");
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGS: return tr("Changed Setting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    case HistoryOperation::LEVELHISTORY_CHANGEDLAYER: return tr("Change Layer");
    case HistoryOperation::LEVELHISTORY_RESIZEBLOCK: return tr("Resize Block");
    case HistoryOperation::LEVELHISTORY_PLACEDOOR: return tr("Place Door");
    case HistoryOperation::LEVELHISTORY_ADDWARP: return tr("Add Warp");
    case HistoryOperation::LEVELHISTORY_REMOVEWARP: return tr("Remove Warp");
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSWARP: return tr("Changed Warpsetting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    case HistoryOperation::LEVELHISTORY_ADDEVENT: return tr("Add Event");
    case HistoryOperation::LEVELHISTORY_REMOVEEVENT: return tr("Remove Event");
    case HistoryOperation::LEVELHISTORY_DULPICATEEVENT: return tr("Copy Event");
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSEVENT: return tr("Changed Eventsetting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    case HistoryOperation::LEVELHISTORY_CHANGEDNEWLAYER: return tr("Move Items to new Layer");
    case HistoryOperation::LEVELHISTORY_ADDLAYER: return tr("New Layer");
    case HistoryOperation::LEVELHISTORY_REMOVELAYER: return tr("Remove layer with items");
    case HistoryOperation::LEVELHISTORY_RENAMEEVENT: return tr("Rename Event");
    case HistoryOperation::LEVELHISTORY_RENAMELAYER: return tr("Rename Layer");
    case HistoryOperation::LEVELHISTORY_REMOVELAYERANDSAVE: return tr("Remove layers and save items");
    case HistoryOperation::LEVELHISTORY_MERGELAYER: return tr("Merge Layer");
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSSECTION: return tr("Changed Sectionsetting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    case HistoryOperation::LEVELHISTORY_CHANGEDSETTINGSLEVEL: return tr("Changed Levelsetting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    case HistoryOperation::LEVELHISTORY_REPLACEPLAYERPOINT: return tr("Place Player Point");
    case HistoryOperation::LEVELHISTORY_RESIZEWATER: return tr("Resize Water");
    case HistoryOperation::LEVELHISTORY_ROTATE: return tr("Rotate");
    case HistoryOperation::LEVELHISTORY_FLIP: return tr("Flip");
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
    case SETTING_LAYER: return tr("Layer");
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
    case SETTING_EV_AUTOSTART: return tr("Autostart");
    case SETTING_EV_SMOKE: return tr("Layer Smoke Effect");
    case SETTING_EV_LHIDEADD: return tr("Add Hide Layer");
    case SETTING_EV_LHIDEDEL: return tr("Remove Hide Layer");
    case SETTING_EV_LSHOWADD: return tr("Add Show Layer");
    case SETTING_EV_LSHOWDEL: return tr("Remove Show Layer");
    case SETTING_EV_LTOGADD: return tr("Add Toggle Layer");
    case SETTING_EV_LTOGDEL: return tr("Remove Toggle Layer");
    case SETTING_EV_MOVELAYER: return tr("Moving Layer");
    case SETTING_EV_SPEEDLAYERX: return tr("Layer Speed Horizontal");
    case SETTING_EV_SPEEDLAYERY: return tr("Layer Speed Vertical");
    case SETTING_EV_AUTOSCRSEC: return tr("Autoscroll Layer");
    case SETTING_EV_AUTOSCRX: return tr("Autoscroll Layer Speed Horizontal");
    case SETTING_EV_AUTOSCRY: return tr("Autoscroll Layer Speed Vertical");
    case SETTING_EV_SECSIZE: return tr("Section Size");
    case SETTING_EV_SECMUS: return tr("Section Music");
    case SETTING_EV_SECBG: return tr("Section Background");
    case SETTING_EV_MSG: return tr("Message");
    case SETTING_EV_SOUND: return tr("Sound");
    case SETTING_EV_ENDGAME: return tr("End Game");
    case SETTING_EV_KUP: return tr("Up Key Activate");
    case SETTING_EV_KDOWN: return tr("Down Key Activate");
    case SETTING_EV_KLEFT: return tr("Left Key Activate");
    case SETTING_EV_KRIGHT: return tr("Right Key Activate");
    case SETTING_EV_KRUN: return tr("Run Key Activate");
    case SETTING_EV_KALTRUN: return tr("Alt Run Key Activate");
    case SETTING_EV_KJUMP: return tr("Jump Key Activate");
    case SETTING_EV_KALTJUMP: return tr("Alt Jump Key Activate");
    case SETTING_EV_KDROP: return tr("Drop Key Activate");
    case SETTING_EV_KSTART: return tr("Start Key Activate");
    case SETTING_EV_TRIACTIVATE: return tr("Trigger Activate");
    case SETTING_EV_TRIDELAY: return tr("Trigger Delay");
    case SETTING_SECISWARP: return tr("Is Warp");
    case SETTING_SECNOBACK: return tr("No Back");
    case SETTING_SECOFFSCREENEXIT: return tr("Off Screen Exit");
    case SETTING_SECUNDERWATER: return tr("Underwater");
    case SETTING_SECBACKGROUNDIMG: return tr("Background Image");
    case SETTING_SECMUSIC: return tr("Music");
    case SETTING_SECCUSTOMMUSIC: return tr("Custom Music");
    case SETTING_LEVELNAME: return tr("Level Name");
    case SETTING_BGOSORTING: return tr("BGO Sorting Priority");
    default:
        return tr("Unknown");
    }
}
