#include "itemsearcher.h"

#include <main_window/dock/lvl_warp_props.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_playerpoint.h>
#include <editing/_scenes/level/items/item_water.h>

#include <common_features/mainwinconnect.h>

ItemSearcher::ItemSearcher(QObject *parent) :
    QObject(parent)
{}

ItemSearcher::ItemSearcher(ItemTypes::itemTypesMultiSelectable typesToFind, QObject *parent) : QObject(parent)
{
    m_findFilter = typesToFind;
}

ItemTypes::itemTypesMultiSelectable ItemSearcher::findFilter() const
{
    return m_findFilter;
}

void ItemSearcher::setFindFilter(const ItemTypes::itemTypesMultiSelectable &findFilter)
{
    m_findFilter = findFilter;
}



void ItemSearcher::find(const LevelData &dataToFind, const QList<QGraphicsItem*> &allItems)
{
    QMap<int, LevelDoors> sortedEntranceDoors;
    QMap<int, LevelDoors> sortedExitDoors;
    if(m_findFilter & ItemTypes::LVL_S_Door){
        foreach (LevelDoors door, dataToFind.doors) {
            if(door.isSetIn&&!door.isSetOut){
                sortedEntranceDoors[door.array_id] = door;
            }
        }
        foreach (LevelDoors door, dataToFind.doors) {
            if(!door.isSetIn&&door.isSetOut){
                sortedExitDoors[door.array_id] = door;
            }
        }
    }
    QMap<int, LevelBlock> sortedBlock;
    if(m_findFilter & ItemTypes::LVL_S_Block){
        foreach (LevelBlock block, dataToFind.blocks)
        {
            sortedBlock[block.array_id] = block;
        }
    }
    QMap<int, LevelBGO> sortedBGO;
    if(m_findFilter & ItemTypes::LVL_S_BGO){
        foreach (LevelBGO bgo, dataToFind.bgo)
        {
            sortedBGO[bgo.array_id] = bgo;
        }
    }
    QMap<int, LevelNPC> sortedNPC;
    if(m_findFilter & ItemTypes::LVL_S_NPC){
        foreach (LevelNPC npc, dataToFind.npc)
        {
            sortedNPC[npc.array_id] = npc;
        }
    }
    QMap<int, LevelPhysEnv> sortedWater;
    if(m_findFilter & ItemTypes::LVL_S_PhysEnv)
    {
        foreach (LevelPhysEnv water, dataToFind.physez) {
            sortedWater[water.array_id] = water;
        }
    }
    QMap<int, PlayerPoint> sortedPlayers;
    if(m_findFilter & ItemTypes::LVL_S_Player){
        foreach (PlayerPoint player, dataToFind.players) {
            sortedPlayers[player.id] = player;
        }
    }

    QMap<int, QGraphicsItem*> sortedGraphBlocks;
    QMap<int, QGraphicsItem*> sortedGraphBGO;
    QMap<int, QGraphicsItem*> sortedGraphNPC;
    QMap<int, QGraphicsItem*> sortedGraphWater;
    QMap<int, QGraphicsItem*> sortedGraphDoorEntrance;
    QMap<int, QGraphicsItem*> sortedGraphDoorExit;
    QMap<int, QGraphicsItem*> sortedGraphPlayers;
    foreach (QGraphicsItem* unsortedItem, allItems)
    {
        if(unsortedItem->data(ITEM_TYPE).toString()=="Block")
        {
            if(m_findFilter & ItemTypes::LVL_S_Block){
                sortedGraphBlocks[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="BGO")
        {
            if(m_findFilter & ItemTypes::LVL_S_BGO){
                sortedGraphBGO[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="NPC")
        {
            if(m_findFilter & ItemTypes::LVL_S_NPC){
                sortedGraphNPC[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Water")
        {
            if(m_findFilter & ItemTypes::LVL_S_PhysEnv){
                sortedGraphWater[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Door_enter")
        {
            if(m_findFilter & ItemTypes::LVL_S_Door){
                sortedGraphDoorEntrance[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }

        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="Door_exit")
        {
            if(m_findFilter & ItemTypes::LVL_S_Door){
                sortedGraphDoorExit[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="playerPoint")
        {
            if(m_findFilter & ItemTypes::LVL_S_Player){
                sortedGraphPlayers[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_Block){
        foreach (QGraphicsItem* item, sortedGraphBlocks)
        {

            if(sortedBlock.size()!=0)
            {
                QMap<int, LevelBlock>::iterator beginItem = sortedBlock.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedBlock.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedBlock.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundBlock(*beginItem, item);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_BGO)
    {
        foreach (QGraphicsItem* item, sortedGraphBGO)
        {
            if(sortedBGO.size()!=0)
            {
                QMap<int, LevelBGO>::iterator beginItem = sortedBGO.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedBGO.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedBGO.begin();
                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundBGO(*beginItem, item);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_NPC)
    {
        foreach (QGraphicsItem* item, sortedGraphNPC)
        {
            if(sortedNPC.size()!=0)
            {
                QMap<int, LevelNPC>::iterator beginItem = sortedNPC.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedNPC.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedNPC.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundNPC(*beginItem, item);
                }
            }
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_PhysEnv)
    {
        foreach (QGraphicsItem* item, sortedGraphWater)
        {
            if(sortedWater.size()!=0)
            {
                QMap<int, LevelPhysEnv>::iterator beginItem = sortedWater.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedWater.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedWater.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundPhysEnv(*beginItem, item);
                }
            }
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_Door)
    {
        foreach (QGraphicsItem* item, sortedGraphDoorEntrance)
        {
            if(sortedEntranceDoors.size()!=0)
            {
                QMap<int, LevelDoors>::iterator beginItem = sortedEntranceDoors.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedEntranceDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedEntranceDoors.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundDoor(*beginItem, item);
                }
            }
        }
        foreach (QGraphicsItem* item, sortedGraphDoorExit)
        {
            if(sortedExitDoors.size()!=0)
            {
                QMap<int, LevelDoors>::iterator beginItem = sortedExitDoors.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedExitDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedExitDoors.begin();

                currentArrayId = (*beginItem).array_id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundDoor(*beginItem, item);
                }
            }
        }
        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2); //update Door data
    }

    if(m_findFilter & ItemTypes::LVL_S_Player)
    {
        foreach (QGraphicsItem* item, sortedGraphPlayers)
        {
            if(sortedPlayers.size()!=0)
            {
                QMap<int, PlayerPoint>::iterator beginItem = sortedPlayers.begin();
                unsigned int currentArrayId = (*beginItem).id;

                //but still test if the next blocks, is the block we search!
                beginItem = sortedPlayers.begin();

                currentArrayId = (*beginItem).id;

                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundPlayerPoint(*beginItem, item);
                }
            }
        }
    }
}

void ItemSearcher::find(const WorldData &dataToFind, const QList<QGraphicsItem *> &allItems)
{
    QMap<int, WorldTiles> sortedTiles;
    if(m_findFilter & ItemTypes::WLD_S_Tile){
        foreach (WorldTiles tile, dataToFind.tiles)
        {
            sortedTiles[tile.array_id] = tile;
        }
    }
    QMap<int, WorldScenery> sortedScenery;
    if(m_findFilter & ItemTypes::WLD_S_Scenery){
        foreach (WorldScenery scenery, dataToFind.scenery)
        {
            sortedScenery[scenery.array_id] = scenery;
        }
    }
    QMap<int, WorldPaths> sortedPath;
    if(m_findFilter & ItemTypes::WLD_S_Path){
        foreach (WorldPaths path, dataToFind.paths)
        {
            sortedPath[path.array_id] = path;
        }
    }
    QMap<int, WorldLevels> sortedLevel;
    if(m_findFilter & ItemTypes::WLD_S_Level)
    {
        foreach (WorldLevels level, dataToFind.levels) {
            sortedLevel[level.array_id] = level;
        }
    }
    QMap<int, WorldMusic> sortedMusic;
    if(m_findFilter & ItemTypes::WLD_S_MusicBox){
        foreach (WorldMusic music, dataToFind.music) {
            sortedMusic[music.id] = music;
        }
    }

    QMap<int, QGraphicsItem*> sortedGraphTile;
    QMap<int, QGraphicsItem*> sortedGraphScenery;
    QMap<int, QGraphicsItem*> sortedGraphPath;
    QMap<int, QGraphicsItem*> sortedGraphLevel;
    QMap<int, QGraphicsItem*> sortedGraphMusicBox;
    foreach (QGraphicsItem* unsortedItem, allItems)
    {
        if(unsortedItem->data(ITEM_TYPE).toString()=="TILE")
        {
            if(m_findFilter & ItemTypes::WLD_S_Tile){
                sortedGraphTile[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="SCENERY")
        {
            if(m_findFilter & ItemTypes::WLD_S_Scenery){
                sortedGraphScenery[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="PATH")
        {
            if(m_findFilter & ItemTypes::WLD_S_Path){
                sortedGraphPath[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="LEVEL")
        {
            if(m_findFilter & ItemTypes::WLD_S_Level){
                sortedGraphLevel[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }
        }
        else
        if(unsortedItem->data(ITEM_TYPE).toString()=="MUSICBOX")
        {
            if(m_findFilter & ItemTypes::WLD_S_MusicBox){
                sortedGraphMusicBox[unsortedItem->data(ITEM_ARRAY_ID).toInt()] = unsortedItem;
            }

        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Tile){
        foreach (QGraphicsItem* item, sortedGraphTile)
        {

            if(sortedTiles.size()!=0)
            {
                QMap<int, WorldTiles>::iterator beginItem = sortedTiles.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedTiles.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedTiles.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundTile(*beginItem, item);
                }
            }
            else
            {
                break;
            }
        }
    }
    if(m_findFilter & ItemTypes::WLD_S_Path){
        foreach (QGraphicsItem* item, sortedGraphPath)
        {

            if(sortedPath.size()!=0)
            {
                QMap<int, WorldPaths>::iterator beginItem = sortedPath.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedPath.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedPath.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundPath(*beginItem, item);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Scenery){
        foreach (QGraphicsItem* item, sortedGraphScenery)
        {

            if(sortedScenery.size()!=0)
            {
                QMap<int, WorldScenery>::iterator beginItem = sortedScenery.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedScenery.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedScenery.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundScenery(*beginItem, item);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Level){
        foreach (QGraphicsItem* item, sortedGraphLevel)
        {

            if(sortedLevel.size()!=0)
            {
                QMap<int, WorldLevels>::iterator beginItem = sortedLevel.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedLevel.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedLevel.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundLevel(*beginItem, item);
                }
            }
            else
            {
                break;
            }
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_MusicBox){
        foreach (QGraphicsItem* item, sortedGraphMusicBox)
        {

            if(sortedMusic.size()!=0)
            {
                QMap<int, WorldMusic>::iterator beginItem = sortedMusic.begin();
                unsigned int currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()>currentArrayId)
                {
                    //not found
                    sortedMusic.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedMusic.begin();
                currentArrayId = (*beginItem).array_id;
                if((unsigned int)item->data(ITEM_ARRAY_ID).toInt()==currentArrayId)
                {
                    emit foundMusicbox(*beginItem, item);
                }
            }
            else
            {
                break;
            }
        }
    }

}
