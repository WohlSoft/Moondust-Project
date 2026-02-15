#include "itemsearcher.h"

#include <main_window/dock/lvl_warp_props.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_playerpoint.h>
#include <editing/_scenes/level/items/item_water.h>

#include <common_features/main_window_ptr.h>

ItemSearcher::ItemSearcher(QObject *parent) :
    QObject(parent)
{}

ItemSearcher::ItemSearcher(uint32_t typesToFind, QObject *parent) : QObject(parent)
{
    m_findFilter = typesToFind;
}

int ItemSearcher::findFilter() const
{
    return m_findFilter;
}

void ItemSearcher::setFindFilter(const int &findFilter)
{
    m_findFilter = findFilter;
}



void ItemSearcher::find(const LevelData &dataToFind, const QList<QGraphicsItem *> &allItems)
{
    QMap<int, LevelDoor> sortedEntranceDoors;
    QMap<int, LevelDoor> sortedExitDoors;
    if(m_findFilter & ItemTypes::LVL_S_Door)
    {
        foreach(LevelDoor door, dataToFind.doors)
        {
            if(door.isSetIn && !door.isSetOut)
                sortedEntranceDoors[door.meta.array_id] = door;
        }

        foreach(LevelDoor door, dataToFind.doors)
        {
            if(!door.isSetIn && door.isSetOut)
                sortedExitDoors[door.meta.array_id] = door;
        }
    }

    QMap<int, LevelBlock> sortedBlock;
    if(m_findFilter & ItemTypes::LVL_S_Block)
    {
        foreach(LevelBlock block, dataToFind.blocks)
            sortedBlock[block.meta.array_id] = block;
    }

    QMap<int, LevelBGO> sortedBGO;
    if(m_findFilter & ItemTypes::LVL_S_BGO)
    {
        foreach(LevelBGO bgo, dataToFind.bgo)
            sortedBGO[bgo.meta.array_id] = bgo;
    }

    QMap<int, LevelNPC> sortedNPC;
    if(m_findFilter & ItemTypes::LVL_S_NPC)
    {
        foreach(LevelNPC npc, dataToFind.npc)
            sortedNPC[npc.meta.array_id] = npc;
    }

    QMap<int, LevelPhysEnv> sortedWater;
    if(m_findFilter & ItemTypes::LVL_S_PhysEnv)
    {
        foreach(LevelPhysEnv water, dataToFind.physez)
            sortedWater[water.meta.array_id] = water;
    }

    QMap<int, PlayerPoint> sortedPlayers;
    if(m_findFilter & ItemTypes::LVL_S_Player)
    {
        foreach(PlayerPoint player, dataToFind.players)
            sortedPlayers[player.id] = player;
    }

    QMap<int, QGraphicsItem *> sortedGraphBlocks;
    QMap<int, QGraphicsItem *> sortedGraphBGO;
    QMap<int, QGraphicsItem *> sortedGraphNPC;
    QMap<int, QGraphicsItem *> sortedGraphWater;
    QMap<int, QGraphicsItem *> sortedGraphDoorEntrance;
    QMap<int, QGraphicsItem *> sortedGraphDoorExit;
    QMap<int, QGraphicsItem *> sortedGraphPlayers;

    foreach(QGraphicsItem *unsortedItem, allItems)
    {
        if(unsortedItem->data(LvlScene::ITEM_IS_ITEM).isNull() || !unsortedItem->data(LvlScene::ITEM_IS_ITEM).toBool())
            continue;

        int objType = unsortedItem->data(WldScene::ITEM_TYPE_INT).toInt();

        switch(objType)
        {
        case ItemTypes::LVL_Block:
            if(m_findFilter & ItemTypes::LVL_S_Block)
                sortedGraphBlocks[unsortedItem->data(LvlScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::LVL_BGO:
            if(m_findFilter & ItemTypes::LVL_S_BGO)
                sortedGraphBGO[unsortedItem->data(LvlScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::LVL_NPC:
            if(m_findFilter & ItemTypes::LVL_S_NPC)
                sortedGraphNPC[unsortedItem->data(LvlScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::LVL_PhysEnv:
            if(m_findFilter & ItemTypes::LVL_S_PhysEnv)
                sortedGraphWater[unsortedItem->data(LvlScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::LVL_META_DoorEnter:
            if(m_findFilter & ItemTypes::LVL_S_Door)
                sortedGraphDoorEntrance[unsortedItem->data(LvlScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::LVL_META_DoorExit:
            if(m_findFilter & ItemTypes::LVL_S_Door)
                sortedGraphDoorExit[unsortedItem->data(LvlScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::LVL_Player:
            if(m_findFilter & ItemTypes::LVL_S_Player)
                sortedGraphPlayers[unsortedItem->data(LvlScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        default:
            break;
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_Block)
    {
        foreach(QGraphicsItem *item, sortedGraphBlocks)
        {
            if(sortedBlock.size() != 0)
            {
                QMap<int, LevelBlock>::iterator beginItem = sortedBlock.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedBlock.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedBlock.begin();
                currentArrayId = (*beginItem).meta.array_id;
                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundBlock(*beginItem, item);
            }
            else
                break;
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_BGO)
    {
        foreach(QGraphicsItem *item, sortedGraphBGO)
        {
            if(sortedBGO.size() != 0)
            {
                QMap<int, LevelBGO>::iterator beginItem = sortedBGO.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedBGO.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedBGO.begin();
                currentArrayId = (*beginItem).meta.array_id;

                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundBGO(*beginItem, item);
            }
            else
                break;
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_NPC)
    {
        foreach(QGraphicsItem *item, sortedGraphNPC)
        {
            if(sortedNPC.size() != 0)
            {
                QMap<int, LevelNPC>::iterator beginItem = sortedNPC.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedNPC.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedNPC.begin();

                currentArrayId = (*beginItem).meta.array_id;

                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundNPC(*beginItem, item);
            }
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_PhysEnv)
    {
        foreach(QGraphicsItem *item, sortedGraphWater)
        {
            if(sortedWater.size() != 0)
            {
                QMap<int, LevelPhysEnv>::iterator beginItem = sortedWater.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedWater.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedWater.begin();

                currentArrayId = (*beginItem).meta.array_id;

                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundPhysEnv(*beginItem, item);
            }
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_Door)
    {
        foreach(QGraphicsItem *item, sortedGraphDoorEntrance)
        {
            if(sortedEntranceDoors.size() != 0)
            {
                QMap<int, LevelDoor>::iterator beginItem = sortedEntranceDoors.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                {
                    //not found
                    sortedEntranceDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedEntranceDoors.begin();

                currentArrayId = (*beginItem).meta.array_id;

                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundDoor(*beginItem, item);
            }
        }

        foreach(QGraphicsItem *item, sortedGraphDoorExit)
        {
            if(sortedExitDoors.size() != 0)
            {
                QMap<int, LevelDoor>::iterator beginItem = sortedExitDoors.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                {
                    //not found
                    sortedExitDoors.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedExitDoors.begin();

                currentArrayId = (*beginItem).meta.array_id;

                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundDoor(*beginItem, item);
            }
        }

        MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2); //update Door data
    }

    if(m_findFilter & ItemTypes::LVL_S_Player)
    {
        foreach(QGraphicsItem *item, sortedGraphPlayers)
        {
            if(sortedPlayers.size() != 0)
            {
                QMap<int, PlayerPoint>::iterator beginItem = sortedPlayers.begin();
                unsigned int currentArrayId;

                //but still test if the next blocks, is the block we search!
                beginItem = sortedPlayers.begin();
                currentArrayId = (*beginItem).id;

                if(item->data(LvlScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundPlayerPoint(*beginItem, item);
            }
        }
    }
}

void ItemSearcher::find(const WorldData &dataToFind, const QList<QGraphicsItem *> &allItems)
{
    QMap<int, WorldTerrainTile> sortedTiles;
    if(m_findFilter & ItemTypes::WLD_S_Tile)
    {
        foreach(WorldTerrainTile tile, dataToFind.tiles)
            sortedTiles[tile.meta.array_id] = tile;
    }

    QMap<int, WorldScenery> sortedScenery;
    if(m_findFilter & ItemTypes::WLD_S_Scenery)
    {
        foreach(WorldScenery scenery, dataToFind.scenery)
            sortedScenery[scenery.meta.array_id] = scenery;
    }

    QMap<int, WorldPathTile> sortedPath;
    if(m_findFilter & ItemTypes::WLD_S_Path)
    {
        foreach(WorldPathTile path, dataToFind.paths)
            sortedPath[path.meta.array_id] = path;
    }

    QMap<int, WorldLevelTile> sortedLevel;
    if(m_findFilter & ItemTypes::WLD_S_Level)
    {
        foreach(WorldLevelTile level, dataToFind.levels)
            sortedLevel[level.meta.array_id] = level;
    }

    QMap<int, WorldMusicBox> sortedMusic;
    if(m_findFilter & ItemTypes::WLD_S_MusicBox)
    {
        foreach(WorldMusicBox music, dataToFind.music)
            sortedMusic[music.meta.array_id] = music;
    }

    QMap<int, QGraphicsItem *> sortedGraphTile;
    QMap<int, QGraphicsItem *> sortedGraphScenery;
    QMap<int, QGraphicsItem *> sortedGraphPath;
    QMap<int, QGraphicsItem *> sortedGraphLevel;
    QMap<int, QGraphicsItem *> sortedGraphMusicBox;

    foreach(QGraphicsItem *unsortedItem, allItems)
    {
        if(unsortedItem->data(WldScene::ITEM_IS_ITEM).isNull() || !unsortedItem->data(WldScene::ITEM_IS_ITEM).toBool())
            continue;

        int objType = unsortedItem->data(WldScene::ITEM_TYPE_INT).toInt();

        switch(objType)
        {
        case ItemTypes::WLD_Tile:
            if(m_findFilter & ItemTypes::WLD_S_Tile)
                sortedGraphTile[unsortedItem->data(WldScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::WLD_Scenery:
            if(m_findFilter & ItemTypes::WLD_S_Scenery)
                sortedGraphScenery[unsortedItem->data(WldScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::WLD_Path:
            if(m_findFilter & ItemTypes::WLD_S_Path)
                sortedGraphPath[unsortedItem->data(WldScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::WLD_Level:
            if(m_findFilter & ItemTypes::WLD_S_Level)
                sortedGraphLevel[unsortedItem->data(WldScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;

        case ItemTypes::WLD_MusicBox:
            if(m_findFilter & ItemTypes::WLD_S_MusicBox)
                sortedGraphMusicBox[unsortedItem->data(WldScene::ITEM_ARRAY_ID).toInt()] = unsortedItem;
            break;
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Tile)
    {
        foreach(QGraphicsItem *item, sortedGraphTile)
        {
            if(sortedTiles.size() != 0)
            {
                QMap<int, WorldTerrainTile>::iterator beginItem = sortedTiles.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedTiles.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedTiles.begin();
                currentArrayId = (*beginItem).meta.array_id;
                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundTile(*beginItem, item);
            }
            else
                break;
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Path)
    {
        foreach(QGraphicsItem *item, sortedGraphPath)
        {
            if(sortedPath.size() != 0)
            {
                QMap<int, WorldPathTile>::iterator beginItem = sortedPath.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedPath.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedPath.begin();
                currentArrayId = (*beginItem).meta.array_id;
                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundPath(*beginItem, item);
            }
            else
                break;
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Scenery)
    {
        foreach(QGraphicsItem *item, sortedGraphScenery)
        {
            if(sortedScenery.size() != 0)
            {
                QMap<int, WorldScenery>::iterator beginItem = sortedScenery.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedScenery.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedScenery.begin();
                currentArrayId = (*beginItem).meta.array_id;
                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundScenery(*beginItem, item);
            }
            else
                break;
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Level)
    {
        foreach(QGraphicsItem *item, sortedGraphLevel)
        {
            if(sortedLevel.size() != 0)
            {
                QMap<int, WorldLevelTile>::iterator beginItem = sortedLevel.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;
                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedLevel.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedLevel.begin();
                currentArrayId = (*beginItem).meta.array_id;
                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundLevel(*beginItem, item);
            }
            else
                break;
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_MusicBox)
    {
        foreach(QGraphicsItem *item, sortedGraphMusicBox)
        {
            if(sortedMusic.size() != 0)
            {
                QMap<int, WorldMusicBox>::iterator beginItem = sortedMusic.begin();
                unsigned int currentArrayId = (*beginItem).meta.array_id;

                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
                {
                    //not found
                    sortedMusic.erase(beginItem);
                }

                //but still test if the next blocks, is the block we search!
                beginItem = sortedMusic.begin();
                currentArrayId = (*beginItem).meta.array_id;

                if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                    emit foundMusicbox(*beginItem, item);
            }
            else
                break;
        }
    }
}
