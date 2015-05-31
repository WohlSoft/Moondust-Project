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

#ifndef GRAPHICS_LVL_BACKGRND_H
#define GRAPHICS_LVL_BACKGRND_H

#include <data_configs/obj_bg.h>

#include <common_features/pge_texture.h>
#include <common_features/rectf.h>

#include "lvl_camera.h"

//Magic background strip value
struct LVL_Background_strip
{
    double repeat_h;
    double top;
    double bottom;
    int height;
};

class PGE_LevelCamera;
class LVL_Background
{
    friend class PGE_LevelCamera;
public:
    LVL_Background();
    LVL_Background(const LVL_Background &_bg);
    ~LVL_Background();

    void setBg(obj_BG &bg);
    void setNone();
    void setBox(PGE_RectF &_box);
    void draw(float x, float y, float w, float h); //draw by camera position
    void applyColor();
    bool isInit();
    int  curBgId();

    bool _isInited;

    enum type
    {
        single_row      = 0,
        double_row      = 1,
        tiled           = 2,
        multi_layered   = 3
    };
    obj_BG setup;
    type bgType;

    bool isNoImage;

    PGE_Texture txData1;
    PGE_Texture txData2;

    bool isAnimated;
    long animator_ID;

    bool isMagic;
    QList<LVL_Background_strip > strips;

    PGEColor color;

    PGE_RectF box;

private:
    void construct();
    PGE_RectF backgrndG;//!< Used in draw process
};


#endif // GRAPHICS_LVL_BACKGRND_H
