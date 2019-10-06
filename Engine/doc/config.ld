-- ldoc configuration file
title = "PGE Engine Lua API"
project = "PGE-Lua"
not_luadoc = true
wrap = false
merge = true
style = '!customfixed'

description = [[
    Documentation of hardcoded PGE Engine Lua API
]]

new_type("enum", "Enum", false, "Constants")

file =
{
    '../script/bindings/core/lua_global_constants.cpp',

    '../script/bindings/core/globalfuncs/luafuncs_core_effect.cpp',
    '../script/bindings/core/classes/luaclass_core_scene_effects.cpp',

    '../script/bindings/core/globalfuncs/luafuncs_core_audio.cpp',

    '../script/bindings/core/globalfuncs/luafuncs_core_paths.cpp',

    '../script/bindings/core/classes/luaclass_core_graphics.cpp',

    '../script/bindings/core/globalfuncs/luafuncs_core_logger.cpp',

    '../script/bindings/core/globalfuncs/luafuncs_core_renderer.cpp',

    '../script/bindings/core/globalfuncs/luafuncs_core_settings.cpp',

    '../script/bindings/core/classes/luaclass_core_simpleevent.cpp',

    '../script/bindings/core/classes/luaclass_core_data.cpp',

    '../script/bindings/core/events/luaevents_core_engine.cpp',

    '../script/bindings/level/globalfuncs/luafuncs_level.cpp',
    '../script/bindings/level/globalfuncs/luafuncs_level_bgo.cpp',
    '../script/bindings/level/globalfuncs/luafuncs_level_blocks.cpp',
    '../script/bindings/level/globalfuncs/luafuncs_level_lvl_npc.cpp',
    '../script/bindings/level/globalfuncs/luafuncs_level_lvl_player.cpp',
    '../script/bindings/level/classes/luaclass_level_camera.cpp',
    '../script/bindings/level/classes/luaclass_level_playerstate.cpp',

    '../script/bindings/level/classes/luaclass_level_physobj.cpp',

    '../script/bindings/level/classes/luaclass_lvl_block.cpp',

    '../script/bindings/level/classes/luaclass_lvl_bgo.cpp',

    '../script/bindings/level/classes/luaclass_level_lvl_npc.cpp',
    '../script/bindings/level/classes/luaclass_level_contact_detector.cpp',
    '../script/bindings/level/classes/luaclass_level_inareadetector.cpp',
    '../script/bindings/level/classes/luaclass_level_playerposdetector.cpp',

    '../script/bindings/level/classes/luaclass_level_lvl_player.cpp',
}

