#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = zlib
INSTALLINCLUDES = $$PWD/include/*
INSTALLINCLUDESTO = zlib
include($$PWD/../audio_codec_common.pri)

INCLUDEPATH += $$PWD $$PWD/include

!win*-msvc*:{
    QMAKE_CFLAGS_WARN_ON += \
        -Wno-implicit-function-declaration

} else {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS_WARN_ON   += /wd4100 /wd4244 /wd4005 /wd4013 /wd4047 /wd4996
    QMAKE_CXXFLAGS_WARN_ON += /wd4100 /wd4101 /wd4244 /wd4800 /wd4104 /wd4146
}

HEADERS += \
    include/zlib.h \
    crc32.h \
    deflate.h \
    gzguts.h \
    inffast.h \
    inffixed.h \
    inflate.h \
    inftrees.h \
    trees.h \
    zconf.h \
    zlib.h \
    zutil.h

SOURCES += \
    adler32.c \
    compress.c \
    crc32.c \
    deflate.c \
    gzclose.c \
    gzlib.c \
    gzread.c \
    gzwrite.c \
    infback.c \
    inffast.c \
    inflate.c \
    inftrees.c \
    trees.c \
    uncompr.c \
    zutil.c

