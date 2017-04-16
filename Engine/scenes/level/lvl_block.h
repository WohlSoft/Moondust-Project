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
        void construct();
    public:
        LVL_Block(LevelScene *_parent = nullptr);
        LVL_Block(const LVL_Block &) = default;        
        ~LVL_Block();
        void init(bool force=false);

        LevelBlock  data; //Local settings
        LevelBlock  dataInitial; //Initial settings

        bool animated;
        bool sizable;
        long animator_ID;
        double offset_x;
        double offset_y;

        bool m_isHidden;
        int  m_blockedOrigin[BLOCK_FILTER_COUNT];

        bool m_destroyed = false;

        obj_block *setup; //Global config

        enum directions
        {
            left = 0,
            right,
            up,
            down
        };

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
        bool m_isInited = false;
};

#endif // LVL_BLOCK_H
