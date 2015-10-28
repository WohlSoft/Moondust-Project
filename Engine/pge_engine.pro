# PGE Engine, a free tool for playable srite design
# This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
# Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

QT += core gui opengl network
#QT += widgets

QMAKE_CXXFLAGS += -Wstrict-aliasing=0
!macx: QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
macx: QMAKE_CXXFLAGS += -Wno-header-guard
!macx: QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'

include(../_common/dest_dir.pri)
include(../_common/build_props.pri)

TARGET = pge_engine
TEMPLATE = app
CONFIG += c++11
CONFIG += thread

CONFIG += static

DEPENDPATH += "$$PWD/../_Libs/oolua/project"

INCLUDEPATH += -$$PWD/../_Libs/SDL2_mixer_modified
INCLUDEPATH += "$$PWD/../_Libs/" "$$PWD/../_common"
INCLUDEPATH += "$$PWD/../_Libs/luabind"
INCLUDEPATH += "$$PWD/../_Libs/luabind/lua"
LIBS += -L$$PWD/../_Libs/_builds/commonlibs
DEFINES += PGE_ENGINE USE_LUA_JIT PGE_FILES_QT


include ($$PWD/../_common/lib_destdir.pri)

INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include
contains(DEFINES, USE_LUA_JIT): INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include/liajit-2.0

LIBS += -L$$PWD/../_Libs/_builds/$$TARGETOS/lib
LIBS += -lluabind
android {
 DEFINES -= USE_LUA_JIT
}

android: {
    LIBS += -lSDL2 -lglut -lGLU
}
win32: {
    LIBS += -lSDL2 -lSDL2_mixer_ext -lSDL2main libversion -lopengl32 -lglu32
}
macx: {
    INCLUDEPATH += $$PWD/../_Libs/_builds/macos/frameworks/SDL2.framework/Headers
    LIBS += -F$$PWD/../_Libs/_builds/macos/frameworks -framework SDL2 -lSDL2_mixer_ext
    QMAKE_POST_LINK = $$PWD/mac_deploy_libs.sh
}
linux-g++||unix:!macx:!android: {
    LIBS += -lSDL2 -lSDL2_mixer_ext -lglut -lGLU
}

contains(DEFINES, USE_LUA_JIT): {
    macx: {
        LIBS += -lluajit
    } else {
        LIBS += -lluajit-5.1
    }
}
unix:{
contains(CONFIG, debug)||lessThan(QT_MINOR_VERSION, 3): LIBS += -ldl
}

macx {
    ICON = _resources/cat.icns
}

RC_FILE = _resources/engine.rc

