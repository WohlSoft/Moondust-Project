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

#include "mode_line.h"

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

#include "../../common_features/themes.h"

LVL_ModeLine::LVL_ModeLine(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Line", parentScene, parent)
{
    drawStartPos = QPointF(0,0);
}

LVL_ModeLine::~LVL_ModeLine()
{}

void LVL_ModeLine::set()
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
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_line_fill));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModeLine::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
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

    if(LvlPlacingItems::npcSpecialAutoIncrement)
        s->IncrementingNpcSpecialSpin = LvlPlacingItems::npcSpecialAutoIncrement_begin;

    WriteToLog(QtDebugMsg, QString("Line mode %1").arg(s->EditingMode));

    if(s->cursor)
    {
        drawStartPos = QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                          QPoint(LvlPlacingItems::c_offset_x,
                                                 LvlPlacingItems::c_offset_y),
                                          LvlPlacingItems::gridSz,
                                          LvlPlacingItems::gridOffset));
        //cursor->setPos( drawStartPos );
        s->cursor->setVisible(true);

        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint()-
                               QPoint(LvlPlacingItems::c_offset_x,
                                      LvlPlacingItems::c_offset_y),
                               LvlPlacingItems::gridSz,
                               LvlPlacingItems::gridOffset);
        dynamic_cast<QGraphicsLineItem *>(s->cursor)->setLine(drawStartPos.x(), drawStartPos.y(),
                                          hw.x(), hw.y());
    }
}

void LVL_ModeLine::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")
        s->setMessageBoxItem(true, mouseEvent->scenePos(), LvlPlacingItems::layer + ", " +
                     QString::number( mouseEvent->scenePos().toPoint().x() ) + "x" +
                     QString::number( mouseEvent->scenePos().toPoint().y() )
                      );
    else
        s->setMessageBoxItem(false);

        if(s->cursor)
        {
            if(s->cursor->isVisible())
            {
                QPoint hs = s->applyGrid( mouseEvent->scenePos().toPoint()-
                                       QPoint(LvlPlacingItems::c_offset_x,
                                              LvlPlacingItems::c_offset_y),
                                       LvlPlacingItems::gridSz,
                                       LvlPlacingItems::gridOffset);

                QLineF sz = item_rectangles::snapLine(QLineF(drawStartPos.x(),drawStartPos.y(), (qreal)hs.x(), (qreal)hs.y()),
                                                     QSizeF((qreal)LvlPlacingItems::itemW, (qreal)LvlPlacingItems::itemH) );

                QPoint hw = s->applyGrid( sz.p2().toPoint(),
                                    LvlPlacingItems::gridSz,
                                    LvlPlacingItems::gridOffset);

                sz.setP2(QPointF((qreal)hw.x(),(qreal)hw.y()));

                dynamic_cast<QGraphicsLineItem *>(s->cursor)->setLine(sz);

                item_rectangles::drawLine(s, sz,
                       QSize(LvlPlacingItems::itemW, LvlPlacingItems::itemH)
                                            );

                if(LvlPlacingItems::npcSpecialAutoIncrement)
                {   //Automatically set direction by line angle in auto-incrementing mode
                    if(sz.angle()>=0 && sz.angle() < 180)
                        LvlPlacingItems::npcSet.direct=1;   //top
                    else
                        LvlPlacingItems::npcSet.direct=-1;  //bottom
                }

            }
        }

}

void LVL_ModeLine::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);

    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->cursor)
    {
        WriteToLog(QtDebugMsg, "Line tool -> Placing items");
        s->placeItemsByRectArray();

        s->Debugger_updateItemList();

        s->LvlData->modified = true;
        s->cursor->hide();
    }
}

void LVL_ModeLine::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void LVL_ModeLine::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
            item_rectangles::clearArray();
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }
}
