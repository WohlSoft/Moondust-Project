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

PGE_RectF::PGE_RectF(const PGE_RectF &r)
{
    _x = r._x;
    _y = r._y;
    _w = r._w;
    _h = r._h;
    _r = r._r;
    _b = r._b;
}

PGE_RectF::PGE_RectF(const PGE_Rect &r)
{
    _x = r._x;
    _y = r._y;
    _w = r._w;
    _h = r._h;
    _r = r._r;
    _b = r._b;
}

PGE_RectF::~PGE_RectF()
{}

void PGE_RectF::setRect(double x, double y, double w, double h)
{
    _x=x;
    _y=y;
    _w=w;
    _h=h;
    _r=_x+_w;
    _b=_y+_h;
}

void PGE_RectF::setPos(double x, double y)
{
    _x=x;
    _y=y;
    _r=_x+_w;
    _b=_y+_h;
}

void PGE_RectF::setSize(double w, double h)
{
    _w=w;
    _h=h;
    _r=_x+_w;
    _b=_y+_h;
}

void PGE_RectF::setLeft(double l)
{
    _x=l;
    _w=_r-_x;
}

void PGE_RectF::setRight(double r)
{
    _r=r;
    _w=_r-_x;
}

void PGE_RectF::setTop(double t)
{
    _y=t;
    _h=_b-_y;
}

void PGE_RectF::setBottom(double b)
{
    _b=b;
    _h=_b-_y;
}


void PGE_RectF::setX(double x)
{
    _x=x;
    _r=_x+_w;
}

void PGE_RectF::setY(double y)
{
    _y=y;
    _b=_y+_h;
}

void PGE_RectF::setWidth(double w)
{
    _w=w;
    _r=_x+_w;
}

void PGE_RectF::setHeight(double h)
{
    _h=h;
    _b=_y+_h;
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
    return _x;
}

double PGE_RectF::y()
{
    return _y;
}

double PGE_RectF::left()
{
    return _x;
}

double PGE_RectF::top()
{
    return _y;
}

double PGE_RectF::bottom()
{
    return _b;
}

double PGE_RectF::right()
{
    return _r;
}

double PGE_RectF::width()
{
    return _w;
}

double PGE_RectF::height()
{
    return _h;
}

PGE_PointF PGE_RectF::center()
{
    return PGE_PointF(_x+_w/2.0, _y+_h/2.0);
}

PGE_SizeF PGE_RectF::size()
{
    return PGE_SizeF(_w, _h);
}

bool PGE_RectF::collidePoint(double x, double y)
{
    if(x<_x) return false;
    if(y<_y) return false;
    if(x>_r) return false;
    if(y>_b) return false;
    return true;
}

bool PGE_RectF::collideRect(double x, double y, double w, double h)
{
    if((x+w) <_x) return false;
    if((y+h) <_y) return false;
    if( x > _r) return false;
    if( y > _b) return false;
    return true;
}

bool PGE_RectF::collideRect(PGE_RectF &rect)
{
    if(rect._r<_x) return false;
    if(rect._b<_y) return false;
    if(rect._x>_r) return false;
    if(rect._y>_b) return false;
    return true;
}

bool PGE_RectF::collideRectDeep(PGE_RectF &rect, float deep)
{
    if(rect._r<(_x+deep)) return false;
    if(rect._b<(_y+deep)) return false;
    if(rect._x>(_r-deep)) return false;
    if(rect._y>(_b-deep)) return false;
    return true;
}

bool PGE_RectF::collideRect(PGE_Rect &rect)
{
    if(rect._r<(int)_x) return false;
    if(rect._b<(int)_y) return false;
    if(rect._x>(int)_r) return false;
    if(rect._y>(int)_b) return false;
    return true;
}

