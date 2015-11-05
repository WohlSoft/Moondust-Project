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

#ifndef PGE_POINTF_H
#define PGE_POINTF_H

class PGE_Point;
class PGE_PointF
{
    friend class PGE_Point;
public:
    inline PGE_PointF() { setPoint(0, 0); }
    inline PGE_PointF(const PGE_PointF &p) { _x=p._x; _y=p._y; }
    PGE_PointF(const PGE_Point &p);
    inline PGE_PointF(double x, double y) { _x=x; _y=y; }
    inline ~PGE_PointF() {}
    inline void setPoint(double x, double y) { _x=x; _y=y; }
    inline void setX(double x) { _x=x; }
    inline void setY(double y) { _y=y; }
    inline bool isNull() { return ((_x==0)&&(_y==0)); }
    inline double x() { return _x; }
    inline double y() { return _y; }
    friend bool operator==(const PGE_PointF& lhs, const PGE_PointF& rhs);
    friend bool operator!=(const PGE_PointF& lhs, const PGE_PointF& rhs);
private:
    double _x;
    double _y;
};

bool operator==(const PGE_PointF& lhs, const PGE_PointF& rhs);
bool operator!=(const PGE_PointF& lhs, const PGE_PointF& rhs);

#endif // PGE_POINTF_H
