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

#include "wld_base_item.h"

WldBaseItem::WldBaseItem()
{}

WldBaseItem::~WldBaseItem()
{}

void WldBaseItem::arrayApply()
{}

void WldBaseItem::returnBack()
{}

void WldBaseItem::removeFromArray()
{}

void WldBaseItem::setLayer(QString layer)
{
    Q_UNUSED(layer);
}

int WldBaseItem::getGridSize()
{
    return 1;
}

QPoint WldBaseItem::gridOffset()
{
    return QPoint(0,0);
}

QPoint WldBaseItem::sourcePos()
{
    return QPoint(0,0);
}


