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
    m_localConfigTerrain.clear();
    m_localConfigTerrain.allocateSlots(m_configs->main_wtiles.total());
    for(i=1; i<m_configs->main_wtiles.size(); i++)
    {
        obj_w_tile & objTile = m_configs->main_wtiles[i];
        obj_w_tile t_tile;
        objTile.copyTo(t_tile);
        SimpleAnimator * aniTile = new SimpleAnimator(
                          ((t_tile.cur_image->isNull())?
                            m_dummyTerrainImg:
                           *t_tile.cur_image),
                            t_tile.setup.animated,
                            t_tile.setup.frames,
                            t_tile.setup.framespeed
                           );

        if(!t_tile.setup.frame_sequence.isEmpty())
            aniTile->setFrameSequance(t_tile.setup.frame_sequence);

        m_animatorsTerrain.push_back( aniTile );
        m_animationTimer.registerAnimation( aniTile );
        t_tile.animator_id = m_animatorsTerrain.size()-1;
        m_localConfigTerrain.storeElement(i, t_tile);
    }

    LogDebug("WorldAnimators -> Build sceneries animator");
    m_localConfigScenery.clear();
    m_localConfigScenery.allocateSlots(m_configs->main_wscene.total());
    for(i=1; i<m_configs->main_wscene.size(); i++)
    {
        obj_w_scenery & objScenery = m_configs->main_wscene[i];
        obj_w_scenery t_scene;
        objScenery.copyTo(t_scene);
        SimpleAnimator * aniScene = new SimpleAnimator(
                         ((t_scene.cur_image->isNull())?
                                m_dummySceneryImg:
                               *t_scene.cur_image),
                                t_scene.setup.animated,
                                t_scene.setup.frames,
                                t_scene.setup.framespeed
                              );

        if(!t_scene.setup.frame_sequence.isEmpty())
            aniScene->setFrameSequance(t_scene.setup.frame_sequence);

        m_animatorsScenery.push_back( aniScene );
        m_animationTimer.registerAnimation( aniScene );
        t_scene.animator_id = m_animatorsScenery.size()-1;
        m_localConfigScenery.storeElement(i, t_scene);
    }

    LogDebug("WorldAnimators -> Build paths animator");
    m_localConfigPaths.clear();
    m_localConfigPaths.allocateSlots(m_configs->main_wpaths.total());
    for(i=1; i<m_configs->main_wpaths.size(); i++)
    {
        obj_w_path & objPath = m_configs->main_wpaths[i];
        obj_w_path t_path;
        objPath.copyTo(t_path);
        SimpleAnimator * aniPath = new SimpleAnimator(
                         ((t_path.cur_image->isNull())?
                                m_dummySceneryImg:
                               *t_path.cur_image),
                                t_path.setup.animated,
                                t_path.setup.frames,
                                t_path.setup.framespeed
                              );

        if(!t_path.setup.frame_sequence.isEmpty())
            aniPath->setFrameSequance(t_path.setup.frame_sequence);

        m_animatorsPaths.push_back( aniPath );
        m_animationTimer.registerAnimation( aniPath );
        t_path.animator_id = m_animatorsPaths.size()-1;
        m_localConfigPaths.storeElement(i, t_path);
    }

    LogDebug("WorldAnimators -> Build levels animator");
    m_localConfigLevels.clear();
    m_localConfigLevels.allocateSlots(m_configs->main_wlevels.total());
    for(i=0; i<m_configs->main_wlevels.size(); i++)
    {
        obj_w_level& objLevel = m_configs->main_wlevels[i];
        obj_w_level  t_level;
        objLevel.copyTo(t_level);
        SimpleAnimator * aniLevel = new SimpleAnimator(
                         ((t_level.cur_image->isNull())?
                                m_dummySceneryImg:
                               *t_level.cur_image),
                                t_level.setup.animated,
                                t_level.setup.frames,
                                t_level.setup.framespeed
                              );

        if(!t_level.setup.frame_sequence.isEmpty())
            aniLevel->setFrameSequance(t_level.setup.frame_sequence);

        m_animatorsLevels.push_back( aniLevel );
        m_animationTimer.registerAnimation( aniLevel );
        t_level.animator_id = m_animatorsLevels.size()-1;
        m_localConfigLevels.storeElement(i, t_level);
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
    m_animationTimer.start(32);
}



void WldScene::stopAnimation()
{
    m_animationTimer.stop();
    update();
}
