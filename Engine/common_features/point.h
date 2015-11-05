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

#ifndef PGE_Point_H
#define PGE_Point_H

class PGE_PointF;
class PGE_Point
{
    friend class PGE_PointF;
public:
    PGE_Point();
    PGE_Point(const PGE_Point &p);
    PGE_Point(const PGE_PointF &p);
    PGE_Point(int x, int y);
    ~PGE_Point();
    void setPoint(int x, int y);
    void setX(int x);
    void setY(int y);
    bool isNull();
    int x();
    int y();
    friend bool operator==(const PGE_Point& lhs, const PGE_Point& rhs);
    friend bool operator!=(const PGE_Point& lhs, const PGE_Point& rhs);
private:
    int _x;
    int _y;
};

bool operator==(const PGE_Point& lhs, const PGE_Point& rhs);
bool operator!=(const PGE_Point& lhs, const PGE_Point& rhs);

#endif // PGE_Point_H
