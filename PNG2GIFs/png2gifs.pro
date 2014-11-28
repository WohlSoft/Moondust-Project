# LazyFixTool, a free tool for fix lazily-made image masks
# and also, convert all BMPs into GIF
# This is a part of Platformer Game Engine by Wohlstand, a free platform for game making
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

QT       += core gui

DESTDIR = ../bin

release:OBJECTS_DIR = ../bin/_build/png2gifs/_release/.obj
release:MOC_DIR     = ../bin/_build/png2gifs/_release/.moc
release:RCC_DIR     = ../bin/_build/png2gifs/_release/.rcc
release:UI_DIR      = ../bin/_build/png2gifs/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/png2gifs/_debug/.obj
debug:MOC_DIR       = ../bin/_build/png2gifs/_debug/.moc
debug:RCC_DIR       = ../bin/_build/png2gifs/_debug/.rcc
debug:UI_DIR        = ../bin/_build/png2gifs/_debug/.ui

TARGET = PNG2GIFs
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += static

TEMPLATE = app

#QMAKE_CFLAGS += -Wno-sign-compare

RC_FILE = _resources/png2gifs.rc

SOURCES += \
    png2gifs.cpp \
    ../_Libs/giflib/dgif_lib.c \
    ../_Libs/giflib/egif_lib.c \
    ../_Libs/giflib/gif_err.c \
    ../_Libs/giflib/gif_font.c \
    ../_Libs/giflib/gif_hash.c \
    ../_Libs/giflib/gifalloc.c \
    ../_Libs/giflib/quantize.c \

HEADERS += \
    ../_Libs/giflib/gif_hash.h \
    ../_Libs/giflib/gif_lib.h \
    ../_Libs/giflib/gif_lib_private.h \
    version.h

RESOURCES += \
    _resources/png2gifs.qrc

OTHER_FILES += \
    _resources/png2gifs.rc
