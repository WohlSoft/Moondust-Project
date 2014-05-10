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
    HistoryOperation newHist;
    LevelData deletedData;
    LevelData newData;
    switch( lastOperation.type )
    {
        case HistoryOperation::LEVELHISTORY_REMOVE:
            //revert remove
            deletedData = lastOperation.data;

            foreach (LevelBlock block, deletedData.blocks)
            {
                //place them back
                unsigned int newID = (LvlData->blocks_array_id++);
                block.array_id = newID;
                LvlData->blocks.push_back(block);
                placeBlock(block);
                //use it so redo can find faster via arrayID
                newData.blocks.push_back(block);
            }

            foreach (LevelBGO bgo, deletedData.bgo)
            {
                //place them back
                unsigned int newID = (LvlData->bgo_array_id++);
                bgo.array_id = newID;
                LvlData->bgo.push_back(bgo);
                placeBGO(bgo);
                //use it so redo can find faster via arrayID
                newData.bgo.push_back(bgo);
            }

            newHist.type = HistoryOperation::LEVELHISTORY_REMOVE;
            newHist.data = newData;
            operationList.replace(historyIndex, newHist);
            break;
    default:
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
            foreach (LevelBlock block, deletedData.blocks)
            {
                sortedBlock[block.array_id] = block;
            }

            QMap<int, LevelBGO> sortedBGO;
            foreach (LevelBGO bgo, deletedData.bgo)
            {
                sortedBGO[bgo.array_id] = bgo;
            }

            bool blocksFinished;
            bool bgosFinished;
            foreach (QGraphicsItem* item, items()){
                if(item->data(0).toString()=="Block")
                {
                    if(sortedBlock.size()!=0)
                    {
                        QMap<int, LevelBlock>::iterator beginItem = sortedBlock.begin();
                        unsigned int currentArrayId = (*beginItem).array_id;
                        if((unsigned int)item->data(2).toInt()==currentArrayId)
                        {
                            ((ItemBlock*)item)->removeFromArray();
                            removeItem(item);
                            sortedBlock.erase(beginItem);
                        }
                    }
                    else
                    {
                        blocksFinished = true;
                    }
                }
                else
                if(item->data(0).toString()=="BGO")
                {
                    if(sortedBGO.size()!=0)
                    {
                        QMap<int, LevelBGO>::iterator beginItem = sortedBGO.begin();
                        unsigned int currentArrayId = (*beginItem).array_id;
                        if((unsigned int)item->data(2).toInt()==currentArrayId)
                        {
                            ((ItemBGO *)item)->removeFromArray();
                            removeItem(item);
                            sortedBGO.erase(beginItem);
                        }
                    }
                    else
                    {
                        bgosFinished = true;
                    }
                }
                if(blocksFinished&&bgosFinished)
                {
                    break;
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
