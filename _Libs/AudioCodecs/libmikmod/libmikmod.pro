#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = mikmod
INSTALLINCLUDES = $$PWD/include/mikmod.h
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

INCLUDEPATH += $$PWD/include/

DEFINES += \
    HAVE_LIMITS_H \
    HAVE_MEMCMP \
    HAVE_MEMORY_H \
    HAVE_SNDIO_H \
    HAVE_STDLIB_H \
    HAVE_STDINT_H \
    HAVE_STRING_H \
    DRV_RAW \
    NO_HQMIXER

unix: DEFINES += HAVE_UNISTD_H

win32: DEFINES += HAVE_WINDOWS_H

HEADERS +=\
    include/mikmod.h \
    dlapi/dlfcn.h \
    include/mikmod_ctype.h \
    include/mikmod_internals.h

SOURCES += \
    depackers/mmcmp.c \
    depackers/pp20.c \
    depackers/s404.c \
    depackers/xpk.c \
    dlapi/dl_hpux.c \
    loaders/load_669.c \
    loaders/load_amf.c \
    loaders/load_asy.c \
    loaders/load_dsm.c \
    loaders/load_far.c \
    loaders/load_gdm.c \
    loaders/load_gt2.c \
    loaders/load_imf.c \
    loaders/load_it.c \
    loaders/load_m15.c \
    loaders/load_med.c \
    loaders/load_mod.c \
    loaders/load_mtm.c \
    loaders/load_okt.c \
    loaders/load_s3m.c \
    loaders/load_stm.c \
    loaders/load_stx.c \
    loaders/load_ult.c \
    loaders/load_umx.c \
    loaders/load_uni.c \
    loaders/load_xm.c \
    mmio/mmalloc.c \
    mmio/mmerror.c \
    mmio/mmio.c \
    playercode/mdreg.c \
    playercode/mdriver.c \
    playercode/mdulaw.c \
    playercode/mloader.c \
    playercode/mlreg.c \
    playercode/mlutil.c \
    playercode/mplayer.c \
    playercode/munitrk.c \
    playercode/mwav.c \
    playercode/npertab.c \
    playercode/sloader.c \
    playercode/virtch.c \
    playercode/virtch2.c \
    playercode/virtch_common.c \
    posix/memcmp.c \
    posix/strcasecmp.c \
    posix/strstr.c \
    drivers/drv_raw.c \
    drivers/drv_nos.c

