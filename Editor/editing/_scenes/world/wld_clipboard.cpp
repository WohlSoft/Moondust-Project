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

#include <editing/edit_level/level_edit.h>

#include "wld_scene.h"
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"

//Copy selected items into clipboard
WorldData WldScene::copy(bool cut)
{

    //Get Selected Items
    QList<QGraphicsItem*> selectedList = selectedItems();

    WorldData copyData;

    if (!selectedList.isEmpty())
    {
        for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
        {
            QString ObjType = (*it)->data(0).toString();

            if( ObjType == "TILE")
            {
                ItemTile* sourceTile = (ItemTile*)(*it);
                copyData.tiles.push_back(sourceTile->tileData);
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
                copyData.scenery.push_back(sourceScene->sceneData);
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
                copyData.paths.push_back(sourcePath->pathData);
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
                copyData.levels.push_back(sourceLevel->levelData);
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
                copyData.music.push_back(sourceMusic->musicData);
                if(cut){
                    sourceMusic->removeFromArray();
                    removeItem(*it);
                    delete (*it);
                }
            }

        }//fetch selected items

        if(cut)
        {
            WldData->modified = true;
            //addRemoveHistory(copyData);
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

    foreach (WorldTiles tile, BufferIn.tiles) {
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
    foreach (WorldPaths path, BufferIn.paths){
        if(path.x<baseX){
            baseX = path.x;
        }
        if(path.y<baseY){
            baseY = path.y;
        }
    }
    foreach (WorldLevels level, BufferIn.levels){
        if(level.x<baseX){
            baseX = level.x;
        }
        if(level.y<baseY){
            baseY = level.y;
        }
    }
    foreach (WorldMusic music, BufferIn.music){
        if(music.x<baseX){
            baseX = music.x;
        }
        if(music.y<baseY){
            baseY = music.y;
        }
    }


    foreach (WorldTiles tile, BufferIn.tiles){
        //Gen Copy of Tile
        WorldTiles dumpTile = tile;
        dumpTile.x = (long)pos.x() + tile.x - baseX;
        dumpTile.y = (long)pos.y() + tile.y - baseY;
        WldData->tile_array_id++;
        dumpTile.array_id = WldData->tile_array_id;

        placeTile(dumpTile);

        WldData->tiles.push_back(dumpTile);
        newData.tiles.push_back(dumpTile);
    }

    foreach (WorldScenery scene, BufferIn.scenery){
        //Gen Copy of Scenery
        WorldScenery dumpScene = scene;
        dumpScene.x = (long)pos.x() + scene.x - baseX;
        dumpScene.y = (long)pos.y() + scene.y - baseY;
        WldData->scene_array_id++;
        dumpScene.array_id = WldData->scene_array_id;

        placeScenery(dumpScene);

        WldData->scenery.push_back(dumpScene);
        newData.scenery.push_back(dumpScene);
    }

    foreach (WorldPaths path, BufferIn.paths){
        //Gen Copy of Path
        WorldPaths dumpPath = path;
        dumpPath.x = (long)pos.x() + path.x - baseX;
        dumpPath.y = (long)pos.y() + path.y - baseY;
        WldData->path_array_id++;
        dumpPath.array_id = WldData->path_array_id;

        placePath(dumpPath);

        WldData->paths.push_back(dumpPath);
        newData.paths.push_back(dumpPath);
    }

    foreach (WorldLevels level, BufferIn.levels){
        //Gen Copy of Level
        WorldLevels dumpLevel = level;
        dumpLevel.x = (long)pos.x() + level.x - baseX;
        dumpLevel.y = (long)pos.y() + level.y - baseY;
        WldData->level_array_id++;
        dumpLevel.array_id = WldData->level_array_id;

        placeLevel(dumpLevel);

        WldData->levels.push_back(dumpLevel);
        newData.levels.push_back(dumpLevel);
    }

    foreach (WorldMusic music, BufferIn.music){
        //Gen Copy of Level
        WorldMusic dumpMusic = music;
        dumpMusic.x = (long)pos.x() + music.x - baseX;
        dumpMusic.y = (long)pos.y() + music.y - baseY;
        WldData->musicbox_array_id++;
        dumpMusic.array_id = WldData->musicbox_array_id;

        placeMusicbox(dumpMusic);

        WldData->music.push_back(dumpMusic);
        newData.music.push_back(dumpMusic);
    }

    applyGroupGrid(selectedItems(), true);

    WldData->modified = true;
    addPlaceHistory(newData);

}
