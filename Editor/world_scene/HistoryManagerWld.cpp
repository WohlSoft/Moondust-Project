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
#include "item_music.h"
#include "item_path.h"
#include "item_scene.h"
#include "item_tile.h"

void WldScene::addRemoveHistory(WorldData removedItems)
{
    //add cleanup redo elements
    cleanupRedoElements();
    //add new element
    HistoryOperation rmOperation;
    rmOperation.type = HistoryOperation::WORLDHISTORY_REMOVE;
    rmOperation.data = removedItems;
    operationList.push_back(rmOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addMoveHistory(WorldData sourceMovedItems, WorldData targetMovedItems)
{
    cleanupRedoElements();

    //set first base
    QPoint base = calcTopLeftCorner(&targetMovedItems);
    QList<QVariant> coor;
    coor << base.x() << base.y();

    HistoryOperation mvOperation;
    mvOperation.type = HistoryOperation::WORLDHISTORY_MOVE;
    mvOperation.data = sourceMovedItems;
    mvOperation.extraData = coor;
    operationList.push_back(mvOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::historyBack()
{
    historyIndex--;
    HistoryOperation lastOperation = operationList[historyIndex];

    switch( lastOperation.type )
    {
    case HistoryOperation::WORLDHISTORY_REMOVE:
    {
        //revert remove
        WorldData deletedData = lastOperation.data;

        foreach (WorldTiles tile, deletedData.tiles)
        {
            //place them back
            WldData->tiles.push_back(tile);
            placeTile(tile);
        }
        foreach (WorldPaths path, deletedData.paths)
        {
            //place them back
            WldData->paths.push_back(path);
            placePath(path);
        }
        foreach (WorldScenery scenery, deletedData.scenery)
        {
            //place them back
            WldData->scenery.push_back(scenery);
            placeScenery(scenery);
        }
        foreach (WorldLevels level, deletedData.levels)
        {
            //place them back
            WldData->levels.push_back(level);
            placeLevel(level);
        }

        //refresh Animation control
        if(opts.animationEnabled) stopAnimation();
        if(opts.animationEnabled) startAnimation();

        break;
    }
    case HistoryOperation::WORLDHISTORY_MOVE:
    {
        //revert move
        WorldData movedSourceData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &WldScene::historyUndoMoveTile, &WldScene::historyUndoMovePath, &WldScene::historyUndoMoveScenery, &WldScene::historyUndoMoveLevels);

        break;
    }
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
    case HistoryOperation::WORLDHISTORY_REMOVE:
    {
        //redo remove
        WorldData deletedData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(deletedData, &lastOperation, cbData, &WldScene::historyRemoveTiles, &WldScene::historyRemovePath, &WldScene::historyRemoveScenery, &WldScene::historyRemoveLevels);

        break;
    }
    case HistoryOperation::WORLDHISTORY_MOVE:
    {

        //revert move
        WorldData movedSourceData = lastOperation.data;

        //calc new Pos:
        long baseX=0, baseY=0;

        //set first base
        QPoint base = calcTopLeftCorner(&movedSourceData);
        baseX = (long)base.x();
        baseY = (long)base.y();

        CallbackData cbData;
        cbData.x = baseX;
        cbData.y = baseY;
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &WldScene::historyRedoMoveTile, &WldScene::historyRedoMovePath, &WldScene::historyRedoMoveScenery, &WldScene::historyRedoMoveLevels);
        break;
    }
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

void WldScene::historyRedoMoveTile(WldScene::CallbackData cbData, WorldTiles data)
{
    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->extraData.toList()[0].toInt()+diffX,cbData.hist->extraData.toList()[1].toInt()+diffY));
    ((ItemTile *)(cbData.item))->tileData.x = (long)cbData.item->scenePos().x();
    ((ItemTile *)(cbData.item))->tileData.y = (long)cbData.item->scenePos().y();
    ((ItemTile *)(cbData.item))->arrayApply();
}

void WldScene::historyRedoMovePath(WldScene::CallbackData cbData, WorldPaths data)
{
    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->extraData.toList()[0].toInt()+diffX,cbData.hist->extraData.toList()[1].toInt()+diffY));
    ((ItemPath *)(cbData.item))->pathData.x = (long)cbData.item->scenePos().x();
    ((ItemPath *)(cbData.item))->pathData.y = (long)cbData.item->scenePos().y();
    ((ItemPath *)(cbData.item))->arrayApply();
}

