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

#include "sizef.h"
#include "size.h"

PGE_SizeF::PGE_SizeF()
{
    setSize(0, 0);
}

PGE_SizeF::PGE_SizeF(const PGE_SizeF &p)
{
    _w=p._w;
    _h=p._h;
}

PGE_SizeF::PGE_SizeF(const PGE_Size &p)
{
    _w=p._w;
    _h=p._h;
}

PGE_SizeF::PGE_SizeF(double w, double h)
{
    _w=w;
    _h=h;
}

PGE_SizeF::~PGE_SizeF()
{}

void PGE_SizeF::setSize(double w, double h)
{
    _w=w;
    _h=h;
}

void PGE_SizeF::setWidth(double w)
{
    _w=w;
}

void PGE_SizeF::setHeight(double h)
{
    _h=h;
}

bool PGE_SizeF::isNull()
{
    return ((_w==0)&&(_h==0));
}

double PGE_SizeF::w()
{
    return _w;
}

double PGE_SizeF::h()
{
    return _h;
}

