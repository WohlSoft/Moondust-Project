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
#include <PGE_File_Formats/file_formats.h>
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
#include <editing/_components/history/historyelementplacedoor.h>
#include <editing/_components/history/historyelementaddwarp.h>
#include <editing/_components/history/historyelementremovewarp.h>
#include <editing/_components/history/historyelementsettingswarp.h>
#include <editing/_components/history/historyelementmodifyevent.h>
#include <editing/_components/history/historyelementsettingsevent.h>
#include <editing/_components/history/historyelementchangednewlayer.h>
#include <editing/_components/history/historyelementaddlayer.h>
#include <editing/_components/history/historyelementremovelayer.h>
#include <editing/_components/history/historyelementrenameevent.h>
#include <editing/_components/history/historyelementrenamelayer.h>
#include <editing/_components/history/historyelementremovelayerandsave.h>
#include <editing/_components/history/historyelementmergelayer.h>
#include <editing/_components/history/historyelementsettingssection.h>
#include <editing/_components/history/historyelementreplaceplayerpoint.h>
#include <editing/_components/history/historyelementresizewater.h>

void LvlScene::addRemoveHistory(LevelData removedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(removedItems, LevelData());
    modf->setCustomHistoryName(tr("Remove"));
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addPlaceHistory(LevelData placedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(LevelData(), placedItems);
    modf->setCustomHistoryName(tr("Place"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addOverwriteHistory(LevelData removedItems, LevelData placedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(removedItems, placedItems);
    modf->setCustomHistoryName(tr("Place & Overwrite"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addPlaceDoorHistory(LevelDoors door, bool isEntrance)
{
    updateHistoryBuffer();

    HistoryElementPlaceDoor* modf = new HistoryElementPlaceDoor(door, isEntrance);
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addMoveHistory(LevelData sourceMovedItems, LevelData targetMovedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(sourceMovedItems, targetMovedItems);
    modf->setCustomHistoryName(tr("Move"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeSettingsHistory(LevelData modifiedItems, HistorySettings::LevelSettingSubType subType, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementItemSetting* modf = new HistoryElementItemSetting(modifiedItems, subType, extraData);
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addResizeSectionHistory(int sectionID, long oldLeft, long oldTop, long oldRight, long oldBottom,
                                       long newLeft, long newTop, long newRight, long newBottom)
{
    updateHistoryBuffer();
    HistoryElementResizeSection *modf = new HistoryElementResizeSection(sectionID,
                                                                        QRect(QPoint(oldLeft, oldTop), QPoint(oldRight, oldBottom)),
                                                                        QRect(QPoint(newLeft, newTop), QPoint(newRight, newBottom)));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangedLayerHistory(LevelData changedItems, QString newLayerName)
{
    updateHistoryBuffer();

    HistoryElementLayerChanged *modf = new HistoryElementLayerChanged(changedItems, newLayerName);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addResizeBlockHistory(LevelBlock bl, long oldLeft, long oldTop, long oldRight, long oldBottom, long newLeft, long newTop, long newRight, long newBottom)
{
    updateHistoryBuffer();

    HistoryElementResizeBlock *modf = new HistoryElementResizeBlock(bl,
                                                                        QRect(QPoint(oldLeft, oldTop), QPoint(oldRight, oldBottom)),
                                                                        QRect(QPoint(newLeft, newTop), QPoint(newRight, newBottom)));
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addResizeWaterHistory(LevelPhysEnv wt, long oldLeft, long oldTop, long oldRight, long oldBottom, long newLeft, long newTop, long newRight, long newBottom)
{
    updateHistoryBuffer();

    HistoryElementResizeWater *modf = new HistoryElementResizeWater(wt,
                                                                        QRect(QPoint(oldLeft, oldTop), QPoint(oldRight, oldBottom)),
                                                                        QRect(QPoint(newLeft, newTop), QPoint(newRight, newBottom)));
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addAddWarpHistory(int array_id, int listindex, int doorindex)
{
    updateHistoryBuffer();

    HistoryElementAddWarp* modf = new HistoryElementAddWarp(array_id, listindex, doorindex);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRemoveWarpHistory(LevelDoors removedDoor)
{
    updateHistoryBuffer();

    HistoryElementRemoveWarp* modf = new HistoryElementRemoveWarp(removedDoor);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeWarpSettingsHistory(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementSettingsWarp* modf = new HistoryElementSettingsWarp(array_id, subtype, extraData);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addAddEventHistory(LevelEvents ev)
{
    updateHistoryBuffer();

    HistoryElementModifyEvent* modf = new HistoryElementModifyEvent(ev, false);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRemoveEventHistory(LevelEvents ev)
{
    updateHistoryBuffer();

    HistoryElementModifyEvent* modf = new HistoryElementModifyEvent(ev, true);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addDuplicateEventHistory(LevelEvents newDuplicate)
{
    updateHistoryBuffer();

    HistoryElementModifyEvent* modf = new HistoryElementModifyEvent(newDuplicate, false);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeEventSettingsHistory(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementSettingsEvent* modf = new HistoryElementSettingsEvent(array_id, subtype, extraData);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangedNewLayerHistory(LevelData changedItems, LevelLayers newLayer)
{
    updateHistoryBuffer();

    HistoryElementChangedNewLayer* modf = new HistoryElementChangedNewLayer(changedItems, newLayer);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addAddLayerHistory(int array_id, QString name)
{
    updateHistoryBuffer();

    HistoryElementAddLayer* modf = new HistoryElementAddLayer(array_id, name);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRemoveLayerHistory(LevelData modData)
{
    updateHistoryBuffer();

    HistoryElementRemoveLayer* modf = new HistoryElementRemoveLayer(modData);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRenameEventHistory(int array_id, QString oldName, QString newName)
{
    updateHistoryBuffer();

    HistoryElementRenameEvent* modf = new HistoryElementRenameEvent(array_id, oldName, newName);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRenameLayerHistory(int array_id, QString oldName, QString newName)
{
    updateHistoryBuffer();

    HistoryElementRenameLayer* modf = new HistoryElementRenameLayer(array_id, oldName, newName);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRemoveLayerAndSaveItemsHistory(LevelData modData)
{
    updateHistoryBuffer();

    HistoryElementRemoveLayerAndSave* modf = new HistoryElementRemoveLayerAndSave(modData);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addMergeLayer(LevelData mergedData, QString newLayerName)
{
    updateHistoryBuffer();

    HistoryElementMergeLayer* modf = new HistoryElementMergeLayer(mergedData, newLayerName);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeSectionSettingsHistory(int sectionID, HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementSettingsSection* modf = new HistoryElementSettingsSection(sectionID, subtype, extraData);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addChangeLevelSettingsHistory(HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementMainSetting* modf = new HistoryElementMainSetting(subtype, extraData);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addPlacePlayerPointHistory(PlayerPoint plr, QVariant oldPos)
{
    updateHistoryBuffer();

    HistoryElementReplacePlayerPoint* modf = new HistoryElementReplacePlayerPoint(plr, oldPos);
    modf->setScene(this);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addRotateHistory(LevelData rotatedItems, LevelData unrotatedItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(unrotatedItems, rotatedItems);
    modf->setCustomHistoryName(tr("Rotate"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::addFlipHistory(LevelData flippedItems, LevelData unflippedItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(unflippedItems, flippedItems);
    modf->setCustomHistoryName(tr("Flip"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void LvlScene::historyBack()
{
    historyIndex--;
    QSharedPointer<IHistoryElement> lastOperation = operationList[historyIndex];

    lastOperation->undo();
    LvlData->modified = true;

    Debugger_updateItemList();
    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Undone: %1").arg(lastOperation->getHistoryName()));

}

void LvlScene::historyForward()
{

    QSharedPointer<IHistoryElement> lastOperation = operationList[historyIndex];

    lastOperation->redo();
    historyIndex++;

    LvlData->modified = true;
    Debugger_updateItemList();
    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Redone: %1").arg(lastOperation->getHistoryName()));
}

int LvlScene::getHistroyIndex()
{
    return historyIndex;
}

void LvlScene::updateHistoryBuffer()
{
    if(canRedo())
    {
        int lastSize = operationList.size();
        for(int i = historyIndex; i < lastSize; i++)
        {
            operationList.pop_back();
        }
    }

    while(operationList.size() >= GlobalSettings::historyLimit){
        operationList.pop_front();
        historyIndex--;
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
