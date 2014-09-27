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

void WldScene::addPlaceHistory(WorldData placedItems)
{
    //add cleanup redo elements
    cleanupRedoElements();
    //add new element
    HistoryOperation plOperation;
    plOperation.type = HistoryOperation::WORLDHISTORY_PLACE;
    plOperation.data = placedItems;

    operationList.push_back(plOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addOverwriteHistory(WorldData removedItems, WorldData placedItems)
{
    cleanupRedoElements();

    HistoryOperation ovOperation;
    ovOperation.type = HistoryOperation::WORLDHISTORY_OVERWRITE;
    ovOperation.data = placedItems;
    ovOperation.data_mod = removedItems;

    operationList.push_back(ovOperation);
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

void WldScene::addChangeWorldSettingsHistory(WldScene::SettingSubType subtype, QVariant extraData)
{
    cleanupRedoElements();

    HistoryOperation chLevelSettingsOperation;
    chLevelSettingsOperation.type = HistoryOperation::WORLDHISTORY_CHANGEDSETTINGSWORLD;
    chLevelSettingsOperation.subtype = subtype;
    chLevelSettingsOperation.extraData = extraData;
    operationList.push_back(chLevelSettingsOperation);
    historyIndex++;

    MainWinConnect::pMainWin->refreshHistoryButtons();
}

void WldScene::addChangeSettingsHistory(WorldData modifiedItems, WldScene::SettingSubType subType, QVariant extraData)
{
    cleanupRedoElements();

    HistoryOperation modOperation;
    modOperation.type = HistoryOperation::WORLDHISTORY_CHANGEDSETTINGSWORLDITEM;
    modOperation.data = modifiedItems;
    modOperation.subtype = subType;
    modOperation.extraData = extraData;
    operationList.push_back(modOperation);
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
        foreach (WorldMusic music, deletedData.music)
        {
            WldData->music.push_back(music);
            placeMusicbox(music);
        }

        //refresh Animation control
        if(opts.animationEnabled) stopAnimation();
        if(opts.animationEnabled) startAnimation();

        break;
    }
    case HistoryOperation::WORLDHISTORY_PLACE:
    {
        //revert place
        WorldData placeData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(placeData, &lastOperation, cbData, &WldScene::historyRemoveTiles, &WldScene::historyRemovePath, &WldScene::historyRemoveScenery, &WldScene::historyRemoveLevels, &WldScene::historyRemoveMusic);

        break;
    }
    case HistoryOperation::WORLDHISTORY_OVERWRITE:
    {

        //revert remove
        WorldData deletedData = lastOperation.data_mod;

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
        foreach (WorldMusic music, deletedData.music)
        {
            WldData->music.push_back(music);
            placeMusicbox(music);
        }

        //revert place
        WorldData placeData = lastOperation.data;

        CallbackData cbData;
        findGraphicsItem(placeData, &lastOperation, cbData, &WldScene::historyRemoveTiles, &WldScene::historyRemovePath, &WldScene::historyRemoveScenery, &WldScene::historyRemoveLevels, &WldScene::historyRemoveMusic);


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
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &WldScene::historyUndoMoveTile, &WldScene::historyUndoMovePath, &WldScene::historyUndoMoveScenery, &WldScene::historyUndoMoveLevels, &WldScene::historyUndoMoveMusic);

        break;
    }
    case HistoryOperation::WORLDHISTORY_CHANGEDSETTINGSWORLD:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        QVariant extraData = lastOperation.extraData;

        if(subtype == SETTING_HUB){
            WldData->HubStyledWorld = !extraData.toBool();
        }else if(subtype == SETTING_RESTARTAFTERFAIL){
            WldData->restartlevel = !extraData.toBool();
        }else if(subtype == SETTING_TOTALSTARS){
            WldData->stars = extraData.toList()[0].toInt();
        }else if(subtype == SETTING_INTROLEVEL){
            WldData->IntroLevel_file = extraData.toList()[0].toString();
        }else if(subtype == SETTING_CHARACTER){
            int ind = MainWinConnect::pMainWin->configs.getCharacterI(extraData.toList()[0].toInt());
            if(ind!=-1)
                WldData->nocharacter[ind] = !extraData.toList()[1].toBool();
        }else if(subtype == SETTING_WORLDTITLE){
            WldData->EpisodeTitle = extraData.toList()[0].toString();
            if(MainWinConnect::pMainWin->activeChildWindow()==3)
                MainWinConnect::pMainWin->activeWldEditWin()->setWindowTitle(
                            extraData.toList()[0].toString() == "" ? MainWinConnect::pMainWin->activeWldEditWin()->userFriendlyCurrentFile() : extraData.toList()[0].toString());
        }

        MainWinConnect::pMainWin->setCurrentWorldSettings();

        break;
    }
    case HistoryOperation::WORLDHISTORY_CHANGEDSETTINGSWORLDITEM:
    {
        WorldData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        if(lastOperation.subtype == SETTING_PATHBACKGROUND){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingPathBackgroundLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_BIGPATHBACKGROUND){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingBigPathBackgroundLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_ALWAYSVISIBLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingAlwaysVisibleLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_GAMESTARTPOINT){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingGameStartPointLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_DOORID){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingDoorIDLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_PATHBYTOP){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingPathByTopLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_PATHBYRIGHT){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingPathByRightLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_PATHBYBOTTOM){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingPathByBottomLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_PATHBYLEFT){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingPathByLeftLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_GOTOX){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingGotoXLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_GOTOY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingGotoYLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_LEVELFILE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingLevelfileLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_LEVELTITLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyUndoSettingLeveltitleLevel, 0, true, true, true, false, true);
        }

        break;
    }
    default:
        break;
    }
    WldData->modified = true;

    Debugger_updateItemList();
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
        findGraphicsItem(deletedData, &lastOperation, cbData, &WldScene::historyRemoveTiles, &WldScene::historyRemovePath, &WldScene::historyRemoveScenery, &WldScene::historyRemoveLevels, &WldScene::historyRemoveMusic);

        break;
    }
    case HistoryOperation::WORLDHISTORY_PLACE:
    {
        //revert remove
        WorldData placedData = lastOperation.data;

        foreach (WorldTiles tile, placedData.tiles)
        {
            //place them back
            WldData->tiles.push_back(tile);
            placeTile(tile);
        }
        foreach (WorldPaths path, placedData.paths)
        {
            //place them back
            WldData->paths.push_back(path);
            placePath(path);
        }
        foreach (WorldScenery scenery, placedData.scenery)
        {
            //place them back
            WldData->scenery.push_back(scenery);
            placeScenery(scenery);
        }
        foreach (WorldLevels level, placedData.levels)
        {
            //place them back
            WldData->levels.push_back(level);
            placeLevel(level);
        }
        foreach (WorldMusic music, placedData.music)
        {
            WldData->music.push_back(music);
            placeMusicbox(music);
        }

        //refresh Animation control
        if(opts.animationEnabled) stopAnimation();
        if(opts.animationEnabled) startAnimation();

        break;
    }
    case HistoryOperation::WORLDHISTORY_OVERWRITE:
    {

        //redo remove
        WorldData deletedData = lastOperation.data_mod;

        CallbackData cbData;
        findGraphicsItem(deletedData, &lastOperation, cbData, &WldScene::historyRemoveTiles, &WldScene::historyRemovePath, &WldScene::historyRemoveScenery, &WldScene::historyRemoveLevels, &WldScene::historyRemoveMusic);
        //revert remove
        WorldData placedData = lastOperation.data;

        foreach (WorldTiles tile, placedData.tiles)
        {
            //place them back
            WldData->tiles.push_back(tile);
            placeTile(tile);
        }
        foreach (WorldPaths path, placedData.paths)
        {
            //place them back
            WldData->paths.push_back(path);
            placePath(path);
        }
        foreach (WorldScenery scenery, placedData.scenery)
        {
            //place them back
            WldData->scenery.push_back(scenery);
            placeScenery(scenery);
        }
        foreach (WorldLevels level, placedData.levels)
        {
            //place them back
            WldData->levels.push_back(level);
            placeLevel(level);
        }
        foreach (WorldMusic music, placedData.music)
        {
            WldData->music.push_back(music);
            placeMusicbox(music);
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

        //calc new Pos:
        long baseX=0, baseY=0;

        //set first base
        QPoint base = calcTopLeftCorner(&movedSourceData);
        baseX = (long)base.x();
        baseY = (long)base.y();

        CallbackData cbData;
        cbData.x = baseX;
        cbData.y = baseY;
        findGraphicsItem(movedSourceData, &lastOperation, cbData, &WldScene::historyRedoMoveTile, &WldScene::historyRedoMovePath, &WldScene::historyRedoMoveScenery, &WldScene::historyRedoMoveLevels, &WldScene::historyRedoMoveMusic);
        break;
    }
    case HistoryOperation::WORLDHISTORY_CHANGEDSETTINGSWORLD:
    {
        SettingSubType subtype = (SettingSubType)lastOperation.subtype;
        QVariant extraData = lastOperation.extraData;

        if(subtype == SETTING_HUB){
            WldData->HubStyledWorld = extraData.toBool();
        }else if(subtype == SETTING_RESTARTAFTERFAIL){
            WldData->restartlevel = extraData.toBool();
        }else if(subtype == SETTING_TOTALSTARS){
            WldData->stars = extraData.toList()[1].toInt();
        }else if(subtype == SETTING_INTROLEVEL){
            WldData->IntroLevel_file = extraData.toList()[1].toString();
        }else if(subtype == SETTING_CHARACTER){
            int ind = MainWinConnect::pMainWin->configs.getCharacterI(extraData.toList()[0].toInt());
            if(ind!=-1)
                WldData->nocharacter[ind] = extraData.toList()[1].toBool();
        }else if(subtype == SETTING_WORLDTITLE){
            WldData->EpisodeTitle = extraData.toList()[1].toString();
            if(MainWinConnect::pMainWin->activeChildWindow()==3)
                MainWinConnect::pMainWin->activeWldEditWin()->setWindowTitle(
                            extraData.toList()[1].toString() == "" ? MainWinConnect::pMainWin->activeWldEditWin()->userFriendlyCurrentFile() : extraData.toList()[1].toString());
        }

        MainWinConnect::pMainWin->setCurrentWorldSettings();

        break;
    }
    case HistoryOperation::WORLDHISTORY_CHANGEDSETTINGSWORLDITEM:
    {
        WorldData modifiedSourceData = lastOperation.data;

        CallbackData cbData;
        if(lastOperation.subtype == SETTING_PATHBACKGROUND){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingPathBackgroundLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_BIGPATHBACKGROUND){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingBigPathBackgroundLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_ALWAYSVISIBLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingAlwaysVisibleLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_GAMESTARTPOINT){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingGameStartPointLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_DOORID){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingDoorIDLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_PATHBYTOP){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingPathByTopLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_PATHBYRIGHT){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingPathByRightLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_PATHBYBOTTOM){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingPathByBottomLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_PATHBYLEFT){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingPathByLeftLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_GOTOX){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingGotoXLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_GOTOY){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingGotoYLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_LEVELFILE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingLevelfileLevel, 0, true, true, true, false, true);
        }else if(lastOperation.subtype == SETTING_LEVELTITLE){
            findGraphicsItem(modifiedSourceData, &lastOperation, cbData, 0, 0, 0, &WldScene::historyRedoSettingLeveltitleLevel, 0, true, true, true, false, true);
        }

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

void WldScene::historyRedoMoveMusic(WldScene::CallbackData cbData, WorldMusic data)
{
    long diffX = data.x - cbData.x;
    long diffY = data.y - cbData.y;

    cbData.item->setPos(QPointF(cbData.hist->extraData.toList()[0].toInt()+diffX,cbData.hist->extraData.toList()[1].toInt()+diffY));
    ((ItemMusic *)(cbData.item))->musicData.x = (long)cbData.item->scenePos().x();
    ((ItemMusic *)(cbData.item))->musicData.y = (long)cbData.item->scenePos().y();
    ((ItemMusic *)(cbData.item))->arrayApply();
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

void WldScene::historyUndoMoveMusic(WldScene::CallbackData cbData, WorldMusic data)
{
    cbData.item->setPos(QPointF(data.x,data.y));
    ((ItemMusic *)(cbData.item))->musicData.x = (long)cbData.item->scenePos().x();
    ((ItemMusic *)(cbData.item))->musicData.y = (long)cbData.item->scenePos().y();
    ((ItemMusic *)(cbData.item))->arrayApply();
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

void WldScene::historyRemoveMusic(WldScene::CallbackData cbData, WorldMusic /*data*/)
{
    ((ItemMusic*)cbData.item)->removeFromArray();
    removeItem(cbData.item);
    delete cbData.item;
}

void WldScene::historyUndoSettingPathBackgroundLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->setPath(data.pathbg);
}

void WldScene::historyRedoSettingPathBackgroundLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->setPath(cbData.hist->extraData.toBool());
}

void WldScene::historyUndoSettingBigPathBackgroundLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->setbPath(data.bigpathbg);
}

