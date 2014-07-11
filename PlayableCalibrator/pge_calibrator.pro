# SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
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
# Project created by QtCreator 2014-04-05T21:35:05
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = pge_calibrator
TEMPLATE = app

RC_FILE = _resourses/caltool.rc


SOURCES += main.cpp\
        calibrationmain.cpp \
    frame_matrix/matrix.cpp \
    about/about.cpp \
    animator/animate.cpp \
    animator/animationedit.cpp \
    frame_matrix/MatrixScene.cpp \
    libs/EasyBMP/EasyBMP.cpp \
    libs/giflib/dgif_lib.c \
    libs/giflib/egif_lib.c \
    libs/giflib/gif_err.c \
    libs/giflib/gif_font.c \
    libs/giflib/gif_hash.c \
    libs/giflib/gifalloc.c \
    libs/giflib/quantize.c \
    main/files_io.cpp \
    main/globals.cpp \
    main/graphics.cpp \
    main/settings.cpp

HEADERS  += calibrationmain.h \
    frame_matrix/matrix.h \
    about/about.h \
    animator/aniFrames.h \
    animator/animate.h \
    animator/animationedit.h \
    animator/AnimationScene.h \
    frame_matrix/MatrixScene.h \
    animator/SpriteScene.h \
    libs/EasyBMP/EasyBMP.h \
    libs/EasyBMP/EasyBMP_BMP.h \
    libs/EasyBMP/EasyBMP_DataStructures.h \
    libs/EasyBMP/EasyBMP_VariousBMPutilities.h \
    libs/giflib/gif_hash.h \
    libs/giflib/gif_lib.h \
    libs/giflib/gif_lib_private.h \
    main/globals.h \
    main/graphics.h

FORMS    += calibrationmain.ui \
    about/about.ui \
    animator/animate.ui \
    animator/animationedit.ui \
    frame_matrix/matrix.ui

RESOURCES += \
    _resourses/images.qrc

OTHER_FILES += \
    caltool.rc \
    calibrator.ico \
    _resourses/caltool.rc
