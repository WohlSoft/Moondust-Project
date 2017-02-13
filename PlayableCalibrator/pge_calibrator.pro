# SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
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
# Project created by QtCreator 2014-04-05T21:35:05
#
#-------------------------------------------------

QT       += gui widgets

include($$PWD/../_common/dest_dir.pri)

android:{
    LANGUAGES_TARGET=/assets/languages
} else {
    LANGUAGES_TARGET=$$PWD/../bin/languages
}
include($$PWD/../_common/build_props.pri)

!macx: TARGET = pge_calibrator
macx:  TARGET = "Character Calibrator"
TEMPLATE = app
CONFIG   += static
CONFIG   += c++11

macx {
    ICON = _resourses/calibrator.icns
    QMAKE_INFO_PLIST = $$PWD/_resourses/calibrator.plist
}

RC_FILE = _resourses/caltool.rc

macx: QMAKE_CXXFLAGS += -Wno-header-guard

QMAKE_CFLAGS += -Wno-sign-compare

include($$PWD/../_common/strip_garbage.pri)

# Dependent libraries
include ($$PWD/../_common/lib_destdir.pri)
INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include
LIBS += -L$$PWD/../_Libs/_builds/$$TARGETOS/lib
LIBS += -lfreeimagelite

SOURCES += \
    main.cpp\
    calibrationmain.cpp \
    frame_matrix/matrix.cpp \
    about/about.cpp \
    animator/animate.cpp \
    animator/animationedit.cpp \
    main/files_io.cpp \
    main/globals.cpp \
    main/graphics.cpp \
    main/settings.cpp \
    image_calibration/image_calibrator.cpp \
    main/mw.cpp \
    animator/AnimationScene.cpp \
    main/app_path.cpp

HEADERS  += \
    calibrationmain.h \
    frame_matrix/matrix.h \
    about/about.h \
    animator/aniFrames.h \
    animator/animate.h \
    animator/animationedit.h \
    animator/AnimationScene.h \
    main/globals.h \
    main/graphics.h \
    version.h \
    image_calibration/image_calibrator.h \
    main/mw.h \
    main/app_path.h

FORMS    += \
    calibrationmain.ui \
    about/about.ui \
    animator/animate.ui \
    animator/animationedit.ui \
    frame_matrix/matrix.ui \
    image_calibration/image_calibrator.ui

RESOURCES += \
    _resourses/images.qrc

OTHER_FILES += \
    caltool.rc \
    calibrator.ico \
    _resourses/caltool.rc
