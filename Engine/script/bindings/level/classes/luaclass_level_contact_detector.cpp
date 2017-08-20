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
