#include "luaclass_level_physobj.h"

#include <scenes/level/lvl_base_object.h>
#include <common_features/rectf.h>
#include <common_features/pointf.h>

luabind::scope Binding_Level_Class_PhysObj::bindBaseToLua()
{
    using namespace luabind;
    return
        class_<PGE_physBody>("PhysBase")
            .enum_("CollisionType")
            [
                value("COLLISION_NONE",   PGE_physBody::Block_NONE),
                value("COLLISION_ANY",    PGE_physBody::Block_ALL),
                value("COLLISION_TOP",    PGE_physBody::Block_TOP),
                value("COLLISION_BOTTOM", PGE_physBody::Block_BOTTOM),
                value("COLLISION_LEFT",   PGE_physBody::Block_LEFT),
                value("COLLISION_RIGHT",  PGE_physBody::Block_RIGHT)
            ]
            .def_readonly("blockedAtLeft",  &PGE_Phys_Object::m_blockedAtLeft)
            .def_readonly("blockedAtRight", &PGE_Phys_Object::m_blockedAtRight);
}

luabind::scope Binding_Level_Class_PhysObj::bindToLua()
{
    using namespace luabind;
    return
        class_<PGE_Phys_Object, PGE_physBody>("PhysBase")
            .enum_("ObjTypes")
            [
                value("Unknown", PGE_Phys_Object::LVLUnknown),
                value("Block", PGE_Phys_Object::LVLBlock),
                value("BGO", PGE_Phys_Object::LVLBGO),
                value("NPC", PGE_Phys_Object::LVLNPC),
                value("Player", PGE_Phys_Object::LVLPlayer),
                value("Warp", PGE_Phys_Object::LVLWarp),
                value("Special", PGE_Phys_Object::LVLSpecial),
                value("PhysEnv", PGE_Phys_Object::LVLPhysEnv)
            ]
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
            .def_readonly("type", &PGE_Phys_Object::type)

            //Physics
            .property("paused_physics", &PGE_Phys_Object::isPaused, &PGE_Phys_Object::setPaused)
            .property("visible", &PGE_Phys_Object::isVisible, &PGE_Phys_Object::setVisible)
            .property("speedX", &PGE_Phys_Object::speedX, &PGE_Phys_Object::setSpeedX)
            .property("speedY", &PGE_Phys_Object::speedY, &PGE_Phys_Object::setSpeedY)
            .property("minVelX", &PGE_Phys_Object::minVelX, &PGE_Phys_Object::setMinVelX)
            .property("minVelY", &PGE_Phys_Object::minVelY, &PGE_Phys_Object::setMinVelY)
            .property("maxVelX", &PGE_Phys_Object::maxVelX, &PGE_Phys_Object::setMaxVelX)
            .property("maxVelY", &PGE_Phys_Object::maxVelY, &PGE_Phys_Object::setMaxVelY)
            .property("gravity", &PGE_Phys_Object::gravityScale, &PGE_Phys_Object::setGravityScale)
            .property("gravity_accel", &PGE_Phys_Object::gravityAccel, &PGE_Phys_Object::setGravityAccel)
            .property("collide_player", &PGE_Phys_Object::collidePlayer, &PGE_Phys_Object::setCollidePlayer)
            .property("collide_npc", &PGE_Phys_Object::collideNpc, &PGE_Phys_Object::setCollideNpc)
            .def("applyAccel", &PGE_Phys_Object::applyAccel)
            .def("setDecelX", &PGE_Phys_Object::setDecelX)
            ;
}
