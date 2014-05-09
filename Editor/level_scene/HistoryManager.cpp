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

void LvlScene::historyBack()
{
    historyIndex--;
    HistoryOperation lastOperation = operationList[historyIndex];
    switch( lastOperation.type )
    {
        case HistoryOperation::LEVELHISTORY_REMOVE:
            //revert remove
            LevelData deletedData = lastOperation.data;
            LevelData newData;
            foreach (LevelBlock block, deletedData.blocks) {
                //place them back
                unsigned int newID = (LvlData->blocks_array_id++);
                block.array_id = newID;
                LvlData->blocks.push_back(block);
                placeBlock(block);
                //use it so redo can find faster via arrayID
                newData.blocks.push_back(block);
            }
            HistoryOperation newHist;
            newHist.type = HistoryOperation::LEVELHISTORY_REMOVE;
            newHist.data = newData;
            operationList.replace(historyIndex, newHist);
            break;
    }

    historyChanged = true;
}

void LvlScene::historyForward()
{

    HistoryOperation lastOperation = operationList[historyIndex];
    switch( lastOperation.type )
    {
        case HistoryOperation::LEVELHISTORY_REMOVE:
            //redo remove
            LevelData deletedData = lastOperation.data;
            QMap<int, LevelBlock> sortedBlock;
            foreach (LevelBlock block, deletedData.blocks){
                sortedBlock[block.array_id] = block;
            }
            foreach (QGraphicsItem* item, items()){
                if(item->data(0).toString()=="Block"){
                    QMap<int, LevelBlock>::iterator beginItem = sortedBlock.begin();
                    if(item->data(2).toInt()==(*beginItem).array_id){
                        ((ItemBlock*)item)->removeFromArray();
                        removeItem(item);
                    }
                    sortedBlock.erase(beginItem);
                }
            }
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
    if(canRedo()){
        int lastSize = operationList.size();
        for(int i = historyIndex; i < lastSize; i++){
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
