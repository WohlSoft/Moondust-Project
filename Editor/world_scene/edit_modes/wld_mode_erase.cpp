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

#include "wld_mode_erase.h"

#include "../wld_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"

#include "../item_tile.h"
#include "../item_scene.h"
#include "../item_path.h"
#include "../item_level.h"
#include "../item_music.h"

#include "../../common_features/themes.h"

WLD_ModeErase::WLD_ModeErase(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Erase", parentScene, parent)
{}


WLD_ModeErase::~WLD_ModeErase()
{}


void WLD_ModeErase::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetCursor();
    s->resetResizers();
    s->unserPointSelector();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_erasing));
    s->_viewPort->setDragMode(QGraphicsView::RubberBandDrag);
}

void WLD_ModeErase::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        return;
    }

    if(s->cursor){
       s->cursor->show();
       s->cursor->setPos(mouseEvent->scenePos());
    }

    s->MousePressEventOnly = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;

    QList<QGraphicsItem*> selectedList = s->selectedItems();
    if (!selectedList.isEmpty())
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
        s->EraserEnabled=true;
    }
}

void WLD_ModeErase::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->cursor) s->cursor->setPos(mouseEvent->scenePos());
    if (s->EraserEnabled)// Remove All items, placed under Cursor
    {
        s->removeItemUnderCursor();
        s->Debugger_updateItemList();
    }
}

void WLD_ModeErase::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
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
        s->addRemoveHistory(s->overwritedItems);
        s->overwritedItems.tiles.clear();
        s->overwritedItems.scenery.clear();
        s->overwritedItems.paths.clear();
        s->overwritedItems.levels.clear();
        s->overwritedItems.music.clear();
    }

    QList<QGraphicsItem*> selectedList = s->selectedItems();

    // check for grid snap
    if ((!selectedList.isEmpty())&&(s->mouseMoved))
    {
        s->removeWldItems(selectedList);
        selectedList = s->selectedItems();
        s->Debugger_updateItemList();
    }

    s->EraserEnabled = false;
}

void WLD_ModeErase::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}


void WLD_ModeErase::keyRelease(QKeyEvent *keyEvent)
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
