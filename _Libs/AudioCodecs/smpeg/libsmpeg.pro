#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = smpeg
INSTALLINCLUDES = $$PWD/smpeg.h
include($$PWD/../audio_codec_common.pri)

win*-msvc*: {
DEFINES += _CRT_SECURE_NO_WARNINGS
QMAKE_CFLAGS += /wd4244 /wd4146
}

debug {
    DEFINES += DEBUG
} else: release: {
    DEFINES += NDEBUG
    QMAKE_CFLAGS += -O3
}

DEFINES += NOCONTROLS

HEADERS +=\
    smpeg.h \
    MPEG.h \
    MPEGaction.h \
    MPEGaudio.h \
    MPEGerror.h \
    MPEGframe.h \
    MPEGlist.h \
    MPEGring.h \
    MPEGstream.h \
    MPEGsystem.h \
    MPEGvideo.h \
    video/decoders.h \
    video/dither.h \
    video/proto.h \
    video/util.h \
    video/video.h

SOURCES += \
    MPEG.cpp \
    MPEGlist.cpp \
    MPEGring.cpp \
    MPEGstream.cpp \
    MPEGsystem.cpp \
    smpeg.cpp \
    audio/bitwindow.cpp \
    audio/filter.cpp \
    audio/filter_2.cpp \
    audio/hufftable.cpp \
    audio/MPEGaudio.cpp \
    audio/mpeglayer1.cpp \
    audio/mpeglayer2.cpp \
    audio/mpeglayer3.cpp \
    audio/mpegtable.cpp \
    audio/mpegtoraw.cpp \
    video/decoders.cpp \
    video/floatdct.cpp \
    video/gdith.cpp \
    video/jrevdct.cpp \
    video/motionvec.cpp \
    video/MPEGvideo.cpp \
    video/parseblock.cpp \
    video/readfile.cpp \
    video/util.cpp \
    video/video.cpp \
    video/mmxflags_asm.S \
    video/mmxidct_asm.S
