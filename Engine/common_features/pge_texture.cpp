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

#include "pge_texture.h"

PGE_Texture::PGE_Texture():
    texture(0),
    w(0),
    h(0),
    frame_w(0),
    frame_h(0),
    inited(false),
    texture_layout(nullptr),
    format(0),
    nOfColors(0),
    ColorUpper{0.0f, 0.0f, 0.0f},
    ColorLower{0.0f, 0.0f, 0.0f}
{}
