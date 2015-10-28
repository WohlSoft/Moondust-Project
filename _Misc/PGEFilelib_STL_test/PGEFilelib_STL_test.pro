TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../_common/
DEFINES -= PGE_FILES_QT

SOURCES += main.cpp \
    ../../_common/PGE_File_Formats/file_formats.cpp \
    ../../_common/PGE_File_Formats/file_rw_lvl.cpp \
    ../../_common/PGE_File_Formats/file_rw_lvlx.cpp \
    ../../_common/PGE_File_Formats/file_rw_meta.cpp \
    ../../_common/PGE_File_Formats/file_rw_npc_txt.cpp \
    ../../_common/PGE_File_Formats/file_rwopen.cpp \
    ../../_common/PGE_File_Formats/file_rw_sav.cpp \
    ../../_common/PGE_File_Formats/file_rw_savx.cpp \
    ../../_common/PGE_File_Formats/file_rw_smbx64_cnf.cpp \
    ../../_common/PGE_File_Formats/file_rw_wld.cpp \
    ../../_common/PGE_File_Formats/file_rw_wldx.cpp \
    ../../_common/PGE_File_Formats/file_strlist.cpp \
    ../../_common/PGE_File_Formats/lvl_filedata.cpp \
    ../../_common/PGE_File_Formats/npc_filedata.cpp \
    ../../_common/PGE_File_Formats/pge_file_lib_globs.cpp \
    ../../_common/PGE_File_Formats/pge_x.cpp \
    ../../_common/PGE_File_Formats/save_filedata.cpp \
    ../../_common/PGE_File_Formats/smbx64.cpp \
    ../../_common/PGE_File_Formats/smbx64_cnf_filedata.cpp \
    ../../_common/PGE_File_Formats/wld_filedata.cpp \

HEADERS += \
    ../../_common/PGE_File_Formats/file_formats.h \
    ../../_common/PGE_File_Formats/file_strlist.h \
    ../../_common/PGE_File_Formats/lvl_filedata.h \
    ../../_common/PGE_File_Formats/meta_filedata.h \
    ../../_common/PGE_File_Formats/npc_filedata.h \
    ../../_common/PGE_File_Formats/pge_file_lib_globs.h \
    ../../_common/PGE_File_Formats/pge_file_lib_sys.h \
    ../../_common/PGE_File_Formats/pge_x.h \
    ../../_common/PGE_File_Formats/pge_x_macro.h \
    ../../_common/PGE_File_Formats/save_filedata.h \
    ../../_common/PGE_File_Formats/smbx64.h \
    ../../_common/PGE_File_Formats/smbx64_cnf_filedata.h \
    ../../_common/PGE_File_Formats/smbx64_macro.h \
    ../../_common/PGE_File_Formats/wld_filedata.h \
