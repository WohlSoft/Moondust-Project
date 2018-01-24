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

#include <scenes/level/lvl_block.h>

/***
Level Block class and functions
@module LevelBlockClass
*/

luabind::scope LVL_Block::bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Block, PGE_Phys_Object, detail::null_type, LVL_Block>("BaseBlock")
            /***
            Block Object base class, inherited from @{PhysBaseClass.PhysBase}
            @type BaseBlock
            */
            .def(constructor<>())

            /***
            ID of Block registered in config pack (Read Only)
            @tfield ulong id
            */
            .property("id", &LVL_Block::lua_getID)
            /***
            ID of NPC or count of coins contained in the block.
            @tfield long contentNpcId Positive - ID of NPC, Negative - Count of coins
            */
            .property("contentNpcId", &LVL_Block::lua_contentID, &LVL_Block::lua_setContentID)
            /***
            ID of NPC or count of coins contained in the block, alias to @{contentNpcId}
            @tfield long contentNPC_ID Positive - ID of NPC, Negative - Count of coins
            */
            .property("contentNPC_ID", &LVL_Block::lua_contentID, &LVL_Block::lua_setContentID)
            /***
            Legacy merge of NPC-ID and count of coins contained in the block.
            @tfield long contentNpcId Between 1 and 999 - count of coins, starts from 1001, NPC-ID with added 1000 (to get NPC-ID you need to just subtract 1000)
            */
            .property("contentID", &LVL_Block::lua_contentID_old, &LVL_Block::lua_setContentID_old)

            /***
            Is block invisible
            @tfield bool True if block is invisible
            */
            .property("invisible", &LVL_Block::lua_invisible, &LVL_Block::lua_setInvisible)
            /***
            Is block has a slippery surface
            @tfield bool True if block has a slippery surface
            */
            .property("slippery", &LVL_Block::lua_slippery, &LVL_Block::lua_setSlippery)
            /***
            The block is solid (collision is turned on all sides) (Read Only)
            @tfield bool If block has all collisions turned on
            */
            .property("isSolid", &LVL_Block::lua_isSolid)
            /***
            Is block destroyed (Read Only)
            @tfield bool isDestroyed true if block is destroyed
            */
            .def_readonly("isDestroyed", &LVL_Block::m_destroyed)
            /***
            Is block destroyed, alias to @{isDestroyed} (Read Only)
            @tfield bool is_destroyed true if block is destroyed
            */
            .def_readonly("is_destroyed", &LVL_Block::m_destroyed)

            /***
            Destroy this block
            @function remove
            @tparam bool playEffect Play destruction effect while destroying this block
            */
            .def("remove", &LVL_Block::destroy)
            /***
            Destroy this block, alias to @{remove}
            @function destroy
            @tparam bool playEffect Play destruction effect while destroying this block
            */
            .def("destroy", &LVL_Block::destroy)

            /***
            Hit this block at right side
            @function hit
            */

            /***
            Hit this block to target side
            @function hit
            @tparam int direction Side of block to hit:<br>
            <ul>
                    <li>0 - left</li>
                    <li>1 - right</li>
                    <li>2 - up</li>
                    <li>3 - down</li>
            </ul>
            */
            .def("hit", static_cast<void(LVL_Block::*)(LVL_Block::directions)>(&LVL_Block::hit))

            /***
            Hit this block to target side by giving playable character
            @function hit
            @tparam int direction Side of block to hit:<br>
            <ul>
                    <li>0 - left</li>
                    <li>1 - right</li>
                    <li>2 - up</li>
                    <li>3 - down</li>
            </ul>
            @tparam BasePlayer player Reference to playable character who hits this block
            */

            /***
            Hit this block to target side by giving playable character, multiple times on one call
            @function hit
            @tparam int direction Side of block to hit:<br>
            <ul>
                    <li>0 - left</li>
                    <li>1 - right</li>
                    <li>2 - up</li>
                    <li>3 - down</li>
            </ul>
            @tparam BasePlayer player Reference to playable character who hits this block
            @tparam int number of repeating hits
            */
            .def("hit", static_cast<void(LVL_Block::*)(LVL_Block::directions, LVL_Player*, int)>(&LVL_Block::hit))

            /***
            Transform this block into another
            @function transformTo
            @tparam ulong id ID of another block to transform
            */

            /***
            Transform this block into another block or into NPC
            @function transformTo
            @tparam ulong id ID of another block or NPC to transform
            @tparam int type Type of object to transform: 1 - NPC, 2 - Another Block
            */
            .def("transformTo", &LVL_Block::transformTo)

            /***
            Original ID of block from which NPC was transformed (Read Only)
            @tfield ulong transformedFromBlockID If 0, NPC was not transformed from block
            */
            .def_readonly("transformedFromBlockID", &LVL_Block::transformedFromBlockID)

            /***
            Original ID of NPC from which NPC was transformed (Read Only)
            @tfield ulong transformedFromNpcID If 0, NPC was not transformed from another NPC
            */
            .def_readonly("transformedFromNpcID", &LVL_Block::transformedFromNpcID)
            ;
}


