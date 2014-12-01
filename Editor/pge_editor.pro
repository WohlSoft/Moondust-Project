#----------------------------------------------------------------------------
#
# Platformer Game Engine by Wohlstand, a free platform for game making
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
#
#----------------------------------------------------------------------------
#
# Project created by QtCreator 2014-03-18T16:09:15
#
#----------------------------------------------------------------------------

QT       += gui widgets network
QT       -= opengl

DESTDIR = ../bin

static: {
release:OBJECTS_DIR = ../bin/_build/editor/_release/.obj
release:MOC_DIR     = ../bin/_build/editor/_release/.moc
release:RCC_DIR     = ../bin/_build/editor/_release/.rcc
release:UI_DIR      = ../bin/_build/editor/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/editor/_debug/.obj
debug:MOC_DIR       = ../bin/_build/editor/_debug/.moc
debug:RCC_DIR       = ../bin/_build/editor/_debug/.rcc
debug:UI_DIR        = ../bin/_build/editor/_debug/.ui
} else {
release:OBJECTS_DIR = ../bin/_build/_dynamic/editor/_release/.obj
release:MOC_DIR     = ../bin/_build/_dynamic/editor/_release/.moc
release:RCC_DIR     = ../bin/_build/_dynamic/editor/_release/.rcc
release:UI_DIR      = ../bin/_build/_dynamic/editor/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/_dynamic/editor/_debug/.obj
debug:MOC_DIR       = ../bin/_build/_dynamic/editor/_debug/.moc
debug:RCC_DIR       = ../bin/_build/_dynamic/editor/_debug/.rcc
debug:UI_DIR        = ../bin/_build/_dynamic/editor/_debug/.ui
}


