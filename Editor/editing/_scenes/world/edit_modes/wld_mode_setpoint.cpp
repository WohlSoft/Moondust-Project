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
#include <common_features/themes.h>
#include <main_window/dock/wld_item_props.h>

#include "../wld_scene.h"
#include "../wld_item_placing.h"
#include "wld_mode_setpoint.h"

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

    s->resetResizers();

    s->m_eraserIsEnabled=false;
    s->m_pastingMode=false;
    s->m_busyMode=true;
    s->m_disableMoveItems=false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_placing));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->m_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->m_viewPort->viewport()->setMouseTracking(true);
}

void WLD_ModeSetPoint::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);
    Q_ASSERT(s);

    if( (!s->m_isSelectionDialog) && ((mouseEvent->buttons() & Qt::RightButton)!=0) )
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    if(s->m_cursorItemImg)
    {
        s->m_cursorItemImg->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                           QPoint(WldPlacingItems::c_offset_x,
                                                  WldPlacingItems::c_offset_y),
                                           WldPlacingItems::gridSz,
                                           WldPlacingItems::gridOffset)));
        s->m_pointSelector.setPoint(s->m_cursorItemImg->scenePos().toPoint());
        emit s->m_pointSelector.pointSelected(s->m_pointSelector.m_pointCoord);
    }
    dontCallEvent=true;
}

void WLD_ModeSetPoint::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->m_cursorItemImg)
    {

               s->m_cursorItemImg->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                                   QPoint(WldPlacingItems::c_offset_x,
                                                          WldPlacingItems::c_offset_y),
                                                 WldPlacingItems::gridSz,
                                                 WldPlacingItems::gridOffset)));
               s->m_cursorItemImg->show();

               s->setMessageBoxItem(true, mouseEvent->scenePos(),
                                      (s->m_cursorItemImg?
                                           (
                                      QString::number( s->m_cursorItemImg->scenePos().toPoint().x() ) + "x" +
                                      QString::number( s->m_cursorItemImg->scenePos().toPoint().y() )
                                           )
                                               :""));

    }
}

void WLD_ModeSetPoint::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(!s->m_isSelectionDialog)
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        MainWinConnect::pMainWin->dock_WldItemProps->WLD_returnPointToLevelProperties(s->m_pointSelector.m_pointCoord);
        s->openProps();
        //s->MouseReleaseEventOnly = true;
        //s->mouseReleaseEvent(mouseEvent);
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
            if(s->m_isSelectionDialog) break; //Disable this key in the point selection dialog
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }

}

