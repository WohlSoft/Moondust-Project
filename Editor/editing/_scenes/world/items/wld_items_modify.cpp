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

#include <common_features/grid.h>
#include <common_features/main_window_ptr.h>
#include <common_features/item_rectangles.h>
#include <editing/edit_world/world_edit.h>
#include <editing/_scenes/world/wld_item_placing.h>

#include "../wld_scene.h"
#include "../wld_history_manager.h"

void WldScene::applyArrayForItemGroup(QList<QGraphicsItem * >items)
{
    foreach(QGraphicsItem * it, items)
    {
        if(it) applyArrayForItem(it);
    }
}

///
/// \brief WldScene::applyArrayForItem
/// \param item
/// Register all changes of item into the main data arrays
///
void WldScene::applyArrayForItem(QGraphicsItem * item)
{
    if(!item) return;
    if(!item->data(ITEM_IS_ITEM).isNull())
    {
        dynamic_cast<WldBaseItem *>(item)->arrayApply();
    }
}

void WldScene::collectDataFromItem(WorldData &dataToStore, QGraphicsItem *item)
{
    if(!item) return;

    QString ObjType = item->data(ITEM_TYPE).toString();
    if( ObjType == "TILE")
    {
        dataToStore.tiles << dynamic_cast<ItemTile *>(item)->m_data;
    }
    else
    if( ObjType == "SCENERY")
    {
        dataToStore.scenery << dynamic_cast<ItemScene *>(item)->m_data;
    }
    else
    if( ObjType == "PATH")
    {
        dataToStore.paths << dynamic_cast<ItemPath *>(item)->m_data;
    }
    else
    if( ObjType == "LEVEL")
    {
        dataToStore.levels << dynamic_cast<ItemLevel *>(item)->m_data;
    }
    else
    if( ObjType == "MUSICBOX" )
    {
        dataToStore.music << dynamic_cast<ItemMusic *>(item)->m_data;
    }
}

void WldScene::collectDataFromItems(WorldData &dataToStore, QList<QGraphicsItem *> items)
{
    foreach (QGraphicsItem* item, items) {
        collectDataFromItem(dataToStore, item);
    }
}


void WldScene::returnItemBackGroup(QList<QGraphicsItem * >items)
{
    foreach(QGraphicsItem * it, items)
    {
        if(it) returnItemBack(it);
    }
}

void WldScene::returnItemBack(QGraphicsItem * item)
{
    if(!item) return;

    if(!item->data(ITEM_IS_ITEM).isNull())
    {
        dynamic_cast<WldBaseItem *>(item)->returnBack();
    }
}





void WldScene::placeItemsByRectArray()
{
    //using namespace wld_control;
    //This function placing items by yellow rectangles
    if(item_rectangles::rectArray.isEmpty()) return;

    QGraphicsItem * backup = m_cursorItemImg;
    while(!item_rectangles::rectArray.isEmpty())
    {
        m_cursorItemImg = item_rectangles::rectArray.first();
        item_rectangles::rectArray.pop_front();

        foreach(dataFlag_w flag, WldPlacingItems::flags)
            m_cursorItemImg->setData(flag.first, flag.second);

        placeItemUnderCursor();

        if(m_cursorItemImg) delete m_cursorItemImg;
    }
    m_cursorItemImg = backup;
    m_cursorItemImg->hide();

    if(!m_overwritedItems.tiles.isEmpty()||
        !m_overwritedItems.scenery.isEmpty()||
        !m_overwritedItems.paths.isEmpty()||
        !m_overwritedItems.levels.isEmpty()||
        !m_overwritedItems.music.isEmpty() )
    {
        m_history->addOverwriteHistory(m_overwritedItems, m_placingItems);
        m_overwritedItems.tiles.clear();
        m_overwritedItems.scenery.clear();
        m_overwritedItems.paths.clear();
        m_overwritedItems.levels.clear();
        m_overwritedItems.music.clear();
        m_placingItems.tiles.clear();
        m_placingItems.paths.clear();
        m_placingItems.scenery.clear();
        m_placingItems.levels.clear();
        m_placingItems.music.clear();
    }
    else
    if(!m_placingItems.tiles.isEmpty()||
            !m_placingItems.paths.isEmpty()||
            !m_placingItems.scenery.isEmpty()||
            !m_placingItems.levels.isEmpty()||
            !m_placingItems.music.isEmpty()){
        m_history->addPlaceHistory(m_placingItems);
        m_placingItems.tiles.clear();
        m_placingItems.paths.clear();
        m_placingItems.scenery.clear();
        m_placingItems.levels.clear();
        m_placingItems.music.clear();
    }

}