translates.path = ../bin/languages
translates.files += languages/*.qm
translates.files += languages/*.png
INSTALLS = translates

TARGET = pge_editor
TEMPLATE = app

CONFIG += c++11

CONFIG += static
CONFIG += thread

LIBS += -lSDL2 -lSDL2_mixer
win32: LIBS += -lSDL2main
win32: LIBS += libversion
win32: LIBS += -lDbghelp

#DEFINES += USE_QMEDIAPLAYER

win32: {
    LIBS += -L../_Libs/_builds/win32/lib
    INCLUDEPATH += ../_Libs/_builds/win32/include
}

macx: {
    LIBS += -L ../_Libs/_builds/macos/lib
    INCLUDEPATH += ../_Libs/_builds/macos/include
}

win32: static: {
    DEFINES += MIKMOD_STATIC
    LIBS +=  -lvorbisfile -lvorbis -lmad -lmikmod.dll -lflac -logg
}


USE_QMEDIAPLAYER: {
    QT += multimedia
}
else
{
    QT -= multimedia
}

TRANSLATIONS += languages/editor_en.ts \
    languages/editor_ru.ts \
    languages/editor_uk.ts \
    languages/editor_de.ts \
    languages/editor_pl.ts \
    languages/editor_es.ts \
    languages/editor_nl.ts \
    languages/editor_it.ts \
    languages/editor_fr.ts \
    languages/editor_pt.ts \
    languages/editor_ja.ts \
    languages/editor_zh.ts

SOURCES += main.cpp\
    mainwindow.cpp \
    ../_Libs/EasyBMP/EasyBMP.cpp \
    about_dialog/aboutdialog.cpp \
    common_features/flowlayout.cpp \
    common_features/graphics_funcs.cpp \
    common_features/graphicsworkspace.cpp \
    common_features/grid.cpp \
    common_features/item_rectangles.cpp \
    common_features/items.cpp \
    common_features/levelfilelist.cpp \
    common_features/logger.cpp \
    common_features/mainwinconnect.cpp \
    common_features/musicfilelist.cpp \
    common_features/npc_animator.cpp \
    common_features/proxystyle.cpp \
    common_features/resizer/cornergrabber.cpp \
    common_features/resizer/item_resizer.cpp \
    common_features/sdl_music_player.cpp \
    common_features/simple_animator.cpp \
    common_features/themes.cpp \
    common_features/timecounter.cpp \
    common_features/util.cpp \
    data_configs/conf_lvl_bgo.cpp \
    data_configs/conf_lvl_bkgrd.cpp \
    data_configs/conf_lvl_block.cpp \
    data_configs/conf_lvl_npc.cpp \
    data_configs/conf_music.cpp \
    data_configs/conf_sound.cpp \
    data_configs/conf_tilesets.cpp \
    data_configs/conf_wld_level.cpp \
    data_configs/conf_wld_path.cpp \
    data_configs/conf_wld_scene.cpp \
    data_configs/conf_wld_tile.cpp \
    data_configs/config_manager.cpp \
    data_configs/configs_main.cpp \
    data_configs/configstatus.cpp \
    data_configs/custom_data.cpp \
    dev_console/devconsole.cpp \
    edit_level/level_edit.cpp \
    edit_level/levelprops.cpp \
    edit_level/lvl_draw.cpp \
    edit_level/lvl_edit_control.cpp \
    edit_level/lvl_files_io.cpp \
    edit_level/saveimage.cpp \
    edit_npc/npc_data_sets.cpp \
    edit_npc/npcedit.cpp \
    edit_npc/npceditscene.cpp \
    edit_npc/npctxt_controls.cpp \
    edit_npc/npctxt_files_io.cpp \
    edit_world/wld_draw.cpp \
    edit_world/wld_edit_control.cpp \
    edit_world/wld_files_io.cpp \
    edit_world/wld_saveimage.cpp \
    edit_world/world_edit.cpp \
    external_tools/gifs2png_gui.cpp \
    external_tools/lazyfixtool_gui.cpp \
    external_tools/png2gifs_gui.cpp \
    file_formats/file_formats.cpp \
    file_formats/file_lvl.cpp \
    file_formats/file_lvlx.cpp \
    file_formats/file_npc_txt.cpp \
    file_formats/file_wld.cpp \
    file_formats/file_wldx.cpp \
    file_formats/lvl_filedata.cpp \
    file_formats/npc_filedata.cpp \
    file_formats/pge_x.cpp \
    file_formats/smbx64.cpp \
    file_formats/wld_filedata.cpp \
    item_select_dialog/itemselectdialog.cpp \
    level_scene/HistoryManager.cpp \
    level_scene/item_bgo.cpp \
    level_scene/item_block.cpp \
    level_scene/item_door.cpp \
    level_scene/item_npc.cpp \
    level_scene/item_playerpoint.cpp \
    level_scene/item_water.cpp \
    level_scene/itemmsgbox.cpp \
    level_scene/lvl_clipboard.cpp \
    level_scene/lvl_collisions.cpp \
    level_scene/lvl_control.cpp \
    level_scene/lvl_init_filedata.cpp \
    level_scene/lvl_item_placing.cpp \
    level_scene/lvl_items.cpp \
    level_scene/lvl_resizers.cpp \
    level_scene/lvl_section.cpp \
    level_scene/lvl_setup.cpp \
    level_scene/lvl_usergfx.cpp \
    level_scene/newlayerbox.cpp \
    main_window/appsettings.cpp \
    main_window/clipboard.cpp \
    main_window/data_configs.cpp \
    main_window/dock/debugger.cpp \
    main_window/dock/lvl_door_props.cpp \
    main_window/dock/lvl_events_props.cpp \
    main_window/dock/lvl_item_props.cpp \
    main_window/dock/lvl_item_toolbox.cpp \
    main_window/dock/lvl_layers_props.cpp \
    main_window/dock/lvl_sctc_props.cpp \
    main_window/dock/lvl_search_toolbox.cpp \
    main_window/dock/tileset_item_box.cpp \
    main_window/dock/wld_item_props.cpp \
    main_window/dock/wld_item_toolbox.cpp \
    main_window/dock/wld_search_toolbox.cpp \
    main_window/dock/wld_world_settings.cpp \
    main_window/edit_common.cpp \
    main_window/edit_control.cpp \
    main_window/edit_level.cpp \
    main_window/edit_new_file.cpp \
    main_window/edit_placincg_switch.cpp \
    main_window/edit_world.cpp \
    main_window/events.cpp \
    main_window/files_io.cpp \
    main_window/main_settings.cpp \
    main_window/mainw_themes.cpp \
    main_window/menubar.cpp \
    main_window/music_player.cpp \
    main_window/recentfiles.cpp \
    main_window/savingnotificationdialog.cpp \
    main_window/sub_windows.cpp \
    main_window/tools_menu.cpp \
    main_window/translator.cpp \
    npc_dialog/npcdialog.cpp \
    SingleApplication/localserver.cpp \
    SingleApplication/singleapplication.cpp \
    tilesets/piecesmodel.cpp \
    tilesets/tileset.cpp \
    tilesets/tilesetconfiguredialog.cpp \
    tilesets/tilesetgroupeditor.cpp \
    tilesets/tilesetitembutton.cpp \
    wld_point_dialog/wld_setpoint.cpp \
    world_scene/HistoryManagerWld.cpp \
    world_scene/item_level.cpp \
    world_scene/item_music.cpp \
    world_scene/item_path.cpp \
    world_scene/item_point.cpp \
    world_scene/item_scene.cpp \
    world_scene/item_tile.cpp \
    world_scene/wld_clipboard.cpp \
    world_scene/wld_collisions.cpp \
    world_scene/wld_control.cpp \
    world_scene/wld_init_filedata.cpp \
    world_scene/wld_item_placing.cpp \
    world_scene/wld_items.cpp \
    world_scene/wld_resizers.cpp \
    world_scene/wld_scene.cpp \
    world_scene/wld_setup.cpp \
    world_scene/wld_usergfx.cpp \
    level_scene/edit_modes/mode_select.cpp \
    common_features/edit_mode_base.cpp \
    level_scene/edit_modes/mode_erase.cpp \
    level_scene/edit_modes/mode_place.cpp \
    level_scene/edit_modes/mode_square.cpp \
    level_scene/edit_modes/mode_line.cpp \
    level_scene/edit_modes/mode_hand.cpp \
    level_scene/edit_modes/mode_resize.cpp \
    common_features/crashhandler.cpp \
    world_scene/edit_modes/wld_mode_erase.cpp \
    world_scene/edit_modes/wld_mode_hand.cpp \
    world_scene/edit_modes/wld_mode_line.cpp \
    world_scene/edit_modes/wld_mode_place.cpp \
    world_scene/edit_modes/wld_mode_resize.cpp \
    world_scene/edit_modes/wld_mode_select.cpp \
    world_scene/edit_modes/wld_mode_square.cpp \
    world_scene/edit_modes/wld_mode_setpoint.cpp \
    level_scene/lvl_scene.cpp \
    main_window/main_testing.cpp \
    smart_import/smartimporter.cpp \
    level_scene/edit_modes/mode_fill.cpp \
    networking/engine_intproc.cpp \
    networking/engine_client.cpp \
    main_window/global_settings.cpp \
    world_scene/edit_modes/wld_mode_fill.cpp \
    main_window/dock/bookmark_box.cpp \
    ../_Libs/giflib/dgif_lib.c \
    ../_Libs/giflib/egif_lib.c \
    ../_Libs/giflib/gif_err.c \
    ../_Libs/giflib/gif_font.c \
    ../_Libs/giflib/gif_hash.c \
    ../_Libs/giflib/gifalloc.c \
    ../_Libs/giflib/quantize.c \
    script/command_compiler/basiccompiler.cpp \
    script/command_compiler/lunaluacompiler.cpp \
    script/commands/basiccommand.cpp \
    script/commands/memorycommand.cpp \
    script/scriptholder.cpp \
    script/commands/eventcommand.cpp


HEADERS  += defines.h \
    version.h \
    mainwindow.h \
    ../_Libs/EasyBMP/EasyBMP_BMP.h \
    ../_Libs/EasyBMP/EasyBMP_DataStructures.h \
    ../_Libs/EasyBMP/EasyBMP_VariousBMPutilities.h \
    ../_Libs/EasyBMP/EasyBMP.h \
    about_dialog/aboutdialog.h \
    common_features/app_path.h \
    common_features/flowlayout.h \
    common_features/graphics_funcs.h \
    common_features/graphicsworkspace.h \
    common_features/grid.h \
    common_features/item_rectangles.h \
    common_features/items.h \
    common_features/levelfilelist.h \
    common_features/logger_sets.h \
    common_features/logger.h \
    common_features/mainwinconnect.h \
    common_features/musicfilelist.h \
    common_features/npc_animator.h \
    common_features/proxystyle.h \
    common_features/resizer/corner_grabber.h \
    common_features/resizer/item_resizer.h \
    common_features/sdl_music_player.h \
    common_features/simple_animator.h \
    common_features/themes.h \
    common_features/timecounter.h \
    common_features/util.h \
    data_configs/config_manager.h \
    data_configs/configstatus.h \
    data_configs/custom_data.h \
    data_configs/data_configs.h \
    data_configs/dc_indexing.h \
    data_configs/obj_BG.h \
    data_configs/obj_bgo.h \
    data_configs/obj_block.h \
    data_configs/obj_npc.h \
    data_configs/obj_tilesets.h \
    data_configs/obj_wld_items.h \
    dev_console/devconsole.h \
    edit_level/level_edit.h \
    edit_level/levelprops.h \
    edit_level/saveimage.h \
    edit_npc/npcedit.h \
    edit_npc/npceditscene.h \
    edit_world/wld_saveimage.h \
    edit_world/world_edit.h \
    external_tools/gifs2png_gui.h \
    external_tools/lazyfixtool_gui.h \
    external_tools/png2gifs_gui.h \
    file_formats/file_formats.h \
    file_formats/lvl_filedata.h \
    file_formats/npc_filedata.h \
    file_formats/wld_filedata.h \
    item_select_dialog/itemselectdialog.h \
    level_scene/item_bgo.h \
    level_scene/item_block.h \
    level_scene/item_door.h \
    level_scene/item_npc.h \
    level_scene/item_playerpoint.h \
    level_scene/item_water.h \
    level_scene/itemmsgbox.h \
    level_scene/lvl_item_placing.h \
    level_scene/newlayerbox.h \
    main_window/appsettings.h \
    main_window/dock/tileset_item_box.h \
    main_window/global_settings.h \
    main_window/music_player.h \
    main_window/savingnotificationdialog.h \
    npc_dialog/npcdialog.h \
    SingleApplication/localserver.h \
    SingleApplication/singleapplication.h \
    tilesets/piecesmodel.h \
    tilesets/tileset.h \
    tilesets/tilesetconfiguredialog.h \
    tilesets/tilesetgroupeditor.h \
    tilesets/tilesetitembutton.h \
    wld_point_dialog/wld_setpoint.h \
    world_scene/item_level.h \
    world_scene/item_music.h \
    world_scene/item_path.h \
    world_scene/item_point.h \
    world_scene/item_scene.h \
    world_scene/item_tile.h \
    world_scene/wld_item_placing.h \
    world_scene/wld_scene.h \
    level_scene/edit_modes/mode_select.h \
    common_features/edit_mode_base.h \
    level_scene/edit_modes/mode_erase.h \
    level_scene/edit_modes/mode_place.h \
    level_scene/edit_modes/mode_square.h \
    level_scene/edit_modes/mode_line.h \
    level_scene/edit_modes/mode_hand.h \
    level_scene/edit_modes/mode_resize.h \
    common_features/crashhandler.h \
    world_scene/edit_modes/wld_mode_erase.h \
    world_scene/edit_modes/wld_mode_hand.h \
    world_scene/edit_modes/wld_mode_line.h \
    world_scene/edit_modes/wld_mode_place.h \
    world_scene/edit_modes/wld_mode_resize.h \
    world_scene/edit_modes/wld_mode_select.h \
    world_scene/edit_modes/wld_mode_square.h \
    world_scene/edit_modes/wld_mode_setpoint.h \
    level_scene/lvl_scene.h \
    smart_import/smartimporter.h \
    level_scene/edit_modes/mode_fill.h \
    networking/engine_intproc.h \
    networking/engine_client.h \
    world_scene/edit_modes/wld_mode_fill.h \
    file_formats/meta_filedata.h \
    ../_Libs/giflib/gif_hash.h \
    ../_Libs/giflib/gif_lib.h \
    ../_Libs/giflib/gif_lib_private.h \
    script/command_compiler/basiccompiler.h \
    script/command_compiler/lunaluacompiler.h \
    script/commands/basiccommand.h \
    script/commands/memorycommand.h \
    script/scriptholder.h \
    script/commands/eventcommand.h


FORMS    += \
    mainwindow.ui \
    about_dialog/aboutdialog.ui \
    common_features/levelfilelist.ui \
    common_features/musicfilelist.ui \
    data_configs/config_manager.ui \
    data_configs/configstatus.ui \
    dev_console/devconsole.ui \
    edit_level/leveledit.ui \
    edit_level/levelprops.ui \
    edit_level/saveimage.ui \
    edit_npc/npcedit.ui \
    edit_world/wld_saveimage.ui \
    edit_world/world_edit.ui \
    external_tools/gifs2png_gui.ui \
    external_tools/lazyfixtool_gui.ui \
    external_tools/png2gifs_gui.ui \
    item_select_dialog/itemselectdialog.ui \
    level_scene/itemmsgbox.ui \
    level_scene/tonewlayerbox.ui \
    main_window/appsettings.ui \
    main_window/savingnotificationdialog.ui \
    npc_dialog/npcdialog.ui \
    tilesets/tilesetconfiguredialog.ui \
    tilesets/tilesetgroupeditor.ui \
    wld_point_dialog/wld_setpoint.ui \
    common_features/crashhandler.ui


RC_FILE = _resources/pge_editor.rc

macx {
    ICON = _resources/mushroom.icns
}

RESOURCES += \
    _resources/editor.qrc

OTHER_FILES += \
    _resources/splash.png \
    _resources/mushroom.ico \
    _resources/pge_editor.rc \
    images/mac/mushroom.icns \
    images/mac/mushroom.hqx \
    images/01.png \
    images/02.png \
    images/03.png \
    images/04.png \
    images/05.png \
    images/06.png \
    images/07.png \
    images/08.png \
    images/09.png \
    images/10.png \
    images/11.png \
    images/12.png \
    images/13.png \
    images/14.png \
    images/15.png \
    images/16.png \
    images/17.png \
    images/18.png \
    images/19.png \
    images/20.png \
    images/21.png \
    images/arrow.png \
    images/coin.png \
    images/cur_pasta.png \
    images/cur_rubber.png \
    images/doors.png \
    images/draw_sand.png \
    images/draw_water.png \
    images/edit_copy.png \
    images/edit_cut.png \
    images/edit_paste.png \
    images/edit_redo.png \
    images/edit_undo.png \
    images/generator_proj.png \
    images/generator_warp.png \
    images/grid.png \
    images/hand.png \
    images/layers.png \
    images/level.png \
    images/level16.png \
    images/lock_bgo_cl.png \
    images/lock_bgo_op.png \
    images/lock_block_cl.png \
    images/lock_block_op.png \
    images/lock_door_cl.png \
    images/lock_door_op.png \
    images/lock_npc_cl.png \
    images/lock_npc_op.png \
    images/lock_water_cl.png \
    images/lock_water_op.png \
    images/mushroom.png \
    images/mushroom16.png \
    images/new.png \
    images/open.png \
    images/player1_point.png \
    images/player1_start.png \
    images/player2_point.png \
    images/player2_start.png \
    images/playmusic.png \
    images/reload_data.png \
    images/reset_pos.png \
    images/rubber.png \
    images/save.png \
    images/saveas.png \
    images/section.png \
    images/section16.png \
    images/select_only.png \
    images/splash.png \
    images/warp_entrance.png \
    images/warp_exit.png \
    images/world.png \
    images/world16.png \
    _resources/mushroom.icns \
    _resources/mushroom.hqx
