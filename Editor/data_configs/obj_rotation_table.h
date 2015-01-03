/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_ROTATION_TABLE_H
#define OBJ_ROTATION_TABLE



///
/// \brief The obj_rotation_table struct
///
/// Defining the transformation rule when you try to rotate or flip selected item
///
struct obj_rotation_table
{
    int type;           //!< Item type index
    long id;            //!< Item ID
    long rotate_left;   //!< Transform into ID on rotate left
    long rotate_right;  //!< Transform into ID on rotate right
    long flip_h;        //!< Transform into ID on flip horizontal
    long flip_v;        //!< Transform into ID on flip vertical
};

#endif


