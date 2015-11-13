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

#include <scenes/level/lvl_block.h>

luabind::scope LVL_Block::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Block, PGE_Phys_Object, detail::null_type, LVL_Block>("Block")
            .def(constructor<>())
            //Functions
            .def("remove", &LVL_Block::destroy)
            .def("hit", static_cast<void(LVL_Block::*)(LVL_Block::directions)>(&LVL_Block::hit))
            .def("hit", static_cast<void(LVL_Block::*)(bool, LVL_Player*, int)>(&LVL_Block::hit))

            //Properties
            .property("id", &LVL_Block::lua_getID)
            .property("contentID", &LVL_Block::lua_contentID_old, &LVL_Block::lua_setContentID_old)
            .property("contentNPC_ID", &LVL_Block::lua_contentID, &LVL_Block::lua_setContentID)
            .property("invisible", &LVL_Block::lua_invisible, &LVL_Block::lua_setInvisible)
            .property("slippery", &LVL_Block::lua_slippery, &LVL_Block::lua_setSlippery)
            .property("isSolid", &LVL_Block::lua_isSolid);
}


