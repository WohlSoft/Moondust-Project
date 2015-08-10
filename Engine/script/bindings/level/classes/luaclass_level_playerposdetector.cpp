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

luabind::scope Binding_Level_Class_PlayerPosDetector::bindToLua()
{
    using namespace luabind;
    return
        class_<PlayerPosDetector>("PlayerPosDetector")
            .def("detected", &PlayerPosDetector::detected)
            .def("directedTo", &PlayerPosDetector::directedTo)
            .def("playerDirection", &PlayerPosDetector::playersDirection)
            .def("positionX", &PlayerPosDetector::positionX)
            .def("positionY", &PlayerPosDetector::positionY)
            .def("playersCharID", &PlayerPosDetector::playersCharID)
            .def("playerStateID", &PlayerPosDetector::playersStateID)
            .def("playerPtr", &PlayerPosDetector::playerPtr);
}
