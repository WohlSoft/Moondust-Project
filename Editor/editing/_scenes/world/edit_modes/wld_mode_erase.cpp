/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/main_window_ptr.h>
#include <common_features/item_rectangles.h>
#include <common_features/themes.h>

#include "wld_mode_erase.h"
#include "../wld_scene.h"
#include "../wld_history_manager.h"
#include "../items/item_tile.h"
#include "../items/item_scene.h"
#include "../items/item_path.h"
#include "../items/item_level.h"
#include "../items/item_music.h"

WLD_ModeErase::WLD_ModeErase(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Erase", parentScene, parent)
{}


WLD_ModeErase::~WLD_ModeErase()
{}


void WLD_ModeErase::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->clearSelection();
    s->resetCursor();
    s->resetResizers();
    s->m_pointSelector.unserPointSelector();

    s->m_eraserIsEnabled = false;
    s->m_pastingMode = false;
    s->m_busyMode = false;
    s->m_disableMoveItems = false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_erasing));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
}

void WLD_ModeErase::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(mouseEvent->buttons() & Qt::RightButton)
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    if(s->m_cursorItemImg)
    {
        s->m_cursorItemImg->show();
        s->m_cursorItemImg->setPos(mouseEvent->scenePos());
    }

    s->m_skipChildMousePressEvent = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;

    QList<QGraphicsItem *> selectedList = s->selectedItems();
    if(!selectedList.isEmpty())
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
    }

    s->m_eraserIsEnabled = true;
}

void WLD_ModeErase::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->m_cursorItemImg) s->m_cursorItemImg->setPos(mouseEvent->scenePos());
    if(s->m_eraserIsEnabled) // Remove All items, placed under Cursor
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
    }
}

void WLD_ModeErase::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(!s->m_overwritedItems.tiles.isEmpty() ||
       !s->m_overwritedItems.scenery.isEmpty() ||
       !s->m_overwritedItems.paths.isEmpty() ||
       !s->m_overwritedItems.levels.isEmpty() ||
       !s->m_overwritedItems.music.isEmpty())
    {
        s->m_history->addRemoveHistory(s->m_overwritedItems);
        s->m_overwritedItems.tiles.clear();
        s->m_overwritedItems.scenery.clear();
        s->m_overwritedItems.paths.clear();
        s->m_overwritedItems.levels.clear();
        s->m_overwritedItems.music.clear();
    }

    QList<QGraphicsItem *> selectedList = s->selectedItems();

    // check for grid snap
    if((!selectedList.isEmpty()) && (s->m_mouseIsMoved))
    {
        s->removeWldItems(selectedList);
        selectedList = s->selectedItems();
        s->Debugger_updateItemList();
    }

    s->m_eraserIsEnabled = false;
}

void WLD_ModeErase::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent)
}


void WLD_ModeErase::keyRelease(QKeyEvent *keyEvent)
{
    switch(keyEvent->key())
    {
    case(Qt::Key_Escape):
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        break;
    default:
        break;
    }
}
