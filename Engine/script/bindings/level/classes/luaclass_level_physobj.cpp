#include "luaclass_level_physobj.h"

#include <scenes/level/lvl_base_object.h>

luabind::scope Binding_Level_Class_PhysObj::bindToLua()
{
    using namespace luabind;
    return
        class_<PGE_Phys_Object>("PhysBase")
            .property("x", &PGE_Phys_Object::posX, &PGE_Phys_Object::setPosX)
            .property("y", &PGE_Phys_Object::posY, &PGE_Phys_Object::setPosY)
            .property("speedX", &PGE_Phys_Object::speedX, &PGE_Phys_Object::setSpeedX)
            .property("speedY", &PGE_Phys_Object::speedY, &PGE_Phys_Object::setSpeedY)
            .property("gravity", &PGE_Phys_Object::gravityScale, &PGE_Phys_Object::setGravityScale);
}
