/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "point.h"
#include "pointf.h"

PGE_Point::PGE_Point()
{
    setPoint(0, 0);
}

PGE_Point::PGE_Point(const PGE_Point &p)
{
    _x=p._x;
    _y=p._y;
}

PGE_Point::PGE_Point(const PGE_PointF &p)
{
    _x=p._x;
    _y=p._y;
}

PGE_Point::PGE_Point(int x, int y)
{
    _x=x;
    _y=y;
}

PGE_Point::~PGE_Point()
{}

void PGE_Point::setPoint(int x, int y)
{
    _x=x;
    _y=y;
}

void PGE_Point::setX(int x)
{
    _x=x;
}

void PGE_Point::setY(int y)
{
    _y=y;
}

bool PGE_Point::isNull()
{
    return ((_x==0)&&(_y==0));
}

int PGE_Point::x()
{
    return _x;
}

int PGE_Point::y()
{
    return _y;
}



