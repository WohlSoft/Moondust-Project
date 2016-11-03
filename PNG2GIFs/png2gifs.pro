# LazyFixTool, a free tool for fix lazily-made image masks
# and also, convert all BMPs into GIF
# This is a part of Platformer Game Engine by Wohlstand, a free platform for game making
# Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

QT       += gui widgets

include(../_common/dest_dir.pri)
include(../_common/build_props.pri)

TARGET = PNG2GIFs
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += static
CONFIG   += thread
CONFIG   += c++11

TEMPLATE = app

include(../_common/strip_garbage.pri)

# Dependent libraries
include($$PWD/../_Libs/giflib/giflib.pri)

RC_FILE = _resources/png2gifs.rc

SOURCES += \
    png2gifs.cpp \
    png2gifs_gui.cpp \
    png2gifs_converter.cpp

HEADERS += \
    version.h \
    png2gifs_gui.h \
    png2gifs_converter.h

RESOURCES += \
    _resources/png2gifs.qrc

OTHER_FILES += \
    _resources/png2gifs.rc

FORMS += \
    png2gifs_gui.ui
