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

#include "item_level.h"
#include "../common_features/mainwinconnect.h"

void WldScene::historyBack()
{
    historyIndex--;
    HistoryOperation lastOperation = operationList[historyIndex];

    switch( lastOperation.type )
    {

    default:
        break;
    }
    WldData->modified = true;

    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Undone: %1").arg(getHistoryText(lastOperation)));
}

void WldScene::historyForward()
{
    HistoryOperation lastOperation = operationList[historyIndex];

    switch( lastOperation.type )
    {

    default:
        break;

    }
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
    MainWinConnect::pMainWin->showStatusMsg(tr("Redone: %1").arg(getHistoryText(lastOperation)));
}

void WldScene::cleanupRedoElements()
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

QString WldScene::getHistoryText(WldScene::HistoryOperation operation)
{
    switch (operation.type) {
    case HistoryOperation::WORLDHISTORY_REMOVE: return tr("Remove");
    case HistoryOperation::WORLDHISTORY_PLACE: return tr("Place");
    case HistoryOperation::WORLDHISTORY_MOVE: return tr("Move");
    default:
        return tr("Unknown");
    }
}
