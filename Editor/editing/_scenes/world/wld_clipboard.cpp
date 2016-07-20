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

#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "wld_scene.h"
#include "wld_history_manager.h"

//Copy selected items into clipboard
WorldData WldScene::copy(bool cut)
{

    //Get Selected Items
    QList<QGraphicsItem*> selectedList = selectedItems();

    WorldData copyData;
    FileFormats::CreateWorldData(copyData);

    if (!selectedList.isEmpty())
    {
        for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
        {
            QString ObjType = (*it)->data(0).toString();

            if( ObjType == "TILE")
            {
                ItemTile* sourceTile = (ItemTile*)(*it);
                copyData.tiles.push_back(sourceTile->m_data);
                if(cut){
                    sourceTile->removeFromArray();
                    removeItem(*it);
                    delete (*it);
                }
            }
            else
            if( ObjType == "SCENERY")
            {
                ItemScene* sourceScene = (ItemScene *)(*it);
                copyData.scenery.push_back(sourceScene->m_data);
                if(cut){
                    sourceScene->removeFromArray();
                    removeItem(*it);
                    delete (*it);
                }
            }
            else
            if( ObjType == "PATH")
            {
                ItemPath* sourcePath = (ItemPath *)(*it);
                copyData.paths.push_back(sourcePath->m_data);
                if(cut){
                    sourcePath->removeFromArray();
                    removeItem(*it);
                    delete (*it);
                }
            }
            else
            if( ObjType == "LEVEL")
            {
                ItemLevel* sourceLevel = (ItemLevel *)(*it);
                copyData.levels.push_back(sourceLevel->m_data);
                if(cut){
                    sourceLevel->removeFromArray();
                    removeItem(*it);
                    delete (*it);
                }
            }
            else
            if( ObjType == "MUSICBOX")
            {
                ItemMusic* sourceMusic = (ItemMusic *)(*it);
                copyData.music.push_back(sourceMusic->m_data);
                if(cut){
                    sourceMusic->removeFromArray();
                    removeItem(*it);
                    delete (*it);
                }
            }

        }//fetch selected items

        if(cut)
        {
            m_data->meta.modified = true;
            m_history->addRemoveHistory(copyData);
            Debugger_updateItemList();
        }
    }

    return copyData;
}

void WldScene::paste(WorldData BufferIn, QPoint pos)
{
    WorldData newData;
    long baseX, baseY;
    //set first base
    if(!BufferIn.tiles.isEmpty()){
        baseX = BufferIn.tiles[0].x;
        baseY = BufferIn.tiles[0].y;
    }else if(!BufferIn.scenery.isEmpty()){
        baseX = BufferIn.scenery[0].x;
        baseY = BufferIn.scenery[0].y;
    }else if(!BufferIn.paths.isEmpty()){
        baseX = BufferIn.paths[0].x;
        baseY = BufferIn.paths[0].y;
    }else if(!BufferIn.levels.isEmpty()){
        baseX = BufferIn.levels[0].x;
        baseY = BufferIn.levels[0].y;
    }else if(!BufferIn.music.isEmpty()){
        baseX = BufferIn.music[0].x;
        baseY = BufferIn.music[0].y;
    }else{
        //nothing to paste
        return;
    }

    foreach (WorldTerrainTile tile, BufferIn.tiles) {
        if(tile.x<baseX){
            baseX = tile.x;
        }
        if(tile.y<baseY){
            baseY = tile.y;
        }
    }
    foreach (WorldScenery scene, BufferIn.scenery){
        if(scene.x<baseX){
            baseX = scene.x;
        }
        if(scene.y<baseY){
            baseY = scene.y;
        }
    }
    foreach (WorldPathTile path, BufferIn.paths){
        if(path.x<baseX){
            baseX = path.x;
        }
        if(path.y<baseY){
            baseY = path.y;
        }
    }
    foreach (WorldLevelTile level, BufferIn.levels){
        if(level.x<baseX){
            baseX = level.x;
        }
        if(level.y<baseY){
            baseY = level.y;
        }
    }
    foreach (WorldMusicBox music, BufferIn.music){
        if(music.x<baseX){
            baseX = music.x;
        }
        if(music.y<baseY){
            baseY = music.y;
        }
    }


    foreach (WorldTerrainTile tile, BufferIn.tiles){
        //Gen Copy of Tile
        WorldTerrainTile dumpTile = tile;
        dumpTile.x = (long)pos.x() + tile.x - baseX;
        dumpTile.y = (long)pos.y() + tile.y - baseY;
        m_data->tile_array_id++;
        dumpTile.meta.array_id = m_data->tile_array_id;

        placeTile(dumpTile);

        m_data->tiles.push_back(dumpTile);
        newData.tiles.push_back(dumpTile);
    }

    foreach (WorldScenery scene, BufferIn.scenery){
        //Gen Copy of Scenery
        WorldScenery dumpScene = scene;
        dumpScene.x = (long)pos.x() + scene.x - baseX;
        dumpScene.y = (long)pos.y() + scene.y - baseY;
        m_data->scene_array_id++;
        dumpScene.meta.array_id = m_data->scene_array_id;

        placeScenery(dumpScene);

        m_data->scenery.push_back(dumpScene);
        newData.scenery.push_back(dumpScene);
    }

    foreach (WorldPathTile path, BufferIn.paths){
        //Gen Copy of Path
        WorldPathTile dumpPath = path;
        dumpPath.x = (long)pos.x() + path.x - baseX;
        dumpPath.y = (long)pos.y() + path.y - baseY;
        m_data->path_array_id++;
        dumpPath.meta.array_id = m_data->path_array_id;

        placePath(dumpPath);

        m_data->paths.push_back(dumpPath);
        newData.paths.push_back(dumpPath);
    }

    foreach (WorldLevelTile level, BufferIn.levels){
        //Gen Copy of Level
        WorldLevelTile dumpLevel = level;
        dumpLevel.x = (long)pos.x() + level.x - baseX;
        dumpLevel.y = (long)pos.y() + level.y - baseY;
        m_data->level_array_id++;
        dumpLevel.meta.array_id = m_data->level_array_id;

        placeLevel(dumpLevel);

        m_data->levels.push_back(dumpLevel);
        newData.levels.push_back(dumpLevel);
    }

    foreach (WorldMusicBox music, BufferIn.music){
        //Gen Copy of Level
        WorldMusicBox dumpMusic = music;
        dumpMusic.x = (long)pos.x() + music.x - baseX;
        dumpMusic.y = (long)pos.y() + music.y - baseY;
        m_data->musicbox_array_id++;
        dumpMusic.meta.array_id = m_data->musicbox_array_id;

        placeMusicbox(dumpMusic);

        m_data->music.push_back(dumpMusic);
        newData.music.push_back(dumpMusic);
    }

    applyGroupGrid(selectedItems(), true);

    m_data->meta.modified = true;
    m_history->addPlaceHistory(newData);

}
