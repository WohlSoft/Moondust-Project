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

#include <scenes/level/lvl_bgo.h>

luabind::scope LVL_Bgo::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Bgo, PGE_Phys_Object, detail::null_type, LVL_Bgo>("BGO")
            .def(constructor<>())
            //Properties
            .property("id", &LVL_Bgo::lua_getID);
}


