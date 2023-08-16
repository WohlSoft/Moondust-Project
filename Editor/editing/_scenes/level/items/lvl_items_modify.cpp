/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/grid.h>
#include <common_features/item_rectangles.h>
#include <editing/edit_level/level_edit.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include <PGE_File_Formats/file_formats.h>
#include <main_window/dock/lvl_warp_props.h>

#include "../lvl_history_manager.h"

void LvlScene::applyArrayForItemGroup(QList<QGraphicsItem * >items)
{
    foreach(QGraphicsItem *it, items)
    {
        if(it) applyArrayForItem(it);
    }
}

///
/// \brief LvlScene::applyArrayForItem
/// \param item
/// Register all changes of item into the main data arrays
///
void LvlScene::applyArrayForItem(QGraphicsItem *item)
{
    if(!item) return;

    if(item->data(ITEM_IS_ITEM).toInt() == 1)
        dynamic_cast<LvlBaseItem *>(item)->arrayApply();
}


void LvlScene::returnItemBackGroup(QList<QGraphicsItem * >items)
{
    foreach(QGraphicsItem *it, items)
    {
        if(it) returnItemBack(it);
    }
}

///
/// \brief LvlScene::returnItemBackGroup
/// \param items
/// Returns moved item back without modify of settings
///
void LvlScene::returnItemBack(QGraphicsItem *item)
{
    if(!item) return;

    if(item->data(ITEM_IS_ITEM).toInt() == 1)
        dynamic_cast<LvlBaseItem *>(item)->returnBack();
}




// ////////////////////Sync settings of warp points with opened warp's settings/////////////////////////
///
/// \brief LvlScene::doorPointsSync
/// \param arrayID        Array ID of warp entry which is a key for found items on the map
/// \param remove         Remove warp points from the map because warp entry will be removed
///
void LvlScene::doorPointsSync(long arrayID, bool remove)
{

    bool doorExist = false;
    bool doorEntranceSynced = false;
    bool doorExitSynced = false;

    int i = 0;
    //find doorItem in array
    for(i = 0; i < m_data->doors.size(); i++)
    {
        if(m_data->doors[i].meta.array_id == (unsigned int)arrayID)
        {
            doorExist = true;
            break;
        }
    }
    if(!doorExist) return;

    //get ItemList
    QList<QGraphicsItem * > items = this->items();

    for(QGraphicsItem *item : items)
    {
        if((!m_data->doors[i].isSetIn) && (!m_data->doors[i].isSetOut)) break;   //Don't sync door points if not placed

        if((item->data(ITEM_TYPE).toString() == "Door_enter") && (item->data(ITEM_ARRAY_ID).toInt() == arrayID))
        {
            if((!(((!m_data->doors[i].lvl_o) && (!m_data->doors[i].lvl_i)) ||
                  ((m_data->doors[i].lvl_o) && (!m_data->doors[i].lvl_i)))
               ) || (remove))
            {
                ItemDoor *d = dynamic_cast<ItemDoor *>(item);
                d->m_data = m_data->doors[i];
                d->removeFromArray();
                delete d;
                doorEntranceSynced = true;

            }
            else
            {
                m_data->doors[i].isSetIn = true;
                ItemDoor *d = dynamic_cast<ItemDoor *>(item);
                d->m_data = m_data->doors[i];
                d->refreshArrows();
                doorEntranceSynced = true;
            }
        }

        if((item->data(ITEM_TYPE).toString() == "Door_exit") && (item->data(ITEM_ARRAY_ID).toInt() == arrayID))
        {
            if((!(((!m_data->doors[i].lvl_o) && (!m_data->doors[i].lvl_i)) ||
                  (m_data->doors[i].lvl_i))
               ) || (remove))
            {
                ItemDoor *d = dynamic_cast<ItemDoor *>(item);
                d->m_data = m_data->doors[i];
                d->removeFromArray();
                delete d;
                doorExitSynced = true;
            }
            else
            {
                m_data->doors[i].isSetOut = true;
                ItemDoor *d = dynamic_cast<ItemDoor *>(item);
                d->m_data = m_data->doors[i];
                d->refreshArrows();
                doorExitSynced = true;
            }
        }
        if((doorEntranceSynced) && (doorExitSynced)) break; // stop fetch, because door points was synced
    }
}

