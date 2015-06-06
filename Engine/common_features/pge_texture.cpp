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

#include "pge_texture.h"

PGE_Texture::PGE_Texture()
{
    inited=false;
    w=0;
    h=0;
    texture_layout=NULL; format=0;nOfColors=0;
    ColorUpper.r=0; ColorUpper.g=0; ColorUpper.b=0;
    ColorLower.r=0; ColorLower.g=0; ColorLower.b=0;
}
