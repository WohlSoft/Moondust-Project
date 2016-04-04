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

#include <common_features/app_path.h>
#include <common_features/themes.h>
#include <editing/edit_level/level_edit.h>

#include "lvl_scene.h"
#include "items/item_block.h"
#include "items/item_bgo.h"
#include "items/item_npc.h"

#include "edit_modes/mode_hand.h"
#include "edit_modes/mode_select.h"
#include "edit_modes/mode_erase.h"
#include "edit_modes/mode_place.h"
#include "edit_modes/mode_square.h"
#include "edit_modes/mode_circle.h"
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
    _edit=NULL;

    if(parent)
    {
        if(strcmp(parent->metaObject()->className(), "LevelEdit")==0)
        {
            _edit = qobject_cast<LevelEdit*>(parent);
        }
    }

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
    Z_npcFore = 100; // foreground NPC
    Z_BlockFore = 150; //foreground BLOCK
    //Foreground-2
    Z_BGOFore2 = 160; // foreground BGO

    //System foreground
    Z_sys_PhysEnv = 500;
    Z_sys_door = 700;
    Z_sys_interspace1 = 1000; // interSection space layer
    Z_sys_sctBorder = 1020; // section Border

    //set dummy images if target not exist or wrong
    uBlockImg = Themes::Image(Themes::dummy_block);
    uNpcImg =   Themes::Image(Themes::dummy_npc);
    uBgoImg =   Themes::Image(Themes::dummy_bgo);


    //Build animators for dummies
    SimpleAnimator * tmpAnimator;
        tmpAnimator = new SimpleAnimator(uBlockImg, 0);
    animates_Blocks.push_back( tmpAnimator );
        tmpAnimator = new SimpleAnimator(uBgoImg, 0);
    animates_BGO.push_back( tmpAnimator );

        obj_npc dummyNpc;
        dummyNpc.frames = 1;
        dummyNpc.framestyle = 0;
        dummyNpc.framespeed = 64;
        dummyNpc.width = uNpcImg.width();
        dummyNpc.height = uNpcImg.height();
        dummyNpc.gfx_w = uNpcImg.width();
        dummyNpc.gfx_h = uNpcImg.height();
        dummyNpc.ani_bidir = false;
        dummyNpc.ani_direct = false;
        dummyNpc.ani_directed_direct = false;
        dummyNpc.custom_animate = false;
        dummyNpc.custom_physics_to_gfx = true;
        AdvNpcAnimator * tmpNpcAnimator = new AdvNpcAnimator(uNpcImg, dummyNpc);
    animates_NPC.push_back( tmpNpcAnimator );

    //Init default rotation tables
    local_rotation_table_blocks.clear();
    local_rotation_table_bgo.clear();
    local_rotation_table_npc.clear();
    foreach(obj_rotation_table x, pConfigs->main_rotation_table)
    {
        if(x.type==ItemTypes::LVL_Block)
            local_rotation_table_blocks[x.id]=x;
        else
        if(x.type==ItemTypes::LVL_BGO)
            local_rotation_table_bgo[x.id]=x;
        else
        if(x.type==ItemTypes::LVL_NPC)
            local_rotation_table_npc[x.id]=x;
    }

    IncrementingNpcSpecialSpin = 0;

    last_block_arrayID = 0;
    last_bgo_arrayID = 0;
    last_npc_arrayID = 0;


    //Locks
    lock_bgo=false;
    lock_block=false;
    lock_npc=false;
    lock_door=false;
    lock_water=false;


    //Editing mode
    EditingMode = MODE_Selecting;
    EraserEnabled = false;
    PasteFromBuffer = false;
    disableMoveItems = false;
    DrawMode=false;
    placingItem=0;


    //Mouse Events
    IsMoved = false;  //Is Mouse moved after pressing key
    mouseMoved=false; //Mouse was moved with right mouseKey
    haveSelected = false;
    contextMenuOpened = false;
    mouseLeft=false; //Left mouse key is pressed
    mouseMid=false;  //Middle mouse key is pressed
    mouseRight=false;//Right mouse key is pressed
    MousePressEventOnly=false;
    MouseMoveEventOnly=false;
    MouseReleaseEventOnly=false;

    emptyCollisionCheck = false;

    pResizer = NULL;
    isFullSection = false;

    cursor = NULL;
    messageBox = NULL;
    resetCursor();

    //HistoryIndex
    historyIndex=0;
    historyChanged = false;

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

    LVL_ModeCircle * modeCircle = new LVL_ModeCircle(this);
    EditModes.push_back(modeCircle);

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
    custom_BGOs.clear();
    custom_Blocks.clear();
    uBGs.clear();
    uBGOs.clear();
    uBlocks.clear();
    uNPCs.clear();

    custom_images.clear();

    while(!EditModes.isEmpty())
    {
        EditMode *tmp = EditModes.first();
        EditModes.pop_front();
        delete tmp;
    }
}

void LvlScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);
    if(!opts.grid_show) return;

    int gridSize=pConfigs->default_grid;
    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVarLengthArray<QLineF, 100> lines;
    for (qreal x = left; x < rect.right(); x += gridSize)
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for (qreal y = top; y < rect.bottom(); y += gridSize)
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setOpacity(0.5);
    painter->setPen(QPen(QBrush(Qt::black), 1, Qt::SolidLine));
    painter->drawLines(lines.data(), lines.size());
    painter->setPen(QPen(QBrush(Qt::white), 1, Qt::DashLine));
    painter->drawLines(lines.data(), lines.size());
}
