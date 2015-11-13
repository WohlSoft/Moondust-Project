#include "luaclass_level_contact_detector.h"

#include <scenes/level/npc_detectors/lvl_dtc_contact.h>

luabind::scope Binding_Level_Class_ContactDetector::bindToLua()
{
    using namespace luabind;
    return
        class_<ContactDetector>("ContactDetector")
            .def("detected", &ContactDetector::detected)
            .def("getBlocks", &ContactDetector::getBlocks)
            .def("getBGOs", &ContactDetector::getBGOs)
            .def("getNPCs", &ContactDetector::getNPCs);
}
