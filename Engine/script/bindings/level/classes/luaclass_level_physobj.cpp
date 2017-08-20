#include "luaclass_level_physobj.h"

#include <scenes/level/lvl_base_object.h>
#include <common_features/rectf.h>
#include <common_features/pointf.h>

/***
Level physical object base class
@module PhysBaseClass
*/

static luabind::class_<PGE_physBody> luaPhysBaseHead()
{
    using namespace luabind;
    return class_<PGE_physBody>("PhysBase")
        /***
        Collision types enumeration (CollisionType)
        @section CollisionType
        */
        .enum_("CollisionType")
        [
            /***
            Colliding is disabled
            @field PhysBase.COLLISION_NONE
            */
            value("COLLISION_NONE",   PGE_physBody::Block_NONE),
            /***
            Collide on any side
            @field PhysBase.COLLISION_ANY
            */
            value("COLLISION_ANY",    PGE_physBody::Block_ALL),
            /***
            Collide on top side
            @field PhysBase.COLLISION_TOP
            */
            value("COLLISION_TOP",    PGE_physBody::Block_TOP),
            /***
            Collide on bottom side
            @field PhysBase.COLLISION_BOTTOM
            */
            value("COLLISION_BOTTOM", PGE_physBody::Block_BOTTOM),
            /***
            Collide on left side
            @field PhysBase.COLLISION_LEFT
            */
            value("COLLISION_LEFT",   PGE_physBody::Block_LEFT),
            /***
            Collide on right side
            @field PhysBase.COLLISION_RIGHT
            */
            value("COLLISION_RIGHT",  PGE_physBody::Block_RIGHT)
        ];
}

static luabind::class_<PGE_Phys_Object, PGE_physBody> luaPhysHead()
{
    using namespace luabind;
    return class_<PGE_Phys_Object, PGE_physBody>("PhysBase")
        /***
        Item types enumeration (ObjTypes)
        @section ObjTypes
        */
        .enum_("ObjTypes")
        [
            /***
            Unknown type of object
            @field PhysBase.Unknown
            */
            value("Unknown", PGE_Phys_Object::LVLUnknown),
            /***
            Block object
            @field PhysBase.Block
            */
            value("Block", PGE_Phys_Object::LVLBlock),
            /***
            BGO object
            @field PhysBase.BGO
            */
            value("BGO", PGE_Phys_Object::LVLBGO),
            /***
            NPC object
            @field PhysBase.NPC
            */
            value("NPC", PGE_Phys_Object::LVLNPC),
            /***
            Playable character object
            @field PhysBase.Player
            */
            value("Player", PGE_Phys_Object::LVLPlayer),
            /***
            Warp entrance object
            @field PhysBase.Warp
            */
            value("Warp", PGE_Phys_Object::LVLWarp),
            /***
            Object of special type, reserved
            @field PhysBase.Special
            */
            value("Special", PGE_Phys_Object::LVLSpecial),
            /***
            Physical environment object
            @field PhysBase.PhysEnv
            */
            value("PhysEnv", PGE_Phys_Object::LVLPhysEnv)
        ];
}

luabind::scope Binding_Level_Class_PhysObj::bindBaseToLua()
{
    using namespace luabind;
    return luaPhysBaseHead()
           .def_readonly("blockedAtLeft",  &PGE_Phys_Object::m_blockedAtLeft)
           .def_readonly("blockedAtRight", &PGE_Phys_Object::m_blockedAtRight);
}

