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



void ItemSearcher::find(const LevelData &dataToFind, LvlScene *scene)
{
    if(m_findFilter & ItemTypes::LVL_S_Door)
    {
        foreach(const LevelDoor &door, dataToFind.doors)
        {
            if(door.isSetIn && !door.isSetOut)
            {
                auto d = scene->m_itemsDoorEnters.find(door.meta.array_id);
                if(d != scene->m_itemsDoorEnters.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == door.meta.array_id)
                    emit foundDoor(door, d.value());
            }
        }

        foreach(const LevelDoor &door, dataToFind.doors)
        {
            if(!door.isSetIn && door.isSetOut)
            {
                auto d = scene->m_itemsDoorExits.find(door.meta.array_id);
                if(d != scene->m_itemsDoorExits.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == door.meta.array_id)
                    emit foundDoor(door, d.value());
            }
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_Block)
    {
        foreach(const LevelBlock &block, dataToFind.blocks)
        {
            auto d = scene->m_itemsBlocks.find(block.meta.array_id);
                if(d != scene->m_itemsBlocks.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == block.meta.array_id)
                    emit foundBlock(block, d.value());
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_BGO)
    {
        foreach(const LevelBGO &bgo, dataToFind.bgo)
        {
            auto d = scene->m_itemsBGO.find(bgo.meta.array_id);
            if(d != scene->m_itemsBGO.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == bgo.meta.array_id)
                emit foundBGO(bgo, d.value());
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_NPC)
    {
        foreach(const LevelNPC &npc, dataToFind.npc)
        {
            auto d = scene->m_itemsNPC.find(npc.meta.array_id);
            if(d != scene->m_itemsNPC.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == npc.meta.array_id)
                emit foundNPC(npc, d.value());
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_PhysEnv)
    {
        foreach(const LevelPhysEnv &water, dataToFind.physez)
        {
            auto d = scene->m_itemsPhysEnv.find(water.meta.array_id);
            if(d != scene->m_itemsPhysEnv.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == water.meta.array_id)
                emit foundPhysEnv(water, d.value());
        }
    }

    if(m_findFilter & ItemTypes::LVL_S_Player)
    {
        foreach(const PlayerPoint &player, dataToFind.players)
        {
            auto d = scene->m_itemsPlayers.find(player.id);
            if(d != scene->m_itemsPlayers.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == player.id)
                emit foundPlayerPoint(player, d.value());
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
            if(sortedTiles.isEmpty())
                break;

            QMap<int, WorldTerrainTile>::iterator beginItem = sortedTiles.begin();
            unsigned int currentArrayId = (*beginItem).meta.array_id;
            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
            {
                //not found
                sortedTiles.erase(beginItem);
                if(sortedTiles.isEmpty())
                    break;
            }

            //but still test if the next blocks, is the block we search!
            beginItem = sortedTiles.begin();
            currentArrayId = (*beginItem).meta.array_id;
            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                emit foundTile(*beginItem, item);
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Path)
    {
        foreach(QGraphicsItem *item, sortedGraphPath)
        {
            if(sortedPath.isEmpty())
                break;

            QMap<int, WorldPathTile>::iterator beginItem = sortedPath.begin();
            unsigned int currentArrayId = (*beginItem).meta.array_id;
            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
            {
                //not found
                sortedPath.erase(beginItem);
                if(sortedPath.isEmpty())
                    break;
            }

            //but still test if the next blocks, is the block we search!
            beginItem = sortedPath.begin();
            currentArrayId = (*beginItem).meta.array_id;
            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                emit foundPath(*beginItem, item);
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Scenery)
    {
        foreach(QGraphicsItem *item, sortedGraphScenery)
        {
            if(sortedScenery.isEmpty())
                break;

            QMap<int, WorldScenery>::iterator beginItem = sortedScenery.begin();
            unsigned int currentArrayId = (*beginItem).meta.array_id;
            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
            {
                //not found
                sortedScenery.erase(beginItem);
                if(sortedScenery.isEmpty())
                    continue;
            }

            //but still test if the next blocks, is the block we search!
            beginItem = sortedScenery.begin();
            currentArrayId = (*beginItem).meta.array_id;
            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                emit foundScenery(*beginItem, item);
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Level)
    {
        foreach(QGraphicsItem *item, sortedGraphLevel)
        {
            if(sortedLevel.isEmpty())
                break;

            QMap<int, WorldLevelTile>::iterator beginItem = sortedLevel.begin();
            unsigned int currentArrayId = (*beginItem).meta.array_id;
            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
            {
                //not found
                sortedLevel.erase(beginItem);
                if(sortedLevel.isEmpty())
                    break;
            }

            //but still test if the next blocks, is the block we search!
            beginItem = sortedLevel.begin();
            currentArrayId = (*beginItem).meta.array_id;
            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                emit foundLevel(*beginItem, item);
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_MusicBox)
    {
        foreach(QGraphicsItem *item, sortedGraphMusicBox)
        {
            if(sortedMusic.isEmpty())
                break;

            QMap<int, WorldMusicBox>::iterator beginItem = sortedMusic.begin();
            unsigned int currentArrayId = (*beginItem).meta.array_id;

            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() > currentArrayId)
            {
                //not found
                sortedMusic.erase(beginItem);
                if(sortedMusic.isEmpty())
                    break;
            }

            //but still test if the next blocks, is the block we search!
            beginItem = sortedMusic.begin();
            currentArrayId = (*beginItem).meta.array_id;

            if(item->data(WldScene::ITEM_ARRAY_ID).toUInt() == currentArrayId)
                emit foundMusicbox(*beginItem, item);
        }
    }
}
