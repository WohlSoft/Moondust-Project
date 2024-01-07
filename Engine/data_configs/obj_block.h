/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_BLOCK_H
#define OBJ_BLOCK_H

#include "../graphics/graphics.h"

#include <string>
#include <ConfigPackManager/level/config_block.h>

struct obj_block
{
    /*   OpenGL    */
    bool isInit;
    PGE_Texture *image;
    GLuint textureID;
    int32_t textureArrayId;
    int32_t animator_ID;
    /*   OpenGL    */

    BlockSetup setup;
};


#endif // OBJ_BLOCK_H
