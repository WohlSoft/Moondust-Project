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
