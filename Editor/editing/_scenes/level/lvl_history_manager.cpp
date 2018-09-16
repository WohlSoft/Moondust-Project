/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/logger.h>
#include <PGE_File_Formats/file_formats.h>
#include <audio/music_player.h>
#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_sctc_props.h>

#include "lvl_history_manager.h"

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

LvlHistoryManager::LvlHistoryManager(LvlScene *scene, QObject *parent) :
    QObject(parent),
    m_scene(scene),
    historyChanged(false),
    historyIndex(0)
{
    connect(this,           &LvlHistoryManager::refreshHistoryButtons,
            m_scene->m_mw,  &MainWindow::refreshHistoryButtons);
    connect(this, &LvlHistoryManager::showStatusMessage,
            m_scene->m_mw,  &MainWindow::showStatusMsg);
}

void LvlHistoryManager::addRemove(LevelData removedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(removedItems, LevelData());
    modf->setCustomHistoryName(tr("Remove"));
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addPlace(LevelData placedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(LevelData(), placedItems);
    modf->setCustomHistoryName(tr("Place"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addOverwrite(LevelData removedItems, LevelData placedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(removedItems, placedItems);
    modf->setCustomHistoryName(tr("Place & Overwrite"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addPlace(LevelDoor door, bool isEntrance)
{
    updateHistoryBuffer();

    HistoryElementPlaceDoor* modf = new HistoryElementPlaceDoor(door, isEntrance);
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addMove(LevelData sourceMovedItems, LevelData targetMovedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(sourceMovedItems, targetMovedItems);
    modf->setCustomHistoryName(tr("Move"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addChangeSettings(LevelData modifiedItems, HistorySettings::LevelSettingSubType subType, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementItemSetting* modf = new HistoryElementItemSetting(modifiedItems, subType, extraData);
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addChangeSettings(LevelData modifiedItems, HistoryElementCustomSetting *setting, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementItemSetting* modf = new HistoryElementItemSetting(modifiedItems, setting, extraData);
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addResizeSection(int sectionID, long oldLeft, long oldTop, long oldRight, long oldBottom,
                                       long newLeft, long newTop, long newRight, long newBottom)
{
    updateHistoryBuffer();
    HistoryElementResizeSection *modf = new HistoryElementResizeSection(sectionID,
                                                                        QRect(QPoint(oldLeft, oldTop), QPoint(oldRight, oldBottom)),
                                                                        QRect(QPoint(newLeft, newTop), QPoint(newRight, newBottom)));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addChangedLayer(LevelData changedItems, QString newLayerName)
{
    updateHistoryBuffer();

    HistoryElementLayerChanged *modf = new HistoryElementLayerChanged(changedItems, newLayerName);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addResizeBlock(LevelBlock bl, long oldLeft, long oldTop, long oldRight, long oldBottom, long newLeft, long newTop, long newRight, long newBottom)
{
    updateHistoryBuffer();

    HistoryElementResizeBlock *modf = new HistoryElementResizeBlock(bl,
                                                                        QRect(QPoint(oldLeft, oldTop), QPoint(oldRight, oldBottom)),
                                                                        QRect(QPoint(newLeft, newTop), QPoint(newRight, newBottom)));
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addResizePhysEnv(LevelPhysEnv wt, long oldLeft, long oldTop, long oldRight, long oldBottom, long newLeft, long newTop, long newRight, long newBottom)
{
    updateHistoryBuffer();

    HistoryElementResizeWater *modf = new HistoryElementResizeWater(wt,
                                                                        QRect(QPoint(oldLeft, oldTop), QPoint(oldRight, oldBottom)),
                                                                        QRect(QPoint(newLeft, newTop), QPoint(newRight, newBottom)));
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addAddWarp(int array_id, int listindex, int doorindex)
{
    updateHistoryBuffer();

    HistoryElementAddWarp* modf = new HistoryElementAddWarp(array_id, listindex, doorindex);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addRemoveWarp(LevelDoor removedDoor)
{
    updateHistoryBuffer();

    HistoryElementRemoveWarp* modf = new HistoryElementRemoveWarp(removedDoor);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addChangeWarpSettings(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementSettingsWarp* modf = new HistoryElementSettingsWarp(array_id, subtype, extraData);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addAddEvent(LevelSMBX64Event ev)
{
    updateHistoryBuffer();

    HistoryElementModifyEvent* modf = new HistoryElementModifyEvent(ev, false);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addRemoveEvent(LevelSMBX64Event ev)
{
    updateHistoryBuffer();

    HistoryElementModifyEvent* modf = new HistoryElementModifyEvent(ev, true);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addDuplicateEvent(LevelSMBX64Event newDuplicate)
{
    updateHistoryBuffer();

    HistoryElementModifyEvent* modf = new HistoryElementModifyEvent(newDuplicate, false);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addChangeEventSettings(int array_id, HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementSettingsEvent* modf = new HistoryElementSettingsEvent(array_id, subtype, extraData);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addChangedNewLayer(LevelData changedItems, LevelLayer newLayer)
{
    updateHistoryBuffer();

    HistoryElementChangedNewLayer* modf = new HistoryElementChangedNewLayer(changedItems, newLayer);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addAddLayer(int array_id, QString name)
{
    updateHistoryBuffer();

    HistoryElementAddLayer* modf = new HistoryElementAddLayer(array_id, name);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addRemoveLayer(LevelData modData)
{
    updateHistoryBuffer();

    HistoryElementRemoveLayer* modf = new HistoryElementRemoveLayer(modData);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addRenameEvent(int array_id, QString oldName, QString newName)
{
    updateHistoryBuffer();

    HistoryElementRenameEvent* modf = new HistoryElementRenameEvent(array_id, oldName, newName);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addRenameLayer(int array_id, QString oldName, QString newName)
{
    updateHistoryBuffer();

    HistoryElementRenameLayer* modf = new HistoryElementRenameLayer(array_id, oldName, newName);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addRemoveLayerAndSaveItems(LevelData modData)
{
    updateHistoryBuffer();

    HistoryElementRemoveLayerAndSave* modf = new HistoryElementRemoveLayerAndSave(modData);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addMergeLayer(LevelData mergedData, QString newLayerName)
{
    updateHistoryBuffer();

    HistoryElementMergeLayer* modf = new HistoryElementMergeLayer(mergedData, newLayerName);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addChangeSectionSettings(int sectionID, HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementSettingsSection* modf = new HistoryElementSettingsSection(sectionID, subtype, extraData);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addChangeLevelSettings(HistorySettings::LevelSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementMainSetting* modf = new HistoryElementMainSetting(subtype, extraData);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addPlacePlayerPoint(PlayerPoint plr, QVariant oldPos)
{
    updateHistoryBuffer();

    HistoryElementReplacePlayerPoint* modf = new HistoryElementReplacePlayerPoint(plr, oldPos);
    modf->setScene(m_scene);
    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addRotate(LevelData rotatedItems, LevelData unrotatedItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(unrotatedItems, rotatedItems);
    modf->setCustomHistoryName(tr("Rotate"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addFlip(LevelData flippedItems, LevelData unflippedItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(unflippedItems, flippedItems);
    modf->setCustomHistoryName(tr("Flip"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void LvlHistoryManager::addTransform(LevelData transformedItems, LevelData sourceItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(sourceItems, transformedItems);
    modf->setCustomHistoryName(tr("Transform"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}


void LvlScene::historyBack()
{
    if(m_editMode == MODE_Resizing) //Don't switch history while resizing in process
    {
        resetResizers();
        return;
    }
    m_history->historyBack();
}

void LvlScene::historyForward()
{
    if(m_editMode==MODE_Resizing) //Don't switch history action while resizing in process
    {
        resetResizers();
        return;
    }
    m_history->historyForward();
}


void LvlHistoryManager::historyBack()
{
    historyIndex--;
    QSharedPointer<IHistoryElement> lastOperation = operationList[historyIndex];

    lastOperation->undo();
    m_scene->m_data->meta.modified = true;

    m_scene->Debugger_updateItemList();
    emit refreshHistoryButtons();
    emit showStatusMessage(tr("Undone: %1").arg(lastOperation->getHistoryName()));
}


void LvlHistoryManager::historyForward()
{
    QSharedPointer<IHistoryElement> lastOperation = operationList[historyIndex];

    lastOperation->redo();
    historyIndex++;

    m_scene->m_data->meta.modified = true;
    m_scene->Debugger_updateItemList();
    emit refreshHistoryButtons();
    emit showStatusMessage(tr("Redone: %1").arg(lastOperation->getHistoryName()));
}

int LvlScene::getHistroyIndex()
{
    return m_history->getHistroyIndex();
}

int LvlHistoryManager::getHistroyIndex()
{
    return historyIndex;
}

void LvlHistoryManager::updateHistoryBuffer()
{
    if(canRedo())
    {
        int lastSize = operationList.size();
        for(int i = historyIndex; i < lastSize; i++)
        {
            operationList.pop_back();
        }
    }

    while(operationList.size() >= GlobalSettings::historyLimit) {
        operationList.pop_front();
        historyIndex--;
    }
}

bool LvlScene::canUndo()
{
    return m_history->canUndo();
}

bool LvlScene::canRedo()
{
    return m_history->canRedo();
}


bool LvlHistoryManager::canUndo()
{
    return historyIndex > 0;
}

bool LvlHistoryManager::canRedo()
{
    return historyIndex < operationList.size();
}

