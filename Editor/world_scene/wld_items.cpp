/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "wld_scene.h"
#include "../edit_world/world_edit.h"

#include "item_tile.h"
#include "item_scene.h"
#include "item_path.h"
#include "item_level.h"
#include "item_music.h"

#include "../common_features/grid.h"


QPoint WldScene::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    if((grid)&&(gridSize>0))
        return Grid::applyGrid(source, gridSize, gridOffset);
    else
        return source;
}

////////////////////////////////// Place new ////////////////////////////////

void WldScene::placeTile(WorldTiles &tile, bool toGrid)
{
    bool noimage=true, found=false;
    int j;
    long animator=0;

    ItemTile *TileItem = new ItemTile;

    //Check Index exists
    if(tile.id < (unsigned int)index_tiles.size())
    {
        j = index_tiles[tile.id].i;
        animator = index_tiles[tile.id].ai;

        if(j<pConfigs->main_wtiles.size())
        {
            if(pConfigs->main_wtiles[j].id == tile.id)
            {
                found=true;noimage=false;
            }
        }
    }


    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_wtiles.size();j++)
        {
            if(pConfigs->main_wtiles[j].id==tile.id)
            {
                noimage=false;
                //if(!isUser)
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_wtiles.size())
        {
            j=0;
        }
    }

    TileItem->gridSize = pConfigs->default_grid;

    addItem(TileItem);

    //Set pointers
    TileItem->setScenePoint(this);

    TileItem->setContextMenu(tileMenu);

    TileItem->setAnimator(animator);

    if((!noimage) && (pConfigs->main_wtiles[j].animated))
    {
        TileItem->setData(4, "animated");
    }

    QPoint newPos = QPoint(tile.x, tile.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(tile.x, tile.y), TileItem->gridSize);
        tile.x = newPos.x();
        TileItem->tileData.x = newPos.x();
        tile.y = newPos.y();
        TileItem->tileData.y = newPos.y();
    }

    TileItem->setPos(QPointF(newPos));

    TileItem->setZValue(tileZ);

    TileItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_tile));
    TileItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_tile));

    TileItem->setData(0, "TILE");
    TileItem->setData(1, QString::number(tile.id) );
    TileItem->setData(2, QString::number(tile.array_id) );

    if(PasteFromBuffer) TileItem->setSelected(true);
}


void WldScene::placeScenery(WorldScenery &scenery, bool toGrid)
{
    bool noimage=true, found=false;
    int j;
    long animator=0;

    ItemScene *SceneItem = new ItemScene;

    //Check Index exists
    if(scenery.id < (unsigned int)index_scenes.size())
    {
        j = index_scenes[scenery.id].i;
        animator = index_scenes[scenery.id].ai;

        if(j<pConfigs->main_wscene.size())
        {
            if(pConfigs->main_wscene[j].id == scenery.id)
            {
                found=true;noimage=false;
            }
        }
    }


    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_wscene.size();j++)
        {
            if(pConfigs->main_wscene[j].id==scenery.id)
            {
                noimage=false;
                //if(!isUser)
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_wscene.size())
        {
            j=0;
        }
    }

    SceneItem->gridSize = qRound(qreal(pConfigs->default_grid)/2);

    addItem(SceneItem);

    //Set pointers
    SceneItem->setScenePoint(this);

    SceneItem->setContextMenu(sceneMenu);

    SceneItem->setAnimator(animator);

    if((!noimage) && (pConfigs->main_wscene[j].animated))
    {
        SceneItem->setData(4, "animated");
    }

    QPoint newPos = QPoint(scenery.x, scenery.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(scenery.x, scenery.y), SceneItem->gridSize);
        scenery.x = newPos.x();
        SceneItem->sceneData.x = newPos.x();
        scenery.y = newPos.y();
        SceneItem->sceneData.y = newPos.y();
    }

    SceneItem->setPos(QPointF(newPos));

    SceneItem->setZValue(sceneZ);

    SceneItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_scene));
    SceneItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_scene));

    SceneItem->setData(0, "SCENERY");
    SceneItem->setData(1, QString::number(scenery.id) );
    SceneItem->setData(2, QString::number(scenery.array_id) );

    if(PasteFromBuffer) SceneItem->setSelected(true);
}



