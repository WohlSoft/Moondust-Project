/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "lvl_history_manager.h"

#include "edit_modes/mode_hand.h"
#include "edit_modes/mode_select.h"
#include "edit_modes/mode_erase.h"
#include "edit_modes/mode_place.h"
#include "edit_modes/mode_square.h"
#include "edit_modes/mode_circle.h"
#include "edit_modes/mode_line.h"
#include "edit_modes/mode_resize.h"
#include "edit_modes/mode_fill.h"

LvlScene::LvlScene(MainWindow *mw,
                   GraphicsWorkspace *parentView,
                   DataConfig &configs,
                   LevelData &FileData,
                   QObject *parent) :
    QGraphicsScene(parent),
    m_mw(mw),
    m_configs(&configs), // Pointer to Main Configs
    m_data(&FileData), //Add pointer to level data
    m_viewPort(parentView),
    m_subWindow(nullptr),

    //set dummy images if target not exist or wrong
    m_dummyBlockImg(Themes::Image(Themes::dummy_block)),
    m_dummyNpcImg(Themes::Image(Themes::dummy_npc)),
    m_dummyBgoImg(Themes::Image(Themes::dummy_bgo)),

    m_lastBlockArrayID(0),
    m_lastBgoArrayID(0),
    m_lastNpcArrayID(0),

    m_IncrementingNpcSpecialSpin(0),

    //Locks
    m_lockBgo(false),
    m_lockBlock(false),
    m_lockNpc(false),
    m_lockDoor(false),
    m_lockPhysenv(false),

    m_emptyCollisionCheck(false),

    //Editing mode
    m_editMode(MODE_Selecting),
    m_editModeObj(nullptr),

    m_placingItemType(0),

    m_cursorItemImg(nullptr),

    m_labelBox(nullptr),

    //Mouse Events
    m_mouseIsMovedAfterKey(false),  //Is Mouse moved after pressing key

    m_eraserIsEnabled(false),
    m_pastingMode(false),
    m_busyMode(false),
    m_disableMoveItems(false),
    m_contextMenuIsOpened(false),

    m_mouseLeftPressed(false), //Left mouse key is pressed
    m_mouseMidPressed(false),  //Middle mouse key is pressed
    m_mouseRightPressed(false),//Right mouse key is pressed

    m_mouseIsMoved(false), //Mouse was moved with right mouseKey

    m_skipChildMousePressEvent(false),
    m_skipChildMouseMoveEvent(false),
    m_skipChildMousReleaseEvent(false),

    m_resizeBox(nullptr),
    m_captureFullSection(false),

    m_history(new LvlHistoryManager(this, this))

