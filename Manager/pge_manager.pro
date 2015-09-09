# PGE Manager, a free tool for playable srite design
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
# Project created by QtCreator 2015-03-31T19:17:26
#
#-------------------------------------------------

QT       += network widgets concurrent xml

win32: {
QT += winextras
} else {
QT -= winextras
}

include(../_common/dest_dir.pri)

TARGET = pge_manager
TEMPLATE = app

CONFIG += c++11

CONFIG += static
CONFIG += thread

!macx: {
QMAKE_CXXFLAGS += -static -static-libgcc
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'
}

android:{
    LANGUAGES_TARGET=/assets/languages
} else {
    LANGUAGES_TARGET=$$PWD/../bin/languages
}

include(../_common/build_props.pri)

INCLUDEPATH += $$PWD $$PWD/_includes "$$PWD/../_Libs" "$$PWD/../_common"

SOURCES += main.cpp\
        mainwindow.cpp \
    common_features/app_path.cpp \
    config_packs.cpp \
    settings.cpp \
    config_packs_repos.cpp \
    http_downloader/http_downloader.cpp \
    xml_parse/xml_cpack_list.cpp

HEADERS  += mainwindow.h \
    common_features/app_path.h \
    version.h \
    config_packs.h \
    settings.h \
    config_packs_repos.h \
    http_downloader/http_downloader.h \
    xml_parse/xml_cpack_list.h

FORMS    += mainwindow.ui \
    config_packs_repos.ui
