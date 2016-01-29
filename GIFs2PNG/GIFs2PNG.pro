# GIFs2PNG, a free tool for merge GIF images with his masks and save into PNG
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
# Project created by QtCreator 2014-06-25T10:57:31
#
#-------------------------------------------------

QT       += core
QT       -= opengl gui

include($$PWD/../_common/dest_dir.pri)
include($$PWD/../_common/build_props.pri)

macx: QMAKE_CXXFLAGS += -Wno-header-guard

include($$PWD/../_common/strip_garbage.pri)

DEFINES += QT_NO_TRANSLATION

TEMPLATE = app
TARGET = GIFs2PNG
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += static
CONFIG   -= import_plugins

QTPLUGIN =
QTPLUGIN.platforms = qminimal

include ($$PWD/../_common/lib_destdir.pri)

INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include
LIBS += -L$$PWD/../_Libs/_builds/$$TARGETOS/lib
LIBS += -lfreeimagelite


RC_FILE = _resources/gifs2png.rc

SOURCES += \
    gifs2png.cpp \
    common_features/config_manager.cpp \
    common_features/file_mapper.cpp

RESOURCES += \
    _resources/gifs2png.qrc

OTHER_FILES += \
    _resources/gifs2png.rc

HEADERS += \
    version.h \
    common_features/config_manager.h \
    common_features/file_mapper.h

