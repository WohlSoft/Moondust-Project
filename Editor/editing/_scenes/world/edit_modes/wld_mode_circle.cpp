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

#include <common_features/main_window_ptr.h>
#include <common_features/item_rectangles.h>
#include <common_features/themes.h>

#include "wld_mode_circle.h"
#include "../wld_scene.h"
#include "../wld_item_placing.h"
#include "../items/item_tile.h"
#include "../items/item_scene.h"
#include "../items/item_path.h"
#include "../items/item_level.h"
#include "../items/item_music.h"

WLD_ModeCircle::WLD_ModeCircle(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Circle", parentScene, parent)
{
    drawStartPos = QPointF(0,0);
}

WLD_ModeCircle::~WLD_ModeCircle()
{}

void WLD_ModeCircle::set()
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

void WLD_ModeCircle::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
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

    if(s->cursor)
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->cursor);
        drawStartPos = QPointF(s->applyGrid( mouseEvent->scenePos().toPoint(),
                                          WldPlacingItems::gridSz,
                                          WldPlacingItems::gridOffset));
        s->cursor->setPos( drawStartPos );
        s->cursor->setVisible(true);

        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint(),
                               WldPlacingItems::gridSz,
                               WldPlacingItems::gridOffset);

        QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );
        cur->setRect(0,0, hs.width(), hs.height());
    }
}

void WLD_ModeCircle::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->cursor && s->cursor->isVisible())
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->cursor);
        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint(),
                               WldPlacingItems::gridSz,
                               WldPlacingItems::gridOffset);

        QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );


        cur->setRect(0,0, hs.width(), hs.height());
        cur->setPos(
                    ((hw.x() < drawStartPos.x() )? hw.x() : drawStartPos.x()),
                    ((hw.y() < drawStartPos.y() )? hw.y() : drawStartPos.y())
                    );
    }
}

void WLD_ModeCircle::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);

    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->cursor)
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->cursor);
        // /////////// Don't draw with zero width or height //////////////
        if( (cur->rect().width()==0) || (cur->rect().height()==0) )
        {
            s->cursor->hide();
            return;
        }

        item_rectangles::drawRound(s,
                                   QRect(cur->x(), cur->y(), cur->rect().width(), cur->rect().height()),
                                   QSize(WldPlacingItems::itemW, WldPlacingItems::itemH)
                                   );

        s->placeItemsByRectArray();
        s->Debugger_updateItemList();

        s->emptyCollisionCheck = false;
        s->collisionCheckBuffer.clear();

        s->WldData->modified = true;
        s->cursor->hide();
    }
}

void WLD_ModeCircle::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void WLD_ModeCircle::keyRelease(QKeyEvent *keyEvent)
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
