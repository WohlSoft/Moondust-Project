/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_BLOCK_H
#define LVL_BLOCK_H

#include "lvl_base_object.h"
#include <data_configs/obj_block.h>
#include <common_features/rectf.h>
#include <PGE_File_Formats/file_formats.h>
#include "lvl_player.h"

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

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

        bool m_sizable;
        struct SizableBlockBorder
        {
            int32_t g;
            int32_t l;
            int32_t t;
            int32_t r;
            int32_t b;
        } sizable_border = {0, 0, 0, 0, 0};

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

        void setSizable(bool m_sizable);

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
        long lua_contentID_old();
        void lua_setContentID_old(long npcid);
        long lua_contentID();
        void lua_setContentID(long npcid);
        bool lua_invisible();
        void lua_setInvisible(bool iv);
        bool lua_slippery();
        void lua_setSlippery(bool sl);
        bool lua_isSolid();
        static luabind::scope bindToLua();
        /*******************************************/

    private:
        /**
         * @brief Render piece of texture, used in Sizable block rendering process
         * @param basePos Destination screen area where to render the base block
         * @param piecePos Block piece coordinates relative to the base coordinates
         * @param texture Texture coordinates of a fragment to render (relative to single frame)
         * @param textureFullW Full width of texture
         * @param textureFullH Full height of texture
         * @param ani Vertical sprite animation position context
         */
        void drawPiece(PGE_RectF basePos,
                       PGE_RectF piecePos,
                       PGE_RectF texturePos,
                       int32_t textureFullW, int32_t textureFullH,
                       AniPos ani);
        bool m_isInited = false;
};

#endif // LVL_BLOCK_H