///
/// \brief LvlScene::collectDataFromItem
/// \param dataToStore The data object where the new data is getting added
/// \param item The source item where the item has to be found
///
/// Copys the data obj from a item and adds it to the LevelData
///
void LvlScene::collectDataFromItem(LevelData &dataToStore, QGraphicsItem *item)
{
    if(!item) return;

    QString ObjType = item->data(ITEM_TYPE).toString();
    if(ObjType == "NPC")
        dataToStore.npc << dynamic_cast<ItemNPC *>(item)->m_data;
    else if(ObjType == "Block")
        dataToStore.blocks << dynamic_cast<ItemBlock *>(item)->m_data;
    else if(ObjType == "BGO")
        dataToStore.bgo << dynamic_cast<ItemBGO *>(item)->m_data;
    else if(ObjType == "Water")
        dataToStore.physez << dynamic_cast<ItemPhysEnv *>(item)->m_data;
    else if((ObjType == "Door_enter") || (ObjType == "Door_exit"))
        dataToStore.doors << dynamic_cast<ItemDoor *>(item)->m_data;
    else if(ObjType == "playerPoint")
        dataToStore.players << dynamic_cast<ItemPlayerPoint *>(item)->m_data;
}

void LvlScene::collectDataFromItems(LevelData &dataToStore, QList<QGraphicsItem *> items)
{
    for(QGraphicsItem *item : items)
        collectDataFromItem(dataToStore, item);
}

void LvlScene::placeAll(const LevelData &data)
{
    bool hasToUpdateDoorData = false;

    for(LevelBlock block : data.blocks)
    {
        //place them back
        m_data->blocks.push_back(block);
        placeBlock(block);

    }

    for(LevelBGO bgo : data.bgo)
    {
        //place them back
        m_data->bgo.push_back(bgo);
        placeBGO(bgo);

    }

    for(LevelNPC npc : data.npc)
    {
        //place them back
        m_data->npc.push_back(npc);
        placeNPC(npc, false);
    }

    for(LevelPhysEnv water : data.physez)
    {
        //place them back
        m_data->physez.push_back(water);
        placeEnvironmentZone(water);
    }

    for(LevelDoor door : data.doors)
    {
        LevelDoor originalDoor;
        bool found = false;
        foreach(LevelDoor findDoor, m_data->doors)
        {
            if(door.meta.array_id == findDoor.meta.array_id)
            {
                originalDoor = findDoor;
                found = true;
                break;
            }
        }
        if(!found)
            break;

        if(door.isSetIn && !door.isSetOut)
        {
            originalDoor.ix = door.ix;
            originalDoor.iy = door.iy;
            originalDoor.isSetIn = true;
            placeDoorEnter(originalDoor, false, false);
        }
        else if(!door.isSetIn && door.isSetOut)
        {
            originalDoor.ox = door.ox;
            originalDoor.oy = door.oy;
            originalDoor.isSetOut = true;
            placeDoorExit(originalDoor, false, false);
        }
        hasToUpdateDoorData = true;
    }

    for(PlayerPoint plr : data.players)
        placePlayerPoint(plr);

    if(hasToUpdateDoorData)
        m_mw->dock_LvlWarpProps->setDoorData(-2);


    //refresh Animation control
    if(m_opts.animationEnabled) stopAnimation();
    if(m_opts.animationEnabled) startAnimation();
}




///
/// \brief LvlScene::placeItemsByRectArray
///
/// Convert array of yellow rectangles into target items
///
void LvlScene::placeItemsByRectArray()
{
    //This function placing items by yellow rectangles
    if(item_rectangles::rectArray.isEmpty()) return;

    QGraphicsItem *backup = m_cursorItemImg;
    while(!item_rectangles::rectArray.isEmpty())
    {
        m_cursorItemImg = item_rectangles::rectArray.first();
        item_rectangles::rectArray.pop_front();

        foreach(dataFlag flag, LvlPlacingItems::flags)
            m_cursorItemImg->setData(flag.first, flag.second);

        placeItemUnderCursor();

        if(m_cursorItemImg) delete m_cursorItemImg;
    }
    m_cursorItemImg = backup;
    m_cursorItemImg->hide();

    if(!m_overwritedItems.blocks.isEmpty() ||
       !m_overwritedItems.bgo.isEmpty() ||
       !m_overwritedItems.npc.isEmpty())
    {
        m_history->addOverwrite(m_overwritedItems, m_placingItems);
        m_overwritedItems.blocks.clear();
        m_overwritedItems.bgo.clear();
        m_overwritedItems.npc.clear();
        m_placingItems.blocks.clear();
        m_placingItems.bgo.clear();
        m_placingItems.npc.clear();
    }
    else if(!m_placingItems.blocks.isEmpty() ||
            !m_placingItems.bgo.isEmpty() ||
            !m_placingItems.npc.isEmpty())
    {
        m_history->addPlace(m_placingItems);
        m_placingItems.blocks.clear();
        m_placingItems.bgo.clear();
        m_placingItems.npc.clear();
    }

}


