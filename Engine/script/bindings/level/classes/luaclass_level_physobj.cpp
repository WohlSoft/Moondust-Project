#include "luaclass_level_physobj.h"

#include <scenes/level/lvl_base_object.h>
#include <common_features/rectf.h>
#include <common_features/pointf.h>

luabind::scope Binding_Level_Class_PhysObj::bindToLua()
{
    using namespace luabind;
    return
        class_<PGE_Phys_Object>("PhysBase")
            //Size and position
            .property("x", &PGE_Phys_Object::posX, &PGE_Phys_Object::setPosX)
            .property("y", &PGE_Phys_Object::posY, &PGE_Phys_Object::setPosY)
            .property("center_x", &PGE_Phys_Object::posCenterX, &PGE_Phys_Object::setCenterX)
            .property("center_y", &PGE_Phys_Object::posCenterY, &PGE_Phys_Object::setCenterY)
            .property("width", &PGE_Phys_Object::width, &PGE_Phys_Object::setWidth)
            .property("height", &PGE_Phys_Object::height, &PGE_Phys_Object::setHeight)
            .property("top", &PGE_Phys_Object::top, &PGE_Phys_Object::setTop)
            .property("left", &PGE_Phys_Object::left, &PGE_Phys_Object::setLeft)
            .property("right", &PGE_Phys_Object::right, &PGE_Phys_Object::setRight)
            .property("bottom", &PGE_Phys_Object::bottom, &PGE_Phys_Object::setBottom)

            //Physics
            .property("paused_physics", &PGE_Phys_Object::isPaused, &PGE_Phys_Object::setPaused)
            .property("speedX", &PGE_Phys_Object::speedX, &PGE_Phys_Object::setSpeedX)
            .property("speedY", &PGE_Phys_Object::speedY, &PGE_Phys_Object::setSpeedY)
            .property("gravity", &PGE_Phys_Object::gravityScale, &PGE_Phys_Object::setGravityScale)
            .property("gravity_accel", &PGE_Phys_Object::gravityAccel, &PGE_Phys_Object::setGravityAccel);
}
