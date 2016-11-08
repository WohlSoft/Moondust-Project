/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

PGE_SizeF::PGE_SizeF(const PGE_Size &p)
{
    m_w = static_cast<double>(p.m_w);
    m_h = static_cast<double>(p.m_h);
}

PGE_SizeF::PGE_SizeF(double w, double h)
{
    m_w = w;
    m_h = h;
}

PGE_SizeF::~PGE_SizeF()
{}

void PGE_SizeF::setSize(double w, double h)
{
    m_w = w;
    m_h = h;
}

void PGE_SizeF::setWidth(double w)
{
    m_w = w;
}

void PGE_SizeF::setHeight(double h)
{
    m_h = h;
}

bool PGE_SizeF::isNull()
{
    return ((m_w == 0.0) && (m_h == 0.0));
}

double PGE_SizeF::w()
{
    return m_w;
}

double PGE_SizeF::h()
{
    return m_h;
}