{
    setItemIndexMethod(QGraphicsScene::NoIndex);
    if(parent)
    {
        if(strcmp(parent->metaObject()->className(), LEVEL_EDIT_CLASS) == 0)
            m_subWindow = qobject_cast<LevelEdit *>(parent);
    }

    //set Default Z Indexes
    Z_backImage     = -1000.0; //Background
    //Background-2
    Z_BGOBack2      = -95.0; // backround BGO
    Z_blockSizable  = -90.0; // sizable blocks
    //Background-1
    Z_BGOBack1      = -85.0; // backround BGO
    Z_npcBack       = -75.0; // background NPC
    Z_Block         = -65.0; // standart block
    Z_npcStd        = -45.0; // standart NPC
    Z_Player        = -25.0; //player Point
    //Foreground-1
    Z_BGOFore1      = -20.0; // foreground BGO
    Z_npcFore       = -15.0; // foreground NPC
    Z_BlockFore     = -10.0; //foreground BLOCK
    //Foreground-2
    Z_BGOFore2      = 2.0; // foreground BGO

    //System foreground
    Z_sys_PhysEnv   = 500.0;
    Z_sys_door      = 700.0;
    Z_sys_interspace1 = 1000.0; // interSection space layer
    Z_sys_sctBorder = 1020.0; // section Border

    //Build animators for dummies
    SimpleAnimator *tmpAnimator;
    tmpAnimator = new SimpleAnimator(m_dummyBlockImg, 0);
    m_animatorsBlocks.push_back(tmpAnimator);
    tmpAnimator = new SimpleAnimator(m_dummyBgoImg, 0);
    m_animatorsBGO.push_back(tmpAnimator);

    obj_npc dummyNpc;
    dummyNpc.setup.frames = 1;
    dummyNpc.setup.framestyle = 0;
    dummyNpc.setup.framespeed = 64;
    dummyNpc.setup.width = m_dummyNpcImg.width();
    dummyNpc.setup.height = m_dummyNpcImg.height();
    dummyNpc.setup.gfx_w = m_dummyNpcImg.width();
    dummyNpc.setup.gfx_h = m_dummyNpcImg.height();
    dummyNpc.setup.ani_bidir = false;
    dummyNpc.setup.ani_direct = false;
    dummyNpc.setup.ani_directed_direct = false;
    dummyNpc.setup.custom_animate = false;
    dummyNpc.setup.custom_physics_to_gfx = true;

    AdvNpcAnimator *tmpNpcAnimator = new AdvNpcAnimator(m_dummyNpcImg, dummyNpc);
    m_animatorsNPC.push_back(tmpNpcAnimator);

    //Init default rotation tables
    local_rotation_table_blocks.clear();
    local_rotation_table_bgo.clear();
    local_rotation_table_npc.clear();
    foreach(obj_rotation_table x, m_configs->main_rotation_table)
    {
        if(x.type == ItemTypes::LVL_Block)
            local_rotation_table_blocks[x.id] = x;
        else if(x.type == ItemTypes::LVL_BGO)
            local_rotation_table_bgo[x.id] = x;
        else if(x.type == ItemTypes::LVL_NPC)
            local_rotation_table_npc[x.id] = x;
    }

    resetCursor();

    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    //Build edit mode classes
    LVL_ModeHand *modeHand = new LVL_ModeHand(this);
    m_editModes.push_back(modeHand);

    LVL_ModeSelect *modeSelect = new LVL_ModeSelect(this);
    m_editModes.push_back(modeSelect);

    LVL_ModeResize *modeResize = new LVL_ModeResize(this);
    m_editModes.push_back(modeResize);

    LVL_ModeErase *modeErase = new LVL_ModeErase(this);
    m_editModes.push_back(modeErase);

    LVL_ModePlace *modePlace = new LVL_ModePlace(this);
    m_editModes.push_back(modePlace);

    LVL_ModeSquare *modeSquare = new LVL_ModeSquare(this);
    m_editModes.push_back(modeSquare);

    LVL_ModeCircle *modeCircle = new LVL_ModeCircle(this);
    m_editModes.push_back(modeCircle);

    LVL_ModeLine *modeLine = new LVL_ModeLine(this);
    m_editModes.push_back(modeLine);

    LVL_ModeFill *modeFill = new LVL_ModeFill(this);
    m_editModes.push_back(modeFill);

    m_editModeObj = modeSelect;
    m_editModeObj->set();
}


LvlScene::~LvlScene()
{
    if(m_labelBox) delete m_labelBox;
    m_customBGOs.clear();
    m_customBlocks.clear();
    m_localConfigBackgrounds.clear();
    m_localConfigBGOs.clear();
    m_localConfigBlocks.clear();
    m_localConfigNPCs.clear();

    m_localImages.clear();

    while(!m_editModes.isEmpty())
    {
        EditMode *tmp = m_editModes.first();
        m_editModes.pop_front();
        delete tmp;
    }
}

void LvlScene::drawForeground(QPainter *painter, const QRectF &rect)
{
    QGraphicsScene::drawForeground(painter, rect);
    if(!m_opts.grid_show)
        return;

    int gridSize = int(m_configs->defaultGrid.general);
    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVarLengthArray<QLineF, 100> lines;
    for(qreal x = left; x < rect.right(); x += gridSize)
        lines.append(QLineF(x, rect.top(), x, rect.bottom()));
    for(qreal y = top; y < rect.bottom(); y += gridSize)
        lines.append(QLineF(rect.left(), y, rect.right(), y));

    painter->setRenderHint(QPainter::Antialiasing, false);
    painter->setOpacity(0.5);
    painter->setPen(QPen(QBrush(Qt::black), 1, Qt::SolidLine));
    painter->drawLines(lines.data(), lines.size());
    painter->setPen(QPen(QBrush(Qt::white), 1, Qt::DashLine));
    painter->drawLines(lines.data(), lines.size());
}
