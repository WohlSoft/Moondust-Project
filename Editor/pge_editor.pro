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

INCLUDEPATH += .

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
    languages/editor_zh.ts \
    languages/editor_bg.ts

SOURCES += main.cpp\
    mainwindow.cpp \
    ../_Libs/EasyBMP/EasyBMP.cpp \
    ../_Libs/giflib/dgif_lib.c \
    ../_Libs/giflib/egif_lib.c \
    ../_Libs/giflib/gif_err.c \
    ../_Libs/giflib/gif_font.c \
    ../_Libs/giflib/gif_hash.c \
    ../_Libs/giflib/gifalloc.c \
    ../_Libs/giflib/quantize.c \
    common_features/crashhandler.cpp \
    common_features/edit_mode_base.cpp \
    common_features/flowlayout.cpp \
    common_features/graphics_funcs.cpp \
    common_features/graphicsworkspace.cpp \
    common_features/grid.cpp \
    common_features/item_rectangles.cpp \
    common_features/items.cpp \
    common_features/logger.cpp \
    common_features/mainwinconnect.cpp \
    common_features/npc_animator.cpp \
    common_features/proxystyle.cpp \
    common_features/simple_animator.cpp \
    common_features/themes.cpp \
    common_features/timecounter.cpp \
    common_features/util.cpp \
    common_features/resizer/cornergrabber.cpp \
    common_features/resizer/item_resizer.cpp \
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
    editing/_dialogs/itemselectdialog.cpp \
    editing/_dialogs/levelfilelist.cpp \
    editing/_dialogs/musicfilelist.cpp \
    editing/_dialogs/npcdialog.cpp \
    editing/_dialogs/wld_setpoint.cpp \
    editing/_scenes/level/HistoryManager.cpp \
    editing/_scenes/level/item_bgo.cpp \
    editing/_scenes/level/item_block.cpp \
    editing/_scenes/level/item_door.cpp \
    editing/_scenes/level/item_npc.cpp \
    editing/_scenes/level/item_playerpoint.cpp \
    editing/_scenes/level/item_water.cpp \
    editing/_scenes/level/itemmsgbox.cpp \
    editing/_scenes/level/lvl_clipboard.cpp \
    editing/_scenes/level/lvl_collisions.cpp \
    editing/_scenes/level/lvl_control.cpp \
    editing/_scenes/level/lvl_init_filedata.cpp \
    editing/_scenes/level/lvl_item_placing.cpp \
    editing/_scenes/level/lvl_items.cpp \
    editing/_scenes/level/lvl_resizers.cpp \
    editing/_scenes/level/lvl_scene.cpp \
    editing/_scenes/level/lvl_section.cpp \
    editing/_scenes/level/lvl_setup.cpp \
    editing/_scenes/level/lvl_usergfx.cpp \
    editing/_scenes/level/newlayerbox.cpp \
    editing/_scenes/level/edit_modes/mode_erase.cpp \
    editing/_scenes/level/edit_modes/mode_fill.cpp \
    editing/_scenes/level/edit_modes/mode_hand.cpp \
    editing/_scenes/level/edit_modes/mode_line.cpp \
    editing/_scenes/level/edit_modes/mode_place.cpp \
    editing/_scenes/level/edit_modes/mode_resize.cpp \
    editing/_scenes/level/edit_modes/mode_select.cpp \
    editing/_scenes/level/edit_modes/mode_square.cpp \
    editing/_scenes/world/HistoryManagerWld.cpp \
    editing/_scenes/world/item_level.cpp \
    editing/_scenes/world/item_music.cpp \
    editing/_scenes/world/item_path.cpp \
    editing/_scenes/world/item_point.cpp \
    editing/_scenes/world/item_scene.cpp \
    editing/_scenes/world/item_tile.cpp \
    editing/_scenes/world/wld_clipboard.cpp \
    editing/_scenes/world/wld_collisions.cpp \
    editing/_scenes/world/wld_control.cpp \
    editing/_scenes/world/wld_init_filedata.cpp \
    editing/_scenes/world/wld_item_placing.cpp \
    editing/_scenes/world/wld_items.cpp \
    editing/_scenes/world/wld_resizers.cpp \
    editing/_scenes/world/wld_scene.cpp \
    editing/_scenes/world/wld_setup.cpp \
    editing/_scenes/world/wld_usergfx.cpp \
    editing/_scenes/world/edit_modes/wld_mode_erase.cpp \
    editing/_scenes/world/edit_modes/wld_mode_fill.cpp \
    editing/_scenes/world/edit_modes/wld_mode_hand.cpp \
    editing/_scenes/world/edit_modes/wld_mode_line.cpp \
    editing/_scenes/world/edit_modes/wld_mode_place.cpp \
    editing/_scenes/world/edit_modes/wld_mode_resize.cpp \
    editing/_scenes/world/edit_modes/wld_mode_select.cpp \
    editing/_scenes/world/edit_modes/wld_mode_setpoint.cpp \
    editing/_scenes/world/edit_modes/wld_mode_square.cpp \
    editing/edit_level/level_edit.cpp \
    editing/edit_level/levelprops.cpp \
    editing/edit_level/lvl_clone_section.cpp \
    editing/edit_level/lvl_draw.cpp \
    editing/edit_level/lvl_edit_control.cpp \
    editing/edit_level/lvl_files_io.cpp \
    editing/edit_level/saveimage.cpp \
    editing/edit_npc/npc_data_sets.cpp \
    editing/edit_npc/npcedit.cpp \
    editing/edit_npc/npceditscene.cpp \
    editing/edit_npc/npctxt_controls.cpp \
    editing/edit_npc/npctxt_files_io.cpp \
    editing/edit_world/wld_draw.cpp \
    editing/edit_world/wld_edit_control.cpp \
    editing/edit_world/wld_files_io.cpp \
    editing/edit_world/wld_saveimage.cpp \
    editing/edit_world/world_edit.cpp \
    file_formats/file_formats.cpp \
    file_formats/file_lvl.cpp \
    file_formats/file_lvlx.cpp \
    file_formats/file_meta.cpp \
    file_formats/file_npc_txt.cpp \
    file_formats/file_wld.cpp \
    file_formats/file_wldx.cpp \
    file_formats/lvl_filedata.cpp \
    file_formats/npc_filedata.cpp \
    file_formats/pge_x.cpp \
    file_formats/smbx64.cpp \
    file_formats/wld_filedata.cpp \
    main_window/appsettings.cpp \
    main_window/clipboard.cpp \
    main_window/data_configs.cpp \
    main_window/edit_common.cpp \
    main_window/edit_control.cpp \
    main_window/edit_level.cpp \
    main_window/edit_new_file.cpp \
    main_window/edit_placincg_switch.cpp \
    main_window/edit_script.cpp \
    main_window/edit_world.cpp \
    main_window/events.cpp \
    main_window/files_io.cpp \
    main_window/global_settings.cpp \
    main_window/main_settings.cpp \
    main_window/main_testing.cpp \
    main_window/mainw_themes.cpp \
    main_window/menubar.cpp \
    main_window/music_player.cpp \
    main_window/recentfiles.cpp \
    main_window/savingnotificationdialog.cpp \
    main_window/sub_windows.cpp \
    main_window/tools_menu.cpp \
    main_window/translator.cpp \
    main_window/about_dialog/aboutdialog.cpp \
    main_window/dock/bookmark_box.cpp \
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
    main_window/tools/main_lvl_section_mods.cpp \
    main_window/tools/main_tool_cdata_cleaner.cpp \
    main_window/tools/main_tool_cdata_import.cpp \
    main_window/tools/main_tool_cdata_lazyfix.cpp \
    networking/engine_client.cpp \
    networking/engine_intproc.cpp \
    script/scriptholder.cpp \
    script/command_compiler/autocodecompiler.cpp \
    script/command_compiler/basiccompiler.cpp \
    script/command_compiler/lunaluacompiler.cpp \
    script/commands/basiccommand.cpp \
    script/commands/eventcommand.cpp \
    script/commands/memorycommand.cpp \
    script/gui/additionalsettings.cpp \
    SingleApplication/localserver.cpp \
    SingleApplication/singleapplication.cpp \
    tools/external_tools/gifs2png_gui.cpp \
    tools/external_tools/lazyfixtool_gui.cpp \
    tools/external_tools/png2gifs_gui.cpp \
    tools/math/blocksperseconddialog.cpp \
    tools/smart_import/smartimporter.cpp \
    tools/tilesets/piecesmodel.cpp \
    tools/tilesets/tileset.cpp \
    tools/tilesets/tilesetconfiguredialog.cpp \
    tools/tilesets/tilesetgroupeditor.cpp \
    tools/tilesets/tilesetitembutton.cpp \
    audio/sdl_music_player.cpp
    

