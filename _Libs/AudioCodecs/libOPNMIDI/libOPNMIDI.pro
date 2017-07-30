#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = OPNMIDI
INSTALLINCLUDES = $$PWD/include/*
INSTALLINCLUDESTO = OPNMIDI
include($$PWD/../audio_codec_common.pri)

macx: QMAKE_CXXFLAGS_WARN_ON += -Wno-absolute-value

INCLUDEPATH += $$PWD $$PWD/include

HEADERS += \
    include/opnmidi.h \
    fraction.h \
    gm_opn_bank.h \
    opnbank.h \
    opnmidi_mus2mid.h \
    opnmidi_private.hpp \
    opnmidi_xmi2mid.h \
    Ym2612_ChipEmu.h

SOURCES += \
    opnmidi.cpp \
    opnmidi_load.cpp \
    opnmidi_midiplay.cpp \
    opnmidi_opn2.cpp \
    opnmidi_private.cpp \
    Ym2612_ChipEmu.cpp \
    opnmidi_mus2mid.c \
    opnmidi_xmi2mid.c
