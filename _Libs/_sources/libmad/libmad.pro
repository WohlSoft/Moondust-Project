#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = mad

DEFINES += FPM_DEFAULT HAVE_CONFIG_H

debug {
    DEFINES += DEBUG
} else: release: {
    DEFINES += NDEBUG
    QMAKE_CFLAGS += -O3
}

QMAKE_CFLAGS    += -fPIC
QMAKE_CXXFLAGS  += -fPIC

HEADERS += \
    bit.h \
    decoder.h \
    fixed.h \
    frame.h \
    global.h \
    huffman.h \
    layer12.h \
    layer3.h \
    mad.h \
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
