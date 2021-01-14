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

#include "luaclass_level_contact_detector.h"

#include <scenes/level/npc_detectors/lvl_dtc_contact.h>

/***
Level NPC class, functions, and callback events
@module LevelNpcClass
*/

luabind::scope Binding_Level_Class_ContactDetector::bindToLua()
{
    using namespace luabind;
    return
        /***
        NPC contact detector context
        @type ContactDetector
        */
        class_<ContactDetector>("ContactDetector")
            /***
            Check are contacts was detected
            @function detected
            @treturn bool true if any object contacted this NPC
            */
            .def("detected", &ContactDetector::detected)
            /***
            Get array of touching blocks
            @function getBlocks
            @treturn table Array of BaseBlock references to the block objects
            */
            .def("getBlocks", &ContactDetector::getBlocks)
            /***
            Get array of touching BGOs
            @function getBGOs
            @treturn table Array of BaseBGO references to the BGO objects
            */
            .def("getBGOs", &ContactDetector::getBGOs)
            /***
            Get array of touching NPCs
            @function getNPCs
            @treturn table Array of BaseNPC references to the NPC objects
            */
            .def("getNPCs", &ContactDetector::getNPCs)
            /***
            Get array of touching Playable Characters
            @function getPlayers
            @treturn table Array of BasePlayer references to the Playable Characters objects
            */
            .def("getPlayers", &ContactDetector::getPlayers);
}
