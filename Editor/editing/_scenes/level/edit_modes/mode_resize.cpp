/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <mainwindow.h>
#include <common_features/themes.h>
#include <common_features/item_rectangles.h>

#include "../lvl_scene.h"
#include "mode_resize.h"

LVL_ModeResize::LVL_ModeResize(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Resize", parentScene, parent)
{}

LVL_ModeResize::~LVL_ModeResize()
{}

void LVL_ModeResize::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->clearSelection();
    s->resetCursor();

    s->m_eraserIsEnabled = false;
    s->m_pastingMode = false;
    s->m_busyMode = true;
    s->m_disableMoveItems = true;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_resizing));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
}

void LVL_ModeResize::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->m_skipChildMousePressEvent = true;
    s->mousePressEvent(mouseEvent);
    dontCallEvent = true;
}

void LVL_ModeResize::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);
    s->clearSelection();

    s->m_skipChildMouseMoveEvent = true;
    s->mouseMoveEvent(mouseEvent);
    dontCallEvent = true;
}

void LVL_ModeResize::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
}

void LVL_ModeResize::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent)

}

void LVL_ModeResize::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    switch(keyEvent->key())
    {
    case(Qt::Key_Escape):
        s->resetResizers();
        break;
    case(Qt::Key_Enter):
    case(Qt::Key_Return):
        s->applyResizers();
        //setSectionResizer(false, true);
        break;
    default:
        break;
    }
}
