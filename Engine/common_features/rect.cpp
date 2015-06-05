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

#include "rect.h"
#include "rectf.h"
#include "point.h"
#include "size.h"

PGE_Rect::PGE_Rect()
{
    setRect(0,0,0,0);
}

PGE_Rect::PGE_Rect(int x, int y, int w, int h)
{
    setRect(x, y,  w, h);
}

PGE_Rect::PGE_Rect(const PGE_Rect &r)
{
    _x = r._x;
    _y = r._y;
    _w = r._w;
    _h = r._h;
    _r = r._r;
    _b = r._b;
}

PGE_Rect::PGE_Rect(const PGE_RectF &r)
{
    _x = r._x;
    _y = r._y;
    _w = r._w;
    _h = r._h;
    _r = r._r;
    _b = r._b;
}

PGE_Rect::~PGE_Rect()
{}

void PGE_Rect::setRect(int x, int y, int w, int h)
{
    _x=x;
    _y=y;
    _w=w;
    _h=h;
    _r=_x+_w;
    _b=_y+_h;
}

void PGE_Rect::setPos(int x, int y)
{
    _x=x;
    _y=y;
    _r=_x+_w;
    _b=_y+_h;
}

void PGE_Rect::setSize(int w, int h)
{
    _w=w;
    _h=h;
    _r=_x+_w;
    _b=_y+_h;
}

void PGE_Rect::setLeft(int l)
{
    _x=l;
    _w=_r-_x;
}

void PGE_Rect::setRight(int r)
{
    _r=r;
    _w=_r-_x;
}

void PGE_Rect::setTop(int t)
{
    _y=t;
    _h=_b-_y;
}

void PGE_Rect::setBottom(int b)
{
    _b=b;
    _h=_b-_y;
}


void PGE_Rect::setX(int x)
{
    _x=x;
    _r=_x+_w;
}

void PGE_Rect::setY(int y)
{
    _y=y;
    _b=_y+_h;
}

void PGE_Rect::setWidth(int w)
{
    _w=w;
    _r=_x+_w;
}

void PGE_Rect::setHeight(int h)
{
    _h=h;
    _b=_y+_h;
}

void PGE_Rect::setTopLeft(PGE_Point p)
{
    setLeft(p.x());
    setTop(p.y());
}

void PGE_Rect::setTopRight(PGE_Point p)
{
    setTop(p.y());
    setRight(p.x());
}

void PGE_Rect::setBottomRight(PGE_Point p)
{
    setRight(p.x());
    setBottom(p.y());
}

void PGE_Rect::setBottomLeft(PGE_Point p)
{
    setLeft(p.x());
    setBottom(p.y());
}

void PGE_Rect::setTopLeft(int l, int t)
{
    setLeft(l);
    setTop(t);
}

void PGE_Rect::setTopRight(int r, int t)
{
    setTop(t);
    setRight(r);
}

void PGE_Rect::setBottomRight(int r, int b)
{
    setBottom(b);
    setRight(r);
}

void PGE_Rect::setBottomLeft(int l, int b)
{
    setBottom(b);
    setLeft(l);
}



int PGE_Rect::x()
{
    return _x;
}

int PGE_Rect::y()
{
    return _y;
}

int PGE_Rect::left()
{
    return _x;
}

int PGE_Rect::top()
{
    return _y;
}

int PGE_Rect::bottom()
{
    return _b;
}

int PGE_Rect::right()
{
    return _r;
}

int PGE_Rect::width()
{
    return _w;
}

int PGE_Rect::height()
{
    return _h;
}

PGE_Point PGE_Rect::center()
{
    return PGE_Point(_x+_w/2, _y+_h/2);
}

PGE_Size PGE_Rect::size()
{
    return PGE_Size(_w, _h);
}

bool PGE_Rect::collidePoint(int x, int y)
{
    if(x<_x) return false;
    if(y<_y) return false;
    if(x>_r) return false;
    if(y>_b) return false;
    return true;
}

bool PGE_Rect::collideRect(int x, int y, int w, int h)
{
    if((x+w) <_x) return false;
    if((y+h) <_y) return false;
    if( x > _r) return false;
    if( y > _b) return false;
    return true;
}

bool PGE_Rect::collideRect(PGE_Rect &rect)
{
    if(rect._r<_x) return false;
    if(rect._b<_y) return false;
    if(rect._x>_r) return false;
    if(rect._y>_b) return false;
    return true;
}

bool PGE_Rect::collideRect(PGE_RectF &rect)
{
    if((int)rect._r<_x) return false;
    if((int)rect._b<_y) return false;
    if((int)rect._x>_r) return false;
    if((int)rect._y>_b) return false;
    return true;
}

