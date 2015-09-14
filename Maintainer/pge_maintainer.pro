# PGE Maintainer, a free tool for playable srite design
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
# Project created by QtCreator 2015-09-08T22:38:48
#
#-------------------------------------------------

QT       += core gui widgets concurrent
CONFIG += c++11
CONFIG += thread

include(../_common/dest_dir.pri)
include(../_common/build_props.pri)

INCLUDEPATH += "$$PWD/../_Libs/" "$$PWD/../_common"

TARGET = pge_maintainer
TEMPLATE = app

DEFINES += PGE_FILES_QT

SOURCES += main.cpp\
        main_window.cpp \
    ../_common/PGE_File_Formats/file_formats.cpp \
    ../_common/PGE_File_Formats/file_rw_lvl.cpp \
    ../_common/PGE_File_Formats/file_rw_lvlx.cpp \
    ../_common/PGE_File_Formats/file_rw_meta.cpp \
    ../_common/PGE_File_Formats/file_rw_npc_txt.cpp \
    ../_common/PGE_File_Formats/file_rwopen.cpp \
    ../_common/PGE_File_Formats/file_rw_sav.cpp \
    ../_common/PGE_File_Formats/file_rw_savx.cpp \
    ../_common/PGE_File_Formats/file_rw_smbx64_cnf.cpp \
    ../_common/PGE_File_Formats/file_rw_wld.cpp \
    ../_common/PGE_File_Formats/file_rw_wldx.cpp \
    ../_common/PGE_File_Formats/file_strlist.cpp \
    ../_common/PGE_File_Formats/lvl_filedata.cpp \
    ../_common/PGE_File_Formats/npc_filedata.cpp \
    ../_common/PGE_File_Formats/pge_file_lib_globs.cpp \
    ../_common/PGE_File_Formats/pge_x.cpp \
    ../_common/PGE_File_Formats/save_filedata.cpp \
    ../_common/PGE_File_Formats/smbx64.cpp \
    ../_common/PGE_File_Formats/smbx64_cnf_filedata.cpp \
    ../_common/PGE_File_Formats/wld_filedata.cpp \
    Music/audiocvt_sox_gui.cpp \
    common_features/app_path.cpp \
    files/episode_box.cpp \
    common_features/logger.cpp

HEADERS  += main_window.h \
    ../_common/PGE_File_Formats/file_formats.h \
    ../_common/PGE_File_Formats/file_strlist.h \
    ../_common/PGE_File_Formats/lvl_filedata.h \
    ../_common/PGE_File_Formats/meta_filedata.h \
    ../_common/PGE_File_Formats/npc_filedata.h \
    ../_common/PGE_File_Formats/pge_file_lib_globs.h \
    ../_common/PGE_File_Formats/pge_file_lib_sys.h \
    ../_common/PGE_File_Formats/pge_x.h \
    ../_common/PGE_File_Formats/pge_x_macro.h \
    ../_common/PGE_File_Formats/save_filedata.h \
    ../_common/PGE_File_Formats/smbx64.h \
    ../_common/PGE_File_Formats/smbx64_cnf_filedata.h \
    ../_common/PGE_File_Formats/smbx64_macro.h \
    ../_common/PGE_File_Formats/wld_filedata.h \
    Music/audiocvt_sox_gui.h \
    common_features/app_path.h \
    version.h \
    files/episode_box.h \
    common_features/logger.h \
    common_features/logger_sets.h

FORMS    += main_window.ui \
    Music/audiocvt_sox_gui.ui
