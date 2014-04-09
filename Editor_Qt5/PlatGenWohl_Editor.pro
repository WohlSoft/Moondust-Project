#-------------------------------------------------
#
# Project created by QtCreator 2014-03-18T16:09:15
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = plweditor
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp \
    levelprops.cpp \
    file_formats.cpp \
    childwindow.cpp \
    npcedit.cpp \
    leveledit.cpp \
    lvlscene.cpp \
    dataconfigs.cpp \
    saveimage.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h \
    levelprops.h \
    lvl_filedata.h \
    wld_filedata.h \
    npc_filedata.h \
    childwindow.h \
    npcedit.h \
    leveledit.h \
    lvlscene.h \
    dataconfigs.h \
    saveimage.h

FORMS    += \
    aboutdialog.ui \
    leveledit.ui \
    levelprops.ui \
    mainwindow.ui \
    npcedit.ui \
    saveimage.ui

RC_FILE = platgenw.rc

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
    images/level16.png

RESOURCES += \
    editor.qrc
