#-------------------------------------------------
#
# Project created by QtCreator 2014-09-05T22:51:01
#
#-------------------------------------------------

QT += core gui opengl network
#QT += widgets

#QMAKE_CXXFLAGS += -Wno-maybe-uninitialized -Wstrict-aliasing=0 -Wno-unused-local-typedefs
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'

DESTDIR = ../bin

static: {
release:OBJECTS_DIR = ../bin/_build/engine/_release/.obj
release:MOC_DIR     = ../bin/_build/engine/_release/.moc
release:RCC_DIR     = ../bin/_build/engine/_release/.rcc
release:UI_DIR      = ../bin/_build/engine/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/engine/_debug/.obj
debug:MOC_DIR       = ../bin/_build/engine/_debug/.moc
debug:RCC_DIR       = ../bin/_build/engine/_debug/.rcc
debug:UI_DIR        = ../bin/_build/engine/_debug/.ui
} else {
release:OBJECTS_DIR = ../bin/_build/_dynamic/engine/_release/.obj
release:MOC_DIR     = ../bin/_build/_dynamic/engine/_release/.moc
release:RCC_DIR     = ../bin/_build/_dynamic/engine/_release/.rcc
release:UI_DIR      = ../bin/_build/_dynamic/engine/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/_dynamic/engine/_debug/.obj
debug:MOC_DIR       = ../bin/_build/_dynamic/engine/_debug/.moc
debug:RCC_DIR       = ../bin/_build/_dynamic/engine/_debug/.rcc
debug:UI_DIR        = ../bin/_build/_dynamic/engine/_debug/.ui
}

TARGET = pge_engine
TEMPLATE = app
CONFIG += c++11
CONFIG += thread

CONFIG += static

DEPENDPATH += "../_Libs/oolua/project"
DEPENDPATH += "../_Libs/Box2D/project"

DEFINES += PGE_ENGINE

INCLUDEPATH += "../_Libs/" "../_common"

LIBS += -L../_Libs/_builds/commonlibs

win32: {
    LIBS += -L../_Libs/_builds/win32/lib
    INCLUDEPATH += ../_Libs/_builds/win32/include
}

LIBS += -loolua -lbox2d -lSDL2
win32: LIBS += -lSDL2main
win32: LIBS += libversion
unix:  LIBS += -glut -lGLU

RC_FILE = _resources/engine.rc

SOURCES += \
    main.cpp \
    physics/base_object.cpp \
    physics/phys_util.cpp \
    graphics/lvl_camera.cpp \
    graphics/graphics.cpp \
    scenes/scene_level.cpp \
    scenes/scene.cpp \
    scenes/scene_world.cpp \
    scenes/scene_credits.cpp \
    scenes/scene_gameover.cpp \
    graphics/gl_renderer.cpp \
    graphics/window.cpp \
    graphics/graphics_lvl_backgrnd.cpp \
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
    physics/contact_listener.cpp \
    scenes/level/lvl_warp.cpp \
    scenes/level/lvl_scene_ptr.cpp \
    scenes/level/lvl_scene_items.cpp \
    scenes/level/lvl_scene_sections.cpp \
    scenes/level/lvl_scene_timers.cpp \
    scenes/level/lvl_scene_files_io.cpp \
    scenes/level/lvl_scene_init.cpp \
    physics/engine/pge_phys_world.cpp \
    physics/engine/Quadtree.cpp \
    physics/engine/pge_phys_body.cpp \
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
    data_configs/obj_player.cpp

HEADERS  += \
    physics/base_object.h \
    physics/phys_util.h \
    graphics/lvl_camera.h \
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
    graphics/graphics_lvl_backgrnd.h \
    version.h \
    physics/contact_listener.h \
    scenes/level/lvl_warp.h \
    scenes/level/lvl_scene_ptr.h \
    physics/engine/pge_phys_world.h \
    physics/engine/Quadtree.h \
    physics/engine/pge_phys_body.h \
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
    data_configs/obj_npc.h

FORMS    += \
    data_configs/select_config.ui

RESOURCES += \
    _resources/engine.qrc
