/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "edit_mode_base.h"

EditMode::EditMode(QString toolTitle, QGraphicsScene *ParentScene, QObject *parent) :
    QObject(parent), toolName(toolTitle), scene(ParentScene), dontCallEvent(false)
{}

EditMode::~EditMode()
{}

void EditMode::set()
{}

QString EditMode::name() const
{
    return toolName;
}

void EditMode::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{Q_UNUSED(mouseEvent);}

void EditMode::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{Q_UNUSED(mouseEvent);}

void EditMode::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{Q_UNUSED(mouseEvent);}

void EditMode::keyPress(QKeyEvent *keyEvent)
{Q_UNUSED(keyEvent);}

void EditMode::keyRelease(QKeyEvent *keyEvent)
{Q_UNUSED(keyEvent);}

bool EditMode::noEvent()
{
    bool state = dontCallEvent;
    dontCallEvent = false;
    return state;
}
