/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_base_object.h"
#include <data_configs/obj_block.h>
#include <common_features/rectf.h>
#include <PGE_File_Formats/file_formats.h>
#include "lvl_player.h"

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class LVL_Block : public PGE_Phys_Object
{
    public:
        LVL_Block(LevelScene *_parent = NULL);
        ~LVL_Block();
        void init();

        LevelBlock data; //Local settings

        bool animated;
        bool sizable;
        long animator_ID;
        double offset_x;
        double offset_y;

        bool m_isHidden;
        int  m_blockedOrigin[BLOCK_FILTER_COUNT];

        bool m_destroyed;

        obj_block *setup; //Global config

        enum directions
        {
            left = 0,
            right,
            up,
            down
        };

        enum shapes
        {
            shape_rect              = 0,
            shape_tr_left_bottom    = 1,
            shape_tr_right_bottom   = -1,
            shape_tr_left_top       = 2,
            shape_tr_right_top      = -2,
            stape_polygon           = 3
        };

        int LEGACY_shape;
        double LEGACY_shape_slope_angle_ratio;

        int taskToTransform;
        int taskToTransform_t;
        void transformTo(unsigned long id, int type = 2);
        void transformTo_x(unsigned long id);

        unsigned long transformedFromBlockID;
        unsigned long transformedFromNpcID;

        void hit(directions _dir = up);
        void hit(directions _dir, LVL_Player *player, int numHits = 1);
        void destroy(bool playEffect = false);
        directions hitDirection;
        void setDestroyed(bool dstr);

        long double zIndex();

        /**************Fader**************/
        double fadeOffset;
        double targetOffset;
        double fade_step;
        double fadeSpeed;
        void setFade(double speed, double target, double step);
        bool isFading();
        bool tickFader(double ticks);
        double manual_ticks;
        void fadeStep();
        /**************Fader**************/

        void render(double camX, double camY);

        bool isInited();

        /************LUA-Specific functions*********/
        long lua_getID();
        int  lua_contentID_old();
        void lua_setContentID_old(int npcid);
        int  lua_contentID();
        void lua_setContentID(int npcid);
        bool lua_invisible();
        void lua_setInvisible(bool iv);
        bool lua_slippery();
        void lua_setSlippery(bool sl);
        bool lua_isSolid();
        static luabind::scope bindToLua();
        /*******************************************/

    private:
        void drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture);
        bool _isInited;
};

#endif // LVL_BLOCK_H