void WldScene::historyRedoMoveScenery(WldScene::CallbackData cbData, WorldScenery data)
{
    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->extraData.toList()[0].toInt()+diffX,cbData.hist->extraData.toList()[1].toInt()+diffY));
    ((ItemScene *)(cbData.item))->sceneData.x = (long)cbData.item->scenePos().x();
    ((ItemScene *)(cbData.item))->sceneData.y = (long)cbData.item->scenePos().y();
    ((ItemScene *)(cbData.item))->arrayApply();
}

void WldScene::historyRedoMoveLevels(WldScene::CallbackData cbData, WorldLevels data)
{
    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->extraData.toList()[0].toInt()+diffX,cbData.hist->extraData.toList()[1].toInt()+diffY));
    ((ItemLevel *)(cbData.item))->levelData.x = (long)cbData.item->scenePos().x();
    ((ItemLevel *)(cbData.item))->levelData.y = (long)cbData.item->scenePos().y();
    ((ItemLevel *)(cbData.item))->arrayApply();
}

void WldScene::historyUndoMoveTile(WldScene::CallbackData cbData, WorldTiles data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemTile *)(cbData.item))->tileData.x = (long)cbData.item->scenePos().x();
    ((ItemTile *)(cbData.item))->tileData.y = (long)cbData.item->scenePos().y();
    ((ItemTile *)(cbData.item))->arrayApply();
}

void WldScene::historyUndoMovePath(WldScene::CallbackData cbData, WorldPaths data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemPath *)(cbData.item))->pathData.x = (long)cbData.item->scenePos().x();
    ((ItemPath *)(cbData.item))->pathData.y = (long)cbData.item->scenePos().y();
    ((ItemPath *)(cbData.item))->arrayApply();
}

void WldScene::historyUndoMoveScenery(WldScene::CallbackData cbData, WorldScenery data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemScene *)(cbData.item))->sceneData.x = (long)cbData.item->scenePos().x();
    ((ItemScene *)(cbData.item))->sceneData.y = (long)cbData.item->scenePos().y();
    ((ItemScene *)(cbData.item))->arrayApply();
}

void WldScene::historyUndoMoveLevels(WldScene::CallbackData cbData, WorldLevels data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemLevel *)(cbData.item))->levelData.x = (long)cbData.item->scenePos().x();
    ((ItemLevel *)(cbData.item))->levelData.y = (long)cbData.item->scenePos().y();
    ((ItemLevel *)(cbData.item))->arrayApply();
}

