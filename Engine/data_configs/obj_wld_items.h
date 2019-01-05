/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
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
    PGE_Texture *image;
    GLuint textureID;
    int textureArrayId;
    int animator_ID;
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
