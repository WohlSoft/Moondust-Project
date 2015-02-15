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

#include <common_features/grid.h>
#include <common_features/mainwinconnect.h>
#include <common_features/item_rectangles.h>
#include <editing/edit_level/level_edit.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include <file_formats/file_formats.h>
#include <main_window/dock/lvl_warp_props.h>

#include "../lvl_scene.h"
#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "item_playerpoint.h"

void LvlScene::applyArrayForItemGroup(QList<QGraphicsItem * >items)
{
    foreach(QGraphicsItem * it, items)
    {
        if(it) applyArrayForItem(it);
    }
}

///
/// \brief LvlScene::applyArrayForItem
/// \param item
/// Register all changes of item into the main data arrays
///
void LvlScene::applyArrayForItem(QGraphicsItem * item)
{
    if(!item) return;

    if( item->data(ITEM_IS_ITEM).toInt() == 1 )
    {
        dynamic_cast<LvlBaseItem *>(item)->arrayApply();
    }
}


void LvlScene::returnItemBackGroup(QList<QGraphicsItem * >items)
{
    foreach(QGraphicsItem * it, items)
    {
        if(it) returnItemBack(it);
    }
}

///
/// \brief LvlScene::returnItemBackGroup
/// \param items
/// Returns moved item back without modify of settings
///
void LvlScene::returnItemBack(QGraphicsItem * item)
{
    if(!item) return;

    if( item->data(ITEM_IS_ITEM).toInt() == 1 )
    {
        dynamic_cast<LvlBaseItem *>(item)->returnBack();
    }
}




