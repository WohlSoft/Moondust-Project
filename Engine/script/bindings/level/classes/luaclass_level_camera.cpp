
#include <scenes/level/lvl_camera.h>

luabind::scope PGE_LevelCamera::bindToLua()
{
    using namespace luabind;
    return
        class_<PGE_LevelCamera>("LevelCamera")
            .property("x", &PGE_LevelCamera::posX)
            .property("y", &PGE_LevelCamera::posY)
            .property("renderX", &PGE_LevelCamera::renderX)
            .property("renderY", &PGE_LevelCamera::renderY)
            .property("centerX", &PGE_LevelCamera::centerX)
            .property("centerY", &PGE_LevelCamera::centerY)
            .property("width", &PGE_LevelCamera::w)
            .property("height", &PGE_LevelCamera::h);
}




