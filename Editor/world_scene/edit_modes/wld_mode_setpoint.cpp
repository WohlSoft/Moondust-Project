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

#include "wld_mode_setpoint.h"

#include "../wld_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../wld_item_placing.h"

#include "../../common_features/themes.h"

WLD_ModeSetPoint::WLD_ModeSetPoint(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("SetPoint", parentScene, parent)
{}

WLD_ModeSetPoint::~WLD_ModeSetPoint()
{

}

void WLD_ModeSetPoint::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=false;

    s->resetResizers();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_placing));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void WLD_ModeSetPoint::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if( (!s->isSelectionDialog) && (mouseEvent->buttons() & Qt::RightButton) )
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->IsMoved = true;
        return;
    }

    if(s->cursor){
        s->cursor->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                           QPoint(WldPlacingItems::c_offset_x,
                                                  WldPlacingItems::c_offset_y),
                                           WldPlacingItems::gridSz,
                                           WldPlacingItems::gridOffset)));
    }

    s->setPoint(s->cursor->scenePos().toPoint());
    emit s->pointSelected(s->selectedPoint);

    dontCallEvent=true;
}

void WLD_ModeSetPoint::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->cursor)
    {

               s->cursor->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                                   QPoint(WldPlacingItems::c_offset_x,
                                                          WldPlacingItems::c_offset_y),
                                                 WldPlacingItems::gridSz,
                                                 WldPlacingItems::gridOffset)));
               s->cursor->show();

               s->setMessageBoxItem(true, mouseEvent->scenePos(),
                                      (s->cursor?
                                           (
                                      QString::number( s->cursor->scenePos().toPoint().x() ) + "x" +
                                      QString::number( s->cursor->scenePos().toPoint().y() )
                                           )
                                               :""));

    }
}

void WLD_ModeSetPoint::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(!s->isSelectionDialog)
    {
        MainWinConnect::pMainWin->WLD_returnPointToLevelProperties(s->selectedPoint);
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        s->openProps();

        s->MousePressEventOnly = true;
        s->mousePressEvent(mouseEvent);
        dontCallEvent=true;
    }

}

void WLD_ModeSetPoint::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void WLD_ModeSetPoint::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
            if(s->isSelectionDialog) break; //Disable this key in the point selection dialog
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }

}

