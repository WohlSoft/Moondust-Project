/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_BG_H
#define OBJ_BG_H

#include <string>
#include <vector>
#include "../graphics/graphics.h"
#include <ConfigPackManager/level/config_bg.h>

struct obj_BG
{
    PGEColor        fill_color;

    /*   OpenGL    */
    bool            isInit = false;
    PGE_Texture     *image = nullptr;
    GLuint          textureID = 0;
    int             textureArrayId = 0;
    int             animator_ID = 0;
    PGEColor        Color_upper;
    PGEColor        Color_lower;
    /*   OpenGL    */

    /*   OpenGL    */
    bool            second_isInit;
    PGE_Texture     *second_image;
    GLuint          second_textureID;
    int             second_textureArrayId;
    int             second_animator_ID;
    PGEColor        second_Color_upper;
    PGEColor        second_Color_lower;
    /*   OpenGL    */

    struct TextureId
    {
        GLuint      textureId = 0;
        PGE_Texture *image = nullptr;
        int64_t     textureArrayId = -1;
        int64_t     animatorId = -1;
    };

    //! Level textures bank index per every layer. -1 is not loaded, >=0 index
    std::vector<TextureId> texturePerLayer;

    BgSetup setup;
};

#endif // OBJ_BG_H
