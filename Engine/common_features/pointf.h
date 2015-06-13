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
    PGE_PointF();
    PGE_PointF(const PGE_PointF &p);
    PGE_PointF(const PGE_Point &p);
    PGE_PointF(double x, double y);
    ~PGE_PointF();
    void setPoint(double x, double y);
    void setX(double x);
    void setY(double y);
    bool isNull();
    double x();
    double y();
private:
    double _x;
    double _y;
};

#endif // PGE_POINTF_H
