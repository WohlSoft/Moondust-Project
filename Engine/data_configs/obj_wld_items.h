/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_WLD_ITEMS_H
#define OBJ_WLD_ITEMS_H

#include <graphics/graphics.h>
#include <ConfigPackManager/world/config_wld_generic.h>

////////////////////World map items///////////////////////////
struct obj_wld_generic
{
    obj_wld_generic():
        isInit(false),
        image(nullptr),
        textureID(0),
        textureArrayId(0),
        animator_ID(0)
    {}
    /*   OpenGL    */
    bool isInit;
    PGE_Texture * image;
    GLuint textureID;
    long textureArrayId;
    long animator_ID;
    /*   OpenGL    */

    WldGenericSetup setup;
};

typedef obj_wld_generic obj_w_tile;
typedef obj_wld_generic obj_w_scenery;
typedef obj_wld_generic obj_w_path;
typedef obj_wld_generic obj_w_level;

//Markers
struct wld_levels_Markers
{
    unsigned long path;
    unsigned long bigpath;
};

#endif // OBJ_WLD_ITEMS_H
