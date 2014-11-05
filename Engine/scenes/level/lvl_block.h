/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_BLOCK_H
#define LVL_BLOCK_H

#include "../../physics/base_object.h"
#include "../../data_configs/obj_block.h"

#include <file_formats.h>

class LVL_Block : public PGE_Phys_Object
{
public:
    LVL_Block();
    ~LVL_Block();
    void init();

    LevelBlock* data; //Local settings
    bool slippery;

    bool animated;
    bool sizable;
    long animator_ID;

    obj_block * setup;//Global config

    //float posX();
    //float posY();
    void render(float camX, float camY);
private:
    void drawPiece(QRectF target, QRectF block, QRectF texture);
};

#endif // LVL_BLOCK_H