///
/// \brief LvlScene::placeItemUnderCursor
///
/// Place item on the position of abstract item
///
void LvlScene::placeItemUnderCursor()
{
    bool wasPlaced = false;

    if(LvlPlacingItems::overwriteMode)
    {
        //remove all colliaded items before placing
        QGraphicsItem *xxx;
        while((xxx = itemCollidesWith(m_cursorItemImg)) != nullptr)
        {
            bool removed = false;
            if(xxx->data(ITEM_TYPE).toString() == "Block")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong() > m_lastBlockArrayID) break;
                m_overwritedItems.blocks.push_back(dynamic_cast<ItemBlock *>(xxx)->m_data);
                dynamic_cast<ItemBlock *>(xxx)->removeFromArray();
                delete xxx;
                removed = true;
            }
            else if(xxx->data(ITEM_TYPE).toString() == "BGO")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong() > m_lastBgoArrayID) break;
                m_overwritedItems.bgo.push_back(dynamic_cast<ItemBGO *>(xxx)->m_data);
                dynamic_cast<ItemBGO *>(xxx)->removeFromArray();
                delete xxx;
                removed = true;
            }
            else if(xxx->data(ITEM_TYPE).toString() == "NPC")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong() > m_lastNpcArrayID) break;
                m_overwritedItems.npc.push_back(dynamic_cast<ItemNPC *>(xxx)->m_data);
                dynamic_cast<ItemNPC *>(xxx)->removeFromArray();
                delete xxx;
                removed = true;
            }

            if(removed) //Remove pointer of deleted item
            {
                if(collisionCheckBuffer.contains(xxx))
                    collisionCheckBuffer.removeAt(collisionCheckBuffer.indexOf(xxx));
            }
        }
    }


    // Verify, is possible to place any objects

    if(m_keyCtrlPressed)
    {
        // Collide with new items only
        PGE_ItemList newItems;
        PGE_ItemList foundItems;
        itemCollidesWith(m_cursorItemImg, nullptr, &foundItems);

        for(auto *xxx : foundItems)
        {
            if(xxx->data(ITEM_TYPE).toString() == "Block")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong() > m_lastBlockArrayID)
                    newItems.push_back(xxx);
            }
            else if(xxx->data(ITEM_TYPE).toString() == "BGO")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong() > m_lastBgoArrayID)
                    newItems.push_back(xxx);
            }
            else if(xxx->data(ITEM_TYPE).toString() == "NPC")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong() > m_lastNpcArrayID)
                    newItems.push_back(xxx);
            }
        }

        if(itemCollidesWith(m_cursorItemImg, &newItems))
            return; // Don't place over recently placed objects
    }
    else if(itemCollidesWith(m_cursorItemImg))
    {
        // Can't place object
        return;
    }


    // Perform the placing itself


    if(m_placingItemType == PLC_Block)
    {
        LvlPlacingItems::blockSet.x = m_cursorItemImg->scenePos().x();
        LvlPlacingItems::blockSet.y = m_cursorItemImg->scenePos().y();

        m_data->blocks_array_id++;
        LvlPlacingItems::blockSet.meta.array_id = m_data->blocks_array_id;

        m_data->blocks.push_back(LvlPlacingItems::blockSet);
        placeBlock(LvlPlacingItems::blockSet, true);
        m_placingItems.blocks.push_back(LvlPlacingItems::blockSet);
        wasPlaced = true;
    }
    else if(m_placingItemType == PLC_BGO)
    {
        LvlPlacingItems::bgoSet.x = m_cursorItemImg->scenePos().x();
        LvlPlacingItems::bgoSet.y = m_cursorItemImg->scenePos().y();

        m_data->bgo_array_id++;
        LvlPlacingItems::bgoSet.meta.array_id = m_data->bgo_array_id;

        m_data->bgo.push_back(LvlPlacingItems::bgoSet);
        placeBGO(LvlPlacingItems::bgoSet, true);
        m_placingItems.bgo.push_back(LvlPlacingItems::bgoSet);
        wasPlaced = true;
    }
    else if(m_placingItemType == PLC_NPC)
    {
        LvlPlacingItems::npcSet.x = m_cursorItemImg->scenePos().x();
        LvlPlacingItems::npcSet.y = m_cursorItemImg->scenePos().y();

        if(LvlPlacingItems::npcSpecialAutoIncrement)
        {
            LvlPlacingItems::npcSet.special_data = m_IncrementingNpcSpecialSpin;
            m_IncrementingNpcSpecialSpin++;
        }

        m_data->npc_array_id++;
        LvlPlacingItems::npcSet.meta.array_id = m_data->npc_array_id;

        m_data->npc.push_back(LvlPlacingItems::npcSet);

        placeNPC(LvlPlacingItems::npcSet, !LvlPlacingItems::npcSet.generator);

        m_placingItems.npc.push_back(LvlPlacingItems::npcSet);
        wasPlaced = true;
    }
    else if(m_placingItemType == PLC_PlayerPoint)
    {

        bool found = true;
        QList<QVariant> oData;
        PlayerPoint pnt = FileFormats::CreateLvlPlayerPoint(LvlPlacingItems::playerID + 1);
        //Check exists point on map
        foreach(PlayerPoint ptr, m_data->players)
        {
            if(ptr.id == pnt.id)
            {
                found = true;
                pnt = ptr;
            }
        }

        if(!found)
        {
            pnt.id = 0;
            pnt.x = 0;
            pnt.y = 0;
            pnt.w = 0;
            pnt.h = 0;
        }

        oData.push_back(pnt.id);
        oData.push_back((qlonglong)pnt.x);
        oData.push_back((qlonglong)pnt.y);
        oData.push_back((qlonglong)pnt.w);
        oData.push_back((qlonglong)pnt.h);


        pnt = FileFormats::CreateLvlPlayerPoint(LvlPlacingItems::playerID + 1);
        pnt.x = m_cursorItemImg->scenePos().x();
        pnt.y = m_cursorItemImg->scenePos().y();

        placePlayerPoint(pnt);

        LogDebug(QString("Placing player point %1 with position %2 %3, %4")
                 .arg(LvlPlacingItems::playerID + 1)
                 .arg(m_cursorItemImg->scenePos().x())
                 .arg(m_cursorItemImg->scenePos().y())
                );

        m_history->addPlacePlayerPoint(pnt, QVariant(oData));
    }
    else if(m_placingItemType == PLC_Door)
    {
        foreach(LevelDoor door, m_data->doors)
        {
            if(door.meta.array_id == (unsigned int)LvlPlacingItems::doorArrayId)
            {
                if(LvlPlacingItems::doorType == LvlPlacingItems::DOOR_Entrance)
                {
                    if(!door.isSetIn)
                    {
                        door.ix = m_cursorItemImg->scenePos().x();
                        door.iy = m_cursorItemImg->scenePos().y();
                        if((door.lvl_i) || (door.lvl_o))
                        {
                            door.ox = door.ix;
                            door.oy = door.iy;
                        }
                        door.isSetIn = true;
                        m_history->addPlace(door, true);
                        placeDoorEnter(door, false, false);
                    }
                }
                else
                {
                    if(!door.isSetOut)
                    {
                        door.ox = m_cursorItemImg->scenePos().x();
                        door.oy = m_cursorItemImg->scenePos().y();
                        if((door.lvl_i) || (door.lvl_o))
                        {
                            door.ix = door.ox;
                            door.iy = door.oy;
                        }
                        door.isSetOut = true;
                        m_history->addPlace(door, false);
                        placeDoorExit(door, false, false);
                    }
                }
                m_mw->dock_LvlWarpProps->setDoorData(-2);
                break;
            }
        }

    }


    if(wasPlaced)
    {
        //Mark level as modified
        m_data->meta.modified = true;
    }
}












