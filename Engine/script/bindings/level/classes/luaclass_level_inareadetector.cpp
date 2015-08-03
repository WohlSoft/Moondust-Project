#include "luaclass_level_inareadetector.h"

#include <scenes/level/npc_detectors/lvl_dtc_inarea.h>

luabind::scope Binding_Level_Class_InAreaDetector::bindToLua()
{
    using namespace luabind;
    return
        class_<InAreaDetector>("InAreaDetector")
            .def("detected", (bool(*)())&InAreaDetector::detected)
            .def("detected", (bool(*)(long, long))&InAreaDetector::detected)
            .def("contacts", &InAreaDetector::contacts);
}