// ////////////////////Sync settings of warp points with opened warp's settings/////////////////////////
///
/// \brief LvlScene::doorPointsSync
/// \param arrayID        Array ID of warp entry which is a key for found items on the map
/// \param remove         Remove warp points from the map because warp entry will be removed
///
void LvlScene::doorPointsSync(long arrayID, bool remove)
{

    bool doorExist=false;
    bool doorEntranceSynced=false;
    bool doorExitSynced=false;

    int i=0;
    //find doorItem in array
    for(i=0; i<LvlData->doors.size(); i++)
    {
        if(LvlData->doors[i].array_id==(unsigned int)arrayID)
        {
            doorExist=true;
            break;
        }
    }
    if(!doorExist) return;

    //get ItemList
    QList<QGraphicsItem * > items = this->items();

    foreach(QGraphicsItem * item, items)
    {
        if((!LvlData->doors[i].isSetIn)&&(!LvlData->doors[i].isSetOut)) break; //Don't sync door points if not placed

        if((item->data(ITEM_TYPE).toString()=="Door_enter")&&(item->data(ITEM_ARRAY_ID).toInt()==arrayID))
        {
            if((! (((!LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)) ||
                   ((LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)))
                )||(remove))
            {
                dynamic_cast<ItemDoor *>(item)->doorData = LvlData->doors[i];
                dynamic_cast<ItemDoor *>(item)->removeFromArray();
                delete dynamic_cast<ItemDoor *>(item);
                doorEntranceSynced = true;
            }
            else
            {
                LvlData->doors[i].isSetIn=true;
                dynamic_cast<ItemDoor *>(item)->doorData = LvlData->doors[i];
                doorEntranceSynced = true;
            }
        }

        if((item->data(ITEM_TYPE).toString()=="Door_exit")&&(item->data(ITEM_ARRAY_ID).toInt()==arrayID))
        {
            if( (! (((!LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)) ||
                                      (LvlData->doors[i].lvl_i) ) )||(remove))
            {
                dynamic_cast<ItemDoor *>(item)->doorData = LvlData->doors[i];
                dynamic_cast<ItemDoor *>(item)->removeFromArray();
                delete dynamic_cast<ItemDoor *>(item);
                doorExitSynced = true;
            }
            else
            {
                LvlData->doors[i].isSetOut=true;
                dynamic_cast<ItemDoor *>(item)->doorData = LvlData->doors[i];
                doorExitSynced = true;
            }
        }
        if((doorEntranceSynced)&&(doorExitSynced)) break; // stop fetch, because door points was synced
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
    if( ObjType == "NPC")
    {
        dataToStore.npc << dynamic_cast<ItemNPC *>(item)->npcData;
    }
    else
    if( ObjType == "Block")
    {
        dataToStore.blocks << dynamic_cast<ItemBlock *>(item)->blockData;
    }
    else
    if( ObjType == "BGO")
    {
        dataToStore.bgo << dynamic_cast<ItemBGO *>(item)->bgoData;
    }
    else
    if( ObjType == "Water")
    {
        dataToStore.physez << dynamic_cast<ItemWater *>(item)->waterData;
    }
    else
    if(( ObjType == "Door_enter")||( ObjType == "Door_exit"))
    {
        dataToStore.doors << dynamic_cast<ItemDoor *>(item)->doorData;
    }
    else
    if( ObjType == "playerPoint" )
    {
        dataToStore.players << dynamic_cast<ItemPlayerPoint *>(item)->pointData;
    }
}

void LvlScene::placeAll(const LevelData &data)
{
    bool hasToUpdateDoorData = false;

    foreach (LevelBlock block, data.blocks)
    {
        //place them back
        LvlData->blocks.push_back(block);
        placeBlock(block);

    }

    foreach (LevelBGO bgo, data.bgo)
    {
        //place them back
        LvlData->bgo.push_back(bgo);
        placeBGO(bgo);

    }

    foreach (LevelNPC npc, data.npc)
    {
        //place them back
        LvlData->npc.push_back(npc);
        placeNPC(npc);

    }

    foreach (LevelPhysEnv water, data.physez)
    {
        //place them back
        LvlData->physez.push_back(water);
        placeWater(water);
    }

    foreach (LevelDoors door, data.doors)
    {
        LevelDoors originalDoor;
        bool found = false;
        foreach(LevelDoors findDoor, LvlData->doors){
            if(door.array_id == findDoor.array_id){
                originalDoor = findDoor;
                found = true;
                break;
            }
        }
        if(!found)
            break;

        if(door.isSetIn&&!door.isSetOut)
        {
            originalDoor.ix = door.ix;
            originalDoor.iy = door.iy;
            originalDoor.isSetIn = true;
            placeDoorEnter(originalDoor, false, false);
        }
        else
        if(!door.isSetIn&&door.isSetOut)
        {
            originalDoor.ox = door.ox;
            originalDoor.oy = door.oy;
            originalDoor.isSetOut = true;
            placeDoorExit(originalDoor, false, false);
        }
        hasToUpdateDoorData = true;
    }

    foreach(PlayerPoint plr, data.players)
    {
        placePlayerPoint(plr);
    }

    if(hasToUpdateDoorData)
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);


    //refresh Animation control
    if(opts.animationEnabled) stopAnimation();
    if(opts.animationEnabled) startAnimation();
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

    QGraphicsItem * backup = cursor;
    while(!item_rectangles::rectArray.isEmpty())
    {
        cursor = item_rectangles::rectArray.first();
        item_rectangles::rectArray.pop_front();

        foreach(dataFlag flag, LvlPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        placeItemUnderCursor();

        if(cursor) delete cursor;
    }
    cursor = backup;
    cursor->hide();

    if(!overwritedItems.blocks.isEmpty()||
        !overwritedItems.bgo.isEmpty()||
        !overwritedItems.npc.isEmpty() )
    {
        addOverwriteHistory(overwritedItems, placingItems);
        overwritedItems.blocks.clear();
        overwritedItems.bgo.clear();
        overwritedItems.npc.clear();
        placingItems.blocks.clear();
        placingItems.bgo.clear();
        placingItems.npc.clear();
    }
    else
    if(!placingItems.blocks.isEmpty()||
            !placingItems.bgo.isEmpty()||
            !placingItems.npc.isEmpty()){
        addPlaceHistory(placingItems);
        placingItems.blocks.clear();
        placingItems.bgo.clear();
        placingItems.npc.clear();
    }

}


///
/// \brief LvlScene::placeItemUnderCursor
///
/// Place item on the position of abstract item
///
void LvlScene::placeItemUnderCursor()
{
    bool wasPlaced=false;

    if(LvlPlacingItems::overwriteMode)
    {   //remove all colliaded items before placing
        QGraphicsItem * xxx;
        while( (xxx=itemCollidesWith(cursor)) != NULL )
        {
            bool removed=false;
            if(xxx->data(ITEM_TYPE).toString()=="Block")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong()>last_block_arrayID) break;
                overwritedItems.blocks.push_back( dynamic_cast<ItemBlock *>(xxx)->blockData );
                dynamic_cast<ItemBlock *>(xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(ITEM_TYPE).toString()=="BGO")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong()>last_bgo_arrayID) break;
                overwritedItems.bgo.push_back( dynamic_cast<ItemBGO *>(xxx)->bgoData );
                dynamic_cast<ItemBGO *>(xxx)->removeFromArray();
                delete xxx; removed=true;
            }
            else
            if(xxx->data(ITEM_TYPE).toString()=="NPC")
            {
                if(xxx->data(ITEM_ARRAY_ID).toLongLong()>last_npc_arrayID) break;
                overwritedItems.npc.push_back( dynamic_cast<ItemNPC *>(xxx)->npcData );
                dynamic_cast<ItemNPC *>(xxx)->removeFromArray();
                delete xxx; removed=true;
            }

            if(removed) //Remove pointer of deleted item
            {
                if(collisionCheckBuffer.contains(xxx))
                    collisionCheckBuffer.removeAt(collisionCheckBuffer.indexOf(xxx));
            }
        }
    }

    QList<QGraphicsItem *> * checkZone;
    if(collisionCheckBuffer.isEmpty())
        checkZone = 0;
    else
        checkZone = &collisionCheckBuffer;

    if( !emptyCollisionCheck && itemCollidesWith(cursor, checkZone) )
    {
        return;
    }
    else
    {
        if(placingItem == PLC_Block)
        {
            LvlPlacingItems::blockSet.x = cursor->scenePos().x();
            LvlPlacingItems::blockSet.y = cursor->scenePos().y();

            LvlData->blocks_array_id++;
            LvlPlacingItems::blockSet.array_id = LvlData->blocks_array_id;

            LvlData->blocks.push_back(LvlPlacingItems::blockSet);
            placeBlock(LvlPlacingItems::blockSet, true);
            placingItems.blocks.push_back(LvlPlacingItems::blockSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_BGO)
        {
            LvlPlacingItems::bgoSet.x = cursor->scenePos().x();
            LvlPlacingItems::bgoSet.y = cursor->scenePos().y();

            LvlData->bgo_array_id++;
            LvlPlacingItems::bgoSet.array_id = LvlData->bgo_array_id;

            LvlData->bgo.push_back(LvlPlacingItems::bgoSet);
            placeBGO(LvlPlacingItems::bgoSet, true);
            placingItems.bgo.push_back(LvlPlacingItems::bgoSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_NPC)
        {
            LvlPlacingItems::npcSet.x = cursor->scenePos().x();
            LvlPlacingItems::npcSet.y = cursor->scenePos().y();

            if(LvlPlacingItems::npcSpecialAutoIncrement)
            {
                LvlPlacingItems::npcSet.special_data = IncrementingNpcSpecialSpin;
                IncrementingNpcSpecialSpin++;
            }

            LvlData->npc_array_id++;
            LvlPlacingItems::npcSet.array_id = LvlData->npc_array_id;

            LvlData->npc.push_back(LvlPlacingItems::npcSet);

            placeNPC(LvlPlacingItems::npcSet, !LvlPlacingItems::npcSet.generator);

            placingItems.npc.push_back(LvlPlacingItems::npcSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_PlayerPoint)
        {

                bool found=true;
                QList<QVariant> oData;
                PlayerPoint pnt = FileFormats::dummyLvlPlayerPoint(LvlPlacingItems::playerID+1);
                //Check exists point on map
                foreach(PlayerPoint ptr, LvlData->players)
                {
                    if(ptr.id == pnt.id)
                    {
                        found=true;
                        pnt=ptr;
                    }
                }

                if(!found)
                {
                    pnt.id=0;pnt.x=0;pnt.y=0;pnt.w=0;pnt.h=0;
                }

                oData.push_back(pnt.id);
                oData.push_back((qlonglong)pnt.x);
                oData.push_back((qlonglong)pnt.y);
                oData.push_back((qlonglong)pnt.w);
                oData.push_back((qlonglong)pnt.h);


                pnt = FileFormats::dummyLvlPlayerPoint(LvlPlacingItems::playerID+1);
                pnt.x = cursor->scenePos().x();
                pnt.y = cursor->scenePos().y();

                placePlayerPoint(pnt);

                WriteToLog(QtDebugMsg, QString("Placing player point %1 with position %2 %3, %4")
                           .arg(LvlPlacingItems::playerID+1)
                           .arg(cursor->scenePos().x())
                           .arg(cursor->scenePos().y())
                           );

            addPlacePlayerPointHistory(pnt, QVariant(oData));
        }
        else
        if(placingItem == PLC_Door)
        {
            foreach(LevelDoors door, LvlData->doors)
            {
             if(door.array_id == (unsigned int)LvlPlacingItems::doorArrayId)
             {
                if(LvlPlacingItems::doorType==LvlPlacingItems::DOOR_Entrance)
                {
                    if(!door.isSetIn)
                    {
                        door.ix = cursor->scenePos().x();
                        door.iy = cursor->scenePos().y();
                        if((door.lvl_i)||(door.lvl_o))
                        {
                            door.ox = door.ix;
                            door.oy = door.iy;
                        }
                        door.isSetIn=true;
                        addPlaceDoorHistory(door, true);
                        placeDoorEnter(door, false, false);
                    }
                }
                else
                {
                    if(!door.isSetOut)
                    {
                        door.ox = cursor->scenePos().x();
                        door.oy = cursor->scenePos().y();
                        if((door.lvl_i)||(door.lvl_o))
                        {
                            door.ix = door.ox;
                            door.iy = door.oy;
                        }
                        door.isSetOut=true;
                        addPlaceDoorHistory(door, false);
                        placeDoorExit(door, false, false);
                    }
                }
                MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
                break;
             }
            }

        }
    }
    if(wasPlaced)
    {
        LvlData->modified = true;
    }
}












///
/// \brief LvlScene::removeItemUnderCursor
///
/// Remove item which collided with abstract item
///
void LvlScene::removeItemUnderCursor()
{
    if(contextMenuOpened) return;

    QGraphicsItem * findItem;
    //bool removeIt=true;
    findItem = itemCollidesCursor(cursor);
    removeLvlItem(findItem, true);
}


///
/// \brief LvlScene::removeSelectedLvlItems
///
/// Remove all currently selected items
///
void LvlScene::removeSelectedLvlItems()
{
    QList<QGraphicsItem*> selectedList = selectedItems();
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
void LvlScene::removeLvlItem(QGraphicsItem * item, bool globalHistory)
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
void LvlScene::removeLvlItems(QList<QGraphicsItem * > items, bool globalHistory)
{
    LevelData historyBuffer;
    bool deleted=false;
    QString objType;

    for (QList<QGraphicsItem*>::iterator it = items.begin(); it != items.end(); it++)
    {
            objType=(*it)->data(ITEM_TYPE).toString();

            if(!(*it)->isVisible()) continue;  //Invisible items can't be deleted

            //remove data from main array before deletion item from scene
            if( objType=="Block" )
            {
                if((lock_block)|| (dynamic_cast<ItemBlock *>(*it)->isLocked) ) continue;

                historyBuffer.blocks.push_back(dynamic_cast<ItemBlock *>(*it)->blockData);
                dynamic_cast<ItemBlock *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="BGO" )
            {
                if((lock_bgo)|| (dynamic_cast<ItemBGO *>(*it)->isLocked) ) continue;

                historyBuffer.bgo.push_back(dynamic_cast<ItemBGO *>(*it)->bgoData);
                dynamic_cast<ItemBGO *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="NPC" )
            {
                if((lock_npc)|| (dynamic_cast<ItemNPC *>(*it)->isLocked) ) continue;

                historyBuffer.npc.push_back(dynamic_cast<ItemNPC *>(*it)->npcData);
                dynamic_cast<ItemNPC *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if( objType=="Water" )
            {
                if((lock_water)|| (dynamic_cast<ItemWater *>(*it)->isLocked) ) continue;

                historyBuffer.physez.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                dynamic_cast<ItemWater *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                deleted=true;
            }
            else
            if(( objType=="Door_enter" )||( objType=="Door_exit" ))
            {
                if((lock_door)|| (dynamic_cast<ItemDoor *>(*it)->isLocked) ) continue;

                bool isEntrance = (objType=="Door_enter");
                LevelDoors doorData = dynamic_cast<ItemDoor *>(*it)->doorData;
                if(isEntrance){
                    doorData.isSetIn = true;
                    doorData.isSetOut = false;
                }else{
                    doorData.isSetIn = false;
                    doorData.isSetOut = true;
                }
                historyBuffer.doors.push_back(doorData);
                dynamic_cast<ItemDoor *>(*it)->removeFromArray();
                if((*it)) delete (*it);
                MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);
                deleted=true;
            }
            else
            if( objType=="playerPoint" )
            {
                 historyBuffer.players.push_back(dynamic_cast<ItemPlayerPoint *>(*it)->pointData);
                 dynamic_cast<ItemPlayerPoint *>(*it)->removeFromArray();
                 if((*it)) delete (*it);
                 deleted=true;
            }
    }

    if(deleted)
    {
        if(globalHistory)
        {
            overwritedItems.blocks << historyBuffer.blocks;
            overwritedItems.bgo << historyBuffer.bgo;
            overwritedItems.npc << historyBuffer.npc;
            overwritedItems.physez << historyBuffer.physez;
            overwritedItems.doors << historyBuffer.doors;
            overwritedItems.players << historyBuffer.players;
        }
        else
            addRemoveHistory(historyBuffer);
    }
}


