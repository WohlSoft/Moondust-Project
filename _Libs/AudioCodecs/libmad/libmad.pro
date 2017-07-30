#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = mad
INSTALLINCLUDES = $$PWD/include/mad.h
include($$PWD/../audio_codec_common.pri)

DEFINES += FPM_DEFAULT HAVE_CONFIG_H

win*-msvc*: {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS += /wd4244 /wd4146
}

!win*-msvc*:{
    QMAKE_CFLAGS_WARN_ON  += \
        -Wno-implicit-fallthrough
}

debug {
    DEFINES += DEBUG
} else: release: {
    DEFINES += NDEBUG
    !win*-msvc*: QMAKE_CFLAGS += -O3
}

HEADERS +=\
    include/mad.h

HEADERS += \
    bit.h \
    decoder.h \
    fixed.h \
    frame.h \
    global.h \
    huffman.h \
    layer12.h \
    layer3.h \
    stream.h \
    synth.h \
    timer.h \
    version.h \
    config.h

SOURCES += \
    bit.c \
    decoder.c \
    fixed.c \
    frame.c \
    huffman.c \
    layer12.c \
    layer3.c \
    stream.c \
    synth.c \
    timer.c \
    version.c

DISTFILES += \
    D.dat \
    qc_table.dat \
    rq_table.dat \
    sf_table.dat \
    imdct_s.dat
