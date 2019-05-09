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

#include "luaclass_level_inareadetector.h"

#include <scenes/level/npc_detectors/lvl_dtc_inarea.h>

/***
Level NPC class, functions, and callback events
@module LevelNpcClass
*/

luabind::scope Binding_Level_Class_InAreaDetector::bindToLua()
{
    using namespace luabind;
    return
        /***
        NPC In-Area detector context
        @type InAreaDetector
        */
        class_<InAreaDetector>("InAreaDetector")
            /***
            Is any object appears in the trap zone
            @function detected
            @return bool True if any allowed by filter element is touches a trap of detector
            */
            .def("detected", static_cast<bool(InAreaDetector::*)()>(&InAreaDetector::detected))
            /***
            Is any object of specified <i>id</i> and <i>type</i> is in the trap zone
            @function detected
            @tparam long id ID of necessary element
            @tparam long type Type of necessary element
            @return bool True if any allowed by filter element is touches a trap of detector
            */
            .def("detected", static_cast<bool(InAreaDetector::*)(long, long)>(&InAreaDetector::detected))
            /***
            Get array of blocks touching a trap zone
            @function getBlocks
            @treturn table Array of BaseBlock references to the block objects
            */
            .def("getBlocks",  &InAreaDetector::getBlocks)
            /***
            Get array of BGOs touching a trap zone
            @function getBGOs
            @treturn table Array of BaseBGO references to the BGO objects
            */
            .def("getBGOs",    &InAreaDetector::getBGOs)
            /***
            Get array of NPCs touching a trap zone
            @function getNPCs
            @treturn table Array of BaseNPC references to the NPC objects
            */
            .def("getNPCs",    &InAreaDetector::getNPCs)
            /***
            Get array of Playable Characters touching a trap zone
            @function getPlayers
            @treturn table Array of BasePlayer references to the Playable Characters objects
            */
            .def("getPlayers", &InAreaDetector::getPlayers)
            /***
            Get count of objects touching a trap zone
            @function contacts
            @treturn int Count of contacted objects
            */
            .def("contacts",   &InAreaDetector::contacts);
}
