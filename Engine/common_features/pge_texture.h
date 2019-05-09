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

#ifndef PGE_TEXTURE_H
#define PGE_TEXTURE_H

#include <SDL2/SDL_opengl.h>

struct PGEColor
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
};

struct PGE_Texture
{
    GLuint texture = 0; // Texture object handle
    int w = 0; //Width of the texture.
    int h = 0; //Height of the texture.
    int frame_w = 0;//Width of animated frame
    int frame_h = 0;//Height of animated frame
    bool inited = false; //Texture is valid and loaded
    GLubyte *texture_layout = nullptr;
    GLenum format = 0;
    GLint  nOfColors = 0;

    PGEColor ColorUpper;
    PGEColor ColorLower;
};


#endif // PGE_TEXTURE_H
