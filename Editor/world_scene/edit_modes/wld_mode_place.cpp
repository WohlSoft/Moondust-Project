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

#include "wld_mode_place.h"

#include "../wld_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"
#include "../wld_item_placing.h"

#include "../../common_features/themes.h"

WLD_ModePlace::WLD_ModePlace(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Placing", parentScene, parent)
{}

WLD_ModePlace::~WLD_ModePlace()
{}

void WLD_ModePlace::set()
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
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_placing));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void WLD_ModePlace::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        item_rectangles::clearArray();
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        return;
    }

    s->last_tile_arrayID=s->WldData->tile_array_id;
    s->last_scene_arrayID=s->WldData->scene_array_id;
    s->last_path_arrayID=s->WldData->path_array_id;
    s->last_level_arrayID=s->WldData->level_array_id;
    s->last_musicbox_arrayID=s->WldData->musicbox_array_id;

    if(s->cursor){
        s->cursor->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                           QPoint(WldPlacingItems::c_offset_x,
                                                  WldPlacingItems::c_offset_y),
                                           WldPlacingItems::gridSz,
                                           WldPlacingItems::gridOffset)));
    }

    s->placeItemUnderCursor();
    s->Debugger_updateItemList();

    s->MousePressEventOnly = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;
}

void WLD_ModePlace::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->clearSelection();

    if(mouseEvent->modifiers() & Qt::ControlModifier )
        s->setMessageBoxItem(true, mouseEvent->scenePos(),
                               (s->cursor?
                                    (
                               QString::number( s->cursor->scenePos().toPoint().x() ) + "x" +
                               QString::number( s->cursor->scenePos().toPoint().y() )
                                    )
                                        :""));
    else
        s->setMessageBoxItem(false);

    if(s->cursor)
    {
               s->cursor->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                                   QPoint(WldPlacingItems::c_offset_x,
                                                          WldPlacingItems::c_offset_y),
                                                 WldPlacingItems::gridSz,
                                                 WldPlacingItems::gridOffset)));
               s->cursor->show();
    }
    if( mouseEvent->buttons() & Qt::LeftButton )
    {
        s->placeItemUnderCursor();
        s->Debugger_updateItemList();
    }
}

void WLD_ModePlace::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(!s->overwritedItems.tiles.isEmpty()||
        !s->overwritedItems.scenery.isEmpty()||
        !s->overwritedItems.paths.isEmpty()||
        !s->overwritedItems.levels.isEmpty()||
        !s->overwritedItems.music.isEmpty() )
    {
        s->addOverwriteHistory(s->overwritedItems, s->placingItems);
        s->overwritedItems.tiles.clear();
        s->overwritedItems.scenery.clear();
        s->overwritedItems.paths.clear();
        s->overwritedItems.levels.clear();
        s->overwritedItems.music.clear();
        s->placingItems.tiles.clear();
        s->placingItems.paths.clear();
        s->placingItems.scenery.clear();
        s->placingItems.levels.clear();
        s->placingItems.music.clear();
    }
    else
    if(!s->placingItems.tiles.isEmpty()||
            !s->placingItems.paths.isEmpty()||
            !s->placingItems.scenery.isEmpty()||
            !s->placingItems.levels.isEmpty()||
            !s->placingItems.music.isEmpty()){
        s->addPlaceHistory(s->placingItems);
        s->placingItems.tiles.clear();
        s->placingItems.paths.clear();
        s->placingItems.scenery.clear();
        s->placingItems.levels.clear();
        s->placingItems.music.clear();
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
        case (Qt::Key_Escape):
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }
}
