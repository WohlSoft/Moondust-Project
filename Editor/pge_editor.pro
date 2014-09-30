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

#DEFINES += USE_QMEDIAPLAYER

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
    languages/editor_ja.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    file_formats/file_formats.cpp \
    file_formats/file_lvl.cpp \
    file_formats/file_npc_txt.cpp \
    file_formats/file_wld.cpp \
    file_formats/smbx64.cpp \
    main_window/recentfiles.cpp \
    main_window/events.cpp \
    main_window/main_settings.cpp \
    main_window/clipboard.cpp \
    main_window/sub_windows.cpp \
    main_window/data_configs.cpp \
    main_window/edit_level.cpp \
    main_window/edit_world.cpp \
    main_window/music_player.cpp \
    main_window/edit_common.cpp \
    main_window/files_io.cpp \
    main_window/menubar.cpp \
    main_window/edit_control.cpp \
    main_window/appsettings.cpp \
    level_scene/lvlscene.cpp \
    level_scene/item_bgo.cpp \
    level_scene/item_block.cpp \
    level_scene/HistoryManager.cpp \
    level_scene/lvl_clipboard.cpp \
    level_scene/item_npc.cpp \
    level_scene/lvl_control.cpp \
    level_scene/lvl_items.cpp \
    level_scene/lvl_usergfx.cpp \
    level_scene/itemmsgbox.cpp \
    level_scene/lvl_collisions.cpp \
    level_scene/newlayerbox.cpp \
    data_configs/configs_main.cpp \
    data_configs/conf_lvl_bgo.cpp \
    data_configs/conf_lvl_bkgrd.cpp \
    data_configs/conf_lvl_block.cpp \
    data_configs/conf_lvl_npc.cpp \
    data_configs/conf_music.cpp \
    common_features/timecounter.cpp \
    common_features/logger.cpp \
    common_features/musicfilelist.cpp \
    edit_npc/npcedit.cpp \
    npc_dialog/npcdialog.cpp \
    edit_level/level_edit.cpp \
    edit_level/levelprops.cpp \
    edit_level/saveimage.cpp \
    edit_level/lvl_files_io.cpp \
    edit_level/lvl_draw.cpp \
    edit_level/lvl_edit_control.cpp \
    edit_npc/npctxt_files_io.cpp \
    edit_npc/npc_data_sets.cpp \
    edit_npc/npctxt_controls.cpp \
    about_dialog/aboutdialog.cpp \
    common_features/grid.cpp \
    common_features/mainwinconnect.cpp \
    level_scene/item_water.cpp \
    level_scene/lvl_setup.cpp \
    level_scene/item_door.cpp \
    level_scene/lvl_init_filedata.cpp \
    level_scene/lvl_section.cpp \
    main_window/dock/lvl_item_props.cpp \
    main_window/dock/lvl_sctc_props.cpp \
    main_window/dock/lvl_door_props.cpp \
    main_window/dock/lvl_item_toolbox.cpp \
    edit_npc/npceditscene.cpp \
    main_window/edit_new_file.cpp \
    main_window/translator.cpp \
    main_window/dock/lvl_events_props.cpp \
    data_configs/conf_sound.cpp \
    main_window/dock/lvl_layers_props.cpp \
    data_configs/configstatus.cpp \
    common_features/graphics_funcs.cpp \
    common_features/levelfilelist.cpp \
    libs/EasyBMP/EasyBMP.cpp \
    item_select_dialog/itemselectdialog.cpp \
    common_features/simple_animator.cpp \
    main_window/dock/lvl_search_toolbox.cpp \
    common_features/util.cpp \
    data_configs/conf_wld_tile.cpp \
    data_configs/conf_wld_scene.cpp \
    data_configs/conf_wld_path.cpp \
    data_configs/conf_wld_level.cpp \
    tilesets/tilesetconfiguredialog.cpp \
    tilesets/piecesmodel.cpp \
    tilesets/tileset.cpp \
    edit_world/world_edit.cpp \
    edit_world/wld_draw.cpp \
    edit_world/wld_edit_control.cpp \
    edit_world/wld_files_io.cpp \
    world_scene/wld_scene.cpp \
    world_scene/item_tile.cpp \
    common_features/resizer/cornergrabber.cpp \
    common_features/resizer/item_resizer.cpp \
    world_scene/item_scene.cpp \
    world_scene/item_path.cpp \
    world_scene/item_level.cpp \
    world_scene/wld_setup.cpp \
    world_scene/wld_collisions.cpp \
    world_scene/wld_clipboard.cpp \
    world_scene/wld_init_filedata.cpp \
    world_scene/wld_items.cpp \
    world_scene/wld_usergfx.cpp \
    world_scene/wld_control.cpp \
    world_scene/wld_item_placing.cpp \
    main_window/dock/wld_item_toolbox.cpp \
    world_scene/HistoryManagerWld.cpp \
    main_window/dock/wld_item_props.cpp \
    world_scene/item_music.cpp \
    main_window/dock/wld_world_settings.cpp \
    common_features/proxystyle.cpp \
    main_window/savingnotificationdialog.cpp \
    wld_point_dialog/wld_setpoint.cpp \
    level_scene/lvl_item_placing.cpp \
    common_features/item_rectangles.cpp \
    edit_world/wld_saveimage.cpp \
    level_scene/lvl_resizers.cpp \
    world_scene/wld_resizers.cpp \
    dev_console/devconsole.cpp \
    world_scene/item_point.cpp \
    main_window/dock/wld_search_toolbox.cpp \
    tilesets/tilesetgroupeditor.cpp \
    tilesets/tilesetitembutton.cpp \
    external_tools/gifs2png_gui.cpp \
    external_tools/lazyfixtool_gui.cpp \
    main_window/tools_menu.cpp \
    common_features/items.cpp \
    common_features/flowlayout.cpp \
    main_window/edit_placincg_switch.cpp \
    common_features/graphicsworkspace.cpp \
    external_tools/png2gifs_gui.cpp \
    main_window/dock/tileset_item_box.cpp \
    main_window/dock/debugger.cpp \
    data_configs/conf_tilesets.cpp \
    common_features/npc_animator.cpp \
    data_configs/config_manager.cpp \
    file_formats/lvl_filedata.cpp \
    file_formats/file_lvlx.cpp \
    file_formats/npc_filedata.cpp \
    file_formats/wld_filedata.cpp \
    file_formats/pge_x.cpp \
    SingleApplication/localserver.cpp \
    SingleApplication/singleapplication.cpp \
    level_scene/item_playerpoint.cpp \
    common_features/sdl_music_player.cpp \
    data_configs/custom_data.cpp \
    file_formats/file_wldx.cpp \
    common_features/themes.cpp \
    main_window/mainw_themes.cpp

