/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_BGO_H
#define LVL_BGO_H

#include "lvl_base_object.h"
#include <data_configs/obj_bgo.h>
#include <PGE_File_Formats/file_formats.h>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class LVL_Bgo : public PGE_Phys_Object
{
    public:
        LVL_Bgo(LevelScene *_parent = NULL);
        LVL_Bgo(const LVL_Bgo &) = default;
        ~LVL_Bgo();
        void init();

        void transformTo_x(unsigned long id);

        LevelBGO data; //Local settings

        bool animated;
        int  animator_ID;

        obj_bgo *setup; //Global config

        void render(double camX, double camY);
        bool isInited();

        /************LUA-Specific functions*********/
        unsigned long lua_getID();
        static luabind::scope bindToLua();
        /*******************************************/

    private:
        bool _isInited;
};


#endif // LVL_BGO_H
