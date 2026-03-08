/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <mainwindow.h>
#include <common_features/themes.h>
#include <common_features/item_rectangles.h>

#include "mode_erase.h"

#include "../lvl_history_manager.h"

LVL_ModeErase::LVL_ModeErase(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Erase", parentScene, parent)
{}


LVL_ModeErase::~LVL_ModeErase()
{}


void LVL_ModeErase::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->clearSelection();
    s->resetCursor();
    s->resetResizers();

    s->setEditFlagEraser(false);
    s->setEditFlagPasteMode(false);
    s->setEditFlagBusyMode(false);
    s->setEditFlagNoMoveItems(false);

    auto *vp = s->curViewPort();
    vp->setInteractive(true);
    vp->setCursor(Themes::Cursor(Themes::cursor_erasing));
    vp->setDragMode(QGraphicsView::NoDrag);
}

void LVL_ModeErase::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene)
        return;

    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(mouseEvent->buttons() & Qt::RightButton)
    {
        s->mw()->on_actionSelect_triggered();
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

    s->setEditFlagEraser(true);
}

void LVL_ModeErase::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->m_cursorItemImg)
        s->m_cursorItemImg->setPos(mouseEvent->scenePos());

    if(s->getEditFlagEraser()) // Remove All items, placed under Cursor
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
    }
}

void LVL_ModeErase::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!s->m_overwritedItems.blocks.isEmpty() ||
       !s->m_overwritedItems.bgo.isEmpty() ||
       !s->m_overwritedItems.npc.isEmpty() ||
       !s->m_overwritedItems.physez.isEmpty() ||
       !s->m_overwritedItems.doors.isEmpty())
    {
        s->m_history->addRemove(s->m_overwritedItems);
        s->m_overwritedItems.blocks.clear();
        s->m_overwritedItems.bgo.clear();
        s->m_overwritedItems.npc.clear();
        s->m_overwritedItems.physez.clear();
        s->m_overwritedItems.doors.clear();
    }

    QList<QGraphicsItem *> selectedList = s->selectedItems();

    // check for grid snap
    if((!selectedList.isEmpty()) && (s->m_mouseIsMoved))
    {
        s->removeLvlItems(selectedList);
        selectedList = s->selectedItems();
        s->Debugger_updateItemList();
    }

    s->setEditFlagEraser(false);
}

void LVL_ModeErase::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent)
}


void LVL_ModeErase::keyRelease(QKeyEvent *keyEvent)
{
    switch(keyEvent->key())
    {
    case(Qt::Key_Escape):
    {
        LvlScene *s = dynamic_cast<LvlScene *>(scene);
        if(s)
            s->mw()->on_actionSelect_triggered();
        break;
    }
    default:
        break;
    }
}
