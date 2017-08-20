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

#include <scenes/level/lvl_bgo.h>

/***
Level BGO class and functions
@module LevelBGOClass
*/

luabind::scope LVL_Bgo::bindToLua()
{
    using namespace luabind;
    /***
    Background Object base class, inherited from @{PhysBaseClass.PhysBase}
    @type BaseBGO
    */
    return class_<LVL_Bgo, PGE_Phys_Object, detail::null_type, LVL_Bgo>("BaseBGO")
            .def(constructor<>())
            /***
            ID of BGO registered in config pack (Read Only)
            @tfield ulong id
            */
            .property("id", &LVL_Bgo::lua_getID);
}
