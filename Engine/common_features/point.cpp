/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

PGE_Point::PGE_Point(const PGE_PointF &p)
{
    m_x = static_cast<int>(p.m_x);
    m_y = static_cast<int>(p.m_y);
}

PGE_Point::PGE_Point(int x, int y)
{
    m_x = x;
    m_y = y;
}

PGE_Point::~PGE_Point()
{}

void PGE_Point::setPoint(int x, int y)
{
    m_x = x;
    m_y = y;
}

void PGE_Point::setX(int x)
{
    m_x = x;
}

void PGE_Point::setY(int y)
{
    m_y = y;
}

bool PGE_Point::isNull() const
{
    return ((m_x == 0) && (m_y == 0));
}

int PGE_Point::x() const
{
    return m_x;
}

int PGE_Point::y() const
{
    return m_y;
}

bool operator==(const PGE_Point &lhs, const PGE_Point &rhs)
{
    return ((lhs.m_x == rhs.m_x) && (lhs.m_y == rhs.m_y));
}

bool operator!=(const PGE_Point &lhs, const PGE_Point &rhs)
{
    return ((lhs.m_x != rhs.m_x) || (lhs.m_y != rhs.m_y));
}
