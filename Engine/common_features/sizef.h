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

#ifndef PGE_SizeF_H
#define PGE_SizeF_H

class PGE_Size;
class PGE_SizeF
{
    friend class PGE_Size;
public:
    PGE_SizeF();
    PGE_SizeF(const PGE_SizeF &p);
    PGE_SizeF(const PGE_Size &p);
    PGE_SizeF(double w, double h);
    ~PGE_SizeF();
    void setSize(double w, double h);
    void setWidth(double w);
    void setHeight(double h);
    bool isNull();
    double w();
    double h();
private:
    double _w;
    double _h;
};

#endif // PGE_SizeF_H
