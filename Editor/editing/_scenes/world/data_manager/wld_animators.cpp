/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/graphics_funcs.h>
#include <editing/edit_world/world_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "../wld_scene.h"


void WldScene::buildAnimators()
{
    int i;

    LogDebug("WorldAnimators -> Build tiles animator");
    uTiles.clear();
    uTiles.allocateSlots(m_configs->main_wtiles.total());
    for(i=1; i<m_configs->main_wtiles.size(); i++)
    {
        obj_w_tile & objTile = m_configs->main_wtiles[i];
        obj_w_tile t_tile;
        objTile.copyTo(t_tile);
        SimpleAnimator * aniTile = new SimpleAnimator(
                          ((t_tile.cur_image->isNull())?
                            m_dummyTerrainImg:
                           *t_tile.cur_image),
                            t_tile.animated,
                            t_tile.frames,
                            t_tile.framespeed
                           );

        m_animatorsTerrain.push_back( aniTile );
        animator.registerAnimation( aniTile );
        t_tile.animator_id = m_animatorsTerrain.size()-1;
        uTiles.storeElement(i, t_tile);
    }

    LogDebug("WorldAnimators -> Build sceneries animator");
    uScenes.clear();
    uScenes.allocateSlots(m_configs->main_wscene.total());
    for(i=1; i<m_configs->main_wscene.size(); i++)
    {
        obj_w_scenery & objScenery = m_configs->main_wscene[i];
        obj_w_scenery t_scene;
        objScenery.copyTo(t_scene);
        SimpleAnimator * aniScene = new SimpleAnimator(
                         ((t_scene.cur_image->isNull())?
                                m_dummySceneryImg:
                               *t_scene.cur_image),
                                t_scene.animated,
                                t_scene.frames,
                                t_scene.framespeed
                              );

        m_animatorsScenery.push_back( aniScene );
        animator.registerAnimation( aniScene );
        t_scene.animator_id = m_animatorsScenery.size()-1;
        uScenes.storeElement(i, t_scene);
    }

    LogDebug("WorldAnimators -> Build paths animator");
    uPaths.clear();
    uPaths.allocateSlots(m_configs->main_wpaths.total());
    for(i=1; i<m_configs->main_wpaths.size(); i++)
    {
        obj_w_path & objPath = m_configs->main_wpaths[i];
        obj_w_path t_path;
        objPath.copyTo(t_path);
        SimpleAnimator * aniScene = new SimpleAnimator(
                         ((t_path.cur_image->isNull())?
                                m_dummySceneryImg:
                               *t_path.cur_image),
                                t_path.animated,
                                t_path.frames,
                                t_path.framespeed
                              );

        m_animatorsPaths.push_back( aniScene );
        animator.registerAnimation( aniScene );
        t_path.animator_id = m_animatorsPaths.size()-1;
        uPaths.storeElement(i, t_path);
    }

    LogDebug("WorldAnimators -> Build levels animator");
    uLevels.clear();
    uLevels.allocateSlots(m_configs->main_wlevels.total());
    for(i=0; i<m_configs->main_wlevels.size(); i++)
    {
        obj_w_level& objLevel = m_configs->main_wlevels[i];
        obj_w_level  t_level;
        objLevel.copyTo(t_level);
        SimpleAnimator * aniScene = new SimpleAnimator(
                         ((t_level.cur_image->isNull())?
                                m_dummySceneryImg:
                               *t_level.cur_image),
                                t_level.animated,
                                t_level.frames,
                                t_level.framespeed
                              );

        m_animatorsLevels.push_back( aniScene );
        animator.registerAnimation( aniScene );
        t_level.animator_id = m_animatorsLevels.size()-1;
        uLevels.storeElement(i, t_level);
    }

    LogDebug("WorldAnimators -> done");
}


void WldScene::startAnimation()
{
    long q = m_data->tiles.size();
    q+= m_data->levels.size();
    q+= m_data->scenery.size();
    q+= m_data->paths.size();

    if( q > GlobalSettings::animatorItemsLimit )
    {
        LogWarning(QString("Can't start animation: too many items on map: %1").arg(q));
        return;
    }
    animator.start(32);
}



void WldScene::stopAnimation()
{
    animator.stop();
    update();
}