void WldScene::placePath(WorldPaths &path, bool toGrid)
{
    bool noimage=true, found=false;
    int j;
    long animator=0;

    ItemPath *PathItem = new ItemPath;

    //Check Index exists
    if(path.id < (unsigned int)index_paths.size())
    {
        j = index_paths[path.id].i;
        animator = index_paths[path.id].ai;

        if(j<pConfigs->main_wpaths.size())
        {
            if(pConfigs->main_wpaths[j].id == path.id)
            {
                found=true;noimage=false;
            }
        }
    }


    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_wpaths.size();j++)
        {
            if(pConfigs->main_wpaths[j].id==path.id)
            {
                noimage=false;
                //if(!isUser)
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_wpaths.size())
        {
            j=0;
        }
    }

    PathItem->gridSize = pConfigs->default_grid;

    addItem(PathItem);

    //Set pointers
    PathItem->setScenePoint(this);

    PathItem->setContextMenu(pathMenu);

    PathItem->setAnimator(animator);

    if((!noimage) && (pConfigs->main_wpaths[j].animated))
    {
        PathItem->setData(4, "animated");
    }

    QPoint newPos = QPoint(path.x, path.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(path.x, path.y), PathItem->gridSize);
        path.x = newPos.x();
        PathItem->pathData.x = newPos.x();
        path.y = newPos.y();
        PathItem->pathData.y = newPos.y();
    }

    PathItem->setPos(QPointF(newPos));

    PathItem->setZValue(pathZ);

    PathItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_scene));
    PathItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_scene));

    PathItem->setData(0, "PATH");
    PathItem->setData(1, QString::number(path.id) );
    PathItem->setData(2, QString::number(path.array_id) );

    if(PasteFromBuffer) PathItem->setSelected(true);
}


void WldScene::placeLevel(WorldLevels &level, bool toGrid)
{
    bool noimage=true, found=false;
    int j;
    long animator=0;

    ItemLevel *LevelItem = new ItemLevel;

    //Check Index exists
    if(level.id < (unsigned int)index_levels.size())
    {
        j = index_levels[level.id].i;
        animator = index_levels[level.id].ai;

        if(j<pConfigs->main_wlevels.size())
        {
            if(pConfigs->main_wlevels[j].id == level.id)
            {
                found=true;noimage=false;
            }
        }
    }


    //if Index found
    if(!found)
    {
        for(j=0;j<pConfigs->main_wlevels.size();j++)
        {
            if(pConfigs->main_wlevels[j].id==level.id)
            {
                noimage=false;
                //if(!isUser)
            }
        }
    }

    if(noimage)
    {
        if(j >= pConfigs->main_wlevels.size())
        {
            j=0;
        }
    }

    LevelItem->gridSize = pConfigs->default_grid;

    addItem(LevelItem);

    //Set pointers
    LevelItem->setScenePoint(this);

    LevelItem->setContextMenu(levelMenu);

    LevelItem->setAnimator(animator);

    if((!noimage) && (pConfigs->main_wlevels[j].animated))
    {
        LevelItem->setData(4, "animated");
    }

    QPoint newPos = QPoint(level.x, level.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(level.x, level.y), LevelItem->gridSize);
        level.x = newPos.x();
        LevelItem->levelData.x = newPos.x();
        level.y = newPos.y();
        LevelItem->levelData.y = newPos.y();
    }

    LevelItem->setPos(QPointF(newPos));

    LevelItem->setZValue(levelZ);

    LevelItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_level));
    LevelItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_level));

    LevelItem->setData(0, "LEVEL");
    LevelItem->setData(1, QString::number(level.id) );
    LevelItem->setData(2, QString::number(level.array_id) );

    if(PasteFromBuffer) LevelItem->setSelected(true);
}


void WldScene::placeMusicbox(WorldMusic &musicbox, bool toGrid)
{

    //Dummy!
    if((musicbox.id) && (toGrid)) goto meow;
    meow:;

}