HEADERS  += mainwindow.h \
    file_formats/file_formats.h \
    level_scene/item_bgo.h \
    level_scene/item_block.h \
    level_scene/lvlscene.h \
    level_scene/item_npc.h \
    data_configs/data_configs.h \
    common_features/timecounter.h \
    common_features/logger.h \
    common_features/logger_sets.h \
    common_features/musicfilelist.h \
    npc_dialog/npcdialog.h \
    level_scene/itemmsgbox.h \
    main_window/appsettings.h \
    level_scene/newlayerbox.h \
    file_formats/lvl_filedata.h \
    file_formats/npc_filedata.h \
    file_formats/wld_filedata.h \
    edit_level/level_edit.h \
    edit_level/levelprops.h \
    edit_level/saveimage.h \
    edit_npc/npcedit.h \
    about_dialog/aboutdialog.h \
    common_features/grid.h \
    main_window/music_player.h \
    common_features/mainwinconnect.h \
    level_scene/item_water.h \
    main_window/global_settings.h \
    level_scene/item_door.h \
    level_scene/lvl_item_placing.h \
    data_configs/obj_npc.h \
    edit_npc/npceditscene.h \
    data_configs/custom_data.h \
    data_configs/configstatus.h \
    common_features/graphics_funcs.h \
    common_features/levelfilelist.h \
    libs/EasyBMP/EasyBMP.h \
    libs/EasyBMP/EasyBMP_BMP.h \
    libs/EasyBMP/EasyBMP_DataStructures.h \
    libs/EasyBMP/EasyBMP_VariousBMPutilities.h \
    item_select_dialog/itemselectdialog.h \
    common_features/simple_animator.h \
    common_features/util.h \
    tilesets/tilesetconfiguredialog.h \
    tilesets/piecesmodel.h \
    tilesets/tileset.h \
    edit_world/world_edit.h \
    world_scene/wld_scene.h \
    world_scene/item_tile.h \
    common_features/resizer/corner_grabber.h \
    common_features/resizer/item_resizer.h \
    world_scene/item_scene.h \
    world_scene/item_path.h \
    world_scene/item_level.h \
    world_scene/item_music.h \
    world_scene/wld_item_placing.h \
    common_features/proxystyle.h \
    main_window/savingnotificationdialog.h \
    wld_point_dialog/wld_setpoint.h \
    common_features/item_rectangles.h \
    edit_world/wld_saveimage.h \
    version.h \
    dev_console/devconsole.h \
    world_scene/item_point.h \
    tilesets/tilesetgroupeditor.h \
    defines.h \
    tilesets/tilesetitembutton.h \
    external_tools/gifs2png_gui.h \
    external_tools/lazyfixtool_gui.h \
    common_features/items.h \
    common_features/flowlayout.h \
    common_features/graphicsworkspace.h \
    external_tools/png2gifs_gui.h \
    main_window/dock/tileset_item_box.h \
    data_configs/obj_block.h \
    data_configs/obj_bgo.h \
    data_configs/obj_BG.h \
    data_configs/obj_wld_items.h \
    data_configs/dc_indexing.h \
    data_configs/obj_tilesets.h \
    common_features/npc_animator.h \
    data_configs/config_manager.h \
    SingleApplication/localserver.h \
    SingleApplication/singleapplication.h \
    level_scene/item_playerpoint.h \
    common_features/sdl_music_player.h \
    common_features/app_path.h \
    common_features/themes.h

FORMS    += \
    mainwindow.ui \
    npc_dialog/npcdialog.ui \
    level_scene/itemmsgbox.ui \
    main_window/appsettings.ui \
    level_scene/tonewlayerbox.ui \
    common_features/musicfilelist.ui \
    edit_level/leveledit.ui \
    edit_level/levelprops.ui \
    edit_level/saveimage.ui \
    edit_npc/npcedit.ui \
    about_dialog/aboutdialog.ui \
    data_configs/configstatus.ui \
    common_features/levelfilelist.ui \
    item_select_dialog/itemselectdialog.ui \
    tilesets/tilesetconfiguredialog.ui \
    edit_world/world_edit.ui \
    main_window/savingnotificationdialog.ui \
    wld_point_dialog/wld_setpoint.ui \
    edit_world/wld_saveimage.ui \
    dev_console/devconsole.ui \
    tilesets/tilesetgroupeditor.ui \
    external_tools/gifs2png_gui.ui \
    external_tools/lazyfixtool_gui.ui \
    external_tools/png2gifs_gui.ui \
    data_configs/config_manager.ui

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
