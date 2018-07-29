/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "wld_mode_line.h"
#include "../wld_scene.h"
#include "../wld_item_placing.h"
#include "../items/item_tile.h"
#include "../items/item_scene.h"
#include "../items/item_path.h"
#include "../items/item_level.h"
#include "../items/item_music.h"

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

    s->clearSelection();
    s->resetResizers();
    s->m_pointSelector.unserPointSelector();

    s->m_eraserIsEnabled=false;
    s->m_pastingMode=false;
    s->m_busyMode=true;
    s->m_disableMoveItems=false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_line_fill));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->m_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->m_viewPort->viewport()->setMouseTracking(true);
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
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    LogDebug(QString("Line mode %1").arg(s->m_editMode));

    s->m_lastTerrainArrayID=s->m_data->tile_array_id;
    s->m_lastSceneryArrayID=s->m_data->scene_array_id;
    s->m_lastPathArrayID=s->m_data->path_array_id;
    s->m_lastLevelArrayID=s->m_data->level_array_id;
    s->m_lastMusicBoxArrayID=s->m_data->musicbox_array_id;

    if(s->m_cursorItemImg)
    {
        drawStartPos = QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                          QPoint(WldPlacingItems::c_offset_x,
                                                 WldPlacingItems::c_offset_y),
                                          WldPlacingItems::gridSz,
                                          WldPlacingItems::gridOffset));
        //cursor->setPos( drawStartPos );
        s->m_cursorItemImg->setVisible(true);

        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint()-
                               QPoint(WldPlacingItems::c_offset_x,
                                      WldPlacingItems::c_offset_y),
                               WldPlacingItems::gridSz,
                               WldPlacingItems::gridOffset);
        ((QGraphicsLineItem *)s->m_cursorItemImg)->setLine(drawStartPos.x(), drawStartPos.y(), hw.x(), hw.y());
    }
}

void WLD_ModeLine::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->m_cursorItemImg)
    {
        if(s->m_cursorItemImg->isVisible())
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

        ((QGraphicsLineItem *)s->m_cursorItemImg)->setLine(sz);

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

    if(s->m_cursorItemImg)
    {
        s->placeItemsByRectArray();
        s->Debugger_updateItemList();
        s->m_data->meta.modified = true;
        s->m_cursorItemImg->hide();
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
