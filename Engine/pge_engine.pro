# PGE Engine, a free tool for playable srite design
# This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
# Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

#-------------------------------------------------
#
# Project created by QtCreator 2014-09-05T22:51:01
#
#-------------------------------------------------

#QT += core
#QT -= dbus declarative svg testlib opengl winextras gui network widgets
CONFIG -= qt

QMAKE_CXXFLAGS += -Wstrict-aliasing=0
!macx:!linux-ic*: QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
macx:  QMAKE_CXXFLAGS += -Wno-header-guard
!macx: QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'

!macx: QMAKE_CXXFLAGS += -ffloat-store

include($$PWD/../_common/strip_garbage.pri)
include($$PWD/../_common/dest_dir.pri)
include($$PWD/../_common/build_props.pri)
include($$PWD/languages.pri)

!macx: TARGET = pge_engine
macx:  TARGET = "PGE Engine"
TEMPLATE = app
CONFIG += c++11
CONFIG += thread
CONFIG += console
CONFIG += static

DEPENDPATH += "$$PWD/../_Libs/SDL2_mixer_modified/"

INCLUDEPATH += -$$PWD/../_Libs/SDL2_mixer_modified
INCLUDEPATH += "$$PWD/../_Libs/" "$$PWD/../_common"
INCLUDEPATH += "$$PWD/../_Libs/luabind"
INCLUDEPATH += "$$PWD/../_Libs/luabind/lua"
DEFINES += PGE_ENGINE USE_LUA_JIT

include ($$PWD/../_common/lib_destdir.pri)

android || macx: {
    DEFINES -= USE_LUA_JIT
}

INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include
INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include/freetype2/
contains(DEFINES, USE_LUA_JIT): INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include/luajit-2.0

LIBS += -L$$PWD/../_Libs/_builds/$$TARGETOS/lib
LIBS += -lluabind

android: {
    LIBS += -lSDL2 -lSDL2_mixer_ext -lfreeimagelite -lGLESv2 -lGLESv1_CM -ldl -landroid
    ANDROID_EXTRA_LIBS += $$PWD/../_Libs/_builds/android/lib/libSDL2.so \
                          $$PWD/../_Libs/_builds/android/lib/libSDL2_mixer_ext.so \
                          $$PWD/../_Libs/_builds/android/lib/libvorbisfile.so \
                          $$PWD/../_Libs/_builds/android/lib/libvorbis.so \
                          $$PWD/../_Libs/_builds/android/lib/libvorbisenc.so \
                          $$PWD/../_Libs/_builds/android/lib/libogg.so \
                          $$PWD/../_Libs/_builds/android/lib/libmad.so \
                          $$PWD/../_Libs/_builds/android/lib/libmodplug.so
}
win32: {
    RC_FILE = _resources/engine.rc
    LIBS += -lSDL2 -lSDL2_mixer_ext -lSDL2main -lfreeimagelite  -lfreetype
    LIBS += -lversion -lopengl32 -ldbghelp -ladvapi32 -lkernel32
}
macx: {
    ICON = _resources/cat.icns
    QMAKE_INFO_PLIST = $$PWD/_resources/pge_engine.plist
    APP_FILEICON_FILES.files = \
            $$PWD/../Editor/_resources/file_lvl.icns \
            $$PWD/../Editor/_resources/file_lvlx.icns \
            $$PWD/../Editor/_resources/file_wld.icns \
            $$PWD/../Editor/_resources/file_wldx.icns
    APP_FILEICON_FILES.path  = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_FILEICON_FILES
    LIBS += -lSDL2 -lSDL2_mixer_ext -lvorbis -lvorbisfile -lFLAC -logg -lmad
    LIBS += -lfreeimagelite -lfreetype
    LIBS += -framework CoreAudio -framework CoreVideo -framework Cocoa \
            -framework IOKit -framework CoreFoundation -framework Carbon \
            -framework ForceFeedback -framework AudioToolbox \
            -framework OpenGL
}
linux-g++||unix:!macx:!android: {
    LIBS += -L$$PWD/../_Libs/_builds/$$TARGETOS/lib64
    LIBS += -lSDL2_mixer_ext -lfreeimagelite -lfreetype -lSDL2
    LIBS += -lGL #-lglut -Wl,-Bstatic -lGLEW -Wl,-Bdynamic
}