SOURCES += \
    main.cpp \
    graphics/graphics.cpp \
    scenes/scene_level.cpp \
    scenes/scene.cpp \
    scenes/scene_world.cpp \
    scenes/scene_credits.cpp \
    scenes/scene_gameover.cpp \
    graphics/gl_renderer.cpp \
    graphics/window.cpp \
    controls/controllable_object.cpp \
    controls/controller.cpp \
    scenes/level/lvl_player.cpp \
    controls/control_keys.cpp \
    data_configs/config_manager.cpp \
    common_features/app_path.cpp \
    common_features/graphics_funcs.cpp \
    ../_Libs/EasyBMP/EasyBMP.cpp \
    data_configs/obj_block.cpp \
    controls/controller_keyboard.cpp \
    data_configs/select_config.cpp \
    common_features/util.cpp \
    scenes/level/lvl_block.cpp \
    data_configs/custom_data.cpp \
    data_configs/config_paths.cpp \
    common_features/simple_animator.cpp \
    data_configs/config_textures.cpp \
    data_configs/obj_bgo.cpp \
    scenes/level/lvl_bgo.cpp \
    data_configs/obj_bg.cpp \
    scenes/level/lvl_warp.cpp \
    scenes/level/lvl_scene_ptr.cpp \
    scenes/level/lvl_scene_items.cpp \
    scenes/level/lvl_scene_sections.cpp \
    scenes/level/lvl_scene_timers.cpp \
    scenes/level/lvl_scene_files_io.cpp \
    scenes/level/lvl_scene_init.cpp \
    gui/pge_msgbox.cpp \
    gui/pge_boxbase.cpp \
    fontman/font_manager.cpp \
    networking/editor_pipe.cpp \
    networking/intproc.cpp \
    common_features/logger.cpp \
    gui/pge_menu.cpp \
    scenes/scene_loading.cpp \
    scenes/scene_title.cpp \
    ../_common/PGE_File_Formats/file_formats.cpp \
    ../_common/PGE_File_Formats/file_rw_lvl.cpp \
    ../_common/PGE_File_Formats/file_rw_lvlx.cpp \
    ../_common/PGE_File_Formats/file_rw_meta.cpp \
    ../_common/PGE_File_Formats/file_rw_npc_txt.cpp \
    ../_common/PGE_File_Formats/file_rw_sav.cpp \
    ../_common/PGE_File_Formats/file_rw_wld.cpp \
    ../_common/PGE_File_Formats/file_rw_wldx.cpp \
    ../_common/PGE_File_Formats/file_rw_smbx64_cnf.cpp \
    ../_common/PGE_File_Formats/file_rwopen.cpp \
    ../_common/PGE_File_Formats/file_strlist.cpp \
    ../_common/PGE_File_Formats/lvl_filedata.cpp \
    ../_common/PGE_File_Formats/npc_filedata.cpp \
    ../_common/PGE_File_Formats/pge_x.cpp \
    ../_common/PGE_File_Formats/save_filedata.cpp \
    ../_common/PGE_File_Formats/smbx64.cpp \
    ../_common/PGE_File_Formats/smbx64_cnf_filedata.cpp \
    ../_common/PGE_File_Formats/wld_filedata.cpp \
    ../_common/PGE_File_Formats/pge_file_lib_globs.cpp \
    scenes/level/lvl_physenv.cpp \
    scenes/level/lvl_player_def.cpp \
    data_configs/obj_player.cpp \
    scenes/level/lvl_z_constants.cpp \
    data_configs/obj_effect.cpp \
    common_features/episode_state.cpp \
    common_features/event_queue.cpp \
    common_features/matrix_animator.cpp \
    scenes/level/lvl_backgrnd.cpp \
    scenes/level/lvl_camera.cpp \
    common_features/player_calibration.cpp \
    audio/SdlMusPlayer.cpp \
    data_configs/config_engine.cpp \
    data_configs/setup_wld_scene.cpp \
    data_configs/setup_load_screen.cpp \
    data_configs/setup_title_screen.cpp \
    data_configs/obj_music.cpp \
    data_configs/obj_sound.cpp \
    audio/pge_audio.cpp \
    settings/global_settings.cpp \
    common_features/maths.cpp \
    scenes/level/lvl_npc.cpp \
    controls/controller_joystick.cpp \
    scenes/world/wld_player_portrait.cpp \
    graphics/gl_color.cpp \
    gui/menu/_pge_menuitem.cpp \
    gui/menu/pge_bool_menuitem.cpp \
    gui/menu/pge_int_menuitem.cpp \
    gui/menu/pge_int_named_menuitem.cpp \
    gui/menu/pge_keygrab_menuitem.cpp \
    data_configs/obj_wld_items.cpp \
    common_features/npc_animator.cpp \
    scenes/level/lvl_section.cpp \
    common_features/rect.cpp \
    common_features/rectf.cpp \
    common_features/pointf.cpp \
    common_features/point.cpp \
    common_features/sizef.cpp \
    common_features/size.cpp \
    common_features/fader.cpp \
    data_configs/setup_credits_screen.cpp \
    scenes/title/ttl_main_menu.cpp \
    scenes/world/wld_tilebox.cpp \
    scenes/level/lvl_scene_rtree.cpp \
    gui/pge_menubox.cpp \
    script/bindings/core/events/luaevents_core_engine.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_logger.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_renderer.cpp \
    ../_common/PGE_File_Formats/file_rw_savx.cpp \
    common_features/pge_texture.cpp \
    data_configs/obj_npc.cpp \
    script/lua_titlescreen_engine.cpp \
    script/lua_global.cpp \
    script/lua_event.cpp \
    script/lua_engine.cpp \
    script/lua_level_engine.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_settings.cpp \
    common_features/number_limiter.cpp \
    data_configs/setup_lvl_scene.cpp \
    script/lua_world_engine.cpp \
    script/lua_credits_engine.cpp \
    scenes/level/lvl_base_object.cpp \
    script/bindings/level/classes/luaclass_level_lvl_player.cpp \
    script/bindings/level/classes/luaclass_level_physobj.cpp \
    script/bindings/level/classes/luaclass_level_lvl_npc.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_audio.cpp \
    scenes/_base/gfx_effect.cpp \
    script/bindings/core/globalfuncs/luafuncs_core_effect.cpp \
    script/bindings/level/globalfuncs/luafuncs_level_lvl_player.cpp \
    script/bindings/level/globalfuncs/luafuncs_level_lvl_npc.cpp \
    common_features/version_cmp.cpp \
    scenes/level/npc_detectors/lvl_base_detector.cpp \
    scenes/level/npc/lvl_npc_throwned_by.cpp \
    scenes/level/npc/lvl_npc_collisions.cpp \
    scenes/level/npc/lvl_npc_setup.cpp \
    scenes/level/npc/lvl_npc_lua.cpp \
    scenes/level/npc/lvl_npc_generator.cpp \
    scenes/level/npc/lvl_npc_kill.cpp \
    scenes/level/npc/lvl_npc_spritewarp.cpp \
    scenes/level/npc/lvl_npc_activate.cpp \
    scenes/level/npc/lvl_npc_update.cpp \
    scenes/level/npc/lvl_npc_render.cpp \
    scenes/level/player/lvl_player_collisions.cpp \
    scenes/level/player/lvl_player_setup.cpp \
    scenes/level/player/lvl_player_attaks_and_kills.cpp \
    scenes/level/player/lvl_player_animation.cpp \
    scenes/level/player/lvl_player_warps_and_teleports.cpp \
    scenes/level/player/lvl_player_update.cpp \
    scenes/level/player/lvl_player_physics.cpp \
    scenes/level/player/lvl_player_render.cpp \
    scenes/level/npc/lvl_npc_groups.cpp \
    scenes/level/lvl_layer_engine.cpp \
    scenes/level/lvl_event_engine.cpp \
    scenes/level/npc_detectors/lvl_dtc_player_pos.cpp \
    scenes/level/npc_detectors/lvl_dtc_inarea.cpp \
    script/bindings/level/classes/luaclass_level_inareadetector.cpp \
    script/bindings/level/classes/luaclass_level_playerposdetector.cpp \
    script/bindings/core/classes/luaclass_core_simpleevent.cpp \
    gui/pge_questionbox.cpp \
    gui/pge_textinputbox.cpp \
    gui/pge_menuboxbase.cpp \
    settings/debugger.cpp \
    ../_common/data_functions/npctxt_manager.cpp

