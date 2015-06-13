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

#include "pointf.h"
#include "point.h"

PGE_PointF::PGE_PointF()
{
    setPoint(0, 0);
}

PGE_PointF::PGE_PointF(const PGE_PointF &p)
{
    _x=p._x;
    _y=p._y;
}

PGE_PointF::PGE_PointF(const PGE_Point &p)
{
    _x=p._x;
    _y=p._y;
}

PGE_PointF::PGE_PointF(double x, double y)
{
    _x=x;
    _y=y;
}

PGE_PointF::~PGE_PointF()
{}

void PGE_PointF::setPoint(double x, double y)
{
    _x=x;
    _y=y;
}

void PGE_PointF::setX(double x)
{
    _x=x;
}

void PGE_PointF::setY(double y)
{
    _y=y;
}

bool PGE_PointF::isNull()
{
    return ((_x==0)&&(_y==0));
}

double PGE_PointF::x()
{
    return _x;
}

double PGE_PointF::y()
{
    return _y;
}

