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

#ifndef PGE_Size_H
#define PGE_Size_H

class PGE_SizeF;
class PGE_Size
{
    friend class PGE_SizeF;
public:
    PGE_Size();
    PGE_Size(const PGE_Size &p);
    PGE_Size(const PGE_SizeF &p);
    PGE_Size(int w, int h);
    ~PGE_Size();
    void setSize(int w, int h);
    void setWidth(int w);
    void setHeight(int h);
    bool isNull();
    int w();
    int h();
private:
    int _w;
    int _h;
};

#endif // PGE_Size_H
