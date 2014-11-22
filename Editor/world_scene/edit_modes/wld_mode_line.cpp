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

#include "wld_mode_line.h"

#include "../wld_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"
#include "../wld_item_placing.h"

#include "../item_tile.h"
#include "../item_scene.h"
#include "../item_path.h"
#include "../item_level.h"
#include "../item_music.h"

#include "../../common_features/themes.h"

WLD_ModeLine::WLD_ModeLine(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Line", parentScene, parent)
{
    drawStartPos = QPointF(0,0);
}

WLD_ModeLine::~WLD_ModeLine()
{}

void WLD_ModeLine::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetResizers();
    s->unserPointSelector();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_line_fill));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void WLD_ModeLine::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        item_rectangles::clearArray();
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->IsMoved = true;
        return;
    }

    WriteToLog(QtDebugMsg, QString("Line mode %1").arg(s->EditingMode));

    s->last_tile_arrayID=s->WldData->tile_array_id;
    s->last_scene_arrayID=s->WldData->scene_array_id;
    s->last_path_arrayID=s->WldData->path_array_id;
    s->last_level_arrayID=s->WldData->level_array_id;
    s->last_musicbox_arrayID=s->WldData->musicbox_array_id;

    if(s->cursor)
    {
        drawStartPos = QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                          QPoint(WldPlacingItems::c_offset_x,
                                                 WldPlacingItems::c_offset_y),
                                          WldPlacingItems::gridSz,
                                          WldPlacingItems::gridOffset));
        //cursor->setPos( drawStartPos );
        s->cursor->setVisible(true);

        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint()-
                               QPoint(WldPlacingItems::c_offset_x,
                                      WldPlacingItems::c_offset_y),
                               WldPlacingItems::gridSz,
                               WldPlacingItems::gridOffset);
        ((QGraphicsLineItem *)s->cursor)->setLine(drawStartPos.x(), drawStartPos.y(), hw.x(), hw.y());
    }
}

void WLD_ModeLine::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->cursor)
    {
        if(s->cursor->isVisible())
        {
        QPoint hs = s->applyGrid( mouseEvent->scenePos().toPoint()-
                               QPoint(WldPlacingItems::c_offset_x,
                                      WldPlacingItems::c_offset_y),
                               WldPlacingItems::gridSz,
                               WldPlacingItems::gridOffset);

        //((QGraphicsLineItem *)cursor)->setLine(drawStartPos.x(),drawStartPos.y(), hw.x(), hw.y());

        QLineF sz = item_rectangles::snapLine(QLineF(drawStartPos.x(),drawStartPos.y(), (qreal)hs.x(), (qreal)hs.y()),
                                             QSizeF((qreal)WldPlacingItems::itemW, (qreal)WldPlacingItems::itemH) );

        QPoint hw = s->applyGrid( sz.p2().toPoint(),
                            WldPlacingItems::gridSz,
                            WldPlacingItems::gridOffset);

        sz.setP2(QPointF((qreal)hw.x(),(qreal)hw.y()));

        ((QGraphicsLineItem *)s->cursor)->setLine(sz);

        item_rectangles::drawLine(s, sz,
               QSize(WldPlacingItems::itemW, WldPlacingItems::itemH)
                                    );

        }
    }
}

void WLD_ModeLine::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);

    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->cursor)
    {
        s->placeItemsByRectArray();
        s->Debugger_updateItemList();
        s->WldData->modified = true;
        s->cursor->hide();
    }
}

void WLD_ModeLine::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void WLD_ModeLine::keyRelease(QKeyEvent *keyEvent)
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
