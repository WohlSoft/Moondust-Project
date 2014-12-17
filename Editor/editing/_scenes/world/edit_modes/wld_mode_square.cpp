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

#include <common_features/mainwinconnect.h>
#include <common_features/item_rectangles.h>
#include <common_features/themes.h>

#include "wld_mode_square.h"
#include "../wld_scene.h"
#include "../wld_item_placing.h"
#include "../items/item_tile.h"
#include "../items/item_scene.h"
#include "../items/item_path.h"
#include "../items/item_level.h"
#include "../items/item_music.h"

WLD_ModeSquare::WLD_ModeSquare(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Square", parentScene, parent)
{
    drawStartPos = QPointF(0,0);
}

WLD_ModeSquare::~WLD_ModeSquare()
{}

void WLD_ModeSquare::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->clearSelection();
    s->resetResizers();
    s->unserPointSelector();

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=false;

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_square_fill));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void WLD_ModeSquare::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
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

    s->last_tile_arrayID=s->WldData->tile_array_id;
    s->last_scene_arrayID=s->WldData->scene_array_id;
    s->last_path_arrayID=s->WldData->path_array_id;
    s->last_level_arrayID=s->WldData->level_array_id;
    s->last_musicbox_arrayID=s->WldData->musicbox_array_id;

    if(s->cursor){
        drawStartPos = QPointF(s->applyGrid( mouseEvent->scenePos().toPoint(),
                                          WldPlacingItems::gridSz,
                                          WldPlacingItems::gridOffset));
        s->cursor->setPos( drawStartPos );
        s->cursor->setVisible(true);

        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint(),
                               WldPlacingItems::gridSz,
                               WldPlacingItems::gridOffset);

        QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );
        ((QGraphicsRectItem *)(s->cursor))->setRect(0,0, hs.width(), hs.height());
    }
}

void WLD_ModeSquare::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->cursor)
    {
        if(s->cursor->isVisible())
        {
        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint(),
                               WldPlacingItems::gridSz,
                               WldPlacingItems::gridOffset);

        QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );


        ((QGraphicsRectItem *)s->cursor)->setRect(0,0, hs.width(), hs.height());
        ((QGraphicsRectItem *)s->cursor)->setPos(
                    ((hw.x() < drawStartPos.x() )? hw.x() : drawStartPos.x()),
                    ((hw.y() < drawStartPos.y() )? hw.y() : drawStartPos.y())
                    );

        item_rectangles::drawMatrix(s, QRect (((QGraphicsRectItem *)s->cursor)->x(),
                                                ((QGraphicsRectItem *)s->cursor)->y(),
                                                ((QGraphicsRectItem *)s->cursor)->rect().width(),
                                                ((QGraphicsRectItem *)s->cursor)->rect().height()),
                                    QSize(WldPlacingItems::itemW, WldPlacingItems::itemH)
                                    );

        }
    }
}

void WLD_ModeSquare::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);

    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->cursor)
    {

        // /////////// Don't draw with zero width or height //////////////
        if( (((QGraphicsRectItem *)s->cursor)->rect().width()==0) ||
          (((QGraphicsRectItem *)s->cursor)->rect().height()==0))
        {
            s->cursor->hide();
            return;
        }
        QPointF p = ((QGraphicsRectItem *)s->cursor)->scenePos();
        QSizeF sz = ((QGraphicsRectItem *)s->cursor)->rect().size();

        s->collisionCheckBuffer = s->items(QRectF(
                    p.x()-10, p.y()-10,
                    sz.width()+20, sz.height()+20),
                    Qt::IntersectsItemBoundingRect);

        if(s->collisionCheckBuffer.isEmpty())
            s->emptyCollisionCheck = true;
        else
            s->prepareCollisionBuffer();

        s->placeItemsByRectArray();

        s->Debugger_updateItemList();

        s->emptyCollisionCheck = false;
        s->collisionCheckBuffer.clear();

        s->WldData->modified = true;
        s->cursor->hide();
    }
}

void WLD_ModeSquare::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void WLD_ModeSquare::keyRelease(QKeyEvent *keyEvent)
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