void WldScene::placeItemUnderCursor()
{
    //using namespace wld_control;
    bool wasPlaced=false;

    if(WldPlacingItems::overwriteMode)
    {   //remove all colliaded items before placing
        QGraphicsItem * xxx;
        while( (xxx=itemCollidesWith(m_cursorItemImg)) != NULL )
        {
            bool removed=false;
            if(xxx->data(ITEM_TYPE).toString()=="TILE")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong()>m_lastTerrainArrayID) break;
                m_overwritedItems.tiles.push_back( ((ItemTile *)xxx)->m_data );
                ((ItemTile *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(ITEM_TYPE).toString()=="SCENERY")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong()>m_lastSceneryArrayID) break;
                m_overwritedItems.scenery.push_back( ((ItemScene *)xxx)->m_data );
                ((ItemScene *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(ITEM_TYPE).toString()=="PATH")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong()>m_lastPathArrayID) break;
                m_overwritedItems.paths.push_back( ((ItemPath *)xxx)->m_data );
                ((ItemPath *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(ITEM_TYPE).toString()=="LEVEL")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong()>m_lastLevelArrayID) break;
                m_overwritedItems.levels.push_back( ((ItemLevel *)xxx)->m_data );
                ((ItemLevel *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(ITEM_TYPE).toString()=="MUSICBOX")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong()>m_lastMusicBoxArrayID) break;
                m_overwritedItems.music.push_back( ((ItemMusic *)xxx)->m_data );
                ((ItemMusic *)xxx)->removeFromArray();
                delete xxx; removed=true;
            }

            if(removed) //Remove pointer of deleted item
            {
                if(collisionCheckBuffer.contains(xxx))
                    collisionCheckBuffer.removeAt(collisionCheckBuffer.indexOf(xxx));
            }
        }
    }

//    QList<QGraphicsItem *> * checkZone;
//    if(collisionCheckBuffer.isEmpty())
//        checkZone = NULL;
//    else
//        checkZone = &collisionCheckBuffer;

    if( itemCollidesWith(m_cursorItemImg) )
    {
        return;
    }
    else
    {
        if(m_placingItemType == PLC_Terrain)
        {
            WldPlacingItems::terrainSet.x = m_cursorItemImg->scenePos().x();
            WldPlacingItems::terrainSet.y = m_cursorItemImg->scenePos().y();

            m_data->tile_array_id++;
            WldPlacingItems::terrainSet.meta.array_id = m_data->tile_array_id;

            m_data->tiles.push_back(WldPlacingItems::terrainSet);
            placeTile(WldPlacingItems::terrainSet, true);
            m_placingItems.tiles.push_back(WldPlacingItems::terrainSet);
            wasPlaced=true;
        }
        else
        if(m_placingItemType == PLC_Scene)
        {
            WldPlacingItems::sceneSet.x = m_cursorItemImg->scenePos().x();
            WldPlacingItems::sceneSet.y = m_cursorItemImg->scenePos().y();

            m_data->scene_array_id++;
            WldPlacingItems::sceneSet.meta.array_id = m_data->scene_array_id;

            m_data->scenery.push_back(WldPlacingItems::sceneSet);
            placeScenery(WldPlacingItems::sceneSet, true);
            m_placingItems.scenery.push_back(WldPlacingItems::sceneSet);
            wasPlaced=true;
        }
        else
        if(m_placingItemType == PLC_Path)
        {
            WldPlacingItems::pathSet.x = m_cursorItemImg->scenePos().x();
            WldPlacingItems::pathSet.y = m_cursorItemImg->scenePos().y();

            m_data->path_array_id++;
            WldPlacingItems::pathSet.meta.array_id = m_data->path_array_id;

            m_data->paths.push_back(WldPlacingItems::pathSet);
            placePath(WldPlacingItems::pathSet, true);
            m_placingItems.paths.push_back(WldPlacingItems::pathSet);
            wasPlaced=true;
        }
        else
        if(m_placingItemType == PLC_Level)
        {
            WldPlacingItems::levelSet.x = m_cursorItemImg->scenePos().x();
            WldPlacingItems::levelSet.y = m_cursorItemImg->scenePos().y();

            m_data->level_array_id++;
            WldPlacingItems::levelSet.meta.array_id = m_data->level_array_id;

            m_data->levels.push_back(WldPlacingItems::levelSet);
            placeLevel(WldPlacingItems::levelSet, true);
            m_placingItems.levels.push_back(WldPlacingItems::levelSet);
            wasPlaced=true;
        }
        else
        if(m_placingItemType == PLC_Musicbox)
        {
            WldPlacingItems::musicSet.x = m_cursorItemImg->scenePos().x();
            WldPlacingItems::musicSet.y = m_cursorItemImg->scenePos().y();

            m_data->musicbox_array_id++;
            WldPlacingItems::musicSet.meta.array_id = m_data->musicbox_array_id;

            m_data->music.push_back(WldPlacingItems::musicSet);
            placeMusicbox(WldPlacingItems::musicSet, true);
            m_placingItems.music.push_back(WldPlacingItems::musicSet);
            wasPlaced=true;
        }

    }
    if(wasPlaced)
    {
        //Mark world map as modified
        m_data->meta.modified = true;
    }
}












