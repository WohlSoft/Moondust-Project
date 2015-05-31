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

PGE_Size::PGE_Size()
{
    setSize(0, 0);
}

PGE_Size::PGE_Size(const PGE_Size &p)
{
    _w=p._w;
    _h=p._h;
}

PGE_Size::PGE_Size(const PGE_SizeF &p)
{
    _w=p._w;
    _h=p._h;
}

PGE_Size::PGE_Size(int w, int h)
{
    _w=w;
    _h=h;
}

PGE_Size::~PGE_Size()
{}

void PGE_Size::setSize(int w, int h)
{
    _w=w;
    _h=h;
}

void PGE_Size::setWidth(int w)
{
    _w=w;
}

void PGE_Size::setHeight(int h)
{
    _h=h;
}

bool PGE_Size::isNull()
{
    return ((_w==0)&&(_h==0));
}

int PGE_Size::w()
{
    return _w;
}

int PGE_Size::h()
{
    return _h;
}