luabind::scope Binding_Level_Class_PhysObj::bindToLua()
{
    using namespace luabind;
    /***
    Level generic physical body object
    @type PhysBase
    */
    return luaPhysHead()
            //Size and position (absolute or relative for stickies)

            /***
            X position on the scene (left-top corner)
            @tfield double x
            */
            .property("x", &PGE_Phys_Object::luaPosX, &PGE_Phys_Object::luaSetPosX)
            /***
            Y position on the scene (left-top corner)
            @tfield double y
            */
            .property("y", &PGE_Phys_Object::luaPosY, &PGE_Phys_Object::luaSetPosY)
            /***
            X position on the scene at center
            @tfield double center_x
            */
            .property("center_x", &PGE_Phys_Object::luaPosCenterX, &PGE_Phys_Object::luaSetCenterX)
            /***
            Y position on the scene at center
            @tfield double center_y
            */
            .property("center_y", &PGE_Phys_Object::luaPosCenterY, &PGE_Phys_Object::luaSetCenterY)
            /***
            Width of body
            @tfield double width
            */
            .property("width", &PGE_Phys_Object::luaWidth, &PGE_Phys_Object::luaSetWidth)
            /***
            Height of body
            @tfield double height
            */
            .property("height", &PGE_Phys_Object::luaHeight, &PGE_Phys_Object::luaSetHeight)
            /***
            Top side position
            @tfield double top
            */
            .property("top", &PGE_Phys_Object::luaTop, &PGE_Phys_Object::luaSetTop)
            /***
            Left side position
            @tfield double left
            */
            .property("left", &PGE_Phys_Object::luaLeft, &PGE_Phys_Object::luaSetLeft)
            /***
            Right side position
            @tfield double right
            */
            .property("right", &PGE_Phys_Object::luaRight, &PGE_Phys_Object::luaSetRight)
            /***
            Bottom side position
            @tfield double bottom
            */
            .property("bottom", &PGE_Phys_Object::luaBottom, &PGE_Phys_Object::luaSetBottom)

            //Size and position (absolute positions even per stickies)

            /***
            Absolute X position (when object is a member of group and has relative coordinates)
            @tfield double abs_x
            */
            .property("abs_x", &PGE_Phys_Object::posX, &PGE_Phys_Object::setPosX)
            /***
            Absolute Y position (when object is a member of group and has relative coordinates)
            @tfield double abs_y
            */
            .property("abs_y", &PGE_Phys_Object::posY, &PGE_Phys_Object::setPosY)
            /***
            Absolute center X position (when object is a member of group and has relative coordinates)
            @tfield double abs_center_x
            */
            .property("abs_center_x", &PGE_Phys_Object::posCenterX, &PGE_Phys_Object::setCenterX)
            /***
            Absolute center Y position (when object is a member of group and has relative coordinates)
            @tfield double abs_center_y
            */
            .property("abs_center_y", &PGE_Phys_Object::posCenterY, &PGE_Phys_Object::setCenterY)
            /***
            Absolute width of body (when object is a member of group with a scale transform)
            @tfield double abs_width
            */
            .property("abs_width", &PGE_Phys_Object::width, &PGE_Phys_Object::setWidth)
            /***
            Absolute height of body (when object is a member of group with a scale transform)
            @tfield double abs_height
            */
            .property("abs_height", &PGE_Phys_Object::height, &PGE_Phys_Object::setHeight)
            /***
            Absolute top position (when object is a member of group and has relative coordinates)
            @tfield double abs_top
            */
            .property("abs_top", &PGE_Phys_Object::top, &PGE_Phys_Object::setTop)
            /***
            Absolute left position (when object is a member of group and has relative coordinates)
            @tfield double abs_left
            */
            .property("abs_left", &PGE_Phys_Object::left, &PGE_Phys_Object::setLeft)
            /***
            Absolute right position (when object is a member of group and has relative coordinates)
            @tfield double abs_right
            */
            .property("abs_right", &PGE_Phys_Object::right, &PGE_Phys_Object::setRight)
            /***
            Absolute bottom position (when object is a member of group and has relative coordinates)
            @tfield double abs_bottom
            */
            .property("abs_bottom", &PGE_Phys_Object::bottom, &PGE_Phys_Object::setBottom)

            /***
            Type of object
            @tfield ObjTypes type
            */
            .def_readonly("type", &PGE_Phys_Object::type)

            //Physics

            /***
            Pause physical processing, object will don't interact with world and will be frozen
            @tfield bool paused_physics
            */
            .property("paused_physics", &PGE_Phys_Object::isPaused, &PGE_Phys_Object::setPaused)

            /***
            Object is visible and will be drawn on player's camera
            @tfield bool visible
            */
            .property("visible", &PGE_Phys_Object::isVisible, &PGE_Phys_Object::setVisible)

            /***
            Moving X velocity in pixels per 1/65 of second units
            @tfield double speedX
            */
            .property("speedX", &PGE_Phys_Object::speedX, &PGE_Phys_Object::setSpeedX)
            /***
            Moving Y velocity in pixels per 1/65 of second units
            @tfield double speedY
            */
            .property("speedY", &PGE_Phys_Object::speedY, &PGE_Phys_Object::setSpeedY)

            /***
            Minimal horizontal moving velocity in pixels per 1/65 of second units
            @tfield double minVelX
            */
            .property("minVelX", &PGE_Phys_Object::minVelX, &PGE_Phys_Object::setMinVelX)

            /***
            Minimal vertical moving velocity in pixels per 1/65 of second units
            @tfield double minVelY
            */
            .property("minVelY", &PGE_Phys_Object::minVelY, &PGE_Phys_Object::setMinVelY)

            /***
            Maximal horizontal moving velocity in pixels per 1/65 of second units
            @tfield double maxVelX
            */
            .property("maxVelX", &PGE_Phys_Object::maxVelX, &PGE_Phys_Object::setMaxVelX)

            /***
            Maximal vertical moving velocity in pixels per 1/65 of second units
            @tfield double maxVelY
            */
            .property("maxVelY", &PGE_Phys_Object::maxVelY, &PGE_Phys_Object::setMaxVelY)

            /***
            Gravity scale multiplier to global gravity force.
            @tfield double gravity 1.0 is same as global, 0.0 is no gravity, any negative value will invert gravity
            */
            .property("gravity", &PGE_Phys_Object::gravityScale, &PGE_Phys_Object::setGravityScale)

            /***
            Gravity acceleration
            @tfield double gravity_accel 1.0 is same as global, 0.0 is no gravity, any negative value will invert gravity
            */
            .property("gravity_accel", &PGE_Phys_Object::gravityAccel, &PGE_Phys_Object::setGravityAccel)

            /***
            Allow player collide with this NPC by the way
            @tfield PhysBase.CollisionType collide_player
            */
            .property("collide_player", &PGE_Phys_Object::collidePlayer, &PGE_Phys_Object::setCollidePlayer)

            /***
            Allow another NPCs collide with this NPC by the way
            @tfield PhysBase.CollisionType collide_npc
            */
            .property("collide_npc", &PGE_Phys_Object::collideNpc, &PGE_Phys_Object::setCollideNpc)

            /***
            Apply acceneration force on this frame
            To have speed acceleration of object you must call this function every frame (for example in <b>onLoop()</b> event callback of NPC or playable character)<br>
            Unit - one pixel per 1/65 of second in 1 / 65 of second. I.e. In one 1/65 of second interval speed will change one pixel per 1/65 of second
            @function applyAccel
            @tparam double x Horizontal acceleration (<0 to left, >0 to right)
            @tparam double y Vertical acceleration (<0 to up, >0 to down)
            */
            .def("applyAccel", &PGE_Phys_Object::applyAccel)

            /***
            Set a horizontal movement deceleration when is no acceleration applying<br>
            Unit - one pixel per 1/65 of second in 1 / 65 of second. I.e. In one 1/65 of second interval speed will change one pixel per 1/65 of second
            @function setDecelX
            @tparam double x Horzontal deceleration
            */
            .def("setDecelX", &PGE_Phys_Object::setDecelX)

            /***
            Set a vertical movement deceleration when is no acceleration or gravity applying<br>
            Unit - one pixel per 1/65 of second in 1 / 65 of second. I.e. In one 1/65 of second interval speed will change one pixel per 1/65 of second
            @function setDecelY
            @tparam double y Horzontal deceleration
            */
            .def("setDecelY", &PGE_Phys_Object::setDecelY)
            ;
}