void WldScene::historyRedoSettingBigPathBackgroundLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->setbPath(cbData.hist->extraData.toBool());
}

void WldScene::historyUndoSettingAlwaysVisibleLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->alwaysVisible(data.alwaysVisible);
}

void WldScene::historyRedoSettingAlwaysVisibleLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->alwaysVisible(cbData.hist->extraData.toBool());
}

void WldScene::historyUndoSettingGameStartPointLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.gamestart = data.gamestart;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingGameStartPointLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.gamestart = cbData.hist->extraData.toBool();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingLevelfileLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.lvlfile = data.lvlfile;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingLevelfileLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.lvlfile = cbData.hist->extraData.toString();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingLeveltitleLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.title = data.title;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingLeveltitleLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.title = cbData.hist->extraData.toString();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingDoorIDLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.entertowarp = data.entertowarp;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingDoorIDLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.entertowarp = cbData.hist->extraData.toInt();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingPathByTopLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.top_exit = data.top_exit;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingPathByTopLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.top_exit = cbData.hist->extraData.toInt();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingPathByRightLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.right_exit = data.right_exit;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingPathByRightLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.right_exit = cbData.hist->extraData.toInt();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingPathByBottomLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.bottom_exit = data.bottom_exit;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingPathByBottomLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.bottom_exit = cbData.hist->extraData.toInt();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingPathByLeftLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.left_exit = data.left_exit;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingPathByLeftLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.left_exit = cbData.hist->extraData.toInt();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingGotoXLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.gotox = data.gotox;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingGotoXLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.gotox = cbData.hist->extraData.toInt();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyUndoSettingGotoYLevel(WldScene::CallbackData cbData, WorldLevels data)
{
    ((ItemLevel*)cbData.item)->levelData.gotoy = data.gotoy;
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::historyRedoSettingGotoYLevel(WldScene::CallbackData cbData, WorldLevels /*data*/)
{
    ((ItemLevel*)cbData.item)->levelData.gotoy = cbData.hist->extraData.toInt();
    ((ItemLevel*)cbData.item)->arrayApply();
}

void WldScene::findGraphicsItem(WorldData toFind, WldScene::HistoryOperation *operation, WldScene::CallbackData customData, WldScene::callBackWorldTiles clbTiles, WldScene::callBackWorldPaths clbPaths, WldScene::callBackWorldScenery clbScenery, WldScene::callBackWorldLevels clbLevels, callBackWorldMusicbox clbMusic, bool ignoreTiles, bool ignorePaths, bool ignoreScenery, bool ignoreLevels, bool ignoreMusicbox)
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
    QMap<int, WorldMusic> sortedMusic;
    if(!ignoreMusicbox){
        foreach (WorldMusic music, toFind.music)
        {
            sortedMusic[music.array_id] = music;
        }
    }

    CallbackData cbData = customData;
    cbData.hist = operation;
    QMap<int, QGraphicsItem*> sortedGraphTiles;
    QMap<int, QGraphicsItem*> sortedGraphPath;
    QMap<int, QGraphicsItem*> sortedGraphScenery;
    QMap<int, QGraphicsItem*> sortedGraphLevels;
    QMap<int, QGraphicsItem*> sortedGraphMusic;
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
        else
        if(unsortedItem->data(0).toString()=="MUSICBOX")
        {
            if(!ignoreLevels){
                sortedGraphMusic[unsortedItem->data(2).toInt()] = unsortedItem;
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

    if(!ignoreMusicbox){
        foreach (QGraphicsItem* item, sortedGraphMusic)
        {

            if(sortedMusic.size()!=0)
            {
                QMap<int, WorldMusic>::iterator beginItem = sortedMusic.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()>currentArrayId)
                {
                    //not found
                    sortedMusic.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedMusic.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(2).toInt()==currentArrayId)
                {
                    cbData.item = item;
                    (this->*clbMusic)(cbData,(*beginItem));
                    sortedMusic.erase(beginItem);
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
    }else if(!data->music.isEmpty()){
        baseX = (int)data->music[0].x;
        baseY = (int)data->music[0].y;
    }

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
    foreach (WorldMusic music, data->music) {
        if((int)music.x<baseX){
            baseX = (int)music.x;
        }
        if((int)music.y<baseY){
            baseY = (int)music.y;
        }
    }



    return QPoint(baseX, baseY);
}

QString WldScene::getHistoryText(WldScene::HistoryOperation operation)
{
    switch (operation.type) {
    case HistoryOperation::WORLDHISTORY_REMOVE: return tr("Remove");
    case HistoryOperation::WORLDHISTORY_PLACE: return tr("Place");
    case HistoryOperation::WORLDHISTORY_OVERWRITE: return tr("Place & Overwrite");
    case HistoryOperation::WORLDHISTORY_MOVE: return tr("Move");
    case HistoryOperation::WORLDHISTORY_CHANGEDSETTINGSWORLD: return tr("Changed Worldsetting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    case HistoryOperation::WORLDHISTORY_CHANGEDSETTINGSWORLDITEM: return tr("Changed Itemsetting [%1]").arg(getHistorySettingText((SettingSubType)operation.subtype));
    default:
        return tr("Unknown");
    }
}

QString WldScene::getHistorySettingText(WldScene::SettingSubType subType)
{
    switch (subType) {
    case SETTING_ALWAYSVISIBLE: return tr("Always Visible");
    case SETTING_BIGPATHBACKGROUND: return tr("Big Path Background");
    case SETTING_CHARACTER: return tr("Character");
    case SETTING_DOORID: return tr("Door ID");
    case SETTING_GAMESTARTPOINT: return tr("Game start point");
    case SETTING_GOTOX: return tr("Goto X");
    case SETTING_GOTOY: return tr("Goto Y");
    case SETTING_HUB: return tr("Hub styled world");
    case SETTING_INTROLEVEL: return tr("Intro Level");
    case SETTING_LEVELFILE: return tr("Level file");
    case SETTING_LEVELTITLE: return tr("Level title");
    case SETTING_PATHBACKGROUND: return tr("Path Background");
    case SETTING_PATHBYBOTTOM: return tr("Exit at bottom");
    case SETTING_PATHBYLEFT: return tr("Exit at left");
    case SETTING_PATHBYRIGHT: return tr("Exit at right");
    case SETTING_PATHBYTOP: return tr("Exit at top");
    case SETTING_RESTARTAFTERFAIL: return tr("Restart after fail");
    case SETTING_TOTALSTARS: return tr("Total stars");
    default:
        return tr("Unknown");
    }
}
