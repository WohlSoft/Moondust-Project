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
    QMAKE_CXXFLAGS_WARN_ON  += \
        -Wno-unused-but-set-variable \
        -Wno-unused-parameter \
        -Wno-implicit-fallthrough
} else {
    DEFINES += _CRT_SECURE_NO_WARNINGS
    QMAKE_CFLAGS_WARN_ON   += /wd4100 /wd4244 /wd4005 /wd4013 /wd4047 /wd4996
    QMAKE_CXXFLAGS_WARN_ON += /wd4100 /wd4101 /wd4244 /wd4800 /wd4104 /wd4146
}

DEFINES += \
    MODPLUG_NO_FILESAVE \
    HAVE_CONFIG_H \
    MODPLUG_STATIC \
    MODPLUG_BUILD=1 \
    _REENTRANT \
    _USE_MATH_DEFINES

HEADERS += \
    include/modplug.h \
    config.h \
    it_defs.h \
    load_pat.h \
    sndfile.h \
    stdafx.h \
    tables.h

SOURCES += \
    fastmix.cpp \
    load_669.cpp \
    load_abc.cpp \
    load_amf.cpp \
    load_ams.cpp \
    load_dbm.cpp \
    load_dmf.cpp \
    load_dsm.cpp \
    load_far.cpp \
    load_it.cpp \
    # load_j2b.cpp \
    load_mdl.cpp \
    load_med.cpp \
    load_mid.cpp \
    load_mod.cpp \
    load_mt2.cpp \
    load_mtm.cpp \
    load_okt.cpp \
    load_pat.cpp \
    load_psm.cpp \
    load_ptm.cpp \
    load_s3m.cpp \
    load_stm.cpp \
    load_ult.cpp \
    load_umx.cpp \
    load_wav.cpp \
    load_xm.cpp \
    mmcmp.cpp \
    modplug.cpp \
    snd_dsp.cpp \
    snd_flt.cpp \
    snd_fx.cpp \
    sndfile.cpp \
    sndmix.cpp
