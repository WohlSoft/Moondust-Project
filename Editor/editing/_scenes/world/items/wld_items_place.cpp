/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <editing/edit_world/world_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../wld_scene.h"
#include "item_tile.h"
#include "item_scene.h"
#include "item_path.h"
#include "item_level.h"
#include "item_music.h"




void WldScene::placeTile(WorldTerrainTile &tile, bool toGrid)
{
    obj_w_tile &mergedSet = m_localConfigTerrain[tile.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_wtiles[1];
        mergedSet.image = m_dummyTerrainImg;
    }

    QPoint newPos = QPoint(tile.x, tile.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(tile.x, tile.y), mergedSet.setup.grid);
        tile.x = newPos.x();
        tile.y = newPos.y();
    }

    ItemTile *TileItem = new ItemTile(this);
    TileItem->setTileData(tile, &mergedSet, &animator);

    TileItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockTerrain));
    TileItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockTerrain));

    if(m_pastingMode)
        TileItem->setSelected(true);
}

void WldScene::placeTile(const WorldTerrainTile &tile)
{
    obj_w_tile &mergedSet = m_localConfigTerrain[tile.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_wtiles[1];
        mergedSet.image = m_dummyTerrainImg;
    }

    ItemTile *tileItem = new ItemTile(this);
    tileItem->setTileData(tile, &mergedSet, &animator);

    tileItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockTerrain));
    tileItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockTerrain));

    if(m_pastingMode)
        tileItem->setSelected(true);
}





void WldScene::placeScenery(WorldScenery &scenery, bool toGrid)
{
    obj_w_scenery &mergedSet = m_localConfigScenery[scenery.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_wscene[1];
        mergedSet.image = m_dummySceneryImg;
    }

    QPoint newPos = QPoint(scenery.x, scenery.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(scenery.x, scenery.y), mergedSet.setup.grid);
        scenery.x = newPos.x();
        scenery.y = newPos.y();
    }

    ItemScene *sceneItem = new ItemScene(this);
    sceneItem->setSceneData(scenery, &mergedSet, &animator);

    sceneItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockScenery));
    sceneItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockScenery));

    if(m_pastingMode)
        sceneItem->setSelected(true);
}

void WldScene::placeScenery(const WorldScenery &scenery)
{
    obj_w_scenery &mergedSet = m_localConfigScenery[scenery.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_wscene[1];
        mergedSet.image = m_dummySceneryImg;
    }

    ItemScene *sceneItem = new ItemScene(this);
    sceneItem->setSceneData(scenery, &mergedSet, &animator);

    sceneItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockScenery));
    sceneItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockScenery));

    if(m_pastingMode)
        sceneItem->setSelected(true);
}





void WldScene::placePath(WorldPathTile &path, bool toGrid)
{
    obj_w_path &mergedSet = m_localConfigPaths[path.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_wpaths[1];
        mergedSet.image = m_dummyPathImg;
    }

    QPoint newPos = QPoint(path.x, path.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(path.x, path.y), mergedSet.setup.grid);
        path.x = newPos.x();
        path.y = newPos.y();
    }

    ItemPath *pathItem = new ItemPath(this);
    pathItem->setPathData(path, &mergedSet, &animator);

    pathItem->setOpacity(m_opts.semiTransparentPaths ? 0.5 : 1);

    pathItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockPath));
    pathItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockPath));

    if(m_pastingMode)
        pathItem->setSelected(true);
}

void WldScene::placePath(const WorldPathTile &path)
{
    obj_w_path &mergedSet = m_localConfigPaths[path.id];
    long animator = mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_wpaths[1];
        mergedSet.image = m_dummyPathImg;
    }

    ItemPath *pathItem = new ItemPath(this);
    pathItem->setPathData(path, &mergedSet, &animator);

    pathItem->setOpacity(m_opts.semiTransparentPaths ? 0.5 : 1);

    pathItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockPath));
    pathItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockPath));

    if(m_pastingMode)
        pathItem->setSelected(true);
}





void WldScene::placeLevel(WorldLevelTile &level, bool toGrid)
{
    long animator = 0, pathAnimator = 0, bPathAnimator = 0;
    obj_w_level &mergedSet = m_localConfigLevels[level.id];
    animator =      mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_wlevels[0];
        mergedSet.image = m_dummyLevelImg;
    }
    pathAnimator =  m_localConfigLevels[m_configs->marker_wlvl.path].animator_id;
    bPathAnimator = m_localConfigLevels[m_configs->marker_wlvl.bigpath].animator_id;

    QPoint newPos = QPoint(level.x, level.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(level.x, level.y), mergedSet.setup.grid);
        level.x = newPos.x();
        level.y = newPos.y();
    }

    ItemLevel *levelItem = new ItemLevel(this);

    levelItem->setLevelData(level, &mergedSet, &animator, &pathAnimator, &bPathAnimator);

    levelItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockLevel));
    levelItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockLevel));

    if(m_pastingMode)
        levelItem->setSelected(true);
}

void WldScene::placeLevel(const WorldLevelTile &level)
{
    long animator = 0, pathAnimator = 0, bPathAnimator = 0;
    obj_w_level &mergedSet = m_localConfigLevels[level.id];
    animator =      mergedSet.animator_id;
    if(!mergedSet.isValid)
    {
        animator = 0;
        mergedSet = m_configs->main_wlevels[0];
        mergedSet.image = m_dummyLevelImg;
    }
    pathAnimator =  m_localConfigLevels[m_configs->marker_wlvl.path].animator_id;
    bPathAnimator = m_localConfigLevels[m_configs->marker_wlvl.bigpath].animator_id;

    ItemLevel *levelItem = new ItemLevel(this);

    levelItem->setLevelData(level, &mergedSet, &animator, &pathAnimator, &bPathAnimator);

    levelItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockLevel));
    levelItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockLevel));

    if(m_pastingMode)
        levelItem->setSelected(true);
}





void WldScene::placeMusicbox(WorldMusicBox &musicbox, bool toGrid)
{
    int j = m_configs->getMusWldI(musicbox.id);

    ItemMusic *musicBoxItem = new ItemMusic(this);

    QPoint newPos = QPoint(musicbox.x, musicbox.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(musicbox.x, musicbox.y), musicBoxItem->getGridSize());
        musicbox.x = newPos.x();
        musicbox.y = newPos.y();
    }

    musicBoxItem->setMusicData(musicbox);
    if(j >= 0)
    {
        musicBoxItem->m_musicTitle =
            (m_configs->music_w_custom_id == musicbox.id) ?
            musicbox.music_file :
            m_configs->main_music_wld[j].name;
    }

    musicBoxItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockMusicBox));
    musicBoxItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockMusicBox));

    if(m_pastingMode)
        musicBoxItem->setSelected(true);
}

void WldScene::placeMusicbox(const WorldMusicBox &musicbox)
{
    int j = m_configs->getMusWldI(musicbox.id);

    ItemMusic *musicBoxItem = new ItemMusic(this);

    musicBoxItem->setMusicData(musicbox);
    if(j >= 0)
    {
        musicBoxItem->m_musicTitle =
            (m_configs->music_w_custom_id == musicbox.id) ?
            musicbox.music_file :
            m_configs->main_music_wld[j].name;
    }

    musicBoxItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockMusicBox));
    musicBoxItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockMusicBox));

    if(m_pastingMode)
        musicBoxItem->setSelected(true);
}

