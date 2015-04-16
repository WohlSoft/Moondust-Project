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

DESTDIR = $$PWD/../bin

android:{
    LANGUAGES_TARGET=/assets/languages
    ARCH=android_arm
} else {
    !contains(QMAKE_TARGET.arch, x86_64) {
    ARCH=x64
    } else {
    ARCH=x32
    }
    LANGUAGES_TARGET=$$PWD/../bin/languages
}
static: {
LINKTYPE=static
} else {
LINKTYPE=dynamic
}
debug: BUILDTP=debug
release: BUILDTP=release
OBJECTS_DIR = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.obj
MOC_DIR     = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.moc
RCC_DIR     = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.rcc
UI_DIR      = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.ui
message("$$TARGET will be built as $$BUILDTP $$ARCH ($$QMAKE_TARGET.arch) $${LINKTYPE}ally in $$OBJECTS_DIR")


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
