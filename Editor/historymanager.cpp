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
#include "historymanager.h"

HistoryManager::HistoryManager() : historyIndex( 0 )
{
}

void HistoryManager::addRemoveHistory(LevelData removedItems)
{
    HistoryOperation rmOperation;
    rmOperation.type = HistoryOperation::LEVELHISTORY_REMOVE;
    rmOperation.data = removedItems;
    operationList.push_back(rmOperation);
    //add cleanup redo elements

    historyIndex++;
}

void HistoryManager::historyBack()
{
    historyIndex--;
    HistoryOperation lastOperation = operationList[historyIndex];
    switch( lastOperation.type )
    {
    case HistoryOperation::LEVELHISTORY_REMOVE:
        //revert remove
        LevelData deletedData = lastOperation.data;
        foreach (LevelBlock block, deletedData.blocks) {
            //place them back
            unsigned int newID = (scene->LvlData->blocks_array_id++);
            block.array_id = newID;
            scene->LvlData->blocks.push_back(block);
            scene->placeBlock(block);
        }
        break;
    };
}

int HistoryManager::getHistroyIndex()
{
    return historyIndex;
}

void HistoryManager::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}
