/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <luabind/luabind.hpp>
#include <luabind/detail/primitives.hpp> // for null_type

#include <scenes/level/lvl_bgo.h>

/***
Level BGO class and functions
@module LevelBGOClass
*/

luabind::scope LVL_Bgo::bindToLua()
{
    using namespace luabind;
    using namespace luabind::detail;
    /***
    Background Object base class, inherited from @{PhysBaseClass.PhysBase}
    @type BaseBGO
    */
    return class_<LVL_Bgo, PGE_Phys_Object, null_type, LVL_Bgo>("BaseBGO")
            .def(constructor<>())
            /***
            ID of BGO registered in config pack (Read Only)
            @tfield ulong id
            */
            .property("id", &LVL_Bgo::lua_getID);
}
