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
LIBS+= -L$$PWD/../_Libs/_builds/sdl2_mixer_mod
LIBS += -L$$PWD/../_Libs/_builds/commonlibs
DEFINES += PGE_ENGINE

LIBS += -lluabind -lluajit-5.1
android: {
    LIBS += -L../_Libs/_builds/android/lib
    INCLUDEPATH += ../_Libs/_builds/android/include
    LIBS += -lSDL2 -lglut -lGLU
}
win32: {
    LIBS += -L../_Libs/_builds/win32/lib
    INCLUDEPATH += ../_Libs/_builds/win32/include
    LIBS += -lSDL2 -lSDL2_mixer -lSDL2main libversion
}
macx: {
    LIBS += -L$$PWD/../_Libs/_builds/macos/lib
    INCLUDEPATH += $$PWD/../_Libs/_builds/macos/include
    INCLUDEPATH += $$PWD/../_Libs/_builds/macos/frameworks/SDL2.framework/Headers
    LIBS += -F$$PWD/../_Libs/_builds/macos/frameworks -framework SDL2 -lSDL2_mixer
    QMAKE_POST_LINK = $$PWD/mac_deploy_libs.sh
}
linux-g++: {
    LIBS += -L ../_Libs/_builds/linux/lib
    INCLUDEPATH += ../_Libs/_builds/linux/include
    LIBS += -lSDL2 -lSDL2_mixer -lglut -lGLU
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
    ../_common/PGE_File_Formats/file_rwopen.cpp \
    ../_common/PGE_File_Formats/file_strlist.cpp \
    ../_common/PGE_File_Formats/lvl_filedata.cpp \
    ../_common/PGE_File_Formats/npc_filedata.cpp \
    ../_common/PGE_File_Formats/pge_x.cpp \
    ../_common/PGE_File_Formats/save_filedata.cpp \
    ../_common/PGE_File_Formats/smbx64.cpp \
    ../_common/PGE_File_Formats/wld_filedata.cpp \
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
    scenes/_base/gfx_effect.cpp


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
    ../_common/PGE_File_Formats/save_filedata.h \
    ../_common/PGE_File_Formats/smbx64.h \
    ../_common/PGE_File_Formats/wld_filedata.h \
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
    scenes/_base/gfx_effect.h

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
