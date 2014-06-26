# LazyFixTool, a free tool for fix lazily-made image masks
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

TARGET = LazyFixTool
CONFIG   += console

TEMPLATE = app


SOURCES += \
    LazyFixTool.cpp \
    libs/giflib/dgif_lib.c \
    libs/giflib/egif_lib.c \
    libs/giflib/gif_err.c \
    libs/giflib/gif_font.c \
    libs/giflib/gif_hash.c \
    libs/giflib/gifalloc.c \
    libs/giflib/quantize.c \
    libs/EasyBMP/EasyBMP.cpp

HEADERS += \
    libs/giflib/gif_hash.h \
    libs/giflib/gif_lib.h \
    libs/giflib/gif_lib_private.h \
    libs/EasyBMP/EasyBMP.h \
    libs/EasyBMP/EasyBMP_BMP.h \
    libs/EasyBMP/EasyBMP_DataStructures.h \
    libs/EasyBMP/EasyBMP_VariousBMPutilities.h
