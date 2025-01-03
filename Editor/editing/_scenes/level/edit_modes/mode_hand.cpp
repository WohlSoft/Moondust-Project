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

#include "mode_hand.h"
#include "../lvl_scene.h"


LVL_ModeHand::LVL_ModeHand(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("HandScroll", parentScene, parent)
{}

LVL_ModeHand::~LVL_ModeHand()
{}

void LVL_ModeHand::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);


    s->clearSelection();
    s->resetCursor();
    s->resetResizers();

    s->m_eraserIsEnabled = false;
    s->m_pastingMode = false;
    s->m_busyMode = true;
    s->m_disableMoveItems = false;

    s->m_viewPort->setCursor(Qt::ArrowCursor);
    s->m_viewPort->setInteractive(false);
    s->m_viewPort->setDragMode(QGraphicsView::ScrollHandDrag);
}

void LVL_ModeHand::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
}

void LVL_ModeHand::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
}

void LVL_ModeHand::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
}

void LVL_ModeHand::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent)
}

void LVL_ModeHand::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent)
}
