/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "item_point.h"




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

    if(m_pastingMode) TileItem->setSelected(true);
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

    ItemScene *SceneItem = new ItemScene(this);
    SceneItem->setSceneData(scenery, &mergedSet, &animator);

    SceneItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockScenery));
    SceneItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockScenery));

    if(m_pastingMode) SceneItem->setSelected(true);
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

    ItemPath *PathItem = new ItemPath(this);
    PathItem->setPathData(path, &mergedSet, &animator);

    PathItem->setOpacity(m_opts.semiTransparentPaths ? 0.5 : 1);

    PathItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockPath));
    PathItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockPath));

    if(m_pastingMode) PathItem->setSelected(true);
}





void WldScene::placeLevel(WorldLevelTile &level, bool toGrid)
{
    long animator=0, pathAnimator=0, bPathAnimator=0;
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

    ItemLevel *LevelItem = new ItemLevel(this);

    LevelItem->setLevelData(level, &mergedSet, &animator, &pathAnimator, &bPathAnimator);

    LevelItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockLevel));
    LevelItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockLevel));

    if(m_pastingMode) LevelItem->setSelected(true);
}





void WldScene::placeMusicbox(WorldMusicBox &musicbox, bool toGrid)
{
    int j = m_configs->getMusWldI(musicbox.id);

    ItemMusic *MusicBoxItem = new ItemMusic(this);

    QPoint newPos = QPoint(musicbox.x, musicbox.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(musicbox.x, musicbox.y), MusicBoxItem->getGridSize());
        musicbox.x = newPos.x();
        musicbox.y = newPos.y();
    }

    MusicBoxItem->setMusicData(musicbox);
    if(j>=0)
    {
        MusicBoxItem->m_musicTitle =
                (m_configs->music_w_custom_id==musicbox.id) ?
                    musicbox.music_file:
                    m_configs->main_music_wld[j].name;
    }

    MusicBoxItem->setFlag(QGraphicsItem::ItemIsSelectable, (!m_lockMusicBox));
    MusicBoxItem->setFlag(QGraphicsItem::ItemIsMovable, (!m_lockMusicBox));

    if(m_pastingMode) MusicBoxItem->setSelected(true);
}



