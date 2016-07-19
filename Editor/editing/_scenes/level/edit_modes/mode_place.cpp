/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../lvl_history_manager.h"
#include "../lvl_item_placing.h"

#include "mode_place.h"

LVL_ModePlace::LVL_ModePlace(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Placing", parentScene, parent)
{}

LVL_ModePlace::~LVL_ModePlace()
{}

void LVL_ModePlace::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->clearSelection();
    s->resetResizers();

    s->m_eraserIsEnabled=false;
    s->m_pastingMode=false;
    s->m_busyMode=true;
    s->m_disableMoveItems=false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_placing));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->m_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->m_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModePlace::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        item_rectangles::clearArray();
        s->m_mw->on_actionSelect_triggered();
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    s->m_lastBlockArrayID=s->m_data->blocks_array_id;
    s->m_lastBgoArrayID=s->m_data->bgo_array_id;
    s->m_lastNpcArrayID=s->m_data->npc_array_id;

    if(LvlPlacingItems::npcSpecialAutoIncrement)
        s->m_IncrementingNpcSpecialSpin = LvlPlacingItems::npcSpecialAutoIncrement_begin;

    if(s->m_cursorItemImg)
    {
        s->m_cursorItemImg->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                           QPoint(LvlPlacingItems::c_offset_x,
                                                  LvlPlacingItems::c_offset_y),
                                           LvlPlacingItems::gridSz,
                                           LvlPlacingItems::gridOffset)));
    }

    if(s->m_placingItemType != LvlScene::PLC_PlayerPoint)
    {
        s->placeItemUnderCursor();
        s->Debugger_updateItemList();
    }

    s->m_skipChildMousePressEvent = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;
}

void LVL_ModePlace::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->clearSelection();

    if((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")||
         (mouseEvent->modifiers() & Qt::ControlModifier) )
        s->setLabelBoxItem(true, mouseEvent->scenePos(),
         ((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")?
            LvlPlacingItems::layer + ", ":"") +
                                   (s->m_cursorItemImg?
                                        (
                                   QString::number( s->m_cursorItemImg->scenePos().toPoint().x() ) + "x" +
                                   QString::number( s->m_cursorItemImg->scenePos().toPoint().y() )
                                        )
                                            :"")
                                   );
    else
        s->setLabelBoxItem(false);

    if(s->m_cursorItemImg)
    {
               s->m_cursorItemImg->setPos( QPointF(s->applyGrid( QPointF(mouseEvent->scenePos()-
                                                   QPointF(LvlPlacingItems::c_offset_x,
                                                          LvlPlacingItems::c_offset_y)).toPoint(),
                                                 LvlPlacingItems::gridSz,
                                                 LvlPlacingItems::gridOffset)));
               s->m_cursorItemImg->show();
    }
    if(( mouseEvent->buttons() & Qt::LeftButton ) && (s->m_placingItemType != LvlScene::PLC_PlayerPoint))
    {
        s->placeItemUnderCursor();
        s->Debugger_updateItemList();
    }
}

void LVL_ModePlace::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->m_placingItemType == LvlScene::PLC_Door)
    {
        s->m_mw->on_actionSelect_triggered();
        dontCallEvent = true;
        return;
    }
    else
    {
        if(s->m_placingItemType == LvlScene::PLC_PlayerPoint)
        {
            s->placeItemUnderCursor();
            s->Debugger_updateItemList();
        }

        if(!s->overwritedItems.blocks.isEmpty()||
            !s->overwritedItems.bgo.isEmpty()||
            !s->overwritedItems.npc.isEmpty() )
        {
            s->m_history->addOverwrite(s->overwritedItems, s->placingItems);
            s->overwritedItems.blocks.clear();
            s->overwritedItems.bgo.clear();
            s->overwritedItems.npc.clear();
            s->placingItems.blocks.clear();
            s->placingItems.bgo.clear();
            s->placingItems.npc.clear();
        }
        else
        if(!s->placingItems.blocks.isEmpty()||
                !s->placingItems.bgo.isEmpty()||
                !s->placingItems.npc.isEmpty())
        {
            s->m_history->addPlace(s->placingItems);
            s->placingItems.blocks.clear();
            s->placingItems.bgo.clear();
            s->placingItems.npc.clear();
        }
    }
}

void LVL_ModePlace::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void LVL_ModePlace::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
        {
            LvlScene *s = dynamic_cast<LvlScene *>(scene);
            if(s) s->m_mw->on_actionSelect_triggered();
            break;
        }
        default:
            break;
    }
}
