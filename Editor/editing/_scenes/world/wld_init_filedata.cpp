/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <editing/edit_world/world_edit.h>

#include "wld_scene.h"

// ///////////////////SET Tiles Objects/////////////////////////////////////////////
void WldScene::setTiles(QProgressDialog &progress)
{
    int i = 0;

    //Applay images to objects
    for(i = 0; i < m_data->tiles.size(); i++)
    {
        //Add tile to scene
        placeTile(m_data->tiles[i]);

        if(progress.wasCanceled())
            return;
    }
}

// ///////////////////SET Scenery Objects/////////////////////////////////////////////
void WldScene::setSceneries(QProgressDialog &progress)
{
    int i = 0;

    //Applay images to objects
    for(i = 0; i < m_data->scenery.size(); i++)
    {
        //Add scenery to scene
        placeScenery(m_data->scenery[i]);

        if(progress.wasCanceled())
            return;
    }
}


// ///////////////////SET Paths Objects/////////////////////////////////////////////
void WldScene::setPaths(QProgressDialog &progress)
{
    int i = 0;

    //Applay images to objects
    for(i = 0; i < m_data->paths.size(); i++)
    {
        //Add path to scene
        placePath(m_data->paths[i]);

        if(progress.wasCanceled())
            return;
    }
}

// ///////////////////SET Levels Objects/////////////////////////////////////////////
void WldScene::setLevels(QProgressDialog &progress)
{
    int i = 0;

    //Applay images to objects
    for(i = 0; i < m_data->levels.size(); i++)
    {
        //Add level to scene
        placeLevel(m_data->levels[i]);

        if(progress.wasCanceled())
            return;
    }
}



// ///////////////////SET Music Objects/////////////////////////////////////////////
void WldScene::setMusicBoxes(QProgressDialog &progress)
{
    int i = 0;

    //Applay images to objects
    for(i = 0; i < m_data->music.size(); i++)
    {
        //Add music to scene
        placeMusicbox(m_data->music[i]);

        if(progress.wasCanceled())
            return;
    }
}