///
/// \brief LvlScene::removeItemUnderCursor
///
/// Remove item which collided with abstract item
///
void LvlScene::removeItemUnderCursor()
{
    if(m_contextMenuIsOpened) return;

    QGraphicsItem *findItem;
    //bool removeIt=true;
    findItem = itemCollidesCursor(m_cursorItemImg);
    removeLvlItem(findItem, true);
}


///
/// \brief LvlScene::removeSelectedLvlItems
///
/// Remove all currently selected items
///
void LvlScene::removeSelectedLvlItems()
{
    QList<QGraphicsItem *> selectedList = selectedItems();
    if(selectedList.isEmpty()) return;
    removeLvlItems(selectedList);
    Debugger_updateItemList();
}


///
/// \brief LvlScene::removeLvlItem
/// \param item
/// \param globalHistory
///
/// Remove single level item
///
void LvlScene::removeLvlItem(QGraphicsItem *item, bool globalHistory)
{
    if(!item) return;
    QList<QGraphicsItem * > items;
    items.push_back(item);
    removeLvlItems(items, globalHistory);
}

///
/// \brief LvlScene::removeLvlItems
/// \param items
/// \param globalHistory
///
/// Remove group of items
///
void LvlScene::removeLvlItems(QList<QGraphicsItem * > items, bool globalHistory, bool forceInvis)
{
    LevelData historyBuffer;
    bool deleted = false;
    QString objType;

    for(QList<QGraphicsItem *>::iterator it = items.begin(); it != items.end(); it++)
    {
        objType = (*it)->data(ITEM_TYPE).toString();

        if(!forceInvis && !(*it)->isVisible())
            continue;  //Invisible items can't be deleted

        //remove data from main array before deletion item from scene
        if(objType == "Block")
        {
            if((m_lockBlock) || (dynamic_cast<ItemBlock *>(*it)->m_locked)) continue;

            historyBuffer.blocks.push_back(dynamic_cast<ItemBlock *>(*it)->m_data);
            dynamic_cast<ItemBlock *>(*it)->removeFromArray();
            if((*it)) delete(*it);
            deleted = true;
        }
        else if(objType == "BGO")
        {
            if((m_lockBgo) || (dynamic_cast<ItemBGO *>(*it)->m_locked)) continue;

            historyBuffer.bgo.push_back(dynamic_cast<ItemBGO *>(*it)->m_data);
            dynamic_cast<ItemBGO *>(*it)->removeFromArray();
            if((*it)) delete(*it);
            deleted = true;
        }
        else if(objType == "NPC")
        {
            if((m_lockNpc) || (dynamic_cast<ItemNPC *>(*it)->m_locked)) continue;

            historyBuffer.npc.push_back(dynamic_cast<ItemNPC *>(*it)->m_data);
            dynamic_cast<ItemNPC *>(*it)->removeFromArray();
            if((*it)) delete(*it);
            deleted = true;
        }
        else if(objType == "Water")
        {
            if((m_lockPhysenv) || (dynamic_cast<ItemPhysEnv *>(*it)->m_locked)) continue;

            historyBuffer.physez.push_back(dynamic_cast<ItemPhysEnv *>(*it)->m_data);
            dynamic_cast<ItemPhysEnv *>(*it)->removeFromArray();
            if((*it)) delete(*it);
            deleted = true;
        }
        else if((objType == "Door_enter") || (objType == "Door_exit"))
        {
            if((m_lockDoor) || (dynamic_cast<ItemDoor *>(*it)->m_locked)) continue;

            bool isEntrance = (objType == "Door_enter");
            LevelDoor doorData = dynamic_cast<ItemDoor *>(*it)->m_data;
            if(isEntrance)
            {
                doorData.isSetIn = true;
                doorData.isSetOut = false;
            }
            else
            {
                doorData.isSetIn = false;
                doorData.isSetOut = true;
            }
            historyBuffer.doors.push_back(doorData);
            dynamic_cast<ItemDoor *>(*it)->removeFromArray();
            if((*it)) delete(*it);
            m_mw->dock_LvlWarpProps->setDoorData(-2);
            deleted = true;
        }
        else if(objType == "playerPoint")
        {
            historyBuffer.players.push_back(dynamic_cast<ItemPlayerPoint *>(*it)->m_data);
            dynamic_cast<ItemPlayerPoint *>(*it)->removeFromArray();
            if((*it)) delete(*it);
            deleted = true;
        }
    }

    if(deleted)
    {
        if(globalHistory)
        {
            m_overwritedItems.blocks << historyBuffer.blocks;
            m_overwritedItems.bgo << historyBuffer.bgo;
            m_overwritedItems.npc << historyBuffer.npc;
            m_overwritedItems.physez << historyBuffer.physez;
            m_overwritedItems.doors << historyBuffer.doors;
            m_overwritedItems.players << historyBuffer.players;
        }
        else
            m_history->addRemove(historyBuffer);
        //Mark level as modified
        m_data->meta.modified = true;
    }
}


