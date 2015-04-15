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

QT       += core gui
QT       -= opengl

DESTDIR = ../bin

static: {
release:OBJECTS_DIR = ../bin/_build/gif2png/_release/.obj
release:MOC_DIR     = ../bin/_build/gif2png/_release/.moc
release:RCC_DIR     = ../bin/_build/gif2png/_release/.rcc
release:UI_DIR      = ../bin/_build/gif2png/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/gif2png/_debug/.obj
debug:MOC_DIR       = ../bin/_build/gif2png/_debug/.moc
debug:RCC_DIR       = ../bin/_build/gif2png/_debug/.rcc
debug:UI_DIR        = ../bin/_build/gif2png/_debug/.ui
} else {
release:OBJECTS_DIR = ../bin/_build/_dynamic/gif2png/_release/.obj
release:MOC_DIR     = ../bin/_build/_dynamic/gif2png/_release/.moc
release:RCC_DIR     = ../bin/_build/_dynamic/gif2png/_release/.rcc
release:UI_DIR      = ../bin/_build/_dynamic/gif2png/_release/.ui

debug:OBJECTS_DIR   = ../bin/_build/_dynamic/gif2png/_debug/.obj
debug:MOC_DIR       = ../bin/_build/_dynamic/gif2png/_debug/.moc
debug:RCC_DIR       = ../bin/_build/_dynamic/gif2png/_debug/.rcc
debug:UI_DIR        = ../bin/_build/_dynamic/gif2png/_debug/.ui
}

macx: QMAKE_CXXFLAGS += -Wno-header-guard

DEFINES += QT_NO_TRANSLATION

TARGET = GIFs2PNG
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += static
CONFIG   -= import_plugins

TEMPLATE = app

RC_FILE = _resources/gifs2png.rc

SOURCES += \
    gifs2png.cpp

RESOURCES += \
    _resources/gifs2png.qrc

OTHER_FILES += \
    _resources/gifs2png.rc

HEADERS += \
    version.h
