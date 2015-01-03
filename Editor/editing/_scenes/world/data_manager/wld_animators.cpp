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

#include <common_features/graphics_funcs.h>
#include <editing/edit_world/world_edit.h>
#include <file_formats/file_formats.h>

#include "../wld_scene.h"


void WldScene::buildAnimators()
{
    int i;

    WriteToLog(QtDebugMsg, "WorldAnimators -> Build tiles animator");
    for(i=0; i<pConfigs->main_wtiles.size(); i++)
    {
        SimpleAnimator * aniTile = new SimpleAnimator(
                         ((pConfigs->main_wtiles[i].image.isNull())?
                                uTileImg:
                               pConfigs->main_wtiles[i].image),
                              pConfigs->main_wtiles[i].animated,
                              pConfigs->main_wtiles[i].frames,
                              pConfigs->main_wtiles[i].framespeed
                              );

        animates_Tiles.push_back( aniTile );
        if(pConfigs->main_wtiles[i].id < (unsigned int)index_tiles.size())
        {
            index_tiles[pConfigs->main_wtiles[i].id].ai = animates_Tiles.size()-1;
        }
    }

    WriteToLog(QtDebugMsg, "WorldAnimators -> Build sceneries animator");
    for(i=0; i<pConfigs->main_wscene.size(); i++)
    {
        SimpleAnimator * aniScene = new SimpleAnimator(
                         ((pConfigs->main_wscene[i].image.isNull())?
                                uSceneImg:
                               pConfigs->main_wscene[i].image),
                              pConfigs->main_wscene[i].animated,
                              pConfigs->main_wscene[i].frames,
                              pConfigs->main_wscene[i].framespeed
                              );

        animates_Scenery.push_back( aniScene );
        if(pConfigs->main_wscene[i].id < (unsigned int)index_scenes.size())
        {
            index_scenes[pConfigs->main_wscene[i].id].ai = animates_Scenery.size()-1;
        }
    }

    WriteToLog(QtDebugMsg, "WorldAnimators -> Build paths animator");
    for(i=0; i<pConfigs->main_wpaths.size(); i++)
    {
        SimpleAnimator * aniPath = new SimpleAnimator(
                         ((pConfigs->main_wpaths[i].image.isNull())?
                                uPathImg:
                               pConfigs->main_wpaths[i].image),
                              pConfigs->main_wpaths[i].animated,
                              pConfigs->main_wpaths[i].frames,
                              pConfigs->main_wpaths[i].framespeed
                              );

        animates_Paths.push_back( aniPath );
        if(pConfigs->main_wpaths[i].id < (unsigned int)index_paths.size())
        {
            index_paths[pConfigs->main_wpaths[i].id].ai = animates_Paths.size()-1;
        }
    }

    WriteToLog(QtDebugMsg, "WorldAnimators -> Build levels animator");
    for(i=0; i<pConfigs->main_wlevels.size(); i++)
    {
        SimpleAnimator * aniLevel = new SimpleAnimator(
                         ((pConfigs->main_wlevels[i].image.isNull())?
                                uLevelImg:
                               pConfigs->main_wlevels[i].image),
                              pConfigs->main_wlevels[i].animated,
                              pConfigs->main_wlevels[i].frames,
                              pConfigs->main_wlevels[i].framespeed
                              );

        animates_Levels.push_back( aniLevel );

        if(pConfigs->main_wlevels[i].id < (unsigned int)index_levels.size())
        {
            index_levels[pConfigs->main_wlevels[i].id].ai = animates_Levels.size()-1;
        }
        WriteToLog(QtDebugMsg, "WorldAnimators -> Item made");
    }

    WriteToLog(QtDebugMsg, "WorldAnimators -> done");
}



void WldScene::startAnimation()
{
    long q = WldData->tiles.size();
    q+= WldData->levels.size();
    q+= WldData->scenery.size();
    q+= WldData->paths.size();

    if( q > GlobalSettings::animatorItemsLimit )
    {
        WriteToLog(QtWarningMsg,
           QString("Can't start animation: too many items on map: %1").arg(q));
        return;
    }

    foreach(SimpleAnimator * tile, animates_Tiles)
    {
        tile->start();
    }
    foreach(SimpleAnimator * scene, animates_Scenery)
    {
        scene->start();
    }
    foreach(SimpleAnimator * path, animates_Paths)
    {
        path->start();
    }
    foreach(SimpleAnimator * lvl, animates_Levels)
    {
        lvl->start();
    }


}



void WldScene::stopAnimation()
{

    foreach(SimpleAnimator * tile, animates_Tiles)
    {
        tile->stop();
    }
    foreach(SimpleAnimator * scene, animates_Scenery)
    {
        scene->stop();
    }
    foreach(SimpleAnimator * path, animates_Paths)
    {
        path->stop();
    }
    foreach(SimpleAnimator * lvl, animates_Levels)
    {
        lvl->stop();
    }

    update();
}
