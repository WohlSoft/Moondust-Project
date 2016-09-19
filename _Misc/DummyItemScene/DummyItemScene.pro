#-------------------------------------------------
#
# Project created by QtCreator 2016-09-15T18:11:37
#
#-------------------------------------------------

QT       += core gui widgets

TARGET   = DummyItemScene
TEMPLATE = app

CONFIG  += c++11

SOURCES += main.cpp\
        itemscene.cpp \
    item_scene/pge_edit_scene.cpp \
    item_scene/pge_edit_scene_item.cpp \
    key_dropper.cpp

HEADERS  += itemscene.h \
    item_scene/pge_edit_scene.h \
    item_scene/pge_edit_scene_item.h \
    key_dropper.h

FORMS    += itemscene.ui
