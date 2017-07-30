#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = modplug
INSTALLINCLUDES = $$PWD/include/*
INSTALLINCLUDESTO = modplug
include($$PWD/../audio_codec_common.pri)

INCLUDEPATH += $$PWD $$PWD/include

!win*-msvc*:{
    QMAKE_CFLAGS_WARN_ON    += -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS_WARN_ON  += -Wno-unused-but-set-variable
} else {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS_WARN_ON   += /wd4100 /wd4244 /wd4005 /wd4013 /wd4047 /wd4996
    QMAKE_CXXFLAGS_WARN_ON += /wd4100 /wd4101 /wd4244 /wd4800 /wd4104 /wd4146
}

DEFINES += \
    HAVE_CONFIG_H \
    MODPLUG_STATIC \
    MODPLUG_BUILD=1 \
    _USE_MATH_DEFINES

HEADERS += \
    $$PWD/include/modplug.h \
    $$PWD/config.h \
    $$PWD/it_defs.h \
    $$PWD/load_pat.h \
    $$PWD/sndfile.h \
    $$PWD/stdafx.h \
    $$PWD/tables.h

SOURCES += \
    $$PWD/fastmix.cpp \
    $$PWD/load_669.cpp \
    $$PWD/load_abc.cpp \
    $$PWD/load_amf.cpp \
    $$PWD/load_ams.cpp \
    $$PWD/load_dbm.cpp \
    $$PWD/load_dmf.cpp \
    $$PWD/load_dsm.cpp \
    $$PWD/load_far.cpp \
    $$PWD/load_it.cpp \
    # $$PWD/load_j2b.cpp \
    $$PWD/load_mdl.cpp \
    $$PWD/load_med.cpp \
    $$PWD/load_mid.cpp \
    $$PWD/load_mod.cpp \
    $$PWD/load_mt2.cpp \
    $$PWD/load_mtm.cpp \
    $$PWD/load_okt.cpp \
    $$PWD/load_pat.cpp \
    $$PWD/load_psm.cpp \
    $$PWD/load_ptm.cpp \
    $$PWD/load_s3m.cpp \
    $$PWD/load_stm.cpp \
    $$PWD/load_ult.cpp \
    $$PWD/load_umx.cpp \
    $$PWD/load_wav.cpp \
    $$PWD/load_xm.cpp \
    $$PWD/mmcmp.cpp \
    $$PWD/modplug.cpp \
    $$PWD/snd_dsp.cpp \
    $$PWD/snd_flt.cpp \
    $$PWD/snd_fx.cpp \
    $$PWD/sndfile.cpp \
    $$PWD/sndmix.cpp
