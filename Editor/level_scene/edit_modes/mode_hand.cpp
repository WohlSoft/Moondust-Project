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

#include "mode_hand.h"
#include "../lvl_scene.h"
#include "../../common_features/mainwinconnect.h"

LVL_ModeHand::LVL_ModeHand(QGraphicsScene *parentScene, QObject *parent)
: EditMode("HandScroll", parentScene, parent)
{}

LVL_ModeHand::~LVL_ModeHand()
{}

void LVL_ModeHand::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->clearSelection();
    s->resetCursor();
    s->resetResizers();

    s->_viewPort->setCursor(Qt::ArrowCursor);
    s->_viewPort->setInteractive(false);
    s->_viewPort->setDragMode(QGraphicsView::ScrollHandDrag);
}

void LVL_ModeHand::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void LVL_ModeHand::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void LVL_ModeHand::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void LVL_ModeHand::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void LVL_ModeHand::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}
