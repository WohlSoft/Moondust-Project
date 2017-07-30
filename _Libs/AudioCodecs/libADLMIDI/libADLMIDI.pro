#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = ADLMIDI
INSTALLINCLUDES = $$PWD/include/*
INSTALLINCLUDESTO = ADLMIDI
include($$PWD/../audio_codec_common.pri)

macx: QMAKE_CXXFLAGS_WARN_ON += -Wno-absolute-value

INCLUDEPATH += $$PWD $$PWD/include

HEADERS += \
    include/adlmidi.h \
    adldata.hh \
    adlmidi_mus2mid.h \
    adlmidi_private.hpp \
    adlmidi_xmi2mid.h \
    dbopl.h \
    fraction.h \
    nukedopl3.h

SOURCES += \
    adldata.cpp \
    adlmidi.cpp \
    adlmidi_load.cpp \
    adlmidi_midiplay.cpp \
    adlmidi_opl3.cpp \
    adlmidi_private.cpp \
    dbopl.cpp \
    adlmidi_mus2mid.c \
    adlmidi_xmi2mid.c \
    nukedopl3.c


