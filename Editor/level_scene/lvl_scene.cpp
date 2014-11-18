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

#include "lvl_scene.h"
#include "../common_features/app_path.h"

#include "../edit_level/level_edit.h"

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"

#include "edit_modes/mode_hand.h"
#include "edit_modes/mode_select.h"
#include "edit_modes/mode_erase.h"
#include "edit_modes/mode_place.h"
#include "edit_modes/mode_square.h"
#include "edit_modes/mode_line.h"
#include "edit_modes/mode_resize.h"
#include "edit_modes/mode_fill.h"

LvlScene::LvlScene(GraphicsWorkspace * parentView, dataconfigs &configs, LevelData &FileData, QObject *parent) : QGraphicsScene(parent)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);

    //Pointerss
    pConfigs = &configs; // Pointer to Main Configs
    LvlData = &FileData; //Ad pointer to level data
    _viewPort = parentView;

    //Options
    opts.animationEnabled = true;
    opts.collisionsEnabled = true;
    grid = true;

    //Indexes
    index_blocks = pConfigs->index_blocks; //Applaying blocks indexes
    index_bgo = pConfigs->index_bgo;
    index_npc = pConfigs->index_npc;

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

    last_block_arrayID = 0;
    last_bgo_arrayID = 0;
    last_npc_arrayID = 0;

    //Editing process flags
    IsMoved = false;
    haveSelected = false;

    emptyCollisionCheck = false;

    placingItem=0;

    pResizer = NULL;
    isFullSection = false;

    contextMenuOpened = false;

    //Events flags
    resetPosition = false;

    cursor = NULL;
    messageBox = NULL;
    resetCursor();

    //set dummy images if target not exist or wrong
    uBlockImg = QPixmap(ApplicationPath + "/" + "data/unknown_block.png");
    uNpcImg = QPixmap(ApplicationPath + "/" + "data/unknown_npc.png");
    uBgoImg = QPixmap(ApplicationPath + "/" + "data/unknown_bgo.png");


    //set Default Z Indexes
    Z_backImage = -1000; //Background

    //Background-2
    Z_BGOBack2 = -160; // backround BGO

    Z_blockSizable = -150; // sizable blocks

    //Background-1
    Z_BGOBack1 = -100; // backround BGO

    Z_npcBack = -10; // background NPC
    Z_Block = 1; // standart block
    Z_npcStd = 30; // standart NPC
    Z_Player = 35; //player Point

    //Foreground-1
    Z_BGOFore1 = 50; // foreground BGO
    Z_BlockFore = 100; //LavaBlock
    Z_npcFore = 150; // foreground NPC
    //Foreground-2
    Z_BGOFore2 = 160; // foreground BGO

    Z_sys_PhysEnv = 500;
    Z_sys_door = 700;
    Z_sys_interspace1 = 1000; // interSection space layer
    Z_sys_sctBorder = 1020; // section Border

    //HistoryIndex
    historyIndex=0;

    historyChanged = false;

    //Locks
    lock_bgo=false;
    lock_block=false;
    lock_npc=false;
    lock_door=false;
    lock_water=false;

    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));


    //Build edit mode classes
    LVL_ModeHand * modeHand = new LVL_ModeHand(this);
    EditModes.push_back(modeHand);

    LVL_ModeSelect * modeSelect = new LVL_ModeSelect(this);
    EditModes.push_back(modeSelect);

    LVL_ModeResize * modeResize = new LVL_ModeResize(this);
    EditModes.push_back(modeResize);

    LVL_ModeErase * modeErase = new LVL_ModeErase(this);
    EditModes.push_back(modeErase);

    LVL_ModePlace * modePlace = new LVL_ModePlace(this);
    EditModes.push_back(modePlace);

    LVL_ModeSquare * modeSquare = new LVL_ModeSquare(this);
    EditModes.push_back(modeSquare);

    LVL_ModeLine * modeLine = new LVL_ModeLine(this);
    EditModes.push_back(modeLine);

    LVL_ModeFill * modeFill = new LVL_ModeFill(this);
    EditModes.push_back(modeFill);

    CurrentMode = modeSelect;
    CurrentMode->set();
}

LvlScene::~LvlScene()
{
    if(messageBox) delete messageBox;
    uBGs.clear();
    uBGOs.clear();
    uBlocks.clear();
    uNPCs.clear();

    while(!EditModes.isEmpty())
    {
        EditMode *tmp = EditModes.first();
        EditModes.pop_front();
        delete tmp;
    }
}




// ////////////////////Sort///////////////////////////

void LvlScene::sortBlockArray(QVector<LevelBlock > &blocks)
{
    LevelBlock tmp1;
    int total = blocks.size();
    long i;
    unsigned long ymin;
    long ymini;
    long sorted = 0;


        while(sorted < blocks.size())
        {
            ymin = blocks[sorted].array_id;
            ymini = sorted;

            for(i = sorted; i < total; i++)
            {
                if( blocks[i].array_id < ymin )
                {
                    ymin = blocks[i].array_id; ymini = i;
                }
            }
            tmp1 = blocks[ymini];
            blocks[ymini] = blocks[sorted];
            blocks[sorted] = tmp1;
            sorted++;
        }
}

void LvlScene::sortBlockArrayByPos(QVector<LevelBlock > &blocks)
{
    LevelBlock tmp1;
    int total = blocks.size();
    long i;
    long xmin;
    long xmini;
    long sorted = 0;


        while(sorted < blocks.size())
        {
            xmin = blocks[sorted].x;
            xmini = sorted;

            for(i = sorted; i < total; i++)
            {
                if( blocks[i].x < xmin )
                {
                    xmin = blocks[i].x; xmini = i;
                }
            }
            tmp1 = blocks[xmini];
            blocks[xmini] = blocks[sorted];
            blocks[sorted] = tmp1;
            sorted++;
        }
}

void LvlScene::sortBGOArray(QVector<LevelBGO > &bgos)
{
    LevelBGO tmp1;
    int total = bgos.size();
    long i;
    unsigned long ymin;
    unsigned long ymini;
    long sorted = 0;

        while(sorted < bgos.size())
        {
            ymin = bgos[sorted].array_id;
            ymini = sorted;

            for(i = sorted; i < total; i++)
            {
                if( bgos[i].array_id < ymin )
                {
                    ymin = bgos[i].array_id; ymini = i;
                }
            }
            tmp1 = bgos[ymini];
            bgos[ymini] = bgos[sorted];
            bgos[sorted] = tmp1;
            sorted++;
        }
}

