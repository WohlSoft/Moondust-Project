#include "luaclass_core_scene_effects.h"

#include <scenes/_base/gfx_effect.h>
#include <data_configs/spawn_effect_def.h>

luabind::scope Binding_Core_Scene_Effects::bindToLua()
{
    using namespace luabind;
    return
        class_<SpawnEffectDef>("EffectDef")
            .def(constructor<>())
            .def_readwrite("id", &SpawnEffectDef::id)
            .def_readwrite("startX", &SpawnEffectDef::startX)
            .def_readwrite("startY", &SpawnEffectDef::startY)
            .def_readwrite("startDelay", &SpawnEffectDef::start_delay)
            .def_readwrite("loopsCount", &SpawnEffectDef::animationLoops)
            .def_readwrite("maxDelay", &SpawnEffectDef::delay)
            .def_readwrite("frameSpeed", &SpawnEffectDef::framespeed)
            .def("setSequence", &SpawnEffectDef::lua_setSequence)
            .def_readwrite("velocityX", &SpawnEffectDef::velocityX)
            .def_readwrite("velocityY", &SpawnEffectDef::velocityY)
            .def_readwrite("gravity", &SpawnEffectDef::gravity)
            .def_readwrite("direction", &SpawnEffectDef::direction)
            .def_readwrite("minVelX", &SpawnEffectDef::min_vel_x)
            .def_readwrite("minVelY", &SpawnEffectDef::min_vel_y)
            .def_readwrite("maxVelX", &SpawnEffectDef::max_vel_x)
            .def_readwrite("maxVelY", &SpawnEffectDef::max_vel_y)
            .def_readwrite("decelerateX", &SpawnEffectDef::decelerate_x)
            .def_readwrite("decelerateY", &SpawnEffectDef::decelerate_y)
            ;
}
