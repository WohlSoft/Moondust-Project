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

#include <common_features/themes.h>
#include <common_features/mainwinconnect.h>
#include <common_features/item_rectangles.h>

#include "mode_erase.h"

#include "../lvl_scene.h"
#include "../items/item_bgo.h"
#include "../items/item_block.h"
#include "../items/item_npc.h"
#include "../items/item_water.h"
#include "../items/item_playerpoint.h"
#include "../items/item_door.h"

LVL_ModeErase::LVL_ModeErase(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Erase", parentScene, parent)
{}


LVL_ModeErase::~LVL_ModeErase()
{}


void LVL_ModeErase::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetCursor();
    s->resetResizers();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_erasing));
    s->_viewPort->setDragMode(QGraphicsView::RubberBandDrag);
}

void LVL_ModeErase::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->IsMoved = true;
        return;
    }

    if(s->cursor){
       s->cursor->show();
       s->cursor->setPos(mouseEvent->scenePos());
    }

    s->MousePressEventOnly = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;

    QList<QGraphicsItem*> selectedList = s->selectedItems();
    if (!selectedList.isEmpty())
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
        s->EraserEnabled=true;
    }
}

void LVL_ModeErase::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->cursor) s->cursor->setPos(mouseEvent->scenePos());
    if (s->EraserEnabled)// Remove All items, placed under Cursor
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
    }
}

void LVL_ModeErase::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!s->overwritedItems.blocks.isEmpty()||
        !s->overwritedItems.bgo.isEmpty()||
        !s->overwritedItems.npc.isEmpty() )
    {
        s->addRemoveHistory(s->overwritedItems);
        s->overwritedItems.blocks.clear();
        s->overwritedItems.bgo.clear();
        s->overwritedItems.npc.clear();
    }
    QList<QGraphicsItem*> selectedList = s->selectedItems();

    // check for grid snap
    if ((!selectedList.isEmpty())&&(s->mouseMoved))
    {
        s->removeLvlItems(selectedList);
        selectedList = s->selectedItems();
        s->Debugger_updateItemList();
    }

    s->EraserEnabled = false;
}

void LVL_ModeErase::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}


void LVL_ModeErase::keyRelease(QKeyEvent *keyEvent)
{
    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }
}
