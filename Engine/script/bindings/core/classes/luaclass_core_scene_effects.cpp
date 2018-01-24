#include "luaclass_core_scene_effects.h"

#include <scenes/_base/gfx_effect.h>
#include <data_configs/spawn_effect_def.h>

/***
Animated graphical effects
@module AnimatedEffects
*/

luabind::scope Binding_Core_Scene_Effects::bindToLua()
{
    using namespace luabind;
    return
        /***
        Animation effect definition
        @type EffectDef
        */
        class_<SpawnEffectDef>("EffectDef")
            .def(constructor<>())
            /***
            <b>[Required]</b> ID of effect entry in a config pack
            @tfield ulong id
            */
            .def_readwrite("id", &SpawnEffectDef::id)
            /***
            <b>[Required]</b> Initial scene X position of effect
            @tfield double startX
            */
            .def_readwrite("startX", &SpawnEffectDef::startX)
            /***
            <b>[Required]</b> Initial scene Y position of effect
            @tfield double startY
            */
            .def_readwrite("startY", &SpawnEffectDef::startY)
            /***
            Start delay in milliseconds. How long to wait to show and begin the animation
            @tfield uint startDelay Wait until animation will be shown. Zero value means instant start of animation.
            */
            .def_readwrite("startDelay", &SpawnEffectDef::start_delay)
            /***
            Limit effect be shown while animation will loop specified times.
            @tfield int loopsCount If value is greater than 1, animation will disappear after specified animation sequence loops. Equal or less than 0 disable this limitation.
            */
            .def_readwrite("loopsCount", &SpawnEffectDef::animationLoops)
            /***
            Timeout limit of effect be shown in milliseconds.
            @tfield int maxDelay If value is greater than 1, animation will disappear after specified animation sequence loops. Equal or less than 0 disable this limitation.
            */
            .def_readwrite("maxDelay", &SpawnEffectDef::delay)
            /***
            Animation frame delay in milliseconds
            @tfield int frameDelay
            */
            .def_readwrite("frameDelay", &SpawnEffectDef::frameDelay)
            /***
            Animation frame delay in milliseconds, Alias to @{frameDelay}
            @tfield int frameSpeed
            */
            .def_readwrite("frameSpeed", &SpawnEffectDef::frameDelay)
            /***
            Set custom animation sequence of frames.
            @function setSequence
            @tparam table frames An array of integers are indeces of frames on frameset. First frame is zero. Frame indeces can be repeated and total sequence length is unlimited.
            */
            .def("setSequence", &SpawnEffectDef::lua_setSequence)
            /***
            Horizontal motion velocity
            @tfield double velocityX in pixels per ~1/65 of second
            */
            .def_readwrite("velocityX", &SpawnEffectDef::velocityX)
            /***
            Vertical motion velocity
            @tfield double velocityY in pixels per ~1/65 of second
            */
            .def_readwrite("velocityY", &SpawnEffectDef::velocityY)
            /***
            Rendering Z position (priority)
            @tfield double zIndex
            */
            .def_readwrite("zIndex", &SpawnEffectDef::zIndex)
            /***
            Rendering Z position (priority), Alias to @{zIndex}.
            @tfield double priority
            */
            .def_readwrite("priority", &SpawnEffectDef::zIndex)
            /***
            Gravity scale (A multiplier of global gravity force)
            @tfield double gravity Scale factor on global gravity value: 0.0 - gravity don't affects. 1.0 - same as environment gravity. -1.0 - inverted gravity.
            */
            .def_readwrite("gravity", &SpawnEffectDef::gravity)
            /***
            Face direction (for effects with frame-style equal to 2)
            @tfield int direction -1 - left, +1 - right
            */
            .def_readwrite("direction", &SpawnEffectDef::direction)
            /***
            Minimal horizontal velocity limit
            @tfield double minVelX
            */
            .def_readwrite("minVelX", &SpawnEffectDef::min_vel_x)
            /***
            Minimal vertical velocity limit
            @tfield double minVelY
            */
            .def_readwrite("minVelY", &SpawnEffectDef::min_vel_y)
            /***
            Maximal horizontal velocity limit
            @tfield double maxVelX
            */
            .def_readwrite("maxVelX", &SpawnEffectDef::max_vel_x)
            /***
            Maximal vertical velocity limit
            @tfield double maxVelY
            */
            .def_readwrite("maxVelY", &SpawnEffectDef::max_vel_y)
            /***
            Horizontal speed deceleration
            @tfield double decelerateX
            */
            .def_readwrite("decelerateX", &SpawnEffectDef::decelerate_x)
            /***
            Vertical speed deceleration
            @tfield double decelerateX
            */
            .def_readwrite("decelerateY", &SpawnEffectDef::decelerate_y)
            ;
}