void WldScene::removeItemUnderCursor()
{
    if(m_contextMenuIsOpened) return;

    QGraphicsItem * findItem;
    findItem = itemCollidesCursor(m_cursorItemImg);
    removeWldItem(findItem, true);
}


void WldScene::removeSelectedWldItems()
{
    QList<QGraphicsItem*> selectedList = selectedItems();
    if(selectedList.isEmpty()) return;
    removeWldItems(selectedList);
    Debugger_updateItemList();
}

void WldScene::removeWldItem(QGraphicsItem * item, bool globalHistory)
{
    if(!item) return;
    QList<QGraphicsItem * > items;
    items.push_back(item);
    removeWldItems(items, globalHistory);
}

void WldScene::removeWldItems(QList<QGraphicsItem * > items, bool globalHistory)
{
    WorldData historyBuffer;
    bool deleted=false;
    QString objType;

    for (QList<QGraphicsItem*>::iterator it = items.begin(); it != items.end(); it++)
    {
            objType=(*it)->data(ITEM_TYPE).toString();

            if(!(*it)->isVisible()) continue;  //Invisible items can't be deleted

            //remove data from main array before deletion item from scene
            if( objType=="TILE" )
            {
                historyBuffer.tiles.push_back(((ItemTile *)(*it))->m_data);
                ((ItemTile *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="SCENERY" )
            {
                historyBuffer.scenery.push_back(((ItemScene*)(*it))->m_data);
                ((ItemScene *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="PATH" )
            {
                historyBuffer.paths.push_back(((ItemPath*)(*it))->m_data);
                ((ItemPath *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="LEVEL" )
            {
                historyBuffer.levels.push_back(((ItemLevel*)(*it))->m_data);
                ((ItemLevel *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="MUSICBOX" )
            {
                historyBuffer.music.push_back(((ItemMusic*)(*it))->m_data);
                ((ItemMusic *)(*it))->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }

    }

    if(deleted)
    {
        if(globalHistory)
        {
            m_overwritedItems.tiles << historyBuffer.tiles;
            m_overwritedItems.scenery << historyBuffer.scenery;
            m_overwritedItems.paths << historyBuffer.paths;
            m_overwritedItems.levels << historyBuffer.levels;
            m_overwritedItems.music << historyBuffer.music;
        }
        else
            m_history->addRemoveHistory(historyBuffer);

        //Mark world map as modified
        m_data->meta.modified = true;
    }
}

void WldScene::placeAll(const WorldData &data){
    foreach (WorldTerrainTile tile, data.tiles)
    {
        //place them back
        m_data->tiles.push_back(tile);
        placeTile(tile);
    }
    foreach (WorldPathTile path, data.paths)
    {
        //place them back
        m_data->paths.push_back(path);
        placePath(path);
    }
    foreach (WorldScenery scenery, data.scenery)
    {
        //place them back
        m_data->scenery.push_back(scenery);
        placeScenery(scenery);
    }
    foreach (WorldLevelTile level, data.levels)
    {
        //place them back
        m_data->levels.push_back(level);
        placeLevel(level);
    }
    foreach (WorldMusicBox music, data.music)
    {
        m_data->music.push_back(music);
        placeMusicbox(music);
    }

    //refresh Animation control
    if(m_opts.animationEnabled) stopAnimation();
    if(m_opts.animationEnabled) startAnimation();
}



