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

#include "wld_mode_resize.h"

#include "../wld_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"

#include "../../common_features/themes.h"

WLD_ModeResize::WLD_ModeResize(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Resize", parentScene, parent)
{}

WLD_ModeResize::~WLD_ModeResize()
{}

void WLD_ModeResize::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=true;

    s->clearSelection();
    s->resetCursor();
    s->unserPointSelector();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_resizing));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
}

void WLD_ModeResize::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->MousePressEventOnly = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;
}

void WLD_ModeResize::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);
    s->clearSelection();

    s->MouseMoveEventOnly = true;
    s->mouseMoveEvent(mouseEvent);
    dontCallEvent = true;
}

void WLD_ModeResize::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void WLD_ModeResize::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);

}

void WLD_ModeResize::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
            s->resetResizers();
            break;
        case (Qt::Key_Enter):
        case (Qt::Key_Return):
            s->applyResizers();
                //setSectionResizer(false, true);
            break;
        default:
            break;
    }
}
