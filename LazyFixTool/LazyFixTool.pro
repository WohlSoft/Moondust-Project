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

include($$PWD/../_common/dest_dir.pri)
include($$PWD/../_common/build_props.pri)

macx: QMAKE_CXXFLAGS += -Wno-header-guard

include($$PWD/../_common/strip_garbage.pri)

TEMPLATE = app
TARGET = LazyFixTool
CONFIG   -= qt
CONFIG   -= windows
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += static
CONFIG   += c++11

include ($$PWD/../_common/lib_destdir.pri)

INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include
INCLUDEPATH += $$PWD/../_common

LIBS += -L$$PWD/../_Libs/_builds/$$TARGETOS/lib
LIBS += -lfreeimagelite

!macx: LIBS += -static-libgcc -static-libstdc++
win32: LIBS += -static -lpthread

include($$PWD/../_common/tclap/tclap.pri)
include($$PWD/../_common/DirManager/dirman.pri)
include($$PWD/../_common/Utils/Utils.pri)
include($$PWD/../_common/Utf8Main/utf8main.pri)
include($$PWD/../_common/FileMapper/FileMapper.pri)

RC_FILE = _resources/lazyfix_tool.rc

SOURCES += \
    LazyFixTool.cpp

HEADERS += \
    version.h

OTHER_FILES += \
    _resources/lazyfix_tool.rc
