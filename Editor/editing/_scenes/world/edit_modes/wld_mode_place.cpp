/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/themes.h>
#include <common_features/main_window_ptr.h>
#include <common_features/item_rectangles.h>

#include "wld_mode_place.h"
#include "../wld_scene.h"
#include "../wld_item_placing.h"
#include "../wld_history_manager.h"

WLD_ModePlace::WLD_ModePlace(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Placing", parentScene, parent)
{}

WLD_ModePlace::~WLD_ModePlace()
{}

void WLD_ModePlace::set()
{
    if(!scene) return;
    WldScene *s = qobject_cast<WldScene *>(scene);

    s->clearSelection();
    s->resetResizers();
    s->m_pointSelector.unserPointSelector();

    s->m_eraserIsEnabled = false;
    s->m_pastingMode = false;
    s->m_busyMode = true;
    s->m_disableMoveItems = false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_placing));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->m_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->m_viewPort->viewport()->setMouseTracking(true);
}

void WLD_ModePlace::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = qobject_cast<WldScene *>(scene);

    if(mouseEvent->buttons() & Qt::RightButton)
    {
        item_rectangles::clearArray();
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    s->m_lastTerrainArrayID = s->m_data->tile_array_id;
    s->m_lastSceneryArrayID = s->m_data->scene_array_id;
    s->m_lastPathArrayID = s->m_data->path_array_id;
    s->m_lastLevelArrayID = s->m_data->level_array_id;
    s->m_lastMusicBoxArrayID = s->m_data->musicbox_array_id;

    if(s->m_cursorItemImg)
    {
        s->m_cursorItemImg->setPos(QPointF(s->applyGrid(mouseEvent->scenePos().toPoint() -
                                           QPoint(WldPlacingItems::c_offset_x,
                                                   WldPlacingItems::c_offset_y),
                                           WldPlacingItems::gridSz,
                                           WldPlacingItems::gridOffset)));
    }

    s->placeItemUnderCursor();
    s->Debugger_updateItemList();

    s->m_skipChildMousePressEvent = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;
}

void WLD_ModePlace::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = qobject_cast<WldScene *>(scene);

    s->clearSelection();

    if(mouseEvent->modifiers() & Qt::ControlModifier)
        s->setMessageBoxItem(true, mouseEvent->scenePos(),
                             (s->m_cursorItemImg ?
                              (
                                  QString::number(s->m_cursorItemImg->scenePos().toPoint().x()) + "x" +
                                  QString::number(s->m_cursorItemImg->scenePos().toPoint().y())
                              )
                              : ""));
    else
        s->setMessageBoxItem(false);

    if(s->m_cursorItemImg)
    {
        s->m_cursorItemImg->setPos(QPointF(s->applyGrid(mouseEvent->scenePos().toPoint() -
                                           QPoint(WldPlacingItems::c_offset_x,
                                                   WldPlacingItems::c_offset_y),
                                           WldPlacingItems::gridSz,
                                           WldPlacingItems::gridOffset)));
        s->m_cursorItemImg->show();
    }
    if(mouseEvent->buttons() & Qt::LeftButton)
    {
        s->placeItemUnderCursor();
        s->Debugger_updateItemList();
    }
}

void WLD_ModePlace::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    WldScene *s = qobject_cast<WldScene *>(scene);

    if(!s->m_overwritedItems.tiles.isEmpty() ||
       !s->m_overwritedItems.scenery.isEmpty() ||
       !s->m_overwritedItems.paths.isEmpty() ||
       !s->m_overwritedItems.levels.isEmpty() ||
       !s->m_overwritedItems.music.isEmpty())
    {
        s->m_history->addOverwriteHistory(s->m_overwritedItems, s->m_placingItems);
        s->m_overwritedItems.tiles.clear();
        s->m_overwritedItems.scenery.clear();
        s->m_overwritedItems.paths.clear();
        s->m_overwritedItems.levels.clear();
        s->m_overwritedItems.music.clear();
        s->m_placingItems.tiles.clear();
        s->m_placingItems.paths.clear();
        s->m_placingItems.scenery.clear();
        s->m_placingItems.levels.clear();
        s->m_placingItems.music.clear();
    }
    else if(!s->m_placingItems.tiles.isEmpty() ||
            !s->m_placingItems.paths.isEmpty() ||
            !s->m_placingItems.scenery.isEmpty() ||
            !s->m_placingItems.levels.isEmpty() ||
            !s->m_placingItems.music.isEmpty())
    {
        s->m_history->addPlaceHistory(s->m_placingItems);
        s->m_placingItems.tiles.clear();
        s->m_placingItems.paths.clear();
        s->m_placingItems.scenery.clear();
        s->m_placingItems.levels.clear();
        s->m_placingItems.music.clear();
    }
}

void WLD_ModePlace::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void WLD_ModePlace::keyRelease(QKeyEvent *keyEvent)
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
