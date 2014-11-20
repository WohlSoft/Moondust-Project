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

#include "mode_place.h"

#include "../lvl_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"
#include "../lvl_item_placing.h"

#include "../../common_features/themes.h"

LVL_ModePlace::LVL_ModePlace(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Placing", parentScene, parent)
{}

LVL_ModePlace::~LVL_ModePlace()
{}

void LVL_ModePlace::set()
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
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_placing));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModePlace::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        item_rectangles::clearArray();
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->IsMoved = true;
        return;
    }

    s->last_block_arrayID=s->LvlData->blocks_array_id;
    s->last_bgo_arrayID=s->LvlData->bgo_array_id;
    s->last_npc_arrayID=s->LvlData->npc_array_id;

    if(s->cursor)
    {
        s->cursor->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                           QPoint(LvlPlacingItems::c_offset_x,
                                                  LvlPlacingItems::c_offset_y),
                                           LvlPlacingItems::gridSz,
                                           LvlPlacingItems::gridOffset)));
    }

    s->placeItemUnderCursor();
    s->Debugger_updateItemList();

    s->MousePressEventOnly = true;
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
        s->setMessageBoxItem(true, mouseEvent->scenePos(),
         ((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")?
            LvlPlacingItems::layer + ", ":"") +
                                   (s->cursor?
                                        (
                                   QString::number( s->cursor->scenePos().toPoint().x() ) + "x" +
                                   QString::number( s->cursor->scenePos().toPoint().y() )
                                        )
                                            :"")
                                   );
    else
        s->setMessageBoxItem(false);

    if(s->cursor)
    {
               s->cursor->setPos( QPointF(s->applyGrid( QPointF(mouseEvent->scenePos()-
                                                   QPointF(LvlPlacingItems::c_offset_x,
                                                          LvlPlacingItems::c_offset_y)).toPoint(),
                                                 LvlPlacingItems::gridSz,
                                                 LvlPlacingItems::gridOffset)));
               s->cursor->show();
    }
    if( mouseEvent->buttons() & Qt::LeftButton )
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

    if(s->placingItem == LvlScene::PLC_Door)
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        return;
    }
    else
    {
        if(!s->overwritedItems.blocks.isEmpty()||
            !s->overwritedItems.bgo.isEmpty()||
            !s->overwritedItems.npc.isEmpty() )
        {
            s->addOverwriteHistory(s->overwritedItems, s->placingItems);
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
            s->addPlaceHistory(s->placingItems);
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
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }
}
