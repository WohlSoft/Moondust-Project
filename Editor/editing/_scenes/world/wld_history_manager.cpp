/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <defines.h>

#include "wld_scene.h"
#include "wld_history_manager.h"

#include <editing/_components/history/historyelementmodification.h>
#include <editing/_components/history/historyelementmainsetting.h>
#include <editing/_components/history/historyelementitemsetting.h>


WldHistoryManager::WldHistoryManager(WldScene *scene, QObject *parent):
    QObject(parent),
    m_scene(scene),
    historyChanged(false),
    historyIndex(0)
{
    connect(this,           &WldHistoryManager::refreshHistoryButtons,
            m_scene->m_mw,  &MainWindow::refreshHistoryButtons);
    connect(this, &WldHistoryManager::showStatusMessage,
            m_scene->m_mw,  &MainWindow::showStatusMsg);
}


void WldHistoryManager::addRemoveHistory(WorldData removedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification *modf = new HistoryElementModification(removedItems, WorldData());
    modf->setCustomHistoryName(tr("Remove"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::addPlaceHistory(WorldData placedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification *modf = new HistoryElementModification(WorldData(), placedItems);
    modf->setCustomHistoryName(tr("Place"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::addOverwriteHistory(WorldData removedItems, WorldData placedItems)
{
    updateHistoryBuffer();

    HistoryElementModification *modf = new HistoryElementModification(removedItems, placedItems);
    modf->setCustomHistoryName(tr("Place & Overwrite"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::addMoveHistory(WorldData sourceMovedItems, WorldData targetMovedItems)
{
    updateHistoryBuffer();

    //set first base
    HistoryElementModification *modf = new HistoryElementModification(sourceMovedItems, targetMovedItems);
    modf->setCustomHistoryName(tr("Move"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::addChangeWorldSettingsHistory(HistorySettings::WorldSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementMainSetting *modf = new HistoryElementMainSetting(subtype, extraData);
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::addChangeSettingsHistory(WorldData modifiedItems, HistorySettings::WorldSettingSubType subType, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementItemSetting *modf = new HistoryElementItemSetting(modifiedItems, subType, extraData);
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::addRotateHistory(WorldData rotatedItems, WorldData unrotatedItems)
{
    updateHistoryBuffer();

    HistoryElementModification *modf = new HistoryElementModification(unrotatedItems, rotatedItems);
    modf->setCustomHistoryName(tr("Rotate"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::addFlipHistory(WorldData flippedItems, WorldData unflippedItems)
{
    updateHistoryBuffer();

    HistoryElementModification *modf = new HistoryElementModification(unflippedItems, flippedItems);
    modf->setCustomHistoryName(tr("Flip"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::addTransformHistory(WorldData transformedItems, WorldData sourceItems)
{
    updateHistoryBuffer();

    HistoryElementModification *modf = new HistoryElementModification(sourceItems, transformedItems);
    modf->setCustomHistoryName(tr("Transform"));
    modf->setScene(m_scene);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    emit refreshHistoryButtons();
}

void WldHistoryManager::historyBack()
{
    historyIndex--;
    QSharedPointer<IHistoryElement> lastOperation = operationList[historyIndex];

    lastOperation->undo();
    m_scene->m_data->meta.modified = true;

    m_scene->Debugger_updateItemList();
    emit refreshHistoryButtons();
    emit showStatusMessage(tr("Undone: %1").arg(lastOperation->getHistoryName()));
}

void WldHistoryManager::historyForward()
{
    QSharedPointer<IHistoryElement> lastOperation = operationList[historyIndex];

    lastOperation->redo();
    historyIndex++;

    m_scene->m_data->meta.modified = true;
    m_scene->Debugger_updateItemList();
    emit refreshHistoryButtons();
    emit showStatusMessage(tr("Redone: %1").arg(lastOperation->getHistoryName()));
}

void WldHistoryManager::updateHistoryBuffer()
{
    if(canRedo())
    {
        int lastSize = operationList.size();
        for(int i = historyIndex; i < lastSize; i++)
            operationList.pop_back();
    }

    while(operationList.size() >= GlobalSettings::historyLimit)
    {
        operationList.pop_front();
        historyIndex--;
    }
}


void WldScene::historyBack()
{
    if(m_editMode == MODE_Resizing) //Don't switch history while resizing in process
    {
        resetResizers();
        return;
    }
    m_history->historyBack();
}

void WldScene::historyForward()
{
    if(m_editMode == MODE_Resizing) //Don't switch history action while resizing in process
    {
        resetResizers();
        return;
    }
    m_history->historyForward();
}

int WldScene::getHistroyIndex()
{
    return m_history->getHistroyIndex();
}

bool WldScene::canUndo()
{
    return m_history->canUndo();
}

bool WldScene::canRedo()
{
    return m_history->canRedo();
}



int WldHistoryManager::getHistroyIndex()
{
    return historyIndex;
}

bool WldHistoryManager::canUndo()
{
    return historyIndex > 0;
}

bool WldHistoryManager::canRedo()
{
    return historyIndex < operationList.size();
}