HEADERS  += defines.h \
    version.h \
    mainwindow.h \
    ../_Libs/EasyBMP/EasyBMP_BMP.h \
    ../_Libs/EasyBMP/EasyBMP_DataStructures.h \
    ../_Libs/EasyBMP/EasyBMP_VariousBMPutilities.h \
    ../_Libs/EasyBMP/EasyBMP.h \
    ../_Libs/giflib/gif_hash.h \
    ../_Libs/giflib/gif_lib.h \
    ../_Libs/giflib/gif_lib_private.h \
    common_features/app_path.h \
    common_features/crashhandler.h \
    common_features/edit_mode_base.h \
    common_features/flowlayout.h \
    common_features/graphics_funcs.h \
    common_features/graphicsworkspace.h \
    common_features/grid.h \
    common_features/item_rectangles.h \
    common_features/items.h \
    common_features/logger.h \
    common_features/logger_sets.h \
    common_features/mainwinconnect.h \
    common_features/npc_animator.h \
    common_features/proxystyle.h \
    common_features/simple_animator.h \
    common_features/themes.h \
    common_features/timecounter.h \
    common_features/util.h \
    common_features/resizer/corner_grabber.h \
    common_features/resizer/item_resizer.h \
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
    editing/_dialogs/itemselectdialog.h \
    editing/_dialogs/levelfilelist.h \
    editing/_dialogs/musicfilelist.h \
    editing/_dialogs/npcdialog.h \
    editing/_dialogs/wld_setpoint.h \
    editing/_scenes/level/item_bgo.h \
    editing/_scenes/level/item_block.h \
    editing/_scenes/level/item_door.h \
    editing/_scenes/level/item_npc.h \
    editing/_scenes/level/item_playerpoint.h \
    editing/_scenes/level/item_water.h \
    editing/_scenes/level/itemmsgbox.h \
    editing/_scenes/level/lvl_item_placing.h \
    editing/_scenes/level/lvl_scene.h \
    editing/_scenes/level/newlayerbox.h \
    editing/_scenes/level/edit_modes/mode_erase.h \
    editing/_scenes/level/edit_modes/mode_fill.h \
    editing/_scenes/level/edit_modes/mode_hand.h \
    editing/_scenes/level/edit_modes/mode_line.h \
    editing/_scenes/level/edit_modes/mode_place.h \
    editing/_scenes/level/edit_modes/mode_resize.h \
    editing/_scenes/level/edit_modes/mode_select.h \
    editing/_scenes/level/edit_modes/mode_square.h \
    editing/_scenes/world/item_level.h \
    editing/_scenes/world/item_music.h \
    editing/_scenes/world/item_path.h \
    editing/_scenes/world/item_point.h \
    editing/_scenes/world/item_scene.h \
    editing/_scenes/world/item_tile.h \
    editing/_scenes/world/wld_item_placing.h \
    editing/_scenes/world/wld_scene.h \
    editing/_scenes/world/edit_modes/wld_mode_erase.h \
    editing/_scenes/world/edit_modes/wld_mode_fill.h \
    editing/_scenes/world/edit_modes/wld_mode_hand.h \
    editing/_scenes/world/edit_modes/wld_mode_line.h \
    editing/_scenes/world/edit_modes/wld_mode_place.h \
    editing/_scenes/world/edit_modes/wld_mode_resize.h \
    editing/_scenes/world/edit_modes/wld_mode_select.h \
    editing/_scenes/world/edit_modes/wld_mode_setpoint.h \
    editing/_scenes/world/edit_modes/wld_mode_square.h \
    editing/edit_level/level_edit.h \
    editing/edit_level/levelprops.h \
    editing/edit_level/lvl_clone_section.h \
    editing/edit_level/saveimage.h \
    editing/edit_npc/npcedit.h \
    editing/edit_npc/npceditscene.h \
    editing/edit_world/wld_saveimage.h \
    editing/edit_world/world_edit.h \
    file_formats/file_formats.h \
    file_formats/lvl_filedata.h \
    file_formats/meta_filedata.h \
    file_formats/npc_filedata.h \
    file_formats/wld_filedata.h \
    main_window/appsettings.h \
    main_window/global_settings.h \
    main_window/music_player.h \
    main_window/savingnotificationdialog.h \
    main_window/about_dialog/aboutdialog.h \
    main_window/dock/tileset_item_box.h \
    networking/engine_client.h \
    networking/engine_intproc.h \
    script/scriptholder.h \
    script/command_compiler/autocodecompiler.h \
    script/command_compiler/basiccompiler.h \
    script/command_compiler/lunaluacompiler.h \
    script/commands/basiccommand.h \
    script/commands/eventcommand.h \
    script/commands/memorycommand.h \
    script/gui/additionalsettings.h \
    SingleApplication/localserver.h \
    SingleApplication/singleapplication.h \
    tools/external_tools/gifs2png_gui.h \
    tools/external_tools/lazyfixtool_gui.h \
    tools/external_tools/png2gifs_gui.h \
    tools/math/blocksperseconddialog.h \
    tools/smart_import/smartimporter.h \
    tools/tilesets/piecesmodel.h \
    tools/tilesets/tileset.h \
    tools/tilesets/tilesetconfiguredialog.h \
    tools/tilesets/tilesetgroupeditor.h \
    tools/tilesets/tilesetitembutton.h \
    audio/sdl_music_player.h