HEADERS  += \
    graphics/graphics.h \
    scenes/scene_level.h \
    scenes/scene.h \
    scenes/scene_world.h \
    scenes/scene_credits.h \
    scenes/scene_gameover.h \
    graphics/gl_renderer.h \
    graphics/window.h \
    controls/controllable_object.h \
    controls/controller.h \
    scenes/level/lvl_player.h \
    controls/control_keys.h \
    data_configs/config_manager.h \
    data_configs/obj_block.h \
    common_features/app_path.h \
    common_features/graphics_funcs.h \
    common_features/pge_texture.h \
    ../_Libs/EasyBMP/EasyBMP.h \
    ../_Libs/EasyBMP/EasyBMP_BMP.h \
    ../_Libs/EasyBMP/EasyBMP_DataStructures.h \
    ../_Libs/EasyBMP/EasyBMP_VariousBMPutilities.h \
    controls/controller_keyboard.h \
    data_configs/select_config.h \
    common_features/util.h \
    scenes/level/lvl_block.h \
    data_configs/custom_data.h \
    common_features/simple_animator.h \
    data_configs/obj_bgo.h \
    scenes/level/lvl_bgo.h \
    data_configs/obj_bg.h \
    version.h \
    scenes/level/lvl_warp.h \
    scenes/level/lvl_scene_ptr.h \
    gui/pge_msgbox.h \
    gui/pge_boxbase.h \
    fontman/font_manager.h \
    networking/editor_pipe.h \
    networking/intproc.h \
    common_features/logger.h \
    common_features/logger_sets.h \
    gui/pge_menu.h \
    scenes/scene_loading.h \
    scenes/scene_title.h \
    ../_common/PGE_File_Formats/file_formats.h \
    ../_common/PGE_File_Formats/file_strlist.h \
    ../_common/PGE_File_Formats/lvl_filedata.h \
    ../_common/PGE_File_Formats/meta_filedata.h \
    ../_common/PGE_File_Formats/npc_filedata.h \
    ../_common/PGE_File_Formats/pge_x.h \
    ../_common/PGE_File_Formats/pge_x_macro.h \
    ../_common/PGE_File_Formats/save_filedata.h \
    ../_common/PGE_File_Formats/smbx64.h \
    ../_common/PGE_File_Formats/smbx64_macro.h \
    ../_common/PGE_File_Formats/smbx64_cnf_filedata.h \
    ../_common/PGE_File_Formats/wld_filedata.h \
    ../_common/PGE_File_Formats/pge_file_lib_globs.h \
    ../_common/PGE_File_Formats/pge_file_lib_sys.h \
    scenes/level/lvl_physenv.h \
    data_configs/obj_player.h \
    scenes/level/lvl_player_def.h \
    data_configs/obj_effect.h \
    data_configs/obj_npc.h \
    common_features/episode_state.h \
    common_features/event_queue.h \
    common_features/matrix_animator.h \
    scenes/level/lvl_backgrnd.h \
    scenes/level/lvl_camera.h \
    common_features/player_calibration.h \
    audio/SdlMusPlayer.h \
    data_configs/setup_wld_scene.h \
    data_configs/setup_load_screen.h \
    data_configs/setup_title_screen.h \
    data_configs/obj_music.h \
    data_configs/obj_sound.h \
    audio/pge_audio.h \
    data_configs/obj_sound_roles.h \
    settings/global_settings.h \
    common_features/maths.h \
    scenes/level/lvl_npc.h \
    controls/controller_joystick.h \
    controls/controller_key_map.h \
    scenes/world/wld_tilebox.h \
    scenes/world/wld_player_portrait.h \
    graphics/gl_color.h \
    gui/menu/_pge_menuitem.h \
    gui/menu/pge_bool_menuitem.h \
    gui/menu/pge_int_menuitem.h \
    gui/menu/pge_int_named_menuitem.h \
    gui/menu/pge_keygrab_menuitem.h \
    data_configs/obj_wld_items.h \
    common_features/npc_animator.h \
    common_features/RTree/RTree.h \
    scenes/level/lvl_section.h \
    common_features/rect.h \
    common_features/rectf.h \
    common_features/pointf.h \
    common_features/point.h \
    common_features/sizef.h \
    common_features/size.h \
    common_features/fader.h \
    data_configs/setup_credits_screen.h \
    script/lua_engine.h \
    gui/pge_menubox.h \
    script/lua_titlescreen_engine.h \
    script/lua_engine.h \
    script/lua_event.h \
    script/lua_level_engine.h \
    script/lua_global.h \
    script/lua_utils.h \
    script/bindings/core/globalfuncs/luafuncs_core_logger.h \
    script/bindings/core/events/luaevents_core_engine.h \
    script/bindings/core/globalfuncs/luafuncs_core_renderer.h \
    script/bindings/core/globalfuncs/luafuncs_core_settings.h \
    common_features/number_limiter.h \
    data_configs/setup_lvl_scene.h \
    script/lua_world_engine.h \
    script/lua_credits_engine.h \
    scenes/level/lvl_base_object.h \
    script/bindings/level/classes/luaclass_level_lvl_player.h \
    script/bindings/level/classes/luaclass_level_physobj.h \
    script/bindings/level/classes/luaclass_level_lvl_npc.h \
    script/bindings/core/globalfuncs/luafuncs_core_audio.h \
    scenes/_base/gfx_effect.h \
    script/bindings/core/globalfuncs/luafuncs_core_effect.h \
    script/bindings/level/globalfuncs/luafuncs_level_lvl_player.h \
    script/bindings/level/globalfuncs/luafuncs_level_lvl_npc.h \
    common_features/version_cmp.h \
    scenes/level/npc_detectors/lvl_base_detector.h \
    scenes/level/lvl_layer_engine.h \
    scenes/level/lvl_event_engine.h \
    scenes/level/npc_detectors/lvl_dtc_player_pos.h \
    scenes/level/npc_detectors/lvl_dtc_inarea.h \
    script/bindings/level/classes/luaclass_level_inareadetector.h \
    script/bindings/level/classes/luaclass_level_playerposdetector.h \
    script/bindings/core/classes/luaclass_core_simpleevent.h \
    gui/pge_questionbox.h \
    gui/pge_textinputbox.h \
    gui/pge_menuboxbase.h \
    settings/debugger.h \
    ../_common/data_functions/npctxt_manager.h


FORMS    += \
    data_configs/select_config.ui

RESOURCES += \
    _resources/engine.qrc

DISTFILES += \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    unused_thungs.txt \
    script/scrapped_code.txt \
    script/binding-tree.txt

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