contains(DEFINES, USE_LUA_JIT): {
    macx: {
        LIBS += -lluajit
    } else {
        LIBS += -lluajit-5.1
        linux-g++: LIBS += -ldl
    }
}
unix:{
    CONFIG(debug, debug|release):||lessThan(QT_MINOR_VERSION, 3): LIBS += -ldl
}

include($$PWD/../_common/fmt/fmt.pri)
include($$PWD/../_common/tclap/tclap.pri)
include($$PWD/../_common/Utf8Main/utf8main.pri)
include($$PWD/../_common/DirManager/dirman.pri)
include($$PWD/../_common/FileMapper/FileMapper.pri)
# DEFINES += INI_PROCESSING_ALLOW_QT_TYPES
include($$PWD/../_common/IniProcessor/IniProcessor.pri)
include($$PWD/../_common/PGE_File_Formats/File_FormatsSTL.pri)
include($$PWD/../_common/ConfigPackManager/PGE_ConfigPackManager.pri)
include($$PWD/../_common/Utils/Utils.pri)
include($$PWD/../_common/Utils/UtilsSDL.pri)
macx:{
    include($$PWD/common_features/apple/apple_extras.pri)
}

SOURCES += \
    audio/pge_audio.cpp \
    audio/play_music.cpp \
    audio/play_sfx.cpp \
    common_features/app_path.cpp \
    common_features/crash_handler.cpp \
    common_features/episode_state.cpp \
    common_features/event_queue.cpp \
    common_features/fader.cpp \
    common_features/graphics_funcs.cpp \
    common_features/logger_sets.cpp \
    common_features/matrix_animator.cpp \
    common_features/md5.cpp \
    common_features/npc_animator.cpp \
    common_features/number_limiter.cpp \
    common_features/pge_texture.cpp \
    common_features/player_calibration.cpp \
    common_features/point.cpp \
    common_features/pointf.cpp \
    common_features/point_mover.cpp \
    common_features/QTranslatorX/ConvertUTF.c \
    common_features/QTranslatorX/qm_translator.cpp \
    common_features/rect.cpp \
    common_features/rectf.cpp \
    common_features/simple_animator.cpp \
    common_features/size.cpp \
    common_features/sizef.cpp \
    common_features/translator.cpp \
    common_features/util.cpp \
    common_features/version_cmp.cpp \
    controls/control_keys.cpp \
    controls/controllable_object.cpp \
    controls/controller.cpp \
    controls/controller_joystick.cpp \
    controls/controller_keyboard.cpp \
    data_configs/config_engine.cpp \
    data_configs/config_manager.cpp \
    data_configs/config_paths.cpp \
    data_configs/config_select_scene/scene_config_select.cpp \
    data_configs/config_textures.cpp \
    data_configs/custom_data.cpp \
    data_configs/obj_bg.cpp \
    data_configs/obj_bgo.cpp \
    data_configs/obj_block.cpp \
    data_configs/obj_effect.cpp \
    data_configs/obj_music.cpp \
    data_configs/obj_npc.cpp \
    data_configs/obj_player.cpp \
    data_configs/obj_sound.cpp \
    data_configs/obj_wld_items.cpp \
    data_configs/setup_credits_screen.cpp \
    data_configs/setup_load_screen.cpp \
    data_configs/setup_lvl_scene.cpp \
    data_configs/setup_title_screen.cpp \
    data_configs/setup_wld_scene.cpp \
    data_configs/spawn_effect_def.cpp \
    engine.cpp \
    fontman/font_manager.cpp \
    fontman/font_manager_private.cpp \
    fontman/raster_font.cpp \
    graphics/gl_color.cpp \
    graphics/gl_renderer.cpp \
    graphics/graphics.cpp \
    graphics/render/render_base.cpp \
    graphics/render/render_opengl21.cpp \
    graphics/render/render_opengl31.cpp \
    graphics/render/render_swsdl.cpp \
    graphics/window.cpp \
    gui/menu/pge_bool_menuitem.cpp \
    gui/menu/pge_int_menuitem.cpp \
    gui/menu/pge_int_named_menuitem.cpp \
    gui/menu/pge_keygrab_menuitem.cpp \
    gui/menu/_pge_menuitem.cpp \
    gui/pge_boxbase.cpp \
    gui/pge_menuboxbase.cpp \
    gui/pge_menubox.cpp \
    gui/pge_menu.cpp \
    gui/pge_msgbox.cpp \
    gui/pge_questionbox.cpp \
    gui/pge_textinputbox.cpp \
    main.cpp \
    networking/editor_pipe.cpp \
    networking/intproc.cpp \
    _resources/resource.cpp \
    scenes/_base/gfx_effect.cpp \
    scenes/_base/msgbox_queue.cpp \
    scenes/level/base/lvl_base_physics.cpp \
    scenes/level/lvl_backgrnd.cpp \
    scenes/level/lvl_base_object.cpp \
    scenes/level/lvl_bgo.cpp \
    scenes/level/lvl_block.cpp \
    scenes/level/lvl_camera.cpp \
    scenes/level/lvl_event_engine.cpp \
    scenes/level/lvl_layer_engine.cpp \
    scenes/level/lvl_npc.cpp \
    scenes/level/lvl_physenv.cpp \
    scenes/level/lvl_player.cpp \
    scenes/level/lvl_player_def.cpp \
    scenes/level/lvl_player_switch.cpp \
    scenes/level/lvl_scene_files_io.cpp \
    scenes/level/lvl_scene_garbage_collecting.cpp \
    scenes/level/lvl_scene_init.cpp \
    scenes/level/lvl_scene_interprocess.cpp \
    scenes/level/lvl_scene_items.cpp \
    scenes/level/lvl_scene_pausemenu.cpp \
    scenes/level/lvl_scene_rtree.cpp \
    scenes/level/lvl_scene_sections.cpp \
    scenes/level/lvl_scene_timers.cpp \
    scenes/level/lvl_section.cpp \
    scenes/level/lvl_warp.cpp \
    scenes/level/lvl_z_constants.cpp \
    scenes/level/npc_detectors/lvl_base_detector.cpp \
    scenes/level/npc_detectors/lvl_dtc_contact.cpp \
    scenes/level/npc_detectors/lvl_dtc_inarea.cpp \
    scenes/level/npc_detectors/lvl_dtc_player_pos.cpp \
    scenes/level/npc/lvl_npc_activate.cpp \
    scenes/level/npc/lvl_npc_generator.cpp \
    scenes/level/npc/lvl_npc_groups.cpp \
    scenes/level/npc/lvl_npc_kill.cpp \
    scenes/level/npc/lvl_npc_lua.cpp \
    scenes/level/npc/lvl_npc_physics.cpp \
    scenes/level/npc/lvl_npc_render.cpp \
    scenes/level/npc/lvl_npc_setup.cpp \
    scenes/level/npc/lvl_npc_spritewarp.cpp \
    scenes/level/npc/lvl_npc_throwned_by.cpp \
    scenes/level/npc/lvl_npc_update.cpp \
    scenes/level/player/lvl_player_animation.cpp \
    scenes/level/player/lvl_player_attaks_and_kills.cpp \
    scenes/level/player/lvl_player_physics.cpp \
    scenes/level/player/lvl_player_render.cpp \
    scenes/level/player/lvl_player_setup.cpp \
    scenes/level/player/lvl_player_update.cpp \
    scenes/level/player/lvl_player_warps_and_teleports.cpp \
    scenes/scene.cpp \
    scenes/scene_credits.cpp \
    scenes/scene_gameover.cpp \
    scenes/scene_level.cpp \
    scenes/scene_loading.cpp \
    scenes/scene_title.cpp \
    scenes/scene_world.cpp \
    scenes/title/ttl_main_menu.cpp \
    scenes/world/wld_pathopener.cpp \
    scenes/world/wld_player_portrait.cpp \
    scenes/world/wld_tilebox.cpp \
    script/bindings/core/classes/luaclass_core_graphics.cpp \
    script/bindings/core/classes/luaclass_core_scene_effects.cpp \
    script/bindings/core/classes/luaclass_core_simpleevent.cpp \
    script/bindings/core/events/luaevents_core_engine.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_audio.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_effect.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_logger.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_paths.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_renderer.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_settings.cpp \
    script/bindings/core/lua_global_constants.cpp \
    script/bindings/level/classes/luaclass_level_camera.cpp \
    script/bindings/level/classes/luaclass_level_contact_detector.cpp \
    script/bindings/level/classes/luaclass_level_inareadetector.cpp \
    script/bindings/level/classes/luaclass_level_lvl_npc.cpp \
    script/bindings/level/classes/luaclass_level_lvl_player.cpp \
    script/bindings/level/classes/luaclass_level_physobj.cpp \
    script/bindings/level/classes/luaclass_level_playerposdetector.cpp \
    script/bindings/level/classes/luaclass_level_playerstate.cpp \
    script/bindings/level/classes/luaclass_lvl_bgo.cpp \
    script/bindings/level/classes/luaclass_lvl_block.cpp \
    script/bindings/level/globalfuncs/luafuncs_level_bgo.cpp \
    script/bindings/level/globalfuncs/luafuncs_level_blocks.cpp \
    script/bindings/level/globalfuncs/luafuncs_level.cpp \
    script/bindings/level/globalfuncs/luafuncs_level_lvl_npc.cpp \
    script/bindings/level/globalfuncs/luafuncs_level_lvl_player.cpp \
    script/lua_credits_engine.cpp \
    script/lua_engine.cpp \
    script/lua_event.cpp \
    script/lua_global.cpp \
    script/lua_level_engine.cpp \
    script/lua_titlescreen_engine.cpp \
    script/lua_world_engine.cpp \
    settings/debugger.cpp \
    settings/global_settings.cpp \
    fontman/utf8_helpers.cpp \
    fontman/ttf_font.cpp


