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
    s->m_pointSelector.unserPointSelector();

    s->m_eraserIsEnabled=false;
    s->m_pastingMode=false;
    s->m_busyMode=true;
    s->m_disableMoveItems=false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_square_fill));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->m_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->m_viewPort->viewport()->setMouseTracking(true);
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
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    s->m_lastTerrainArrayID=s->m_data->tile_array_id;
    s->m_lastSceneryArrayID=s->m_data->scene_array_id;
    s->m_lastPathArrayID=s->m_data->path_array_id;
    s->m_lastLevelArrayID=s->m_data->level_array_id;
    s->m_lastMusicBoxArrayID=s->m_data->musicbox_array_id;

    if(s->m_cursorItemImg)
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->m_cursorItemImg);
        drawStartPos = QPointF(s->applyGrid( mouseEvent->scenePos().toPoint(),
                                          WldPlacingItems::gridSz,
                                          WldPlacingItems::gridOffset));
        s->m_cursorItemImg->setPos( drawStartPos );
        s->m_cursorItemImg->setVisible(true);

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

    if(s->m_cursorItemImg && s->m_cursorItemImg->isVisible())
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->m_cursorItemImg);
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

    if(s->m_cursorItemImg)
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->m_cursorItemImg);
        // /////////// Don't draw with zero width or height //////////////
        if( (cur->rect().width()==0) || (cur->rect().height()==0) )
        {
            s->m_cursorItemImg->hide();
            return;
        }

        item_rectangles::drawRound(s,
                                   QRect(cur->x(), cur->y(), cur->rect().width(), cur->rect().height()),
                                   QSize(WldPlacingItems::itemW, WldPlacingItems::itemH)
                                   );

        s->placeItemsByRectArray();
        s->Debugger_updateItemList();

        s->m_emptyCollisionCheck = false;
        s->collisionCheckBuffer.clear();

        s->m_data->meta.modified = true;
        s->m_cursorItemImg->hide();
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
