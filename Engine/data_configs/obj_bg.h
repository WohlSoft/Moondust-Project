/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_BG_H
#define OBJ_BG_H

#include <string>
#include <vector>
#include "../graphics/graphics.h"

struct obj_BG
{
    uint64_t        id = 0;
    std::string     name;
    std::string     image_n;

    /*   OpenGL    */
    bool            isInit = false;
    PGE_Texture     *image = nullptr;
    GLuint          textureID = 0;
    int             textureArrayId = 0;
    int             animator_ID = 0;
    PGEColor        Color_upper;
    PGEColor        Color_lower;
    /*   OpenGL    */

    uint32_t        type;//convert from string
    double          repeat_h;
    uint32_t        repead_v;
    uint32_t        attached;
    bool            editing_tiled;
    bool            animated;
    uint32_t        frames;
    uint32_t        framespeed;
    uint32_t        frame_h; //Hegth of the frame. Calculating automatically

    uint32_t        display_frame;

    bool            magic;
    uint32_t        magic_strips;
    std::string     magic_splits;
    std::vector<uint32_t> magic_splits_i;
    std::string     magic_speeds;
    std::vector<double> magic_speeds_i;

    std::string second_image_n;
    //QPixmap second_image;

    /*   OpenGL    */
    bool            second_isInit;
    PGE_Texture     *second_image;
    GLuint          second_textureID;
    int             second_textureArrayId;
    int             second_animator_ID;
    PGEColor        second_Color_upper;
    PGEColor        second_Color_lower;
    /*   OpenGL    */

    double          second_repeat_h;
    uint32_t        second_repeat_v;
    uint32_t        second_attached;
};

#endif // OBJ_BG_H
