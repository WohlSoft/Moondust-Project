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
#include "../common_features/app_path.h"

#include "item_tile.h"
#include "item_scene.h"
#include "item_path.h"
#include "item_level.h"
#include "item_music.h"

#include "edit_modes/wld_mode_hand.h"
#include "edit_modes/wld_mode_select.h"
#include "edit_modes/wld_mode_erase.h"
#include "edit_modes/wld_mode_place.h"
#include "edit_modes/wld_mode_square.h"
#include "edit_modes/wld_mode_line.h"
#include "edit_modes/wld_mode_resize.h"
#include "edit_modes/wld_mode_setpoint.h"

WldScene::WldScene(GraphicsWorkspace * parentView, dataconfigs &configs, WorldData &FileData, QObject *parent) : QGraphicsScene(parent)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);

    //Pointerss
    pConfigs = &configs; // Pointer to Main Configs
    WldData = &FileData; //Ad pointer to level data
    _viewPort = parentView;

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

    mouseLeft=false; //Left mouse key is pressed
    mouseMid=false;  //Middle mouse key is pressed
    mouseRight=false;//Right mouse key is pressed

    mouseMoved=false; //Mouse was moved with right mouseKey

    MousePressEventOnly=false;
    MouseMoveEventOnly=false;
    MouseReleaseEventOnly=false;

    last_tile_arrayID=0;
    last_scene_arrayID=0;
    last_path_arrayID=0;
    last_level_arrayID=0;
    last_musicbox_arrayID=0;

    isSelectionDialog=false;

    //Editing process flags
    IsMoved = false;
    haveSelected = false;

    emptyCollisionCheck = false;

    placingItem=0;

    pResizer = NULL;

    contextMenuOpened = false;

    selectedPoint = QPoint(0, 0);
    selectedPointNotUsed=true;
    pointTarget = NULL;
    pointAnimation = NULL;
    pointImg=QPixmap(":/images/set_point.png");

    cursor = NULL;
    resetCursor();
    messageBox = NULL;

    //set dummy images if target not exist or wrong
    uTileImg=QPixmap(ApplicationPath + "/" + "data/unknown_tile.png");
    uSceneImg=QPixmap(ApplicationPath + "/" + "data/unknown_scene.png");
    uPathImg=QPixmap(ApplicationPath + "/" + "data/unknown_path.png");
    uLevelImg=QPixmap(ApplicationPath + "/" + "data/unknown_wlvl.png");

    musicBoxImg=QPixmap(ApplicationPath + "/" + "data/music_box.png");

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

    long padding=100000;

    QGraphicsRectItem * bigRect = addRect(-padding, -padding, padding*2, padding*2, QPen(Qt::transparent), QBrush(Qt::transparent));
    bigRect->setZValue(-10000000000);


    //Build edit mode classes
    WLD_ModeHand * modeHand = new WLD_ModeHand(this);
    EditModes.push_back(modeHand);

    WLD_ModeSelect * modeSelect = new WLD_ModeSelect(this);
    EditModes.push_back(modeSelect);

    WLD_ModeResize * modeResize = new WLD_ModeResize(this);
    EditModes.push_back(modeResize);

    WLD_ModeErase * modeErase = new WLD_ModeErase(this);
    EditModes.push_back(modeErase);

    WLD_ModePlace * modePlace = new WLD_ModePlace(this);
    EditModes.push_back(modePlace);

    WLD_ModeSquare * modeSquare = new WLD_ModeSquare(this);
    EditModes.push_back(modeSquare);

    WLD_ModeLine * modeLine = new WLD_ModeLine(this);
    EditModes.push_back(modeLine);

    WLD_ModeSetPoint * modeSetPoint = new WLD_ModeSetPoint(this);
    EditModes.push_back(modeSetPoint);

    CurrentMode = modeSelect;
    CurrentMode->set();
}

WldScene::~WldScene()
{
    if(messageBox) delete messageBox;
    uTiles.clear();
    uScenes.clear();
    uPaths.clear();
    uLevels.clear();

    while(!EditModes.isEmpty())
    {
        EditMode *tmp = EditModes.first();
        EditModes.pop_front();
        delete tmp;
    }
}