HEADERS  += \
    audio/pge_audio.h \
    audio/play_music.h \
    audio/play_sfx.h \
    common_features/app_path.h \
    common_features/crash_handler.h \
    common_features/data_array.h \
    common_features/episode_state.h \
    common_features/event_queue.h \
    common_features/fader.h \
    common_features/gif-h/gif.h \
    common_features/graphics_funcs.h \
    common_features/logger.h \
    common_features/logger_sets.h \
    common_features/matrix_animator.h \
    common_features/md5.h \
    common_features/npc_animator.h \
    common_features/number_limiter.h \
    common_features/pge_texture.h \
    common_features/player_calibration.h \
    common_features/pointf.h \
    common_features/point.h \
    common_features/point_mover.h \
    common_features/QTranslatorX/ConvertUTF.h \
    common_features/QTranslatorX/qm_translator.h \
    common_features/rectf.h \
    common_features/rect.h \
    common_features/RTree/RTree.h \
    common_features/simple_animator.h \
    common_features/sizef.h \
    common_features/size.h \
    common_features/translator.h \
    common_features/tr.h \
    common_features/util.h \
    common_features/version_cmp.h \
    controls/control_keys.h \
    controls/controllable_object.h \
    controls/controller.h \
    controls/controller_joystick.h \
    controls/controller_keyboard.h \
    controls/controller_key_map.h \
    data_configs/config_manager.h \
    data_configs/config_manager_private.h \
    data_configs/config_select_scene/scene_config_select.h \
    data_configs/custom_data.h \
    data_configs/obj_bg.h \
    data_configs/obj_bgo.h \
    data_configs/obj_block.h \
    data_configs/obj_effect.h \
    data_configs/obj_music.h \
    data_configs/obj_npc.h \
    data_configs/obj_player.h \
    data_configs/obj_sound.h \
    data_configs/obj_sound_roles.h \
    data_configs/obj_wld_items.h \
    data_configs/setup_credits_screen.h \
    data_configs/setup_load_screen.h \
    data_configs/setup_lvl_scene.h \
    data_configs/setup_title_screen.h \
    data_configs/setup_wld_scene.h \
    data_configs/spawn_effect_def.h \
    engine.hpp \
    fontman/font_manager.h \
    fontman/font_manager_private.h \
    fontman/raster_font.h \
    graphics/gl_color.h \
    graphics/gl_debug.h \
    graphics/gl_renderer.h \
    graphics/graphics.h \
    graphics/render/render_base.h \
    graphics/render/render_opengl21.h \
    graphics/render/render_opengl31.h \
    graphics/render/render_swsdl.h \
    graphics/window.h \
    gui/menu/pge_bool_menuitem.h \
    gui/menu/pge_int_menuitem.h \
    gui/menu/pge_int_named_menuitem.h \
    gui/menu/pge_keygrab_menuitem.h \
    gui/menu/_pge_menuitem.h \
    gui/pge_boxbase.h \
    gui/pge_menuboxbase.h \
    gui/pge_menubox.h \
    gui/pge_menu.h \
    gui/pge_msgbox.h \
    gui/pge_questionbox.h \
    gui/pge_textinputbox.h \
    networking/editor_pipe.h \
    networking/intproc.h \
    _resources/resource_data.h \
    _resources/resource.h \
    scenes/_base/gfx_effect.h \
    scenes/_base/msgbox_queue.h \
    scenes/level/base/lvl_base_physics.h \
    scenes/level/lvl_backgrnd.h \
    scenes/level/lvl_base_object.h \
    scenes/level/lvl_bgo.h \
    scenes/level/lvl_block.h \
    scenes/level/lvl_camera.h \
    scenes/level/lvl_event_engine.h \
    scenes/level/lvl_layer_engine.h \
    scenes/level/lvl_npc.h \
    scenes/level/lvl_physenv.h \
    scenes/level/lvl_player_def.h \
    scenes/level/lvl_player.h \
    scenes/level/lvl_player_switch.h \
    scenes/level/lvl_section.h \
    scenes/level/lvl_warp.h \
    scenes/level/lvl_z_constants.h \
    scenes/level/npc_detectors/lvl_base_detector.h \
    scenes/level/npc_detectors/lvl_dtc_contact.h \
    scenes/level/npc_detectors/lvl_dtc_inarea.h \
    scenes/level/npc_detectors/lvl_dtc_player_pos.h \
    scenes/scene_credits.h \
    scenes/scene_gameover.h \
    scenes/scene.h \
    scenes/scene_level.h \
    scenes/scene_loading.h \
    scenes/scene_title.h \
    scenes/scene_world.h \
    scenes/world/wld_pathopener.h \
    scenes/world/wld_player_portrait.h \
    scenes/world/wld_tilebox.h \
    script/bindings/core/classes/luaclass_core_graphics.h \
    script/bindings/core/classes/luaclass_core_scene_effects.h \
    script/bindings/core/classes/luaclass_core_simpleevent.h \
    script/bindings/core/events/luaevents_core_engine.h \
    script/bindings/core/globalfuncs/luafuncs_core_audio.h \
    script/bindings/core/globalfuncs/luafuncs_core_effect.h \
    script/bindings/core/globalfuncs/luafuncs_core_logger.h \
    script/bindings/core/globalfuncs/luafuncs_core_paths.h \
    script/bindings/core/globalfuncs/luafuncs_core_renderer.h \
    script/bindings/core/globalfuncs/luafuncs_core_settings.h \
    script/bindings/core/lua_global_constants.h \
    script/bindings/level/classes/luaclass_level_contact_detector.h \
    script/bindings/level/classes/luaclass_level_inareadetector.h \
    script/bindings/level/classes/luaclass_level_lvl_npc.h \
    script/bindings/level/classes/luaclass_level_lvl_player.h \
    script/bindings/level/classes/luaclass_level_physobj.h \
    script/bindings/level/classes/luaclass_level_playerposdetector.h \
    script/bindings/level/classes/luaclass_level_playerstate.h \
    script/bindings/level/globalfuncs/luafuncs_level_bgo.h \
    script/bindings/level/globalfuncs/luafuncs_level_blocks.h \
    script/bindings/level/globalfuncs/luafuncs_level.h \
    script/bindings/level/globalfuncs/luafuncs_level_lvl_npc.h \
    script/bindings/level/globalfuncs/luafuncs_level_lvl_player.h \
    script/lua_credits_engine.h \
    script/lua_defines.h \
    script/lua_engine.h \
    script/lua_engine.h \
    script/lua_event.h \
    script/lua_global.h \
    script/lua_level_engine.h \
    script/lua_titlescreen_engine.h \
    script/lua_utils.h \
    script/lua_world_engine.h \
    settings/debugger.h \
    settings/global_settings.h \
    version.h \
    fontman/font_engine_base.h \
    fontman/ttf_font.h

DISTFILES += \
    ../changelog.engine.txt \
    unused_thungs.txt \
    script/scrapped_code.txt \
    script/binding-tree.txt
