#-------------------------------------------------
#
# Project created by QtCreator 2014-03-18T16:09:15
#
#-------------------------------------------------

QT       += core gui widgets multimedia

TARGET = plweditor
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    levelprops.cpp \
    npcedit.cpp \
    leveledit.cpp \
    dataconfigs.cpp \
    saveimage.cpp \
    musicfilelist.cpp \
    logger.cpp \
    file_formats/file_formats.cpp \
    file_formats/file_lvl.cpp \
    file_formats/file_npc_txt.cpp \
    file_formats/file_wld.cpp \
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
    level_scene/lvlscene.cpp \
    level_scene/item_bgo.cpp \
    level_scene/item_block.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    levelprops.h \
    lvl_filedata.h \
    wld_filedata.h \
    npc_filedata.h \
    npcedit.h \
    leveledit.h \
    dataconfigs.h \
    saveimage.h \
    musicfilelist.h \
    logger.h \
    item_block.h \
    file_formats/file_formats.h \
    level_scene/item_bgo.h \
    level_scene/item_block.h \
    level_scene/lvlscene.h

FORMS    += \
    aboutdialog.ui \
    leveledit.ui \
    levelprops.ui \
    mainwindow.ui \
    npcedit.ui \
    saveimage.ui \
    musicfilelist.ui

RC_FILE = platgenw.rc

macx {
    ICON = images/mac/mushroom.icns
}

OTHER_FILES += \
    images/saveas.png \
    images/save.png \
    images/open.png \
    images/new.png \
    images/world.png \
    images/saveas.png \
    images/save.png \
    images/rubber.png \
    images/open.png \
    images/new.png \
    images/mushroom.png \
    images/level.png \
    images/arrow.png \
    images/21.png \
    images/20.png \
    images/19.png \
    images/18.png \
    images/17.png \
    images/16.png \
    images/15.png \
    images/14.png \
    images/13.png \
    images/12.png \
    images/11.png \
    images/10.png \
    images/09.png \
    images/08.png \
    images/07.png \
    images/06.png \
    images/05.png \
    images/04.png \
    images/03.png \
    images/02.png \
    images/01.png \
    platgenw.rc \
    images/unknown_npc.gif \
    images/unknown_block.gif \
    images/unknown_bgo.gif \
    images/world16.png \
    images/section16.png \
    images/mushroom16.png \
    images/level16.png \
    splash.png \
    mushroom.ico \
    images/coin.png \
    images/cur_rubber.png \
    images/grid.png \
    images/hand.png \
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
    images/player1_point.png \
    images/player1_start.png \
    images/player2_point.png \
    images/player2_start.png \
    images/playmusic.png \
    images/reload_data.png \
    images/reset_pos.png \
    images/section.png \
    images/splash.png \
    images/warp_entrance.png \
    images/warp_exit.png

RESOURCES += \
    editor.qrc
