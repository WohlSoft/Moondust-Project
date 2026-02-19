#include "itemsearcher.h"

#include <main_window/dock/lvl_warp_props.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>
#include <editing/_scenes/level/items/item_door.h>
#include <editing/_scenes/level/items/item_playerpoint.h>
#include <editing/_scenes/level/items/item_water.h>

#include <editing/_scenes/world/items/item_tile.h>
#include <editing/_scenes/world/items/item_scene.h>
#include <editing/_scenes/world/items/item_path.h>
#include <editing/_scenes/world/items/item_level.h>
#include <editing/_scenes/world/items/item_music.h>

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

void ItemSearcher::find(const WorldData &dataToFind, WldScene *scene)
{
    if(m_findFilter & ItemTypes::WLD_S_Tile)
    {
        foreach(const WorldTerrainTile &tile, dataToFind.tiles)
        {
            auto d = scene->m_itemsTiles.find(tile.meta.array_id);
            if(d != scene->m_itemsTiles.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == tile.meta.array_id)
                emit foundTile(tile, d.value());
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Scenery)
    {
        foreach(const WorldScenery &scenery, dataToFind.scenery)
        {
            auto d = scene->m_itemsScenery.find(scenery.meta.array_id);
            if(d != scene->m_itemsScenery.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == scenery.meta.array_id)
                emit foundScenery(scenery, d.value());
        }
    }


    if(m_findFilter & ItemTypes::WLD_S_Path)
    {
        foreach(const WorldPathTile &path, dataToFind.paths)
        {
            auto d = scene->m_itemsPaths.find(path.meta.array_id);
            if(d != scene->m_itemsPaths.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == path.meta.array_id)
                emit foundPath(path, d.value());
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_Level)
    {
        foreach(const WorldLevelTile &level, dataToFind.levels)
        {
            auto d = scene->m_itemsLevels.find(level.meta.array_id);
            if(d != scene->m_itemsLevels.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == level.meta.array_id)
                emit foundLevel(level, d.value());
        }
    }

    if(m_findFilter & ItemTypes::WLD_S_MusicBox)
    {
        foreach(const WorldMusicBox &music, dataToFind.music)
        {
            auto d = scene->m_itemsMusicBoxes.find(music.meta.array_id);
            if(d != scene->m_itemsMusicBoxes.end() && d.value()->data(LvlScene::ITEM_ARRAY_ID).toUInt() == music.meta.array_id)
                emit foundMusicbox(music, d.value());
        }
    }
}
