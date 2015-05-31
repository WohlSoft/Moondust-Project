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

#ifndef PGE_RECT_H
#define PGE_RECT_H

class PGE_RectF;
class PGE_Point;
class PGE_Size;

class PGE_Rect
{
    friend class PGE_RectF;
public:
    PGE_Rect();
    PGE_Rect(int x, int y, int w, int h);
    PGE_Rect(const PGE_Rect& r);
    PGE_Rect(const PGE_RectF& r);
    ~PGE_Rect();
    void setRect(int x, int y, int w, int h);
    void setPos(int x, int y);
    void setSize(int w, int h);

    void setLeft(int l);
    void setRight(int r);
    void setTop(int t);
    void setBottom(int b);

    void setX(int x);
    void setY(int y);
    void setWidth(int w);
    void setHeight(int h);

    void setTopLeft(PGE_Point p);
    void setTopRight(PGE_Point p);
    void setBottomRight(PGE_Point p);
    void setBottomLeft(PGE_Point p);

    void setTopLeft(int l, int t);
    void setTopRight(int r, int t);
    void setBottomRight(int r, int b);
    void setBottomLeft(int l, int b);

    int x();
    int y();

    int left();
    int top();
    int bottom();
    int right();

    int width();
    int height();

    PGE_Point center();
    PGE_Size size();

    bool collidePoint(int x, int y);
    bool collideRect(int x, int y, int w, int h);
    bool collideRect(PGE_Rect &rect);
    bool collideRect(PGE_RectF &rect);
private:
    int _x;
    int _y;
    int _w;
    int _h;
    int _r;
    int _b;
};

#endif // PGE_RECT_H
