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

#include "sizef.h"
#include "size.h"

PGE_Size::PGE_Size()
{
    setSize(0, 0);
}

PGE_Size::PGE_Size(const PGE_SizeF &p)
{
    m_w = static_cast<int>(p.m_w);
    m_h = static_cast<int>(p.m_h);
}

PGE_Size::PGE_Size(int w, int h)
{
    m_w = w;
    m_h = h;
}

PGE_Size::~PGE_Size()
{}

void PGE_Size::setSize(int w, int h)
{
    m_w = w;
    m_h = h;
}

void PGE_Size::setWidth(int w)
{
    m_w = w;
}

void PGE_Size::setHeight(int h)
{
    m_h = h;
}

bool PGE_Size::isNull() const
{
    return ((m_w == 0) && (m_h == 0));
}

int PGE_Size::w() const
{
    return m_w;
}

int PGE_Size::h() const
{
    return m_h;
}
