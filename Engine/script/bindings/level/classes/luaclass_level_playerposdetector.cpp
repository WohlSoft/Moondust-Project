#include "luaclass_level_playerposdetector.h"

#include <scenes/level/npc_detectors/lvl_dtc_player_pos.h>
#include <scenes/level/lvl_player.h>

/*

    int directedTo();       //! where you shold go to be nearer to player
    int playersDirection(); //! self direction of player
    PGE_PointF position();  //! X-Y position of a player's center
    int playersCharID();    //! returns ID of playable character
    int playersStateID();   //! returns ID of player's state
    LVL_Player * playerPtr();//! returns pointer to detected platable character

*/

/***
Level NPC class, functions, and callback events
@module LevelNpcClass
*/

luabind::scope Binding_Level_Class_PlayerPosDetector::bindToLua()
{
    using namespace luabind;
    return
        /***
        NPCs' Player position detector
        @type PlayerPosDetector
        */
        class_<PlayerPosDetector>("PlayerPosDetector")
            /***
            Was playable character detected on the scene?
            @function detected
            @treturn bool True if any playable character is alive and presented on the level scene
            */
            .def("detected", &PlayerPosDetector::detected)
            /***
            Direction of left or right side where nearest playable character is located
            @function directedTo
            @treturn bool -1 if nearest playable character lefter than this NPC or +1 if righter
            */
            .def("directedTo", &PlayerPosDetector::directedTo)
            /***
            Face direction of nearest playable character
            @function playerDirection
            @treturn bool -1 if nearest playable character is faced to left or +1 if nearest playable character is faced to right
            */
            .def("playerDirection", &PlayerPosDetector::playersDirection)

            /***
            Absolute horizintal position of nearest playable character
            @function positionX
            @treturn double X position of nearest playable character
            */
            .def("positionX", &PlayerPosDetector::positionX)
            /***
            Absolute vertical position of nearest playable character
            @function positionY
            @treturn double Y position of nearest playable character
            */
            .def("positionY", &PlayerPosDetector::positionY)
            /***
            Get ID of nearest playable character
            @function playersCharID
            @treturn ulong ID of nearest playable character registered in config pack
            */
            .def("playersCharID", &PlayerPosDetector::playersCharID)
            /***
            Get ID of nearest playable character's state
            @function playerStateID
            @treturn ulong ID of nearest playable character's state registered in config pack
            */
            .def("playerStateID", &PlayerPosDetector::playersStateID)
            /***
            Get reference to nearest playable character
            @function playerPtr
            @treturn BasePlayer Reference to nearest playable character
            */
            .def("playerPtr", &PlayerPosDetector::playerPtr);
}
