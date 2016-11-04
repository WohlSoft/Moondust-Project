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

#include "rectf.h"
#include "rect.h"
#include "pointf.h"
#include "sizef.h"

PGE_RectF::PGE_RectF()
{
    setRect(0.0, 0.0, 0.0, 0.0);
}

PGE_RectF::PGE_RectF(double x, double y, double w, double h)
{
    setRect(x, y,  w, h);
}

PGE_RectF::PGE_RectF(const PGE_Rect &r)
{
    m_x = static_cast<double>(r.m_x);
    m_y = static_cast<double>(r.m_y);
    m_w = static_cast<double>(r.m_w);
    m_h = static_cast<double>(r.m_h);
    m_r = static_cast<double>(r.m_r);
    m_b = static_cast<double>(r.m_b);
}

PGE_RectF::~PGE_RectF()
{}

void PGE_RectF::setRect(double x, double y, double w, double h)
{
    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
    m_r = m_x + m_w;
    m_b = m_y + m_h;
}

void PGE_RectF::setPos(double x, double y)
{
    m_x = x;
    m_y = y;
    m_r = m_x + m_w;
    m_b = m_y + m_h;
}

void PGE_RectF::setSize(double w, double h)
{
    m_w = w;
    m_h = h;
    m_r = m_x + m_w;
    m_b = m_y + m_h;
}

void PGE_RectF::setLeft(double l)
{
    m_x = l;
    m_w = m_r - m_x;
}

void PGE_RectF::setRight(double r)
{
    m_r = r;
    m_w = m_r - m_x;
}

void PGE_RectF::setTop(double t)
{
    m_y = t;
    m_h = m_b - m_y;
}

void PGE_RectF::setBottom(double b)
{
    m_b = b;
    m_h = m_b - m_y;
}

PGE_RectF PGE_RectF::withMargin(double margin)
{
    PGE_RectF rect;
    rect.m_x = m_x - margin;
    rect.m_y = m_y - margin;
    rect.m_b = m_b + margin;
    rect.m_r = m_r + margin;
    rect.m_w = m_w + margin * 2.0;
    rect.m_h = m_h + margin * 2.0;
    return rect;
}

PGE_RectF PGE_RectF::withMargin(double marginX, double marginY)
{
    PGE_RectF rect;
    rect.m_x = m_x - marginX;
    rect.m_y = m_y - marginY;
    rect.m_b = m_b + marginX;
    rect.m_r = m_r + marginY;
    rect.m_w = m_w + marginX * 2.0;
    rect.m_h = m_h + marginY * 2.0;
    return rect;
}


void PGE_RectF::setX(double x)
{
    m_x = x;
    m_r = m_x + m_w;
}

void PGE_RectF::setY(double y)
{
    m_y = y;
    m_b = m_y + m_h;
}

void PGE_RectF::setWidth(double w)
{
    m_w = w;
    m_r = m_x + m_w;
}

void PGE_RectF::setHeight(double h)
{
    m_h = h;
    m_b = m_y + m_h;
}

void PGE_RectF::addX(double x)
{
    m_x += x;
    m_r = m_x + m_w;
}

void PGE_RectF::addY(double y)
{
    m_y += y;
    m_b = m_y + m_h;
}

void PGE_RectF::setTopLeft(PGE_PointF p)
{
    setLeft(p.x());
    setTop(p.y());
}

void PGE_RectF::setTopRight(PGE_PointF p)
{
    setTop(p.y());
    setRight(p.x());
}

void PGE_RectF::setBottomRight(PGE_PointF p)
{
    setRight(p.x());
    setBottom(p.y());
}

void PGE_RectF::setBottomLeft(PGE_PointF p)
{
    setLeft(p.x());
    setBottom(p.y());
}


void PGE_RectF::setTopLeft(double l, double t)
{
    setLeft(l);
    setTop(t);
}

void PGE_RectF::setTopRight(double r, double t)
{
    setTop(t);
    setRight(r);
}

void PGE_RectF::setBottomRight(double r, double b)
{
    setBottom(b);
    setRight(r);
}

void PGE_RectF::setBottomLeft(double l, double b)
{
    setBottom(b);
    setLeft(l);
}



double PGE_RectF::x()
{
    return m_x;
}

double PGE_RectF::y()
{
    return m_y;
}

double PGE_RectF::left()
{
    return m_x;
}

double PGE_RectF::top()
{
    return m_y;
}

double PGE_RectF::bottom()
{
    return m_b;
}

double PGE_RectF::right()
{
    return m_r;
}

double PGE_RectF::width()
{
    return m_w;
}

double PGE_RectF::height()
{
    return m_h;
}

PGE_PointF PGE_RectF::center()
{
    return PGE_PointF(m_x + m_w / 2.0, m_y + m_h / 2.0);
}

PGE_SizeF PGE_RectF::size()
{
    return PGE_SizeF(m_w, m_h);
}

double PGE_RectF::centerX()
{
    return m_x + m_w / 2.0;
}

double PGE_RectF::centerY()
{
    return m_y + m_h / 2.0;
}

bool PGE_RectF::collidePoint(double x, double y)
{
    if(x < m_x) return false;

    if(y < m_y) return false;

    if(x > m_r) return false;

    if(y > m_b) return false;

    return true;
}

bool PGE_RectF::collideRect(double x, double y, double w, double h)
{
    if((x + w) < m_x) return false;

    if((y + h) < m_y) return false;

    if(x > m_r) return false;

    if(y > m_b) return false;

    return true;
}

bool PGE_RectF::collideRect(PGE_RectF &rect)
{
    if(rect.m_r < m_x) return false;

    if(rect.m_b < m_y) return false;

    if(rect.m_x > m_r) return false;

    if(rect.m_y > m_b) return false;

    return true;
}

bool PGE_RectF::collideRectDeep(PGE_RectF &rect, double deep)
{
    if(rect.m_r < (m_x + deep)) return false;

    if(rect.m_b < (m_y + deep)) return false;

    if(rect.m_x > (m_r - deep)) return false;

    if(rect.m_y > (m_b - deep)) return false;

    return true;
}

bool PGE_RectF::collideRectDeep(PGE_RectF &rect, double deepX, double deepY)
{
    if(rect.m_r < (m_x + deepX)) return false;

    if(rect.m_b < (m_y + deepY)) return false;

    if(rect.m_x > (m_r - deepX)) return false;

    if(rect.m_y > (m_b - deepY)) return false;

    return true;
}

bool PGE_RectF::collideRect(PGE_Rect &rect)
{
    if(rect.m_r < int(m_x)) return false;

    if(rect.m_b < int(m_y)) return false;

    if(rect.m_x > int(m_r)) return false;

    if(rect.m_y > int(m_b)) return false;

    return true;
}