FORMS    += \
    common_features/crashhandler.ui \
    data_configs/config_manager.ui \
    data_configs/configstatus.ui \
    dev_console/devconsole.ui \
    editing/_dialogs/itemselectdialog.ui \
    editing/_dialogs/levelfilelist.ui \
    editing/_dialogs/musicfilelist.ui \
    editing/_dialogs/npcdialog.ui \
    editing/_dialogs/wld_setpoint.ui \
    editing/_scenes/level/itemmsgbox.ui \
    editing/_scenes/level/tonewlayerbox.ui \
    editing/edit_level/leveledit.ui \
    editing/edit_level/levelprops.ui \
    editing/edit_level/lvl_clone_section.ui \
    editing/edit_level/saveimage.ui \
    editing/edit_npc/npcedit.ui \
    editing/edit_world/wld_saveimage.ui \
    editing/edit_world/world_edit.ui \
    main_window/appsettings.ui \
    main_window/savingnotificationdialog.ui \
    main_window/about_dialog/aboutdialog.ui \
    script/gui/additionalsettings.ui \
    tools/external_tools/gifs2png_gui.ui \
    tools/external_tools/lazyfixtool_gui.ui \
    tools/external_tools/png2gifs_gui.ui \
    tools/math/blocksperseconddialog.ui \
    tools/tilesets/tilesetconfiguredialog.ui \
    tools/tilesets/tilesetgroupeditor.ui \
    mainwindow.ui



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
