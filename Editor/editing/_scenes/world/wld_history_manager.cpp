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

#include <common_features/mainwinconnect.h>

#include "items/item_level.h"
#include "items/item_music.h"
#include "items/item_path.h"
#include "items/item_scene.h"
#include "items/item_tile.h"

#include <editing/_components/history/historyelementmodification.h>
#include <editing/_components/history/historyelementmainsetting.h>
#include <editing/_components/history/historyelementitemsetting.h>

#include <defines.h>

void WldScene::addRemoveHistory(WorldData removedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(removedItems, WorldData());
    modf->setCustomHistoryName(tr("Remove"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addPlaceHistory(WorldData placedItems)
{
    //add cleanup redo elements
    updateHistoryBuffer();
    //add new element
    HistoryElementModification* modf = new HistoryElementModification(WorldData(), placedItems);
    modf->setCustomHistoryName(tr("Place"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addOverwriteHistory(WorldData removedItems, WorldData placedItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(removedItems, placedItems);
    modf->setCustomHistoryName(tr("Place & Overwrite"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addMoveHistory(WorldData sourceMovedItems, WorldData targetMovedItems)
{
    updateHistoryBuffer();

    //set first base
    HistoryElementModification* modf = new HistoryElementModification(sourceMovedItems, targetMovedItems);
    modf->setCustomHistoryName(tr("Move"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addChangeWorldSettingsHistory(HistorySettings::WorldSettingSubType subtype, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementMainSetting* modf = new HistoryElementMainSetting(subtype, extraData);
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addChangeSettingsHistory(WorldData modifiedItems, HistorySettings::WorldSettingSubType subType, QVariant extraData)
{
    updateHistoryBuffer();

    HistoryElementItemSetting* modf = new HistoryElementItemSetting(modifiedItems, subType, extraData);
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addRotateHistory(WorldData rotatedItems, WorldData unrotatedItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(unrotatedItems, rotatedItems);
    modf->setCustomHistoryName(tr("Rotate"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addFlipHistory(WorldData flippedItems, WorldData unflippedItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(unflippedItems, flippedItems);
    modf->setCustomHistoryName(tr("Flip"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addTransformHistory(WorldData transformedItems, WorldData sourceItems)
{
    updateHistoryBuffer();

    HistoryElementModification* modf = new HistoryElementModification(sourceItems, transformedItems);
    modf->setCustomHistoryName(tr("Flip"));
    modf->setScene(this);

    operationList.push_back(QSharedPointer<IHistoryElement>(modf));
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::historyBack()
{
    historyIndex--;
    QSharedPointer<IHistoryElement> lastOperation = operationList[historyIndex];

    lastOperation->undo();
    WldData->modified = true;

    Debugger_updateItemList();
    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Undone: %1").arg(lastOperation->getHistoryName()));
}

void WldScene::historyForward()
{
    QSharedPointer<IHistoryElement> lastOperation = operationList[historyIndex];

    lastOperation->redo();
    historyIndex++;

    WldData->modified = true;
    Debugger_updateItemList();
    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Redone: %1").arg(lastOperation->getHistoryName()));
}

void WldScene::updateHistoryBuffer()
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

int WldScene::getHistroyIndex()
{
    return historyIndex;
}

bool WldScene::canUndo()
{
    return historyIndex > 0;
}

bool WldScene::canRedo()
{
    return historyIndex < operationList.size();
}
