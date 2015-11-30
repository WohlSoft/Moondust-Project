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

#ifndef PGE_TEXTURE_H
#define PGE_TEXTURE_H

#define SDL_MAIN_HANDLED
#include <SDL2/SDL_opengl.h>

struct PGEColor
{
    float r;
    float g;
    float b;
};

struct PGE_Texture
{
    PGE_Texture();
    GLuint texture; // Texture object handle
    int w; //Width of the texture.
    int h; //Height of the texture.
    bool inited; //Texture is valid and loaded
    GLubyte *texture_layout;
    GLenum format;
    GLint  nOfColors;

    PGEColor ColorUpper;
    PGEColor ColorLower;
};


#endif // PGE_TEXTURE_H