void WldScene::historyRemoveTiles(WldScene::CallbackData cbData, WorldTiles /*data*/)
{
    ((ItemTile*)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void WldScene::historyRemovePath(WldScene::CallbackData cbData, WorldPaths /*data*/)
{
    ((ItemPath*)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void WldScene::historyRemoveScenery(WldScene::CallbackData cbData, WorldScenery /*data*/)
{
    ((ItemScene*)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void WldScene::historyRemoveLevels(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void WldScene::findGraphicsItem(WorldData toFind, WldScene::HistoryOperation *operation, WldScene::CallbackData customData, WldScene::callBackWorldTiles clbTiles, WldScene::callBackWorldPaths clbPaths, WldScene::callBackWorldScenery clbScenery, WldScene::callBackWorldLevels clbLevels, bool ignoreTiles, bool ignorePaths, bool ignoreScenery, bool ignoreLevels)
{
    QMap<int, WorldTiles> sortedTiles;
    if(!ignoreTiles){
        foreach (WorldTiles tile, toFind.tiles)
        {
            sortedTiles[tile.array_id] = tile;
        }
    }
    QMap<int, WorldPaths> sortedPaths;
    if(!ignorePaths){
        foreach (WorldPaths path, toFind.paths)
        {
            sortedPaths[path.array_id] = path;
        }
    }
    QMap<int, WorldScenery> sortedScenery;
    if(!ignoreScenery){
        foreach (WorldScenery scenery, toFind.scenery)
        {
            sortedScenery[scenery.array_id] = scenery;
        }
    }
    QMap<int, WorldLevels> sortedLevels;
    if(!ignoreLevels){
        foreach (WorldLevels level, toFind.levels)
        {
            sortedLevels[level.array_id] = level;
        }
    }

    CallbackData cbData = customData;
    cbData.hist = operation;
    QMap<int, QGraphicsItem*> sortedGraphTiles;
    QMap<int, QGraphicsItem*> sortedGraphPath;
    QMap<int, QGraphicsItem*> sortedGraphScenery;
    QMap<int, QGraphicsItem*> sortedGraphLevels;

    foreach (QGraphicsItem* unsortedItem, items())
    {
        if(unsortedItem->data(0).toString()=="TILE")
        {
            if(!ignoreTiles){
                sortedGraphTiles[unsortedItem->data(2).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(0).toString()=="PATH")
        {
            if(!ignorePaths){
                sortedGraphPath[unsortedItem->data(2).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(0).toString()=="SCENERY")
        {
            if(!ignoreScenery){
                sortedGraphScenery[unsortedItem->data(2).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(0).toString()=="LEVEL")
        {
            if(!ignoreLevels){
                sortedGraphLevels[unsortedItem->data(2).toInt()] = unsortedItem;
            }
        }
    }

    if(!ignoreTiles){
        foreach (QGraphicsItem* item, sortedGraphTiles)
        {

            if(sortedTiles.size()!=0)
            {
                QMap<int, WorldTiles>::iterator beginItem = sortedTiles.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedTiles.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedTiles.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbTiles)(cbData,(*beginItem));
                    sortedTiles.erase(beginItem);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(!ignorePaths){
        foreach (QGraphicsItem* item, sortedGraphPath)
        {

            if(sortedPaths.size()!=0)
            {
                QMap<int, WorldPaths>::iterator beginItem = sortedPaths.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedPaths.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedPaths.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbPaths)(cbData,(*beginItem));
                    sortedPaths.erase(beginItem);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(!ignoreScenery){
        foreach (QGraphicsItem* item, sortedGraphScenery)
        {

            if(sortedScenery.size()!=0)
            {
                QMap<int, WorldScenery>::iterator beginItem = sortedScenery.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedScenery.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedScenery.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbScenery)(cbData,(*beginItem));
                    sortedScenery.erase(beginItem);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(!ignoreLevels){
        foreach (QGraphicsItem* item, sortedGraphLevels)
        {

            if(sortedLevels.size()!=0)
            {
                QMap<int, WorldLevels>::iterator beginItem = sortedLevels.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedLevels.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedLevels.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbLevels)(cbData,(*beginItem));
                    sortedLevels.erase(beginItem);
                }
            }
            else
            {
                break;
            }
        }
    }

}

QPoint WldScene::calcTopLeftCorner(WorldData *data)
{
    //calc new Pos:
    int baseX=0, baseY=0;

    //set first base
    if(!data->tiles.isEmpty()){
        baseX = (int)data->tiles[0].x;
        baseY = (int)data->tiles[0].y;
    }else if(!data->paths.isEmpty()){
        baseX = (int)data->paths[0].x;
        baseY = (int)data->paths[0].y;
    }else if(!data->scenery.isEmpty()){
        baseX = (int)data->scenery[0].x;
        baseY = (int)data->scenery[0].y;
    }else if(!data->levels.isEmpty()){
        baseX = (int)data->levels[0].x;
        baseY = (int)data->levels[0].y;
    }
    /*else if(!data->music.isEmpty()){
        baseX = (int)data->music[0].x;
        baseY = (int)data->music[0].y;
    }*/

    foreach (WorldTiles tiles, data->tiles) {
        if((int)tiles.x<baseX){
            baseX = (int)tiles.x;
        }
        if((int)tiles.y<baseY){
            baseY = (int)tiles.y;
        }
    }
    foreach (WorldPaths path, data->paths){
        if((int)path.x<baseX){
            baseX = (int)path.x;
        }
        if((int)path.y<baseY){
            baseY = (int)path.y;
        }
    }
    foreach (WorldScenery scenery, data->scenery){
        if((int)scenery.x<baseX){
            baseX = (int)scenery.x;
        }
        if((int)scenery.y<baseY){
            baseY = (int)scenery.y;
        }
    }
    foreach (WorldLevels level, data->levels){
        if((int)level.x<baseX){
            baseX = (int)level.x;
        }
        if((int)level.y<baseY){
            baseY = (int)level.y;
        }
    }
//    foreach (WorldMusic music, data->music) {
//        if((int)music.x<baseX){
//            baseX = (int)music.x;
//        }
//        if((int)music.y<baseY){
//            baseY = (int)music.y;
//        }
//    }



    return QPoint(baseX, baseY);
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
