# PGE Maintainer, a free tool for playable srite design
# This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
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
# Project created by QtCreator 2015-09-08T22:38:48
#
#-------------------------------------------------

QT       += core gui widgets concurrent
CONFIG += c++11
CONFIG += thread

include($$PWD/../_common/dest_dir.pri)
include($$PWD/../_common/build_props.pri)

INCLUDEPATH += "$$PWD/../_Libs/" "$$PWD/../_common"

TARGET = pge_maintainer
TEMPLATE = app

include($$PWD/../_common/strip_garbage.pri)

include($$PWD/../_common/PGE_File_Formats/File_FormatsQT.pri)

SOURCES += main.cpp\
    main_window.cpp \
    Music/audiocvt_sox_gui.cpp \
    common_features/app_path.cpp \
    files/episode_box.cpp \
    common_features/logger.cpp \
    EpisodeCvt/episode_converter.cpp

HEADERS  += main_window.h \
    Music/audiocvt_sox_gui.h \
    common_features/app_path.h \
    version.h \
    files/episode_box.h \
    common_features/logger.h \
    common_features/logger_sets.h \
    EpisodeCvt/episode_converter.h

FORMS    += main_window.ui \
    Music/audiocvt_sox_gui.ui \
    EpisodeCvt/episode_converter.ui
