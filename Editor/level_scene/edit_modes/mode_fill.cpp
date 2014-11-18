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
#include "mode_fill.h"

#include "../lvl_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"
#include "../lvl_item_placing.h"

#include "../item_bgo.h"
#include "../item_block.h"
#include "../item_npc.h"
#include "../item_water.h"
#include "../item_playerpoint.h"
#include "../item_door.h"

LVL_ModeFill::LVL_ModeFill(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Fill", parentScene, parent)
{

}

LVL_ModeFill::~LVL_ModeFill()
{}

void LVL_ModeFill::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetResizers();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Qt::CrossCursor);
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModeFill::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);
    if( mouseEvent->buttons() & Qt::RightButton )
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        return;
    }
    if(! mouseEvent->buttons() & Qt::LeftButton)
        return;

    if(s->placingItem == LvlScene::PLC_Block){
        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint(),
                               LvlPlacingItems::gridSz,
                               LvlPlacingItems::gridOffset);
        LvlPlacingItems::blockSet.x = hw.x();
        LvlPlacingItems::blockSet.y = hw.y();

        s->LvlData->blocks_array_id++;

        LvlPlacingItems::blockSet.array_id = s->LvlData->blocks_array_id;
        s->LvlData->blocks.push_back(LvlPlacingItems::blockSet);

        s->placeBlock(LvlPlacingItems::blockSet, true);
    }


    Q_UNUSED(s)
}

void LVL_ModeFill::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);

}

void LVL_ModeFill::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);

}

void LVL_ModeFill::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void LVL_ModeFill::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }

}
