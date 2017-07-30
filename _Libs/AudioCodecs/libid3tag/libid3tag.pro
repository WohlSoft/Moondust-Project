#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = id3tag
INSTALLINCLUDES = $$PWD/include/*
INSTALLINCLUDESTO = id3tag
include($$PWD/../audio_codec_common.pri)

INCLUDEPATH += $$PWD $$PWD/include

LIBS += -lSDL2

!win*-msvc*:{
    QMAKE_CFLAGS_WARN_ON += \
        -Wno-implicit-function-declaration \
        -Wno-implicit-fallthrough \
        -Wno-pointer-sign \

} else {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS_WARN_ON   += /wd4100 /wd4244 /wd4005 /wd4013 /wd4047 /wd4996
    QMAKE_CXXFLAGS_WARN_ON += /wd4100 /wd4101 /wd4244 /wd4800 /wd4104 /wd4146
}

HEADERS += \
    include/id3tag.h \
    compat.h \
    crc.h \
    debug.h \
    field.h \
    file.h \
    frame.h \
    frametype.h \
    genre.h \
    global.h \
    latin1.h \
    parse.h \
    render.h \
    tag.h \
    ucs4.h \
    utf8.h \
    utf16.h \
    util.h \
    version.h

SOURCES += \
    id3_file.c \
    id3_compat.c \
    id3_crc.c \
    id3_field.c \
    id3_frame.c \
    id3_frametype.c \
    id3_genre.c \
    id3_latin1.c \
    id3_parse.c \
    id3_render.c \
    id3_tag.c \
    id3_ucs4.c \
    id3_utf16.c \
    id3_utf8.c \
    id3_util.c \
    id3_version.c

contains(DEFINES, DEBUG): include($$PWD/debug.pri)
