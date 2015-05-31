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

#ifndef LVL_BLOCK_H
#define LVL_BLOCK_H

#include <physics/base_object.h>
#include <data_configs/obj_block.h>
#include <common_features/rectf.h>
#include <PGE_File_Formats/file_formats.h>

class LVL_Block : public PGE_Phys_Object
{
public:
    LVL_Block();
    ~LVL_Block();
    void init();

    LevelBlock data; //Local settings
    bool slippery;

    bool animated;
    bool sizable;
    long animator_ID;
    float offset_x;
    float offset_y;

    bool isHidden;

    bool destroyed;

    obj_block * setup;//Global config

    enum directions{
        left=0,
        right,
        up,
        down
    };

    int taskToTransform;
    int taskToTransform_t;
    void transformTo(long id, int type=0);
    void transformTo_x(long id);
    b2Fixture *f_block;
    b2Fixture *f_edge;


    void hit(directions _dir=up);
    directions hitDirection;

    GLdouble zIndex();

    /**************Fader**************/
    float fadeOffset;
    float targetOffset;
    float fade_step;
    int   fadeSpeed;
    void setFade(int speed, float target, float step);
    bool isFading();
    bool tickFader(int ticks);
    int manual_ticks;
    void fadeStep();
    /**************Fader**************/

    //float posX();
    //float posY();
    void render(double camX, double camY);
private:
    void drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture);
};

#endif // LVL_BLOCK_H
