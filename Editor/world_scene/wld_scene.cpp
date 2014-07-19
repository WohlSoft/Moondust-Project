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

WldScene::WldScene(dataconfigs &configs, WorldData &FileData, QObject *parent) : QGraphicsScene(parent)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);

    //Pointerss
    pConfigs = &configs; // Pointer to Main Configs
    WldData = &FileData; //Ad pointer to level data

    //Options
    opts.animationEnabled = true;
    opts.collisionsEnabled = true;
    grid = true;

    //Indexes
    index_tiles = pConfigs->index_wtiles; //Applaying blocks indexes
    index_scenes = pConfigs->index_wscene;
    index_paths = pConfigs->index_wpaths;
    index_levels = pConfigs->index_wlvl;

    //Editing mode
    EditingMode = 0;
    EraserEnabled = false;
    PasteFromBuffer = false;
    disableMoveItems = false;
    DrawMode=false;

    //Editing process flags
    IsMoved = false;
    haveSelected = false;

    placingItem=0;

    pResizer = NULL;

    contextMenuOpened = false;

    //Events flags
    cursor = NULL;
    resetCursor();

    //set dummy images if target not exist or wrong
    uTileImg=QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_tile.png");;
    uSceneImg=QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_scene.png");;
    uPathImg=QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_path.png");;
    uLevelImg=QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_wlvl.png");;

    //set Default Z Indexes
    tileZ=0; // tiles
    sceneZ=5; // scenery
    pathZ=10; // paths
    levelZ=15; // levels
    musicZ=20; // musicboxes

    //HistoryIndex
    historyIndex=0;

    //Locks
    lock_tile=false;
    lock_scene=false;
    lock_path=false;
    lock_level=false;
    lock_musbox=false;

    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

}

WldScene::~WldScene()
{
    uTiles.clear();
    uScenes.clear();
    uPaths.clear();
    uLevels.clear();
}
