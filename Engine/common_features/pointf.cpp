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

#include "pointf.h"
#include "point.h"
#include <Utils/maths.h>

PGE_PointF::PGE_PointF(const PGE_Point &p)
{
    m_x = p.m_x;
    m_y = p.m_y;
}

bool operator==(const PGE_PointF &lhs, const PGE_PointF &rhs)
{
    return (Maths::equals(lhs.m_x, rhs.m_x) && Maths::equals(lhs.m_y, rhs.m_y));
}

bool operator!=(const PGE_PointF &lhs, const PGE_PointF &rhs)
{
    return (!Maths::equals(lhs.m_x, rhs.m_x) || Maths::equals(lhs.m_y, rhs.m_y));
}
